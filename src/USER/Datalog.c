/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file Datalog.c
       \brief Implements support for data logging 

<!-- DESCRIPTION: -->
       Implements support for data logging 

<!-- NOTES: -->
       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/Datalog.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/12/17 22:00:52  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.9  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.8 Thu Jan 31 20:54:29 2008
         -Added feature to clear DPF missing fault when DPF is uninstalled.
         -Made external and on-board memory applications independant of each other.

         Revision 1.7 Fri Nov 30 18:52:07 2007 
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

         Revision 1.6 Fri Oct 26 18:42:14 2007 
         Added 5 minute cooldown for APU after DPF regeneration.
         Changed DPF variable menu to show all variables in one screen.
         Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
         Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

         Revision 1.5 Tue Oct 23 18:57:39 2007 
         Fixed indexing errors related to the long record.

         Revision 1.4 Tue Oct 16 20:40:22 2007 
         Version 35-DCP-004. External memory error correction.

         Revision 1.3 Mon Oct 15 16:21:08 2007 
         Fixing memory error.

         Revision 1.2 Fri Sep 21 20:48:49 2007 
         Fixed external memory detection bug. The flag was being cleared after detection.

         Revision 1.1 Mon Sep 17 18:40:16 2007 
         Datalogging features.

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s -------------- */
#include "Datalog.h"
#include "tfxstddef.h"
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "rtclock.h"

/* ---------- I n t e r n a l   D e f i n i t i o n s ------ */


/* ---------- I n t e r n a l   D a t a -------------------- */
static void Log_Record (void);

/* ---------- G l o b a l   D a t a ------------------------ */
DATALOG Datalog;
unsigned char Long_Record [MAX_LONG_LOG_LENGTH];


/* ---------- F u n c t i o n s ---------------------------- */

void Init_Datalogging (void)
{
    Datalog.Logging_On = FALSE;
    Datalog.Sync_Insertion_Counter = 0;
    Datalog.Index = 0;
}

void Start_Datalog(void)
{
    Datalog.Logging_On = TRUE;
    Datalog.Sync_Insertion_Counter = 0;

    Long_Record[0] = SYNC_BYTE;
    Long_Record[1] = SYNC_BYTE;

    Long_Record[2] = RTC_RECORD;

    Long_Record[3] = (rtgClock.rtxMinutes << 6);
    Long_Record[3] += rtgClock.rtxSeconds;

    Long_Record[4] = (rtgClock.rtxMinutes >> 2);
    Long_Record[4] += (rtgClock.rtxHours << 4);

    Long_Record[5] = (rtgClock.rtxHours >> 4);
    Long_Record[5] += (rtgClock.rtxDate << 1);
    Long_Record[5] += (rtgClock.rtxMonth << 6);

    Long_Record[6] = (rtgClock.rtxMonth >> 2);
    Long_Record[6] += (rtgClock.rtxYear << 2);

    Long_Record[7] = LONG_RECORD;

    // Long_Record[8] is reserved for the length of the record

    Datalog.Index = 8;      // Will be incremented before writing
}

void Stop_Datalog(void)
{
    Datalog.Logging_On = FALSE;
    // Log what we've got
    Log_Record();
}

void Add_To_Long_Record(unsigned int pressure, unsigned int temperature)
{
    if (Datalog.Index <= (MAX_LONG_LOG_LENGTH - 3))
    {
        Datalog.Index++;
        Long_Record[Datalog.Index] = pressure;
        Datalog.Index++;
        Long_Record[Datalog.Index] = temperature;
    }
    if (Datalog.Index >= (MAX_LONG_LOG_LENGTH - 2))
    {
        // The record is full, log it
        Log_Record();
        // Start a new one
        Start_Datalog();
    }
}

static void Log_Record (void)
{
    Long_Record[8] = (Datalog.Index + 1);   // zero indexed buffer
#ifdef EXT_MEM
    (void) EXM_fnLogData (Long_Record, Long_Record[8]);
#endif // EXT_MEM
}

#ifdef EXT_MEM
void Increment_Sync_Insertion_Counter(void)
{
unsigned char Data[2];

    Datalog.Sync_Insertion_Counter++;
    if (Datalog.Sync_Insertion_Counter == 10)
    {
        // Record sync bytes to the external memory
        Data[0] = SYNC_BYTE;
        Data[1] = SYNC_BYTE;
        (void) EXM_fnLogData (Data, 2);

        Datalog.Sync_Insertion_Counter = 0;
    }
}
#endif // EXT_MEM
