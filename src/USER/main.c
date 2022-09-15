/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file main.c
       \brief Main entry point and control loop for DCP driver control panel application

<!-- DESCRIPTION: -->
<!-- NOTES: -->

       \if ShowHist
       Copyright � 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/main.c-arc   1.1   Jul 10 2018 14:45:08   ctctmas  $

          $Log:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/main.c-arc  $
 * 
 * RTC_80155 APU Master Password, March 5, 2020, Hannon
 *  Version 02.03.03
 *  
 * RTC_75512 APU Driver control panel - increase password length to 4 digits, November 13, 2019, Hannon
 *  Version 02.03.02
 * 
 *    Rev 1.2   Apr 26 2019 ctctmas
 * RTC_064664 APU minimum compressor off time 3.5 minutes
 *  Version 02.03.01
 * 
 *    Rev 1.1   Jul 10 2018 14:45:08   ctctmas
 * Version 02.03.00
 * 
 *    Rev 1.0.1.0   Jun 08 2018 17:34:30   ctctmas
 * PPR032836
 * Exp Version 81.02.01
 * 
 *    Rev 1.0   Jun 08 2018 17:10:22   ctctmas
 * Initial Carrier revision.
 
          Revision 1.1  2015/06/30 19:41:54  blepojevic
          This is former software version DCP3-019-02K imported to new folder APU\DieselAPU\DCP and will be tagged using new naming convention starting from TD-DCP-02.02.01

          Revision 1.9  2015/06/30 19:41:54  blepojevic
          Prototype version K software tested in the field, ready for production. It will be released as 02D production version.

          Revision 1.8  2014/08/25 19:31:35  blepojevic
          Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

          Revision 1.7  2014/08/11 19:07:43  blepojevic
          Changed debounce time for fault detection to 60 sec. Software made more robust to protect from storing dummy faults.

          Revision 1.6  2014/07/25 15:24:16  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.5  2014/06/27 17:35:27  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.4  2014/04/16 14:07:09  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.3  2013/09/19 13:07:44  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.2  2010/12/01 22:44:32  dharper
          Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
          This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
          This is because REV B was never checked in.
    
          Revision 1.1  2010/03/11 20:24:10  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
          Revision 1.58  2008-11-14 19:46:27  gfinlay
          Version 0.55

          Revision 1.57  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.56  2008-10-29 17:08:21  msalimi
          Adjusted the position of some messages.

          Revision 1.55  2008-10-28 15:54:28  msalimi
          - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
          - Removed the display of APU shutdown reason after ESC/RESET key push.
          - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

          Revision 1.54  2008-10-20 18:08:52  msalimi
          Improvements in displayed messages.

          Revision 1.53  2008-10-09 18:02:29  msalimi
          Changed and cleaned messages.

          Revision 1.52  2008-09-25 17:20:21  msalimi
          -Added a new feature to prevent multiple regenerations when the exhaust pressure sensor fault is unstable. The fault will result in normal action when it happens for the first time but won't force regen for next fault triggers until the "first time" condition is removed. This condition will be removed if two normal soot collection/regen period are observed. The previous feature that forced regen every 8 hours if the fault exists is still in effect.

          Revision 1.51  2008-09-17 16:08:18  msalimi
          - Changed the text displayed when the exhaust pressure sensor or the snubber fail.

          Revision 1.50  2008-09-04 21:12:00  msalimi
          Improved the display format of some messages.

          Revision 1.49  2008-08-21 15:43:11  msalimi
          - Added feature to turn on the backlight when fault or important messages are being displayed.
          - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
          - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
          - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

          Revision 1.48  2008-08-13 17:36:37  msalimi
          Added comments

          Revision 1.47  2008-08-12 20:40:07  msalimi
          Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

          Revision 1.46  2008-08-07 20:06:27  msalimi
          - Adjusted some displayed text on screen

          Revision 1.45  2008-07-28 17:13:21  msalimi
          Implemented a two minutes delay in detecting exhaust pressure sensor fault to allow the APU warm up before forcing regeneration as a result of this fault.

          Revision 1.44  2008-07-14 16:52:42  msalimi
          Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

          Revision 1.43  2008-06-19 16:15:58  msalimi
          *** empty log message ***

          Revision 1.42  2008-06-12 17:21:28  msalimi
          Fixed a bug related to emergency and normal user initiated regens.

          Revision 1.41  2008-06-03 17:34:27  msalimi
          - ded a message for heat adjustment operation to display "Regenerating" when in regen mode.
          - Cancelled restoring the loads when the sensor pressure and extreme pressure failures are cleared. The loads are now being restored only when regen is finished.

          Revision 1.40  2008-05-21 17:37:59  msalimi
          - Renamed some of the DPF timers and their associated events to better reflect their usage.
          - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

          Revision 1.39  2008-05-13 19:13:35  msalimi
          - Added new feature to treat transitions from regenning to any other state the same as transitioning to regen ok state.
          - Removed warning message related to compliing for COMFORTPRO as this is always the case.

          Revision 1.38  2008-05-06 18:47:35  msalimi
          Added hardware level filter to CAN operation to receive only messages with 29 bit identifier.

          Revision 1.37  2008-04-29 21:30:45  msalimi
          *** empty log message ***

          Revision 1.36  2008-04-10 16:45:17  msalimi
          - Made a distinction between normal and emergency regen.
          - Extreme pressure and pressure sensor faults will be detected during normal regen.
          - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
          - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
          - A new command will be sent to DPF when an emergency regen is initiated.
          - APU warmup reduced to 2 minutes (from 5).

          Revision 1.35  2008-04-02 19:50:11  msalimi
          - Fixed onboard memory reading bug.
          - There is no one hour recall of faults if APU is off.
          - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
          - DPF lost fault will not be detected if APU is off.

          Revision 1.34  2008-03-26 21:27:50  msalimi
          Fixed side effects of recent changes to how APU warmup period is handled for regeneration.

          Revision 1.33  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.32  2008-03-11 19:16:03  msalimi
          -Added a code inquirey mechanism to editing APU hours menu. This code is internally generated based on the time and date and must also be entered by the user in order to access the menu.

          Revision 1.31  2008-02-25 19:42:36  msalimi
          Fixed OBM reading issues.

          Revision 1.30  2008-02-21 20:08:04  msalimi
          - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
          - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
          - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

          Revision 1.29  2008-02-06 16:29:56  msalimi
          - Corrected low voltage SPN. This was 5 and now is 4.

          Revision 1.28  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.27  2008-01-09 21:46:17  msalimi
          *** empty log message ***

          Revision 1.26  2008-01-07 22:59:34  msalimi
          Fixed memory error problem. It was coming up when the memory board was not installed as the software was still trying to write to it.

          Revision 1.25  2007-12-21 20:36:44  msalimi
          - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
          - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
          - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
          - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

          Revision 1.24  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.23  2007-12-06 23:21:32  msalimi
          Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

          Revision 1.22  2007-12-06 21:40:10  msalimi
          - Changed the message that delays the APU shutdown to also show the coolddown period.
          - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
          - Eliminated the "Heater Status" from DPF menu.

          Revision 1.21  2007-11-30 18:52:08  msalimi
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

          Revision 1.20  2007-11-01 19:41:33  msalimi
          -Changed the frequency of recording the CCU Status to external memory to every five minutes.
          -Fixed the bug that was causing the DM1 messages to be cleared after the clearing time was up.
          -Minor fixes in message display location on screen.

          Revision 1.19  2007-10-26 21:11:21  msalimi
          Fixed triggering of APU going off when in regen soon.

          Revision 1.18  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.17  2007-10-24 22:24:02  msalimi
          Version update.

          Revision 1.16  2007-10-24 15:09:30  msalimi
          Removed left over test code. Compile options: UNIT_TEST & MEMORY_TESTING.

          Revision 1.15  2007-10-23 19:01:08  msalimi
          Updated version number.

          Revision 1.14  2007-10-19 21:12:42  msalimi
          New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

          Revision 1.13  2007-10-18 20:21:42  msalimi
          Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

          Revision 1.12  2007-10-17 18:34:44  msalimi
          Fixed external memory log problem.

          Revision 1.11  2007-10-16 20:40:22  msalimi
          Version 35-DCP-004. External memory error correction.

          Revision 1.10  2007-10-15 16:26:32  msalimi
          Excluding on board memory module to make room.
          Fixed memory error.
          Added event trigger when there is memory error.

          Revision 1.9  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.8  2007-09-21 20:45:29  msalimi
          Removed unused code to free space.

          Revision 1.7  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.6  2007-08-28 16:44:31  msalimi
          Added originator address checking for incoming J1939 ack message.

          Revision 1.5  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.4  2007-08-01 20:09:52  nallyn
          Added commenting for Doxygen

          Revision 1.3  2007-07-17 20:45:17  msalimi
          Timing fixes.

          Revision 1.2  2007-07-10 22:34:52  gfinlay
          Fixed timer tick configuration for proper values to get a 1ms tick with a 40MHz internal clock (fOsc=4x10MHz).

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.19  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
    
              Revision 1.18  2007/02/27 01:20:17  nallyn
              Added 4 more timers for a total of 10 timers. The timers function differently
              than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
              timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
              depending on the temperture selected:
              18-20: High AC
              21-23: Low AC
              24: HVAC Off
              25-27: Low Heat
              28-30: High Heat

              Revision 1.17  2007/02/26 17:49:52  nallyn
              Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

              Revision 1.16  2007/02/23 18:11:28  nallyn
              Changed from 2 timers to 4 timers for testing purposes

              Revision 1.15  2006/04/05 23:11:39  nallyn
              Carrier ComfortPro rebranding

          Revision 1.14  2006/02/09 22:09:12  nallyn
          Updated the default values for clock and voltage calibration access
    
          Revision 1.13  2006/02/08 20:19:04  nallyn
          Changes based on code review
    
          Revision 1.12  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
//?#include <pic18.h>       // common include file

/* configuration word:
    WDTEN - enable watch dog timer - only for production
    PWRTEN - power up timer enable
    HS - high speed oscillator
    UNPROTECT - no code protect
    LVPDIS - disable low voltage programming */

//?#include "hwconfig.h"

#ifdef CLEAR_HOURS
#ifndef MM32F0133C7P		//? Add
__EEPROM_DATA(0,0,0,0,0,0,0,0);   //initilize Hour Timer to 0
__EEPROM_DATA(0,0,0,0,0,0,0,0);
__EEPROM_DATA(0,0,0,0,0,0,0,0);
__EEPROM_DATA(0,0,0,0,0,0,10,0);
#endif
#else
#warning Hours will not be cleared - main.c
#endif

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "defaults.h"   // Default settings for individual customers
#include "lcd_grp1.h" // Graphic lcd functions
#include "adc18x.h" // analog to digital functions
#include "pwm18x.h" // pulse width modulation functions
#include "fonts.h" // font-set display functions
#include "delay18.h"  // delay functions (ms and us)
#include "timers.h"   // timers
#include "keypad-timers.h"
#include <stdio.h>
#include "main.h"
#include "rtclock.h"            // Interface to real-time clock
#include "menu.h"               // Menu definitions
#include "apu.h"                // Interface to APU State Machine
#include "keypad.h"             // Keypad functions
#include "ice_pnl.h"
#include "battmon.h"            // Interface to Battery Voltage Monitor
#include "coldstart.h"
#include "tempmon.h"            // Interface to Temperature Monitor
#include "timekeep.h"           // Interface for clock time keeping
#include "iconblinker.h"
#include "hrmeter2.h"
#include "backlit.h"            // Backlight state machine
#include "nvmem.h"              // Interface to Non-Volatile Memory storage
#include "syncserial.h"         // Interface to Syncronous Serial Port 
#include "can.h"
#include "j1939.h"
#include "Events.h"
#include "DPF.h"
#include "ViewMan.h"            // Interface to using View Manager
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "Datalog.h"
#include "jmain.h"
#include "serial.h"
#include "uart.h"				//? Add
#include "tim14.h"			//? Add
#include "iwdg.h"				//? Add
#include "TestMode.h"		//? Add

#ifdef MM32F0133C7P			//? Add
#define TestModePin		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)
#endif

DCP_DIAG DCP_Diag;

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

//!Software version displayed when menu and esc keys are pressed simultaneously
//const char version[] = "TD-DCP-02.03.03";
const char version[] = "TD-DCP-10.00.02";
 
/* ---------- I n t e r n a l   D a t a ---------- */
static unsigned char Timer10ms = 0;
//static u8 xtest = 1;

/* ---------- G l o b a l   D a t a ---------- */
uint8 SYS_u8SysCfgAccess;

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: int_isr -->
       \brief Main Interrupt Service Routine (ISR) for timer interrupt

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/

#ifndef MM32F0133C7P		//? Add
/******************************************************************************/
void interrupt int_isr(void)
{
    if(TMR1IF)
    {
       // Real Timer Interrupt
       TMR1IF = 0; //clear timer0 interupt flag
       TMR1L=0x3C; //reset timer0 to intial value
       TMR1H=0xF6; // with prescale set to 1:4 and normal timer increment
       // ever 4 clock cycles (once per instruction cycle)
       // gives an overall clock tick of 1:16
       // with a desired timeout every 1ms on a 16 bit timer
       // the reset value is:
       //    2^16-(0.001/(1/20mhz*16)) = 64286
       //              = 0xFB1E
       //    2^16-(0.001/(1/40mhz*16)) = 63036
       //              = 0xF63C

       if(Timer10ms)
          Timer10ms--;
       else
       {
            // Sub Time Base of 10 mS
            Timer10ms = 9;
            msTick = 1;
            kpTick = 1;
            milliseconds++;
       }
       
       if(milliseconds == 100)
       {
          // Sub Time Base of 1 S
          milliseconds = 0;
          sTick = 1;
          seconds++;
       }

       if(seconds == 60)
       {
          // Sub Time Base of 60 S
          seconds = 0;
          mTick = 1;
            HourMeter_interrupt(APU_ON_FLAG);
          cool_timer++;
            minutes++;
       }
        if (minutes >= 60)
        {
            minutes = 0;
            hTick = 1;
       }

       j1939_TicksTimerTask();   // Needs to be called each 1mS
    }

    // If there is a synchronous serial interrupt pending
    if ( SSPIF )
    {
        // Call handler
        ssfIsr();
    }

    // There is a bug in the PIC18F6680 hardware.  Reading CANSTAT register
    // does not indicate any receive interrupts.  This problem is related
    // to the fact that the !FIFOEMPTY flag causes an error interrupt and
    // and cannot be cleared until the FIFO is empty, that is, the receive
    // message has been read.
    // Here, check the receive buffer interrupt flag and check if the
    // receive interrupt is enabled. The receive buffer flag is set if
    // a receive buffer is not empty even if the interrupt is not enabled.
    if ( RXBNIF && RXBNIE )
    {
       // Call Interrupt Handler
       CANIT_handler(); 
    }
}
#else		///?OK
////////////////////////////////////////////////////////////////////////////////
/// @brief  Waiting for the timer to overflow and interrupt
/// @note   Pay attention to clearing interruption marks.
/// @param  TxMessage:Sent message.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void TIM14_IRQHandler(void)
{
			 TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
			 TIM_ClearFlag(TIM14, TIM_FLAG_Update);
	
			 //TP2_TOGGLE();
			 //TP2_HI();

			 if(Timer10ms)
          Timer10ms--;
       else
       {
          // Sub Time Base of 10 mS
          Timer10ms = 9;
          msTick = 1;
          kpTick = 1;
					milliseconds++;
       }
       
       if(milliseconds == 100)
       {
          // Sub Time Base of 1 S
          milliseconds = 0;
          sTick = 1;
          seconds++;
       }

       if(seconds == 60)
       {
          // Sub Time Base of 60 S
          seconds = 0;
          mTick = 1;
          HourMeter_interrupt(APU_ON_FLAG);
          cool_timer++;
          minutes++;
       }

       if (minutes >= 60)
       {
          minutes = 0;
          hTick = 1;
       }

       j1939_TicksTimerTask();   // Needs to be called each 1mS
			 //TP2_LO();
}

#endif

/******************************************************************************/
/*!

<!-- FUNCTION NAME: set_defaults -->
       \brief Sets the default values for global configuration variables

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void set_defaults(void)
{
#ifndef EXTRA_TIMERS
    volt_monitor = nvfGetNumber(NV_ID_BM_ENABLE, DEFAULT_BATT_MON_ENABLE);
    if ( ( volt_monitor != ON ) && ( volt_monitor != OFF ) )
        volt_monitor = DEFAULT_BATT_MON_ENABLE;
        
    BMON_VOLTAGE = nvfGetVoltage(NV_ID_BM_VOLTAGE, DEFAULT_BATT_MON_VOLTS);
    bmgVOffset = (signed char) nvfGetNumber( NV_ID_BM_V_OFFSET, 8 );
#endif // EXTRA_TIMERS

#ifdef USE_COLDSTART
    bColdMonitor = nvfGetNumber(NV_ID_CS_ENABLE, DEFAULT_COLD_START_ENABLE);
    if ( ( bColdMonitor != ON ) && ( bColdMonitor != OFF ) )
        bColdMonitor = DEFAULT_COLD_START_ENABLE;

    tCOLD_START.cStartTemp = nvfGetTemperature(NV_ID_CS_TEMP, DEFAULT_COLD_START_TEMP);
    tCOLD_START.uiStartTempVoltage = tmfCtoVad(tCOLD_START.cStartTemp);
    tCOLD_START.uiColdRunTime = nvfGetNumber(NV_ID_CS_TIME, DEFAULT_COLD_START_TIME);
#endif

    tkgMaxRunEnable = nvfGetNumber(NV_ID_MR_ENABLE, DEFAULT_MAX_RUN_ENABLE);
    if ( ( tkgMaxRunEnable != ON ) && ( tkgMaxRunEnable != OFF ) )
        tkgMaxRunEnable = DEFAULT_MAX_RUN_ENABLE;
    tkgMaxRunTime = nvfGetNumber(NV_ID_MR_TIME, DEFAULT_MAX_RUN_TIME);
        
#ifndef EXTRA_TIMERS
    tmgTMEnable = nvfGetNumber(NV_ID_TM_ENABLE, DEFAULT_TEMP_MON_ENABLE);
    if ( ( tmgTMEnable != ON ) && ( tmgTMEnable != OFF ) )
        tmgTMEnable = DEFAULT_TEMP_MON_ENABLE;
    tmgTMTemp = nvfGetTemperature(NV_ID_TM_TEMP, DEFAULT_TEMP_MON_TEMP);
    tmgTMRange = nvfGetTemperature(NV_ID_TEMP_RANGE, DEFAULT_TEMP_MON_RANGE);
    tmgFSRange = nvfGetNumber(NV_ID_FAN_SPEED_RANGE, DEFAULT_FAN_SPEED_RANGE);
    tmgTMDuration = nvfGetNumber(NV_ID_TM_DURATION, DEFAULT_TEMP_MON_DURATION);
    tmfSetMode();
#endif // EXTRA_TIMERS
    
    tkgTimer1.tkxEnable = nvfGetNumber(NV_ID_TIMER1_ENABLE, DEFAULT_TIMER1_ENABLE);
    tkgTimer2.tkxEnable = nvfGetNumber(NV_ID_TIMER2_ENABLE, DEFAULT_TIMER2_ENABLE);
#ifndef USE_COLDSTART
    tkgTimer3.tkxEnable = nvfGetNumber(NV_ID_TIMER3_ENABLE, DEFAULT_TIMER3_ENABLE);
    tkgTimer4.tkxEnable = nvfGetNumber(NV_ID_TIMER4_ENABLE, DEFAULT_TIMER4_ENABLE);
#endif // USE_COLDSTART
    tkgTimer1.tkxDuration = nvfGetNumber(NV_ID_TIMER1_DUR, DEFAULT_TIMER1_DURATION);
    tkgTimer2.tkxDuration = nvfGetNumber(NV_ID_TIMER2_DUR, DEFAULT_TIMER2_DURATION);
#ifndef USE_COLDSTART
    tkgTimer3.tkxDuration = nvfGetNumber(NV_ID_TIMER3_DUR, DEFAULT_TIMER3_DURATION);
    tkgTimer4.tkxDuration = nvfGetNumber(NV_ID_TIMER4_DUR, DEFAULT_TIMER4_DURATION);
#endif // USE_COLDSTART
    tkgTimer1.tkxTempSet = nvfGetTemperature(NV_ID_TIMER1_TEMP, DEFAULT_TIMER1_TEMP);
    tkgTimer2.tkxTempSet = nvfGetTemperature(NV_ID_TIMER2_TEMP, DEFAULT_TIMER2_TEMP);
#ifndef USE_COLDSTART
    tkgTimer3.tkxTempSet = nvfGetTemperature(NV_ID_TIMER3_TEMP, DEFAULT_TIMER3_TEMP);
    tkgTimer4.tkxTempSet = nvfGetTemperature(NV_ID_TIMER4_TEMP, DEFAULT_TIMER4_TEMP);
#endif // USE_COLDSTART
    nvfGetTime( NV_ID_TIMER1_TIME, &tkgTimer1.tkxTime, DEFAULT_TIMER1_DAY, DEFAULT_TIMER1_HOUR, DEFAULT_TIMER1_MINUTE );
    nvfGetTime( NV_ID_TIMER2_TIME, &tkgTimer2.tkxTime, DEFAULT_TIMER2_DAY, DEFAULT_TIMER2_HOUR, DEFAULT_TIMER2_MINUTE );
#ifndef USE_COLDSTART
    nvfGetTime( NV_ID_TIMER3_TIME, &tkgTimer3.tkxTime, DEFAULT_TIMER3_DAY, DEFAULT_TIMER3_HOUR, DEFAULT_TIMER3_MINUTE );
    nvfGetTime( NV_ID_TIMER4_TIME, &tkgTimer4.tkxTime, DEFAULT_TIMER4_DAY, DEFAULT_TIMER4_HOUR, DEFAULT_TIMER4_MINUTE );
#endif // USE_COLDSTART
    tkgTimer1.tkxRun = FALSE;
    tkgTimer2.tkxRun = FALSE;
#ifndef USE_COLDSTART
    tkgTimer3.tkxRun = FALSE;
    tkgTimer4.tkxRun = FALSE;
#endif // USE_COLDSTART
    tkgTimer1.tkxIcon = ICON_TIMER1;
    tkgTimer2.tkxIcon = ICON_TIMER2;
#ifndef USE_COLDSTART
    tkgTimer3.tkxIcon = ICON_TIMER3;
    tkgTimer4.tkxIcon = ICON_TIMER4;
#endif // USE_COLDSTART
    tkgTimer1.tkxPriority = APU_ON_BY_TIMER1;
    tkgTimer2.tkxPriority = APU_ON_BY_TIMER2;
#ifndef USE_COLDSTART
    tkgTimer3.tkxPriority = APU_ON_BY_TIMER3;
    tkgTimer4.tkxPriority = APU_ON_BY_TIMER4;
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    tkgTimer5.tkxEnable = nvfGetNumber(NV_ID_TIMER5_ENABLE, DEFAULT_TIMER5_ENABLE);
    tkgTimer6.tkxEnable = nvfGetNumber(NV_ID_TIMER6_ENABLE, DEFAULT_TIMER6_ENABLE);
    tkgTimer5.tkxDuration = nvfGetNumber(NV_ID_TIMER5_DUR, DEFAULT_TIMER5_DURATION);
    tkgTimer6.tkxDuration = nvfGetNumber(NV_ID_TIMER6_DUR, DEFAULT_TIMER6_DURATION);
    tkgTimer5.tkxTempSet = nvfGetTemperature(NV_ID_TIMER5_TEMP, DEFAULT_TIMER5_TEMP);
    tkgTimer6.tkxTempSet = nvfGetTemperature(NV_ID_TIMER6_TEMP, DEFAULT_TIMER6_TEMP);
    nvfGetTime( NV_ID_TIMER5_TIME, &tkgTimer5.tkxTime, DEFAULT_TIMER5_DAY, DEFAULT_TIMER5_HOUR, DEFAULT_TIMER5_MINUTE );
    nvfGetTime( NV_ID_TIMER6_TIME, &tkgTimer6.tkxTime, DEFAULT_TIMER6_DAY, DEFAULT_TIMER6_HOUR, DEFAULT_TIMER6_MINUTE );
    tkgTimer5.tkxRun = FALSE;
    tkgTimer6.tkxRun = FALSE;
    tkgTimer5.tkxIcon = ICON_TIMER5;
    tkgTimer6.tkxIcon = ICON_TIMER6;
    tkgTimer5.tkxPriority = APU_ON_BY_TIMER5;
    tkgTimer6.tkxPriority = APU_ON_BY_TIMER6;

    tkgTimer7.tkxEnable = nvfGetNumber(NV_ID_TIMER7_ENABLE, DEFAULT_TIMER7_ENABLE);
    tkgTimer7.tkxDuration = nvfGetNumber(NV_ID_TIMER7_DUR, DEFAULT_TIMER7_DURATION);
    tkgTimer7.tkxTempSet = nvfGetTemperature(NV_ID_TIMER7_TEMP, DEFAULT_TIMER7_TEMP);
    nvfGetTime( NV_ID_TIMER7_TIME, &tkgTimer7.tkxTime, DEFAULT_TIMER7_DAY, DEFAULT_TIMER7_HOUR, DEFAULT_TIMER7_MINUTE );
    tkgTimer7.tkxPriority = APU_ON_BY_TIMER7;
    tkgTimer7.tkxRun = FALSE;
    tkgTimer7.tkxIcon = ICON_TIMER7;
        
    tkgTimer8.tkxEnable = nvfGetNumber(NV_ID_TIMER8_ENABLE, DEFAULT_TIMER8_ENABLE);
    tkgTimer8.tkxDuration = nvfGetNumber(NV_ID_TIMER8_DUR, DEFAULT_TIMER8_DURATION);
    tkgTimer8.tkxTempSet = nvfGetTemperature(NV_ID_TIMER8_TEMP, DEFAULT_TIMER8_TEMP);
    nvfGetTime( NV_ID_TIMER8_TIME, &tkgTimer8.tkxTime, DEFAULT_TIMER8_DAY, DEFAULT_TIMER8_HOUR, DEFAULT_TIMER8_MINUTE );
    tkgTimer8.tkxPriority = APU_ON_BY_TIMER8;
    tkgTimer8.tkxRun = FALSE;
    tkgTimer8.tkxIcon = ICON_TIMER8;

    tkgTimer9.tkxEnable = nvfGetNumber(NV_ID_TIMER9_ENABLE, DEFAULT_TIMER9_ENABLE);
    tkgTimer9.tkxDuration = nvfGetNumber(NV_ID_TIMER9_DUR, DEFAULT_TIMER9_DURATION);
    tkgTimer9.tkxTempSet = nvfGetTemperature(NV_ID_TIMER9_TEMP, DEFAULT_TIMER9_TEMP);
    nvfGetTime( NV_ID_TIMER9_TIME, &tkgTimer9.tkxTime, DEFAULT_TIMER9_DAY, DEFAULT_TIMER9_HOUR, DEFAULT_TIMER9_MINUTE );
    tkgTimer9.tkxPriority = APU_ON_BY_TIMER9;
    tkgTimer9.tkxRun = FALSE;
    tkgTimer9.tkxIcon = ICON_TIMER9;
    
    tkgTimer10.tkxEnable = nvfGetNumber(NV_ID_TIMER10_ENABLE, DEFAULT_TIMER10_ENABLE);
    tkgTimer10.tkxDuration = nvfGetNumber(NV_ID_TIMER10_DUR, DEFAULT_TIMER10_DURATION);
    tkgTimer10.tkxTempSet = nvfGetTemperature(NV_ID_TIMER10_TEMP, DEFAULT_TIMER10_TEMP);
    nvfGetTime( NV_ID_TIMER10_TIME, &tkgTimer10.tkxTime, DEFAULT_TIMER10_DAY, DEFAULT_TIMER10_HOUR, DEFAULT_TIMER10_MINUTE );
    tkgTimer10.tkxPriority = APU_ON_BY_TIMER10;
    tkgTimer10.tkxRun = FALSE;
    tkgTimer10.tkxIcon = ICON_TIMER10;
#endif // EXTRA_TIMERS
    
    // User access to menu
#ifndef EXTRA_TIMERS
    bmgBMAccess = nvfGetNumber(NV_ID_BM_ACCESS, DEFAULT_BATT_MON_ACCESS);
    tmgTMAccess = nvfGetNumber(NV_ID_TM_ACCESS, DEFAULT_TEMP_MON_ACCESS);
    bmgVoltAccess =         nvfGetNumber( NV_ID_VOLT_ACCESS, DEFAULT_VOLT_CAL_ACCESS );
#endif // EXTRA_TIMERS

    SYS_u8SysCfgAccess = nvfGetNumber( NV_ID_SYS_CFG_ACCESS, DEFAULT_SYS_CFG_ACCESS );
    DPF_u8DpfAccess = nvfGetNumber( NV_ID_DPF_ACCESS, DEFAULT_DPF_ACCESS );

#ifdef USE_COLDSTART
    tCOLD_START.ucAccess = nvfGetNumber(NV_ID_CS_ACCESS, DEFAULT_COLD_START_ACCESS);
#endif
    tkgMaxRunAccess = nvfGetNumber(NV_ID_MR_ACCESS, DEFAULT_MAX_RUN_ACCESS);
    tkgTimer1.tkxAccess = nvfGetNumber(NV_ID_TIMER1_ACCESS, DEFAULT_TIMER1_ACCESS);
    tkgTimer2.tkxAccess = nvfGetNumber(NV_ID_TIMER2_ACCESS, DEFAULT_TIMER2_ACCESS);
#ifndef USE_COLDSTART
    tkgTimer3.tkxAccess = nvfGetNumber(NV_ID_TIMER3_ACCESS, DEFAULT_TIMER3_ACCESS);
    tkgTimer4.tkxAccess = nvfGetNumber(NV_ID_TIMER4_ACCESS, DEFAULT_TIMER4_ACCESS);
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    tkgTimer5.tkxAccess = nvfGetNumber(NV_ID_TIMER5_ACCESS, DEFAULT_TIMER5_ACCESS);
    tkgTimer6.tkxAccess = nvfGetNumber(NV_ID_TIMER6_ACCESS, DEFAULT_TIMER6_ACCESS);
    tkgTimer7.tkxAccess = nvfGetNumber(NV_ID_TIMER7_ACCESS, DEFAULT_TIMER7_ACCESS);
    tkgTimer8.tkxAccess = nvfGetNumber(NV_ID_TIMER8_ACCESS, DEFAULT_TIMER8_ACCESS);
    tkgTimer9.tkxAccess = nvfGetNumber(NV_ID_TIMER9_ACCESS, DEFAULT_TIMER9_ACCESS);
    tkgTimer10.tkxAccess = nvfGetNumber(NV_ID_TIMER10_ACCESS, DEFAULT_TIMER10_ACCESS);
#endif // EXTRA_TIMERS
    tkgClockAccess =        nvfGetNumber( NV_ID_CLOCK_ACCESS, DEFAULT_CLOCK_ACCESS );
    
    mugClockMode24 = nvfGetNumber( NV_ID_CLOCK_MODE, DEFAULT_24HR_CLOCK );
    user_password = nvfGetNumber( NV_ID_PASS_BYTE2, DEFAULT_PASSWORD >> 8 );
    user_password = ( user_password << 8 ) + nvfGetNumber( NV_ID_PASS_BYTE1,
                                                           DEFAULT_PASSWORD );
    mugSetup = nvfGetNumber( NV_ID_SETUP, OFF );
		if(mugSetup != 1) mugSetup = 0;

    APU_Start_Total = nvfGetNumber(NV_ID_APU_START_TOTAL, APU_Start_Total);
    DPF_Diag.APU_Hours_Without_Regen = nvfGetNumber(NV_ID_APU_NO_REGEN_HOUR, DPF_Diag.APU_Hours_Without_Regen);

    HEAT_ENABLE = nvfGetNumber( NV_ID_HEAT_ENABLE, DEFAULT_HEAT_ENABLE);
}

//!Temperature filtering - average of last 30 readings;
#define TEMP_AVE   30
//!Voltage filtering - average of last 20 readings;
#define VOLT_AVE   20
//!Array of latest voltage readings
int volt_hist[VOLT_AVE];
//!Temporary battery voltage
int temp_batv;
//!Index in the voltage array of the current reading 
char volt_index = 0;
#ifndef MM32F0133C7P		//? Add
//!Array of latest temperature readings
bank1 int temp_hist[TEMP_AVE];
//!Index in the temperature array of the current reading
bank1 char temp_index = 0;
//!Index for the current ADC channel being read
bank1 char adc_index;
#else
//!Array of latest temperature readings
int temp_hist[TEMP_AVE];
//!Index in the temperature array of the current reading
char temp_index = 0;
//!Index for the current ADC channel being read
char adc_index;
#endif

/******************************************************************************/
/*!

<!-- FUNCTION NAME: getADvalues -->
       \brief Reads the next ADC channel in sequence

<!-- PURPOSE: -->
       This function will reads the next ADC channel in round-robin fashion. The
       sequence is:

       \code
          Index   Channel Value
          --------------------------------------------------------
          0       0       AC_BR1_BR2_SERV
          1       1       OIL_OH_WAIT
          2       3       TEMPERATURE (average of 30 samples)
          3       2       BATTERY_VOLTAGE (average of 20 samples)
          0       0       AC_BR1_BR2_SERV
          .
          .  Advancing index once for each time through main loop. 
          .  Index wraps around back to 0 at end of sequence
          .
       \endcode

<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void getADvalues(void)
{
    char x;
    switch(adc_index)
    {
       case 0:
          if(!timers.adcTmr)
          {
						 //printf("CH0-");
             timers.adcTmr = 1;//adcSTimer(1);
             AC_BR1_BR2_SERV = Read_ADC();
             SetADCChannel(1); //setup for next read - OIL_OH_WAIT
             adc_index = 1;
          }
          break;
       case 1:
          if(!timers.adcTmr)
          {
						 //printf("CH1-");
             timers.adcTmr = 1;//adcSTimer(1);
             OIL_OH_WAIT = Read_ADC();
             SetADCChannel(3); //setup for next read - TEMPERATURE
             adc_index = 2;
          }
          break;
       case 2:
          if(!timers.adcTmr)
          {
						 //printf("CH2-");
             timers.adcTmr = 1;//adcSTimer(1);
             temp_hist[temp_index] = Read_ADC();
             temp_index = (temp_index + 1)%TEMP_AVE;
             for(x=0;x<TEMP_AVE;x++)
             {
                TEMPERATURE+=temp_hist[x];
             }
             // Average includes previous value plus all stored values
             TEMPERATURE /= (TEMP_AVE+1);
             SetADCChannel(2); //setup for next read - BATTERY_VOLTAGE
             adc_index = 3;
          }
          break;
       case 3:
          if(!timers.adcTmr)
          {
						 //printf("CH3-");
             timers.adcTmr = 1;//adcSTimer(1);
             volt_hist[volt_index] = Read_ADC();
             volt_index = (volt_index + 1)%VOLT_AVE;
             for(x=0;x<VOLT_AVE;x++)
             {
                BATTERY_VOLTAGE += volt_hist[x];
             }
             // Average includes previous value plus all stored values
             BATTERY_VOLTAGE /= (VOLT_AVE+1);
             SetADCChannel(0);
             adc_index = 0;
          }
          break;
       default:
				  //printf("CH0-");
          SetADCChannel(0); // the routine will initialize to this state
          adc_index = 0;    // by setting adc_index = 4 in initialization code
          break;
    }
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: getPWMvalues -->
       \brief Maps AC, Heat, Fan and Outlet signals to encoded duty cycle values
       for PWM outputs RC1 and RC2

<!-- PURPOSE: -->
      RC1 (HEAT_AC) Control <BR>
         Output RC1 is a PWM-encoded output from the DCP
         controller to the CCU controller called HEAT_AC. The PWM duty cycle is
         used to multiplex the signals to the CCU controller. If/else logic is
         used to map the HEAT_CONT heater control and AC_CONT air-conditioning
         control boolean signals to the corresponding output duty cycle for RC1
         according to the table shown below.
      \code
            HEAT_CONT   AC_CONT    HEAT_AC   RC1 Output
                                   Duty %    Filtered V
            --------------------------------------------
            OFF         OFF        0         0.0
            OFF         ON        40         2.0
            ON          OFF       80         4.0
            ON          ON         0         0.0
      \endcode
      RC2 (FAN_LOW_HIGH) Control <BR>
         Output RC2 is a PWM-encoded output from the DCP controller to the CCU
         controller called FAN_LOW_HIGH. The PWM duty cycle is used to multiplex
         control signals to the CCU controller. If/else logic is used to map the
         FAN_LOW, FAN_HIGH, and OUTLET boolean signals to the corresponding
         output duty cycle for RC1 according to the table below.
      \code
            FAN_LOW    FAN_HIGH    OUTLET    FAN_LOW_HIGH RC2 Output
                                             Duty %       Filtered V
            ----------------------------------------------------------
            OFF        OFF         OFF         0          0.0
            OFF        OFF         ON         20          1.0
            OFF        ON          OFF        80          4.0
            OFF        ON          ON        100          5.0
            ON         OFF         OFF        40          2.0
            ON         OFF         ON         60          3.0
            ON         ON          OFF        40          2.0
            ON         ON          ON         60          3.0
      \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void getPWMvalues(void)
{
    if (!bCmRunHighHeat)             // only when cold monitor do not disable heating
       HEAT_CONT &= HEAT_ENABLE;

    if(!HEAT_CONT && !AC_CONT)
    {
       heat_ac = 0;
    }
    else if(!HEAT_CONT && AC_CONT)
    {
       heat_ac = 40;
    }
    else if(HEAT_CONT && !AC_CONT)
    {
       heat_ac = 80;
    }
    else
    {
       heat_ac = 0;
    }

    if(!FAN_LOW && !FAN_HIGH && !OUTLET)
    {
       fan_low_high = 0;
    }
    else if(!FAN_LOW && !FAN_HIGH && OUTLET)
    {
       fan_low_high = 20;
    }
    else if(FAN_LOW && !OUTLET)
    {
       fan_low_high = 40;
    }
    else if(FAN_LOW && OUTLET)
    {
       fan_low_high = 60;
    }
    else if(FAN_HIGH && !OUTLET)
    {
       fan_low_high = 80;
    }
    else if(FAN_HIGH && OUTLET)
    {
       fan_low_high = 100;
    }
    else
    {
       fan_low_high = 0;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: enableTimerOne -->
       \brief Enables interrupts for PIC timer channel 1 (TMR1)

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void enableTimerOne(void)
{
#ifndef MM32F0133C7P		//? Add
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
#else		///?OK
		//NOP
#endif
}

//!State variable for testCold
enum getCold c_state = unPressed;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: testCold -->
       \brief Manages the on/off temperature control for the coldstart state

<!-- PURPOSE: -->
       This function manages the hysteresis deadband to control the coldstart state (c_state).
       The turn-on (pressed) state is entered when the temperature goes below -5 degC.
       The turn-off (unPressed) state is entered when the temperature rises above 0 degC.
       In between 0C and -5C is the hysteresis deadband - the c_state variable will 
       neither be asserted nor cleared in this range.

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void testCold( void )
{
  switch(c_state)
  {
    case unPressed:
      COLD = 0;
        if(( TEMPERATURE >= 834 ) && ( TEMPERATURE <= 996 )) // -5 C - FIXME - magic number
      {
        c_state = pressed;
      }
      break;
    case pressing:
      break;
    case pressed:
      COLD = 1;
      //iconShow( ICON_COLDSTART );
        if(( TEMPERATURE < 788 ) || ( TEMPERATURE > 996 ))   // 0 C - FIXME - magic number
      {
        //iconErase( ICON_COLDSTART );
        c_state = unPressed;
      }
      break;
    case unPressing:
      break;
  }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: main -->
       \brief Main entry point for DCP driver control panel application

<!-- PURPOSE: -->
       This is the main entry point and main loop for the DCP application.

<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void main(void)
{
    unsigned char Current_Event;
    u8 test_mode_flag;
    u8 test_mode_count_down;

#ifdef DEBUG_UART_PUTCH
    unsigned char bSavePort;    
#endif // DEBUG_UART_PUTCH

#ifdef EXT_MEM
    unsigned char cIsConnected;
#endif // EXT_MEM

  CLRWDT();
#ifndef MM32F0133C7P		//? Add
	GIE = 0;
#else		///?OK
	//NOP
#endif
//    RB1 = 1;
	DELAY_Init();					//? Add
	CONSOLE_Init(115200);	//? Add
	//IWDG_Init();				//? Add 408ms x 4
  port_init();     //initialize ports
#ifndef MM32F0133C7P		//? Add
  TRISF = 0x00;
#else		///?OK
	//NOP
#endif
  //init_timers();    //initialize timers
	TP1_TOGGLE();
  KPD_fnInitKeypad();
  init_keypad_timers();  //initialize keypad timers
  InitialiseADC(0);   //initialize ADC ports
#ifndef MM32F0133C7P		//? Add
  InitialiseADC(1);
  InitialiseADC(2);
  InitialiseADC(3);
#endif
  SetADCChannel(0);
  pwm1_init();     //initialize PWM ports
  pwm2_init();
	ssfI2CInit();		 //Initialize the I2C driver.
  lcd_init();      //initialize LCD Display
  lcd_cls();
	//while(1);
#ifdef DEBUG_UART_PUTCH
  UART_init_comms();    // initialize UART serial port
#endif

    //DCP Diagnostics initialization:
    DCP_Diag.EXT_Memory_Fault = FALSE;

    Initialize_J1939_Variables();

  LCD_BKLT_0;
  key_pressed = FALSE;
  BREAKER_TRIP = 0;

  //initialize analog variables.
  AC_BR1_BR2_SERV = 0;
  OIL_OH_WAIT = 0;
  DECODE_TEMP = 10;
  TEMP = 7;
#ifndef MM32F0133C7P		//? Add
    TRISF = 0x00;
    PORTF = 0x00;
#else		///?OK
		//NOP
#endif

  //button Init
  START_BTN = 0;
  ON_OFF_BTN = 0;
  FAN_BTN = 0;
  AC_BTN = 0;
  HEAT_BTN = 0;
  HVAC_OFF_BTN = 0;
  TEMP_UP_BTN = 0;
  TEMP_DOWN_BTN = 0;
  BRESET_BTN = 0;
  adc_index = 4;
  temp_index = 0;

  CLRWDT();
  DelayMs(240);
  DelayMs(240);

  HourMeter_init();

  timers.CCU_Record_Timer = 5;

  CLRWDT();

  set_defaults();

  timers.settleTmr = 1;     // Give temperature a minute to settle

    //ssfI2CInit();             //Initialize the I2C driver.
		//I2C_NVIC_EepromTest();
		//I2C_NVIC_Eeprom256KbTest();
		//PWM_Test();
    enableTimerOne();
    Initialize_Diag();
    Initialize_DPF_Variables();
    DPF_Diag.APU_Minutes_Without_Regen = DPF_Diag.APU_Hours_Without_Regen * 60;
    DPF_Diag.Emergency_Regen_Flag = FALSE;
    DPF_Timers.Eight_Hour = 0;
    // On Board Memory (OBM) initialization. Timer is also used for external
    timers.OBM_Timer = 2;

#ifdef OBM
    if( !OBM_fnInit(1) )
    {
        OBM_fnReset(1); //memory is corrupted or new. Must reset the memory space.
    }

    if( !OBM_fnInit(2) )
    {
        OBM_fnReset(2); //memory is corrupted or new. Must reset the memory space.
    }
#endif // OBM

#ifdef RESET_OBM
    OBM_fnReset(1);
    OBM_fnReset(2);
#endif // RESET_OBM

#ifdef EXT_MEM
    // External memroy initialization    
    if(!EXM_fnInit())
    {
        cIsConnected = EXM_fnIsConnected();
        if( cIsConnected)
        {
            EXM_fnReset(); //memory is corrupted or new. Must reset the memory.     
        }      
        else
        {//memory module is not connected
        }
    }
    // Check again to see if the memory is connected (above line won't be called all the time)
    if (cIsConnected = EXM_fnIsConnected())
    {
        Datalog.Ext_Mem_Present = TRUE;        
    }
#endif // EXT_MEM

    // Datalog initialization
    Init_Datalogging();
    
    UART_printf("pllmul = 0x%x plldiv = 0x%x\r\n", (RCC->CR >> 26) & 0x3f, (RCC->CR >> 20) & 0x03);
  test_mode_flag = 0;
  test_mode_count_down = 10;
  while(test_mode_flag || test_mode_count_down){
#ifdef MM32F0133C7P			//? Add	
		if (/*xtest || */(!test_mode_flag && TestModePin == 0 && test_mode_count_down)) {
            TestMode();
            test_mode_count_down = 0;
            //xtest = 0;
        }
        if(test_mode_count_down != 0) test_mode_count_down -= 1;
        test_mode_flag = test_mode_process();
#endif
  }
  UART_printf("exit test mode\r\n");
  
  while(1)
  {
		//UART_printf(".");
		TP1_TOGGLE();
    CLRWDT();
    enableTimerOne();

    timerTick();         // increment timers
    keypadTimerTick();
    /*** KeyPad functions - one for each button - see keypad.h ***/
//        fnScanPad();
        KPD_fnKeypad();// FIXME
    /*************************************************************/

    testCold();

#ifndef MM32F0133C7P		//? Add
    GIE = 0;
#else		///?OK
		//NOP
#endif
    APU_HOURS = HourMeter() % 100000;
#ifndef MM32F0133C7P		//? Add
    GIE = 1;
#else		///?OK
		//NOP
#endif
    lcd_position(0,0,small);
    printf("HOURS");
    lcd_position(0,1,small);
    printf( "%05ld\r\n", APU_HOURS );   // display APU hours

    back_light();        // Backlight State Machine - backlit.h
    getPWMvalues();         // Retrieve appropriate PWM duty cycle
	//	fan_low_high = 40;
	//	heat_ac = 40;
    //pwm1(fan_low_high);        // pwm fan control signal - see pwm.h
    //pwm2(heat_ac);       // pwm A/C and Head control signal - see pwm.h
    pwm2(fan_low_high);        // pwm fan control signal - see pwm.h
    pwm1(heat_ac);       // pwm A/C and Head control signal - see pwm.h
		//while(1);

    getADvalues();       // Retrieve Analog signal levels
    decode_oil_oh_wait(OIL_OH_WAIT); // Decode analog levels for Oil Pressure and Over Heat
    decode_ac_br1_br2(AC_BR1_BR2_SERV); // Decode analog levels for AC Power and Breakers

    /***************** State Machines - see ice_pnl.c ************/

    HVAC_ON = check_hvac_on();
    apu();
    rtfPoll();                          // Process real-time clock request
    nvmUpdate();            // Delete all DTCs if latest DTC older than 30 days
    shore_power();    //See apu.c

#ifndef EXTRA_TIMERS
    battery_monitor();               //APU state machine
#endif // EXTRA_TIMERS
        
#ifdef USE_COLDSTART        
    (void) cold_start_monitor();
#endif

#ifndef EXTRA_TIMERS
    tmfTempMon();
#endif // EXTRA_TIMERS

    tkfTimeMonitor( &tkgTimer1 );
    tkfTimeMonitor( &tkgTimer2 );
#ifndef USE_COLDSTART
        tkfTimeMonitor( &tkgTimer3 );
        tkfTimeMonitor( &tkgTimer4 );
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
        tkfTimeMonitor( &tkgTimer5 );
        tkfTimeMonitor( &tkgTimer6 );
        tkfTimeMonitor( &tkgTimer7 );
        tkfTimeMonitor( &tkgTimer8 );
        tkfTimeMonitor( &tkgTimer9 );
        tkfTimeMonitor( &tkgTimer10 );
#endif // EXTRA_TIMERS

    tkfMaxTimer();

    fan_flag();       //Fan Flag state machine
    ac_heat_flag();      //A/C and Heat Flag state machine
    //control();         //Fan, A/C and Heat main control state machine
    outlet();         //Outlet control state machine
    thermostat();     //Thermostate control state machine
    VWM_fnViewManager();            // Process view changes
    /*************************************************************/
    key_pressed = 0;           // clear

    /******************* CAN Process ****************************/
#ifndef MM32F0133C7P		//? Add
    RXBNIE = 0;                 // disable receive interrupt during queue processing
#else		///?OK
		CAN_Peli_ITConfig(CAN_IT_RI, DISABLE);
#endif
    can_process();              // Needs to be called at a rate higher than (J1939TX_QUEUE_SIZE * 400uS)
                                // For some application, if only few message are transmitted, this rule
                                // can be not respected.  
    j1939_ProcessRefresh();     // Needs to be called at a rate higher than (J1939RX_QUEUE_SIZE * 400uS)
                                // If this rule is respected, no message will be lost even at a bus load
                                // of 100%

    Check_J1939_Mailboxex();
#ifndef MM32F0133C7P		//? Add
    RXBNIE = 1;                 // enable receive interrupt
#else		///?OK
		CAN_Peli_ITConfig(CAN_IT_RI, ENABLE);
#endif

    Current_Event = Get_Event();
    if (Current_Event != NO_EVENT)
    {
#ifdef DEBUG_UART_PUTCH
        bSavePort = SERIAL_bPortSelect;
        SERIAL_bPortSelect = PORT_UART; 
         printf("Event %d\r\n", Current_Event);
        SERIAL_bPortSelect = bSavePort;
#endif
        // If the escape key is pressed
        if ( Current_Event == EV_ESC_KEY )
        {
            // Clear the top most alert message
            VWM_fnClearAlert();
        }
        DPF_State_Machine (Current_Event);
    }
    

  }
}

