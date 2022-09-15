/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file hrmeter2.h
       \brief Header file for module to provide hours of APU runtime

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/hrmeter2.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2009/07/31 16:49:52  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.3  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.2  2008-02-06 16:29:56  msalimi
	      - Corrected low voltage SPN. This was 5 and now is 4.

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.3  2006/02/08 20:15:33  nallyn
	      Updated based on code review.
	      Eliminated commented out and unused code.  More comments on how the routine works.
	
	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

#ifndef	_HRMETER_H_
#define _HRMETER_H_

#define HRMETER_ADDR_BASE       0       // Top of hour meter block in EEPROM

// public function prototypes
void HourMeterPutHours( long hours );
void HourMeter_init(void);
long HourMeter(void);	// Called in main to increment hours
void HourMeter_interrupt(unsigned char increment); // where increment is a boolean variable


#endif  // _HRMETER_H_
