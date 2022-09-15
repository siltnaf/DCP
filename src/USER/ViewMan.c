/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file ViewMan.c
       \brief View Manager to control view messages

<!-- DESCRIPTION: -->
    This module selects what messages are in view.
    
<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/ViewMan.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

          $Log: not supported by cvs2svn $
          Revision 1.5  2014/07/25 15:24:15  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.4  2014/04/16 14:07:07  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.3  2013/09/19 13:07:42  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.2  2010/12/01 22:44:24  dharper
          Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
          This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
          This is because REV B was never checked in.
    
          Revision 1.1  2010/01/20 20:24:06  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
          Revision 1.34  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.33  2008-10-29 17:08:21  msalimi
          Adjusted the position of some messages.

          Revision 1.32  2008-10-28 15:54:28  msalimi
          - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
          - Removed the display of APU shutdown reason after ESC/RESET key push.
          - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

          Revision 1.31  2008-10-20 18:08:52  msalimi
          Improvements in displayed messages.

          Revision 1.30  2008-10-09 18:02:28  msalimi
          Changed and cleaned messages.

          Revision 1.29  2008-09-17 17:35:35  msalimi
          *** empty log message ***

          Revision 1.28  2008-09-17 16:08:18  msalimi
          - Changed the text displayed when the exhaust pressure sensor or the snubber fail.

          Revision 1.27  2008-09-04 21:12:00  msalimi
          Improved the display format of some messages.

          Revision 1.26  2008-08-21 17:05:46  msalimi
          *** empty log message ***

          Revision 1.25  2008-08-21 16:50:42  msalimi
          *** empty log message ***

          Revision 1.24  2008-08-21 15:43:11  msalimi
          - Added feature to turn on the backlight when fault or important messages are being displayed.
          - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
          - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
          - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

          Revision 1.23  2008-08-12 20:40:07  msalimi
          Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

          Revision 1.22  2008-08-07 20:06:27  msalimi
          - Adjusted some displayed text on screen

          Revision 1.21  2008-07-14 16:52:42  msalimi
          Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

          Revision 1.20  2008-05-21 17:37:58  msalimi
          - Renamed some of the DPF timers and their associated events to better reflect their usage.
          - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

          Revision 1.19  2008-05-13 19:13:35  msalimi
          - Added new feature to treat transitions from regenning to any other state the same as transitioning to regen ok state.
          - Removed warning message related to compliing for COMFORTPRO as this is always the case.

          Revision 1.18  2008-04-24 15:41:15  msalimi
          - Made display of regen status at home screen conditional depending on APU being on.

          Revision 1.17  2008-04-10 16:45:17  msalimi
          - Made a distinction between normal and emergency regen.
          - Extreme pressure and pressure sensor faults will be detected during normal regen.
          - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
          - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
          - A new command will be sent to DPF when an emergency regen is initiated.
          - APU warmup reduced to 2 minutes (from 5).

          Revision 1.16  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.15  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.14  2008-01-09 21:45:44  msalimi
          - Increased the low voltage detection delay at APU start up to 20 second. Also increased the number of consecutive low generator voltage indicator messages from 3 to 5.
          - Added DPF regeneration countdown message to the bottom of the home screen.

          Revision 1.13  2007-12-21 20:36:43  msalimi
          - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
          - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
          - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
          - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

          Revision 1.12  2007-12-06 23:21:32  msalimi
          Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

          Revision 1.11  2007-12-06 21:40:09  msalimi
          - Changed the message that delays the APU shutdown to also show the coolddown period.
          - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
          - Eliminated the "Heater Status" from DPF menu.

          Revision 1.10  2007-11-30 18:52:07  msalimi
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

          Revision 1.9  2007-11-01 19:41:33  msalimi
          -Changed the frequency of recording the CCU Status to external memory to every five minutes.
          -Fixed the bug that was causing the DM1 messages to be cleared after the clearing time was up.
          -Minor fixes in message display location on screen.

          Revision 1.8  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.7  2007-10-24 22:24:34  msalimi
          Fixed spelling error.

          Revision 1.6  2007-10-23 19:03:19  msalimi
          Added display of low voltage engine shutdown.

          Revision 1.5  2007-10-15 16:30:15  msalimi
          Added memory error display.

          Revision 1.4  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.3  2007-09-21 20:31:42  msalimi
          No change

          Revision 1.2  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.1  2007-08-27 22:26:33  msalimi
          *** empty log message ***


       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "ViewMan.h"            // Interface to using View Manager
#include "main.h"               // Common definitions
#include <stdio.h>              // for printf
#include "fonts.h"              // Interface to LCD fonts
#include "lcd_grp1.h"           // Interface to LCD

#include "battmon.h"            // Interface to Battery Voltage Monitor
#include "iconblinker.h"        // Interface to icon drawing
#include "ice_pnl.h"            // Interface to heating and cooling control
#include "menu.h"               // Menu definitions
#include "rtclock.h"            // Interface to real-time clock
#include "tempmon.h"            // Interface to Temperature Monitor
#include "timers.h"             // Interface to software counter timers
#include "DPF.h"
#include "apu.h"

/* ---------- Ex t e r n a l   D a t a ---------- */
extern unsigned char DPF_State;
extern APU_DIAG APU_Diag;

/* ---------- I n t e r n a l   D e f i n i t i o n s ---------- */

static uint8 vwm_fnBestView( void );
static void vwm_fnChangeView( VWM_eVIEW_ID eViewId );
static void vwm_fnClearWorkArea( void );
static void vwm_fnShowClock( bool bRedraw );
static void vwm_fnShowVersion( void );

/* ---------- I n t e r n a l   D a t a ---------- */

// List of all view requests, all initialized to FALSE;
static bool vwm_bViewRequests[ VWM_MAX_VIEWS ];

// Active View
static uint8 vwm_u8ActiveView;


/* ---------- F u n c t i o n s ---------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnClearAlert -->
       \brief Clear the top most Alert from view

<!-- PURPOSE: -->
    This function is used to remove the top most alert view from the
    active view list allowing other views to take over the screen.
    The definition of an alert view is a view what is in this list.
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void VWM_fnClearAlert( void )
{
    switch ( vwm_u8ActiveView )
    {
    case VWM_eVIEW_DPF_HOLD_ON_OFF_BUTTON:
    case VWM_eVIEW_DPF_LOW_VOLTAGE_ENGINE_SHUT_DOWN:
    case VWM_eVIEW_DPF_FREQ_FAULT_ENGINE_SHUT_DOWN:
    case VWM_eVIEW_DPF_EXTREME_PRESS_ENGINE_SHUT_DOWN:
    case VWM_eVIEW_DPF_ACK_ERROR_ENGINE_SHUT_DOWN:
    case VWM_eVIEW_DPF_HEATER_FAIL:
    case VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL:
    case VWM_eVIEW_DPF_DISCONNECT_ALERT:
    case VWM_eVIEW_DPF_EXHST_TEMP_SNSR_FAIL:
    case VWM_eVIEW_DPF_EXHST_TEMP_HIGH:
    case VWM_eVIEW_DPF_EXHST_PRESS_HIGH_POST_REGEN:
    case VWM_eVIEW_DPF_CLEANING_REQUIRED:
    case VWM_eVIEW_DPF_DETECTED:
    case VWM_eVIEW_DCP_EXT_MEMORY_FAULT:
    case VWM_eVIEW_DPF_TURN_APU_ON:
    case VWM_eVIEW_DPF_APU_NO_REGEN_FOR_TOO_LONG:
    case VWM_eVIEW_DPF_TWO_FAILURES_SHUT_DOWN:
    case VWM_eVIEW_CRANK_LIMIT:
    case VWM_eVIEW_ENGINE_INTERLOCK:
//    case VWM_eVIEW_CHECK_ENGINE_ALERT:
        // Mark the view as absent
        vwm_bViewRequests[ vwm_u8ActiveView ] = FALSE;
        break;
    default:
        // Ignore other views
        break;
    }
    DPF_Timers.MSG_Display = 0;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnViewBlocksApu -->
       \brief Check if APU should be blocked from startong

<!-- PURPOSE: -->
    This function is used to check of the APU should not be allowed to start
    because some fault report is in view
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return TRUE to block the APU from starting

*/
/******************************************************************************/

bool VWM_fnViewBlocksApu( void )
{
    return ( vwm_u8ActiveView >= VWM_eVIEW_APU_BLOCKING );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnRequestView -->
       \brief Request a particular view

<!-- PURPOSE: -->
    This function is used to register the desire to show the user a particular
    view screen to inform or prompt the user.  If the view has the highest
    priority then the view will be drawn on the screen.  To test if the view
    is the on-screen active view, use VWM_fnQueryView, to drop the view from
    the active view list use VWM_fnRemoveView.
    
<!-- PARAMETERS:  -->
       @param[in]  eViewId   View identifier
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void VWM_fnRequestView( VWM_eVIEW_ID eViewId , unsigned char backlight_condition)
{
//    assert( eViewId < VWM_MAX_VIEWS );
    
    // If view is marked as not present
    if ( vwm_bViewRequests[ eViewId ] == FALSE )
    {
        if (backlight_condition == BACKLIGHT_ON)
            key_pressed = TRUE;
        // Mark the view as present
        vwm_bViewRequests[ eViewId ] = TRUE;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnRemoveView -->
       \brief 

<!-- PURPOSE: -->
    This function is used to remove a view request from the active view list.
    Views must be removed by the calling code when it is no longer needed.
    If the view was the highest priority view the screen will be changed.
    
<!-- PARAMETERS:  -->
       @param[in]  eViewId   View identifier
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void VWM_fnRemoveView( VWM_eVIEW_ID eViewId )
{
//    assert( eViewId < VWM_MAX_VIEWS );
    
    // If view is marked as present
    if ( vwm_bViewRequests[ eViewId ] != FALSE )
    {
        // Mark the view as absent
        vwm_bViewRequests[ eViewId ] = FALSE;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnQueryView -->
       \brief Ask if a view is active on the display

<!-- PURPOSE: -->
    This function is used to determine if a particular view has the highest
    priority and is presently on the screen.  In most cases to handle user
    input events (key events) the view should be on the screen.
    
<!-- PARAMETERS:  -->
       @param[in]  eViewId   View identifier
       
<!-- RETURNS: -->
      \return TRUE if the given view has the display, or FALSE if not showing

*/
/******************************************************************************/

bool VWM_fnQueryView( VWM_eVIEW_ID eViewId )
{
    return ( eViewId == vwm_u8ActiveView );
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnRefresh -->
       \brief Repaint the screen

<!-- PURPOSE: -->
    This function is used to redraw the screen.  It is used from the
    backlight control when it re-initializes the LCD controller.
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void VWM_fnRefresh( void )
{
    vwm_fnChangeView( vwm_u8ActiveView );
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: vwm_fnBestView -->
       \brief Determine the highest priority view

<!-- PURPOSE: -->
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return View Identifier of highest priority

*/
/******************************************************************************/

static uint8 vwm_fnBestView( void )
{
    uint8 u8ViewId;
    
    // If Setup has never been run
    if ( ! mugSetup )
    {
        // Stay in the menu
        return VWM_eVIEW_MENU;
    }
    
    // Search for the highest priority view
    for ( u8ViewId = ( VWM_MAX_VIEWS - 1 ); u8ViewId > 0; u8ViewId-- )
    {
        // If view is active
        if ( vwm_bViewRequests[ u8ViewId ] )
        {
            return u8ViewId;
        }
    }
    
    // Clock is the best view if no other view is active
    return VWM_eVIEW_CLOCK;
}

static void vwm_fnDpfService( void )
{
    printf("DPF SERVICE REQUIRED");
}

static void vwm_fnDpfStatus( void )
{
    if ( ( DPF_Timers.APU_Cooldown_Timer != 0 ) && ( APU_Diag.APU_Off_Requested ) )
    {
        printf( "DPF COOL DOWN: %u S", DPF_Timers.APU_Cooldown_Timer );
    }
    else
    {
        switch ( DPF_State )
        {
        case DPF_eST_REGEN_SOON:
            printf("DPF REGEN SOON");
            break;
        case DPF_eST_REGEN_NOW:
            if ( DPF_Diag.bDeferRegen )
                printf( "DPF REGEN START: %u S", APU_Diag.APU_Warmup_Timer );
            else
                printf("DPF REGEN STARTING");
            break;
        case DPF_eST_REGENNING:
            printf("DPF REGENERATING");
            break;
        case DPF_eST_REGEN_DONE:
            printf("DPF REGEN COMPLETE");
            break;
        case DPF_eST_REGEN_ABORTED:
            printf("DPF REGEN ABORTED");
            break;
        default:
            if ( DPF_Diag.Record )
                vwm_fnDpfService();
            else
                // leave blank
            break;
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: vwm_fnChangeView -->
       \brief Draw a new view

<!-- PURPOSE: -->
    This function is used to clear parts of the screen and to draw a new view.
    
<!-- PARAMETERS:  -->
       @param[in]  eViewId   View identifier
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

static void vwm_fnChangeView( VWM_eVIEW_ID eViewId )
{
    static bool bToggle = FALSE;

    // If the view is changing
    if ( eViewId != vwm_u8ActiveView )
    {
        // Clear the view area
        vwm_fnClearWorkArea();
        
        // Save the view as the active view
        // There is some reliance on the view being set before the switch statement    
        vwm_u8ActiveView = eViewId;
    }
    
    // Assume icons should be visible
    iconMakeVisible( TRUE );
    
    switch ( vwm_u8ActiveView )
    {
        case VWM_eVIEW_CLOCK:
            // View clock -- Draw it to put it on the screen right away
            vwm_fnShowClock( TRUE );
            // Current fan setting should be visible
            ICE_fnDrawFan();
            bToggle = ! bToggle;
            
            // Clear DPF show area
            lcd_cla(0,131,7,7);
            // This is the default position for DPF text, set once to save memory
            lcd_position(0,7,small);
            
            // DPF fault report
            if ( bToggle )
            {
                if ( DPF_Diag.Record )
                    vwm_fnDpfService();
                else if ( APU_ON_FLAG )
                    vwm_fnDpfStatus();
            }
            else
            {
                if ( APU_ON_FLAG )
                    vwm_fnDpfStatus();
                else if ( DPF_Diag.Record )
                    vwm_fnDpfService();
                else if (DPF_Timers.MSG_Display != 0)
                    vwm_fnDpfStatus();
            }
            break;
        
        case VWM_eVIEW_VERSION:
            vwm_fnShowVersion();
            break;

        case VWM_eVIEW_MENU:
            // Do nothing here, ViewManager will pass control
            // to menus() when appropriate
            break;
            
        case VWM_eVIEW_DPF_DETECTED:
//            lcd_position(20,3,large);
//            printf("DPF DETECTED");
            mufCenterLargeText("DPF DETECTED",3);
            break;

        case VWM_eVIEW_DPF_CLEANING_REQUIRED:
//            lcd_position(24,3,large);
//            printf("DPF FILTER");
            mufCenterLargeText("DPF FILTER",3);
            lcd_position(24,5,small);
            printf("NEEDS CLEANING");
            break;

        case VWM_eVIEW_MANUAL_THERMOSTAT:
            // Manual Thermostat should be visible
            ICE_fnDrawThermostat();
            // Current fan setting shold be visible
            ICE_fnDrawFan();
            break;
            
        case VWM_eVIEW_HVAC_BREAKER_TRIP:
            //lcd_position(20,3,large);
            //printf("HVAC BREAKER");
            mufCenterLargeText("HVAC BREAKER",3);
            break;
            
        case VWM_eVIEW_HVAC_BREAKER_RESET:
//            lcd_position(20,3,large);
//            printf("HVAC BREAKER");
            mufCenterLargeText("HVAC BREAKER",3);
            lcd_position(33,5,small);
            printf("PRESS RESET");
            break;
            
        case VWM_eVIEW_UNDER_VOLTAGE:
            //lcd_position(14,3,large);
            //printf("UNDER VOLTAGE");
            mufCenterLargeText("UNDER VOLTAGE",3);
            break;
            
        case VWM_eVIEW_APU_STARTING:
//            lcd_position(17,4,large);
//            printf("APU STARTING");
//            lcd_position(32,3,small);
//            printf("PLEASE WAIT");
            mufCenterLargeText("PLEASE WAIT",3);
            mufCenterLargeText("APU STARTING",5);
            break;
            
        case VWM_eVIEW_ENGINE_WAIT:
            lcd_position(1,4,large);
            printf("ENGINE RUN CHECK");
            lcd_position(32,3,small);
            printf("PLEASE WAIT");
            break;
            
        case VWM_eVIEW_APU_RESTARTING:
            //lcd_position(28, 4, large);
            //printf("RESTARTING");
            mufCenterLargeText("RESTARTING",4);
            break;
            
        case VWM_eVIEW_CRANK_LIMIT:
            //lcd_position(22, 4, large);
            //printf("CRANK LIMIT");
            mufCenterLargeText("CRANK LIMIT",4);
            break;
            
        case VWM_eVIEW_ENGINE_INTERLOCK:
//            lcd_position(22,3,large);
//            printf("MAIN ENGINE");
//            lcd_position(30,5,large);
//            printf("INTERLOCK");
            mufCenterLargeText("MAIN ENGINE",3);
            mufCenterLargeText("INTERLOCK",5);
            break;

		case VWM_eVIEW_HEAT_DISABLED:
            mufCenterLargeText("HEATER DISABLED",3);
            mufCenterLargeText("BY FLEET MANAGER",5);
            break;

        case VWM_eVIEW_DPF_EXHST_TEMP_SNSR_FAIL:
//            lcd_position(24,3,large);
//            printf("EXHST TEMP");
            mufCenterLargeText("EXHST TEMP",3);
            lcd_position(24,5,small);
            printf("SENSOR FAILURE");
            break;
            
        case VWM_eVIEW_DPF_EXHST_TEMP_HIGH:
//            lcd_position(24,3,large);
//            printf("EXHST TEMP");
//            lcd_position(48,5,large);
//            printf("HIGH");
            mufCenterLargeText("EXHST TEMP",3);
            mufCenterLargeText("HIGH",5);
            break;
            
        case VWM_eVIEW_CHECK_ENGINE_ALERT:
//            lcd_position(2, 3, large);
//            printf("CHECK APU ENGINE");
            mufCenterLargeText("CHECK APU ENGINE",3);
            break;

        case VWM_eVIEW_DPF_HOLD_ON_OFF_BUTTON:
            // Clear screen for large text line below
            lcd_cla( 14,131,2,3 );
            if (DPF_State == DPF_eST_REGENNING)
            {
//                lcd_position(20, 2, large);
//                printf("REGENERATING");
                mufCenterLargeText("REGENERATING",2);
            }
            else if (DPF_Timers.APU_Cooldown_Timer)
            {
//                lcd_position(20, 2, large);
//                printf("COOLING DOWN");
                mufCenterLargeText("COOLING DOWN",2);
            }
            else
            {
//                lcd_position(14, 2, large);
//                printf("REGEN REQUIRED");
                mufCenterLargeText("REGEN REQUIRED",2);
            }
            lcd_position(2, 5, small);
            printf("HOLD ON/OFF FOR 5 SEC");
            lcd_position(35, 6, small);
            printf("TO SHUTDOWN");
            break;

        case VWM_eVIEW_DPF_TURN_APU_ON:
//            lcd_position(22, 2, large);
//            printf("TURN APU ON");
            mufCenterLargeText("TURN APU ON",2);
            break;

        case VWM_eVIEW_DPF_DISCONNECT_ALERT:
//            lcd_position(35, 3, large);
//            printf("DPF LOST");
            mufCenterLargeText("DPF LOST",3);
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_EXHST_PRESS_HIGH_POST_REGEN:
            lcd_position(1, 3, large);
            printf("MUST CLEAN FLTR");
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL:
//            lcd_position(24, 3, large);
//            printf("PRESS SENSOR");
//            lcd_position(7, 5, large);
//            printf("OR SNUBBER FAIL");
            mufCenterLargeText("PRESS SENSOR",3);
            mufCenterLargeText("OR SNUBBER FAIL",5);
            break;

        case VWM_eVIEW_DPF_HEATER_FAIL:
//            lcd_position(4, 3, large);
//            printf("DPF HEATER FAIL");
            mufCenterLargeText("DPF HEATER FAIL",3);
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_EXTREME_PRESS_ENGINE_SHUT_DOWN:
            lcd_position(0, 3, large);
            printf("DPF FLTR PLUGGED");
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_LOW_VOLTAGE_ENGINE_SHUT_DOWN:
            lcd_position(4, 3, large);
            printf("GEN VOLTAGE LOW");
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_FREQ_FAULT_ENGINE_SHUT_DOWN:
            lcd_position(4, 3, large);
            printf("GEN FREQ FAULT");
            lcd_position(18, 5, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_ACK_ERROR_ENGINE_SHUT_DOWN:
//            lcd_position(22, 3, large);
//            printf("ACKNOWLEDGE");
//            lcd_position(45, 5, large);
//            printf("ERROR");
            mufCenterLargeText("ACKNOWLEDGE",3);
            mufCenterLargeText("ERROR",5);
            lcd_position(18, 7, small);
            printf("ENGINE SHUT DOWN");
            break;

        case VWM_eVIEW_DPF_APU_NO_REGEN_FOR_TOO_LONG:
//            lcd_position(20, 3, large);
//            printf("EXCESSIVE TIME");
//            lcd_position(18, 5, large);
//            printf("BETWEEN REGENS");
            mufCenterLargeText("EXCESSIVE TIME",3);
            mufCenterLargeText("BETWEEN REGENS",5);	
            lcd_position(44, 7, small);
            printf("SERVICE DPF");
            break;

        case VWM_eVIEW_DPF_TWO_FAILURES_SHUT_DOWN:
            lcd_position(9, 3, large);
            printf("PRESS AND TEMP");
            lcd_position(9, 5, large);
            printf("SENSOR FAILURE");
            lcd_position(18, 7, small);
            printf("ENGINE SHUT DOWN");
            break;

#ifdef EXT_MEM
        case VWM_eVIEW_DCP_EXT_MEMORY_FAULT:
//            lcd_position(22, 4, large);
//            printf("MEMORY ERROR");
            mufCenterLargeText("MEMORY ERROR",4);
            break;
#endif // EXT_MEM
        default:
//            assert( FALSE );
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: VWM_fnViewManager -->
       \brief Key and event handler for view manager

<!-- PURPOSE: -->
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

// Key and event handler for the view manager
void VWM_fnViewManager( void )
{
    static bool bOnce = FALSE;
    uint8 u8BestView;

    if ( !bOnce )
    {
        // Start with clock as the best view
        vwm_fnChangeView( VWM_eVIEW_CLOCK );
        
        bOnce = TRUE;
    }
    
    // REMEMBER - Real-time is used for clock and for USER real-timers
    // Reading the real-time clock is put here because the clock display needs
    // to check the time more often so the seconds show more accurately
    // If the Real-time clock read again timer has expired
    if ( !timers.readRTCTmr )
    {
        // Request an update from the real-time clock
        rtfRequestClock();
        
        // Restart the timer ( tenth of second )
        timers.readRTCTmr = 10;
    }

    // If menu button is pressed and the view is lower priority than menu
    if ( ( MENU_BTN ) && ( vwm_u8ActiveView < VWM_eVIEW_MENU_IDLE ) )
    {
        // Bring the menu into view
        VWM_fnRequestView( VWM_eVIEW_MENU, BACKLIGHT_ON );
    }

    // Pick the best view and show it
    u8BestView = vwm_fnBestView();
    
    // If the best view is not the active view
    if ( u8BestView != vwm_u8ActiveView )
    {
        // Change the view
        vwm_fnChangeView( u8BestView );
    }

    // Manage events for these views
    switch ( vwm_u8ActiveView )
    {
        case VWM_eVIEW_CLOCK:
            // Update clock display when it is out of date
            vwm_fnShowClock( FALSE );
            break;
            
        case VWM_eVIEW_VERSION: // fall through to menu
        case VWM_eVIEW_MENU:
                // menus() manages its own view and key events
                menus();
            break;

        default:
            break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: vwm_fnClearWorkArea -->
       \brief Clears the menu work area region of the screen

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void vwm_fnClearWorkArea( void )
{
    lcd_cla( 0, 131, 2, 7 );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: vwm_fnShowClock -->
       \brief Displays the clock 

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

static void vwm_fnShowClock( bool bDraw )
{
    static char lastSecond;
    char rtHours;
    char testSecond;
    const char *ampm;
    char xpos;

    // If the clock is Ready use RTC seconds else use software seconds
    testSecond = ( rtgClock.rtxReady ? rtgClock.rtxSeconds : seconds );
    
    // If time has changed or redraw requested
    if ( ( testSecond != lastSecond ) || bDraw )
    {
        lastSecond = testSecond;
        // Erase clock screen area
        //lcd_cla( 20, 108, 3, 6 );
        lcd_cla( 0, 108, 3, 6 );
    
        if ( ( !rtgClock.rtxReady ) && ( seconds & 0x01 ) )
            // Blink clock by not displaying on odd seconds
            return;
            
        // Use real-time clock hours for display
        rtHours = rtgClock.rtxHours;

        // If 24 hour mode then do not change hours
        if ( mugClockMode24 )
        {
            // No AM or PM
            ampm = "";
            xpos = 8;
        }
        else
        {
            // Change hours to 12 hour mode from 24 hour mode
            xpos = 8;
            
            // Set AM or PM and adjust hours
            if ( rtHours > 11 )
            {
                rtHours -= 12;
                if ( rtHours == 0 )
                    rtHours = 12;
                ampm = "PM";
            }
            else
            {
                if ( rtHours == 0 )
                    rtHours = 12;
                ampm = "";
            }
        }
        
        lcd_position( xpos, 3, large );
        printf( "%02d:%02d%s|",
                        rtHours,
                        rtgClock.rtxMinutes,
                        //rtgClock.rtxSeconds,
                        ampm );

        if ( rtgClock.rtxDayOfWeek < 7 )
        {

            lcd_position(xpos+12, 5, large);
            printf("%s|", mufChoiceString( CHOOSE_SUNDAY + rtgClock.rtxDayOfWeek ));

            lcd_position( xpos+61, 5, large);
            printf( "%3dF ",  (signed int)tmfVtoCF( TEMPERATURE, FAHRENHEIT));

            lcd_position( xpos+61, 3, large);
            printf( " %2dC ", (TM_CELSIUS)tmfVtoCF( TEMPERATURE, CELSIUS));
            
        
            
        }
        
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: vwm_fnShowVersion -->
       \brief Draw the onscreen version information

<!-- PURPOSE: -->
    
<!-- PARAMETERS: none -->
       
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

static void vwm_fnShowVersion( void )
{
    int v;

    lcd_position(26,3,large);
    printf("COMFORTPRO");
    lcd_position(5,5,small);
    printf("MULTI-FUNCTION PANEL");
    lcd_position(5,6,small);
    printf(version);
    
#ifndef EXTRA_TIMERS
     lcd_position( 15, 7, small );
     v = battery_voltage();
     printf( "%2d.%dV", v / 10, v % 10 );
#endif // EXTRA_TIMERS
    lcd_position( 100, 7, small );
    printf( "%2dC", (TM_CELSIUS)tmfVtoCF( TEMPERATURE, CELSIUS ) );
}

///******************************************************************************/
///*!
//
//<!-- FUNCTION NAME: VWM_fnCenterLargeText -->
//       \brief Calculate the extent of string and display it centered across
//
//<!-- PURPOSE: -->
//<!-- PARAMETERS: -->
//       @param[in]  label   String to display
//       @param[in]  page    page (Y coordinate) for line on which to display string
//<!-- RETURNS: -->
//      \return Void
//
//*/
///******************************************************************************/
//
//void VWM_fnCenterLargeText( const char *label, char page )
//{
//    char len;
//    char textX;
//    
//    // Get string length
//    for ( len = 0; label[ len ] != 0; len++ )
//        ;
//        
//    // Convert length to character widths/2
//    len *= 8/2;
//
//    // Position text
//    textX = 66 - len;         // 66 is taken for center character
//    
//    lcd_position( textX, page, large );
//    printf( "%s", label );
//}

