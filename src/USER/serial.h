/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file serial.h
       \brief Header file for UART serial functions

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

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/serial.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:38  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.1  2008-11-14 19:27:37  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h


       \endif
*/
/******************************************************************************/
#ifndef SERIAL_H
#define SERIAL_H

#define BAUD 115200      
#define FOSC 40000000L
#define NINE 0     /* Use 9bit communication? FALSE=8bit */
#define OUTPUT 0
#define INPUT 1

#define DIVIDER ((int)(FOSC/(16UL * BAUD) -1))
#define HIGH_SPEED 1

#if NINE == 1
#define NINE_BITS 0x40
#else
#define NINE_BITS 0
#endif

#if HIGH_SPEED == 1
#define SPEED 0x4
#else
#define SPEED 0
#endif

#define PORT_LCD		(0)
#define PORT_UART		(1)

#ifdef DEBUG_UART_PUTCH

// Set to SERIAL_bPortSelect to PORT_LCD or PORT_UART
// to control output selection for putch
extern unsigned char SERIAL_bPortSelect;

void UART_init_comms(void);
void UART_putch(unsigned char);
void putch(unsigned char);
unsigned char UART_getch(void);

#else

void putch(unsigned char);

#endif // DEBUG_UART_PUTCH


#endif	// SERIAL_H
