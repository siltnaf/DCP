/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file backlit.c
       \brief Module to control backlighting output. 

<!-- DESCRIPTION: -->
       The backlighting is turned on for 12 seconds as a result
       of any key pressed event.

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	   $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/backlit.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	   $Log: not supported by cvs2svn $
	   Revision 1.1  2009/02/27 19:16:32  blepojevic
	   No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	   Revision 1.6  2008-11-14 19:27:36  gfinlay
	   - Added file header comment blocks for several files that were missing these.
	   - Added debug uart support with serial.c and serial.h

	   Revision 1.5  2007-11-30 18:52:07  msalimi
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

	   Revision 1.4  2007-09-17 18:39:03  msalimi
	   DPF related modifications

	   Revision 1.3  2007-08-27 22:19:08  msalimi
	   CAN communication working.

	   Revision 1.2  2007-08-01 19:43:18  nallyn
	   Added commenting for Doxygen

	   Revision 1.1  2007-07-05 20:10:12  gfinlay
	   Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	   Revision 1.1  2006/02/08 19:06:38  nallyn
	   Modularized code by taking functions out of headers and placing them in .c files
	   Updated file headers to match proposed Teleflex coding standard
	
       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s -------------- */
#include "backlit.h"
#include "main.h"
#include "lcd_grp1.h"
#include "timers.h"
#include "ViewMan.h"            // Interface to using View Manager

/* ---------- I n t e r n a l   D e f i n i t i o n s ------ */
/* ---------- I n t e r n a l   D a t a -------------------- */
//!Possible states for the backlight 
enum bl_st {B_ON,B_OFF};
//!State variable for the backlight
enum bl_st BLIGHT_ST=B_OFF;
//enum bl_st BUZZER_ST=B_OFF;

#define BACKLIGHT_ON_PERIOD 20  // seconds

/* ---------- G l o b a l   D a t a ------------------------ */
/* ---------- F u n c t i o n s ---------------------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: back_light -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void back_light(void)
{
  switch(BLIGHT_ST)
  {
    case B_OFF:
      LCD_BKLT_1;
      if ( key_pressed )
      {
        lcd_init();
        VWM_fnRefresh();
        timers.bkLightTmr = BACKLIGHT_ON_PERIOD;
        BLIGHT_ST = B_ON;
      }
      break;
    case B_ON:
      LCD_BKLT_0;
      if ( key_pressed )
      {
        timers.bkLightTmr = BACKLIGHT_ON_PERIOD;
      }
      else if(!timers.bkLightTmr)
      {
        BLIGHT_ST = B_OFF;
      }
      break;
    default:
      BLIGHT_ST = B_OFF;
      break;
  }
}

