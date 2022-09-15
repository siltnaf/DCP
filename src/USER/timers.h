/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file timers.h
       \brief Header file defining interface to module that maintains an array of countdown timers

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/timers.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/07/25 15:24:18  blepojevic
	      Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection
	
	      Revision 1.2  2014/04/16 14:07:10  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.1  2009/07/31 17:07:06  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.6  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.5  2007-10-26 18:42:14  msalimi
	      Added 5 minute cooldown for APU after DPF regeneration.
	      Changed DPF variable menu to show all variables in one screen.
	      Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
	      Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

	      Revision 1.4  2007-09-21 20:52:21  msalimi
	      *** empty log message ***

	      Revision 1.3  2007-09-17 18:41:36  msalimi
	      DPF related modifications

	      Revision 1.2  2007-08-01 20:20:37  nallyn
	      Added comments for Doxygen.

	      Revision 1.1  2007-07-05 20:10:13  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.14  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
	
	      Revision 1.13  2007/02/27 01:20:17  nallyn
	      Added 4 more timers for a total of 10 timers. The timers function differently
	      than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
	      timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
	      depending on the temperture selected:
	      18-20: High AC
	      21-23: Low AC
	      24: HVAC Off
	      25-27: Low Heat
	      28-30: High Heat
	
	      Revision 1.12  2007/02/26 17:49:52  nallyn
	      Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
	
	      Revision 1.11  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

#ifndef TIMERS_H
#define TIMERS_H

//!Structure of the timers
struct timer
{
	unsigned char blinkTmr;		//10ms resolution
#ifndef EXTRA_TIMERS
	unsigned char blinkBmTmr;	//10ms resolution
	unsigned char blinkTMTmr;	//10ms resolution
#endif // EXTRA_TIMERS
	unsigned char blinkMxTmr;	//10ms resolution
	unsigned char blinkCSTmr;	//10ms resolution
	unsigned char blinkT1Tmr;	//10ms resolution
	unsigned char blinkT2Tmr;	//10ms resolution
#ifndef USE_COLDSTART
	unsigned char blinkT3Tmr;	//10ms resolution
	unsigned char blinkT4Tmr;	//10ms resolution
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    unsigned char blinkT5Tmr;
    unsigned char blinkT6Tmr;
    unsigned char blinkT7Tmr;
    unsigned char blinkT8Tmr;
    unsigned char blinkT9Tmr;
    unsigned char blinkT10Tmr;
#endif // EXTRA_TIMERS
	unsigned char menuScanTmr;	//10ms resolution
	unsigned char menuTmr;		//second resolution
	unsigned char menu2Tmr;		//second resolution
	unsigned char menu3Tmr;		//10ms resolution
	unsigned int  bmTmr;		//second resolution
	unsigned int  cmTmr;		//second resolution
	unsigned char cmhTmr;	    //seconds timer to start high heat
	unsigned char bkLightTmr;	//second resolution
	unsigned char faultTmr;		//second resolution
	unsigned char waitTmr;		//10ms resolution
	unsigned char glowingTmr;	//second resolution
	unsigned char adcTmr;		//second resolution
	unsigned char apuOnTmr;		//second resolution
	unsigned char apuAcTmr;		//seconds timer to delay apu full loading
	unsigned int  restartTmr;	//minute resolution
	unsigned int  maxRunTmr;   	//minute resolution
	unsigned char minRunTmr; 	//minute resolution
	unsigned int  tmRunTmr;     //minute resolution
    unsigned char settleTmr;    //minute resolution
	unsigned int  APU_ONTmr;  	//minute resolution
	unsigned char a_cTmr;		//second resolution
	unsigned char b1Tmr;		//10ms resolution
	unsigned char b2Tmr;		//10ms resolution
	unsigned char readRTCTmr; 	//10ms resolution
    unsigned char OBM_Timer;    // 10ms resolution
    unsigned char CCU_Record_Timer; // second resolution
};

extern struct timer timers;
extern bit kpTick,msTick,sTick,mTick;
extern unsigned char milliseconds;
extern unsigned char seconds;
extern unsigned char minutes;
extern bit hTick;

void timerTick(void);

#endif //TIMER_H
