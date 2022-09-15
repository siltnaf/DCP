/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file timekeep.h
       \brief Header file for day timer and timekeeping module

<!-- DESCRIPTION: -->
    To provide the interface to using timekeeping module.  This includes
    clock based timers and storage and display of clock time.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/timekeep.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.4  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.3  2007-09-17 18:41:36  msalimi
          DPF related modifications

          Revision 1.2  2007-08-01 20:19:45  nallyn
          Modified commenting for Doxygen.

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

#ifndef TIMEKEEP_H
#define TIMEKEEP_H

#include "tempmon.h"            // Interface to Temperature Monitor

/* ---------- D e f i n i t i o n s ---------- */

//! Weekday time is kept in an unsigned integer
//! weekday * hour * minute, max 7 * 24 * 60 = 10080 = 0x2760
//! define day values for weekdays, weekends and all (everyday)
typedef enum tkdayofweek
{
    TK_SUNDAY = 0,
    TK_MONDAY,
    TK_TUESDAY,
    TK_WEDNESDAY,
    TK_THURSDAY,
    TK_FRIDAY,
    TK_SATURDAY,
    TK_WEEKDAY,
    TK_WEEKEND,
    TK_EVERYDAY
} TKZDAYOFWEEK;

typedef struct tkztime
{
    char tkxDayOfWeek;
    char tkxHours;
    char tkxMinutes;
} TKZTIME;

typedef struct tkznvids
{
    char tkxEnableId;
    char tkxDurationId;
    char tkxTemperatureId;
    char tkxTimeId;
} TKZNVIDS;

typedef struct tkzrealtimer
{
    char        tkxEnable;
    char        tkxRun;
    char        tkxThermo;
    char        tkxIcon;
    char        tkxPriority;
    char        tkxAccess;
    char        tkxDuration;
    unsigned char tkxThermoCount;
    TM_CELSIUS  tkxTempSet;
    TKZTIME     tkxTime;
} TKZREALTIMER;

/* ---------- G l o b a l   D a t a ---------- */

// Clock
extern char tkgClockAccess;             // Password access to setting the clock

// Non-volatile Identifiers for the clock based timers
extern TKZNVIDS const tkgTimer1NvIds;
extern TKZNVIDS const tkgTimer2NvIds;
#ifndef USE_COLDSTART
extern TKZNVIDS const tkgTimer3NvIds;
extern TKZNVIDS const tkgTimer4NvIds;
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
extern TKZNVIDS const tkgTimer5NvIds;
extern TKZNVIDS const tkgTimer6NvIds;
extern TKZNVIDS const tkgTimer7NvIds;
extern TKZNVIDS const tkgTimer8NvIds;
extern TKZNVIDS const tkgTimer9NvIds;
extern TKZNVIDS const tkgTimer10NvIds;
#endif // EXTRA_TIMERS

// Clock based timers
extern TKZREALTIMER tkgTimer1;          // TIMER 1 feature
extern TKZREALTIMER tkgTimer2;          // TIMER 2 feature
#ifndef USE_COLDSTART
extern TKZREALTIMER tkgTimer3;          // TIMER 3 feature
extern TKZREALTIMER tkgTimer4;          // TIMER 4 feature
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
extern TKZREALTIMER tkgTimer5;          // Timer 5 Feature
extern TKZREALTIMER tkgTimer6;          // Timer 6 Feature
extern TKZREALTIMER tkgTimer7;          // Timer 7 Feature
extern TKZREALTIMER tkgTimer8;          // Timer 8 Feature
extern TKZREALTIMER tkgTimer9;          // Timer 9 Feature
extern TKZREALTIMER tkgTimer10;          // Timer 10 Feature
#endif // EXTRA_TIMERS

// Maximum runtime
extern char tkgMaxRunAccess;            //!< Password access to maximum run time
extern bit tkgMaxRunEnable;             //!< Is maximum run timer active?
extern bit tkgMaxRunTimedOut;           //!< Has maximum run time been reached?
extern char tkgMaxRunTime;              //!< Maximum run time in tenths of hours

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void tkfTimeMonitor( TKZREALTIMER *tp );
void tkfMaxTimer( void );

#endif  // TIMEKEEP_H
