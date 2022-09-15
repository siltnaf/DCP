/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file menufunctions.h
       \brief Header file for module containing drawing support routines for LCD menus

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/menufunctions.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:38  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

#ifndef MENUFUNCTIONS_H
#define MENUFUNCTIONS_H

void fncShowArrows(char left_x, char right_x, char page);

void blink_digit(char x, char page, char number);

char change_digit(char num, char inc, char max, char min);

char change_number(char num, char inc, char max, char min);

signed char fncChangeSignedNumber(signed char num, signed char inc,
									signed char max, signed char min);


#endif //MENUFUNCTIONS_H

