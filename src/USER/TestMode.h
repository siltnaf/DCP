/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file TestMode.h
       \brief Header file for module that reads and writes to EEPROM

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright ?2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/eeprom.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

#ifndef TESTMODE_H
#define TESTMODE_H

#include "types.h"

/* ---------- F u n c t i o n  P r o t o t y p e s ------------------------- */
void TestMode(void);
u8 test_mode_process(void);

#endif //TESTMODE_H
