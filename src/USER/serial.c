/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file serial.c
       \brief UART serial functions

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

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/serial.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:38  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.1  2008-11-14 19:27:37  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h


       \endif
*/
/******************************************************************************/

#include "main.h"               // Common definitions
#include "tfxstddef.h"
#include <stdio.h>
#include <stdarg.h>
#include "serial.h"
#include "fonts.h"

#ifdef DEBUG_UART_PUTCH
unsigned char SERIAL_bPortSelect = PORT_LCD;

/* Serial initialization */
void UART_init_comms(void)
{
	SPBRG = DIVIDER;
	TXSTA = (SPEED|NINE_BITS|0x20);
	RCSTA = (NINE_BITS|0x90);
	TRISC6=OUTPUT;
	TRISC7=INPUT;
}

void UART_putch(unsigned char c) 
{
	/* output one byte */
	while(!TRMT)	/* set whilst TX in progress */
		continue;
	TXREG = c;
}

void putch(unsigned char c)
{
	if (SERIAL_bPortSelect == PORT_UART)
	{
		UART_putch(c);
	}
	else
	{
		lcd_putch(c);
	}
}

unsigned char UART_getch() 
{
	/* retrieve one byte */
	while(!RCIF)	/* set when register is not empty */
		continue;
	return RCREG;	
}

#else
void putch(unsigned char c)
{
		lcd_putch(c);
}

#endif

