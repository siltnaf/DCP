/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file lcd_grp1.c
       \brief TM13264 LCD graphics display driver module

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/lcd_grp1.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2010/03/11 19:33:14  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-11-30 18:52:07  msalimi
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

	      Revision 1.2  2007-08-01 20:08:06  nallyn
	      Removed unused lines. Added and modified commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.3  2006/02/08 20:18:25  nallyn
	      Removed unused code
	
	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include	"lcd_grp1.h"
#include	"delay18.h"
#include	"delay.h"					//? Add
#include	"GPIO.h"					//? Add

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#ifndef MM32F0133C7P		//? Add
static bit LCD_CS1		@ PORTBIT(PORTE, 4);	//!< Chip Select
static bit LCD_RESET	@ PORTBIT(PORTE, 3);	//!< Reset - Active low
static bit LCD_RS		@ PORTBIT(PORTE, 2);	//!< Register select
static bit LCD_WR		@ PORTBIT(PORTE, 1);	//!< Write
static bit LCD_RD		@ PORTBIT(PORTE, 0);	//!< Read
#endif

#define INVERTED_OUTPUTS    0   //!<0 for non-inverted, 1 for inverted
#define NEW_LCD             1    //1 for DCP3, 0 for DCP2

/* ---------- G l o b a l   D a t a ---------- */
#ifndef MM32F0133C7P		//? Add
bit LCD_BKLT     	@ PORTBIT(PORTC, 0);    // Backlight
#endif
/* ----------------- F u n c t i o n s ----------------- */


/******************************************************************************/
/*!

<!-- FUNCTION NAME: port_init -->
       \brief Initializes the PIC port connected to the dot matrix LCD

<!-- PURPOSE: -->
       \brief Initializes the PIC micro's SFR registers for the port connected to the 
       matrix LCD column outputs and row inputs
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void port_init(void)
{
#ifndef MM32F0133C7P		//? Add
//	ADCON0 = 0x00;
//	ADCON1 = 0x04;
	//OPTION = 0x0F;
	//SWDTEN = 1;
	//CONFIG2H = 0x0F;
	TRISB = 0xC7; //0b11000111;//TRISB = 0x00;
	TRISA = 0x0F; //&= 0b11000001;
	TRISC = 0x80; //&= 0b00000111;
	TRISD = 0x00;
	TRISE = 0x00;
    TRISF = 0x00;
	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	PORTE = 0x00;
    PORTF = 0x00;
#else		///?OK
	PORT_Init();
#endif
}

//inverted outputs
#if INVERTED_OUTPUTS

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_data -->
       \brief Sends 1 byte of parameter data to the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  c       Parameter data byte to send to the LCD controller
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_data(unsigned char c)
{
    LCD_DATA = ~c;
	LCD_RS = 0;
	LCD_CS1 = 1;
	LCD_WR = 1;
	DelayUs(DELAY);
	LCD_WR = 0;
	LCD_CS1 = 0;
	LCD_RS = 1;
	DelayUs(DELYS);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_cmd -->
       \brief Sends a one byte command opcode to the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  c       Command opcode to send to the LCD controller
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_cmd(unsigned char c)
{
	LCD_DATA = ~c;
	LCD_RS = 1;
	LCD_CS1 = 1;
	LCD_WR = 1;
	DelayUs(DELAY);
	LCD_WR = 0;
	LCD_CS1 = 0;
	LCD_RS = 0;
	DelayUs(DELYS);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_init -->
       \brief Function to initialize the status of the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_init(void)
{
	LCD_CS1 = 0;
	DelayUs(DELAY);
	LCD_WR = 0;
	LCD_RD = 0;
	LCD_RESET = 0;		//Reset Display
	DelayUs(50);		//Wait for Power to stabilize
	LCD_RESET = 1;		//
	DelayUs(50);
	LCD_RESET = 0;
	DelayUs(50);

#if NEW_LCD
	lcd_cmd(0b10100010);	// Bias = 1/9
	lcd_cmd(0b10100001);	// ADC Select reverse
	lcd_cmd(0b11000000);	// Com Scan Direction
	lcd_cmd(0b00101111);	// Power Control, On, On, On
	DelayUs(100);
	lcd_cmd(0b00100110);	// Regulator Resistor Select
	DelayUs(100);
	lcd_cmd(0b10000001);	// Electronic Volume Control Mode
	lcd_cmd(0b00001110);	// Electronic Volume setting
#else
	lcd_cmd(0b10100001);	//ADC Select
	lcd_cmd(0b10100011);	//Bias = 1/9
	lcd_cmd(0b11001000);	//Com Scan Direction
	lcd_cmd(0b00101111);	//Power Control
	DelayUs(100);
	lcd_cmd(0b00100110);	//Regulator Resistor Select
	DelayUs(100);
	lcd_cmd(0b10000001);	//Ref Voltage Mode
	DelayUs(100);
	lcd_cmd(0b11011001);	//Ref Voltage Register
#endif

	lcd_cmd(0b10101111);	// Display On
	DelayUs(250);
	lcd_cmd(0b10100100);	// Entire Display Off
	lcd_cmd(0b10100110);	// Reverse Display Off
	lcd_cls();
}
#else

//non-inverted outputs

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_data -->
       \brief Sends 1 byte of parameter data to the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  c       Parameter data byte to send to the LCD controller
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_data(unsigned char c)
{
	u16 temp;
#ifndef MM32F0133C7P		//? Add
    LCD_DATA = c;
	LCD_RS = 1;
	LCD_CS1 = 0;
	LCD_WR = 0;
	DelayUs(DELAY);
	LCD_WR = 1;
	LCD_CS1 = 1;
	LCD_RS = 0;
	DelayUs(DELYS);
#else
    LCD_DATA(c);
	//temp = (GPIO_ReadOutputData(GPIOC)&0xFF00) | c;
	//UART_printf("DATA = %04X\r\n", temp);
	//GPIO_Write(GPIOC, temp);
	LCD_RS_1;
	LCD_CS1_0;
	LCD_WR_0;
	DelayUs(DELAY);
	LCD_WR_1;
	LCD_CS1_1;
	LCD_RS_0;
	DelayUs(DELYS);
#endif
}

//non-inverted outputs

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_cmd -->
       \brief Sends a one byte command opcode to the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  c       Command opcode to send to the LCD controller
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_cmd(unsigned char c)
{
	u16 temp;
#ifndef MM32F0133C7P		//? Add
	LCD_DATA = c;
	LCD_RS = 0;
	LCD_CS1 = 0;
	LCD_WR = 0;
	DelayUs(DELAY);
	LCD_WR = 1;
	LCD_CS1 = 1;
	LCD_RS = 1;
	DelayUs(DELYS);
#else
		LCD_DATA(c);
	//temp = (GPIO_ReadOutputData(GPIOC)&0xFF00) | c;
	//UART_printf("CMD = %04X\r\n", temp);
	//GPIO_Write(GPIOC, temp);
	LCD_RS_0;
	LCD_CS1_0;
	LCD_WR_0;
	DelayUs(DELAY);
	LCD_WR_1;
	LCD_CS1_1;
	LCD_RS_1;
	DelayUs(DELYS);
#endif
}


//non-inverted outputs
/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_init -->
       \brief Function to initialize the status of the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_init(void)
{
#ifndef MM32F0133C7P		//? Add
	LCD_CS1 = 0;
	DelayUs(DELAY);
	LCD_WR = 1;
	LCD_RD = 1;
	LCD_RESET = 1;		//Reset Display
	DelayUs(50);		//Wait for Power to stabilize
	LCD_RESET = 0;		//
	DelayUs(50);
	LCD_RESET = 1;
	DelayUs(50);
#else
	LCD_CS1_0;
	DelayUs(DELAY);
	LCD_WR_1;
	LCD_RD_1;
	LCD_RESET_1;		//Reset Display
	DelayUs(50);		//Wait for Power to stabilize
	LCD_RESET_0;		//
	DelayUs(50);
	LCD_RESET_1;
	DelayUs(50);
#endif

#if NEW_LCD
#ifndef MM32F0133C7P		//? Add
	lcd_cmd(0b10100010);	// Bias = 1/9
	lcd_cmd(0b10100001);	// ADC Select reverse
	lcd_cmd(0b11000000);	// Com Scan Direction
	lcd_cmd(0b00101111);	// Power Control, On, On, On
	DelayUs(100);
	lcd_cmd(0b00100110);	// Regulator Resistor Select
	DelayUs(100);
	lcd_cmd(0b10000001);	// Electronic Volume Control Mode
	lcd_cmd(0b00001100);	// Electronic Volume setting
#else
	lcd_cmd(0xA2);				// Bias = 1/9
	lcd_cmd(0xA1);				// ADC Select reverse
	lcd_cmd(0xC0);				// Com Scan Direction
	lcd_cmd(0x2F);				// Power Control, On, On, On
	DelayUs(100);
	lcd_cmd(0x26);				// Regulator Resistor Select
	DelayUs(100);
	lcd_cmd(0x81);				// Electronic Volume Control Mode
	lcd_cmd(0x0C);				// Electronic Volume setting
#endif
#else
	lcd_cmd(0b10100001);	//ADC Select
	lcd_cmd(0b10100011);	//Bias = 1/9
	lcd_cmd(0b11001000);	//Com Scan Direction
	lcd_cmd(0b00101111);	//Power Control
	DelayUs(100);
	lcd_cmd(0b00100110);	//Regulator Resistor Select
	DelayUs(100);
	lcd_cmd(0b10000001);	//Ref Voltage Mode
	DelayUs(100);
	lcd_cmd(0b11011001);	//Ref Voltage Register
#endif
#ifndef MM32F0133C7P		//? Add
	lcd_cmd(0b10101111);	// Display On
	DelayUs(250);
	lcd_cmd(0b10100100);	// Entire Display Off
	lcd_cmd(0b10100110);	// Reverse Display Off
	lcd_cls();
#else
	lcd_cmd(0xAF);				// Display On
	DelayUs(250);
	lcd_cmd(0xA4);				// Entire Display Off
	lcd_cmd(0xA6);				// Reverse Display Off
#endif
}
#endif

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_goto -->
       \brief Sends a goto xy command and parameters to the LCD controller

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  x       X coordinate
       @param[in]  page    page (Y) coordinate
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_goto(unsigned char x, unsigned char page)
{
    lcd_cmd(0xB0 | (0x0F & page));
    lcd_cmd(0x0f & x);
    x = (x>>4);
    lcd_cmd(0x10|(0x0f & x));
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_cls -->
       \brief Clears the LCD screen

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_cls(void)
{
    char i,j;
    for(i=0;i<8;i++)
    {
    	lcd_goto(0,i);
    	for(j=0;j<132;j++)
    	{
    		lcd_data(0x00);
    	}
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_cla -->
       \brief Clears the specified area or rectable within the LCD screen

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  xstart  Starting X coordinate of the rectangle area to clear
       @param[in]  xend    Ending x coordinate of the rectangle area to clear
       @param[in]  pstart  Starting page Y coordinate of the rectangle area to clear
       @param[in]  pend    Ending page Y coordinate of the rectangle area to clear
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_cla(unsigned char xstart,unsigned char xend,unsigned char pstart,unsigned char pend)
{
    char i,j;
    for(i=pstart;i<=pend;i++)
    {
    	lcd_goto(xstart,i);
    	for(j=xstart;j<=xend;j++)
    	{
    		lcd_data(0x00);
    	}
    }
}
