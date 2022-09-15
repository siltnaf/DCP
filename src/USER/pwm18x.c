/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file pwm18x.c
       \brief PWM Output control routines for PIC 18F family

<!-- DESCRIPTION: -->
       PWM Output control routines for PIC 18F family

       \if ShowHist
       Copyright � 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/pwm18x.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

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

/* ---------- D e f i n i t i o n   F i l e s ---------- */
//?#include <pic18.h>
#include "pwm18x.h"
//#include "tim16_pwm_output.h"
//#include "tim17_pwm_output.h"
#include "tim1_pwm1_2.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

/* ---------- I n t e r n a l   D a t a ---------- */

/* ---------- G l o b a l   D a t a ---------- */

/* ----------------- F u n c t i o n s ----------------- */
void pwm1_on(void)
{
#ifndef MM32F0133C7P		//? Add
    CCP1CON &= 0x30;
    CCP1CON |= 0x0C;
#else		///?OK
		PWM1_On();
#endif
}

void pwm1_init(void)
{
#ifndef MM32F0133C7P		//? Add
    // set the PWM period
    // Set the PWM duty cycle
    // Make CCP1 pin an output
    // Set TMR2 prescale value and enable Timer2
    // Sec CCP1 for PWM operation

    CCP1CON = 0x0C;
    PR2 = PWM_FREQ;
    pwm1(0);
    TRISC2 = 0;
    T2CON = TM2_ON;
    pwm1_on();
#else		///?OK
		PWM1_Init();
#endif
}

void pwm1(int duty)
{
#ifndef MM32F0133C7P		//? Add
    char testchar;
    testchar = CCP1CON;
    testchar &= 0x0C;
    if(testchar != 0x0C)
    {
        CCP1CON |= 0x0C;
        PR2 = PWM_FREQ;
        //pwm(0);
        TRISC2 = 0;
        T2CON = TM2_ON;
        pwm1_on();
    }
    duty = duty*SCALE;
    CCPR1L = (duty >> 2); //0b01100100;
    CCP1X = ((duty >> 1) & 1);
    CCP1Y = (duty & 1);
#else		///?OK
		PWM1_Duty(duty);
#endif
}

void pwm1_off(void)
{
#ifndef MM32F0133C7P		//? Add
    CCP1CON &= 0b00110000;
#else		///?OK
		PWM1_Off();
#endif
}

void pwm2_on(void)
{
#ifndef MM32F0133C7P		//? Add
    CCP2CON &= 0b00110000;
    CCP2CON |= 0b00001100;
#else		///?OK
		PWM2_On();
#endif
}

void pwm2_init(void)
{
#ifndef MM32F0133C7P		//? Add
    CCP2CON = 0B00001100;
    PR2 = PWM_FREQ;
    pwm2(0);
    TRISC1 = 0;
    T2CON = TM2_ON;
    pwm2_on();
#else		///?OK
		PWM2_Init();
#endif
}

void pwm2(int duty) // 0 - 250 based on a frequency of 5khz
{
#ifndef MM32F0133C7P		//? Add
    char testchar;
    testchar = CCP2CON;
    testchar &= 0x0C;
    if(testchar != 0x0C)
    {
        CCP2CON |= 0x0C;
        PR2 = PWM_FREQ;
        //pwm(0);
        TRISC1 = 0;
        T2CON = TM2_ON;
        pwm2_on();
    }
    duty = duty*SCALE;
    CCPR2L = (duty >> 2); //0b01100100;
    CCP2X = ((duty >> 1) & 1);
    CCP2Y = (duty & 1);
#else		///?OK
		PWM2_Duty(duty);
#endif
}

void pwm2_off(void)
{
#ifndef MM32F0133C7P		//? Add
    CCP2CON &= 0b00110000;
#else		///?OK
		PWM2_Off();
#endif
}


///********************************************************************
//*    Function Name:  OpenPWM1                                       *
//*    Return Value:   void                                           *
//*    Parameters:     period: PWM period                             *
//*    Description:    This routine first resets the PWM registers    *
//*                    to the POR state.  It then configures clock    *
//*                    source.                                        *
//********************************************************************/
//void OpenPWM1( char period )
//{
//
//  CCP1CON=0b00001100;    //ccpxm3:ccpxm0 11xx=pwm mode
//
//#if defined(__18F6520) || defined(__18F6620) || defined(__18F6720) || 
//    defined(__18F8520) || defined(__18F8620) || defined(__18F8720) || 
//    defined(__18F6525) || defined(__18F6621) || 
//    defined(__18F8525) || defined(__18F8621) || 
//    defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || 
//    defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || 
//    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || 
//    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || 
//    defined(__18F6527) || defined(__18F6622) || 
//    defined(__18F6627) || defined(__18F6722) || 
//    defined(__18F8527) || defined(__18F8622) || 
//    defined(__18F8627) || defined(__18F8722)
//  if (T3CCP2 == 1 && T3CCP1 == 1)
//  {
//     TMR4ON = 0;  // STOP TIMER4 registers to POR state
//     PR4 = period;          // Set period
//     TRISC2=0;    //configure pin portc,2 an output
//     TMR4ON = 1;  // Turn on PWM1
//  }
//  else
//  {
//     TMR2ON = 0;  // STOP TIMER2 registers to POR state
//     PR2 = period;          // Set period
//     TRISC2=0;    //configure pin portc,2 an output
//     TMR2ON = 1;  // Turn on PWM1
//  }
//#else
//  TMR2ON = 0;  // STOP TIMER2 registers to POR state
//  PR2 = period;          // Set period
//#if defined(__18F1220) || defined(__18F1320)
//  TRISB3=0;    //configure pin portb,3 as output
//#else
//  TRISC2=0;    //configure pin portc,2 an output
//#endif
//  TMR2ON = 1;  // Turn on PWM1
//#endif
//}
//
///********************************************************************
//*    Function Name:  ClosePWM1                                      *
//*    Return Value:   void                                           *
//*    Parameters:     void                                           *
//*    Description:    This routine turns off PWM1.                   *
//********************************************************************/
//void ClosePWM1(void)
//{
//  CCP1CON=0;            // Turn off PWM1
//#if defined(__18F1220) || defined(__18F1320)
//  TRISB3=1;   //Return pin to its default input
//#else
//  TRISC2=1;   //Return pin to its default input status from output
//#endif
//}
//
///********************************************************************
//*    Function Name:  SetDCPWM1                                      *
//*    Return Value:   void                                           *
//*    Parameters:     dutycycle: holds 10-bit duty cycle value       *
//*    Description:    This routine writes the 10-bit value from      *
//*                    dutycycle into the PWM1 duty cycle registers   *
//*                    CCPR1L and CCP1CON.                            *
//********************************************************************/
//void SetDCPWM1(unsigned int dutycycle)
//{
//  union PWMDC DCycle;
//
//  // Save the dutycycle value in the union
//  DCycle.lpwm = dutycycle << 6;
//
//  // Write the high byte into CCPR1L
//  CCPR1L = DCycle.bpwm[1];
//
//  // Write the low byte into CCP1CON5:4
//  CCP1CON = (CCP1CON & 0xCF) | ((DCycle.bpwm[0] >> 2) & 0x30);
//}
//
//#define __CONFIG3H 0x300005
//char eccpmx; /* will be set iff bit 1 of __CONFIG3H is set */
//
///********************************************************************
//*    Function Name:  SetOutputPWM1                                  *
//*    Return Value:   void                                           *
//*    Parameters:     outputconfig: holds PWM output configuration.  *
//*                    These configurations are defined in pwm.h.     *
//*                    outputmode: holds PWM output mode.             *
//*                    These modes are defined in pwm.h.              *
//*    Description:    This routine sets the PWM output config bits   *
//*                    and mode bits for ECCP and configures the      *
//*                    appropriate TRIS bits for output.              *
//********************************************************************/
//void SetOutputPWM1(unsigned char outputconfig, unsigned char outputmode)
//{
//  /* set P1M1 and P1M0 */
//  CCP1CON = (CCP1CON | 0b11000000) & outputconfig;
//  /* set CCP1M3, CCP1M2, CCP1M1, CCP1M0 */
//  CCP1CON = (CCP1CON | 0b00001111) & outputmode;
//
//#if defined(__18F8525) || defined(__18F8621) || 
//    defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || 
//    defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || 
//    defined(__18F8527) || defined(__18F8622) || 
//    defined(__18F8627) || defined(__18F8722)
//  if (((*(unsigned char far rom *)__CONFIG3H) & 0b00000010))
//     eccpmx=0xff;
//  else
//     eccpmx = 0;
//#endif
//
//  if (SINGLE_OUT == outputconfig)
//  {
//#if defined(__18F1220) || defined(__18F1320)
//      TRISB3 = 0;
//#else
//      TRISC2 = 0;
//#endif
//  }
//  else if (IS_DUAL_PWM(outputconfig))
//  {
//#if defined(__18F1220) || defined(__18F1320)
//      TRISB3 = 0;
//      TRISB2 = 0;
//#elif defined(__18F6585) || defined(__18F6680) || 
//      defined(__18F8585) || defined(__18F8680) || 
//      defined(__18F6525) || defined(__18F6621) || 
//      defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || 
//      defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || 
//      defined(__18F6527) || defined(__18F6622) || 
//      defined(__18F6627) || defined(__18F6722)
//      TRISC2 = 0;
//      TRISE6 = 0;
//#elif defined(__18F8525) || defined(__18F8621) || 
//      defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || 
//      defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || 
//      defined(__18F8527) || defined(__18F8622) || 
//      defined(__18F8627) || defined(__18F8722)
//      if (eccpmx)
//      {
//         TRISC2 = 0;
//         TRISE6 = 0;
//      }
//      else
//      {
//         TRISC2 = 0;
//         TRISH7 = 0;
//      }
//#else
//  TRISC2 = 0;
//  TRISD5 = 0;
//#endif
//  }
//  else if (IS_QUAD_PWM(outputconfig))
//  {
//#if defined(__18F1220) || defined(__18F1320)
//      TRISB3 = 0;
//      TRISB2 = 0;
//      TRISB6 = 0;
//      TRISB7 = 0;
//#elif defined(__18F6585) || defined(__18F6680) || 
//      defined(__18F8585) || defined(__18F8680) || 
//      defined(__18F6525) || defined(__18F6621) || 
//      defined(__18F64J15) || defined(__18F65J10) || defined(__18F65J15) || 
//      defined(__18F66J10) || defined(__18F66J15) || defined(__18F67J10) || 
//      defined(__18F6527) || defined(__18F6622) || 
//      defined(__18F6627) || defined(__18F6722)
//      TRISC2 = 0;
//      TRISE6 = 0;
//      TRISE5 = 0;
//      TRISG4 = 0;
//#elif defined(__18F8525) || defined(__18F8621) || 
//      defined(__18F84J15) || defined(__18F85J10) || defined(__18F85J15) || 
//      defined(__18F86J10) || defined(__18F86J15) || defined(__18F87J10) || 
//      defined(__18F8527) || defined(__18F8622) || 
//      defined(__18F8627) || defined(__18F8722)
//      if (eccpmx)
//      {
//         TRISC2 = 0;
//         TRISE6 = 0;
//         TRISE5 = 0;
//         TRISG4 = 0;
//      }
//      else
//      {
//         TRISC2 = 0;
//         TRISG4 = 0;
//         TRISH7 = 0;
//         TRISH6 = 0;
//      }
//#else
//  TRISC2 = 0;
//  TRISD5 = 0;
//  TRISD6 = 0;
//  TRISD7 = 0;
//#endif
//  }
//}
//
