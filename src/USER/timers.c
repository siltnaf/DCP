/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file timers.c
       \brief Module provides functions to maintain an array of countdown timers

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/timers.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/07/25 15:24:17  blepojevic
	      Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection
	
	      Revision 1.2  2014/04/16 14:07:10  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.1  2009/07/31 17:09:54  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.24  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.23  2008-10-09 18:02:29  msalimi
	      Changed and cleaned messages.

	      Revision 1.22  2008-09-25 17:20:21  msalimi
	      -Added a new feature to prevent multiple regenerations when the exhaust pressure sensor fault is unstable. The fault will result in normal action when it happens for the first time but won't force regen for next fault triggers until the "first time" condition is removed. This condition will be removed if two normal soot collection/regen period are observed. The previous feature that forced regen every 8 hours if the fault exists is still in effect.

	      Revision 1.21  2008-08-21 15:43:12  msalimi
	      - Added feature to turn on the backlight when fault or important messages are being displayed.
	      - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
	      - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
	      - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

	      Revision 1.20  2008-07-28 17:13:21  msalimi
	      Implemented a two minutes delay in detecting exhaust pressure sensor fault to allow the APU warm up before forcing regeneration as a result of this fault.

	      Revision 1.19  2008-05-21 17:37:59  msalimi
	      - Renamed some of the DPF timers and their associated events to better reflect their usage.
	      - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

	      Revision 1.18  2008-04-24 15:37:43  msalimi
	      - Added new FMIs clearing timer functionality.

	      Revision 1.17  2008-04-02 19:50:11  msalimi
	      - Fixed onboard memory reading bug.
	      - There is no one hour recall of faults if APU is off.
	      - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
	      - DPF lost fault will not be detected if APU is off.

	      Revision 1.16  2008-03-25 19:14:38  msalimi
	      - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
	      - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
	      - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
	      - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
	      - APU cooldown period is reduced to 2 minutes.
	      - DPF related faults won't be captured when APU is off.
	      - Freed code space by removing redundant code.
	      - Faults are not displayed in DPF menu anymore.
	      - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

	      Revision 1.15  2008-01-31 20:54:29  msalimi
	      -Added feature to clear DPF missing fault when DPF is uninstalled.
	      -Made external and on-board memory applications independant of each other.

	      Revision 1.14  2008-01-09 21:45:44  msalimi
	      - Increased the low voltage detection delay at APU start up to 20 second. Also increased the number of consecutive low generator voltage indicator messages from 3 to 5.
	      - Added DPF regeneration countdown message to the bottom of the home screen.

	      Revision 1.13  2007-12-21 20:36:44  msalimi
	      - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
	      - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
	      - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
	      - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

	      Revision 1.12  2007-11-30 18:52:08  msalimi
	      - Made improvements suggested in code review.
	      - Menu timeout period extended to two minutes.
	      - Backlight on period extended to one minute.
	      - Moved CAN main loop operation to a new file (jmain).
	      - Added compile option switch to include or exclude internal versus extenral memories.
	      - Merged user regen and emergency regen menu items.
	      - Emergency regen asks the user to turn on the APU.
	      - Emergency flas is now set at menu and cleared when APU shuts down.
	      - Added a write enable of RTC chip each time the Time & Date menu is entered. Previously this was done only once at startup which was causing the RTC to be written only once (with the new chip). So the time could be updated only once after each reset.
	      - Display updates now will be done regularly to display all active messages based on priority.
	      - Changed the routine that performs a write on I2C to return true or false so that the calling routine can repeate the write requests.

	      Revision 1.11  2007-11-01 19:41:33  msalimi
	      -Changed the frequency of recording the CCU Status to external memory to every five minutes.
	      -Fixed the bug that was causing the DM1 messages to be cleared after the clearing time was up.
	      -Minor fixes in message display location on screen.

	      Revision 1.10  2007-10-26 18:42:14  msalimi
	      Added 5 minute cooldown for APU after DPF regeneration.
	      Changed DPF variable menu to show all variables in one screen.
	      Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
	      Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

	      Revision 1.9  2007-10-19 21:12:42  msalimi
	      New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

	      Revision 1.8  2007-10-16 20:40:22  msalimi
	      Version 35-DCP-004. External memory error correction.

	      Revision 1.7  2007-10-15 16:29:22  msalimi
	      Fixed memory error.

	      Revision 1.6  2007-10-09 15:03:18  msalimi
	      Version 35-PPP-002. Second round of testing..

	      Revision 1.5  2007-09-21 20:52:21  msalimi
	      *** empty log message ***

	      Revision 1.4  2007-09-17 18:41:36  msalimi
	      DPF related modifications

	      Revision 1.3  2007-08-27 22:19:08  msalimi
	      CAN communication working.

	      Revision 1.2  2007-08-01 20:20:10  nallyn
	      Added and modified commenting for Doxygen.

	      Revision 1.1  2007-07-05 20:10:13  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.14  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
	
	      Revision 1.13  2007/02/27 01:20:17  nallyn
	      Added 4 more timers for a total of 10 timers. The timers function differently
	      than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
	      timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
	      depending on the temperture selected:
	      18-20: High AC
	      21-23: Low AC
	      24: HVAC Off
	      25-27: Low Heat
	      28-30: High Heat
	
	      Revision 1.12  2007/02/26 17:49:52  nallyn
	      Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
	
	      Revision 1.11  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "timers.h"
#include "DPF.h"
#include "Events.h"
#include "ViewMan.h"
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "Datalog.h"
#include "main.h"
#include "apu.h"
#include "ice_pnl.h"
#include "tempmon.h"            // Interface to Temperature Monitor

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */


/* ---------- I n t e r n a l   D a t a ---------- */
//!The timers
struct timer timers;
//!Keypad timer tick
bit kpTick;
//!10 millisecond timer tick
bit msTick;
//!Second timer tick
bit sTick;
//!Minute timer tick
bit mTick;
//!Hour timer tick
bit hTick;
//!Variable represtenting 10 milliseconds, used for counting seconds
unsigned char milliseconds;
//!Variable representing seconds, used for counting hours on the hour meter
unsigned char seconds;
//!Variable representing seconds, used for counting hours
unsigned char minutes = 0;


/* ---------- G l o b a l   D a t a ---------- */


/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: timerTick -->
       \brief Routine to update list of timers - called from ISR during tick function

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void timerTick(void)
{
	if(msTick)
	{
		msTick = 0;

		if(timers.blinkTmr != 0)        // Menu blink timer and APU blink timer
		{
			--timers.blinkTmr;
  		}

		if(timers.menuScanTmr != 0)     // Scan and update the menu timer
		{
			--timers.menuScanTmr;
  		}

#ifndef EXTRA_TIMERS
		if(timers.blinkBmTmr != 0)
		{
			--timers.blinkBmTmr;
  		}
        
		if(timers.blinkTMTmr != 0)      // TempMon blink timer
		{
			--timers.blinkTMTmr;
  		}
#endif // EXTRA_TIMERS

		if(timers.blinkCSTmr != 0)
		{
			--timers.blinkCSTmr;
  		}

		if(timers.blinkMxTmr != 0)      // Max runtime icon blink timer
		{
			--timers.blinkMxTmr;
  		}

		if(timers.blinkT1Tmr != 0)      // Time 1 blink timer
		{
			--timers.blinkT1Tmr;
  		}

		if(timers.blinkT2Tmr != 0)      // Timer 2 blink timer
		{
			--timers.blinkT2Tmr;
  		}
#ifndef USE_COLDSTART
		if(timers.blinkT3Tmr != 0)      // Time 3 blink timer
		{
			--timers.blinkT3Tmr;
  		}

		if(timers.blinkT4Tmr != 0)      // Timer 4 blink timer
		{
			--timers.blinkT4Tmr;
  		}
#endif // USE_COLDSTART

#ifdef EXTRA_TIMERS
        if(timers.blinkT5Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT5Tmr;
        }
        
        if(timers.blinkT6Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT6Tmr;
        }
        
        if(timers.blinkT7Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT7Tmr;
        }
        
        if(timers.blinkT8Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT8Tmr;
        }
        
        if(timers.blinkT9Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT9Tmr;
        }
        
        if(timers.blinkT10Tmr != 0)      // Timer 4 blink timer
        {
            --timers.blinkT10Tmr;
        }
#endif // EXTRA_TIMERS
		if(timers.waitTmr != 0)
		{
			--timers.waitTmr;
  		}

		if(timers.b1Tmr != 0)           // Breaker 1 debounce timer
		{
			--timers.b1Tmr;
  		}

		if(timers.b2Tmr != 0)           // Breaker 2 debounce timer
		{
			--timers.b2Tmr;
  		}

		if(timers.menu2Tmr != 0)        // Right,Left button delay/repeat timer
		{
			--timers.menu2Tmr;
		}

		if(timers.menu3Tmr != 0)        // Right,Left button delay/repeat timer
		{
			--timers.menu3Tmr;
		}

		if(timers.adcTmr != 0)          // Read A to D delay timer
		{
			--timers.adcTmr;
		}
        
		if(timers.readRTCTmr != 0)        // Timed events for clock and timers
		{
			--timers.readRTCTmr;
  		}

        if(timers.OBM_Timer != 0)
		{
			--timers.OBM_Timer;
			if (timers.OBM_Timer == 0)
			{
				timers.OBM_Timer = 2;  // Memories must be processed no more frequently than 10mS
#ifdef OBM
                // Refresh the internal memory process
                OBM_fnProcess();
#endif // OBM

#ifdef EXT_MEM
                if ( Datalog.Ext_Mem_Present )
                    EXM_fnProcess();
#endif // EXT_MEM
			}
		}

		if(DPF_Timers.Wait_For_ACK != 0)
		{
			--DPF_Timers.Wait_For_ACK;
			if (DPF_Timers.Wait_For_ACK == 0)
			{
				Add_Event (EV_DPF_NO_ACK);
			}
		}
	}

	if(sTick)
	{
		sTick = 0;

		if(timers.menuTmr != 0)
		{
			--timers.menuTmr;
		}

		if(timers.bmTmr != 0)
		{
			--timers.bmTmr;
		}

		if(timers.cmTmr != 0)
		{
			--timers.cmTmr;
		}
		
		if(timers.cmhTmr != 0)
		{
			--timers.cmhTmr;
		}

		if(timers.bkLightTmr != 0)
		{
			--timers.bkLightTmr;
		}

		if(timers.faultTmr != 0)
		{
			--timers.faultTmr;
		}

		if(timers.glowingTmr != 0)
		{
			--timers.glowingTmr;
		}


		if(timers.apuOnTmr != 0)
		{
			--timers.apuOnTmr;
		}

		if(timers.apuAcTmr != 0)
		{
			--timers.apuAcTmr;
		}

		if(timers.a_cTmr != 0)
		{
			--timers.a_cTmr;
		}
        
		if(timers.restartTmr != 0)
		{
			--timers.restartTmr;
		}

		if ( DPF_Timers.Disconnection )
		{
			--DPF_Timers.Disconnection;
			if (DPF_Timers.Disconnection == 0)
			{
				Add_Event (EV_DPF_DISCONNECTED);
			}
		}

        // Check DM1s for dropped conditions
        DPF_DM1_Tick();
        
		if(DPF_Timers.MSG_Display)
		{
			--DPF_Timers.MSG_Display;
			if (DPF_Timers.MSG_Display == 0)
			{
				Add_Event (EV_DPF_MSG_DISPLAY);
			}
		}

		if(DPF_Timers.APU_Cooldown_Timer)
		{
			--DPF_Timers.APU_Cooldown_Timer;
			if (DPF_Timers.APU_Cooldown_Timer == 0)
			{
				Add_Event (EV_APU_COOL_NOW);
			}
		}
        
		if ( APU_Diag.APU_Warmup_Timer )
		{
    		--APU_Diag.APU_Warmup_Timer;
    		if ( APU_Diag.APU_Warmup_Timer == 0 )
			{
				Add_Event( EV_APU_WARMUP );
			}
		}

        VWM_fnRefresh();
#ifdef OBM_WRAPUP_TEST
        Record_Fault (SPN_EXHST_PRESS, FMI_PRESS_SNSR_FAIL, SET);
#endif // OBM_WRAPUP_TEST
	}

	if(mTick)
	{
		mTick = 0;

		if(timers.settleTmr)
		{
			--timers.settleTmr;
		}

		if (APU_ON_FLAG)
		{
			timers.APU_ONTmr++;
		}
		else
			timers.APU_ONTmr = 0;
        
		if(timers.minRunTmr)
		{
			--timers.minRunTmr;
		}
        
		if(timers.maxRunTmr)
		{
			--timers.maxRunTmr;
		}

        if(timers.tmRunTmr)
        {
            // If timer is expiring
            if ( timers.tmRunTmr == 1 )
            {
                // Deactivate Temperature Monitor
                tmgTMActive = 0;
            }
            --timers.tmRunTmr;
        }
        
		if(DPF_Timers.One_Hour)
		{
			--DPF_Timers.One_Hour;
			if (DPF_Timers.One_Hour == 0)
			{
				Add_Event (EV_DPF_ONE_HR);
			}
		}

#ifdef EXT_MEM
        if (timers.CCU_Record_Timer)
        {
            timers.CCU_Record_Timer--;
            if(timers.CCU_Record_Timer == 0)
            {
                timers.CCU_Record_Timer = 5;
                if ( APU_ON_FLAG )
                    Record_CCU_Status_External();
            }
        }
#endif // EXT_MEM
	}

	if (hTick)
	{
		hTick = 0;

		if ((DPF_Timers.Eight_Hour) && (APU_ON_FLAG))
		{
			--DPF_Timers.Eight_Hour;
			if (DPF_Timers.Eight_Hour == 0)
			{
				Add_Event (EV_DPF_8_HR);
			}
		}
	}
}

