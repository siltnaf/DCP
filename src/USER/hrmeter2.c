/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file hrmeter2.c
       \brief Module to provide hours of APU runtime.

<!-- DESCRIPTION: -->
       The APU run time is stored internally as minutes. The EEPROM is updated with the
       APU run time as persistent data so that the next time the DCP boots, it will remember the 
       accumulated total seen so far.

<!-- NOTES: -->

       \if ShowHist
       Copyright ?2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/hrmeter2.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2009/10/16 22:40:02  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.8  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.7  2008-07-14 16:52:42  msalimi
	      Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

	      Revision 1.6  2008-03-25 19:14:38  msalimi
	      - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
	      - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
	      - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
	      - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
	      - APU cooldown period is reduced to 2 minutes.
	      - DPF related faults won't be captured when APU is off.
	      - Freed code space by removing redundant code.
	      - Faults are not displayed in DPF menu anymore.
	      - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

	      Revision 1.5  2008-02-21 20:08:04  msalimi
	      - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
	      - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
	      - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

	      Revision 1.4  2008-02-06 16:29:56  msalimi
	      - Corrected low voltage SPN. This was 5 and now is 4.

	      Revision 1.3  2007-09-21 20:44:32  msalimi
	      Added J1939 data update with APU run minutes.

	      Revision 1.2  2007-08-01 20:02:48  nallyn
	      Modified commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.5  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
	
	      Revision 1.4  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.3  2006/02/08 20:15:33  nallyn
	      Updated based on code review.
	      Eliminated commented out and unused code.  More comments on how the routine works.
	
	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#ifdef _PIC16
	#include <pic.h>
#elif defined _PIC18
	#include <pic18.h>
#else
	//?#error Hourmeter module will not work with other than _PIC16 or _PIC18
#endif

#include "hrmeter2.h"
#include "DPF.h"
#include "Events.h"
#include "nvmem.h"
#include "i2c.h"				//? Add

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

// Hours Meter EEPROM data can be initialized in main.c
// depending on the CLEAR_HOURS compile flag.
//
//__EEPROM_DATA(0,0,0,0,0,0,0,0);	//initialize Hour Timer to 0
//__EEPROM_DATA(0,0,0,0,0,0,0,0);
//__EEPROM_DATA(0,0,0,0,0,0,0,0);
//__EEPROM_DATA(0,0,0,0,0,0,10,0);

#define HRMETER_NCFLAG_OFFSET 0x1E    //!< Offset to the "10" in the above table
#define HRMETER_INSTANCE_COUNT 10


// Macro Definitions
#define INC_MOD(x,y)	x=((x)+1)%(y)	//!<Circular index implemenation

/* ---------- I n t e r n a l   D a t a ---------- */

#ifndef MM32F0133C7P		//? Add
bank1 unsigned char hrAddrIdx=0;
// Global-to-module variables
bank1 unsigned long hrMtr_mins;
bank1 unsigned char newMinutes=0;
#else
unsigned char hrAddrIdx=0;
// Global-to-module variables
unsigned long hrMtr_mins;
unsigned char newMinutes=0;
#endif

/* ---------- G l o b a l   D a t a ---------- */

extern unsigned char TxPgn_65280[8];

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: HourMeter_RslvAddr -->
       \brief EEPROM address resolved based on address offset 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  addr    Offset of location in EEPROM to convert to address
<!-- RETURNS: -->
      \return Equivalent byte or unsigned char address of location in EEPROM specified by offset

*/
/******************************************************************************/
char HourMeter_RslvAddr(char addr)
{
	addr = ( addr * 3 ) + HRMETER_ADDR_BASE;
	return( addr );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_GetLong -->
       \brief Get a long value from EEPROM

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  addr    Address of long word value to retrieve from EEPROM - LSByte is in the higher EEPROM address
<!-- RETURNS: -->
      \return Long word value retrieve from EEPROM at specified location

*/
/******************************************************************************/
unsigned long EEPROM_GetLong( char addr )
{
	unsigned long tempMinutes;
    
	addr = HourMeter_RslvAddr( addr );

	tempMinutes = eeprom_read( addr );
	tempMinutes = ( tempMinutes << 8 ) + eeprom_read( addr + 1 );
	tempMinutes = ( tempMinutes << 8 ) + eeprom_read( addr + 2 );
	return( tempMinutes );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_PutLong -->
       \brief Write a long value to EEPROM - LSByte is in the higher EEPROM address

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  addr    Address where long value is to be stored in EEPROM
       @param[in]  value   Long word value to be stored at specified address in EEPROM
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void EEPROM_PutLong( char addr, unsigned long value )
{
	addr = HourMeter_RslvAddr( addr );
    
	eeprom_write( addr + 2, ( value & 0xFF ));
	eeprom_write( addr + 1, (( value >> 8 ) & 0xFF ));
	eeprom_write( addr,     (( value >> 16 ) & 0xFF ));
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: HourMeterPutHours -->
       \brief Write hour meter time to EEPROM

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  hours   Value to write
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void HourMeterPutHours( long hours )
{
	char i;
    
	// For all instances of the hour meter
	for( i = 0; i < HRMETER_INSTANCE_COUNT; i++ )
	{
        // Save hour meter in minutes
		EEPROM_PutLong( i, hours * 60L );
        hrMtr_mins = EEPROM_GetLong(i);
	}
    
    eeprom_write( HRMETER_NCFLAG_OFFSET, 0 );
	hrAddrIdx = 0;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: HourMeter_init -->
       \brief Initialize hour meter module

<!-- PURPOSE: -->
      Hour meter time is stored as a count of minutes of run time.
      An individual record of minutes occupies 3 bytes in EEPROM.
      To lessen over-use of the EEPROM cells 10 instances (HRMETER_INSTANCE_COUNT)
      of the run time are stored with only one cell being updated
      when the minute count changes. The record with the highest value is the
      the current value, and the next record entry is the next to be overwritten.<BR><BR>
      hrAddrIdx is used to index to the current value of the hour meter. <BR><BR>
      The hour meter record can be initialized at FLASH programming time if the
      code was compiled with the CLEAR_HOURS compile flag.  If the data is loaded
      at the time of FLASH programming the byte at HRMETER_NCFLAG_OFFSET is set
      to 10 instances (HRMETER_INSTANCE_COUNT).
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void HourMeter_init(void)
{
	char i,j;
	u8 a[32], addr;
	u8 pRxData;

#ifdef CLEAR_HOURS
#ifdef MM32F0133C7P		//? Add
	u8 EEPROM_DATA[] = {0,0,0,0,0,0,0,0,			//initilize Hour Timer to 0
											0,0,0,0,0,0,0,0,
											0,0,0,0,0,0,0,0,
											0,0,0,0,0,0,10,0};
#if 0
	for (i=0; i<32; i++)
	{
		eeprom_write(i, EEPROM_DATA[i]);
	}
#else
	for (i=0; i<8; i++)
	{
		eeprom_read(i);					//?? Add, dummy read for setting SCL pin high
	}
											
	EEPROM_WriteBuff_Sub2B(I2C1, EEPROM_BASEADDR, EEPROM_DATA, 32);			//? Add
#endif										
#endif
#endif

	for (i=0; i<8; i++)
	{
		eeprom_read(i);					//?? Add, dummy read for setting SCL high
	}
											
    // Start with hour meter address index of 0
	hrAddrIdx = 0;
    
    // If eeprom data initialized by FLASH loading
	if( eeprom_read( HRMETER_NCFLAG_OFFSET ) == HRMETER_INSTANCE_COUNT )
	{
		// For all instances of the hour meter
		for( i = 0; i < HRMETER_INSTANCE_COUNT; i++ )
		{
            // Set value to zero
			EEPROM_PutLong( i, 0L );
  		}
 	}
 	else
 	{
		// For all instances of the hour meter after the first
		for( i = 1; i < HRMETER_INSTANCE_COUNT; i++ )
		{
            // Look for record that has the highest value
			if( EEPROM_GetLong( i ) > EEPROM_GetLong( i - 1 ) )
            {
                // Set hrAddrIdx to instance index of highest value
				hrAddrIdx = i;
            }
		}
	
        // Preset minutes to highest possible value
		hrMtr_mins = 0xffffff;
        
		// For all instances of the hour meter
		for( i = 0; i < HRMETER_INSTANCE_COUNT; i++ )
		{
            // Look for record of lowest value
			if( EEPROM_GetLong( i ) < hrMtr_mins )
			{
                // Save lowest value for next compare
				hrMtr_mins = EEPROM_GetLong(i);
                
                // Save index of lowest value found
				j=i;
			}
		}
		
        // Check if difference of highest and lowest values is 9 or less.
		// It will be equal to 9 at wrap point unless reset occurs before
        // we have 10 entries and j'th is still 0 in which case it will be
        // less than 9.
		// A value greater than 9 indicates a spurious entry.
		if( ( EEPROM_GetLong( hrAddrIdx ) - EEPROM_GetLong( j ) ) <= 9 )
		{
            // Use the highest hour meter value
			hrMtr_mins = EEPROM_GetLong( hrAddrIdx );
		}
		else
		{
            // Spurious entry - Use the lowest hour meter value (fallback
            // in case there is a large value which is not contiguous)
			hrAddrIdx = j;
			hrMtr_mins = EEPROM_GetLong( hrAddrIdx );
		}
	}
}



/******************************************************************************/
/*!

<!-- FUNCTION NAME: HourMeter -->
       \brief Called in main to return the accumulated hours

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Current hours

*/
/******************************************************************************/
long HourMeter( void )
{
	unsigned char ncf, i;
    
	//ensure that the current value is 1 greater than previous and 10 less
	// than next
	if(newMinutes)
	{
		newMinutes = 0;

		// load "new controller flag"
		ncf = eeprom_read( HRMETER_NCFLAG_OFFSET );

		// if it isn't 0
		if( ncf != 0 )
		{
			//decrement the flag
			ncf--;
			//...save it...
			eeprom_write( HRMETER_NCFLAG_OFFSET, ncf );
            
			//...increment the minutes index...
			INC_MOD( hrAddrIdx, HRMETER_INSTANCE_COUNT );
            
			//...write the minutes to the index point...
			EEPROM_PutLong(hrAddrIdx,hrMtr_mins);
		}

		// if it is 0 and minutes = lastminutes + 1;
		else if(hrMtr_mins == (EEPROM_GetLong(hrAddrIdx)+1))
		{
			//...increment the minutes index...
			INC_MOD( hrAddrIdx, HRMETER_INSTANCE_COUNT );
            
			//...write the minutes to the index point...
			EEPROM_PutLong(hrAddrIdx,hrMtr_mins);
	 	}
		else
		{
    		// For all instances of the hour meter
			for( i = 0; i < HRMETER_INSTANCE_COUNT; i++ )
			{
				if( EEPROM_GetLong( i ) < hrMtr_mins )
				{
					hrMtr_mins=EEPROM_GetLong(i);
					hrAddrIdx=i;
    			}
   			}
            
            // Save hour meter at current index
   			EEPROM_PutLong( hrAddrIdx, hrMtr_mins );
  		}
 	}
    
    // Return hour meter value in hours
    return( hrMtr_mins / 60 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: HourMeter_interrupt -->
       \brief Increment hour meter - called from the timer interrupt service routine

<!-- PURPOSE: -->
       When the main timer ISR causes the minutes to rollover, then it is the appropriate 
       time to increment the accumulated hour meter count (in minutes).
<!-- PARAMETERS: -->
       @param[in]  increment TRUE when it is time to increment accumulated hour meter count (in minutes)
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void HourMeter_interrupt( unsigned char increment )
{
	if( increment )
	{
		hrMtr_mins++;
		newMinutes = 1;
        if ( DPF_Diag.bPresent )
        {
            DPF_Diag.APU_Minutes_Without_Regen ++;
            if (DPF_Diag.APU_Minutes_Without_Regen >= MAX_MINUTES_WITHOUT_REGEN)
                Add_Event (EV_APU_NO_REGEN_FOR_TOO_LONG);
            if (DPF_Diag.APU_Minutes_Without_Regen % 60 == 0)
            {
                DPF_Diag.APU_Hours_Without_Regen++;
                Add_Event (EV_UPDATE_APU_HOUR_WITHOUT_REGEN);
            }
        }
        // J1939 Formating for APU Status message
        TxPgn_65280[0] = (hrMtr_mins & 0x000000FF);
        TxPgn_65280[1] = ((hrMtr_mins >> 8) & 0x000000FF);
        TxPgn_65280[2] = ((hrMtr_mins >> 16) & 0x000000FF);
        // End of J1939 Formating for APU Status message
	}
}

