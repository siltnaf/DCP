/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file apu.c
       \brief Auxiliary Power Unit control state machine

<!-- DESCRIPTION: -->
    To provide control for sending signals to start or stop the APU.

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/apu.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
    
       $Log: not supported by cvs2svn $
       Revision 1.7  2014/08/25 19:31:34  blepojevic
       Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

       Revision 1.6  2014/08/11 19:07:42  blepojevic
       Changed debounce time for fault detection to 60 sec. Software made more robust to protect from storing dummy faults.

       Revision 1.5  2014/06/27 17:35:26  blepojevic
       Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

       Revision 1.4  2014/06/09 17:52:53  blepojevic
       Changed only debounce time for faults from 3 sec back to 1 sec

       Revision 1.3  2014/04/16 14:07:07  blepojevic
       Added diagnostic of APU and CCU

       Revision 1.2  2013/09/19 13:07:43  blepojevic
       Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

       Revision 1.1  2009/10/14 16:41:56  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.30  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.29  2008-10-28 15:54:28  msalimi
       - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
       - Removed the display of APU shutdown reason after ESC/RESET key push.
       - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

       Revision 1.28  2008-10-20 18:08:52  msalimi
       Improvements in displayed messages.

       Revision 1.27  2008-10-09 18:02:28  msalimi
       Changed and cleaned messages.

       Revision 1.26  2008-08-21 15:43:11  msalimi
       - Added feature to turn on the backlight when fault or important messages are being displayed.
       - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
       - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
       - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

       Revision 1.25  2008-08-12 20:40:07  msalimi
       Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

       Revision 1.24  2008-06-19 16:17:59  msalimi
       -Fixed the problem of automatic APU starts being blocked by the popup messages.

       Revision 1.23  2008-05-21 17:37:59  msalimi
       - Renamed some of the DPF timers and their associated events to better reflect their usage.
       - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

       Revision 1.22  2008-04-10 16:45:17  msalimi
       - Made a distinction between normal and emergency regen.
       - Extreme pressure and pressure sensor faults will be detected during normal regen.
       - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
       - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
       - A new command will be sent to DPF when an emergency regen is initiated.
       - APU warmup reduced to 2 minutes (from 5).

       Revision 1.21  2008-03-26 21:27:50  msalimi
       Fixed side effects of recent changes to how APU warmup period is handled for regeneration.

       Revision 1.20  2008-03-25 20:22:02  msalimi
       *** empty log message ***

       Revision 1.19  2008-03-25 19:14:38  msalimi
       - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
       - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
       - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
       - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
       - APU cooldown period is reduced to 2 minutes.
       - DPF related faults won't be captured when APU is off.
       - Freed code space by removing redundant code.
       - Faults are not displayed in DPF menu anymore.
       - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

       Revision 1.18  2008-01-31 20:54:29  msalimi
       -Added feature to clear DPF missing fault when DPF is uninstalled.
       -Made external and on-board memory applications independant of each other.

       Revision 1.17  2008-01-09 21:45:44  msalimi
       - Increased the low voltage detection delay at APU start up to 20 second. Also increased the number of consecutive low generator voltage indicator messages from 3 to 5.
       - Added DPF regeneration countdown message to the bottom of the home screen.

       Revision 1.16  2007-12-21 20:36:43  msalimi
       - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
       - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
       - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
       - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

       Revision 1.15  2007-12-06 23:21:32  msalimi
       Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

       Revision 1.14  2007-12-06 21:40:09  msalimi
       - Changed the message that delays the APU shutdown to also show the coolddown period.
       - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
       - Eliminated the "Heater Status" from DPF menu.

       Revision 1.13  2007-11-30 18:52:07  msalimi
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

       Revision 1.12  2007-11-01 19:41:33  msalimi
       -Changed the frequency of recording the CCU Status to external memory to every five minutes.
       -Fixed the bug that was causing the DM1 messages to be cleared after the clearing time was up.
       -Minor fixes in message display location on screen.

       Revision 1.11  2007-10-26 21:11:21  msalimi
       Fixed triggering of APU going off when in regen soon.

       Revision 1.10  2007-10-26 18:42:14  msalimi
       Added 5 minute cooldown for APU after DPF regeneration.
       Changed DPF variable menu to show all variables in one screen.
       Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
       Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

       Revision 1.9  2007-10-24 22:21:39  msalimi
       Removed unused flag.

       Revision 1.8  2007-10-19 21:12:42  msalimi
       New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

       Revision 1.7  2007-10-18 20:18:40  msalimi
       Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

       Revision 1.6  2007-10-09 15:03:18  msalimi
       Version 35-PPP-002. Second round of testing..

       Revision 1.5  2007-09-21 20:43:38  msalimi
       -New flags implemented for APU on and alternator outputs.
       -Added J1939 data update with APU information.

       Revision 1.4  2007-09-17 18:38:46  msalimi
       DPF related modifications

       Revision 1.3  2007-08-27 22:19:08  msalimi
       CAN communication working.

       Revision 1.2  2007-08-01 19:36:24  nallyn
       Added comments on variables

       Revision 1.1  2007-07-05 20:10:12  gfinlay
       Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

       Revision 1.16  2007-06-01 17:40:51  msalimi
       Neil added comments.

       Revision 1.15  2007/04/03 18:44:42  gfinlay
       Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.

       Revision 1.14  2007/02/27 01:20:17  nallyn
       Added 4 more timers for a total of 10 timers. The timers function differently
       than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
       timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
       depending on the temperture selected:
       18-20: High AC
       21-23: Low AC
       24: HVAC Off
       25-27: Low Heat
       28-30: High Heat
   
       Revision 1.13  2007/02/26 17:49:52  nallyn
       Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
   
       Revision 1.12  2007/02/23 18:11:28  nallyn
       Changed from 2 timers to 4 timers for testing purposes

       Revision 1.11  2006/02/08 20:09:29  nallyn
       Updates based on code review
       Checked array bounds

       Revision 1.10  2006/02/08 19:39:08  nallyn
       Review Code with updated file headers

       Revision 1.9  2005/12/20 18:04:46  nallyn
       November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "apu.h"                // Interface to APU State Machine
#include "main.h"               // Common definitions
#include "menu.h"               // Menu definitions
#include "timers.h"             // Interface to software counter timers
#include "lcd_grp1.h"           // Interface to LCD
#include "fonts.h"              // Interface to LCD fonts
#include "battmon.h"            // Interface to Battery Voltage Monitor
#include "coldstart.h"
#include "timekeep.h"           // Interface for clock time keeping
#include "tempmon.h"            // Interface to Temperature Monitor
#include "iconblinker.h"        // Interface to Icon definitions
#include "hrmeter2.h"
#include "Events.h"
#include "ViewMan.h"            // Interface to using View Manager
#include "nvmem.h"
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "defaults.h"
#include "Datalog.h"
#include "rtclock.h"
#include "DPF.h"
#include "delay18.h"
#include "uart.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#define MODIFY1
#define CRANK_LIMIT         5

#ifndef MM32F0133C7P		//? Add
// Define engine control bit masks in PORTE
#define APU_ALT_OFF_MASK    0x20
#define APU_START_MASK      0x40
#define APU_ON_MASK         0x80
#define APU_CTRL_PORT       LATE    // use latch register on PORTE
#else
// Define engine control bit masks in PORTD
#define APU_ALT_OFF_MASK    0x40
#define APU_START_MASK      0x08
#define APU_ON_MASK         0x04
#define APU_CTRL_PORT       PortDFlag			// use latch register on PORTD
#endif

enum apu_eENGINE_CRTL
{
    APU_eENGINE_OFF,
    APU_eENGINE_ON,
    APU_eENGINE_START,
    APU_eENGINE_ALT_OFF,
    APU_eENGINE_ALT_ON,
    APU_eENGINE_LAST_ENTRY
};

#ifdef PRODUCTION
    #define RESTART_TIME    180     //seconds (3 minutes)
#else
    #warning PRODUCTION NOT DEFINED
#endif

//!State definitions for cold start
enum Cold {apuOff,waitOne,glowTen,startTen};


/* ---------- I n t e r n a l   D a t a ---------- */

char overcrank = 0;
static char apu_on_reasons;     // Bit map of reasons APU should be on
#ifdef MM32F0133C7P		//? Add
static unsigned char PortDFlag;
#endif

/* ---------- G l o b a l   D a t a ---------- */
APU_DIAG APU_Diag;

//! Decoding the feedbacks signal from the APU
typedef struct
{
    int adValue;    //!< Raw ADC value
    char faultOneOil;  //!< The APU is in a fault state OIL fault
    char faultOneTemp;  //!< The APU is in a fault state Coolant Overtemperature fault
    char faultOneAltFb;  //!< The APU is in a fault state Alernator Feedback fault
    char interlock;  //!< The APU is in a fault state Alernator Feedback fault
    char glowWait;  //!< The APU is in the glowing state
    char apuOn;     //!< The APU is on
} oilOhWait;

//! The structure of possible APU feedback values -
//! where xxxx in adCal(xxxx) is in millivolts
const oilOhWait oilOverHeatWait[6] =
{
    {adCal(300),OFF,OFF,OFF,OFF,OFF,OFF},
    {adCal(1100),ON,OFF,OFF,OFF,OFF,OFF},
    {adCal(2100),OFF,ON,OFF,OFF,OFF,OFF},
    {adCal(3100),OFF,OFF,ON,OFF,OFF,OFF},
    {adCal(4050),OFF,OFF,OFF,ON,OFF,OFF},
    {adCal(5000),OFF,OFF,OFF,OFF,ON,ON}
};

extern unsigned char TxPgn_65280[8];

/* ----------------- F u n c t i o n s ----------------- */
static void Clear_APU_Related_Messages(void);

/******************************************************************************/
/*!

<!-- FUNCTION NAME: decode_oil_oh_wait -->
      \brief Function decodes the multiplexed signals on the OIL_OH_WAIT PWM input signal

<!-- PURPOSE: --> 
      Input RA1 is connected to a PWM-encoded output from
      the APU controller (the PWM levels are used to multiplex logical digital
      signals) called OIL_OH_WAIT (Oil overheat wait).<BR>
      The input is first decoded using a lookup table which maps the input to one of 3 ranges to
      extract the corresponding values of the status of the un-debounced
      "overheat fault" signal and the "WAIT" and "APU_ON_FLAG" signals.
      
      \code
      Input Range(mV)      faultOIL   faultTemp     faultAltFb   interlock      WAIT                      APU_ON_FLAG
      0-500                OFF        OFF           OFF          OFF            OFF                       OFF
      501-1500             ON         OFF           OFF          OFF            OFF                       OFF
      1501-2500            OFF        ON            OFF          OFF            OFF                       OFF
      2501-3500            OFF        OFF           ON           OFF        OFF                       OFF 
      3501-4500            OFF        OFF           OFF          ON         OFF                       OFF
      4501-5000        OFF        OFF           OFF          OFF            OFF((APU_STATE ==         If ((APU_STATE !=
                                                                            ST_APU_GLOW )             ST_APU_GLOW)
                                                                            || START_BTN))            && !START_BTN))
                                                                            then ON                   then ON
                                                                            else OFF                  else OFF
      \endcode

      If the raw fault remains asserted (AN1 is in range 501-3500mV) for
      each time it is sampled (typically 2 consecutive samples) until a
      10msec timer elapses, then the corresponding debounced
      ENGINE_FAULT signal is tripped or turned on. If the raw fault
      signal returns to OFF (AN1 is in range 0-500 or in range
      4501-5000) then the state machine is reset and a new 10msec
      countdown timer period will be required before a transition to
      assert ENGINE_FAULT. No debounce or countdown timer is needed for
      the WAIT or APU_ON_FLAG signals - i.e. a single sample in the
      range 4501-5000mW is all that is needed to set either WAIT or
      APU_ON_FLAG (depending on the APU_STATE or the condition of
      START_BTN).

<!-- PARAMETERS: -->
       @param[in]  tmp     Raw ADC value to be decoded (read from RA1 input)
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void decode_oil_oh_wait(volatile int tmp)
{
    static enum b_state faultState = ST_OFF;
    static bit fault;       // made static to use bit; bit ops use less code
    static bit faultOil;       // made static to use bit; bit ops use less code
    static bit faultTemp;       // made static to use bit; bit ops use less code
    static bit faultAltFb;       // made static to use bit; bit ops use less code
    static bit faultInterlock;       // made static to use bit; bit ops use less code
    static char faultType;
    char i;
    
    // Scan through oil overheat ad values
    for ( i = 0; i < DIM( oilOverHeatWait ); i++ )
    {
        // If data below the table value
        if ( tmp <= oilOverHeatWait[i].adValue )
        {
            // Quit search and use that index
            break;
        }
    }

    // Preset flags
    fault           = FALSE;
    faultOil        = FALSE;
    faultTemp       = FALSE;
    faultAltFb      = FALSE;
    faultInterlock  = FALSE;
    WAIT            = FALSE;
    APU_ON_FLAG     = FALSE;
    ENGINE_INTRLCK  = FALSE;
    
    // Action depends on oil overheat setting
    switch ( i )
    {
        case 1:
            fault = TRUE;
            faultOil = TRUE;
            break;
        case 2:
            fault = TRUE;
            faultTemp = TRUE;
            break;
        case 3:
            fault = TRUE;
            faultAltFb = TRUE;
            break;
        case 4:
            fault = TRUE;
            faultInterlock = TRUE;
            break;
        case 5:
            if ( ( APU_STATE == ST_APU_GLOW ) || START_BTN )
            {
                WAIT = TRUE;
            }
            else
            {
                APU_ON_FLAG = TRUE;
            }
            break;
        case 0:
        default:
            break;
    }

    // Debounce fault
    switch ( faultState )
    {
        case ST_OFF:
            ENGINE_FAULT = 0;
            engineFaultOilPrev = FALSE;
            engineFaultTempPrev = FALSE;
            engineFaultAltFbPrev = FALSE;
			TxPgn_65280[6] = 0;
			faultType = 0;

            ResetFaults();

            if ( fault )
            {
                faultState = ST_TRIPPING;
                timers.faultTmr = 2; //Changed back to 1 ( !!! 1 sec )
                faultType = i;
            }
            break;
        case ST_TRIPPING:
            if ( fault && (i == faultType))
            {
               if ( !timers.faultTmr )
                {
                    faultState = ST_TRIPPED;
                }
            }
            else
            {
                faultState = ST_OFF;
            }
            break;
        case ST_TRIPPED:
            ENGINE_FAULT = 1;

            if ((faultOil == TRUE) && (i == faultType))
            {	
                ENGINE_FAULT_OIL = TRUE;
                if (engineFaultOilPrev == FALSE)
                {
                    Record_Fault( SPN_APU_OIL_PRESS, FMI_APU_LOW_OIL_PRESS, SET );
                    TxPgn_65280[6] |= 0x01;
                    
                }
            }
            else if ((faultTemp == TRUE) && (i == faultType))
            {
                ENGINE_FAULT_TEMP = TRUE;
                if (engineFaultTempPrev == FALSE)
                {
                    Record_Fault( SPN_APU_COOLANT_TEMP, FMI_APU_OVERTEMP, SET );
                    TxPgn_65280[6] |= 0x02;
                }
            }
            else if ((faultAltFb == TRUE) && (i == faultType))
            {
                ENGINE_FAULT_ALTFB = TRUE;
                if(engineFaultAltFbPrev == FALSE)
                {
                    Record_Fault( SPN_APU_ALT_STATUS, FMI_APU_ALT_FB, SET );
                    TxPgn_65280[6] |= 0x04;
                }
            }
            else if (faultInterlock == TRUE)
                ENGINE_INTRLCK = TRUE;

            engineFaultOilPrev = ENGINE_FAULT_OIL;
            engineFaultTempPrev = ENGINE_FAULT_TEMP;
            engineFaultAltFbPrev = ENGINE_FAULT_ALTFB;

            if ( !fault )
            {
                faultState = ST_OFF;
            }
            break;
        default:
            ENGINE_FAULT = 0;
            faultState = ST_OFF;
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: shore_power -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void shore_power(void)
{
    if(SHORE_POWER)
    {
        iconShow( ICON_SHORE_POWER );
    }
    else
    {
        iconErase( ICON_SHORE_POWER );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: apuClearMinRun -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: void -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void apuClearMinRun( void )
{
    timers.minRunTmr = 0;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: apuPriority -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  priority    Priority value to test against apu_on_reasons
<!-- RETURNS: -->
      \return TRUE if the on reasons include the test priority and there 
      is no greater priority item, otherwise FALSE.

*/
/******************************************************************************/
char apuPriority( char priority )
{
    char test;
    
    test = ( apu_on_reasons >> 1 );
    
    // If not in AUTO mode priority is nothing
    if ( !AUTO )
        return FALSE;
    // If the on reasons include this priority and there is none greater
    else if ( ( priority & apu_on_reasons ) && ( priority > test ) )
        return TRUE;
    else
        return FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: set_on_reasons -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void set_on_reasons( void )
{
    apu_on_reasons = 0;
    
    // Battery monitor can start APU even if fault lockout is true
    // or if there is shore power
#ifndef EXTRA_TIMERS
    if ( BM_RUN )
        apu_on_reasons |= APU_ON_BY_BATTMON;
#endif // EXTRA_TIMERS
        
    // Other features are not to run if engine fault lockout is true
    if ( apuFaultLockout )
        return;
        
    // Here are reasons to start and run if we do not have shore power
    // Cannot run heating or cooling if HVAC breaker is tripped
    if ( !SHORE_POWER /*&& !BR1*/ )
    {
#ifdef USE_COLDSTART
        if ( bCmRun )
            apu_on_reasons |= APU_ON_BY_COLDSTART;
#endif
        if ( tkgTimer1.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER1;
        if ( tkgTimer2.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER2;
#ifndef USE_COLDSTART
        if ( tkgTimer3.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER3;
        if ( tkgTimer4.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER4;
#endif // USE_COLDSTART
#ifndef EXTRA_TIMERS
        if ( tmgTempMonRun )
            apu_on_reasons |= APU_ON_BY_TEMPMON;
#else
        if ( tkgTimer5.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER5;
        if ( tkgTimer6.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER6;
        if ( tkgTimer7.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER7;
        if ( tkgTimer8.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER8;
        if ( tkgTimer9.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER9;
        if ( tkgTimer10.tkxRun )
            apu_on_reasons |= APU_ON_BY_TIMER10;
#endif // EXTRA_TIMERS
        if ( timers.minRunTmr )
            apu_on_reasons |= APU_ON_BY_MINIMUM_RUN;
    }
}

void apu_fnEngineCtrl( uint8 u8Control )
{
    static uint8 u8EngineState;

    // Set engine control output lines
    switch ( u8Control )
    {
        case APU_eENGINE_ON:
            // If engine was off
            if ( u8EngineState == APU_eENGINE_OFF )
            {
                // Just set the ON bit
                APU_CTRL_PORT ^= APU_ON_MASK;
#ifdef MODIFY1
								GPIO_SetBits(GPIOD, GPIO_Pin_2);
#endif
                // State is on
                u8EngineState = APU_eENGINE_ON;
            }
            // If engine was in Start State
            else if ( u8EngineState == APU_eENGINE_START )
            {
                // Set ON bit and clear START bit with XOR
                APU_CTRL_PORT ^= ( APU_ON_MASK | APU_START_MASK );
#ifdef MODIFY1
								GPIO_SetBits(GPIOD, GPIO_Pin_2);
								GPIO_ResetBits(GPIOD, GPIO_Pin_3);
#endif
                // State is On
                u8EngineState = APU_eENGINE_ON;
            }
            // Cannot transition to On from Alternator Off
            break;
            
        case APU_eENGINE_START:
            // Engine must be in On state to transition to Start state
            if ( u8EngineState == APU_eENGINE_ON )
            {
                // Clear ON bit and set START bit with XOR
                APU_CTRL_PORT ^= ( APU_ON_MASK | APU_START_MASK );
#ifdef MODIFY1
								GPIO_ResetBits(GPIOD, GPIO_Pin_2);
								GPIO_SetBits(GPIOD, GPIO_Pin_3);
#endif
                // State is Start
                u8EngineState = APU_eENGINE_START;
            }
            break;
            
        case APU_eENGINE_ALT_OFF:
            // Engine must be in On state to transition to Alt Off state
            if ( u8EngineState == APU_eENGINE_ON )
            {
                // Clear ON bit and set ALT_OFF bit with XOR
                APU_CTRL_PORT ^= ( APU_ON_MASK | APU_ALT_OFF_MASK );
#ifdef MODIFY1
								GPIO_ResetBits(GPIOD, GPIO_Pin_2);
								GPIO_SetBits(GPIOD, GPIO_Pin_6);
#endif
                // State is Alt Off
                u8EngineState = APU_eENGINE_ALT_OFF;
            }
            break;
            
        case APU_eENGINE_ALT_ON:
            // Engine must be in Alt Off state to transition to On state
            if ( u8EngineState == APU_eENGINE_ALT_OFF )
            {
                // Set ON bit and clear ALT_OFF bit with XOR
                APU_CTRL_PORT ^= ( APU_ON_MASK | APU_ALT_OFF_MASK );
#ifdef MODIFY1
								GPIO_SetBits(GPIOD, GPIO_Pin_2);
								GPIO_ResetBits(GPIOD, GPIO_Pin_6);
#endif
                // Alternator on state devolves to engine on state
                u8EngineState = APU_eENGINE_ON;
            }
            break;
            
        default:                   // Default to Engine Off for safety
        case APU_eENGINE_OFF:
            // Always set the outputs for engine off, not just on state change
            // Turn off all engine control bits
            APU_CTRL_PORT &= ~( APU_ON_MASK | APU_START_MASK | APU_ALT_OFF_MASK );
#ifdef MODIFY1
						GPIO_ResetBits(GPIOD, GPIO_Pin_2);
						GPIO_ResetBits(GPIOD, GPIO_Pin_3);
						GPIO_ResetBits(GPIOD, GPIO_Pin_6);
#endif
            u8EngineState = APU_eENGINE_OFF;
            TxPgn_65280[3] &= ~(0x01);
            break;
            
    }
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: apu -->
       \brief State Machine for APU starting and running process. 

<!-- PURPOSE: -->
The descriptions here give an overview of the states. For specific timing details, see the individial
states.
    \code
    Can assume one of fourteen states:
        1:  ST_APU_OFF - Apu off. This state can be exited by either the user
            starting the APU, the presence of shorepower or an auto start
            function.
        2:  ST_SHORE_POWER - if AC_PWR is present but the unit is off, this is
            the state of the APU. This state prevents the user from starting
            the APU, but allows certain auto start functions to start the APU. 
        3:  ST_CHECK_LOCK - this state checks to see if there is an engine
            interlock being signalled by the APU.
        4:  ST_APU_GLOW - this state is the glowing state of the APU. it waits
            for the APU to signal that it. 
        5:  ST_APU_LOCK_OUT - this state is assumed when an engine interlock
            has been detected.
        6:  ST_APU_START - APU cranking state
        7:  ST_COLD_START - APU cranking state for a cold start
        8:  ST_APU_WAIT - State to determine if the APU is running or not
            after a start attempt.
        9:  ST_APU_RESTART - In case of no start this is the restart state.
            6 second pause to let the starter cool. Also checks the over_crank 
            variable to see if it has reached the CRANK_LIMIT.
        10: ST_APU_CRANK_LIMIT - If the engine fails to start, the crank limit 
            is reached and "OVER CRANK" is displayed. This state is exited 
            only by pressing the ON/OFF button.
        11: ST_APU_RUN - The run state. As long as the APU is running, this
            state is maintained. It can be terminated by the user, by the APU
            in the case of a FAULT or by one of the auto-functions expiring
        12: ST_APU_STOPPING - This state verifies that a fault occured  
            If there is a fault, the next state is ST_APU_OH. If there is no 
            fault and the APU has not shut down, the next state is ST_APU_RUN.
            If there is no fault and the APU has shut down the next 
            state is ST_APU_OFF. This would be the situation if the control
            cable is cut
        13: ST_APU_STOPPED - sets the APU_STOPPED flag which is used in 
            ice_pnl.c for disabling the outlet, ac and fan. Goes to
            ST_SHORE_POWER if AC_PWR is detected and the APU is off.
        14: ST_APU_FAULT - if there has been an APU fault 
            (either oil or overheat) "CHECK APU ENGINE" 
            is displayed.
    external (global) variables:
        ON_OFF_BTN - mapped to keypad On/off button
        WAIT
        APU_ON_FLAG

    Note:
        timers.glowingTmr is mis-named. It is used as a general timer in the state machine
    \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void apu(void)
{
    static enum Cold coldStart = apuOff;
    bool bStopNow;
    bool bStopLater;
    bool bContinue;
    
    tmfSetMode();
    set_on_reasons();
    
    switch(APU_STATE)
    {
        case ST_APU_OFF:         //APU off state
            apu_fnEngineCtrl( APU_eENGINE_OFF );
            VWM_fnRemoveView( VWM_eVIEW_APU_STARTING );
            iconErase( ICON_APU_ON );
            iconShow( ICON_APU_BAR );
            
            APU_Diag.APU_Warmed_Up = FALSE;
            APU_Diag.APU_On_Off_Flag = OFF;
            APU_Diag.Noncritical_Shutdown = FALSE;
            
            if ( APU_Diag.APU_Off_Requested )
            {
                Abort_Regen();
                APU_Diag.APU_Off_Requested = FALSE;
            }
            
            // If ENGINE_FAULT when APU off then lock out extended operations
            apuFaultLockout = ENGINE_FAULT;

            // If engine lockout
            if ( apuFaultLockout )
            {
                // Deactivate Comfort Monitor
                tmgTMActive = FALSE;
            }
                        
            // waitTmr is set in ST_APU_STOPPED to 1/2 second. This resets the
            // APU_STOPPED flag
            if(!timers.waitTmr)
            {
                APU_STOPPED = 0;
            }
            SHORE_POWER = 0;

            if(AC_PWR)
            {
                APU_STATE = ST_SHORE_POWER;
                timers.waitTmr = 100;
            }
            // Only start the APU if:
            else if ( ( ( DPF_Diag.Record & MISSING ) == 0 )
                   && ( ! VWM_fnViewBlocksApu() )
                   && ( ! VWM_fnQueryView( VWM_eVIEW_MENU ) ) )
            {
                if ( ON_OFF_BTN ) //if the On button is pressed
                {
                    // If in Temperature Monitor Mode
                    if ( tmgTMMode )
                    {
                        // Toggle Active temperature monitoring 
                        tmgTMActive = ! tmgTMActive;
                        
                        // Start or stop the timer
                        if ( tmgTMActive )
                            timers.tmRunTmr = ( HOUR_TENTHS_TO_MINUTES( tmgTMDuration ) );
                        else
                            timers.tmRunTmr = 0;
                    }
                    else
                    {
                        // Do manual (user) start of APU
                        tkgMaxRunTimedOut = 0;
                        AUTO = 0;
                        overcrank = 0;

                        timers.glowingTmr = 7;
                        timers.waitTmr = 50;
                        iconErase( ICON_APU_BAR );
                        APU_STATE = ST_CHECK_LOCK;
                    }
                }
                else
                {
                    // The AUTO flag indicates that the unit has started from one of the 
                    // autostart features and was not started by the user
                    AUTO = 1;
                    if( apu_on_reasons != 0 )
                    {
                        if (APU_Diag.Crtical_Shutdown)
                            break;
                        // restartTmr is always set to RESTART_TIME - 10 (now 3) minutes. This is the 
                        // minimum off time before the engine can be restarted by an auto function
                        if ( timers.restartTmr == 0 )
                        {
                            tkgMaxRunTimedOut = 0;
                            overcrank = 0;
                            timers.glowingTmr = 7;
                            timers.waitTmr = 50;
                            iconErase( ICON_APU_BAR );
                            APU_STATE = ST_APU_GLOW;
                        }
                    }
                }
            }
            // If DPF lost and user turning on APU
            else if ( ( DPF_Diag.Record & MISSING )
                     && ( ON_OFF_BTN ) )
            
            {
                // Show DPF LOST message
                VWM_fnRequestView( VWM_eVIEW_DPF_DISCONNECT_ALERT, BACKLIGHT_ON );
            }
            
            break;
        case ST_SHORE_POWER:
            apu_fnEngineCtrl( APU_eENGINE_OFF );
            iconShow( ICON_APU_BAR );
            // Set in ST_APU_OFF for 1 second - prevents icon flicker on the screen
            // Set in ST_APU_STOPPED for 1/2 second - prevents icon flicker on the
            // screen
            if(!timers.waitTmr)
            {
                SHORE_POWER = 1;
            }

            if(!AC_PWR)
            {
                APU_STATE = ST_APU_OFF;
            }
            // Restart timer is always set to 10 minutes. Some functions may start the 
            // APU even when shorepower is present (eg: battery monitor) But, if the 
            // APU had been running, it must wait 10 minutes before a restart
            else if(( apu_on_reasons != 0 ) && !timers.restartTmr)
            {
                AUTO = 1;
                tkgMaxRunTimedOut = 0;
                overcrank = 0;
                timers.glowingTmr = 7;
                timers.waitTmr = 50;
                APU_STATE = ST_APU_GLOW;
            }
            break;
        case ST_CHECK_LOCK:
            iconControl( ICON_APU_ON, ICON_BLINK );
            VWM_fnRequestView( VWM_eVIEW_APU_STARTING, BACKLIGHT_OFF );

            // 1/2 second delay set in ST_APU_OFF. Gives the APU opportunity to respond 
            // with an interlock signal.
            if ( !timers.waitTmr )
            {
                if ( ENGINE_FAULT )
                {
                    timers.glowingTmr = 2;
                    APU_STATE = ST_APU_LOCK_OUT;
                }
                else
                {
                    tkgMaxRunTimedOut = 0;
                    AUTO = 0;
                    overcrank = 0;
                    timers.glowingTmr = 7;
                    timers.waitTmr = 50;
                    APU_STATE = ST_APU_GLOW;
                }
            }
            break;
        case ST_APU_GLOW:
            apu_fnEngineCtrl( APU_eENGINE_ON );
            VWM_fnRequestView( VWM_eVIEW_APU_STARTING, BACKLIGHT_OFF );
            iconControl( ICON_APU_ON, ICON_BLINK );
            
            // Things to do before the apu engine starts
            // Clear the fault record
            DPF_Diag.Record = 0;
            
            if(ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                APU_STATE = ST_APU_OFF;
            }
            else if(START_BTN)
            {
                MANUAL = 1;
                APU_STATE = ST_APU_START;
                timers.glowingTmr = 20;
            }
            // glowingTmr is set in ST_APU_OFF, ST_SHORE_POWER, ST_CHECK_LOCK for 
            // 7 seconds - this is the minimum glow time. When coming from 
            // ST_APU_RESTART, this is set at 5 seconds, since the glowplugs have 
            // already been enabled in previous start attempts
            else if(!WAIT && !timers.glowingTmr)
            {
                MANUAL = 0;
                if(COLD)
                {
                    APU_STATE = ST_COLD_START;
                }
                else
                {
                    APU_STATE = ST_APU_START;
                    timers.glowingTmr = 10;
                }
            }
            else
            {
                APU_STATE = ST_APU_GLOW;
            }
            break;
        case ST_APU_LOCK_OUT:
            tmgTMActive = FALSE;
            apu_fnEngineCtrl( APU_eENGINE_OFF );
            VWM_fnRemoveView( VWM_eVIEW_APU_STARTING );
            VWM_fnRequestView( VWM_eVIEW_ENGINE_INTERLOCK, BACKLIGHT_ON );
            // glowingTmr is set in ST_CHECK_LOCK and is used as a general purpose timer 
            // here to reset the display in 2 seconds if the user does not hit the 
            // ON_OFF button
            if(ON_OFF_BTN || !timers.glowingTmr)
            {
                VWM_fnRemoveView( VWM_eVIEW_ENGINE_INTERLOCK );
                APU_STATE = ST_APU_OFF;
            }
            break;
        case ST_APU_START:
            apu_fnEngineCtrl( APU_eENGINE_START );
            VWM_fnRequestView( VWM_eVIEW_APU_STARTING, BACKLIGHT_OFF );
            iconControl( ICON_APU_ON, ICON_BLINK );
            if(ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                APU_STATE = ST_APU_OFF;
            }
            // glowingTmr is set in ST_APU_GLOW at 10 seconds, waiting for the APU to 
            // crank, giving a maximum of 10 seconds before the APU is confirmed running 
            // in ST_APU_WAIT. The APU can end this start period early by signalling 
            // that it is on
            else if(!timers.glowingTmr || APU_ON_FLAG)
            {
                timers.apuOnTmr = 15;
                timers.glowingTmr = 4;
                APU_STATE = ST_APU_WAIT;
            }
            else if(MANUAL && !START_BTN)
            {
                timers.apuOnTmr = 15;
                timers.glowingTmr = 4;
                APU_STATE = ST_APU_WAIT;
            }
            break;
        case ST_COLD_START:
            if(ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                APU_STATE = ST_APU_OFF;
            }
            else
            {
                switch (coldStart)
                {
                    case apuOff:
                        apu_fnEngineCtrl( APU_eENGINE_OFF );
                        timers.glowingTmr = 1;
                        coldStart = waitOne;
                        break;
                    case waitOne:
                        // glowingTmr is used as a local timer for the coldStart sub-state machine. 
                        // It is set in apuOff to 1 second. This delay ensures the APU is off
                        // before attempting the cold start
                        if(!timers.glowingTmr)
                        {
                            timers.glowingTmr = 10;
                            coldStart = glowTen;
                        }
                        break;
                    case glowTen:
                        apu_fnEngineCtrl( APU_eENGINE_ON );
                        // glowingTmr is set in waitOne to 10 seconds. The APU glowplugs are allowed 
                        // to Glow for 10 seconds
                        if(!timers.glowingTmr)
                        {
                            timers.glowingTmr = 15;
                            coldStart = startTen;
                        }
                        break;
                    case startTen:
                        apu_fnEngineCtrl( APU_eENGINE_START );
                        // glowing timer is set in glowTen to 15 seconds. This causes the APU to 
                        // crank for 15 seconds, without looking to see if the apu has started or 
                        // not.
                        if(!timers.glowingTmr)
                        {
                            timers.apuOnTmr = 15;
                            timers.glowingTmr = 4;
                            coldStart = apuOff;
                            APU_STATE = ST_APU_WAIT;
                        }
                        break;
                }
            }
            break;
        case ST_APU_WAIT:
            APU_STOPPED = 0;
            apu_fnEngineCtrl( APU_eENGINE_ON );
            VWM_fnRemoveView( VWM_eVIEW_APU_STARTING );
            VWM_fnRequestView( VWM_eVIEW_ENGINE_WAIT, BACKLIGHT_OFF );
            iconControl( ICON_APU_ON, ICON_ON );
            if (ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                timers.waitTmr = 50;
                VWM_fnRemoveView( VWM_eVIEW_ENGINE_WAIT );
                APU_STATE = ST_APU_STOPPED;
            }
            // apuOnTmr is set in ST_APU_START and ST_COLD_START to 15 seconds
            // glowingTmr is set in the same states to 4 seconds giving a minimum of 4 
            // and maximum of 10 seconds to check if the APU is running (APU_ON_FLAG)
            else if(!APU_ON_FLAG && (timers.apuOnTmr != 0) && !timers.glowingTmr)
            {
                VWM_fnRemoveView( VWM_eVIEW_ENGINE_WAIT );
                APU_STATE = ST_APU_RESTART;
                overcrank++;
                timers.apuOnTmr = 6;
            }
            // glowingTmr is set in ST_APU_START and ST_COLD_START to 4 seconds to give 
            // at least 4 seconds for the APU to respond that it is running
            else if(APU_ON_FLAG && !timers.glowingTmr)
            {
                timers.maxRunTmr = ( HOUR_TENTHS_TO_MINUTES( tkgMaxRunTime ) );
                if ( apu_on_reasons & APU_ON_BY_TEMPMON )
                {
                    // Start the minimum run timer
                    timers.minRunTmr = TM_MINIMUM_RUN;
                }
                VWM_fnRemoveView( VWM_eVIEW_ENGINE_WAIT );
                APU_STATE = ST_APU_RUN;
            }
            else
            {
                APU_STATE = ST_APU_WAIT;
            }
            break;
        case ST_APU_RESTART:
            apu_fnEngineCtrl( APU_eENGINE_OFF );
            iconControl( ICON_APU_ON, ICON_ON );
            VWM_fnRequestView( VWM_eVIEW_APU_RESTARTING, BACKLIGHT_OFF );
            if(overcrank >= CRANK_LIMIT)
            {
                APU_STATE = ST_APU_CRANK_LIMIT;
                VWM_fnRemoveView( VWM_eVIEW_APU_RESTARTING );
            }
            else if(ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                APU_STATE = ST_APU_OFF;
                VWM_fnRemoveView( VWM_eVIEW_APU_RESTARTING );
            }
            // apuOnTmr is set in ST_APU_WAIT to 6 seconds so that if, after 6 seconds, 
            // this state is still active, we try a restart. This results in a 11 second 
            // pause between cranking: 6 here + 5 glowing
            else if(!timers.apuOnTmr)
            {
                timers.glowingTmr = 5;
                APU_STATE = ST_APU_GLOW;
                VWM_fnRemoveView( VWM_eVIEW_APU_RESTARTING );
            }
            break;
        case ST_APU_CRANK_LIMIT:
            AUTO = 0;
            iconShow( ICON_APU_BAR );
            VWM_fnRequestView( VWM_eVIEW_CRANK_LIMIT, BACKLIGHT_ON );
            if(ON_OFF_BTN)
            {
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                VWM_fnRemoveView( VWM_eVIEW_CRANK_LIMIT );
                APU_STATE = ST_APU_OFF;
            }
            break;
        case ST_APU_RUN:
            //DPF_Diag.Record = 0;
            if (APU_Diag.APU_On_Off_Flag != ON)
            {
                APU_Diag.APU_On_Off_Flag = ON;
                APU_Diag.APU_Off_Requested = FALSE;
                APU_Diag.APU_Warmup_Timer = APU_WARMUP_TIME;
                Initialize_DPF_Variables();
                
                // If the eight hour timer is running
                if ( DPF_Timers.Eight_Hour != 0 )
                {
                    // Re-instate the EXHST_PRESS_SNSR_FAIL_ONCE
                    DPF_Diag.Record |= EXHST_PRESS_SNSR_FAIL_ONCE;
                }
                
#ifdef EXT_MEM
                APU_Start_Total++;
                (void) nvfPutNumber (NV_ID_APU_START_TOTAL, APU_Start_Total);
#endif // EXT_MEM
            }
#ifdef EXT_MEM
            if (( Datalog.Ext_Mem_Present ) && ( Datalog.Logging_On == FALSE))
            {
                // Don't want to start if we have already done so
                Record_APU_Status_External();
                Start_Datalog();
            }
#endif // EXT_MEM
            // Update data in J1939 APU Status message
            TxPgn_65280[3] |= 0x01; 
            // End of update data in J1939 APU Status message
            apu_fnEngineCtrl( APU_eENGINE_ON );
            iconControl( ICON_APU_ON, ICON_ON );

            VWM_fnRemoveView (VWM_eVIEW_DPF_TURN_APU_ON);

            if (( DPF_Diag.Emergency_Regen_Flag ) && (APU_Diag.Emergency_Regen_Sent_Once == FALSE))
            {
                APU_Diag.Emergency_Regen_Sent_Once = TRUE;
                Add_Event (EV_REGEN_NOW);
            }

            if(HVAC_ON && !SHORE_POWER)
            {
                // Disable automatic features
                AUTO = 0;
            }

            // HVAC Off button and not Tempmon mode
            if( HVAC_OFF_BTN && !tmgTMMode )
            {
                AUTO = 0;
            }

            bStopNow = FALSE;
            bStopLater = FALSE;
            bContinue = ( ( DPF_Diag.bNeedToRegen )
                       || ( DPF_Diag.bNeedToCool )
                       || ( DPF_Diag.bRegenCmdSent ) );
            
            // The Start button occurs when on/off button has been
            // held for 5 seconds, so the occurance of the
            // start button means the user is forcing the APU to turn off
            if ( START_BTN )
            {
                bStopNow = TRUE;
            }
            // The user has pushed the on/off button
            if ( ON_OFF_BTN )
            {
                tmgTMActive = FALSE;
                if ( bContinue )
                {
                    bStopLater = TRUE;
                    // Turn off HVAC controls
                    // Be aware this is only reinstated by Abort_Regen later
                    DPF_fnTurnOffLoads();
                    // Display message to hold the APU ON/OFF for
                    // 5 seconds to turn the APU off
                    VWM_fnRequestView (VWM_eVIEW_DPF_HOLD_ON_OFF_BUTTON, BACKLIGHT_ON);
                }
                else
                {
                    bStopNow = TRUE;
                }
            }
            // Turn off the APU for a non-critical fault (delayed for cool down)
            if ( APU_Diag.Noncritical_Shutdown )
            {
                if ( bContinue )
                {
                    // Turn off HVAC controls
                    // Be aware this is only reinstated by Abort_Regen
                    DPF_fnTurnOffLoads();
                    bStopLater = TRUE;
                }
                else
                {
                    bStopNow = TRUE;
                }
                // Clear the flag
                APU_Diag.Noncritical_Shutdown = FALSE;
            }
            // If the deferred Off is set and the continue reasons have ended
            if ( ( APU_Diag.APU_Off_Requested ) && ( APU_Diag.APU_Off_Now ) )
            {
                bStopNow = TRUE;
            }
            // maxRunTmr is set in ST_APU_WAIT and is determined by the user via the 
            // menu system where the time is set and the MAXIMUM RUNTIME is enabled.
            if ( !AUTO && !timers.maxRunTmr && tkgMaxRunEnable )
            {
                tkgMaxRunTimedOut = TRUE;
                if ( bContinue )
                {
                    bStopLater = TRUE;
                }
                else
                {
                    bStopNow = TRUE;
                }
            }
            // If all the auto feature are off
            if ( AUTO && ( apu_on_reasons == 0 ) )
            {
                if ( bContinue )
                {
                    bStopLater = TRUE;
                }
                else
                {
                    bStopNow = TRUE;
                }
            }
            
            
            // If the APU controller signals the APU is off
            if ( !APU_ON_FLAG )
            {
                // Start timer and go to waiting state
                timers.apuOnTmr = 3;
                APU_STATE = ST_APU_STOPPING;
            }
            else if ( bStopNow )
            {
                AUTO = 0;
                timers.restartTmr = RESTART_TIME;
                timers.waitTmr = 50;
                APU_STATE = ST_APU_STOPPED;
            }
            else if ( bStopLater )
            {
                // Set the deferred Off request
                APU_Diag.APU_Off_Requested = TRUE;
                APU_Diag.APU_Off_Now = FALSE;
                
                // If needing to regen
                if ( DPF_Diag.bNeedToRegen )
                {
                    // If the regen command has not been sent
                    if ( ! DPF_Diag.bRegenCmdSent )
                    {
                        // Send the regen command
                        Add_Event( EV_APU_GOING_OFF );
                    }
                }
            }
            break;
        case ST_APU_STOPPING:
            apu_fnEngineCtrl( APU_eENGINE_ON );
            iconControl( ICON_APU_ON, ICON_BLINK_FAST );
            if ( ENGINE_FAULT )
            {
                APU_STATE = ST_APU_FAULT;
            }
            // apuOnTmr is set in ST_APU_RUN to 3 seconds. This allows three seconds to 
            // check for a fault or if the APU is not on. Otherwise ST_APU_RUN is 
            // resumed.
            else if(!APU_ON_FLAG && !timers.apuOnTmr)
            {
                timers.restartTmr = RESTART_TIME;
                timers.waitTmr = 50;
                APU_STATE = ST_APU_STOPPED;
            }
            else if(!timers.apuOnTmr)
            {
                APU_STATE = ST_APU_RUN;
            }
            break;
        case ST_APU_STOPPED:
            iconErase( ICON_APU_ON );
            DPF_fnTurnOffLoads();
            Abort_Regen();
            APU_Diag.APU_Off_Requested = FALSE;
            Reset_DPF_Flags();
            Clear_APU_Related_Messages();
#ifdef EXT_MEM
            if (( Datalog.Ext_Mem_Present ) && ( Datalog.Logging_On ))
            {
                Stop_Datalog();
                Record_APU_Status_External();
            }
#endif // EXT_MEM
            // Update data in J1939 APU Status message
            TxPgn_65280[3] &= ~(0x01); 
            // End of update data in J1939 APU Status message
            
            if(SHORE_POWER)
            {
                APU_STATE = ST_SHORE_POWER;
                timers.waitTmr = 50;
            }
            else
            {
                APU_STOPPED = 1;
            // waitTmr is set in ST_APU_WAIT, ST_APU_RUN and AT_APU_STOPPING to 1/2 
            // second to allow for shore power detection
                if(!timers.waitTmr)
                {
                    APU_STATE = ST_APU_OFF;
                    timers.waitTmr = 50;
                }
            }
            break;

        case ST_APU_FAULT:
            apu_fnEngineCtrl( APU_eENGINE_ON );
            AUTO = 0;
            iconErase( ICON_APU_ON );
            iconShow( ICON_APU_BAR );
            if (ENGINE_INTRLCK == FALSE)
                VWM_fnRequestView( VWM_eVIEW_CHECK_ENGINE_ALERT, BACKLIGHT_ON );
            if(ON_OFF_BTN)
            {
                DPF_Timers.MSG_Display = 0;
                Reset_DPF_Flags();
                tmgTMActive = FALSE;
                timers.restartTmr = RESTART_TIME;
                VWM_fnRemoveView( VWM_eVIEW_CHECK_ENGINE_ALERT );
                APU_STATE = ST_APU_OFF;
            }
            break;
            
        default:
            APU_STATE = ST_APU_OFF;
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: APU_fnLoadControl -->
       \brief Control loads for Regeneration

<!-- PURPOSE: -->
       This function removes or resumes APU load control for DPF regeneration.
       During regeneration all energy loads must be turned off; on the APU
       the alternator is an energy load.
<!-- PARAMETERS: -->
       @param[in]  bLoadsOff    TRUE to turn off loads, FALSE to resume loads.
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void APU_fnLoadControl( bool bLoadsOff )
{
    if ( bLoadsOff )
    {
        apu_fnEngineCtrl( APU_eENGINE_ALT_OFF );
        // Update data in J1939 APU Status message
        TxPgn_65280[3] &= ~(0x02); 
        // End of update data in J1939 APU Status message
    }
    else
    {
        apu_fnEngineCtrl( APU_eENGINE_ALT_ON );
        // Update data in J1939 APU Status message
        TxPgn_65280[3] |= 0x02; 
        // End of update data in J1939 APU Status message
    }
}
#ifdef EXT_MEM
void Record_APU_Status_External (void)
{
unsigned char Data[11];

    Record_Real_Time_And_Date (EXTERNAL);

    Data[0] = APU_RECORD_NEW;

    // Operating hours (total)
    Data[1] = APU_HOURS & 0x000000FF;
    Data[2] = ((APU_HOURS >> 8) & 0x000000FF);

    // Number of starter actuations
    Data [3] = APU_Start_Total & 0x00FF;
    Data[4] = ((APU_Start_Total >> 8) & 0x00FF);

    // Timer 1 setting
    nvfGetTime( NV_ID_TIMER1_TIME, &tkgTimer1.tkxTime, DEFAULT_TIMER1_DAY, DEFAULT_TIMER1_HOUR, DEFAULT_TIMER1_MINUTE );
    Data [5] = tkgTimer1.tkxTime.tkxDayOfWeek;
    Data [5] += ((tkgTimer1.tkxTime.tkxHours << 3) & 0xF8);
    Data [6] = tkgTimer1.tkxTime.tkxMinutes;

    // Timer 2 setting
    nvfGetTime( NV_ID_TIMER2_TIME, &tkgTimer2.tkxTime, DEFAULT_TIMER2_DAY, DEFAULT_TIMER2_HOUR, DEFAULT_TIMER2_MINUTE );
    Data [7] = tkgTimer2.tkxTime.tkxDayOfWeek;
    Data [7] += ((tkgTimer2.tkxTime.tkxHours << 3) & 0xF8);
    Data [8] = tkgTimer2.tkxTime.tkxMinutes;
#ifndef EXTRA_TIMERS
    // Temperature set point
    Data [9] = nvfGetTemperature(NV_ID_TM_TEMP, DEFAULT_TEMP_MON_TEMP);

    // Battery low set point
    Data [10]= nvfGetVoltage(NV_ID_BM_VOLTAGE, DEFAULT_BATT_MON_VOLTS);
#endif // EXTRA_TIMERS

#ifdef EXT_MEM
    // Write to external memory
    if ( EXM_fnLogData (Data, 10) )
        Increment_Sync_Insertion_Counter();
#endif // EXT_MEM
}

#endif // EXT_MEM

void APU_Critical_ShutDown (void)
{
    APU_Diag.Crtical_Shutdown = TRUE;
    APU_Diag.APU_Warmup_Timer = 0;
    DPF_Diag.Record &= ~GEN_LOW_VOLTAGE;

    // Do not control the APU engine and state machine if it is in a fault state
    // If APU is not in fault state
    if ( APU_STATE != ST_APU_FAULT )
    {
        // Turn off the APU engine
        apu_fnEngineCtrl( APU_eENGINE_OFF );
        APU_STATE = ST_APU_OFF;
    }
    timers.restartTmr = RESTART_TIME;
    Clear_APU_Related_Messages();

    if (APU_Diag.APU_On_Off_Flag != OFF)
    {
        APU_Diag.APU_On_Off_Flag = OFF;
        DPF_Timers.MSG_Display = 0;
        Reset_DPF_Flags();
    }

    Abort_Regen();
#ifdef EXT_MEM
    if ( ( Datalog.Ext_Mem_Present ) && ( Datalog.Logging_On ) )
    {
        Stop_Datalog();
        Record_APU_Status_External();
    }
#endif // EXT_MEM
    // Update data in J1939 APU Status message
    TxPgn_65280[3] &= ~(0x01); 
    // End of update data in J1939 APU Status message
}

static void Clear_APU_Related_Messages (void)
{
    VWM_fnRemoveView (VWM_eVIEW_APU_STARTING);
    VWM_fnRemoveView (VWM_eVIEW_ENGINE_WAIT);
    VWM_fnRemoveView (VWM_eVIEW_APU_RESTARTING);
}   

void APU_Critical_Shutdown_Can_Clear(void)
{
    if ((DPF_Diag.Record & (MISSING | EXHST_PRESS_HIGH | HEATER_FAIL | GEN_LOW_VOLTAGE | TWO_FAILURES))== FALSE)
        APU_Diag.Crtical_Shutdown = FALSE;
}

void Initialize_Diag (void)
{
#ifdef MODIFY1
		GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

		GPIO_ResetBits(GPIOD, GPIO_Pin_2);
		GPIO_ResetBits(GPIOD, GPIO_Pin_3);
		GPIO_ResetBits(GPIOD, GPIO_Pin_6);
#endif
    APU_Diag.Crtical_Shutdown = FALSE;
    APU_Diag.Noncritical_Shutdown = FALSE;
    APU_Diag.APU_On_Off_Flag = OFF;
    APU_Diag.APU_Off_Requested = FALSE;
    APU_Diag.APU_Off_Now = FALSE;
    APU_Diag.APU_Warmup_Timer = 0;
    APU_Diag.Emergency_Regen_Sent_Once = 0;
    engineFaultOilPrev = 0;
    engineFaultTempPrev = 0;
    engineFaultAltFbPrev = 0;
    mainBrakerTripped = 0;
}