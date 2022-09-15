/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file DPF.h
       \brief Interface for DPF Manager and DPF state machine functions

<!-- DESCRIPTION: -->
       Interface for DPF Manager and DPF state machine functions

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/DPF.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.3  2014/04/16 14:07:06  blepojevic
       Added diagnostic of APU and CCU

       Revision 1.2  2013/09/19 13:07:42  blepojevic
       Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

       Revision 1.1  2010/01/12 23:22:06  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
		
    	Revision 1.27  2010-01-12 15:12  echu
        - Added apu on/off qualifier to EV_LOW_VOLTAGE and EV_FREQ_FAULT. The DPF does not regonize fast enough that the APU has shut down and may throw these events.   

       Revision 1.26  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.25 Tue Oct 28 15:54:28 2008 
         - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
         - Removed the display of APU shutdown reason after ESC/RESET key push.
         - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

         Revision 1.24 Thu Oct 9 18:02:28 2008 
         Changed and cleaned messages.

         Revision 1.23 Thu Sep 25 17:20:21 2008 
         -Added a new feature to prevent multiple regenerations when the exhaust pressure sensor fault is unstable. The fault will result in normal action when it happens for the first time but won't force regen for next fault triggers until the "first time" condition is removed. This condition will be removed if two normal soot collection/regen period are observed. The previous feature that forced regen every 8 hours if the fault exists is still in effect.

         Revision 1.22 Thu Aug 21 15:43:11 2008 
         - Added feature to turn on the backlight when fault or important messages are being displayed.
         - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
         - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
         - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

         Revision 1.21 Tue Aug 12 20:40:07 2008 
         Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

         Revision 1.20 Mon Jul 28 17:15:58 2008 
         *** empty log message ***

         Revision 1.19 Mon Jul 14 16:52:42 2008 
         Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

         Revision 1.18 Wed May 21 17:37:58 2008 
         - Renamed some of the DPF timers and their associated events to better reflect their usage.
         - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

         Revision 1.17 Thu Apr 24 15:44:27 2008 
         - Added supporting definitions for new FMIs

         Revision 1.16 Thu Apr 10 16:45:17 2008 
         - Made a distinction between normal and emergency regen.
         - Extreme pressure and pressure sensor faults will be detected during normal regen.
         - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
         - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
         - A new command will be sent to DPF when an emergency regen is initiated.
         - APU warmup reduced to 2 minutes (from 5).

         Revision 1.15 Tue Mar 25 19:14:38 2008 
         - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
         - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
         - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
         - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
         - APU cooldown period is reduced to 2 minutes.
         - DPF related faults won't be captured when APU is off.
         - Freed code space by removing redundant code.
         - Faults are not displayed in DPF menu anymore.
         - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

         Revision 1.14 Thu Feb 21 20:08:04 2008 
         - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
         - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
         - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

         Revision 1.13 Wed Feb 6 16:29:55 2008 
         - Added APU hour editing functionality. This can be excluded by removing the compile option switch "EDIT_APU_HR".
         - Corrected low voltage SPN. This was 5 and now is 4.

         Revision 1.12 Wed Jan 9 21:45:44 2008 
         - Increased the low voltage detection delay at APU start up to 20 second. Also increased the number of consecutive low generator voltage indicator messages from 3 to 5.
         - Added DPF regeneration countdown message to the bottom of the home screen.

         Revision 1.11 Fri Dec 21 20:36:43 2007 
         - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
         - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
         - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
         - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

         Revision 1.10 Thu Dec 20 18:19:06 2007 
         - Fixed triggering, clearing and logging of DPF Lost.
         - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
         - Fixed using the invalid 520192 SPN.
         - Changed the routine ssfI2CReadFrom to return a value (true or false)
         - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
         - Changed the format of OBM records to include time and the fault in one same record.
         - Added a menu item to display the historical faults.
         - OBM related changes are excludable with OBM compile option.

         Revision 1.9  Thu Dec 6 21:40:09 2007 
         - Changed the message that delays the APU shutdown to also show the coolddown period.
         - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
         - Eliminated the "Heater Status" from DPF menu.

         Revision 1.8  Fri Nov 30 18:52:07 2007 
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

         Revision 1.7  Fri Oct 26 18:42:14 2007 
         Added 5 minute cooldown for APU after DPF regeneration.
         Changed DPF variable menu to show all variables in one screen.
         Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
         Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

         Revision 1.6  Wed Oct 24 15:09:30 2007 
         Removed left over test code. Compile options: UNIT_TEST & MEMORY_TESTING.

         Revision 1.5  Fri Oct 19 21:12:42 2007 
         New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

         Revision 1.4  Thu Oct 18 20:18:40 2007 
         Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

         Revision 1.3  Tue Oct 9 15:03:18 2007 
         Version 35-PPP-002. Second round of testing..

         Revision 1.2  Mon Sep 17 18:41:35 2007 
         DPF related modifications

         Revision 1.1  Mon Aug 27 22:26:33 2007 
         *** empty log message ***

       \endif
*/
/******************************************************************************/

#ifndef DPF_H
#define DPF_H

#include "j1939.h"
#include "tfxstddef.h"     // Teleflex Standard definitions

// DPF state definitions
enum
{
	DPF_eST_STANDBY,
	DPF_eST_REGEN_SOON,
	DPF_eST_REGEN_NOW,
	DPF_eST_REGENNING,
	DPF_eST_REGEN_DONE,
    DPF_eST_REGEN_ABORTED
};

// Bit definitions of status message byte 1 and the values they can take
#define DPF_REGEN_REQUIRED_BITS     0x03
#define DPF_REGEN_REQUIRED_OK       0
#define	DPF_REGEN_REQUIRED_SOON     1
#define DPF_REGEN_REQUIRED_NOW      2
#define DPF_REGEN_REQUIRED_UNKNOWN  3

// Bit definitions of status message byte 4 and the values they can take
#define DPF_HSM_STATE_BITS          0x3E
#define DPF_REGEN_PROGRESS_BITS     0xC0
#define DPF_REGEN_IS_DONE_OK        0
#define DPF_REGEN_IS_DONE_NOT_OK    1
#define DPF_REGEN_IS_ACTIVE_NORMAL  2
#define DPF_REGEN_IS_ACTIVE_EMERG   3


#define FAULT_BITS			0x0C
#define NO_FAULT			0
#define NON_CRITICAL		1
#define CRITICAL			2

#define	FILTER_FROZEN		0x10

#define GEN_VOLTAGE_BIT		0x20
#define VOLTAGE_LOW			0
#define VOLTAGE_OK			1
#define MAX_MINUTES_WITHOUT_REGEN   3000    // 50 hours

#define CLEAN_FLTR_BIT		0x80
#define CLEAN				0
#define DIRTY				1

// Regeneration command message constants (PGN 65301)
#define END_REGEN			0   // Initial state - No action required
#define START_REGEN			1   // Start regen (caused by a fault)
#define ABORT_REGEN			2   // Abort regen
#define EMERGENCY_REGEN     3   // Start emergency regen
#define START_REGEN_NORMAL  4   // Start normal regen

#define DISCONNECT_DETECTION_TIME	10	// Seconds
#define DPF_COOL_DOWN_TIME			120 // Seconds
#define DPF_MSG_DISPLAY_TIME		60 // Seconds
#define DM1_CLEARING_TIME			3
#define COMMAND_ACK_TIME			250	// Milli seconds
#define MAX_NUMBER_NO_ACK_COMMAND   60

// Bit definitions for reported DM1 faults
#define DM1_EXHST_TEMP_SNSR_FAIL		0x0001
#define DM1_EXHST_PRESS_EXTREME			0x0002
#define DM1_EXHST_PRESS_SNSR_FAIL		0x0004
#define DM1_EXHST_PRESS_SNUB_PLUG		0x0008
#define DM1_HEATER_FAIL					0x0010
#define DM1_GEN_LOW_VOLTAGE				0x0020
#define DM1_EXHST_TEMP_HIGH         	0x0040
#define DM1_EXHST_PRESS_HIGH_POST_REGEN 0x0080
#define DM1_HEATER_TIMEOUT	        	0x0100
#define DM1_GEN_FREQ_FAULT          	0x0200

// DPF_Diag.Record bit definitions
#define MISSING					        0x0001
#define EXHST_TEMP_SNSR_FAIL	        0x0002
#define EXHST_PRESS_HIGH		        0x0004
#define EXHST_PRESS_SNSR_FAIL	        0x0008
#define HEATER_FAIL				        0x0010
#define GEN_LOW_VOLTAGE			        0x0020
#define MUST_CLEAN_FILTER		        0x0040
#define EXHST_TEMP_HIGH                 0x0080
#define EXHST_PRESS_SNSR_FAIL_ONCE      0x0100
#define EXHST_PRESS_HIGH_POST_REGEN     0x0200
#define EXHST_PRESS_SNUB_PLUG           0x0400
#define HEATER_TIMEOUT	                0x0800
#define TWO_FAILURES       	            0x1000
#define GEN_FREQ_FAULT                  0x2000
//#define free		                    0x4000
//#define free	                        0x8000

// Reasons for APU to be forced shutdown:
#define NO_REASON               0
#define REASON_MISSING          1
#define REASON_EXHST_PRESS_HIGH 2
#define REASON_HEATER_FAIL      3
#define REASON_GEN_LOW_VOLTAGE  4
#define REASON_TWO_FAILURES     5
#define REASON_EXHST_TEMP_HIGH  6
#define REASON_FREQ_FAULT       7

// J1939 protocol related constants
#define DPF_SOURCE_ADDRESS		130
#define POSITIVE_ACK			0
#define VALID_CAN_DATA			250

// SPN definitions for faults
#define SPN_DPF_DISCONNECTED    520194
#define FMI_DPF_DISCONNECTED    12
#define FMI_DPF_ABNORMAL_FREQ   8

#define SPN_HEATER_FAIL         520195
#define FMI_HEATER_FAIL         12
#define FMI_HEATER_TIMEOUT      31

#define SPN_EXHST_TEMP          520196
#define FMI_EXHST_TEMP_HIGH     0
#define FMI_EXHST_TEMP_SNSR_FAIL     12

#define SPN_EXHST_PRESS         520197
#define FMI_EXTREME_PRESS       0
#define FMI_PRESS_SNUBBER_PLUG  7
#define FMI_PRESS_SNSR_FAIL     12
#define FMI_PRESS_HIGH_POST_REGEN	31

#define SPN_GEN_VOLTAGE         520202
#define FMI_LOW_VOLTAGE         4

#define SPN_GEN_FREQ            520201
#define FMI_FREQ_FAULT			1

// Temperature related constants
#define EXHST_TEMP_RESOLUTION   3
#define EXHST_TEMP_OFFSET       0

typedef struct
{
	// J1939 Status message variables
	unsigned char EXHST_Press;              // Raw exhaust pressure value
	unsigned char EXHST_Press_To_View;      // Processed exhaust pressure 
                                            // value to be shown on screen
	unsigned int  EXHST_Press_To_Average;   // samples of pressure will be 
                                            // added together here to be 
                                            // averaged later

	unsigned char EXHST_Temp;               // Raw exhaust temperature value
	unsigned int  EXHST_Temp_To_View;       // Processed exhaust pressure 
                                            // value to be shown on screen
	unsigned int  EXHST_Temp_To_Average;    // samples of pressure will be 
                                            // added together here to be 
                                            // averaged later

    unsigned char Number_Of_Samples;        // variable to hold how many 
                                            // samples of temperature and 
                                            // pressure we have accumulated

    unsigned char Required          :2;     // Regeneration required
	unsigned char Faults			:2;     // Faults status
    unsigned char Frozen_Filter     :1;     // Weather too cold, filter frozen
    unsigned char Gen_Voltage       :1;     // Generator voltage (present or not)
	unsigned char Clean_Filter		:1;     // Do we need to clean the filter?
    unsigned char Regenning         :1;     // Regeneration in progress
    unsigned char Progress          :2;     // Regeneration status
} DPF_INFO;

typedef struct
{
	unsigned char bPresent		        	:1; // is the DPF present in the system?
	unsigned char bInstalled		        :1; // is the DPF installed in the system?
	unsigned char bDeferRegen		        :1; // hold off regen for APU warmup
	unsigned char bRegenCmdSent		        :1; // has regen command be sent
	unsigned char bNeedToRegen		        :1; // is a regen required
	unsigned char bNeedToCool		        :1; // is cool down time required
    unsigned char Press_Snsr_Wants_Regen    :1; // flag pressure sensor failure regen success
    unsigned char Emergency_Regen_Flag      :1; // To allow APU to run once when it 
                                                // would normally shutdown due to 
                                                // a faulty condition
    unsigned char Consecutive_Good_Regens;      // Count of consecutive good regens
    unsigned char Number_No_ACK_Commands;       // how many times we didn't get the ACK?
	unsigned int Record;                        // This holds which faults have already 
                                                // been detected or cleared
    unsigned char Last_Regen_Command;           // Last command that was send to DPF
    unsigned long APU_Minutes_Without_Regen;     // How long (minutes) we have been running without a regen
    unsigned char APU_Hours_Without_Regen;      // How long (hours) we have been running without a regen
} DPF_DIAG;

typedef struct
{
	unsigned char Wait_For_ACK;
	unsigned char Disconnection;
	unsigned char MSG_Display;
	unsigned char One_Hour;
	unsigned char Eight_Hour;
    unsigned char APU_Cooldown_Timer;
}DPF_TIMERS;

extern DPF_INFO DPF_Info;
extern DPF_TIMERS DPF_Timers;
extern DPF_DIAG DPF_Diag;

extern uint8 DPF_u8DpfAccess;

void Initialize_DPF_Variables (void);
void Reset_DPF_Flags(void);
void Update_DPF_Status (j1939_RxMsg_Struct *message);
void DPF_State_Machine (unsigned char event);
void DPF_DM1_Tick( void );
void DPF_DM1_Faults(unsigned long spn, unsigned char fmi);
void DPF_Acknowledgement_Received(j1939_RxMsg_Struct *message);
void Record_Fault(unsigned long spn, unsigned char fmi, unsigned char flag);
void Abort_Regen (void);
void Install_DPF (void);
void Uninstall_DPF (void);
void Clear_DPF_Screen(void);
void Set_Emergency_Regen_Flag(void);
void DPF_fnTurnOffLoads( void );
void Service_Two_Failures(void);

#endif  // DPF_H
