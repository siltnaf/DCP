/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file iconblinker.c
       \brief Module responsible for control of icon blinking

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

	      $Header:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/iconblinker.c-arc   1.0.1.0   Jun 08 2018 17:20:26   ctctmas  $

	      $Log:   D:/PDB/PRJ/CE Truck Trailer/archives/APU/DieselAPU/DCP3/TD-DCP-02_02_01/iconblinker.c-arc  $
 * 
 *    Rev 1.1   Apr 24 2019 ctctmas
 * RTC_064664 APU minimum compressor off time 3.5 minutes
 *  Move this file to RTC Tip 
 * 
 *    Rev 1.0.1.0   Jun 08 2018 17:20:26   ctctmas
 * PPR032836
 * Experimental Modifications to Prevent Corruption Due to Buffer Overrun.
 * 
 *    Rev 1.0   Jun 08 2018 17:10:22   ctctmas
 * Initial Carrier revision.
 
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      This is former software version DCP3-019-02K imported to new folder APU\DieselAPU\DCP and will be tagged using new naming convention starting from TD-DCP-02.02.01
	
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.8  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.7  2007-11-30 18:52:07  msalimi
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

	      Revision 1.6  2007-10-09 15:03:18  msalimi
	      Version 35-PPP-002. Second round of testing..

	      Revision 1.5  2007-09-21 20:38:36  msalimi
	      Added compile option flags for timer 3 $ 4.

	      Revision 1.4  2007-09-17 18:41:35  msalimi
	      DPF related modifications

	      Revision 1.3  2007-08-27 22:19:08  msalimi
	      CAN communication working.

	      Revision 1.2  2007-08-01 20:05:16  nallyn
	      Added USE_COLDSTART to allow for a standard build with only two timers

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.15  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
	
	      Revision 1.14  2007/02/27 01:20:17  nallyn
	      Added 4 more timers for a total of 10 timers. The timers function differently
	      than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
	      timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
	      depending on the temperture selected:
	      18-20: High AC
	      21-23: Low AC
	      24: HVAC Off
	      25-27: Low Heat
	      28-30: High Heat
	
	      Revision 1.13  2007/02/26 17:49:52  nallyn
	      Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
	
	      Revision 1.12  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes

	      Revision 1.11  2006/02/08 20:17:28  nallyn
	      Changes based on code review
	      Array bounds checking
	
	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "iconblinker.h"
#include "main.h"               // for DIM macro
#include "fonts.h"
#include "lcd_grp1.h"
#include "timers.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#ifndef NULL
#define NULL    0
#endif

//! Descriptor for each icon in array list
typedef struct
{
    char      index;        //!< number of icon in suIcon array in TFXICON.H
    char      x;            //!< lcd x axis
    char      page;         //!< lcd page
    char      *timerPtr;    //!< blink timer pointer
} iconDesc;

/* ---------- I n t e r n a l   D a t a ---------- */

/*!
   Icon array list for descriptors for all icons which can be displayed.
   \showinitializer
*/
//JFH 180601 EXP_81_02_01
//static iconDesc iconList[] =
static iconDesc iconList[20] =
{   //index, x,  page,  timer pointer
    {   0,   47,    0,  
#ifndef EXTRA_TIMERS
                        &(timers.blinkBmTmr)
#else
                        NULL
#endif
                                                },  //!< battery
    {   1,    0,    2,  NULL                    },  //!< cool
    {   2,    0,    2,  NULL                    },  //!< heat
    {   3,  118,    0,  NULL                    },  //!< outlet on
    {   4,   31,    0,  &(timers.blinkTmr)      },  //!< APU on
    {   5,   31,    0,  NULL                    },  //!< APU with bar
    {   6,   60,    0,  &(timers.blinkMxTmr)    },  //!< max run
    {   7,  105,    0,  NULL                    },  //!< shore power
    {   8,  118,    0,  NULL                    },  //!< outlet trip
    {   9,  118,    0,  NULL                    },  //!< heat trip
    {   10, 118,    0,  NULL                    },  //!< outlet reset
    {   11, 118,    0,  NULL                    },  //!< outlet off
    {   12,  69,    0,  &(timers.blinkCSTmr)    },  //!< cold start
#ifndef USE_COLDSTART
    {   13,  86,    0,  &(timers.blinkT1Tmr)    },  //!< timer 1
    {   14,  91,    0,  &(timers.blinkT2Tmr)    },  //!< timer 2
#else
    {   13,  86,    0,  &(timers.blinkT1Tmr)    },  //!< timer 1
    {   14,  96,    0,  &(timers.blinkT2Tmr)    },  //!< timer 2
#endif // USE_COLDSTART
    {   15,  76,    0,  
#ifndef EXTRA_TIMERS
                        &(timers.blinkTMTmr)
#else
                        NULL                        
#endif // EXTRA_TIMERS
                                                },  //!< temp monitor
#ifndef USE_COLDSTART
    {   16,  96,    0,  &(timers.blinkT3Tmr)    },  //!< timer 3
    {   17, 101,    0,  &(timers.blinkT4Tmr)    },  //!< timer 4
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS_NOT_SHORT_OF_RAM
    {   18,  48,    0,  &(timers.blinkT5Tmr)    },  //!< timer 5
    {   19,  53,    0,  &(timers.blinkT6Tmr)    },  //!< timer 6
    {   20,  58,    0,  &(timers.blinkT7Tmr)    },  //!< timer 7
    {   21,  63,    0,  &(timers.blinkT8Tmr)    },  //!< timer 8
    {   22,  68,    0,  &(timers.blinkT9Tmr)    },  //!< timer 9
    {   23,  73,    0,  &(timers.blinkT10Tmr)   },  //!< timer 10
#endif // EXTRA_TIMERS
};

#define ICON_LIST_SIZE  ( sizeof(iconList) / sizeof(iconList[0]) )

char iconCtrl[ ICON_LIST_SIZE ];  // working states

// State variable for Icon visibility
static bit bAllowIcons;

/* ---------- G l o b a l   D a t a ---------- */

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: iconMakeVisible -->
       \brief Control visiblity of icons

<!-- PURPOSE: -->
    Give the view manager control of the visibilty of all icons.
    
<!-- PARAMETERS: -->
       @param[in]  allow     TRUE to allow icons, FALSE to no show icons
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void iconMakeVisible( char allow )
{
    unsigned char i;
    
    // If icons are not allowed
    if ( allow == FALSE )
    {
        // Erase all icons
        for ( i = 0; i < ICON_LIST_SIZE; i++ )
        {
            iconErase( i );
        }
        
        // Do not allow any icon drawing
        bAllowIcons = FALSE;
    }
    else
    {
        // Allow icon to be seen
        bAllowIcons = TRUE;
        
        // Go through icon list and show all active icons
        for ( i = 0; i < ICON_LIST_SIZE; i++ )
        {
            if ( iconCtrl[ i ] == 1 )
            {
                iconShow( i );
            }
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: iconShow -->
       \brief Show icon specified by icon ID

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  iconId  ID of icon to show
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void iconShow( char iconId )
{
    // Validate icon identifier
    if ( iconId < DIM( iconList ) )
    {
        // If icon drawing is allowed
        if ( bAllowIcons )
        {
            // Always need to show icon, they can be erased by other means
        lcd_putIcon( iconList[ iconId ].x,
                     iconList[ iconId ].page,
                     iconList[ iconId ].index );
        }
        iconCtrl[ iconId ] = 1;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: iconErase -->
       \brief Erase icon specified by icon id

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  iconId  ID of icon to erase
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void iconErase( char iconId )
{
    // Validate icon identifier
    if ( iconId < DIM( iconList ) )
    {
        // If icon drawing is allowed
        if ( bAllowIcons )
        {
        // Only need to erase icon if it is drawn
        if ( iconCtrl[ iconId ] )
        {
            lcd_eraseIcon( iconList[ iconId ].x,
                           iconList[ iconId ].page,
                           iconList[ iconId ].index );
            iconCtrl[ iconId ] = 0;
            }
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: iconControl -->
       \brief Control the blinking pattern of the icon with the specified ID

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  iconId  Number of icon to blink
       @param[in]  ctrl    Enumerated icon blinking pattern. Can be one of:
       \code
          ICON_OFF         Turn off 
          ICON_ON          Turn on solid (no blink)
          ICON_BLINK       Turn on normal length blink (50% duty)
          ICON_BLINK_ASYMETRIC_ON   Turn on blink (88% duty)
          ICON_BLINK_ASYMETRIC_OFF  Turn on blink (30% duty) 
          ICON_BLINK_SLOW  Turn on slow blink (50% duty)
          ICON_BLINK_FAST  Turn on fast blink (55% duty)
       \endcode
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void iconControl( char iconId, ICONCTRL ctrl )
{
    char *timerPtr;
    char onTime = 0;
    char offTime = 0;
    
    // Validate icon identifier
    if ( iconId < DIM( iconList ) )
    {
        timerPtr = iconList[ iconId ].timerPtr;
        
        switch ( ctrl )
        {
            case ICON_OFF:
                iconErase( iconId );
                return;
                
            case ICON_ON:
                iconShow( iconId );
                return;
                
            case ICON_BLINK:
                onTime = 50;
                offTime = 50;
                break;
            case ICON_BLINK_ASYMETRIC_ON:
                onTime = 132;
                offTime = 18;
                break;
            case ICON_BLINK_ASYMETRIC_OFF:
                onTime = 30;
                offTime = 70;
                break;
            case ICON_BLINK_FAST:
                onTime = 22;
                offTime = 18;
                break;
            case ICON_BLINK_SLOW:
                onTime = 100;
                offTime = 100;
                break;

            default:
                break;
        }
        
        // If there is a timer pointer and timer has reached zero
        if ( ( timerPtr ) && ( *timerPtr == 0 ) )
        {
            // If blink on
            if ( iconCtrl[ iconId ] )
            {
                // Erase the icon and set up the off time
                iconErase( iconId );
                *timerPtr = offTime;
            }
            else
            {
                // Show the icon and set up the on time
                iconShow( iconId );
                *timerPtr = onTime;
            }
        }
    }
}

