/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file adc18x.h
       \brief Header file for the ADC unit

<!-- DESCRIPTION: -->
<!-- NOTES: -->
    When initializing the ADC, initialization should be done in sequence from 
    lowest number to highest number

       \if ShowHist
Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
	This source file and the information contained in it are confidential and 
	proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in 
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

CHANGE HISTORY:

	$Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/adc18x.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	$Log: not supported by cvs2svn $
	Revision 1.3  2014/04/16 14:07:07  blepojevic
	Added diagnostic of APU and CCU
	
	Revision 1.2  2013/09/19 13:07:43  blepojevic
	Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements
	
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

#ifndef ADC18_H
#define ADC18_H

#include "tfxstddef.h"

/* ---------- D e f i n i t i o n s ---------- */


/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */
extern int8 InitialiseADC( uint8 u8NumChannels ); 
extern int8 SetADCChannel( uint8 ADC_Channel );
extern uint16 Read_ADC( void );
extern uint8 ReadADCeight( void );

#endif  // ADC18_H
