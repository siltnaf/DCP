/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file pwm18x.h
       \brief Header file for PWM output controll functions

<!-- DESCRIPTION: -->
       Header file for PWM output controll functions

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/pwm18x.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	    $Log: not supported by cvs2svn $
	    Revision 1.1  2008/11/14 19:27:38  blepojevic
	    No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	    Revision 1.2  2008-11-14 19:27:37  gfinlay
	    - Added file header comment blocks for several files that were missing these.
	    - Added debug uart support with serial.c and serial.h

	    Revision 1.1  2007-07-05 20:10:13  gfinlay
	    Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.
       
	    Revision 1.2  2006/02/08 19:39:08  nallyn
	    Review Code with updated file headers
	    
       \endif
*/
/******************************************************************************/

#ifndef PWM18_H
#define PWM18_H

/* ---------- D e f i n i t i o n s ------------------------ */

//PWM code for 16f87x
//enter the correct clock frequency - get from delay.h - MAKE SURE THEY ARE THE SAME
#define CLK_FREQ	40000	//x10^3
#define CCP1X	DC1B1
#define CCP1Y	DC1B0 
#define CCP2X	DC2B1
#define CCP2Y	DC2B0
#define TM2_PRE		1
#define TM2_ON	0x4

//following values are based on a 40khz pwm frequency - see "pwm values.xls"
#if(CLK_FREQ == 4000)
    #define PWM_FREQ    24
    #define SCALE   1
#elif(CLK_FREQ == 10000)
    #define PWM_FREQ    62
    #define SCALE   2.5
#elif(CLK_FREQ == 16000)
    #define PWM_FREQ    99
    #define SCALE   4
#elif(CLK_FREQ == 20000)
    #define PWM_FREQ    124
    #define SCALE   5
#elif(CLK_FREQ == 40000)
    #define PWM_FREQ    249
    #define SCALE   10
#else
    #error pwm18x.h - illdefined CLK_FREQ
#endif

/* ---------- F u n c t i o n   P r o t o t y p e s --=----- */

extern void pwm1_init(void);
extern void pwm1_on(void);
extern void pwm1(int duty);
extern void pwm1_off(void);

extern void pwm2_init(void);
extern void pwm2_on(void);
extern void pwm2(int duty);
extern void pwm2_off(void);

#endif   // PWM18_H
