/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file OnBoardNvmem.c
       \brief Header file defining interface to On Board Non-Volatile Memory
       
<!-- DESCRIPTION: -->
       Data is stored in the EEPROM as fixed length records in 2 circular memory 
       spaces. The size of each memory space, as well as the size of the 
       individual records is configurable by the user via a literal definition. 
       The first two records in each memory space are reserved for storing status 
       information for the respective memory space, including the next available 
       write address and the number of stored records.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
       $Header:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/OnBoardNvmem.c-arc   1.0.1.0   Jun 08 2018 17:20:26   ctctmas  $
    
       $Log:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/OnBoardNvmem.c-arc  $
 * 
 *    Rev 1.1   Apr 24 2019 ctctmas
 * RTC_064664 APU minimum compressor off time 3.5 minutes
 *  Move this file to RTC Tip 
 * 
 *    Rev 1.0.1.0   Jun 08 2018 17:20:26   ctctmas
 * PPR032836
 * Experimental Modifications to Prevent Corruption Due to Buffer Overrun.
 * 
 *    Rev 1.0   Jun 08 2018 17:10:22   ctctmas
 * Initial Carrier revision.

       Revision 1.1  2014/06/27 17:35:26  blepojevic
       This is former software version DCP3-019-02K imported to new folder APU\DieselAPU\DCP and will be tagged using new naming convention starting from TD-DCP-02.02.01

       Revision 1.4  2014/06/27 17:35:26  blepojevic
       Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

       Revision 1.3  2014/04/16 14:07:06  blepojevic
       Added diagnostic of APU and CCU

       Revision 1.2  2013/09/19 13:07:42  blepojevic
       Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

       Revision 1.1  2009/10/14 12:37:14  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.10  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.9  2008-04-02 19:50:10  msalimi
       - Fixed onboard memory reading bug.
       - There is no one hour recall of faults if APU is off.
       - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
       - DPF lost fault will not be detected if APU is off.

       Revision 1.8  2008-03-26 22:45:18  gfinlay
       Fixed file name in header comment block

       Revision 1.7  2008-03-26 22:39:11  gfinlay
       Fixed file header comment block so it parses properly for Doxygen, and so it retrieves the file history log properly with CVS.

       \endif
*/
/******************************************************************************/
#ifdef OBM
/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "OnBoardNvmem.h"
#include "syncserial.h"
#include "tfxstddef.h"
#include "rtclock.h"
#include "menu.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#define OBM_DEVICE				0xA7    //node address for NVMEM
#define NUM_MEM_SPACE 			2       //number of memory spaces

#define PAGE_SIZE				64		    //Size of each page in bytes.	
#define TOT_MEM_SIZE			32768     //Must be > MEM1_SIZE + RECORD_SIZE and
                                                                        // less than TOT_MEM_SIZE

#define NUM_HEADERS				2        //number of headers per memory space   

#define HEADER_OFFSET			( NUM_HEADERS * HDR_W_CHKSM_LEN )

//Defines for header information

//Relative addresses of Headers.
#define HEADER1_ADDR			 0										
#define HEADER2_ADDR			 ( 1 * HDR_W_CHKSM_LEN ) 

#define CKSM_LENGTH				 2    //16 bit Checksum

#define HEADER_LENGTH			 6   //length of the header without checksum

#define REC_W_CHKSM_LEN			(RECORD_SIZE + CKSM_LENGTH)
#define HDR_W_CHKSM_LEN			(HEADER_LENGTH + CKSM_LENGTH)

//Relative Indices for information in the header 
#define KEY1_INDEX				0
#define KEY2_INDEX				1

#define NUM_RECH_INDEX			2
#define NUM_RECL_INDEX			3

#define NXT_ADDRH_INDEX			4
#define NXT_ADDRL_INDEX			5

#define CKSMH_INDEX				6
#define CKSML_INDEX				7

//Key characters to see verify data validity
#define KEY1_CHAR				0x55
#define KEY2_CHAR				0xAA


/* ---------- L o c a l  S t r u c t u r e s ---------- */

typedef enum
{
    MEM_SPACE1 = 0,
    MEM_SPACE2 = 1,
}teMEM_SPACE_SEL;
 
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
     teMEM_SPACE_SEL teMemSpaceSel; //selects which memory space to store in
     uint8 record[REC_W_CHKSM_LEN];//the data
}zRECORD;


/* ---------- L o c a l    D a t a ---------- */


//record buffer variables
static zRECORD	au8RecordBuff[BUFFER_SIZE];
static uint8 	u8BufferCount = 0;

//memory management variables

//mememory space is divided into slots which are the size of a record

#define NUM_SLOTS1  (( MEM1_SIZE - 2*HDR_W_CHKSM_LEN) / REC_W_CHKSM_LEN )

#define NUM_SLOTS2  (( TOT_MEM_SIZE -  MEM1_SIZE - 2*HDR_W_CHKSM_LEN)    \
                                       / REC_W_CHKSM_LEN )

static uint16  u16CurrentSlot1;
static uint16  u16CurrentSlot2;
static unsigned char OBM_LastData [10];
static unsigned char month;
static unsigned char day;
static unsigned char year;
static uint16 dayOfYearAprox;
static uint16 dayOfYearAproxRTC;
static char currentDay; 

static uint16	u16NumRecords1;
static uint16	u16NumRecords2;

static bit bResetPending1 = FALSE;
static bit bResetPending2 = FALSE;		

static bit bBusy= FALSE;

static tOBM_STATE tCurrentState = IDLE; 

static uint8 au8HeaderBuff[8];

/* ----------------- L o c a l  F u n c t i o n s ----------------- */
static uint16 fnCalcChecksum(uint8 *pu8CksmData,uint8 u8DataLen);

static void fnShiftBuffer(void);

/* ----------------- Global F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnInit -->
       \brief Initialize a memory space

<!-- PURPOSE: -->
            Reads information from the header and initializes the memory space.
            This function needs to be called for each memory space at startup.
<!-- PARAMETERS: -->
       @param[in]  u8MemSel   Selects which memory space to initialize

<!-- RETURNS: -->
      \return TRUE if succesful. FALSE if unsuccesful.

*/
/******************************************************************************/

char OBM_fnInit( uint8 u8MemSel )
{
    char cRetVal;
    //Expected checksums calculated from headers 1 and 2
    static uint16 u16CalcCksm1;
    static uint16 u16CalcCksm2;
    
    //Checksums read from headers 1 and 2
    static uint16 u16ReadCksm1;
    static uint16 u16ReadCksm2;
    
    //temporary variables
    uint16 u16MyBaseAddress; 
    uint16 u16MyNumRecords=0;
    uint16 u16MyCurrentAddress=0;
    uint8 au8Header[HDR_W_CHKSM_LEN]; 
     
    if( (u8MemSel -1) == MEM_SPACE1 )
    {
        u16MyBaseAddress =0;
    }
    else if( (u8MemSel -1) == MEM_SPACE2 )
    {
        u16MyBaseAddress = MEM1_SIZE;
    }
      else
    {
        return FALSE;
    }

    //initialize local data
  
    //read header 1
    while ( !ssgDone );
    (void) ssfI2CReadFrom( OBM_DEVICE, u16MyBaseAddress, au8Header, HDR_W_CHKSM_LEN);
    while(!ssgDone); 
  
    //save checksum values from header 
    u16CalcCksm1 = fnCalcChecksum(au8Header,HEADER_LENGTH);
    u16ReadCksm1 = au8Header[CKSMH_INDEX];
    u16ReadCksm1 = ( u16ReadCksm1 << 8);
    u16ReadCksm1 |= au8Header[CKSML_INDEX];
  
    if( u16CalcCksm1 == u16ReadCksm1)
    {
        //checksum for header 1 is good use it
        u16MyNumRecords = au8Header[NUM_RECH_INDEX];
        u16MyNumRecords  = ( u16MyNumRecords << 8);
        u16MyNumRecords  |= au8Header[NUM_RECL_INDEX];
    
        u16MyCurrentAddress  = ( au8Header[NXT_ADDRH_INDEX]);
        u16MyCurrentAddress  = ( u16MyCurrentAddress << 8);
        u16MyCurrentAddress  |= au8Header[NXT_ADDRL_INDEX];
        cRetVal = TRUE;
    }
    //checksum for header 1 is bad
    else
    {
        //read second header
        while ( !ssgDone );
        (void) ssfI2CReadFrom( OBM_DEVICE, u16MyBaseAddress+HDR_W_CHKSM_LEN, au8Header,
                                 HDR_W_CHKSM_LEN);
        while(!ssgDone); 
 
        //calculate checksum
    
        u16CalcCksm2 = fnCalcChecksum(au8Header,HEADER_LENGTH);

        u16ReadCksm2 = au8Header[CKSMH_INDEX];
        u16ReadCksm2 = ( u16ReadCksm2 << 8);
        u16ReadCksm2 |= au8Header[CKSML_INDEX];	
                
        if( u16CalcCksm2 == u16ReadCksm2)
        {
            //checksum 2 is good, use 2nd header
            u16MyNumRecords  = au8Header[NUM_RECH_INDEX];
            u16MyNumRecords  = ( u16MyNumRecords << 8);
            u16MyNumRecords  |= au8Header[NUM_RECL_INDEX];
    
            u16MyCurrentAddress = ( au8Header[NXT_ADDRH_INDEX]);
            u16MyCurrentAddress = ( u16MyCurrentAddress << 8);
            u16MyCurrentAddress |= au8Header[NXT_ADDRL_INDEX];
            cRetVal = TRUE;
        } 
        else
        {
            //both checksums bad, memory space is not initialized or corrupted
            cRetVal = FALSE;
        }	
    }   		
    
    //Save the Current Address and Number of Records
    if( (u8MemSel -1) == MEM_SPACE1 )
    {
        u16CurrentSlot1 = ( (u16MyCurrentAddress - HDR_W_CHKSM_LEN - u16MyBaseAddress)/ REC_W_CHKSM_LEN );
        u16NumRecords1 = u16MyNumRecords;
    }
    else if( (u8MemSel -1) == MEM_SPACE2 )
    {
        u16CurrentSlot2 = ( (u16MyCurrentAddress - HDR_W_CHKSM_LEN - u16MyBaseAddress)/ REC_W_CHKSM_LEN );
        u16NumRecords2 = u16MyNumRecords;
    }

    return cRetVal;
} 	
/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnAddRecord -->
       \brief Add a record to the buffer for storing in memory

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8MemSel   Selects which memory space to store record in
       @param[in]  pu8Data		pointer to the data to store in the record

<!-- RETURNS: -->
      \return TRUE if succesful. FALSE if unsuccesful.

*/
/******************************************************************************/
char OBM_fnAddRecord(uint8 *pu8Data, uint8 u8MemSel)
{
    uint8 u8Index;
    uint16 u16TmpChksm;
    
    //can't add records if either memroy space is resetting
    //if(( bResetPending1 || bResetPending2))
    //	return FALSE;
    
    // If the write buffer is not full
    if ( u8BufferCount < BUFFER_SIZE )
    {
        u8BufferCount++;
        bBusy = TRUE;	
        u16TmpChksm = fnCalcChecksum(pu8Data,RECORD_SIZE);
    
        //set the memory select of the record
        au8RecordBuff[u8BufferCount - 1].teMemSpaceSel = u8MemSel -1;
        // Buffer the record data values
        for ( u8Index = 0; u8Index < RECORD_SIZE; u8Index++ )
        {
            au8RecordBuff[u8BufferCount - 1].record[u8Index] = pu8Data[u8Index];
        }
    
        //Add the checksum to the record buffer
        //high byte
        au8RecordBuff[u8BufferCount - 1].record[u8Index] = (uint8)(u16TmpChksm >>8);
        //low byte
        au8RecordBuff[u8BufferCount - 1].record[u8Index+1]= (uint8)(u16TmpChksm);

        return TRUE;
    }
    // Failure
    return FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnProcess -->
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
void OBM_fnProcess(void)
{
    //local temporary variables
    uint16 u16TmpChksm;
    uint16 u16LastAddrInPage;
    
    static uint8	u8BytesSent = 0;
    static teMEM_SPACE_SEL teMemSel;
    
    static uint16	u16CurrAddress = 0;
    static uint16	u16CurrentPage= 0; 
    static uint16	u16BaseAddress;
    
    // If the serial driver is not busy
    if ( ssgDone )
    {
        switch ( tCurrentState )
        {
            case IDLE:
            {
                //checking for pending reset in memory space 1
                if(bResetPending1)
                { //Process reset for memory space 1
                    teMemSel = MEM_SPACE1;
                    u16CurrAddress	= 2*HDR_W_CHKSM_LEN;
                    u16NumRecords1  = 0;
                    u16CurrentSlot1 = 0;    	
                    u16BaseAddress = 0;
                    //	u8BufferCount =0; //clear the buffer
                    bBusy=TRUE;
                    tCurrentState = WRITE_HEADER1;
                }
                //checking for pending reset in memory space 2
                else if( bResetPending2 )
                { //Process reset for memory space 1
                    teMemSel = MEM_SPACE2;
                    u16CurrAddress = 	MEM1_SIZE + 2*HDR_W_CHKSM_LEN;
                    u16NumRecords2  =  0;
                    u16CurrentSlot2 = 0;
                    u16BaseAddress = MEM1_SIZE; 
                    //	u8BufferCount =0; //clear the buffer
                    bBusy=TRUE;
                    tCurrentState = WRITE_HEADER1;
                }
                    
                // If there are buffered records
                else if ( u8BufferCount > 0 )
                {	
                    //check which memory space to store record in
                    teMemSel = au8RecordBuff[0].teMemSpaceSel;
                    if( teMemSel == MEM_SPACE1 )
                    {
                        u16BaseAddress = 0;
                        u16CurrAddress = (u16CurrentSlot1* REC_W_CHKSM_LEN) + HEADER_OFFSET + u16BaseAddress;
                    }
                    else
                    {
                        u16BaseAddress = MEM1_SIZE;
                        u16CurrAddress = (u16CurrentSlot2 * REC_W_CHKSM_LEN) + HEADER_OFFSET + u16BaseAddress;
                    }
                                    
                    u16CurrentPage = u16CurrAddress/PAGE_SIZE;

                    u16LastAddrInPage =( (( u16CurrentPage + 1) * PAGE_SIZE)-1 );
            
                    //check if record fits on the current page
                    if( (u16CurrAddress + REC_W_CHKSM_LEN)  > u16LastAddrInPage)
                    {
                        //Doesn't all fit on one page. Write some of the data.
                        u8BytesSent =( u16LastAddrInPage - u16CurrAddress ) + 1;	
                        ssfI2CWriteTo( OBM_DEVICE, u16CurrAddress,au8RecordBuff[0].record, 
                                             u8BytesSent );                
                        u16CurrAddress += u8BytesSent;
                        bBusy=TRUE;
                        tCurrentState = WRITE2;   			
                    }
            
                    else
                    {
                        //Does all fit on one page. Write all of the data.     			
                        ssfI2CWriteTo( OBM_DEVICE, u16CurrAddress,au8RecordBuff[0].record, 
                        REC_W_CHKSM_LEN);      
                        
                        u16CurrAddress += REC_W_CHKSM_LEN;
                
                        if( teMemSel == MEM_SPACE1 )
                        {
                            u16CurrentSlot1 = (u16CurrentSlot1+ 1)% NUM_SLOTS1;
                            if( u16NumRecords1 < NUM_SLOTS1 )
                            {
                                u16NumRecords1++;
                            }
                        }
                        else
                        {
                            u16CurrentSlot2 = (u16CurrentSlot2 + 1)% NUM_SLOTS2;
                            if( u16NumRecords2 < NUM_SLOTS2 )
                            {
                                u16NumRecords2++;
                            }
                        }        
                        bBusy=TRUE;
                        tCurrentState = WRITE_HEADER1;
                    }
                    // else nothing to do
                }
            }
            break;
      
            case WRITE2:
            {
                //Write the remaining data on the next page
                ssfI2CWriteTo( OBM_DEVICE, u16CurrAddress, &(au8RecordBuff[0].record[u8BytesSent]), REC_W_CHKSM_LEN - u8BytesSent);     
       
                u16CurrAddress += ( REC_W_CHKSM_LEN - u8BytesSent );
            
                if( teMemSel == MEM_SPACE1 )
                {
                    u16CurrentSlot1 = (u16CurrentSlot1+ 1)% NUM_SLOTS1;
                    if( u16NumRecords1 < NUM_SLOTS1 )
                    {
                        u16NumRecords1++;
                    }
                }
                else
                {
                    u16CurrentSlot2 = (u16CurrentSlot2 + 1)% NUM_SLOTS2;
                    if( u16NumRecords2 < NUM_SLOTS2 )
                    {
                        u16NumRecords2++;
                    }
                }   
                tCurrentState = WRITE_HEADER1;
            }
            break; 
        
            case WRITE_HEADER1:
            { 
                //update the header of the first page
                au8HeaderBuff[KEY1_INDEX]      = KEY1_CHAR;
                au8HeaderBuff[KEY2_INDEX]      = KEY2_CHAR;
                
    
                au8HeaderBuff[NXT_ADDRH_INDEX] = (uint8)(u16CurrAddress >> 8);
                au8HeaderBuff[NXT_ADDRL_INDEX] = (uint8) u16CurrAddress;

                
                if( teMemSel == MEM_SPACE1 )
                { 
                    au8HeaderBuff[NUM_RECH_INDEX]  = (uint8)(u16NumRecords1 >>8);
                    au8HeaderBuff[NUM_RECL_INDEX]  = (uint8) u16NumRecords1;
                }
                else
                {
                    au8HeaderBuff[NUM_RECH_INDEX]  = (uint8)(u16NumRecords2 >>8);
                    au8HeaderBuff[NUM_RECL_INDEX]  = (uint8) u16NumRecords2;
                }
                u16TmpChksm = fnCalcChecksum(au8HeaderBuff,HEADER_LENGTH);
                
                au8HeaderBuff[CKSMH_INDEX] =  (uint8)(u16TmpChksm >>8);
                au8HeaderBuff[CKSML_INDEX] =  (uint8)(u16TmpChksm);
                
          
                ssfI2CWriteTo( OBM_DEVICE, u16BaseAddress, au8HeaderBuff, HDR_W_CHKSM_LEN );
                
                tCurrentState = WRITE_HEADER2;
            }         
            break;
      
            case WRITE_HEADER2:
            {   //write the second header 
                ssfI2CWriteTo( OBM_DEVICE, u16BaseAddress + HDR_W_CHKSM_LEN, au8HeaderBuff, HDR_W_CHKSM_LEN );
     
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
                if(!( bResetPending1|| bResetPending2))
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
                        bBusy = FALSE;
                    }
                }
                else
                {
                    //Just finished a reset.    
                    if( teMemSel == MEM_SPACE1 )
                    {
                        bResetPending1 = FALSE;
                    }
                    else
                    {
                        bResetPending2 = FALSE;
                    }
                    //if one of the memory spaces still has a reset pending then
                    //module is still busy
                    bBusy= (bResetPending1 || bResetPending2 || (u8BufferCount != 0)); 	
                }	
                tCurrentState = IDLE;
            }                		
            break;	
           
            default:
                tCurrentState = IDLE;
            break;
        }
    }
}
/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnGetRecord -->
       \brief Retrieve a record from the memory

<!-- PURPOSE: -->
            Returns in *pu8Data the record given by u8RecordNum, where 1 represents 
            the most recently added.  Returns TRUE if successful or FALSE if 
            unsuccessful. A false condition will be returned if the buffer is busy or 
            if the checksum is invalid. The size of u8RecordNum should be less than 
            the total number stored records, or request is ignored and method 
            returns FALSE. User application should call fnCheckBusy first to see if 
            there are buffered records.
<!-- PARAMETERS: -->
       @param[in]  u8MemSel       Selects which memory space to retrieve from
       @param[in]  pu8Data		    Pointer to location to store record data
       @param[in]  u8RecordNum		The record to recieve (1 is most recent)

<!-- RETURNS: -->
      \return TRUE if succesful. FALSE if unsuccesful.

*/
/******************************************************************************/
char OBM_fnGetRecord(unsigned char *pu8Data, uint16 u16RecordNum, uint8 u8MemSel)
{
    char cRetVal = FALSE;
    //temporary local variables
    uint16 u16CalcCksm;
    uint16 u16ReadCksm;
    
    int16 i16ReadSlot;
    uint16 u16RxAddress;
    
    uint16 u16MyBaseAddress;
    uint16 u16MyNumRecords;
    uint16 u16MyCurrentSlot;
    uint16 u16MyNumSlots;
    
    if( (u8MemSel -1) == MEM_SPACE1 )
    {
        u16MyBaseAddress 	= 0;
        u16MyNumRecords = u16NumRecords1;
        u16MyCurrentSlot = u16CurrentSlot1;
        u16MyNumSlots = NUM_SLOTS1;
    }
    else if( (u8MemSel -1) == MEM_SPACE2 )
    {
        u16MyBaseAddress 	= MEM1_SIZE;
        u16MyNumRecords = u16NumRecords2;
        u16MyCurrentSlot = u16CurrentSlot2;
        u16MyNumSlots = NUM_SLOTS2;
    }
    else
    {
        return FALSE;
    }	

    //make sure record requested is in range and that memspace is not busy
    if( u16RecordNum <= u16MyNumRecords && !bBusy )
    {	
        //find which record slot is requested
        i16ReadSlot = u16MyCurrentSlot  - u16RecordNum;	
        if( i16ReadSlot >= 0 )
        {
            u16RxAddress = (i16ReadSlot * REC_W_CHKSM_LEN) + HEADER_OFFSET
                                            + u16MyBaseAddress ;
        }
        else
        {
            //if the slot is negative then we have wrapped around
            u16RxAddress = ((u16MyNumSlots + i16ReadSlot) * REC_W_CHKSM_LEN) 
                                                + HEADER_OFFSET + u16MyBaseAddress;
        }

        //read from the memory
        while ( !ssgDone);
        (void) ssfI2CReadFrom( OBM_DEVICE, u16RxAddress, pu8Data, REC_W_CHKSM_LEN);
        while(!ssgDone);	
        
        u16CalcCksm =  fnCalcChecksum(pu8Data,RECORD_SIZE);
        
        u16ReadCksm = pu8Data[RECORD_SIZE];
        u16ReadCksm = ( u16ReadCksm << 8);
        u16ReadCksm |= pu8Data[RECORD_SIZE+1];
        //verify the checksum
        if( u16CalcCksm == u16ReadCksm)
        {
            cRetVal = TRUE;
        } 
    }

    return cRetVal;
}
/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnReset -->
       \brief Used to clear a memory space.

<!-- PURPOSE: -->
            This method effectively clears all records in the memory space indicated 
            by u8MemSel. The init method should not be called after resetting.
<!-- PARAMETERS: -->
       @param[in]  u8MemSel       Selects which memory space to reset

<!-- RETURNS: -->
      \return void

*/
/******************************************************************************/
void OBM_fnReset(uint8 u8MemSel)
{
    bBusy = TRUE;
    if( (u8MemSel -1) == MEM_SPACE1 )
    {
        bResetPending1 = TRUE;
    }
    else if( (u8MemSel -1) == MEM_SPACE2 )
    {
        bResetPending2 = TRUE;
    }
    else
    {
        //Do nothing
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnCheckBusy -->
       \brief Used to check if a module is busy. (has data pending a write)

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8MemSel    Selects which memory space to reset

<!-- RETURNS: -->
      \return TRUE if busy, FALSE if not.

*/
/******************************************************************************/
unsigned char OBM_fnCheckBusy(void)
{
    return bBusy;
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

    // Check for i < BUFFER_SIZE added for PPR032836
    for(i=0; (i < u8BufferCount) && (i < BUFFER_SIZE); i++)
    {
         au8RecordBuff[i] =  au8RecordBuff[i+1];
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
    
/******************************************************************************/
/*!

<!-- FUNCTION NAME: OBM_fnGetTotalRecords -->
       \brief Add a record to the buffer for storing in memory

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8MemSel   Selects which memory space to store record in

<!-- RETURNS: -->
      \return The number of records in the memory space .

*/
/******************************************************************************/
uint16 OBM_fnGetTotalRecords(uint8 u8MemSel)
{
    if( (u8MemSel -1) == MEM_SPACE1 )
    {
        return u16NumRecords1;
    }
    else if( (u8MemSel -1) == MEM_SPACE2 )
    {
        return u16NumRecords2;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
/* <!-- FUNCTION NAME: nvmUpdate -->  Erase all records older than 30 days in memory

<!-- PARAMETERS: -->
<!-- RETURNS: -->
*/
/******************************************************************************/
void nvmUpdate(void)
{
    if (currentDay != rtgClock.rtxDate)		  // Every day one time check last record (when changed day in the month) 
    { 
        OBM_fnGetRecord (&OBM_LastData[0], 1, 1);	  // Read last record from EEPROM (! last record has RecordNum = 1)
        day = (OBM_LastData[2] & 0x3E) >> 1;
        month = (OBM_LastData[2] & 0xC0) >> 6;
        month += (OBM_LastData [3] & 0x03) << 2;
        year = OBM_LastData[3] >> 2;
            
        dayOfYearAprox = day + (month - 1)* 30;
        dayOfYearAproxRTC = rtgClock.rtxDate + ((rtgClock.rtxMonth - 1) * 30);

        if ( dayOfYearAprox + 30 <  dayOfYearAproxRTC)		  // clear all faults, if from last fault expired 30 days
        {
            OBM_fnReset(1);
            ResetFaults();
        }
        else if ((year == rtgClock.rtxYear - 1) && (rtgClock.rtxMonth == 1) &&  (month == 12))
        {
           if (day <= rtgClock.rtxDate) 	// in January clear all faults, if from last fault in December previous year expired 30 days
           {
               OBM_fnReset(1);
               ResetFaults();
           }
        }
        else if (year < rtgClock.rtxYear)
        {
            OBM_fnReset(1);
            ResetFaults();
        }
        currentDay = rtgClock.rtxDate;
    }    
}
        
#endif // OBM
