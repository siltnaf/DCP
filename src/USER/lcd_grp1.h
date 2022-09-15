/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file lcd_grp1.h
       \brief Header for TM13264 LCD graphics display driver module

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/lcd_grp1.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-11-30 18:52:08  msalimi
	      - Made improvements suggested in code review.
	      - Menu timeout period extended to two minutes.
	      - Backlight on period extended to one minute.
	      - Moved CAN main loop operation to a new file (jmain).
	      - Added compile option switch to include or exclude internal versus extenral memories.
	      - Merged user regen and emergency regen menu items.
	      - Emergency regen asks the user to turn on the APU.
	      - Emergency flas is now set at menu and cleared when APU shuts down.
	      - Added a write enable of RTC chip each time the Time & Date menu is entered. Previously this was done only once at startup which was causing the RTC to be written only once (with the new chip). So the time could be updated only once after each reset.
	      - Display updates now will be done regularly to display all active messages based on priority.
	      - Changed the routine that performs a write on I2C to return true or false so that the calling routine can repeate the write requests.

	      Revision 1.2  2007-08-01 20:08:55  nallyn
	      Removed unused code. Added and modified comments for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

// function prototypes and constants and port definitions
#ifndef LCD_GRP_H
#define LCD_GRP_H

#include	"hal_conf.h"			//? Add

//?#include <pic18.h>
#define	PORTBIT(adr, bit)	((unsigned)(&adr)*8+(bit))
#ifndef MM32F0133C7P		//? Add
#define LCD_DATA	PORTD
#else
#define LCD_DATA(x)		GPIO_Write(GPIOC, ((GPIO_ReadOutputData(GPIOC)&0xFF00) | x))
#define LCD_CS1_0			GPIO_ResetBits(GPIOC, GPIO_Pin_10)
#define LCD_CS1_1			GPIO_SetBits(GPIOC, GPIO_Pin_10)
#define LCD_RESET_0		GPIO_ResetBits(GPIOC, GPIO_Pin_11)
#define LCD_RESET_1		GPIO_SetBits(GPIOC, GPIO_Pin_11)
#define LCD_RS_0			GPIO_ResetBits(GPIOC, GPIO_Pin_12)
#define LCD_RS_1			GPIO_SetBits(GPIOC, GPIO_Pin_12)
#define LCD_WR_0			GPIO_ResetBits(GPIOC, GPIO_Pin_8)
#define LCD_WR_1			GPIO_SetBits(GPIOC, GPIO_Pin_8)
#define LCD_RD_0			GPIO_ResetBits(GPIOC, GPIO_Pin_9)
#define LCD_RD_1			GPIO_SetBits(GPIOC, GPIO_Pin_9)
#endif

#define	DELAY	22    //!< Used with DelayUs
#define DELYS	4     //!< Short delay, used with DelayUs 

// Initialization Constants

#define ADC_SELECT 	0b10100001
#define SHL_SELECT 	0b11000000
#define DISP_ON		0b10101111
#define DISP_OFF	0b10101110
#define LCD_BIAS	0b10100011		//!< Duty 1/65 bias 1/9
#define REG_RES		0b00100110
#define SV_SEL		0b10000001
#define SV_REG		0b00010011

#ifndef MM32F0133C7P		//? Add
extern bit LCD_BKLT;     // Backlight
#else
#define LCD_BKLT_0		GPIO_ResetBits(GPIOC, GPIO_Pin_13)
#define LCD_BKLT_1		GPIO_SetBits(GPIOC, GPIO_Pin_13)
#endif

/* ---------- F u n c t i o n  P r o t o t y p e s ------------------ */
extern void port_init(void);
extern void lcd_data(unsigned char c);
extern void lcd_cmd(unsigned char c);
extern void lcd_goto(unsigned char x, unsigned char page);
extern void lcd_cls(void);
extern void lcd_cla(unsigned char xstart,unsigned char xend,unsigned char pstart,unsigned char pend);
extern void lcd_init(void);

#endif 	// LCD_GRP_H
