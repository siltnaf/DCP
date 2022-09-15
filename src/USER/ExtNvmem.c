/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file ExtNvmem.c
       \brief Header file defining interface to External Non-Volatile Memory
       
<!-- DESCRIPTION: -->


<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
       $Header:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/ExtNvmem.c-arc   1.0.1.0   Jun 08 2018 17:20:24   ctctmas  $
    
       $Log:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/ExtNvmem.c-arc  $
 * 
 *    Rev 1.1   Apr 24 2019 ctctmas
 * RTC_064664 APU minimum compressor off time 3.5 minutes
 *  Move this file to RTC Tip 
 * 
 *    Rev 1.0.1.0   Jun 08 2018 17:20:24   ctctmas
 * PPR032836
 * Experimental Modifications to Prevent Corruption Due to Buffer Overrun.
 * 
 *    Rev 1.0   Jun 08 2018 17:10:22   ctctmas
 * Initial Carrier revision.
 
       Revision 1.1  2010/12/01 22:44:24  blepojevic
       This is former software version DCP3-019-02K imported to new folder APU\DieselAPU\DCP and will be tagged using new naming convention starting from TD-DCP-02.02.01

       Revision 1.2  2010/12/01 22:44:24  dharper
       Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
       This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
       This is because REV B was never checked in.

       Revision 1.1  2009/10/14 12:37:14  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.13  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.12  2008-03-26 22:44:31  gfinlay
       Fixed filename in header comment block

       Revision 1.11  2008-03-26 22:38:16  gfinlay
       Fixed file header comment block so it parses properly for Doxygen, and so it retrieves the file history log properly with CVS.

       \endif
*/
/******************************************************************************/
#ifdef EXT_MEM
/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "ExtNvmem.h"
#include "syncserial.h"
#include "tfxstddef.h"
#include <pic18.h>
#include "Events.h"
#include "Datalog.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

#define EXM_DEVICE_HIGH				 0xA9      // Control byte for high block
#define EXM_DEVICE_LOW				 0xA1     // Control byte for low block

#define PAGE_SIZE							128		    // Size of each page in bytes.	

//maximum length of data that can be logged
#define MAX_LOG_LENGTH		100     //(EXT_BUFFER_SIZE * BUFF_ELEM_SIZE)

#define EX_MEM_SIZE				0x100000 //size of memory in bytes


#define NUM_HEADERS						2             //number of headers   

#define HEADER_OFFSET			( NUM_HEADERS * HDR_W_CHKSM_LEN )

//Defines for header information

//Relative addresses of Headers.
#define HEADER1_ADDR				0										
#define HEADER2_ADDR				( 1 * HDR_W_CHKSM_LEN ) 

#define CKSM_LENGTH					2    //16 bit Checksum

#define HEADER_LENGTH				5   //length of the header without checksum

#define HDR_W_CHKSM_LEN			(HEADER_LENGTH + CKSM_LENGTH)

//Relative Indices for information in the header 
#define KEY1_INDEX				0
#define KEY2_INDEX				1

#define NXT_ADDR2_INDEX		2
#define NXT_ADDR1_INDEX		3
#define NXT_ADDR0_INDEX		4

#define CKSMH_INDEX				5
#define CKSML_INDEX				6

//Key characters to see verify data validity
#define KEY1_CHAR					0x55
#define KEY2_CHAR					0xAA

/* ---------- L o c a l  S t r u c t u r e s ---------- */

//states for the state machine
typedef enum 
{
   	IDLE,
   	WRITE2,
    DONE_WRITE,
    WRITE_HEADER1,
    WRITE_HEADER2,
    WAIT_STATE
}tOBM_STATE;


typedef struct
{
	 uint8 u8DataLen;	 
	 uint8 au8Data[BUFF_ELEM_SIZE];   //the data
}zRECORD;

/* ---------- Global    D a t a ---------- */
unsigned long u32CurrAddress_To_View;

/* ---------- L o c a l    D a t a ---------- */


//record buffer variables
static zRECORD  au8RecordBuff[EXT_BUFFER_SIZE];
static uint8 	  u8BufferCount = 0;

//memory management variables

static  uint32  u32CurrAddress;

static	bit	bResetPending= FALSE;

static	bit	bConnected = FALSE;
 		
static 	bit	bBusy = FALSE;

static 	tOBM_STATE tCurrentState =IDLE ; 

static 	uint8 au8HeaderBuff[HDR_W_CHKSM_LEN];

/* ----------------- L o c a l  F u n c t i o n s ----------------- */
static uint16 fnCalcChecksum(uint8 *pu8CksmData,uint8 u8DataLen);

static void fnShiftBuffer(void);

/* ----------------- Global F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnInit -->
       \brief Initialize the module

<!-- PURPOSE: -->
			Reads information from the header and initializes the module.
			This function needs to be called for each memory space at startup.
<!-- PARAMETERS: none -->
       @param[in]  
<!-- RETURNS: -->
      \return void.

*/
/******************************************************************************/

char EXM_fnInit(void)
{
  char cRetVal;
  //Expected checksums calculated from headers 1 and 2
  static uint16 u16CalcCksm1;
  static uint16 u16CalcCksm2;
  
  //Checksums read from headers 1 and 2
  static uint16 u16ReadCksm1;
  static uint16 u16ReadCksm2;
  
	uint8 au8Header[HDR_W_CHKSM_LEN]; 
   

  TRISF &= 0xC0;
  PORTF &= 0xC8;  
  PORTF |= 0x08;
  
    //read header 1
    while ( !ssgDone );
    (void)ssfI2CReadFrom( EXM_DEVICE_LOW, HEADER1_ADDR, au8Header, HDR_W_CHKSM_LEN);
    while(!ssgDone); 
 	
 	if(ssgAck)
  {
  //	if memory didn't acknowledge then assume it is not present
  	bConnected = TRUE;
  }

  //save checksum values from header 
  u16CalcCksm1 = fnCalcChecksum(au8Header,HEADER_LENGTH);
  u16ReadCksm1 = au8Header[CKSMH_INDEX];
  u16ReadCksm1 = ( u16ReadCksm1 << 8);
	u16ReadCksm1 |= au8Header[CKSML_INDEX];
  
 	if( u16CalcCksm1 == u16ReadCksm1)
	{
		//checksum for header 1 is good use it
		//save info from header
 	 	u32CurrAddress  = ( au8Header[NXT_ADDR2_INDEX]);
 	 	u32CurrAddress  = ( u32CurrAddress << 8);
 	 	u32CurrAddress  |= au8Header[NXT_ADDR1_INDEX];
 	 	u32CurrAddress  = ( u32CurrAddress << 8);
 	 	u32CurrAddress  |= au8Header[NXT_ADDR0_INDEX];
 		cRetVal = TRUE;
 	}
 	//checksum for header 1 is bad
 	else
 	{
 		//read second header
 		while ( !ssgDone );
        (void) ssfI2CReadFrom( EXM_DEVICE_LOW, HEADER2_ADDR, au8Header, HDR_W_CHKSM_LEN);
 		while(!ssgDone); 
 
 		//calculate checksum
 		u16CalcCksm2 = fnCalcChecksum(au8Header,HEADER_LENGTH);
 		u16ReadCksm2 = au8Header[CKSMH_INDEX];
  	u16ReadCksm2 = ( u16ReadCksm2 << 8);
		u16ReadCksm2 |= au8Header[CKSML_INDEX];	
 		 		
 		if( u16CalcCksm2 == u16ReadCksm2)
 		{
 			//checksum 2 is good, use 2nd header
 	 		u32CurrAddress  = ( au8Header[NXT_ADDR2_INDEX]);
 	 		u32CurrAddress  = ( u32CurrAddress << 8);
 	 		u32CurrAddress  |= au8Header[NXT_ADDR1_INDEX];
 	 		u32CurrAddress  = ( u32CurrAddress << 8);
 	 		u32CurrAddress  |= au8Header[NXT_ADDR0_INDEX];
 			cRetVal = TRUE;
 		} 
 		else
 		{
 			//both checksums bad, memory is not initialized or corrupted
 			cRetVal = FALSE;
 		}	
 	}   		
	return cRetVal;
} 	
/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnLogData -->
       \brief Add a record to the buffer for storing in memory

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8DataLen  Length of data to store in bytes
       @param[in]  pu8Data		pointer to the data to store in the record

<!-- RETURNS: -->
      \return TRUE if succesful. FALSE if unsuccesful.

*/
/******************************************************************************/
char EXM_fnLogData(uint8 *pu8Data, uint8 u8DataLen)
{
	uint8 u8Index;    
	uint8 u8ElemIndex;
	
	uint8 u8NumElem;
	uint8 u8RemainElem =0;

    if ( ! Datalog.Ext_Mem_Present )
        return FALSE;

 	//Check if record is too big or small
 	if((u8DataLen > MAX_LOG_LENGTH) ||( u8DataLen < 1))
 	{
 			return FALSE;
 	}
 	//check if there is enough room in memory
 	if( (u8DataLen + u32CurrAddress) > EX_MEM_SIZE)
 	{
 			return FALSE;
 	}

 	u8NumElem = u8DataLen / BUFF_ELEM_SIZE;
 	
 	if((u8DataLen % BUFF_ELEM_SIZE) > 0 )
 	{
 		//if there's remaining data need another buffer element
 		u8RemainElem = 1;
 	} 
 	
 	// If the write buffer is not full (0ct 12, changed from <= ; TM)
	if((u8NumElem + u8BufferCount) <  EXT_BUFFER_SIZE)
    {
  	    bBusy = TRUE;	
 		//no Checksum for now
  	    // Buffer the record data values
   	    for(u8ElemIndex = 0; u8ElemIndex < u8NumElem; u8ElemIndex++)
   	    {
   		    u8BufferCount++;
   		    for ( u8Index = 0; u8Index < BUFF_ELEM_SIZE; u8Index++ )
   		    {
    		    au8RecordBuff[u8BufferCount - 1].au8Data[u8Index] 
    		        = pu8Data[(u8ElemIndex * BUFF_ELEM_SIZE) +  u8Index];
   		    }
   	        au8RecordBuff[u8BufferCount - 1].u8DataLen = BUFF_ELEM_SIZE;
   	    }
   	    if(u8RemainElem ==1)
   	    {
   		    u8BufferCount++;
   		    for ( u8Index = 0; u8Index < (u8DataLen % BUFF_ELEM_SIZE); u8Index++ )
   		    {
    		    au8RecordBuff[u8BufferCount - 1].au8Data[u8Index]
    		    = pu8Data[(u8ElemIndex * BUFF_ELEM_SIZE) +  u8Index];
   		    }
   	        au8RecordBuff[u8BufferCount - 1].u8DataLen = (u8DataLen % BUFF_ELEM_SIZE);
   	    }
   	    return TRUE;
 	}
    // Failure
    Add_Event (EXT_MEMORY_FAULT);
  return FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnProcess -->
       \brief Process the storing of records in memory

<!-- PURPOSE: -->
			This function should be called periodically by the application to process 
			the storing of data into the memory. This function should not be called 
			more than once per 10 ms or functionality is not guaranteed.
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return void.

*/
/******************************************************************************/
void EXM_fnProcess(void)
{
	//local temporary variables
	uint16 u16TmpChksm;
	static uint32 u32LastAddrInPage;
	uint8 u8ChipSel;
	
	static uint8	u8BytesSent = 0;
	static uint16	u16CurrentPage = 0; //doesn't need to be static
	static uint8  u8ControlByte;
	static uint8	 u8CurrentBlock;  //0-15, 0 is the low block of chip 0,
	                                //15 is the high block of chip 7
	
	//SET TRIS Registers every time just to make sure.
	TRISF &= 0xC0;
	
	// If the serial driver is not busy
 	if ( ssgDone )
  {
  	switch ( tCurrentState )
    {
        case IDLE:
        {
            //checking for pending reset in memory space 1
      	    if(bResetPending)
      	    { 
      		    //Process reset
    			u32CurrAddress	= 2*HDR_W_CHKSM_LEN;
      		    bBusy=TRUE;
      		    tCurrentState = WRITE_HEADER1;
      	    }
      	    // If there are buffered records
            else if ( u8BufferCount > 0 )
     	    {	
			    u16CurrentPage = ( u32CurrAddress)/PAGE_SIZE;
       		    u32LastAddrInPage =(((uint32)( u16CurrentPage + 1) * PAGE_SIZE)-1 );
       		    u8CurrentBlock = (uint8)( u32CurrAddress >> 16);
       		
       		    //select chip to write to
       		    u8ChipSel = u8CurrentBlock/2;
                PORTF &= 0xC8;
			    PORTF |= (u8ChipSel | 0x08);
			    //setup control byte
			    if((u8CurrentBlock %2) == 0 )
       		    {
       			    u8ControlByte = EXM_DEVICE_LOW;
       		    }
       		    else
       		    { 
       			    u8ControlByte =EXM_DEVICE_HIGH;
       		    } 	 
       	
       		    //check if record fits on the current page
       		    if( (u32CurrAddress + au8RecordBuff[0].u8DataLen - 1) > u32LastAddrInPage)
       		    {
       			    //Doesn't all fit on this page. Write some of the data.
       			    u8BytesSent = ( u32LastAddrInPage - u32CurrAddress ) + 1;	

           			if (ssfI2CWriteTo( u8ControlByte, (uint16)u32CurrAddress,au8RecordBuff[0].au8Data, 
       										 u8BytesSent ) == FALSE)
                        return;
            
                    u32CurrAddress += u8BytesSent;
       			    bBusy=TRUE;
       			    tCurrentState = WRITE2;		   			
       		    }	
        	    else
        	    {
        		    //Does all fit on this page. Write all of the data.     			
  			
       			    if (ssfI2CWriteTo( u8ControlByte, (uint16)u32CurrAddress,au8RecordBuff[0].au8Data, 
                         au8RecordBuff[0].u8DataLen) == FALSE)
                        return;
						
				    u32CurrAddress += au8RecordBuff[0].u8DataLen;	
						
          	        bBusy=TRUE;
          	        tCurrentState = WRITE_HEADER1;
        	    }
 		    }
   		}
      break;
      
      case WRITE2:
      {
    		//Write the remaining data that didn't fit on the next page
        u8CurrentBlock = (uint8)( u32CurrAddress >> 16);
        u8ChipSel = u8CurrentBlock/2;
        PORTF &= 0xC8;
				PORTF |= (u8ChipSel | 0x08);
			    		
       	if((u8CurrentBlock %2) ==0 )
       	{
       		u8ControlByte = EXM_DEVICE_LOW;
       	}
       	else
       	{ 
       		u8ControlByte =EXM_DEVICE_HIGH;
       	} 
       	          
        if (ssfI2CWriteTo( u8ControlByte, (uint16)u32CurrAddress,
                       &(au8RecordBuff[0].au8Data[u8BytesSent]),
                         au8RecordBuff[0].u8DataLen - u8BytesSent) == FALSE)
            return;
       
     		u32CurrAddress += au8RecordBuff[0].u8DataLen - u8BytesSent;
        tCurrentState = WRITE_HEADER1;
      }
      break; 
        
   		case WRITE_HEADER1:
      { 
      	//update the header of the first page
			  //select chip 0
			  PORTF &= 0xC8;
			  PORTF |= 0x08;
      	u8ControlByte = EXM_DEVICE_LOW; 
      	
      	au8HeaderBuff[KEY1_INDEX]      = KEY1_CHAR;
        au8HeaderBuff[KEY2_INDEX]      = KEY2_CHAR;
   
        au8HeaderBuff[NXT_ADDR2_INDEX] = (uint8)(u32CurrAddress >> 16);
        au8HeaderBuff[NXT_ADDR1_INDEX] = (uint8)(u32CurrAddress >> 8);  
        au8HeaderBuff[NXT_ADDR0_INDEX] = (uint8) u32CurrAddress;

				u16TmpChksm = fnCalcChecksum(au8HeaderBuff,HEADER_LENGTH);
				
				au8HeaderBuff[CKSMH_INDEX] =  (uint8)(u16TmpChksm >>8);
				au8HeaderBuff[CKSML_INDEX] =  (uint8)(u16TmpChksm);
				
          
        if (ssfI2CWriteTo( u8ControlByte, HEADER1_ADDR, au8HeaderBuff,
                       HDR_W_CHKSM_LEN ) == FALSE)
            return;
                
        tCurrentState = WRITE_HEADER2;
      }         
      break;
      
      case WRITE_HEADER2:
      {   //write the second header
      	//select chip 0
        PORTF &= 0xC8;
			  PORTF |= 0x08;
      	u8ControlByte = EXM_DEVICE_LOW; 
        if (ssfI2CWriteTo( u8ControlByte,HEADER2_ADDR , au8HeaderBuff,
                        HDR_W_CHKSM_LEN ) == FALSE)
            return;
    
        tCurrentState = WAIT_STATE;
      }
			break;
			
			case WAIT_STATE:
			{
				//Do nothing just wait once.
				tCurrentState = DONE_WRITE;
			}
			break;
			
			case DONE_WRITE:
      { 	  
      	if(!bResetPending )
      	{
      	 //Not performing a reset so must have just finished storing a
      	 //record. Remove record from buffer. 
            
          // Ensure u8BufferCount > 0. Added for PPR032836
          if (u8BufferCount > 0)      
          {
             fnShiftBuffer();
             u8BufferCount--;
          }             

          if(u8BufferCount == 0)
          {
           	bBusy = (u8BufferCount >0);
          }
      	}
      	else
      	{
      		//Just finished a reset.    
        		bResetPending = FALSE;
        		bBusy= (u8BufferCount >0); 	//still busy if data buffered
      	}	
        tCurrentState = IDLE;
      }                		
   	  break;	
           
    	default:
      	tCurrentState = IDLE;
        break;
    }
  }

    u32CurrAddress_To_View = u32CurrAddress;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnReset -->
       \brief Used to clear a memory space.

<!-- PURPOSE: -->
			This method effectively clears all data in the external memory.
<!-- PARAMETERS: -->
<!-- RETURNS: -->
      \return void

*/
/******************************************************************************/
void EXM_fnReset(void)
{
  	bBusy = TRUE;

  	bResetPending = TRUE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnCheckBusy -->
       \brief Used to check if a module is busy. (has data pending a write)

<!-- PURPOSE: -->
<!-- PARAMETERS: None-->


<!-- RETURNS: -->
      \return TRUE if busy, FALSE if not.

*/
/******************************************************************************/
char EXM_fnCheckBusy(void)
{
	return bBusy;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnCheckBytesLeft -->
       \brief Used to check remaining space in the external memory.

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
<!-- RETURNS: -->
      \return The number of bytes left in the external memory.

*/
/******************************************************************************/
uint32 EXM_fnCheckBytesLeft(void)
{
	return (EX_MEM_SIZE - u32CurrAddress);
} 

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EXM_fnIsConnected -->
       \brief Check if external memory is connected..

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
<!-- RETURNS: -->
      \return TRUE if connected. Else FALSE

*/
/******************************************************************************/
char EXM_fnIsConnected(void)
{
	return bConnected;
} 
 
/******************************************************************************/
/*!

<!-- FUNCTION NAME: fnShiftBuffer -->
       \brief Helper function to update the buffer

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return void

*/
/******************************************************************************/
static void fnShiftBuffer( void )
{
	uint8 i;
	//(0ct 12, was off by one, fixed ; TM)
    // Check for i << BUFFER_SIZE added for PPR032836
    for(i=1; (i < u8BufferCount) && (i < EXT_BUFFER_SIZE); i++)
	{
		 au8RecordBuff[i-1] =  au8RecordBuff[i];
	}
}	
/******************************************************************************/
/*!

<!-- FUNCTION NAME: fnCalcChecksum -->
       \brief Helper function to calculate a checksum

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8DataLen       Length of the data to caculate checksum over
       @param[in]  pu8CksmData     Pointer to checksum data
<!-- RETURNS: -->
      \returns the 16 bit checksum

*/
/******************************************************************************/				
static uint16 fnCalcChecksum(uint8 *pu8CksmData, uint8 u8DataLen)
{
	  uint8 u8DataIndex;
    uint32 u32CalcChksm = 0;
            
    for(u8DataIndex = 0; u8DataIndex < u8DataLen; u8DataIndex++  )
    {
        u32CalcChksm += pu8CksmData[u8DataIndex];      
        u32CalcChksm += ((u32CalcChksm & 0x10000) >> 16);     // Add Carry
        u32CalcChksm &= 0x0000FFFF;                           //Clear Carry
    }

    return (uint16)(~(u32CalcChksm & 0x0000FFFF));
}
#endif // EXT_MEM
