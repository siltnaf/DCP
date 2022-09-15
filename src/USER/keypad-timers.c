/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file keypad-timers.c
       \brief Module provides functions to maintain an array of countdown timers for the keypad

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/keypad-timers.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-08-01 20:06:40  nallyn
	      Added commenting for Doxygen

	      Revision 1.2  2007-07-10 22:36:19  gfinlay
	      Updated timer programming for correct values to get a 1msec tick with 40MHz clock.

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 20:18:01  nallyn
	      Removal of unused code
	
	      Revision 1.1  2006/02/08 19:06:38  nallyn
	      Modularized code by taking functions out of headers and placing them in .c files
	      Updated file headers to match proposed Teleflex coding standard

      \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s -------------- */
//?#include <pic18.h>
#include "timers.h"
#include "keypad-timers.h"
#include "tim14.h"			//? Add

/* ---------- I n t e r n a l   D e f i n i t i o n s ------ */

/* ---------- I n t e r n a l   D a t a -------------------- */
//!The structure of keypad timers
struct kptimer kptimers;
/* ---------- G l o b a l   D a t a ------------------------ */

/* ---------- F u n c t i o n s ---------------------------- */
/******************************************************************************/
/*!

<!-- FUNCTION NAME: ini_keypad_timers -->
       \brief Initializes the SFR registers for the keypad timers

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void init_keypad_timers(void)
{
#ifndef MM32F0133C7P		//? Add
 	//OPTION = 0x07;
 	T1CON = 0b00100101;
    TMR1L=0x58; 
    TMR1H=0x9E;
#else		///?OK
		Keypad_Timer_Init();
#endif
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: keypadTimerTick -->
       \brief Routine to update list of keypad timers  - called from ISR during tick function

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void keypadTimerTick(void)
{
	if (kpTick==1)
	{
#ifndef MM32F0133C7P		//? Add
		TMR1IE = 0;
		kpTick = 0;
		TMR1IE = 1;
#else		///?OK
		kpTick = 0;
#endif

		if(kptimers.lbTmr != 0)
		{
			--kptimers.lbTmr;
		}
		if(kptimers.rbTmr != 0)
		{
			--kptimers.rbTmr;
		}
		if(kptimers.fbTmr != 0)
		{
			--kptimers.fbTmr;
		}

		if(kptimers.oobTmr != 0)
		{
			--kptimers.oobTmr;
		}

		if(kptimers.sbTmr != 0)
		{
			--kptimers.sbTmr;
		}
		if(kptimers.acbTmr != 0)
		{
			--kptimers.acbTmr;
		}
		if(kptimers.tdbTmr != 0)
		{
			--kptimers.tdbTmr;
		}
		if(kptimers.tubTmr != 0)
		{
			--kptimers.tubTmr;
		}
		if(kptimers.hbTmr != 0)
		{
			--kptimers.hbTmr;
		}
		if(kptimers.obTmr != 0)
		{
			--kptimers.obTmr;
		}
		if(kptimers.brbTmr != 0)
		{
			--kptimers.brbTmr;
		}
		if(kptimers.fanTmr != 0)
		{
			--kptimers.fanTmr;
		}
	}
}
