/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file delay18.h
       \brief Header file for delay functions 

<!-- DESCRIPTION: -->
<!-- NOTES: -->
       \note
         \code
	      Functions available:
		      DelayUs(x)	Delay specified number of microseconds
		      DelayMs(x)	Delay specified number of milliseconds

	      Note that there are range limits: 
	      - on small values of x (i.e. x<10), the delay becomes less
	      accurate. DelayUs is accurate with xtal frequencies in the
	      range of 4-16MHZ, where x must not exceed 255. 
	      For xtal frequencies > 16MHz the valid range for DelayUs
	      is even smaller - hence affecting DelayMs.
	      To use DelayUs it is only necessary to include this file.
	      To use DelayMs you must include delay.c in your project.

	      Set the crystal frequency in the CPP predefined symbols list in
	      HPDPIC, or on the PICC commmand line, e.g.
	      picc -DXTAL_FREQ=4MHZ

	      or
	      picc -DXTAL_FREQ=100KHZ

	      Note that this is the crystal frequency, the CPU clock is
	      divided by 4.

	      MAKE SURE this code is compiled with full optimization!!!
         \endcode

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/delay18.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.5  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.4  2007-09-17 18:41:35  msalimi
	      DPF related modifications

	      Revision 1.3  2007-08-27 22:19:08  msalimi
	      CAN communication working.

	      Revision 1.2  2007-07-17 20:44:58  msalimi
	      Timing fixes.

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

#ifndef DELAY18_H
#define DELAY18_H

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "delay.h"			//? Add

/* ---------- D e f i n i t i o n s ------------------------ */

#define	MHZ	*1

#ifndef	XTAL_FREQ
   #define	XTAL_FREQ	40MHZ		/* Crystal frequency in MHz */
#endif

// For millisecond delays, compensate for some code overhead time
#define uS_CNT  246     // quarter millisecond time, ideally 250 uS

#define FREQ_MULT	((XTAL_FREQ)/(4MHZ))
//?#if FREQ_MULT < 2
//?#error "Clock must be >= 8"
//?#endif

// Inner _dcnt while loop is 2 machine instructions
#ifndef MM32F0133C7P		//? Add
#define	DelayUs(x)  {   unsigned char _dcnt, _rcnt;     \
                        _rcnt = FREQ_MULT/2;            \
                        while( --_rcnt > 0 )            \
                        {                               \
                            _dcnt = x;                  \
			  while(--_dcnt > 0) \
				{\
                            }                           \
                        }                               \
		} 
#else
#define	DelayUs(x)	DELAY_Us(x)
#endif 

/* ---------- F u n c t i o n   P r o t o t y p e s ------------------ */
void DelayMs(unsigned char);

#endif // DELAY18_H
