/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file fonts.c
       \brief Bitmap fonts for LCD display

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright � 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/fonts.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-09-17 18:41:35  msalimi
	      DPF related modifications

	      Revision 1.2  2007-08-01 20:01:31  nallyn
	      Added and modified commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.4  2006/02/08 20:13:39  nallyn
	      Changes based on code review
	      Mostly bounds checking for array traversing.  Do diff with previous version to see all differences
	
	      Revision 1.3  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
       \endif
*/
/******************************************************************************/


/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "fonts.h"
#include "main.h"               // for DIM macro
#include "lcd_grp1.h"
#include "TFXARR_2.H"
#include "TFXICON.H"
#include "TX16FULL.H"
#include "TFX8FULL.H"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

#define TOP_ROW         0
#define BOTTOM_ROW      7
#define DOUBLE_HEIGHT   2

#define ASCII_FIRST_PRINTABLE   ( 0x20 )
#define ASCII_FIRST_DIGIT       ( 0x30 )

#define GAUGE_TICK_START    	16
#define GAUGE_TICK_END          116
#define GAUGE_TICK_INCREMENT    10

#define GAUGE_INDICATOR_WIDTH	( 10 )
#define GAUGE_TICK_ROW          ( BOTTOM_ROW )
#define GAUGE_TEXT_ROW          ( GAUGE_TICK_ROW - 1 )

#define CHAR_C_XPOS             0
#define CHAR_H_XPOS             124

#define CHAR_C_INDEX            0x23
#define CHAR_H_INDEX            0x28
#define CHAR_0_INDEX            0x10

//!Structure to define where the cursor should be
static struct
{
    enum fontsizes size;       //!<current font size to use
    char x;                    //!<current x position on the LCD
    char page;                 //!<current y position on the LCD
} cursor;


/* ---------- I n t e r n a l   D a t a ---------- */
/* ---------- G l o b a l   D a t a ---------- */

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_position -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  x       x coordinate for cursor position
       @param[in]  page    page for cursor position
       @param[in]  size    Font size
<!-- RETURNS: -->
      \return Desc of return value

*/
/******************************************************************************/
void lcd_position(char x, char page, enum fontsizes size)
{
    cursor.x = x;
    cursor.page = page;
    cursor.size = size;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_putIcon -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  x        X coordinate
       @param[in]  page     page
       @param[in]  icon_num Number of icon to display
<!-- RETURNS: -->
      \return Desc of return value

*/
/******************************************************************************/
void lcd_putIcon(char x, char page, char icon_num)
{
   unsigned char i,n,k;
    
    // Check that icon_num fits into icon array
    if ( icon_num >= DIM( suIcon ) )
    	return;
        
    cursor.x = x;
    cursor.page = page;
    
    k=0;
    for( n = 0; n < DOUBLE_HEIGHT; n++ )// height of icon
    {
        lcd_goto( x, page + n );
        for(  i = 0; i < suIcon[ icon_num ].sH.cxPix; i++ )
        {
            lcd_data( suIcon[ icon_num ].b[ k ]);
            k++;
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_eraseIcon -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  x        X coordinate
       @param[in]  page     page
       @param[in] icon_num Number of icon to erase
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_eraseIcon(char x, char page, char icon_num)
{
    // Check that icon_num fits into icon array
    if ( icon_num >= DIM( suIcon ) )
        return;
        
    lcd_cla( x, x + suIcon[ icon_num ].sH.cxPix - 1,
            page, page + suIcon[ icon_num ].sH.cyPix/8 - 1 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_settemp -->
       \brief Draw indicator on temperature gauge

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  x       X coordinate
       @param[in]  page    page
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_settemp( char x, char page )
{
    char i,n,k ;
    
    k = 0;
    n = 0;
    
    // Arrange to center arrow at given x position
    x -= ( ( suArrow[ 0 ].sH.cxPix + 1 ) / 2 );
    
    for( n = 0; n < DOUBLE_HEIGHT; n++ )
    {
        lcd_goto( x , page + n );
        for( i = 0; i < suArrow[ 0 ].sH.cxPix; i++ )
        {									   
            lcd_data( suArrow[ 0 ].b[ k ] );
            k++;
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_puttemp -->
       \brief Update temperature gauge output

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_puttemp(void)
{
    char i,n,k ;
    k=0;
    
    //display C at left end
    for( n = 0; n < DOUBLE_HEIGHT; n++ )
    {
        lcd_goto( CHAR_C_XPOS, GAUGE_TEXT_ROW + n );
        for( i = 0; i < suFont16[ CHAR_C_INDEX ].sH.cxPix; i++ )
        {
            lcd_data( suFont16[ CHAR_C_INDEX ].b[ k ] );
            k++;
        }
    }
    k=0;
    //display H at right end
    for( n = 0; n < DOUBLE_HEIGHT; n++ )
    {
        lcd_goto( CHAR_H_XPOS, GAUGE_TEXT_ROW + n );
        for( i = 0; i < suFont16[ CHAR_H_INDEX ].sH.cxPix; i++ )
        {
            lcd_data( suFont16[ CHAR_H_INDEX ].b[ k ] );
            k++;
        }
    }
    
    // Place tick marks
    for( i = GAUGE_TICK_START; i <= GAUGE_TICK_END; i += GAUGE_TICK_INCREMENT )
    {
       lcd_goto( i, GAUGE_TICK_ROW );
       lcd_data( 0x0f );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_putch -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  input   Ascii character to draw
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void lcd_putch(char input)
{
    char i;
    char line; // used to keep track of the current line (two line font)
    char data=0; // Font data is consecutive so cannot be reset for second
                      // line of font.  This variable continues to increment
                      // throught the font data, in this case 0 to 15 (16 bits)
                      
    // Input is an ASCII character. Cannot show characters below 32
    if ( input < ASCII_FIRST_PRINTABLE )
        return;
    
    // Make input into array index    
    input -= ASCII_FIRST_PRINTABLE;
    
    switch( cursor.size )
    {
        case large: // For large font
       
            // Quit if index out of bound
            if ( input >= DIM( suFont16 ) )
               return;
               
            // Starting at the first line of the font   
            // and proceeding to the next
            for( line = 0; line < DOUBLE_HEIGHT; line++)
            {
                lcd_goto( cursor.x, cursor.page + line );
                for( i = 0; i < (int)(suFont16[input].sH.cxPix) ; i++ )
                {
                    lcd_data( suFont16[ input ].b[ data ] );
                    data++;
                }
            }
            
            // Set the position for the next character
            cursor.x += suFont16[input].sH.cxPix;
            break;
            
        case small:
          
            // Quit if index out of bound
            if ( input >= DIM( suFont8 ) )
                return;
               
            lcd_goto(cursor.x, cursor.page);
            for( i = 0; i < suFont8[input].sH.cxPix; i++ )
            {
                lcd_data(suFont8[ input ].b[ i ]);
            }
            
            // Set the position for the next character
            cursor.x += i;
            break;
    }
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: lcd_putnum -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  x        X coordinate
       @param[in]  page     page
       @param[in] num       Number to display
<!-- RETURNS: -->
      \return Desc of return value

*/
/******************************************************************************/
void lcd_putnum(char x, char page, char num)
{
    char n,i,k;

    // If num is a decimal digit
//    if( ( ( num >= 1 ) && ( num <= 9 ) ) || ( num == 0 ) )
    {
        k = 0;
        num += CHAR_0_INDEX;
        
        // Starting at the first line of the font   
        // and proceeding to the next
        for( n = 0; n < DOUBLE_HEIGHT; n++ )
        {
           lcd_goto( x, page + n );
           for( i = 0; i < suFont16[ num ].sH.cxPix; i++ )
           {
              lcd_data( suFont16[ num ].b[ k ] );
              k++;
           }
        }
    }
}

