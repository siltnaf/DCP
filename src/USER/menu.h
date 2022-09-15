/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file menu.h
       \brief Header file for module containing routines for displaying and navigating menu tree

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

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/menu.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

          $Log: not supported by cvs2svn $
          Revision 1.6  2014/07/25 15:24:17  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.5  2014/06/27 17:35:28  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.4  2014/04/16 14:07:09  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.3  2013/09/19 13:07:45  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.2  2010/12/01 22:44:33  dharper
          Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
          This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
          This is because REV B was never checked in.
    
          Revision 1.1  2009/07/31 20:28:34  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
          Revision 1.16  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.15  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.14  2008-03-11 19:16:03  msalimi
          -Added a code inquirey mechanism to editing APU hours menu. This code is internally generated based on the time and date and must also be entered by the user in order to access the menu.

          Revision 1.13  2008-02-25 19:42:36  msalimi
          Fixed OBM reading issues.

          Revision 1.12  2008-02-06 16:29:56  msalimi
          - Corrected low voltage SPN. This was 5 and now is 4.

          Revision 1.11  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.10  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.9  2007-12-06 21:40:10  msalimi
          - Changed the message that delays the APU shutdown to also show the coolddown period.
          - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
          - Eliminated the "Heater Status" from DPF menu.

          Revision 1.8  2007-11-30 18:52:08  msalimi
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

          Revision 1.7  2007-10-19 21:12:42  msalimi
          New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

          Revision 1.6  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.5  2007-09-21 20:29:34  msalimi
          Removed Diagnostic and SPM related menu items.

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:15:21  nallyn
          Added USE_COLDSTART to allow for standard production build with two timers and cold start function. Added comments for Doxygen.

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.16  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
    
          Revision 1.15  2007/02/27 01:20:17  nallyn
          Added 4 more timers for a total of 10 timers. The timers function differently
          than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
          timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
          depending on the temperture selected:
          18-20: High AC
          21-23: Low AC
          24: HVAC Off
          25-27: Low Heat
          28-30: High Heat
    
          Revision 1.14  2007/02/26 17:43:03  nallyn
          Added the ability turn on extra timers (2) and turn off battery monitor and comfort monitor
        
          Revision 1.13  2007/02/23 18:11:28  nallyn
          Changed from 2 timers to 4 timers for testing purposes
    
          Revision 1.12  2006/03/01 23:43:17  nallyn
          enable the comfort monitor feature
    
          Revision 1.11  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers
    
       \endif
*/
/******************************************************************************/

#ifndef MENU_H
#define MENU_H

/* ---------- D e f i n i t i o n s ---------- */

// Defines for use in main feature presentations
#define MENU_FIRST_FEATURE  ( MENU_BEFORE_FIRST_FEATURE + 1 )
#define MENU_LAST_FEATURE   ( MENU_AFTER_LAST_FEATURE - 1 )
#define TEMPMON_HIGH_TEMP   ( 30 )  //!< Maximum temperature set point in C
#define TEMPMON_LOW_TEMP    ( 18 )  //!< Minimum temperature set point in C
#define CSTART_HIGH_TEMP    ( 5 )   //!< Maximum cold start temperature in C
#define CSTART_LOW_TEMP     ( -20 ) //!< Minimum cold start temperature in C
#define TEMPMON_HIGH_RANGE  ( 12 )  //!< Maximum range set point in C
#define TEMPMON_LOW_RANGE    ( 0 )  //!< Minimum range set point in C
#define FAN_SPEED_HIGH_ONLY	( 0 )
#define FAN_SPEED_LOW_HIGH	( 1 )

//!Menu states
typedef enum menu_state
{
    // Menu Off state
    MENU_ST_INIT,
    MENU_ST_OFF,
    MENU_ST_SETUP,
    
    // Feature states
    MENU_BEFORE_FIRST_FEATURE,// Add features after here and before LAST_FEATURE
    MENU_ST_TIMER1,
    MENU_ST_TIMER2,
#ifndef USE_COLDSTART
    MENU_ST_TIMER3,
    MENU_ST_TIMER4,
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    MENU_ST_TIMER5,
    MENU_ST_TIMER6,
    MENU_ST_TIMER7,
    MENU_ST_TIMER8,
    MENU_ST_TIMER9,
    MENU_ST_TIMER10,
#else
    MENU_ST_BATTMON,
    MENU_ST_TEMPMON,
    MENU_ST_SETTEMP,
#endif // EXTRA_TIMERS

#ifdef USE_COLDSTART        
    MENU_ST_COLDSTART,
#endif // USE_COLDSTART
    MENU_ST_MAXRUN,
    MENU_ST_CLOCK,
#ifndef EXTRA_TIMERS
    MENU_ST_CALIBRATE,
#endif // EXTRA_TIMERS
    MENU_ST_SYS_CFG,
    MENU_ST_DPF,
#ifdef OBM
    MENU_ST_FAULTS,
#endif // OBM
    MENU_ST_ACCESS,
    MENU_ST_CHANGE_PASS,
    MENU_AFTER_LAST_FEATURE,
// These features are partially done but are not in the feature selection menu
// To use these features, place them in the list above MENU_AFTER_LAST_FEATURE    
    
    // Feature substates
#ifndef EXTRA_TIMERS
    MENU_ST_BATMON_SELECT,
    MENU_ST_BATMON_SET,
    MENU_ST_TEMPMON_SELECT,
    MENU_ST_TEMPMON_SET_TEMP,
    MENU_ST_TEMPMON_SET_RANGE,
    MENU_ST_TEMPMON_SET_FAN_SPEED,
    MENU_ST_TEMPMON_SET_DURATION,
    MENU_ST_TEMP_SELECT,
#endif
#ifdef USE_COLDSTART        
    MENU_ST_CSTART_SELECT,
    MENU_ST_CSTART_SET_TEMP,
    MENU_ST_CSTART_SET_TIME,
#endif
    MENU_ST_MAXRUN_TIME,
    MENU_ST_MAXRUN_TIME_SET,
    MENU_ST_ACCESS_SELECT,

    MENU_ST_TIMER_SELECT,
    MENU_ST_TIMER_SET_HOUR,
    MENU_ST_TIMER_SET_MINUTE,
    MENU_ST_TIMER_SET_AM_PM,
    MENU_ST_TIMER_SET_DAY,
    MENU_ST_TIMER_SET_DURATION,
    MENU_ST_TIMER_SET_TEMP,

    MENU_ST_CLOCK_MODE,
    MENU_ST_CLOCK_HOUR,
    MENU_ST_CLOCK_MINUTE,
    MENU_ST_CLOCK_AM_PM,
    MENU_ST_CLOCK_DOW,          // day of week
    MENU_ST_CLOCK_MONTHDAY,
    MENU_ST_CLOCK_MONTH,
    MENU_ST_CLOCK_YEAR,

    MENU_ST_ADAPT_VOLTAGE,
    MENU_ST_RUN_HOURS,
#ifdef OBM
    MENU_ST_FAULTS_SELECT,
    MENU_ST_FAULTS_DISPLAY,
    MENU_ST_FAULTS_RESET,
#endif // OBM

    MENU_ST_SYS_CFG_SELECT,
    MENU_ST_SYS_CFG_LIST_DEVICES,
    MENU_ST_SYS_CFG_CONFIGURE,
    MENU_ST_SYS_CFG_DPF,
    MENU_ST_SYS_CFG_HEATERS,
    MENU_ST_DPF_SELECT,
    MENU_ST_DPF_START_REGEN,
    MENU_ST_DPF_STATUS,
    MENU_ST_DPF_VARS,

    // Administrative states
    MENU_ST_PASSWORD,
    MENU_ST_WRONG_PASSWORD,
    MENU_ST_NEW_PASSWORD,
    MENU_ST_CONFIRM_PASSWORD,
    MENU_ST_WRONG_CONFIRM
} MENU_STATE;

//!Sub menu choices
//!If you add entry here you probably want to add a string to choice_string
typedef enum menu_choice
{
    NO_CHOICE = 0,
    CHOOSE_OFF,
    CHOOSE_ON,
    CHOOSE_SET,
    CHOOSE_SET_TEMP,
    CHOOSE_SET_RANGE,
    CHOOSE_SET_FAN_SPEED,
    CHOOSE_SET_TIME,
    CHOOSE_SET_START,
    CHOOSE_SET_DURATION,
    CHOOSE_MODE_12,
    CHOOSE_MODE_24,
    CHOOSE_SUNDAY,          // Days of week must be contiguous
    CHOOSE_MONDAY,
    CHOOSE_TUESDAY,
    CHOOSE_WEDNESDAY,
    CHOOSE_THURSDAY,
    CHOOSE_FRIDAY,
    CHOOSE_SATURDAY,
    CHOOSE_WEEKDAYS,
    CHOOSE_WEEKENDS,
    CHOOSE_ALL_DAYS,
    CHOOSE_START_REGEN,
    CHOOSE_STATUS,
    CHOOSE_VARS,
    CHOOSE_CHECK,
    CHOOSE_CONFIGURE,
    CHOOSE_DPF,
    CHOOSE_HEATERS,
    CHOOSE_DISABLE,
    CHOOSE_ENABLE,
#ifdef OBM
    CHOOSE_VIEW_FAULTS,
    CHOOSE_RESET_FAULTS,
    CHOOSE_HIGH_ONLY,
    CHOOSE_LOW_HIGH,
#endif // OBM
    CHOOSE_NO,
    CHOOSE_YES
} MENU_CHOICE;

#ifdef OBM

typedef struct
{
    unsigned char read_now          :1; // set at the entry of menu or
                                        // each time an arrow button is pushed
    unsigned char record_ready      :1; // record has been read and is ready to be displayed

    unsigned int number_of_records;     // how many records do we have?
    unsigned int record_index;          // which record do we want to display

}OBM_RECORD_DISPLAY;

#endif // OBM
/* ---------- G l o b a l   D a t a ---------- */

extern bit mugClockMode24;
extern bit mugBlinkOn;
extern bit mugSetup;

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void menus( void );
const char * mufChoiceString( char chooser );
void Set_OBM_Read_Done_Flags (void);
void mufCenterLargeText( const char *label, char page );
void ResetFaults( void);

#endif


