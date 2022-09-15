/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file menu.c
       \brief Module containing routines for displaying and navigating menu tree

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

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/menu.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

 * 
 * RTC_80155 APU Master Password, March 5, 2020, Hannon
 *  mufPasswordDigits( ) - Modify to decode 4 digit password.
 *
 * RTC_75512 APU Driver control panel - increase password length to 4 digits, November 13, 2019, Hannon
 *  Use 4 Digit Passwords - Initial Carrier Revision.
 * 

          $Log: not supported by cvs2svn $
          Revision 1.7  2014/08/25 19:31:35  blepojevic
          Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

          Revision 1.6  2014/07/25 15:24:17  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.5  2014/06/27 17:35:28  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.4  2014/04/16 14:07:09  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.3  2013/09/19 13:07:44  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.2  2010/12/01 22:44:33  dharper
          Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
          This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
          This is because REV B was never checked in.
    
          Revision 1.1  2009/10/14 14:46:02  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
          Revision 1.30  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.29  2008-11-07 20:26:22  gfinlay
          *** empty log message ***

          Revision 1.28  2008-08-21 15:43:11  msalimi
          - Added feature to turn on the backlight when fault or important messages are being displayed.
          - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
          - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
          - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

          Revision 1.27  2008-06-12 17:21:28  msalimi
          Fixed a bug related to emergency and normal user initiated regens.

          Revision 1.26  2008-04-29 21:22:15  msalimi
          - Fixed some display issues.
          - Changed fault display to say # of #

          Revision 1.25  2008-04-24 15:35:44  msalimi
          - Added new FMIs
          - Added frozen filter functionality
          - Added display of SPN and FMI when displaying recorded faults.

          Revision 1.24  2008-04-10 16:45:17  msalimi
          - Made a distinction between normal and emergency regen.
          - Extreme pressure and pressure sensor faults will be detected during normal regen.
          - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
          - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
          - A new command will be sent to DPF when an emergency regen is initiated.
          - APU warmup reduced to 2 minutes (from 5).

          Revision 1.23  2008-04-02 19:50:11  msalimi
          - Fixed onboard memory reading bug.
          - There is no one hour recall of faults if APU is off.
          - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
          - DPF lost fault will not be detected if APU is off.

          Revision 1.22  2008-03-26 23:07:24  gfinlay
          Fixed mistakes in the function parameter descriptions (doxygen) for the mufShowVar and mufShowClockSet routines

          Revision 1.21  2008-03-26 21:27:50  msalimi
          Fixed side effects of recent changes to how APU warmup period is handled for regeneration.

          Revision 1.20  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.19  2008-03-11 19:16:03  msalimi
          -Added a code inquirey mechanism to editing APU hours menu. This code is internally generated based on the time and date and must also be entered by the user in order to access the menu.

          Revision 1.18  2008-02-25 19:42:36  msalimi
          Fixed OBM reading issues.

          Revision 1.17  2008-02-21 20:08:04  msalimi
          - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
          - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
          - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

          Revision 1.16  2008-02-06 16:29:56  msalimi
          - Corrected low voltage SPN. This was 5 and now is 4.

          Revision 1.15  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.14  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.13  2007-12-06 21:40:10  msalimi
          - Changed the message that delays the APU shutdown to also show the coolddown period.
          - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
          - Eliminated the "Heater Status" from DPF menu.

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

          Revision 1.11  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.10  2007-10-23 19:02:36  msalimi
          Corrected spelling error.

          Revision 1.9  2007-10-19 21:12:42  msalimi
          New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

          Revision 1.8  2007-10-18 20:18:40  msalimi
          Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

          Revision 1.7  2007-10-15 16:27:44  msalimi
          Added menu item to display ext. memory address.

          Revision 1.6  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.5  2007-09-21 20:37:16  msalimi
          Removed Diagnostic and SPM related menu items.

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:14:24  nallyn
          Added USE_COLDSTART to allow for standard production build with two timers and cold start function. Added commenting for Doxygen

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.18  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
    
          Revision 1.17  2007/02/27 01:20:17  nallyn
          Added 4 more timers for a total of 10 timers. The timers function differently
          than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
          timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
          depending on the temperture selected:
          18-20: High AC
          21-23: Low AC
          24: HVAC Off
          25-27: Low Heat
          28-30: High Heat
    
          Revision 1.16  2007/02/26 17:49:52  nallyn
          Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
    
          Revision 1.15  2007/02/23 18:11:28  nallyn
          Changed from 2 timers to 4 timers for testing purposes
    
          Revision 1.14  2006/04/05 23:11:14  nallyn
          Removed references to Proheat Gen4 and added ComfortPro
    
          Revision 1.13  2006/02/08 20:21:09  nallyn
          Changes based on code review
          Updated how the battery voltage is read and how the offset is calculated
    
          Revision 1.12  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include <stdio.h>              // for printf
#include "menu.h"               // Menu definitions
#include "main.h"               // Common definitions
#include "timers.h"             // Interface to software counter timers
#include "lcd_grp1.h"           // Interface to LCD
#include "fonts.h"              // Interface to LCD fonts
#include "rtclock.h"            // Interface to real-time clock
#include "timekeep.h"           // Interface for clock time keeping
#include "menufunctions.h"
#include "battmon.h"            // Interface to Battery Voltage Monitor
#include "coldstart.h"          // Interface to Cold Start Monitor
#include "tempmon.h"            // Interface to Temperature Monitor
#include "iconblinker.h"        // Interface to Icon definitions
#include "nvmem.h"              // Interface to Non-Volatile Memory storage
#include "syncserial.h"         // Interface to Syncronous Serial Port 
#include "hrmeter2.h"           // Interface to Hour Meter
#include "defaults.h"           // Default settings for individual customers
#include "ViewMan.h"            // Interface to using View Manager
#include "DPF.h"                // Interface to Diesel Particulate Filter manager
#include "Events.h"
#include "OnBoardNvmem.h"
#include "Datalog.h"
#include "delay18.h"
#include "apu.h"
#include "ice_pnl.h"

/* ---------- I n t e r n a l   D e f i n i t i o n s ---------- */

// Structure for setting the clock
typedef struct muzclockset
{
    char muxPm;
    char muxDayOfWeek;
    char muxHours;
    char muxMinutes;
    char muxSeconds;
    char muxDate;
    char muxMonth;
    char muxYear;
} MUZCLOCKSET;

enum clockfield
{
    MU_HOURS,
    MU_MINUTES,
    MU_AMPM
};

enum datefield
{
    MU_DAY,
    MU_MONTH,
    MU_YEAR
};

typedef enum mu_eVAR_TYPE
{
    MU_eVAR_UCHAR,
    MU_eVAR_UINT,
    MU_eVAR_U32INT,
    MU_eVAR_LAST_ENTRY
}MU_eVAR_TYPE;



typedef struct muzShowVar
{
    const char *pLabel;
    const char *pUnits;
    char  varType;
    void *pValue;
} MUZSHOWVAR;

#ifdef EXT_MEM
extern unsigned long u32CurrAddress_To_View;
#endif // EXT_MEM


/* ---------- I n t e r n a l   D a t a ---------- */

static enum menu_state muhMenuState = MENU_ST_INIT;
static bit muhStateChange;

static char muhNumDigits;
static char muhNextDigit;
static char muhFirstDigit;
static char muhDigitLine;
static char muhDigitSet[ 5 ];

#ifdef OBM
OBM_RECORD_DISPLAY OBM_Record_Display;

const char *month_ptr, *device, *flag;
static unsigned long spn;
static unsigned char month, day, year, hour, minute, second, fmi;
static unsigned char OBM_Data [10];

static unsigned char Decode_Data(unsigned char *OBM_Data, unsigned char *month, unsigned char *day, 
                            unsigned char *year, unsigned char *hour, unsigned char *minute, unsigned char *second,
                            unsigned long *spn, unsigned char *fmi, unsigned char *runTimeSinceLastStart);
#endif // OBM


MUZCLOCKSET muhClockSet;

enum menu_state last_menu_state = MENU_FIRST_FEATURE;
enum menu_state next_menu_state;

char set_choices[] =
{
    CHOOSE_OFF,
    CHOOSE_ON,
    CHOOSE_SET
};

char cold_choices[] =
{
    CHOOSE_OFF,
    CHOOSE_ON,
    CHOOSE_SET_TEMP,
    CHOOSE_SET_DURATION
};

char tempmon_choices[] =
{
    CHOOSE_OFF,
    CHOOSE_ON,
    CHOOSE_SET_TEMP,
    CHOOSE_SET_RANGE,
    CHOOSE_SET_FAN_SPEED,
    CHOOSE_SET_DURATION
};

char fanspeed_choices[] =
{
    CHOOSE_HIGH_ONLY,
    CHOOSE_LOW_HIGH
};

char timer_choices[] =
{
    CHOOSE_OFF,
    CHOOSE_ON,
    CHOOSE_SET_START,
    CHOOSE_SET_DURATION,
    CHOOSE_SET_TEMP
};

char clock_mode_choices[] =
{
    CHOOSE_MODE_24,
    CHOOSE_MODE_12
};

char yesno_choices[] =
{
    CHOOSE_NO,
    CHOOSE_YES
};

char day_choices[] =
{
    CHOOSE_SUNDAY,
    CHOOSE_MONDAY,
    CHOOSE_TUESDAY,
    CHOOSE_WEDNESDAY,
    CHOOSE_THURSDAY,
    CHOOSE_FRIDAY,
    CHOOSE_SATURDAY
};

char all_day_choices[] =
{
    CHOOSE_SUNDAY,
    CHOOSE_MONDAY,
    CHOOSE_TUESDAY,
    CHOOSE_WEDNESDAY,
    CHOOSE_THURSDAY,
    CHOOSE_FRIDAY,
    CHOOSE_SATURDAY,
    CHOOSE_WEEKDAYS,
    CHOOSE_WEEKENDS,
    CHOOSE_ALL_DAYS
};

char sysCfgChoices[] =
{
    CHOOSE_CHECK,
    CHOOSE_CONFIGURE,
    CHOOSE_RESET_FAULTS,
};

char sysCfgDeviceChoices[] =
{
    CHOOSE_DPF,
    CHOOSE_HEATERS,
};

char sysInstallChoices[] =
{
    CHOOSE_DISABLE,
};

char heatersInstallChoices[] =
{
    CHOOSE_DISABLE,
    CHOOSE_ENABLE,
};

char dpfChoices[] =
{
    CHOOSE_START_REGEN,
    CHOOSE_STATUS,
    CHOOSE_VARS
};

char faultsChoices[] =
{
    CHOOSE_VIEW_FAULTS,
};

bit mugClockMode24;
bit mugBlinkOn;
bit mugSetup;

/* ---------- F u n c t i o n s ---------- */

extern uint16 Calculate_Live_Code (void);

extern unsigned char TxPgn_65280[8];

static void mufClearMenu( void );

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufClearMenu -->
       \brief Clears the menu area

<!-- PURPOSE: -->
    This function erases the menu working area, lines 3 to 7
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void mufClearMenu( void )
{
    // Clear menu working area
    lcd_cla( 0, 131, 3, 7 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufChangeMenu -->
       \brief Changes the menu tree position 

<!-- PURPOSE: -->
       \brief Changes the menu tree position or node to the top level feature state,
       the menu feature substate, or else the administrative state. By clearing the menu,
       and invalidating the currently displayed state, it will cause the new state to be drawn.
<!-- PARAMETERS: -->
       @param[in]  newState The enumerated MENU_STATE value which identifies the target feature or subfeature state to set
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufChangeMenu( MENU_STATE newState )
{
    timers.menuTmr = MENU_PAUSE;
    muhMenuState = newState;
    muhStateChange = TRUE;
    mufClearMenu();
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufShowDecimalHours -->
       \brief Display the number of hours in decimal and turn on arrows

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  number  The decimal hours value to display
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufShowDecimalHours( char number )
{
    fncShowArrows( 21, 106, 4 );
    lcd_position( 35, 4, large );
    printf( "%2d.%d HRS", number / 10, number % 10 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufShowDecimalVolts -->
       \brief Display the voltage in decimal and turn on arrows

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  number  The decimal voltage value to display
<!-- RETURNS: -->
      \return Desc of return value

*/
/******************************************************************************/
void mufShowDecimalVolts( char number )
{
    fncShowArrows( 21, 106, 4 );
    lcd_position( 42, 4, large );
    printf( "%2d.%d V", number / 10, number % 10 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufShowTemperature -->
       \brief Display the temperature in celsius and fahrenheit and turn on arrows

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  celsius  The celsius temperature value to display
<!-- RETURNS: -->
      \return Desc of return value

*/
/******************************************************************************/
void mufShowTemperature( signed char celsius )
{
    signed char fahrenheit;
    
    // If celsius below 0
    if ( celsius < 0 )
        // Fahrenheit below 0 round down ( -20C -> -4F )
        fahrenheit = 32 + ( ( ( celsius * 9 ) - 2 ) / 5 );
    else if ( muhMenuState == MENU_ST_TEMPMON_SET_RANGE)
        // Fahrenheit unit comparing celsius unit )
        fahrenheit =  ( ( celsius * 9 ) + 2 ) / 5 ;
    else
        // Fahrenheit rounded upwards ( 21C -> 70F )
        fahrenheit = 32 + ( ( ( celsius * 9 ) + 2 ) / 5 );
    
    fncShowArrows( 21, 102, 4 );
    lcd_position( 28, 4, large );
    printf( "%3dC ", celsius );
    printf( "%2dF", fahrenheit );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufCenterLargeText -->
       \brief Calculate the length or extent of string and display it centered 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  label   String to display
       @param[in]  page    page (Y coordinate) for line on which to display string
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufCenterLargeText( const char *label, char page )
{
    char len;
    char textX;
    
    // Get string length
    for ( len = 0; label[ len ] != 0; len++ )
        ;
        
    // Convert length to character widths/2
    len *= 8/2;

    // Position text
    textX = 66 - len;         // 66 is taken for center character
    
    lcd_position( textX, page, large );
    printf( "%s", label );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLargePrompt -->
       \brief Draws a centered string label on the prompt row

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  prompt  The string prompt to display
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLargePrompt( const char *prompt )
{
    mufCenterLargeText( prompt, 4 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufFeatureStr1 -->
       \brief Lookup the first line of a menu string corresponding to a menu feature state 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Pointer to 1st line of static menu string corresponding to feature input

*/
/******************************************************************************/
const char * mufFeatureStr1( char feature )
{
    switch( feature )
    {
#ifndef EXTRA_TIMERS
        case MENU_ST_BATTMON:       return "BATTERY";
#endif // EXTRA_TIMERS

#ifdef USE_COLDSTART        
        case MENU_ST_COLDSTART:     return "TEMP START";
#endif

#ifndef EXTRA_TIMERS
        case MENU_ST_TEMPMON:       return "COMFORT";
        case MENU_ST_SETTEMP:       return "SET TEMP";
#endif // EXTRA_TIMERS
        case MENU_ST_MAXRUN:        return "MAXIMUM";
        case MENU_ST_ACCESS:        return "PASSWORD";
        case MENU_ST_CHANGE_PASS:   return "SET/CHANGE";
        case MENU_ST_TIMER1:        return "TIMER 1";
        case MENU_ST_TIMER2:        return "TIMER 2";
#ifndef USE_COLDSTART
        case MENU_ST_TIMER3:        return "TIMER 3";
        case MENU_ST_TIMER4:        return "TIMER 4";
#endif // USE_COLDSTART

#ifdef EXTRA_TIMERS
        case MENU_ST_TIMER5:        return "TIMER 5";
        case MENU_ST_TIMER6:        return "TIMER 6";
        case MENU_ST_TIMER7:        return "TIMER 7";
        case MENU_ST_TIMER8:        return "TIMER 8";
        case MENU_ST_TIMER9:        return "TIMER 9";
        case MENU_ST_TIMER10:       return "TIMER 10";
#endif // EXTRA_TIMERS
        case MENU_ST_SETUP:         return "SET UP";
        case MENU_ST_CLOCK:         return "TIME & DATE";
#ifndef EXTRA_TIMERS
        case MENU_ST_CALIBRATE:     return "CALIBRATE";
#endif // EXTRA_TIMERS
        case MENU_ST_SYS_CFG:       return "SYSTEM";
        case MENU_ST_DPF:           return "DPF";
#ifdef OBM
        case MENU_ST_FAULTS:        return "FAULTS";
#endif // OBM
        default:                    return "BOGUS";
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufFeatureStr2 -->
       \brief Lookup the second line of a menu string corresponding to a menu feature state 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Pointer to 2nd line of static menu string corresponding to feature input

*/
/******************************************************************************/
const char * mufFeatureStr2( char feature )
{
    switch( feature )
    {
#ifndef EXTRA_TIMERS
        case MENU_ST_BATTMON:       // fall through to "monitor"
        case MENU_ST_TEMPMON:       return "MONITOR";
#endif // EXTRA_TIMERS        
        case MENU_ST_MAXRUN:        return "RUN TIME";
        case MENU_ST_ACCESS:        return "PROTECT";
        case MENU_ST_CHANGE_PASS:   return "PASSWORD";
        case MENU_ST_SYS_CFG:       return "CONFIG";
        case MENU_ST_DPF:           return "MANAGER";
#ifndef EXTRA_TIMERS
        case MENU_ST_CALIBRATE:     return "VOLTAGE";
#endif // EXTRA_TIMERS
#ifdef USE_COLDSTART        
        case MENU_ST_COLDSTART:     // fall through to NULL
#endif
        case MENU_ST_TIMER1:        // fall through to NULL
        case MENU_ST_TIMER2:        // fall through to NULL
#ifndef USE_COLDSTART
        case MENU_ST_TIMER3:        // fall through to NULL
        case MENU_ST_TIMER4:        // fall through to NULL
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
        case MENU_ST_TIMER5:        // fall through to NULL
        case MENU_ST_TIMER6:        // fall through to NULL
        case MENU_ST_TIMER7:        // fall through to NULL
        case MENU_ST_TIMER8:        // fall through to NULL
        case MENU_ST_TIMER9:        // fall through to NULL
        case MENU_ST_TIMER10:       // fall through to NULL
#endif // EXTRA_TIMERS
        case MENU_ST_SETUP:         // fall through to NULL
        case MENU_ST_CLOCK:         // fall through to NULL
#ifdef OBM
        case MENU_ST_FAULTS: // fall through to NULL
#endif // OBM

        default:                    return NULL;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufFeatureString -->
       \brief Lookup the full menu string corresponding to a menu feature state 

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Pointer to full static menu string corresponding to feature input

*/
/******************************************************************************/
const char * mufFeatureString( char feature )
{
    switch( feature )
    {
#ifndef EXTRA_TIMERS        
        case MENU_ST_BATTMON:       return "BATTERY MONITOR";       
        case MENU_ST_TEMPMON:       return "COMFORT MONITOR";
#endif // EXTRA_TIMERS        
        case MENU_ST_DPF:           return "DPF";
        case MENU_ST_MAXRUN:        return "MAXIMUM RUN TIME";
        case MENU_ST_ACCESS:        return "PASSWORD PROTECT";
        case MENU_ST_CHANGE_PASS:   return "CHANGE PASSWORD";
        default: return mufFeatureStr1( feature );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLabelFeature -->
       \brief Display the menu string corresponding to a menu state feature
       on one line or two as appropriate

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLabelFeature( char feature )
{
    const char *str1;
    const char *str2;

    fncShowArrows( 11, 112, 4 );
    
    // Get string for the selected choice
    str1 = mufFeatureStr1( feature );
    str2 = mufFeatureStr2( feature );
    
    if ( str2 != NULL )
    {
        // Center the large text label
        mufCenterLargeText( str1, 3 );
        mufCenterLargeText( str2, 5 );
    }
    else
    {
        // Center the large text label
        mufCenterLargeText( str1, 4 );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLabelFeatureLarge -->
       \brief Display the menu string corresponding to a menu state feature in large on one line

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLabelFeatureLarge( char feature )
{
    const char *label;
    
    // Get string for the selected choice
    label = mufFeatureString( feature );
    
    // Center the large text label
    mufLargePrompt( label );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLabelFeatureSmall -->
       \brief Display the menu string corresponding to a menu state feature in small on one line

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  feature enumerated menu feature state 
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLabelFeatureSmall( char feature )
{
    lcd_position( 0, 3, small );
    printf( "%s", mufFeatureString( feature ) );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLabelFeatureSet -->
       \brief Display menu feature item with word SET and optional suffix

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated feature state
       @param[in]  str     optional suffix 
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLabelFeatureSet( char feature, const char *str )
{
    lcd_position( 0, 3, small );
    printf( "%s:SET %s", mufFeatureString( feature ), str  );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufLabelFeatureNoSet -->
       \brief Display menu feature item with optional suffix but no word SET

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature enumerated feature state
       @param[in]  str     optional suffix
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufLabelFeatureNoSet( char feature, const char *str )
{
    lcd_position( 0, 3, small );
    printf( "%s:%s", mufFeatureString( feature ), str  );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufChoiceString -->
       \brief Map enumerated item to choice string

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  chooser Enumerated constant to map to choice string
<!-- RETURNS: -->
      \return Pointer to constant string representing choice item

*/
/******************************************************************************/
const char * mufChoiceString( char chooser )
{
    switch( chooser )
    {
        case CHOOSE_OFF:            return "OFF";
        case CHOOSE_ON:             return "ON";
        case CHOOSE_SET:            return "SET";
        case CHOOSE_SET_TEMP:       return "SET TEMP";
        case CHOOSE_SET_RANGE:      return "SET RANGE";
        case CHOOSE_SET_FAN_SPEED:  return "SET FAN SPEED";
        case CHOOSE_SET_START:      return "SET START";
        case CHOOSE_SET_DURATION:   return "SET DURATION";
        case CHOOSE_MODE_24:        return "24 HOUR";
        case CHOOSE_MODE_12:        return "12 HOUR";
        case CHOOSE_NO:             return "NO";
        case CHOOSE_YES:            return "YES";
        case CHOOSE_SUNDAY:         return "SUN";
        case CHOOSE_MONDAY:         return "MON";
        case CHOOSE_TUESDAY:        return "TUE" ;
        case CHOOSE_WEDNESDAY:      return "WED";
        case CHOOSE_THURSDAY:       return "THU";
        case CHOOSE_FRIDAY:         return "FRI";
        case CHOOSE_SATURDAY:       return "SAT";
        case CHOOSE_WEEKDAYS:       return "WEEKDAYS";
        case CHOOSE_WEEKENDS:       return "WEEKENDS";
        case CHOOSE_ALL_DAYS:       return "ALL DAYS";
        case CHOOSE_START_REGEN:    if (APU_ON_FLAG)
                                        return "START REGEN";
                                    else
                                        return "EMERGENCY REGEN";
        case CHOOSE_STATUS:         return "STATUS";
        case CHOOSE_VARS:           return "VARIABLES";
        case CHOOSE_CHECK:          return "CHECK";
        case CHOOSE_CONFIGURE:      return "CONFIGURE";
        case CHOOSE_DPF:            return "DPF";
        case CHOOSE_HEATERS:        return "HEATER";
        case CHOOSE_DISABLE:      return "DISABLE";
        case CHOOSE_ENABLE:        return "ENABLE";
        case CHOOSE_VIEW_FAULTS:    return "VIEW";
        case CHOOSE_RESET_FAULTS:   return "RESET FAULTS";
        case CHOOSE_HIGH_ONLY:      return "SPEED HIGH";
        case CHOOSE_LOW_HIGH:       return "SPEED LOW-HIGH";
        default:                    return "NONE";
    }
} 

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufChoose -->
       \brief Choose from array of enumerated constants mapped to choice strings

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  previous Previous choice
       @param[in]  listSize Number of items in array
       @param[in]  choiceList Pointer to array list of enumerated choice items
       @param[in]  control  Override for Y offset (page)   
<!-- RETURNS: -->
      \return Index of item chosen

*/
/******************************************************************************/
char mufChoose( char previous, char listSize, char *choiceList, char control )
{
    const char *label;
    int listIndex;
    char len;
    char leftArrowX;
    char rightArrowX;
    char textX;
    char textY;
    
    if ( control == 6 )
        textY = 6;
    else
        textY = 4;
    
    // Find previous choice in list
    for ( listIndex = 0; listIndex < listSize; listIndex++ )
    {
        if ( choiceList[ listIndex ] == previous )
        {
            break;
        }
    }
    
    // If choice is not found default to first choice in list
    if ( listIndex == listSize )
    {
        listIndex = 0;
    }
    
    // Get string for the selected choice
    label = mufChoiceString( choiceList[ listIndex ] );
    
    // Get string length
    for ( len = 0; label[ len ] != 0; len++ )
        ;
        
    // Convert length to length in character widths/2
    len *= 8/2;

    if ( len < 50 )
    {
        // Position text and arrows
        textX = 66 - len;               // 66 is taken for center character
        leftArrowX = 50 - len;          // 50 because it looks good
        rightArrowX = 76 + len;         // 76 because it looks good

        // Set minimum separation between arrows
        if ( leftArrowX > 36 )
        {
            leftArrowX = 36;
            rightArrowX = 88;
        }

        // Show arrows
        if ( listSize > 1 )    // show arrows if list is more than 1
            fncShowArrows( leftArrowX, rightArrowX, textY );
    }
    else if ( len <= 60 )
    {
        // Position text and arrows
        textX = 66 - len;               // 66 is taken for center character
        leftArrowX = 0;
        rightArrowX = 125;
    
        // Show arrows
        if ( listSize > 1 )    // show arrows if list is more than 1
            fncShowArrows( leftArrowX, rightArrowX, textY );
    }
    else
    {
        textX = 0;
        // No arrows
    }
    // Show text
    lcd_position( textX, textY, large );
    printf( "%s", label );

    // If right button move up list, wrapping top to bottom
    if ( RIGHT_BTN && !timers.menu2Tmr )
    {
        lcd_cla( 0, 131, textY, textY+1 );
        timers.menu2Tmr = 5;        //menuMsTimer(3);
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        if ( ++listIndex == listSize )
            listIndex = 0;
    }
    // If left button move down list, wrapping bottom to top
    else if ( LEFT_BTN && !timers.menu2Tmr )
    {
        lcd_cla( 0, 131, textY, textY+1 );
        timers.menu2Tmr = 5;        //menuMsTimer(3);
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        if ( --listIndex < 0 )
            listIndex = listSize - 1;
    }
    
    return ( choiceList[ listIndex ] );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufMonthString -->
       \brief Return string for month selector

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  month    month starting at 1
<!-- RETURNS: -->
      \return Pointer to constant string for month

*/
/******************************************************************************/
const char * mufMonthString( char month )
{
    switch ( month )
    {
        case 1:     return "JAN";
        case 2:     return "FEB";
        case 3:     return "MAR";
        case 4:     return "APR";
        case 5:     return "MAY";
        case 6:     return "JUN";
        case 7:     return "JUL";
        case 8:     return "AUG";
        case 9:     return "SEP";
        case 10:    return "OCT";
        case 11:    return "NOV";
        case 12:    return "DEC";
        default:    return "UNK";   // unknown
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufShowVar -->
       \brief Shows DPF Status variables from DPF Info structure

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufShowVar(void)
{
    unsigned char u8Value;
    unsigned int u16Value;
#ifdef EXT_MEM
    unsigned long u32Value;
#endif // EXT_MEM
    char measure[ 32 ];

    // Exhaust pressure    
    lcd_position( 0, 4, small );
    u8Value = DPF_Info.EXHST_Press_To_View;
    if ( u8Value == NO_DATA )
    {
        sprintf( measure, "---" );
    }
    else
    {
        sprintf( measure, "%d", u8Value );
    }
    printf( "EXHST P %4s MBAR", measure);

    // Exhaust temperature
    lcd_position( 0, 5, small );
    u16Value = DPF_Info.EXHST_Temp_To_View;
    if ( u16Value == NO_DATA_WORD )
    {
        sprintf( measure, "---" );
    }
    else
    {
        sprintf( measure, "%d", u16Value );
    }
    printf( "EXHST T %4s C",measure);
    
#ifdef EXT_MEM
    // External memory current address pointer
    lcd_position( 0, 6, small );
    u32Value = u32CurrAddress_To_View;
    sprintf( measure, "%lu", u32Value );
    printf( "MEMORY %4s",measure);
#endif // EXT_MEM
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufFeatureScroll -->
       \brief Scroll to a menu item in appropriate direction 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  next   next menu state if hit MENU key
       @param[in]  enabled Access to Next menu state - TRUE if enabled
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufFeatureScroll( enum menu_state next, char enabled )
{
    // Draw current feature
    mufLabelFeature( muhMenuState );
    
    if ( RIGHT_BTN && !timers.menu2Tmr )
    {
        timers.menuTmr = MENU_PAUSE;
        timers.menu2Tmr = 5;
        muhMenuState++;
        if ( ( muhMenuState == MENU_ST_ACCESS ) && !user_password )
            muhMenuState++;
            if ((muhMenuState == MENU_ST_SETTEMP) && ((tmgTMEnable == OFF ) || !user_password))
            {
                muhMenuState++;                             // skip driver set temperature if no comfort monitor
            }
        if ( muhMenuState > MENU_LAST_FEATURE )
            muhMenuState = MENU_FIRST_FEATURE;
        mufClearMenu();
    }
    else if ( LEFT_BTN && !timers.menu2Tmr )
    {
        timers.menuTmr = MENU_PAUSE;
        timers.menu2Tmr = 5;
        muhMenuState--;
        if ( ( muhMenuState == MENU_ST_ACCESS ) && !user_password )
            muhMenuState--;
        if ((muhMenuState == MENU_ST_SETTEMP) && ((tmgTMEnable == OFF ) || !user_password))
            {
                muhMenuState--;                             // skip driver set temperature if no comfort monitor
            }
        if ( muhMenuState < MENU_FIRST_FEATURE )
            muhMenuState = MENU_LAST_FEATURE;
        mufClearMenu();
    }
    else if ( MENU_BTN )
    {
        last_menu_state = muhMenuState;
        timers.menuTmr = MENU_PAUSE;
        
        if ( enabled || ( user_password == 0 ) )
        {
            muhMenuState = next;
        }
        else
        {
            next_menu_state = next;
            muhMenuState = MENU_ST_PASSWORD;
        }
    
        // Call to mufChangeMenu to set the menu change variables
        mufChangeMenu( muhMenuState );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufSetAccess -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  feature   Desc of Parm1
       @param[in]  pControl   Desc of Parm2
       @param[out] nv_id   Desc of Parm3
<!-- RETURNS: -->
      \return Feature state enumeration for selected item

*/
/******************************************************************************/
char mufSetAccess( char feature, char *pControl, NV_ID nv_id )
{
    static char choice;
    
    mufLabelFeatureLarge( feature );
    if ( choice == NO_CHOICE )
        choice = ( *pControl ? CHOOSE_NO : CHOOSE_YES );
    choice = mufChoose( choice,
                     DIM( yesno_choices ),
                     yesno_choices,
                     6 );
    if (MENU_BTN)
    {
        *pControl = ( choice == CHOOSE_NO );
        (void)nvfPutNumber( nv_id, *pControl );
        timers.menuTmr = MENU_PAUSE;
        mufClearMenu();
        choice = NO_CHOICE;
        feature++;
    }
    
    return feature;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufMakeClockSet -->
       \brief Set clock time to specified day/hours/minutes

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  local_day     Day of week to set
       @param[in]  local_hours   Hours of day to set
       @param[out] local_minutes Minutes of day to set
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufMakeClockSet( char local_day, char local_hours, char local_minutes )
{
    char pm;
    
    muhClockSet.muxDayOfWeek = local_day;
    muhClockSet.muxMinutes = local_minutes;
    
    if ( mugClockMode24 )
    {
        // Time is kept in 24 hour clock
        muhClockSet.muxHours = local_hours;
        pm = FALSE;
    }
    else
    {
        if ( local_hours > 11 )
        {
            local_hours -= 12;
            pm = TRUE;
        }
        else
        {
            pm = FALSE;
        }
    
        // 0:00 becomes 12:00       
        if ( local_hours == 0 )
        {
            local_hours = 12;
        }
    }
    
    muhClockSet.muxHours = local_hours;
    muhClockSet.muxPm = pm;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufMakeDateSet -->
       \brief Set clock date to specified date/month/year

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  date     Date of month to set
       @param[in]  month    Month to set
       @param[out] year     Year to set
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufMakeDateSet( char date, char month, char year )
{
    muhClockSet.muxDate = date;
    muhClockSet.muxMonth = month;
    muhClockSet.muxYear = year;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufSetTimeField -->
       \brief Set the value of the corresponding date/time field

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  field   Field to Set: MU_HOURS, MU_MINUTES or MU_AMPM
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufSetTimeField( char field )
{
    char max;
    char min;
    const char *ampm;
    char xpos;
    
    max = 12;
    min = 1;
    xpos = 36;
    if ( mugClockMode24 )
    {
       // No AM or PM
       ampm = "";
       max = 23;
       min = 0;
       xpos = 42;
    }
    else if ( muhClockSet.muxPm )
    {
        ampm = "PM";
    }
    else
    {
        ampm = "AM";
    }
    
    lcd_position( xpos, 4, large );
    if ( mugBlinkOn )
    {
        printf( "%02d:%02d %s", muhClockSet.muxHours,
                             muhClockSet.muxMinutes,
                             ampm );
    }
    else
    {
        switch ( field )
        {
            case MU_HOURS:
                printf( "  :%02d %s",
                            muhClockSet.muxMinutes,
                            ampm );
                break;
                
            case MU_MINUTES:
                printf( "%02d:   %s",
                            muhClockSet.muxHours,
                            ampm );
                break;
                
           case MU_AMPM:
                printf( "%02d:%02d   ",
                            muhClockSet.muxHours,
                            muhClockSet.muxMinutes );
                break;
        }
    }
    
    switch ( field )
    {
        case MU_HOURS:
            muhClockSet.muxHours =
                    change_digit( muhClockSet.muxHours, 1, max, min );
            break;
            
        case MU_MINUTES:
            muhClockSet.muxMinutes =
                    change_digit( muhClockSet.muxMinutes, 1, 59, 0 );
            
            break;
            
        case MU_AMPM:
            muhClockSet.muxPm =
                    change_digit( muhClockSet.muxPm, 1, 1, 0 );
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufSetDateField -->
       \brief Set the value of the corresponding date field

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  field   Field to Set: MU_DAY, MU_MONTH or MU_YEAR
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufSetDateField( char field )
{
    const char *pMonth;
    char maxday;
    
    pMonth = mufMonthString( muhClockSet.muxMonth );
    
    lcd_position( 22, 4, large );
    if ( mugBlinkOn )
    {
        printf( "20%02d %s %02d", 
                muhClockSet.muxYear,
                pMonth,
                muhClockSet.muxDate );
    }
    else
    {
        switch ( field )
        {
            case MU_DAY:
                printf( "20%02d %s   ",
                        muhClockSet.muxYear,
                        pMonth );
                break;
                
            case MU_MONTH:
                printf( "20%02d     %02d",
                        muhClockSet.muxYear,
                        muhClockSet.muxDate );
                break;
                
           case MU_YEAR:
                printf( "     %s %02d",
                        pMonth,
                        muhClockSet.muxDate );
                break;
       
           default:
                break;
        }
    }
    
    switch ( field )
    {
        case MU_DAY:
            switch ( muhClockSet.muxMonth )
            {
                case 2:     // february
                    // If is is a leap year
                    if ( ( muhClockSet.muxYear % 4 ) == 0 )
                    {
                        maxday = 29;   
                    }
                    else
                    {
                        maxday = 28;
                    }
                    break;
                case 4:     // april
                case 6:     // june
                case 9:     // september
                case 11:    // november
                    maxday = 30;
                    break;
                default:
                    maxday = 31;
                    break;
            }
            if ( muhClockSet.muxDate > maxday )
            {
                muhClockSet.muxDate = maxday;
            }
                
            muhClockSet.muxDate =
                    change_digit( muhClockSet.muxDate, 1, maxday, 1 );
            break;
            
        case MU_MONTH:
            muhClockSet.muxMonth =
                    change_digit( muhClockSet.muxMonth, 1, 12, 1 );
            
            break;
            
        case MU_YEAR:
            muhClockSet.muxYear =
                    change_digit( muhClockSet.muxYear, 1, 99, 1 );
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufAmPmAdjust -->
       \brief Adjust hours if in 12-hour clock instead of 24-hour clock

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void mufAmPmAdjust( void )
{
    // If we are in 12 hour mode
    if ( !mugClockMode24 )
    {
        // 12:00 AM becomes 00:00, 12:00 PM becomes 12:00
        if ( muhClockSet.muxHours == 12 )
            muhClockSet.muxHours = 0;
    
        // PM adds 12 hours to clock
        if ( muhClockSet.muxPm )
            muhClockSet.muxHours += 12;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: mufPasswordDigits -->
       \brief Return the password

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
int mufPasswordDigits( void )
{
    int val;

   val = (int) muhDigitSet[ 0 ] * 1000
        + muhDigitSet[ 1 ] * 100
        + muhDigitSet[ 2 ] * 10
        + muhDigitSet[ 3 ];        
         
    return val;
}

long mufLongDigits( void )
{
    long val;
    int i;
    
    for ( i = 0, val = 0; i < 5; i++ )
    {
        val = val * 10 + muhDigitSet[ i ];
    }
        
   return val;
}

void mufStartDigits( char nDigits, char xPos, char yPos )
{
    muhNumDigits = nDigits;
    muhNextDigit = 0;
    muhDigitSet[ 0 ] = 0;
    muhDigitSet[ 1 ] = 0;
    muhDigitSet[ 2 ] = 0;
    muhDigitSet[ 3 ] = 0;
    muhDigitSet[ 4 ] = 0;
    muhFirstDigit = xPos;
    muhDigitLine = yPos;
}

char mufGetDigits( void )
{
    char xPos;
    char digit;
    char i;
    
    xPos = muhFirstDigit;
    
    for ( i = 0; i < muhNumDigits; i++ )
    {
        digit = muhDigitSet[ i ];
        if ( i == muhNextDigit )
        {
            blink_digit( xPos, muhDigitLine, digit );
            muhDigitSet[ i ] = change_digit( digit, 1, 9, 0 );
        }
        else
        {
            lcd_putnum( xPos, muhDigitLine, digit );
        }
        
        xPos += 8;
    }
    
    if ( MENU_BTN )
    {
        timers.menuTmr = MENU_PAUSE;
        muhNextDigit++;
        if ( muhNextDigit >= muhNumDigits )
        {
            return TRUE;
        }
    }
    return FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: menus -->
       \brief Main function to navigate the menu structure of system - called by main

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void
*/
/******************************************************************************/
void menus( void )
{
    static char feature;
    static char choice;
    static char access;
    static char scratch;
    static char signedScratch;
    static int newPass;
    static TKZREALTIMER *pRealTimer;
    static TKZNVIDS const *pRtNvIds;
    int pword;
    bool bUpdate;

    unsigned char runTimeSinceLastStartHours;
    unsigned char runTimeSinceLastStartMinutes;

		TM_CELSIUS tmcMax;	//? Add
		TM_CELSIUS tmcMin;	//? Add
	
    bUpdate = FALSE;
    
    // If menu is initializing
    if ( muhMenuState == MENU_ST_INIT )
    {
        // If the setup phase has never been run
        if ( ! mugSetup )
        {
            // Start the setup feature in the set clock mode state
            feature = MENU_ST_SETUP;
            mufChangeMenu( MENU_ST_CLOCK_MODE );
        }
        else
        {
            // Start in the Off state
            feature = MENU_ST_OFF;
            mufChangeMenu( MENU_ST_OFF );
        }
    }
    
    // If blink timer has expired
    if ( !timers.blinkTmr )
    {
        // Reload timer, short interval if blink is on, long time if off
        timers.blinkTmr = ( mugBlinkOn ? 10 : 25 );
        bUpdate = TRUE;
        
        // Toggle blink state
        mugBlinkOn = !mugBlinkOn;
    }
    
    // Do not change to Off state if setup phase is not complete
    if ( mugSetup )
    {    
        // Check for exit conditions
        if ( BRESET_BTN || !timers.menuTmr )
        {
            // If not in off state
            if ( muhMenuState != MENU_ST_OFF )
            {
                // Change to off state and clear menu from screen
                mufChangeMenu( MENU_ST_OFF );
            }
        }
    }
    
    // If menu scan timer has expired
    if ( !timers.menuScanTmr )
    {
        // Update menu data items at least 10 times a second (100 milliseconds)
        timers.menuScanTmr = 10;
        bUpdate = TRUE;
    }
    
    // If a button used in the menu system is active
    if ( MENU_BTN || RIGHT_BTN || LEFT_BTN || BRESET_BTN || MENU_AND_ESC_BTN )
    {
        // Need to run the menu state machine
        bUpdate = TRUE;
    }
    
    // If no need to update the menu
    if ( bUpdate == FALSE )
    {
        // Do not run the menu state machine
        return;
    }
        
    switch ( muhMenuState )
    {
        case MENU_ST_OFF:
            if ( muhStateChange )
            {
                muhStateChange = FALSE;
                VWM_fnRemoveView( VWM_eVIEW_MENU );
            }
    
            // Check buttons for this version thing
            if ( MENU_AND_ESC_BTN )
            {
                VWM_fnRequestView( VWM_eVIEW_VERSION, BACKLIGHT_ON );
            }
            else if ( MENU_BTN || BRESET_BTN )
            {
                VWM_fnRemoveView( VWM_eVIEW_VERSION );
                
                // If MENU button is hit
                if ( MENU_BTN )
                {
                    // Put menu back in view
                    VWM_fnRequestView( VWM_eVIEW_MENU, BACKLIGHT_ON );
                    
                    // Clear menu area and return to last used feature menu
                    mufChangeMenu( last_menu_state );
                }
            }
            break;
            
        // Start of FEATURE selection
#ifndef EXTRA_TIMERS        
        case MENU_ST_BATTMON:
            feature = MENU_ST_BATTMON;
            mufFeatureScroll( MENU_ST_BATMON_SELECT, bmgBMAccess );
            break;
#endif // EXTRA_TIMERS            
            
#ifdef USE_COLDSTART        
        case MENU_ST_COLDSTART:
            feature = MENU_ST_COLDSTART;
            mufFeatureScroll( MENU_ST_CSTART_SELECT, tCOLD_START.ucAccess );
            break;
#endif
             
#ifndef EXTRA_TIMERS             
        case MENU_ST_TEMPMON:
            feature = MENU_ST_TEMPMON;
            mufFeatureScroll( MENU_ST_TEMPMON_SELECT, tmgTMAccess );
            break;
#endif // EXTRA_TIMERS            

        case MENU_ST_SETTEMP:
            feature = MENU_ST_SETTEMP;
            mufFeatureScroll( MENU_ST_TEMP_SELECT, TRUE );
            break;
            
        case MENU_ST_MAXRUN:
            feature = MENU_ST_MAXRUN;
            mufFeatureScroll( MENU_ST_MAXRUN_TIME, tkgMaxRunAccess );
            break;
            
        case MENU_ST_ACCESS:
            feature = MENU_ST_ACCESS;
            mufFeatureScroll( MENU_ST_ACCESS_SELECT, FALSE );
            break;
            
        case MENU_ST_CHANGE_PASS:
            feature = MENU_ST_CHANGE_PASS;
            mufFeatureScroll( MENU_ST_NEW_PASSWORD, FALSE );
            break;
            
        case MENU_ST_CLOCK:
            feature = MENU_ST_CLOCK;
            mufFeatureScroll( MENU_ST_CLOCK_MODE, tkgClockAccess );
            break;
#ifdef OBM
        case MENU_ST_FAULTS:
            feature = MENU_ST_FAULTS;
            mufFeatureScroll( MENU_ST_FAULTS_SELECT, ON );
            OBM_Record_Display.record_index = 1;
            OBM_Record_Display.read_now = TRUE;
            OBM_Record_Display.record_ready = FALSE;
            month_ptr = "BOGUS";    // This is to prevent display of data if nothing is read yet
        break;
#endif // OBM
            
#ifndef EXTRA_TIMERS            
        case MENU_ST_CALIBRATE:
            feature = MENU_ST_CALIBRATE;
            mufFeatureScroll( MENU_ST_ADAPT_VOLTAGE, bmgVoltAccess );
            break;
#endif

        case MENU_ST_SYS_CFG:
            feature = MENU_ST_SYS_CFG;
            mufFeatureScroll( MENU_ST_SYS_CFG_SELECT, SYS_u8SysCfgAccess );
            break;

        case MENU_ST_DPF:
            feature = MENU_ST_DPF;
            mufFeatureScroll( MENU_ST_DPF_SELECT, DPF_u8DpfAccess );
            break;
            
        case MENU_ST_TIMER1:
            feature = MENU_ST_TIMER1;
            pRealTimer = &tkgTimer1;
            pRtNvIds = &tkgTimer1NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer1.tkxAccess );
            break;
            
        case MENU_ST_TIMER2:
            feature = MENU_ST_TIMER2;
            pRealTimer = &tkgTimer2;
            pRtNvIds = &tkgTimer2NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer2.tkxAccess );
            break;
#ifndef USE_COLDSTART            
        case MENU_ST_TIMER3:
            feature = MENU_ST_TIMER3;
            pRealTimer = &tkgTimer3;
            pRtNvIds = &tkgTimer3NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer3.tkxAccess );
            break;
            
        case MENU_ST_TIMER4:
            feature = MENU_ST_TIMER4;
            pRealTimer = &tkgTimer4;
            pRtNvIds = &tkgTimer4NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer4.tkxAccess );
            break;
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
        case MENU_ST_TIMER5:
            feature = MENU_ST_TIMER5;
            pRealTimer = &tkgTimer5;
            pRtNvIds = &tkgTimer5NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer5.tkxAccess );
            break;

        case MENU_ST_TIMER6:
            feature = MENU_ST_TIMER6;
            pRealTimer = &tkgTimer6;
            pRtNvIds = &tkgTimer6NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer6.tkxAccess );
            break;
            
        case MENU_ST_TIMER7:
            feature = MENU_ST_TIMER7;
            pRealTimer = &tkgTimer7;
            pRtNvIds = &tkgTimer7NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer7.tkxAccess );
            break;
        
            
        case MENU_ST_TIMER8:
            feature = MENU_ST_TIMER8;
            pRealTimer = &tkgTimer8;
            pRtNvIds = &tkgTimer8NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer8.tkxAccess );
            break;
            
            
        case MENU_ST_TIMER9:
            feature = MENU_ST_TIMER9;
            pRealTimer = &tkgTimer9;
            pRtNvIds = &tkgTimer9NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer9.tkxAccess );
            break;
            
            
        case MENU_ST_TIMER10:
            feature = MENU_ST_TIMER10;
            pRealTimer = &tkgTimer10;
            pRtNvIds = &tkgTimer10NvIds;
            mufFeatureScroll( MENU_ST_TIMER_SELECT, tkgTimer10.tkxAccess );
            break;
#endif // EXTRA_TIMERS

        // End of FEATURE selection
            
        // Start of battery monitor cases
#ifndef EXTRA_TIMERS
        case MENU_ST_BATMON_SELECT:
            if ( muhStateChange )
            {
                choice = ( volt_monitor ? CHOOSE_ON : CHOOSE_OFF );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( set_choices), set_choices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_OFF:
                        volt_monitor = OFF;
                        (void) nvfPutNumber( NV_ID_BM_ENABLE, volt_monitor );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ON:
                        volt_monitor = ON;
                        (void) nvfPutNumber( NV_ID_BM_ENABLE, volt_monitor );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_SET:
                        mufChangeMenu( MENU_ST_BATMON_SET );
                        break;
                }
            }
            break;
            
        case MENU_ST_BATMON_SET:
            if ( muhStateChange )
            {
                scratch = BMON_VOLTAGE;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "" );
            scratch = change_number( scratch, 1, 135, 110 );
            mufShowDecimalVolts( scratch );
            if (MENU_BTN)
            {
                BMON_VOLTAGE = scratch;
                (void) nvfPutVoltage( NV_ID_BM_VOLTAGE, BMON_VOLTAGE );
                mufChangeMenu( MENU_ST_BATMON_SELECT );
            }
            break;
        // End battery monitor cases
#endif // EXTRA_TIMERS

        // Start cold temperature start cases
#ifdef USE_COLDSTART        
        case MENU_ST_CSTART_SELECT:
            if ( muhStateChange )
            {
                choice = ( bColdMonitor ? CHOOSE_ON : CHOOSE_OFF );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( cold_choices ), cold_choices, 4 );
            if ( MENU_BTN )
            {
                switch ( choice )
                {
                    case CHOOSE_OFF:
                        bColdMonitor = OFF;
                        (void) nvfPutNumber( NV_ID_CS_ENABLE, bColdMonitor );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ON:
                        bColdMonitor = ON;
                        (void) nvfPutNumber( NV_ID_CS_ENABLE, bColdMonitor );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_SET_TEMP:
                        mufChangeMenu( MENU_ST_CSTART_SET_TEMP );
                        break;
                    case CHOOSE_SET_DURATION:
                        mufChangeMenu( MENU_ST_CSTART_SET_TIME );
                        break;
                }
            }
            break;
            
        case MENU_ST_CSTART_SET_TEMP:
            if ( muhStateChange )
            {
                signedScratch = tCOLD_START.cStartTemp;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "TEMP" );
            signedScratch = fncChangeSignedNumber( signedScratch,
                                                   1,
                                                   CSTART_HIGH_TEMP,
                                                   CSTART_LOW_TEMP );
            mufShowTemperature( signedScratch );
            if (MENU_BTN)
            {
                tCOLD_START.cStartTemp = signedScratch;
                tCOLD_START.uiStartTempVoltage = tmfCtoVad( tCOLD_START.cStartTemp );
                (void) nvfPutTemperature( NV_ID_CS_TEMP, tCOLD_START.cStartTemp );
                mufChangeMenu( MENU_ST_CSTART_SELECT );
            }
            break;
            
        case MENU_ST_CSTART_SET_TIME:
            if ( muhStateChange )
            {
                scratch = tCOLD_START.uiColdRunTime;
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "DURATION" );
            scratch = change_number( scratch, 5, 40, 10 );
            mufShowDecimalHours( scratch );
            if ( MENU_BTN )
            {
                tCOLD_START.uiColdRunTime = scratch;
                (void) nvfPutNumber( NV_ID_CS_TIME, tCOLD_START.uiColdRunTime );
                mufChangeMenu( MENU_ST_CSTART_SELECT );
            }
            break;
#endif // USE_COLDSTART
        // End of cold temperature start cases

#ifndef EXTRA_TIMERS
        // Start of temperature monitor cases
        case MENU_ST_TEMPMON_SELECT:
            if ( muhStateChange )
            {
                choice = ( tmgTMEnable ? CHOOSE_ON : CHOOSE_OFF );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( tempmon_choices), tempmon_choices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_OFF:
                        tmgTMEnable = OFF;
                        (void) nvfPutNumber( NV_ID_TM_ENABLE, tmgTMEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ON:
                        tmgTMEnable = ON;
                        (void) nvfPutNumber( NV_ID_TM_ENABLE, tmgTMEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_SET_TEMP:
                        mufChangeMenu( MENU_ST_TEMPMON_SET_TEMP );
                        break;
                    case CHOOSE_SET_RANGE:
                        mufChangeMenu( MENU_ST_TEMPMON_SET_RANGE );
                        break;
                    case CHOOSE_SET_FAN_SPEED:
                        mufChangeMenu( MENU_ST_TEMPMON_SET_FAN_SPEED );
                        break;
                    case CHOOSE_SET_DURATION:
                        mufChangeMenu( MENU_ST_TEMPMON_SET_DURATION );
                        break;
                }
                tmfSetMode();
            }
            break;
            
        case MENU_ST_TEMPMON_SET_TEMP:
            if ( muhStateChange )
            {
                signedScratch = tmgTMTemp;
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "TEMP" );
            signedScratch = fncChangeSignedNumber( signedScratch,
                                                   1,
                                                   TEMPMON_HIGH_TEMP,
                                                   TEMPMON_LOW_TEMP );
            mufShowTemperature( signedScratch );
            if (MENU_BTN)
            {
                tmgTMTemp = signedScratch;
                (void) nvfPutTemperature( NV_ID_TM_TEMP, tmgTMTemp );
                mufChangeMenu( MENU_ST_TEMPMON_SELECT );
            }
            break;

        case MENU_ST_TEMPMON_SET_RANGE:
            if ( muhStateChange )
            {
                signedScratch = tmgTMRange;
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "RANGE" );
            signedScratch = fncChangeSignedNumber( signedScratch,
                                                   1,
                                                   TEMPMON_HIGH_RANGE,
                                                   TEMPMON_LOW_RANGE );
            mufShowTemperature( signedScratch );
            if (MENU_BTN)
            {
                tmgTMRange = signedScratch;
                (void) nvfPutTemperature( NV_ID_TEMP_RANGE, tmgTMRange );
                mufChangeMenu( MENU_ST_TEMPMON_SELECT );
            }
            break;

        case MENU_ST_TEMPMON_SET_FAN_SPEED:
            if ( muhStateChange )
            {
                choice = ( tmgFSRange ? CHOOSE_LOW_HIGH : CHOOSE_HIGH_ONLY );
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "FAN" );     
            choice = mufChoose( choice, DIM( fanspeed_choices), fanspeed_choices, 4 );
            switch (choice)
            {
                case CHOOSE_HIGH_ONLY:
                    tmgFSRange = FAN_SPEED_HIGH_ONLY;
                break;

                case CHOOSE_LOW_HIGH:
                    tmgFSRange = FAN_SPEED_LOW_HIGH;
                break;
            }
            if (MENU_BTN)
            {
                (void) nvfPutNumber( NV_ID_FAN_SPEED_RANGE, tmgFSRange );
                mufChangeMenu( MENU_ST_TEMPMON_SELECT );
            }
            break;
            
        case MENU_ST_TEMPMON_SET_DURATION:
            if ( muhStateChange )
            {
                scratch = tmgTMDuration;
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "HOURS" );
#if 0   // TESTING
            scratch = change_number( scratch, 1, 240, 0 );
#else
            scratch = change_number( scratch, 5, 240, 0 );
#endif
            mufShowDecimalHours( scratch );
            if ( MENU_BTN )
            {
                tmgTMDuration = scratch;
                (void) nvfPutNumber( NV_ID_TM_DURATION, scratch );
                tmfSetMode();
                // If the timer is active
                if ( tmgTMActive )
                {
                    // Restart timer with the new setting
                    timers.tmRunTmr = ( HOUR_TENTHS_TO_MINUTES( tmgTMDuration ) );
                }
                mufChangeMenu( MENU_ST_TEMPMON_SELECT );
            }
            break;
#endif // EXTRA_TIMERS

        case MENU_ST_TEMP_SELECT:
            if ( muhStateChange )
            {
                scratch = tmgTMTemp;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "TEMPERATURE" );

#ifndef MM32F0133C7P		//? Add
            TM_CELSIUS tmcMax = tmgTMTemp + tmgTMRange;
            TM_CELSIUS tmcMin = tmgTMTemp - tmgTMRange;
#else		///?OK
            tmcMax = tmgTMTemp + tmgTMRange;
            tmcMin = tmgTMTemp - tmgTMRange;
#endif
            if (tmcMax > TEMPMON_HIGH_TEMP)
                 tmcMax = TEMPMON_HIGH_TEMP;
            if (tmcMin < TEMPMON_LOW_TEMP)
                tmcMin = TEMPMON_LOW_TEMP;

            scratch = change_number( scratch, 1, tmcMax, tmcMin );
            mufShowTemperature( scratch );
            if (MENU_BTN)
            {
                tmgTMTemp = scratch;
                mufChangeMenu( MENU_ST_SETTEMP );
            }
            break;
            
        // Start maximum run time cases
        case MENU_ST_MAXRUN_TIME:
            if ( muhStateChange )
            {
                choice = ( tkgMaxRunEnable ? CHOOSE_ON : CHOOSE_OFF );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( set_choices), set_choices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_OFF:
                        tkgMaxRunEnable = OFF;
                        (void) nvfPutNumber( NV_ID_MR_ENABLE, tkgMaxRunEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ON:
                        tkgMaxRunEnable = ON;
                        (void) nvfPutNumber( NV_ID_MR_ENABLE, tkgMaxRunEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_SET:
                        mufChangeMenu( MENU_ST_MAXRUN_TIME_SET );
                        break;
                }
            }
            break;
            
        case MENU_ST_MAXRUN_TIME_SET:
            if ( muhStateChange )
            {
                scratch = tkgMaxRunTime;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "" );
            
            scratch = change_number( scratch, 5, 240, 20 );
            mufShowDecimalHours( scratch );
            if (MENU_BTN)
            {
                tkgMaxRunTime = scratch;
                (void) nvfPutNumber( NV_ID_MR_TIME, tkgMaxRunTime );
                mufChangeMenu( MENU_ST_MAXRUN_TIME );
            }
            break;
        // End maximum run time cases

        // Start of TIMER cases
        case MENU_ST_TIMER_SELECT:
            if ( muhStateChange )
            {
                choice = ( pRealTimer->tkxEnable ? CHOOSE_ON : CHOOSE_OFF );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( timer_choices), timer_choices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_OFF:
                        pRealTimer->tkxEnable = OFF;
                        (void) nvfPutNumber( pRtNvIds->tkxEnableId,
                                             pRealTimer->tkxEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ON:
                        pRealTimer->tkxEnable = ON;
                        (void) nvfPutNumber( pRtNvIds->tkxEnableId,
                                             pRealTimer->tkxEnable );
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_SET_START:
                        mufMakeClockSet( pRealTimer->tkxTime.tkxDayOfWeek,
                                         pRealTimer->tkxTime.tkxHours,
                                         pRealTimer->tkxTime.tkxMinutes );
                        mufChangeMenu( MENU_ST_TIMER_SET_HOUR );
                        break;
                    case CHOOSE_SET_DURATION:
                        mufChangeMenu( MENU_ST_TIMER_SET_DURATION );
                        break;
                    case CHOOSE_SET_TEMP:
                        mufChangeMenu( MENU_ST_TIMER_SET_TEMP );
                        break;
                }
            }
            break;
            
        case MENU_ST_TIMER_SET_HOUR:
            mufLabelFeatureSet( feature, "HOURS" );
            mufSetTimeField( MU_HOURS );
            if ( MENU_BTN )
            {
                mufChangeMenu( MENU_ST_TIMER_SET_MINUTE );
            }
            break;
            
        case MENU_ST_TIMER_SET_MINUTE:
            mufLabelFeatureSet( feature, "MINUTES" );
            mufSetTimeField( MU_MINUTES );
            if ( MENU_BTN )
            {
                if ( mugClockMode24 )
                    mufChangeMenu( MENU_ST_TIMER_SET_DAY );
                else
                    mufChangeMenu( MENU_ST_TIMER_SET_AM_PM );
            }
            break;
            
        case MENU_ST_TIMER_SET_AM_PM:
            mufLabelFeatureSet( feature, "AM/PM" );
            mufSetTimeField( MU_AMPM );
            if ( MENU_BTN )
            {
                mufAmPmAdjust();
                mufChangeMenu( MENU_ST_TIMER_SET_DAY );
            }
            break;
            
        case MENU_ST_TIMER_SET_DAY:
            if ( muhStateChange )
            {
                choice = all_day_choices[ muhClockSet.muxDayOfWeek ];
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "DAY" );
            choice = mufChoose( choice, DIM(all_day_choices), all_day_choices, 4 );
            if ( MENU_BTN )
            {
                TKZTIME newTime;
                
                muhClockSet.muxDayOfWeek = choice - CHOOSE_SUNDAY;
                
                // Convert clock set time to time keeper time
                newTime.tkxDayOfWeek = muhClockSet.muxDayOfWeek;
                newTime.tkxHours = muhClockSet.muxHours;
                newTime.tkxMinutes = muhClockSet.muxMinutes;
                                   
                pRealTimer->tkxTime = newTime;
                (void) nvfPutTime( pRtNvIds->tkxTimeId,
                                   pRealTimer->tkxTime );
                mufChangeMenu( MENU_ST_TIMER_SELECT );
            }
            break;
            
        case MENU_ST_TIMER_SET_DURATION:
            if ( muhStateChange )
            {
                scratch = pRealTimer->tkxDuration;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "DURATION" );
            scratch = change_number( scratch, 5, 100, 5 );
            mufShowDecimalHours( scratch );
            if ( MENU_BTN )
            {
                pRealTimer->tkxDuration = scratch;
                (void) nvfPutNumber( pRtNvIds->tkxDurationId,
                                     scratch );
                mufChangeMenu( MENU_ST_TIMER_SELECT );
            }
            break;
            
        case MENU_ST_TIMER_SET_TEMP:
            if ( muhStateChange )
            {
                signedScratch = pRealTimer->tkxTempSet;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "TEMP" );
            signedScratch = fncChangeSignedNumber( signedScratch,
                                                   1,
                                                   TEMPMON_HIGH_TEMP,
                                                   TEMPMON_LOW_TEMP );
            mufShowTemperature( signedScratch );
            if (MENU_BTN)
            {
                pRealTimer->tkxTempSet = signedScratch;
                (void) nvfPutNumber( pRtNvIds->tkxTemperatureId,
                                     signedScratch );
                mufChangeMenu( MENU_ST_TIMER_SELECT );
            }
            break;
            
        // end of of TIMER cases
            
        // Start of Clock cases
        case MENU_ST_CLOCK_MODE:
            rtfEnableWrite();
            if ( muhStateChange )
            {
                choice = ( mugClockMode24 ? CHOOSE_MODE_24 : CHOOSE_MODE_12 );
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "CLOCK MODE" );
            choice = mufChoose( choice,
                             DIM( clock_mode_choices),
                             clock_mode_choices,
                             4 );
            if ( MENU_BTN )
            {
                mugClockMode24 = ( choice == CHOOSE_MODE_24 );
                (void) nvfPutNumber( NV_ID_CLOCK_MODE, mugClockMode24 );
                // Use the current time as starting time for setting clock
                mufMakeClockSet( rtgClock.rtxDayOfWeek,
                                   rtgClock.rtxHours,
                                   rtgClock.rtxMinutes );
                mufMakeDateSet( rtgClock.rtxDate,
                                   rtgClock.rtxMonth,
                                   rtgClock.rtxYear );
                mufChangeMenu( MENU_ST_CLOCK_HOUR );
            }
            break;
            
        case MENU_ST_CLOCK_HOUR:
            mufLabelFeatureSet( feature, "HOURS" );
            mufSetTimeField( MU_HOURS );
            if ( MENU_BTN )
            {
                mufChangeMenu( MENU_ST_CLOCK_MINUTE );
            }
            break;
            
        case MENU_ST_CLOCK_MINUTE:
            mufLabelFeatureSet( feature, "MINUTES" );
            mufSetTimeField( MU_MINUTES );
            if ( MENU_BTN )
            {
                if ( mugClockMode24 )
                {
                    mufChangeMenu( MENU_ST_CLOCK_DOW );
                }
                else
                {
                    mufChangeMenu( MENU_ST_CLOCK_AM_PM );
                }
            }
            break;
            
        case MENU_ST_CLOCK_AM_PM:
            mufLabelFeatureSet( feature, "AM/PM" );
            mufSetTimeField( MU_AMPM );
            if ( MENU_BTN )
            {
                mufAmPmAdjust();
                mufChangeMenu( MENU_ST_CLOCK_DOW );
            }
            break;
            
        case MENU_ST_CLOCK_DOW:
            if ( muhStateChange )
            {
                choice = day_choices[ muhClockSet.muxDayOfWeek ];
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "DAY" );
            choice = mufChoose( choice, DIM( day_choices ), day_choices, 4 );
            if ( MENU_BTN )
            {
                muhClockSet.muxDayOfWeek = choice - CHOOSE_SUNDAY;
                mufChangeMenu( MENU_ST_CLOCK_YEAR );
            }
            break;
            
        case MENU_ST_CLOCK_YEAR:
            mufLabelFeatureSet( feature, "YEAR" );
            mufSetDateField( MU_YEAR );
            if ( MENU_BTN )
            {
                mufChangeMenu( MENU_ST_CLOCK_MONTH );
            }
            break;
            
        case MENU_ST_CLOCK_MONTH:
            mufLabelFeatureSet( feature, "MONTH" );
            mufSetDateField( MU_MONTH );
            if ( MENU_BTN )
            {
                mufChangeMenu( MENU_ST_CLOCK_MONTHDAY );
            }
            break;
            
        case MENU_ST_CLOCK_MONTHDAY:
            mufLabelFeatureSet( feature, "DAY" );
            mufSetDateField( MU_DAY );
            if ( MENU_BTN )
            {
                // Set the real-time clock time using timekeeper (24 hour) time
                rtfSetClock( muhClockSet.muxDayOfWeek,
                             muhClockSet.muxHours,
                             muhClockSet.muxMinutes,
                             muhClockSet.muxDate,
                             muhClockSet.muxMonth,
                             muhClockSet.muxYear);
                             
                // If we are still in setup phase
                if ( feature == MENU_ST_SETUP )
                {
                    mufChangeMenu( MENU_ST_RUN_HOURS );
                }
                else
                {
                    mufChangeMenu( MENU_ST_OFF );
                }
            }
            break;
            
        // End of Clock cases
        
#ifndef EXTRA_TIMERS
        // Start of Voltage cases
        case MENU_ST_ADAPT_VOLTAGE:
            if ( muhStateChange )
            {
                scratch = (char) battery_voltage();
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "VOLTAGE" );
            scratch = change_number( scratch, 1, 150, 100 );
            mufShowDecimalVolts( scratch );
            if (MENU_BTN)
            {
                // Remove the battery adjustment before calculating new one
                bmgVOffset = 0;
                bmgVOffset = scratch - battery_voltage();
                (void) nvfPutNumber( NV_ID_BM_V_OFFSET,
                                             (unsigned char) bmgVOffset );
                mufChangeMenu( MENU_ST_OFF );
            }
            break;
        // End of Voltage cases
#endif // EXTRA_TIMERS

        // Start of Set Run Hours cases
        case MENU_ST_RUN_HOURS:
            if ( muhStateChange )
            {
                muhStateChange = FALSE;
                mufStartDigits( 5, 50, 4 );
            }
            mufLabelFeatureSet( feature, "RUN HOURS" );
            if ( mufGetDigits() )
            {
                // Get the digits and save as Run Hours
                HourMeterPutHours( mufLongDigits() );
                // Finished with setup phase
                mugSetup = TRUE;
                (void) nvfPutNumber( NV_ID_SETUP, mugSetup );
                mufChangeMenu( MENU_ST_OFF );
            }
            break;
        // End of Set Run Hours cases
            
#ifdef OBM
        case MENU_ST_FAULTS_SELECT:
            if ( muhStateChange )
            {
                choice = CHOOSE_VIEW_FAULTS;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( faultsChoices ), faultsChoices, 4 );
            if (MENU_BTN)
            {
                switch(choice)
                {
                    case CHOOSE_VIEW_FAULTS:
                        mufChangeMenu( MENU_ST_FAULTS_DISPLAY );
                        break;
                }
            }
            break;

        case MENU_ST_FAULTS_DISPLAY:
            if ( muhStateChange )
            {
                muhStateChange = FALSE;
            }
            
            OBM_Record_Display.number_of_records = OBM_fnGetTotalRecords (1);
            lcd_position( 0, 2, small );
            printf("VIEW FAULTS (%5d)", OBM_Record_Display.record_index);
            lcd_position( 0, 3, small );
            printf("         OF (%5d)", OBM_Record_Display.number_of_records);

            if (OBM_Record_Display.number_of_records == 0)
            {
                lcd_position( 0, 4, small );
                printf("NO RECORDS TO VIEW");
                break;  // case MENU_ST_FAULTS_DISPLAY
            }

            if (MENU_BTN)
            {
                lcd_cla( 0, 131, 2, 7 );
                mufChangeMenu( MENU_ST_FAULTS );
                break;
            }
            else if ( LEFT_BTN && !timers.menu2Tmr )
            {
                timers.menuTmr = MENU_PAUSE;
                if (OBM_Record_Display.record_index < OBM_Record_Display.number_of_records)
                {
                    OBM_Record_Display.record_index ++;
                }
                else 
                {
                    OBM_Record_Display.record_index = 1;
                }
                OBM_Record_Display.read_now = TRUE;
                OBM_Record_Display.record_ready = FALSE;
                month_ptr = "BOGUS";    // This is to prevent display of data if nothing is read yet
            }
            else if ( RIGHT_BTN && !timers.menu2Tmr )
            {
                timers.menuTmr = MENU_PAUSE;
                if (OBM_Record_Display.record_index > 1)
                {
                    OBM_Record_Display.record_index --;
                }
                else 
                {
                    OBM_Record_Display.record_index = OBM_Record_Display.number_of_records;
                }
                OBM_Record_Display.read_now = TRUE;
                OBM_Record_Display.record_ready = FALSE;
                month_ptr = "BOGUS";    // This is to prevent display of data if nothing is read yet
            }

            if (OBM_Record_Display.read_now)
            {
                lcd_cla(0,131,4,7);
                if ((ssgDone) && (!(OBM_fnCheckBusy())))
                {
                    if (OBM_fnGetRecord (&OBM_Data[0], OBM_Record_Display.record_index, 1))
                    {
                        OBM_Record_Display.read_now = FALSE;
                    }
                }
            }

            if (OBM_Record_Display.record_ready)
            {
                if (Decode_Data(&OBM_Data[0], &month, &day, &year, &hour, &minute, &second, &spn, &fmi, &runTimeSinceLastStart) == CLEAR)
                    flag = "CLEARED";
                else
                    flag = "";
                month_ptr = mufMonthString((char) month);
                if (spn > 520000)
                    device = "DPF";
                else if (spn == SPN_APU_OIL_PRESS || spn == SPN_APU_COOLANT_TEMP || spn == SPN_APU_ALT_STATUS)
                    device = "APU";
                else if (spn == SPN_CCU_MAIN_BR_TRIPPED || spn == SPN_CCU_BR1_TRIPPED || spn == SPN_CCU_BR2_TRIPPED)
                    device = "CCU";
            }

            if (month_ptr != "BOGUS")
            {
                // Display time & date
                lcd_position(0,4,small);
                printf( "%s %02d %02d %02d:%02d:%02d %s",
                                        month_ptr,
                                        day,
                                        year,
                                        hour,
                                        minute,
                                        second,
                                        device );
            
                // Display fault
                lcd_position(0,6,small);
                switch (spn)
                {
                    case SPN_DPF_DISCONNECTED:
                        if (fmi == FMI_DPF_DISCONNECTED)
                        {
                            printf("DPF LOST");
                        }
                        else if (fmi == FMI_DPF_ABNORMAL_FREQ)
                        {
                            printf("ACKNOWLEDGE ERROR");
                        }
                    break;

                    case SPN_HEATER_FAIL:
                        if ((fmi == FMI_HEATER_FAIL) || (fmi == FMI_HEATER_TIMEOUT))
                        {
                            printf("DPF HEATER FAIL");
                        }
                    break;
    
                    case SPN_EXHST_TEMP:
                        if (fmi == FMI_EXHST_TEMP_SNSR_FAIL)
                        {
                            printf("EXHST TEMP SNSR FAIL");
                        }
                        else if (fmi == FMI_EXHST_TEMP_HIGH)
                        {
                            printf("EXHST TEMP HIGH");
                        }
                    break;

                    case SPN_EXHST_PRESS:
                        if (fmi == FMI_EXTREME_PRESS)
                        {
                            printf("DPF FLTR PLUGGED");
                        }
                        else if (fmi == FMI_PRESS_SNSR_FAIL)
                        {
                            printf("EXHST PRESS SNSR FAIL");
                        }
                        else if (fmi == FMI_PRESS_SNUBBER_PLUG)
                        {
                            printf("EXHST PRESS SNUBR PLUG");
                        }
                        else if (fmi == FMI_PRESS_HIGH_POST_REGEN )
                        {
                            printf("EXHST PRESS POST REGEN");
                        }
                    break;

                    case SPN_GEN_VOLTAGE:
                        if (fmi == FMI_LOW_VOLTAGE)
                        {
                            printf("GEN LOW VOLTAGE");
                        }
                    break;

                    case SPN_GEN_FREQ:
                        if ( fmi == FMI_FREQ_FAULT )
                        {
                            printf( "GEN FREQ FAULT" );
                        }
                    break;

                    case SPN_APU_OIL_PRESS:
                        if ( fmi == FMI_APU_LOW_OIL_PRESS )
                        {
                            printf( "APU LOW OIL PRESSURE" );
                        }
                    break;

                    case SPN_APU_COOLANT_TEMP:
                        if ( fmi == FMI_APU_OVERTEMP )
                        {
                            printf( "APU ENGINE OVERHEAT" );
                        }
                    break;

                    case SPN_APU_ALT_STATUS:
                        if ( fmi == FMI_APU_ALT_FB )
                        {
                            printf( "APU ALT FEEDBACK FAULT" );
                        }
                    break;

                    case SPN_CCU_MAIN_BR_TRIPPED:
                        if ( fmi == FMI_CCU_MAIN_BR_TRIPPED )
                        {
                            printf( "CCU MAIN BRKR TRIPPED" );
                        }
                    break;

                    case SPN_CCU_BR1_TRIPPED:
                        if ( fmi == FMI_CCU_BR1_TRIPPED )
                        {
                            printf( "CCU HVAC BR1 TRIPPED" );
                        }
                    break;

                    case SPN_CCU_BR2_TRIPPED:
                        if ( fmi == FMI_CCU_BR2_TRIPPED )
                        {
                            printf( "CCU OUTLET BR2 TRIPPED" );
                        }
                    break;

                    default:
                        printf("UNKNOWN FAULT");
                    break;
                }

                if ((runTimeSinceLastStart & 0x40) == 0)
                {  /* if bit 6 = 0, 0 hours and minutes are in lowest 6 bits   */
                    runTimeSinceLastStartHours = 0;
                    runTimeSinceLastStartMinutes = runTimeSinceLastStart;
                }
                else
                {  /* if bit 6 = 1, bits 2-5 are for hours, and lowest 2 bits are for 15' quarters */
                   runTimeSinceLastStartHours = (runTimeSinceLastStart & 0x3C) >> 2;		/* bits 2-5 are for hours*/ 
                   runTimeSinceLastStartMinutes = (runTimeSinceLastStart & 0x03) * 15;		/* lowest 2 bits are for 15' quarters*/ 
                }

                lcd_position(0,7,small);
                printf("%lu,%2d, RT=%2d:%2d", spn, fmi, runTimeSinceLastStartHours, runTimeSinceLastStartMinutes );
                
                lcd_position(80,7,small);
                printf("%s", flag);
                OBM_Record_Display.record_ready = FALSE;
            }
        break;

        case MENU_ST_FAULTS_RESET:
            if ( muhStateChange )
            {
                choice = CHOOSE_NO; 
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "RESET" );
            choice = mufChoose( choice, DIM( yesno_choices ), yesno_choices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_YES:
                        OBM_fnReset(1);
                        OBM_fnReset(2);
                        ResetFaults();
                        mufChangeMenu( MENU_ST_SYS_CFG_SELECT );
                        break;
                    case CHOOSE_NO:
                        mufChangeMenu( MENU_ST_SYS_CFG_SELECT );
                        break;
                }
            }

        break;
#endif // OBM

        case MENU_ST_SYS_CFG_SELECT:
            if ( muhStateChange )
            {
                choice = CHOOSE_CHECK;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( sysCfgChoices ), sysCfgChoices, 4 );
            
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_CHECK:
                        mufChangeMenu( MENU_ST_SYS_CFG_LIST_DEVICES );
                        break;
                    case CHOOSE_CONFIGURE:
                        mufChangeMenu( MENU_ST_SYS_CFG_CONFIGURE );
                        break;
                    case CHOOSE_RESET_FAULTS:
                        mufChangeMenu( MENU_ST_FAULTS_RESET );
                        break;
                }
            }
            break;
            
        case MENU_ST_SYS_CFG_LIST_DEVICES:
            mufLabelFeatureNoSet( feature, "PRESENT DEVICES" );
            // Print list of devices here
            lcd_position( 0, 4, large );
            // If the DPF is present
            if ( DPF_Diag.bPresent )
            {
                printf( "APU CCU DPF" );
            }
            else
            {
                printf( "APU CCU    " );
            }
            if (MENU_BTN)
            {
                mufChangeMenu( MENU_ST_SYS_CFG_SELECT );
            }
            break;
            
        case MENU_ST_SYS_CFG_CONFIGURE:
            if ( muhStateChange )
            {
                        choice = CHOOSE_DPF;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSet( feature, "DEVICES" );
            choice = mufChoose( choice, DIM( sysCfgDeviceChoices ), sysCfgDeviceChoices, 4 );
            
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_DPF:
                        mufChangeMenu( MENU_ST_SYS_CFG_DPF );
                        break;
                    case CHOOSE_HEATERS:
                        mufChangeMenu( MENU_ST_SYS_CFG_HEATERS );
                        break;
                }
            }
            break;

        case MENU_ST_SYS_CFG_DPF:
            mufLabelFeatureNoSet( feature, "DPF" );
            if ( muhStateChange )
            {
                // Look up DPF install settting and set as present choice
                if (DCP_Configuration & DPF_INSTALLED)
                    choice = CHOOSE_DISABLE;
                muhStateChange = FALSE;
            }
            choice = mufChoose( choice, DIM(sysInstallChoices), sysInstallChoices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_DISABLE:
                        Uninstall_DPF();
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                }
            }
            break;

        case MENU_ST_SYS_CFG_HEATERS:
            mufLabelFeatureNoSet( feature, "HEATER" );
            if ( muhStateChange )
            {	
                choice = ( HEAT_ENABLE ? CHOOSE_DISABLE : CHOOSE_ENABLE );
                muhStateChange = FALSE;
            }
            choice = mufChoose( choice, DIM(heatersInstallChoices), heatersInstallChoices, 4 );
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_DISABLE:
                        Enable_Heaters(OFF);
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                    case CHOOSE_ENABLE:
                        Enable_Heaters(ON);
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                }
            }
            break;

        case MENU_ST_DPF_SELECT:
            if ( muhStateChange )
            {
                choice = CHOOSE_START_REGEN;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            choice = mufChoose( choice, DIM( dpfChoices ), dpfChoices, 4 );
            
            if (MENU_BTN)
            {
                switch ( choice )
                {
                    case CHOOSE_START_REGEN:
                        mufChangeMenu( MENU_ST_DPF_START_REGEN );
                        break;
                    case CHOOSE_STATUS:
                        mufChangeMenu( MENU_ST_DPF_STATUS );
                        break;
                    case CHOOSE_VARS:
                        mufChangeMenu( MENU_ST_DPF_VARS );
                        break;
                }
            }
            break;
            
        case MENU_ST_DPF_START_REGEN:
            if ( muhStateChange )
            {
                choice = CHOOSE_NO;
                muhStateChange = FALSE;
            }
            mufLabelFeatureNoSet( feature, "START REGEN" );
            choice = mufChoose( choice, DIM(yesno_choices), yesno_choices, 4 );
            if ( MENU_BTN )
            {
                switch ( choice )
                {
                    case CHOOSE_NO:
                        mufChangeMenu( MENU_ST_DPF_SELECT );
                        break;
                    case CHOOSE_YES:
                        if (!(APU_ON_FLAG))
                        {
                            Set_Emergency_Regen_Flag();
                            VWM_fnRequestView (VWM_eVIEW_DPF_TURN_APU_ON, BACKLIGHT_ON );
                        }
                        Add_Event (EV_USER_REGEN);
                        // Exit the menu
                        mufChangeMenu( MENU_ST_OFF );
                        break;
                }
            }
            break;

        case MENU_ST_DPF_STATUS:
            mufLabelFeatureNoSet( feature, "STATUS" );
            if ( DPF_Info.Regenning )
            {
                mufCenterLargeText( "REGENNING", 4 );
            }
            else
            {
                switch( DPF_Info.Required )
                {
                    case DPF_REGEN_REQUIRED_SOON:
                        mufCenterLargeText( "REGEN SOON", 4 );
                        break;
                    case DPF_REGEN_REQUIRED_NOW:
                        mufCenterLargeText( "REGEN NOW", 4 );
                        break;
                    default:
                        mufCenterLargeText( "UNKNOWN", 4 );
                        break;
                }
            }
            if (MENU_BTN)
            {
                mufChangeMenu( MENU_ST_DPF_SELECT );
            }
            break;

        case MENU_ST_DPF_VARS:
            mufLabelFeatureNoSet( feature, "VARIABLES" );
            mufShowVar();

            if (MENU_BTN)
            {
                mufChangeMenu( MENU_ST_DPF_SELECT );
            }
            break;
    
        // Start administration cases
        case MENU_ST_ACCESS_SELECT:
            if ( muhStateChange )
            {
                access = MENU_FIRST_FEATURE;
                choice = NO_CHOICE;
                muhStateChange = FALSE;
            }
            mufLabelFeatureSmall( feature );
            if ( access > MENU_LAST_FEATURE )
            {
                mufChangeMenu( MENU_ST_OFF );
            }
            switch ( access )
            {
                case MENU_ST_TIMER1:
                    access = mufSetAccess( access,
                                           &tkgTimer1.tkxAccess,
                                           NV_ID_TIMER1_ACCESS );
                    break;
                case MENU_ST_TIMER2:
                    access = mufSetAccess( access,
                                           &tkgTimer2.tkxAccess,
                                           NV_ID_TIMER2_ACCESS );
                    break;
#ifndef USE_COLDSTART
                case MENU_ST_TIMER3:
                    access = mufSetAccess( access,
                                           &tkgTimer3.tkxAccess,
                                           NV_ID_TIMER3_ACCESS );
                    break;
                case MENU_ST_TIMER4:
                    access = mufSetAccess( access,
                                           &tkgTimer4.tkxAccess,
                                           NV_ID_TIMER4_ACCESS );
                    break;
#endif // USE_COLDSTART
#ifndef EXTRA_TIMERS
                case MENU_ST_BATTMON:
                    access = mufSetAccess( access,
                                           &bmgBMAccess,
                                           NV_ID_BM_ACCESS );
                    break;
                    
                case MENU_ST_CALIBRATE:
                    access = mufSetAccess( access,
                                           &bmgVoltAccess,
                                           NV_ID_VOLT_ACCESS );
                    break;
#endif // EXTRA_TIMERS

                case MENU_ST_SYS_CFG:
                    access = mufSetAccess( access,
                                           &SYS_u8SysCfgAccess,
                                           NV_ID_SYS_CFG_ACCESS );
                    break;

                case MENU_ST_DPF:
                    access = mufSetAccess( access,
                                           &DPF_u8DpfAccess,
                                           NV_ID_DPF_ACCESS );
                    break;
            

#ifdef USE_COLDSTART        
                case MENU_ST_COLDSTART:
                    access = mufSetAccess( access,
                                           &tCOLD_START.ucAccess,
                                           NV_ID_CS_ACCESS );
                    break;
#endif

#ifndef EXTRA_TIMERS
                case MENU_ST_TEMPMON:
                    access = mufSetAccess( access,
                                           &tmgTMAccess,
                                           NV_ID_TM_ACCESS );
                    break;
#endif // EXTRA_TIMERS

                case MENU_ST_MAXRUN:
                    access = mufSetAccess( access,
                                           &tkgMaxRunAccess,
                                           NV_ID_MR_ACCESS );
                    break;
                case MENU_ST_CLOCK:
                    access = mufSetAccess( access,
                                           &tkgClockAccess,
                                           NV_ID_CLOCK_ACCESS );
                    break;

                default:
                    access++;
                    break;
            }
            break;
            
        case MENU_ST_PASSWORD:
            if ( muhStateChange )
            {
                mufStartDigits( 4, 50, 6 );
                muhStateChange = FALSE;
            }
            lcd_position( 10, 4, large );
            mufLargePrompt( "ENTER PASSWORD" );
            if ( mufGetDigits() )
            {
                pword = mufPasswordDigits();
                if (( pword == user_password ) || ( pword == TFX_PASSWORD ))
                {
                    mufChangeMenu( next_menu_state );
                }
                else
                {
                    mufChangeMenu( MENU_ST_WRONG_PASSWORD );
                }
            }
            break;
            
        case MENU_ST_WRONG_PASSWORD:
            if ( muhStateChange )
            {
                // Reload blink timer with delay time and set blink to On
                timers.blinkTmr = 250;
                mugBlinkOn = TRUE;
                muhStateChange = FALSE;
            }
            mufLargePrompt( "WRONG PASSWORD" );
            // Wait for blink to go off
            if ( !mugBlinkOn )
            {
                mufChangeMenu( MENU_ST_PASSWORD );
            }
            break;
            
        case MENU_ST_NEW_PASSWORD:
            if ( muhStateChange )
            {
                mufStartDigits( 4, 50, 6 );
                muhStateChange = FALSE;
            }
            mufLargePrompt( "NEW PASSWORD" );
            if ( mufGetDigits() )
            {
                newPass = mufPasswordDigits();
                mufChangeMenu( MENU_ST_CONFIRM_PASSWORD );
            }
            break;
            
        case MENU_ST_CONFIRM_PASSWORD:
            if ( muhStateChange )
            {
                mufStartDigits( 4, 50, 6 );
                muhStateChange = FALSE;
            }
            mufLargePrompt( "CONFIRM" );
            if ( mufGetDigits() )
            {
                pword = mufPasswordDigits();
                if ( newPass == pword )
                {
                    // If the old password is zero
                    if ( user_password == 0 )
                    {
                        // Change all feature accesses to no access
#ifndef EXTRA_TIMERS
                        bmgBMAccess =          OFF;
                        tmgTMAccess =          OFF;
                        bmgVoltAccess =        OFF;                        
#endif // EXTRA_TIMERS
#ifdef USE_COLDSTART
                        tCOLD_START.ucAccess = OFF;
#endif
                        tkgMaxRunAccess =      OFF;
                        tkgTimer1.tkxAccess =  OFF;
                        tkgTimer2.tkxAccess =  OFF;
#ifndef USE_COLDSTART
                        tkgTimer3.tkxAccess =  OFF;
                        tkgTimer4.tkxAccess =  OFF;
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
                        tkgTimer5.tkxAccess =  OFF;
                        tkgTimer6.tkxAccess =  OFF;
                        tkgTimer7.tkxAccess =  OFF;
                        tkgTimer8.tkxAccess =  OFF;
                        tkgTimer9.tkxAccess =  OFF;
                        tkgTimer10.tkxAccess = OFF;
#endif // EXTRA_TIMERS
                        tkgClockAccess =       OFF;
                    }
                    
                    // Save new password
                    (void)nvfPutNumber( NV_ID_PASS_BYTE1, pword );
                    (void)nvfPutNumber( NV_ID_PASS_BYTE2, pword >> 8 );
                    user_password = pword;
                    mufChangeMenu( last_menu_state );
                }
                else
                {
                    // Reload blink timer and set blink to On
                    mufChangeMenu( MENU_ST_WRONG_CONFIRM );
                }
            }
            break;
            
        case MENU_ST_WRONG_CONFIRM:
            if ( muhStateChange )
            {
                // Reload blink timer and set blink to On
                timers.blinkTmr = 250;
                mugBlinkOn = TRUE;
                muhStateChange = FALSE;
            }
            mufLargePrompt( "PASS MISMATCH" );
            // Wait for blink to go off
            if ( !mugBlinkOn )
            {
                mufChangeMenu( MENU_ST_NEW_PASSWORD );
            }
            break;
            
        // End administration cases

        default:
            mufChangeMenu( MENU_ST_OFF );
            break;
    }
}


void ResetFaults(void)
{
    TxPgn_65280[6] = 0x00;
    TxPgn_65280[7] = 0x00;
}

#ifdef OBM

static unsigned char Decode_Data(unsigned char *OBM_Data, unsigned char *month, unsigned char *day, 
                            unsigned char *year, unsigned char *hour, unsigned char *minute, unsigned char *second,
                            unsigned long *spn, unsigned char *fmi, unsigned char *runTimeSinceLastStart )
{
    unsigned char type = CLEAR;

    *second = OBM_Data[0] & 0x3F;

    *minute = ((OBM_Data[0] & 0xC0) >> 6);
    *minute += ((OBM_Data[1] & 0x0F) << 2);


    *hour = ((OBM_Data[1]& 0xF0) >> 4);
    *hour += ((OBM_Data[2] & 0x01) << 4);

    *day = ((OBM_Data[2] & 0x3E) >> 1);

    *month = ((OBM_Data[2] & 0xC0) >> 6);
    *month += ((OBM_Data [3] & 0x03) << 2);

    *year = (OBM_Data[3] >> 2);

    if (OBM_Data [7] & 0x80)
        type = SET;

    OBM_Data [7] &= ~0x80; // take out the set/clear flag bit anyways
    *runTimeSinceLastStart = OBM_Data [7];

    *spn = OBM_Data [6] & 0x07;
    *spn <<= 8;
    *spn += OBM_Data [5];
    *spn <<= 8;
    *spn += OBM_Data [4];
    *fmi = ((OBM_Data [6] & 0xF8)  >> 3);

    return type;
}

void Set_OBM_Read_Done_Flags (void)
{
    if ( OBM_Record_Display.read_now )
        OBM_Record_Display.record_ready = TRUE;
}

#endif // OBM


