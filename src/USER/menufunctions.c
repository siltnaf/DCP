/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file menufunctions.c
       \brief Module containing drawing support routines for LCD menus

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

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/menufunctions.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2009/07/31 17:19:42  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.11  2006/02/08 20:22:07  nallyn
	      Eliminated duplicate code
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "main.h"
#include "menu.h"
#include "menufunctions.h"
#include <stdio.h>
#include "fonts.h"
#include "lcd_grp1.h"
#include "timers.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
/* ---------- I n t e r n a l   D a t a ---------- */
/* ---------- G l o b a l   D a t a ---------- */
/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fncShowArrows -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  left_x  X position of left arrow
       @param[in]  right_x X position of right arrow 
       @param[in]  page    Y position of arrows
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fncShowArrows(char left_x, char right_x, char page)
{
    lcd_position(left_x, page, large);
    printf("<");
    lcd_position(right_x, page, large);
    printf(">");
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: blink_digit -->
       \brief Display a blinking digit at a specified location

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  x       X position for digit
       @param[in]  page    Y position for digit
       @param[in]  number  value of digit to display
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void blink_digit(char x, char page, char number)
{
    if ( mugBlinkOn )
    {
        lcd_putnum(x, page, number);
    }
    else
    {
        lcd_position(x, page, large);
        printf(" ");
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: change_digit -->
       \brief Poll the left or right buttons to determine whether to increment 
       or decrement the value of a digit with a min to max range.

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  num     Current value of digit
       @param[in]  inc     Size or magnitude of increment/decrement to add to digit
       @param[in]  max     Maximum value of digit
       @param[in]  min     Minimum value of digit
<!-- RETURNS: -->
      \return Updated value of digit based on button press (if any)

*/
/******************************************************************************/
char change_digit(char num, char inc, char max, char min)
{
    if (RIGHT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        num += inc;
        if (num > max)
        {
            num = min;
        }
    }
    else if (LEFT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        if (num <= min)
        {
            num = max;
        }
        else
        {
            num -= inc;
        }
    }
    return(num);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: change_number -->
       \brief Poll the left or right buttons to determine whether to increment 
       or decrement the value of an unsigned number with a min to max range.

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  num     Current value of number
       @param[in]  inc     Size or magnitude of increment/decrement to add to number
       @param[in]  max     Maximum value of number
       @param[in]  min     Minimum value of number
<!-- RETURNS: -->
      \return Updated value of number based on button press (if any)

*/
/******************************************************************************/
char change_number(char num, char inc, char max, char min)
{
    if (RIGHT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        num += inc;
    }
    else if (LEFT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        if ( num > inc )
            num -= inc;
        else
            num = 0;
    }
    
    if (num >= max)
    {
        num = max;
    }
    else if (num <= min)
    {
        num = min;
    }
    
    return(num);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fncChangeSignedNumber -->
       \brief Poll the left or right buttons to determine whether to increment 
       or decrement the value of a signed number with a min to max range.

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  num     Current value of number
       @param[in]  inc     Size or magnitude of increment/decrement to add to number
       @param[in]  max     Maximum value of number (could be negative)
       @param[in]  min     Minimum value of number (could be negative)
<!-- RETURNS: -->
      \return Updated value of signed number based on button press (if any)

*/
/******************************************************************************/
signed char fncChangeSignedNumber( signed char num, signed char inc, signed char max, signed char min )
{
    if (RIGHT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        num += inc;
        if (num >= max)
        {
            num = max;
        }
    }
    else if (LEFT_BTN)
    {
        timers.menuTmr = MENU_PAUSE; //menuSTimer(MENU_PAUSE);
        num -= inc;
        if (num <= min)
        {
            num = min;
        }
    }
    return(num);
}

