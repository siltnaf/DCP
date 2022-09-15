/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file rtclock.h
       \brief Header file defining interface to driver functions for RTC Real-time Clock device

<!-- DESCRIPTION: -->
         To provide the interface to using the Real-time Clock device

            \code
              GLOBAL FUNCTIONS:
                rtfpoll             - Poll for completion of reads and writes
                rtfEnableWrite      - Send write enable message to RTC device
                rtfSetClock         - Send time to RTC device
                rtfRequestClock     - Start a read request for time to RTC device
            \endcode


<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/rtclock.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.4  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.3  2007-09-21 20:39:18  msalimi
          *** empty log message ***

          Revision 1.2  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.11  2006/02/08 20:23:06  nallyn
          Changes based on code review
          Eliminated magic numbers

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

#ifndef RTCLOCK_H
#define RTCLOCK_H

/* ---------- D e f i n i t i o n s ---------- */

#define RT_DAYS_IN_WEEK     7

typedef struct rtztime
{
    char rtxDayOfWeek;
    char rtxHours;
    char rtxMinutes;
    char rtxSeconds;
    char rtxDate;
    char rtxMonth;
    char rtxYear;
    char rtxReady;
} RTZTIME;

    
/* ---------- G l o b a l   D a t a ---------- */

extern RTZTIME rtgClock;

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void rtfPoll( void );
void rtfEnableWrite( void );
void rtfSetClock( char day, char hours, char minutes, char date, char month, char year );
void rtfRequestClock( void );
void Record_Real_Time_And_Date (unsigned char device);

#endif

