/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file Events.h
       \brief Header file for event manager

<!-- DESCRIPTION: -->
       Header file for event manager

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/Events.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2009/02/06 00:18:18  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.14  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.13 Tue Oct 28 15:54:28 2008 
         - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
         - Removed the display of APU shutdown reason after ESC/RESET key push.
         - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

         Revision 1.12 Thu Sep 25 17:20:21 2008 
         -Added a new feature to prevent multiple regenerations when the exhaust pressure sensor fault is unstable. The fault will result in normal action when it happens for the first time but won't force regen for next fault triggers until the "first time" condition is removed. This condition will be removed if two normal soot collection/regen period are observed. The previous feature that forced regen every 8 hours if the fault exists is still in effect.

         Revision 1.11 Thu Aug 21 15:43:11 2008 
         - Added feature to turn on the backlight when fault or important messages are being displayed.
         - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
         - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
         - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

         Revision 1.10 Mon Jul 14 16:52:42 2008 
         Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

         Revision 1.9  Wed May 21 17:37:58 2008 
         - Renamed some of the DPF timers and their associated events to better reflect their usage.
         - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

         Revision 1.8  Thu Apr 24 15:44:27 2008 
         - Added supporting definitions for new FMIs

         Revision 1.7  Fri Dec 21 20:36:43 2007 
         - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
         - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
         - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
         - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

         Revision 1.6  Fri Oct 26 18:42:14 2007 
         Added 5 minute cooldown for APU after DPF regeneration.
         Changed DPF variable menu to show all variables in one screen.
         Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
         Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

         Revision 1.5  Thu Oct 18 20:18:40 2007 
         Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

         Revision 1.4  Mon Oct 15 16:51:21 2007 
         Added ext. memory error event.

         Revision 1.3  Tue Oct 9 15:03:18 2007 
         Version 35-PPP-002. Second round of testing..

         Revision 1.2  Mon Sep 17 18:41:35 2007 
         DPF related modifications

         Revision 1.1  Mon Aug 27 22:26:33 2007 
         *** empty log message ***

         \endif
*/
/******************************************************************************/

// Event definitions
enum 
{
	// keypad generated events
//	EV_ON_OFF_KEY,
//	EV_HVAC_OFF_KEY,
//	EV_MENU_KEY,
	EV_ESC_KEY,
//	EV_LEFT_ARROW_KEY,
//	EV_RIGHT_ARROW_KEY,
//	EV_AC_KEY,
//	EV_FAN_KEY,
//	EV_HEAT_KEY,
	// menu generated events
	EV_USER_REGEN,
	// timer generated events
	EV_DPF_NO_ACK,			// one second
	EV_DPF_DISCONNECTED,	// 10 seconds
	EV_DPF_DM1_CLEARED,		// 15 seconds (must be greater than disconnection detection time)
	EV_DPF_MSG_DISPLAY,
	EV_DPF_ONE_HR,
    EV_DPF_8_HR,
	EV_APU_WARMUP,
	// APU generated events
	EV_APU_GOING_OFF,
	// CCU generated events
	// J1939/DPF module generated events
	EV_ACK_RCVD,
	EV_DPF_STAT_RCVD,
	EV_REGENNING,
    EV_REGEN_DONE,
    EV_REGEN_IDLE,
	EV_REGEN_NOW,
	EV_REGEN_SOON,
	EV_CLEAN_FLTR,
	EV_EXHST_PRESS_SNSR_FAIL,
	EV_EXHST_PRESS_SNUB_PLUG,
	EV_EXHST_PRESS_HIGH_POST_REGEN,
	EV_EXHST_TEMP_SNSR_FAIL,
	EV_EXHST_TEMP_HIGH,
    EV_HEATER_FAIL,
    EV_HEATER_TIMEOUT,
	EV_EXTREME_PRESS,
    EV_ACK_ERROR,
	EV_EXHST_PRESS_SNSR_CLR,
	EV_EXHST_PRESS_SNUB_PLUG_CLR,
	EV_EXHST_TEMP_SNSR_CLR,
	EV_EXHST_PRESS_HIGH_POST_REGEN_CLR,
	EV_EXHST_TEMP_HIGH_CLR,
	EV_EXTREME_PRESS_CLR,
	EV_HEATER_FAIL_CLR,
    EV_HEATER_TIMEOUT_CLR,
    EV_LOW_VOLTAGE,
    EV_LOW_VOLTAGE_CLR,
    EV_FREQ_FAULT,
    EV_FREQ_FAULT_CLR,
	EV_FLTR_CLEANED,
    EV_APU_COOL_NOW,
    EV_APU_NO_REGEN_FOR_TOO_LONG,
    EV_UPDATE_APU_HOUR_WITHOUT_REGEN,
	// DCP core generated events
	EV_MAP_LOW,
    // External memory fault
    EXT_MEMORY_FAULT
};

#define EVENT_BUFFER_SIZE	10
#define NO_EVENT	0xFF

void Add_Event (unsigned char event);
unsigned char Get_Event (void);
