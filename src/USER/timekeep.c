/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file timekeep.c
       \brief Module for day timer and timekeeping

<!-- DESCRIPTION: -->
    To provide functions for keeping time (clock timers) and for
    storage and display of clock time.

<!-- NOTES: -->
       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/timekeep.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.2  2014/07/25 15:24:17  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.3  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.2  2007-09-17 18:41:36  msalimi
          DPF related modifications

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

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "timekeep.h"           // Interface for clock time keeping
#include "main.h"               // Common definitions
#include "apu.h"                // Interface to APU State Machine
#include "rtclock.h"            // Interface to real-time clock
#include "timers.h"             // Interface to software counter timers
#include "iconblinker.h"
#include "nvmem.h"              // Interface to Non-Volatile Memory storage


/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

#define TK_HOUR_MINUTES     60
#define TK_DAY_HOURS        24
#define TK_DAY_MINUTES      ( 24 * 60 )

char tkfTempControl(signed char temperature);

/* ---------- I n t e r n a l   D a t a ---------- */


/* ---------- G l o b a l   D a t a ---------- */

// Clock
char tkgClockAccess;                // Password access to setting the clock

// Clock based timers
TKZREALTIMER tkgTimer1;             // TIMER 1 feature
TKZREALTIMER tkgTimer2;             // TIMER 2 feature
#ifndef USE_COLDSTART
TKZREALTIMER tkgTimer3;             // TIMER 3 feature
TKZREALTIMER tkgTimer4;             // TIMER 4 feature
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
TKZREALTIMER tkgTimer5;             // TIMER 5 feature
TKZREALTIMER tkgTimer6;             // TIMER 6 feature
TKZREALTIMER tkgTimer7;             // TIMER 7 feature
TKZREALTIMER tkgTimer8;             // TIMER 8 feature
TKZREALTIMER tkgTimer9;             // TIMER 9 feature
TKZREALTIMER tkgTimer10;            // TIMER 10 feature
#endif // EXTRA_TIMERS

TKZNVIDS const tkgTimer1NvIds =
{
    NV_ID_TIMER1_ENABLE,
    NV_ID_TIMER1_DUR,
    NV_ID_TIMER1_TEMP,
    NV_ID_TIMER1_TIME
};

TKZNVIDS const tkgTimer2NvIds =
{
    NV_ID_TIMER2_ENABLE,
    NV_ID_TIMER2_DUR,
    NV_ID_TIMER2_TEMP,
    NV_ID_TIMER2_TIME
};
#ifndef USE_COLDSTART
TKZNVIDS const tkgTimer3NvIds =
{
    NV_ID_TIMER3_ENABLE,
    NV_ID_TIMER3_DUR,
    NV_ID_TIMER3_TEMP,
    NV_ID_TIMER3_TIME
};

TKZNVIDS const tkgTimer4NvIds =
{
    NV_ID_TIMER4_ENABLE,
    NV_ID_TIMER4_DUR,
    NV_ID_TIMER4_TEMP,
    NV_ID_TIMER4_TIME
};
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
TKZNVIDS const tkgTimer5NvIds =
{
    NV_ID_TIMER5_ENABLE,
    NV_ID_TIMER5_DUR,
    NV_ID_TIMER5_TEMP,
    NV_ID_TIMER5_TIME
};

TKZNVIDS const tkgTimer6NvIds =
{
    NV_ID_TIMER6_ENABLE,
    NV_ID_TIMER6_DUR,
    NV_ID_TIMER6_TEMP,
    NV_ID_TIMER6_TIME
};

TKZNVIDS const tkgTimer7NvIds =
{
    NV_ID_TIMER7_ENABLE,
    NV_ID_TIMER7_DUR,
    NV_ID_TIMER7_TEMP,
    NV_ID_TIMER7_TIME
};

TKZNVIDS const tkgTimer8NvIds =
{
    NV_ID_TIMER8_ENABLE,
    NV_ID_TIMER8_DUR,
    NV_ID_TIMER8_TEMP,
    NV_ID_TIMER8_TIME
};

TKZNVIDS const tkgTimer9NvIds =
{
    NV_ID_TIMER9_ENABLE,
    NV_ID_TIMER9_DUR,
    NV_ID_TIMER9_TEMP,
    NV_ID_TIMER9_TIME
};

TKZNVIDS const tkgTimer10NvIds =
{
    NV_ID_TIMER10_ENABLE,
    NV_ID_TIMER10_DUR,
    NV_ID_TIMER10_TEMP,
    NV_ID_TIMER10_TIME
};
#endif // EXTRA_TIMERS

// Maximum runtime
char tkgMaxRunAccess;               // Password access to maximum run time
bit tkgMaxRunEnable;                // Is maximum run timer active?
bit tkgMaxRunTimedOut = FALSE;      // Has maximum run time been reached?
char tkgMaxRunTime = 80;            // Tenths of hours, default 8.0 hours

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tkfMatchDay -->
       \brief Checks if today is within specified timerDay region or matches a specific timerDay 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  today    Enumerated Day of week
       @param[in]  timerDay Enumerated region to match (TK_WEEKDAY, TK_WEEKEND, or TK_EVERYDAY)
                            else a specific timerDay to match (from TK_SUNDAY to TK_SATURDAY)
<!-- RETURNS: -->
      \return TRUE if today matches specified timerDay region, FALSE otherwise

*/
/******************************************************************************/
static char tkfMatchDay( char today, char timerDay )
{
    switch ( timerDay )
    {
        case TK_WEEKDAY:
            return ( ( today >= TK_MONDAY ) && ( today <= TK_FRIDAY ) );
        case TK_WEEKEND:
            return ( ( today == TK_SUNDAY ) || ( today == TK_SATURDAY ) );
        case TK_EVERYDAY:
            return ( today <= TK_SATURDAY );
        default:
            return ( today == timerDay );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tkfTimeMonitor -->
       \brief Update local timer from RTC clock and determine if there is a scheduled APU run event for the specified timer

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  tp   Pointer to TKZREALTIMER structure (timer schedule structure corresponding to each day timer)
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void tkfTimeMonitor( TKZREALTIMER *tp )
{
    int dayMinutes;
    int startMinutes;
    int endMinutes;
    char startDay;
    char shouldRun;
    char thermo;
    char iconCtrl;
    char runable;
    
    // Assume the timer should not be running, temperature not controlled
    shouldRun = FALSE;
    thermo = TM_SEARCHING;
    
    // If timer and clock are enabled icon defaults to on, otherwise off
    if ( rtgClock.rtxReady && tp->tkxEnable )
        iconCtrl = ICON_ON;
    else
        iconCtrl = ICON_OFF;
    
    runable = ( !apuFaultLockout && AUTO && !BREAKER_TRIP );
    
    // If timer and clock are enabled
    // and the fault lockout is not set
    if ( rtgClock.rtxReady && tp->tkxEnable && runable )
    {
        // Convert the current time to minutes into the day
        dayMinutes = rtgClock.rtxHours * 60 + rtgClock.rtxMinutes;
        
        // Starting day is the current day, adjusted later if necessary
        startDay = rtgClock.rtxDayOfWeek;
        
        // Starting time (in minutes into the day) is from the timer
        startMinutes = tp->tkxTime.tkxHours * 60 + tp->tkxTime.tkxMinutes;
        
        // Add the duration (in minutes) to the start time
        endMinutes = startMinutes + HOUR_TENTHS_TO_MINUTES( tp->tkxDuration );
        
        // If the ending time is past the end of a day
        if ( endMinutes > TK_DAY_MINUTES )
        {
            // Adjust the end time by a day
            endMinutes -= TK_DAY_MINUTES;
            
            // If the current time is less than the end time
            if ( dayMinutes < endMinutes )
            {
                // Wrap the day of week to the previous day
                // which should be the starting day of week
                startDay = ( startDay == 0 ? 6 : startDay - 1 );
                    
                // If the day matches the start condition
                if ( tkfMatchDay( startDay, tp->tkxTime.tkxDayOfWeek ) )
                {
                    // Timer should be running
                    shouldRun = TRUE;
                }
            }
            // If current time is past the starting time in the same day
            else if ( dayMinutes >= startMinutes )
            {
                // If the day matches
                if ( tkfMatchDay( startDay, tp->tkxTime.tkxDayOfWeek ) )
                {
                    // Timer should be running
                    shouldRun = TRUE;
                }
            }
        }
        else
        {
            // If the current time is past start and before end
            if ( ( dayMinutes >= startMinutes ) && ( dayMinutes < endMinutes ) )
            {
                // If the day matches
                if ( tkfMatchDay( startDay, tp->tkxTime.tkxDayOfWeek ) )
                {
                    // Timer should be running
                    shouldRun = TRUE;
                }
            }
        }
    }
    
    // If we are set to run
    if ( shouldRun )
    {
        // If there is no higher priority
        if ( apuPriority( tp->tkxPriority ) )
        {
            // Set heating/cooling control in timer record
#ifdef EXTRA_TIMERS
            thermo = tkfTempControl( tp->tkxTempSet );
#else
            thermo = tmfThermostat( tp->tkxThermo,
                                    tp->tkxTempSet,
                                    &(tp->tkxThermoCount) );
#endif // EXTRA_TIMERS
        
            // Icon should blink
            iconCtrl = ICON_BLINK;
        }
    }
    
    // Set icon
    iconControl( tp->tkxIcon, iconCtrl );
    
    // Assign the working run value to the timer record

    if ((thermo == TM_HEATING) && (!HEAT_ENABLE))
    {
        shouldRun = FALSE;
    }
    
    tp->tkxRun = shouldRun;
    tp->tkxThermo = thermo;
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: tkfMaxTimer -->
       \brief Check if max run time has expired and update icon as appropriate

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void tkfMaxTimer( void )
{
    // If Timing of Runtime is enabled
    if ( tkgMaxRunEnable )
    {
        // If we have not reached max run time
        if ( !tkgMaxRunTimedOut )
        {
            // Max Run enabled but not reached, icon should be on
            iconControl( ICON_MAXRUN, ICON_ON );
        }
        else
        {
            // Have reached max run time, icon should blink
            iconControl( ICON_MAXRUN, ICON_BLINK );
        }
    }
    else
    {
        // Max Run not enabled, icon should be off
        iconControl( ICON_MAXRUN, ICON_OFF );
    }
}



/******************************************************************************/
/*!

<!-- FUNCTION NAME: tkfTempControl -->
       \brief Identify thermostat heating cycle state as a function of supplied temperature

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  temperature   Temperature value (8-bit signed integer) to classify
<!-- RETURNS: -->
      \return Integer value (enum tmthermostat constant) respresenting thermostat status

*/
/******************************************************************************/
char tkfTempControl(signed char temperature)
{
    if ( temperature < 24 )
    {
        return TM_COOLING;
    }
    
    if ( temperature == 24 ) 
    {
        return TM_SEARCHING;
    }
    
    return TM_HEATING;
}
