/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file keypad.h
       \brief Header file for crosspoint keypad input device driver module

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/keypad.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.3  2006/02/08 19:06:38  nallyn
	      Modularized code by taking functions out of headers and placing them in .c files
	      Updated file headers to match proposed Teleflex coding standard
	
       \endif
*/
/******************************************************************************/

#ifndef KEYPAD_H
#define KEYPAD_H

void KPD_fnInitKeypad(void);
void KPD_fnKeypad(void);

#endif // KEYPAD_H
