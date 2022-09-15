/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file apu.h
       \brief Header file defining interface to APU state machine

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

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/apu.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.4  2014/08/25 19:31:34  blepojevic
          Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

          Revision 1.3  2014/06/27 17:35:27  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.2  2014/04/16 14:07:07  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.1  2009/07/31 21:26:18  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.11  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.10  2008-03-26 21:27:50  msalimi
          Fixed side effects of recent changes to how APU warmup period is handled for regeneration.

          Revision 1.9  2007-12-06 23:21:32  msalimi
          Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

          Revision 1.8  2007-11-30 18:52:07  msalimi
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

          Revision 1.7  2007-10-24 22:22:12  msalimi
          Removed unused flag.

          Revision 1.6  2007-10-19 21:12:42  msalimi
          New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

          Revision 1.5  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 19:39:04  nallyn
          Modified comments for Doxygen

          Revision 1.1  2007-07-05 20:10:12  gfinlay
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
      
          Revision 1.12  2007/02/26 17:22:16  nallyn
          Added Timer5 and Timer6 with the ability to choose the extra timers or keep the battery monitor and coldstart functions.
      
          Revision 1.11  2007/02/23 18:11:28  nallyn
          Changed from 2 timers to 4 timers for testing purposes
      

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

#ifndef APU_H
#define APU_H

#include "tfxstddef.h"     // Teleflex Standard definitions

/* ---------- D e f i n i t i o n s ---------- */

#define adLSBs   (1024L)        //!< number of A/D lsb's for Fan A/D converter
#define adRange  (5000L)        //!< Fan A/D full-scale range in mvolts
#define adCal(thresh) ((thresh * adLSBs) / adRange)   //!< where thresh is in millivolts


// Reasons for the APU to be ON, higher number is higher priority
#ifndef EXTRA_TIMERS
#define APU_ON_BY_BATTMON       0x80
#define APU_ON_BY_COLDSTART     0x40
#else
#define APU_ON_BY_TIMER10       0x88
#define APU_ON_BY_TIMER9        0x84
#define APU_ON_BY_TIMER8        0x82
#define APU_ON_BY_TIMER7        0x81
#define APU_ON_BY_TIMER6        0x80
#define APU_ON_BY_TIMER5        0x40
#endif // EXTRA_TIMERS

#define APU_ON_BY_TIMER4        0x20
#define APU_ON_BY_TIMER3        0x10
#define APU_ON_BY_TIMER2        0x08
#define APU_ON_BY_TIMER1        0x04
#define APU_ON_BY_TEMPMON       0x02
#define APU_ON_BY_MINIMUM_RUN   0x01

#define APU_WARMUP_TIME         120     // seconds

// SPN definitions for APU faults
#define SPN_APU_OIL_PRESS   	100
#define FMI_APU_LOW_OIL_PRESS   1

#define SPN_APU_COOLANT_TEMP	110
#define FMI_APU_OVERTEMP   	0

#define SPN_APU_ALT_STATUS	3353
#define FMI_APU_ALT_FB   	12

typedef struct
{
    unsigned char Crtical_Shutdown      :1;
    unsigned char Noncritical_Shutdown  :1;
    unsigned char APU_On_Off_Flag       :1;
    unsigned char APU_Off_Requested     :1;
    unsigned char APU_Off_Now           :1;
    unsigned char Emergency_Regen_Sent_Once :1;
    unsigned char APU_Warmed_Up			:1;
    unsigned char APU_Warmup_Timer;
} APU_DIAG;

/* ---------- G l o b a l   D a t a ---------- */

extern APU_DIAG APU_Diag;

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void decode_oil_oh_wait( volatile int tmp );
void blink_apu( void );
void shore_power( void );
void apuClearMinRun( void );
char apuPriority( char priority );
void apu( void );
void APU_fnLoadControl( bool bLoadsOff );
void Record_APU_Status_External (void);
void APU_Critical_ShutDown (void);
void APU_Critical_Shutdown_Can_Clear(void);
void Initialize_Diag (void);

#endif  // APU_H
