/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file keypad-timers.h
       \brief Header file for module providing functions to maintain an array of countdown timers for the keypad

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/keypad-timers.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.3  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.2  2007-08-01 20:07:15  nallyn
	      Added commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 19:06:38  nallyn
	      Modularized code by taking functions out of headers and placing them in .c files
	      Updated file headers to match proposed Teleflex coding standard
	
       \endif
*/
/******************************************************************************/

#ifndef KEYPAD_TIMERS_H
#define KEYPAD_TIMERS_H

/* ---------- D e f i n i t i o n s ------------------------ */
//!The keypad timers
struct kptimer
{
	unsigned char lbTmr;
	unsigned char rbTmr;
	unsigned char fbTmr;	//10ms resolution
	unsigned char oobTmr;	//10ms resolution
	unsigned char sbTmr;	//10ms resolution
	unsigned char acbTmr;	//10ms resolution
	unsigned char tdbTmr;	//10ms resolution
	unsigned char tubTmr;	//10ms resolution
	unsigned char hbTmr;	//10ms resolution
	unsigned char obTmr;	//10ms resolution
	unsigned char brbTmr;	//10ms resolution
	unsigned char fanTmr;	//10ms resolution
};

extern struct kptimer kptimers;

/* ---------- F u n c t i o n   P r o t o t y p e s -------- */
void init_keypad_timers(void);
void keypadTimerTick(void);

#endif   // KEYPAD_TIMERS_H


