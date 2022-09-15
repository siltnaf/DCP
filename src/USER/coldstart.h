/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file coldstart.h
       \brief Header file for module to provided functionality for Coldstart Extended Mode

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/coldstart.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/04/16 14:07:08  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.2  2013/09/19 13:07:43  blepojevic
	      Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements
	
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.3  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.2  2007-08-01 20:00:14  nallyn
	      Modified commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.13  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
	
              Revision 1.12  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.11  2006/02/08 20:11:47  nallyn
	      Changes based on code review
	      Updated the temperature interpolation routine
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n s ------------------------ */

#ifndef COLDSTART_H
#define COLDSTART_H

//!Structure to define the Cold start parameters
typedef struct
{
	unsigned char ucAccess;			    //!< enable cold monitor user access
	unsigned char ucColdStart; 		    //!< cold start selection state variable
	unsigned char uiColdRunTime;		//!< time to run unit when cold started
	signed char cStartTemp;				//!< default start temp in C
	unsigned int uiStartTempVoltage;	//!< voltage corresponding to start temp
}gtCOLD_START;

#ifdef USE_COLDSTART
extern gtCOLD_START tCOLD_START;
extern bit bColdMonitor;
extern bit bCmRun;
extern bit bCmRunHighHeat;
#endif

/* ---------- F u n c t i o n s ---------------------------- */

/********************************************************************
	function takes in a value of temperature in the range of
 5 to -20 C and returns the 10 bit adc count based on the
 hefei thermostatic NTC temp sensor
********************************************************************/
#ifdef USE_COLDSTART        
unsigned int tmfCtoVad( signed char temp );
char cold_start_monitor(void);
#endif

#endif //COLDSTART_H

