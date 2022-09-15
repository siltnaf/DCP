/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file iconblinker.h
       \brief Header file for control of icon blinking
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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/iconblinker.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-10-09 15:03:18  msalimi
	      Version 35-PPP-002. Second round of testing..

	      Revision 1.2  2007-08-27 22:19:08  msalimi
	      CAN communication working.

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
	
              Revision 1.12  2007/02/26 17:26:04  nallyn
	      Added Timer3-6 icons
	
	      Revision 1.11  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/


#ifndef ICONBLINKER_H
#define ICONBLINKER_H

/* ---------- D e f i n i t i o n s ---------- */
#define ICON_BATTERY        0
#define ICON_COOL           1
#define ICON_HEAT           2
#define ICON_OUTLET_ON      3
#define ICON_APU_ON         4
#define ICON_APU_BAR        5
#define ICON_MAXRUN         6
#define ICON_SHORE_POWER    7
#define ICON_OUTLET_TRIP    8
#define ICON_HEAT_TRIP      9
#define ICON_OUTLET_RESET   10
#define ICON_OUTLET_OFF     11
#define ICON_COLDSTART      12
#define ICON_TIMER1         13
#define ICON_TIMER2         14
#define ICON_TEMPMON        15
#define ICON_TIMER3         16
#define ICON_TIMER4         17

#ifdef EXTRA_TIMERS

#define ICON_TIMER5         18
#define ICON_TIMER6         19
#define ICON_TIMER7         20
#define ICON_TIMER8         21
#define ICON_TIMER9         22
#define ICON_TIMER10        23

#endif // EXTRA_TIMERS

typedef enum
{
    ICON_OFF,
    ICON_ON,
    ICON_BLINK,
    ICON_BLINK_ASYMETRIC_ON,
    ICON_BLINK_ASYMETRIC_OFF,
    ICON_BLINK_SLOW,
    ICON_BLINK_FAST
} ICONCTRL;

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */
void iconMakeVisible( char allow );
void iconShow( char iconId );
void iconErase( char iconId );
void iconControl( char iconId, ICONCTRL ctrl );

#endif //ICONBLINKER_H

