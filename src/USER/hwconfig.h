/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file hwconfig.h
       \brief Configure configuration bits for PIC18 series microcontrollers

<!-- DESCRIPTION: -->
       Configure configuration bits for PIC18 series microcontrollers

<!-- NOTES: -->
       \note Edit this file as necessary for each project. See the data sheet and
             pic18.h for the appropriate macro definitions

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/hwconfig.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.2  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.1  2007-07-05 20:10:12  gfinlay
       Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

   \endif
*/
/******************************************************************************/

#ifndef HWCONFIG_H_
#define HWCONFIG_H_

#ifndef _PIC18
#error This file is only for PIC18 devices
#endif

#include <pic18.h>

// External clock/oscillator with 4x PLL
__CONFIG( 1, HSHW       // High speed clock/osc w/ 4x hardware PLL 
            );
            
#ifndef MPLAB_ICD   // If not compiling for debug
__CONFIG( 2, BORV20     // Brown out 2.5V 
            & PWRTEN    // Power up timer enabled
            & WDTPS256  // Watchdog timer enabled, 256 prescaler
            );

__CONFIG( 4, DEBUGDIS   // Debug disabled
            & LVPDIS    // Low voltage programming disabled
            & STVREN    // Stack over/underflow will cause reset
            );
            
#else   //MPLAB_ICD // compiling for debug
__CONFIG( 2, BORV20     // Brown out 2.0V 
            & PWRTEN    // Power up timer enabled
            & WDTDIS    // Watchdog timer disabled
            );

__CONFIG( 4, DEBUGEN    // Debug enabled
            & LVPDIS    // Low voltage programming disabled
            & STVREN    // Stack over/underflow will cause reset
            );
#endif  //MPLAB_ICD

__CONFIG( 3, MCLREN     // Master Clear (MCLR) enabled 
            & CCP2RC1   // CCP2 Multiplexed to RC1 
            );


__CONFIG( 5, UNPROTECT  // Do not protect memory
            );

__CONFIG( 6, WRTEN      // Program memory write enabled 
            );

//__CONFIG( 7, );       // Commented out to enable table read

#endif //HWCONFIG_H_
