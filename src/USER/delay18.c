/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file delay18.c
       \brief Delay functions

<!-- DESCRIPTION: -->
<!-- NOTES: -->
       \note
	      See delay.h for details
	      Make sure this code is compiled with full optimization!!!

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/delay18.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include	"delay18.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
/* ---------- I n t e r n a l   D a t a ---------- */
/* ---------- G l o b a l   D a t a ---------- */

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DelayMs -->
       \brief Delay for the number of milliseconds specified

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  cnt     Number of milliseconds to delay
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DelayMs(unsigned char cnt)
{
#ifndef MM32F0133C7P		//? Add
	unsigned char i;
	while (cnt--) {
		i=4;
		while(i--) {
			DelayUs(uS_CNT);	/* Adjust for error */
		} ;
	} ;
#else
	DELAY_Ms(cnt);
#endif
}


