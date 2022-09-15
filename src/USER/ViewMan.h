/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file viewman.h
       \brief Header file defining the interface to the View Manager

<!-- DESCRIPTION: -->
    This file provides the interface for using the View Manager

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/ViewMan.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.3  2014/07/25 15:24:15  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.2  2010/12/01 22:44:24  dharper
          Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
          This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
          This is because REV B was never checked in.

          Revision 1.1  2009/02/17 21:36:36  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.14  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.13  2008-10-28 15:54:28  msalimi
          - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
          - Removed the display of APU shutdown reason after ESC/RESET key push.
          - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

          Revision 1.12  2008-10-09 18:02:28  msalimi
          Changed and cleaned messages.

          Revision 1.11  2008-08-21 15:43:11  msalimi
          - Added feature to turn on the backlight when fault or important messages are being displayed.
          - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
          - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
          - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

          Revision 1.10  2008-08-12 20:40:07  msalimi
          Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

          Revision 1.9  2008-07-14 16:52:42  msalimi
          Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

          Revision 1.8  2008-04-24 15:44:27  msalimi
          - Added supporting definitions for new FMIs

          Revision 1.7  2007-12-06 23:21:32  msalimi
          Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

          Revision 1.6  2007-11-30 18:52:07  msalimi
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

          Revision 1.5  2007-10-23 19:04:30  msalimi
          *** empty log message ***

          Revision 1.4  2007-10-15 16:57:46  msalimi
          Added memory error display.

          Revision 1.3  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.2  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.1  2007-08-27 22:26:33  msalimi
          *** empty log message ***


       \endif
*/
/******************************************************************************/

#ifndef VIEW_MAN_H
#define VIEW_MAN_H

#include "tfxstddef.h"     // Teleflex Standard definitions

/* ---------- D e f i n i t i o n s ---------- */

// Enumeration of views in priority order, larger is better
typedef enum vwm_eVIEW_ID
{
    VWM_eVIEW_CLOCK = 0,            // Lowest Priority
    VWM_eVIEW_MANUAL_THERMOSTAT,
    VWM_eVIEW_DPF_HOLD_ON_OFF_BUTTON,
    VWM_eVIEW_DPF_TURN_APU_ON,
    VWM_eVIEW_MENU_IDLE,            // Place holder for priority of no menu
    VWM_eVIEW_VERSION,              // Should probably be just above menu idle
    VWM_eVIEW_DPF_DETECTED,
    VWM_eVIEW_DPF_CLEANING_REQUIRED,
    VWM_eVIEW_APU_STARTING,
    VWM_eVIEW_ENGINE_WAIT,
    VWM_eVIEW_APU_RESTARTING,
    VWM_eVIEW_MENU,
    VWM_eVIEW_UNDER_VOLTAGE,
    VWM_eVIEW_DPF_EXHST_TEMP_SNSR_FAIL,
    VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL,
    VWM_eVIEW_DCP_EXT_MEMORY_FAULT,
    VWM_eVIEW_DPF_APU_NO_REGEN_FOR_TOO_LONG,
                                    // These fault views block the APU from starting
    VWM_eVIEW_APU_BLOCKING,         // Place holder for APU blocking
    VWM_eVIEW_DPF_EXHST_TEMP_HIGH,
    VWM_eVIEW_DPF_ACK_ERROR_ENGINE_SHUT_DOWN,
    VWM_eVIEW_DPF_DISCONNECT_ALERT,
    VWM_eVIEW_DPF_EXTREME_PRESS_ENGINE_SHUT_DOWN,
    VWM_eVIEW_DPF_FREQ_FAULT_ENGINE_SHUT_DOWN,
    VWM_eVIEW_DPF_HEATER_FAIL,
    VWM_eVIEW_DPF_TWO_FAILURES_SHUT_DOWN,
    VWM_eVIEW_HVAC_BREAKER_TRIP,
    VWM_eVIEW_HVAC_BREAKER_RESET,
    VWM_eVIEW_DPF_LOW_VOLTAGE_ENGINE_SHUT_DOWN,
    VWM_eVIEW_CRANK_LIMIT,
    VWM_eVIEW_ENGINE_INTERLOCK,
	VWM_eVIEW_HEAT_DISABLED,
    VWM_eVIEW_CHECK_ENGINE_ALERT,
    VWM_eVIEW_DPF_EXHST_PRESS_HIGH_POST_REGEN,
    VWM_eVIEW_LAST_ENTRY
} VWM_eVIEW_ID;

#define VWM_MAX_VIEWS           VWM_eVIEW_LAST_ENTRY

/* ---------- G l o b a l   D a t a ---------- */


/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void VWM_fnClearAlert( void );
bool VWM_fnViewBlocksApu( void );
void VWM_fnRequestView( VWM_eVIEW_ID eViewId , unsigned char backlight_condition );
void VWM_fnRemoveView( VWM_eVIEW_ID eViewId );
bool VWM_fnQueryView( VWM_eVIEW_ID eViewId );
void VWM_fnRefresh( void );
void VWM_fnViewManager( void );
//void VWM_fnCenterLargeText( const char *label, char page );

#endif  // ALERT_VIEW_H


