/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file fonts.h
       \brief Header file for bitmap fonts used by LCD display

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/fonts.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.3  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.2  2007-08-01 20:01:58  nallyn
	      Added commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.3  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

#ifndef FONTS_H
#define FONTS_H

/* ---------- D e f i n i t i o n s ------------------------ */


#define BYTE unsigned char
#define MAXBYTE8    6
#define MAXBYTE14   16
#define MAXBYTE16   16
#define MAXBYTE20  20
#define MAXBYTE32   32
#define MAXBYTEARR 18
#define MAXBYTELOAD   60
#define MAXBYTEFAN  32

//!Possible font sizes
enum fontsizes
{
    small,
    large
};

//!Structure for the characters in the font sets
typedef struct
{
   char ccChar; //!< The value for this character
   int cxPix;   //!< The size of the character in the x direction
   int cyPix;   //!< The size of the character in the y direction
} sHeader;

//!Structure for 32 byte sized fonts - used for large icons
typedef struct
{
    sHeader sH;         //!<The character definition
    BYTE b[MAXBYTE32];  //!<The character data
} sUnitIcon;

//!Structure for 20 byte sized fonts - used for small icons
typedef struct
{
    sHeader sH;         //!<The character definition
    BYTE b[MAXBYTE20];  //!<The character data
} sUnitIcon8;

//!Structure for the arrow icon used for the temperature bar
typedef struct
{
    sHeader sH;         //!<The character definition
    BYTE b[MAXBYTEARR]; //!<The character data
} sUnitArrow;

//!Structure for the 8 byte sized fonts
typedef struct
{
    sHeader sH;         //!<The character definition
    BYTE b[MAXBYTE8];   //!<The character data
} sUnit8;

//!Structure for the 16 byte sized fonts
typedef struct
{
    sHeader sH;         //!<The character definition
    BYTE b[MAXBYTE16];  //!<The character data
} sUnit16;

/* ---------- F u n c t i o n   P r o t o t y p e s ----------------- */

extern void lcd_position(char x, char page, enum fontsizes size);
extern void lcd_putIcon(char x, char page, char icon_num);
extern void lcd_eraseIcon(char x, char page, char icon_num);
extern void lcd_putIcon8(char x, char page, char icon_num);
extern void lcd_putch(char input);
extern void lcd_settemp(char x,char page);
extern void lcd_puttemp(void);
extern void lcd_puts(char x, char page,char * string,char size);
extern void lcd_putnum(char x, char page, char num);


#endif // FONTS_H
