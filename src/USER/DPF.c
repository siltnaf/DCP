/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file DPF.c
       \brief DPF application control and state machine

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

       CHANGE HISTORY:
  
        $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/DPF.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
  
        $Log: not supported by cvs2svn $
        Revision 1.3  2014/04/16 14:07:06  blepojevic
        Added diagnostic of APU and CCU

        Revision 1.2  2013/09/19 13:07:41  blepojevic
        Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

        Revision 1.1  2010/01/12 23:22:06  blepojevic
        No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

        Revision 1.48  2010-01-12 15:12  echu
        - Added apu on/off qualifier to EV_LOW_VOLTAGE and EV_FREQ_FAULT. The DPF does not regonize fast enough that the APU has shut down and may throw these events.   

        Revision 1.47  2008-11-14 19:27:36  gfinlay
        - Added file header comment blocks for several files that were missing these.
        - Added debug uart support with serial.c and serial.h

        Revision 1.46 Wed Oct 29 17:08:21 2008
        Adjusted the position of some messages.

        Revision 1.45 Tue Oct 28 15:54:28 2008
        - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
        - Removed the display of APU shutdown reason after ESC/RESET key push.
        - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

        Revision 1.44 Mon Oct 20 18:08:52 2008
        Improvements in displayed messages.

        Revision 1.43 Thu Oct 9 18:02:28 2008
        Changed and cleaned messages.

        Revision 1.42 Wed Oct 1 17:22:05 2008
        *** empty log message ***

        Revision 1.41 Thu Sep 25 18:43:13 2008
        *** empty log message ***

        Revision 1.40 Thu Sep 25 17:33:29 2008
        - Added a two minutes delay for any regen after APU is turned on.

        Revision 1.39 Thu Sep 25 17:20:21 2008
        -Added a new feature to prevent multiple regenerations when the exhaust pressure sensor fault is unstable. The fault will result in normal action when it happens for the first time but won't force regen for next fault triggers until the "first time" condition is removed. This condition will be removed if two normal soot collection/regen period are observed. The previous feature that forced regen every 8 hours if the fault exists is still in effect.

        Revision 1.38 Wed Sep 3 15:49:04 2008
        *** empty log message ***

        Revision 1.37 Thu Aug 21 15:43:11 2008
        - Added feature to turn on the backlight when fault or important messages are being displayed.
        - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
        - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
        - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

        Revision 1.36 Wed Aug 13 17:36:37 2008
        Added comments

        Revision 1.35 Tue Aug 12 20:40:07 2008
        Implemented a new feature to turn off the APU if there are temperatrue and pressure sensor failures at the same time.

        Revision 1.34 Mon Jul 28 17:13:21 2008
        Implemented a two minutes delay in detecting exhaust pressure sensor fault to allow the APU warm up before forcing regeneration as a result of this fault.

        Revision 1.33 Mon Jul 14 16:52:42 2008
        Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

        Revision 1.32 Tue Jun 3 17:34:27 2008
        - ded a message for heat adjustment operation to display "Regenerating" when in regen mode.
        - Cancelled restoring the loads when the sensor pressure and extreme pressure failures are cleared. The loads are now being restored only when regen is finished.

        Revision 1.31 Wed May 21 17:57:47 2008
        - Renamed some of the DPF timers and their associated events to better reflect their usage.
        - Added a check for emergency regen request flag at the time of normal regen to avoid clearing of this condition by a normal regen condition that may be present.

        Revision 1.30 Tue May 13 19:13:35 2008
        - Added new feature to treat transitions from regenning to any other state the same as transitioning to regen ok state.
        - Removed warning message related to compliing for COMFORTPRO as this is always the case.

        Revision 1.29 Tue May 6 18:47:34 2008
        Added hardware level filter to CAN operation to receive only messages with 29 bit identifier.

        Revision 1.28 Thu Apr 24 15:32:24 2008
        - Added new FMIs.
        - Added frozen filter functionality

        Revision 1.27 Thu Apr 10 16:45:17 2008
        - Made a distinction between normal and emergency regen.
        - Extreme pressure and pressure sensor faults will be detected during normal regen.
        - Extreme pressure and pressure sensor faults will not be detected during emergency regen.
        - The start regen menu will show EMERGENCY REGEN when the APU is off and this regen will be considered an emergency regen with all of its applicable features.
        - A new command will be sent to DPF when an emergency regen is initiated.
        - APU warmup reduced to 2 minutes (from 5).

        Revision 1.26 Wed Apr 2 19:50:10 2008
        - Fixed onboard memory reading bug.
        - There is no one hour recall of faults if APU is off.
        - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
        - DPF lost fault will not be detected if APU is off.

        Revision 1.25 Wed Mar 26 21:27:50 2008
        Fixed side effects of recent changes to how APU warmup period is handled for regeneration.

        Revision 1.24 Tue Mar 25 19:14:38 2008
        - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
        - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
        - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
        - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
        - APU cooldown period is reduced to 2 minutes.
        - DPF related faults won't be captured when APU is off.
        - Freed code space by removing redundant code.
        - Faults are not displayed in DPF menu anymore.
        - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

        Revision 1.23 Thu Jan 31 20:54:29 2008
        -Added feature to clear DPF missing fault when DPF is uninstalled.
        -Made external and on-board memory applications independant of each other.

        Revision 1.22 Wed Jan 9 21:45:44 2008
        - Increased the low voltage detection delay at APU start up to 20 second. Also increased the number of consecutive low generator voltage indicator messages from 3 to 5.
        - Added DPF regeneration countdown message to the bottom of the home screen.

        Revision 1.21 Tue Jan 8 19:22:07 2008
        Added history

        Revision 1.20 Tue Jan 8 19:20:04 2008
        Added history

        Revision 1.19 Fri Dec 21 20:36:43 2007
        - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
        - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
        - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
        - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

        Revision 1.18 Thu Dec 20 18:19:06 2007
        - Fixed triggering, clearing and logging of DPF Lost.
        - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
        - Fixed using the invalid 520192 SPN.
        - Changed the routine ssfI2CReadFrom to return a value (true or false)
        - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
        - Changed the format of OBM records to include time and the fault in one same record.
        - Added a menu item to display the historical faults.
        - OBM related changes are excludable with OBM compile option.

        Revision 1.17 Thu Dec 6 23:21:32 2007
        Added a new message showing why the APU was shutdown, even if the reason doesn't exist anymore.

        Revision 1.16 Thu Dec 6 21:40:09 2007
        - Changed the message that delays the APU shutdown to also show the coolddown period.
        - APU related messages are cleared when the APU is shutdown. (including critical shutdown)
        - Eliminated the "Heater Status" from DPF menu.

        Revision 1.15 Fri Nov 30 18:52:07 2007
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

        Revision 1.14 Thu Nov 1 19:41:33 2007
        -Changed the frequency of recording the CCU Status to external memory to every five minutes.
        -Fixed the bug that was causing the DM1 messages to be cleared after the clearing time was up.
        -Minor fixes in message display location on screen.

        Revision 1.13 Fri Oct 26 18:42:14 2007
        Added 5 minute cooldown for APU after DPF regeneration.
        Changed DPF variable menu to show all variables in one screen.
        Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
        Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

        Revision 1.12 Wed Oct 24 22:23:03 2007
        Removed unused flag.

        Revision 1.11 Wed Oct 24 15:05:52 2007
        Cancelled SPN 520192. Now SPN 520197 will be used for all faults related to exhaust pressure.

        Revision 1.10 Tue Oct 23 19:00:23 2007
        Added comments.

        Revision 1.9 Fri Oct 19 21:12:42 2007
        New feature: Added menu item to allow the user set a flag to ignore extreme pressure fault for one round of APU run and regeneration. After this one regeneration APU Shutdown and all other actions will be back to normal.

        Revision 1.8 Thu Oct 18 20:18:40 2007
        Change of DPF Status message. Op Mode is taken out and merged into Regen Status. Extreme pressure indicator is a bit of its own. Generator voltage has now a bit in the message.

        Revision 1.7 Tue Oct 16 20:40:22 2007
        Version 35-DCP-004. External memory error correction.

        Revision 1.6 Mon Oct 15 16:22:31 2007
        Excluding onboard memory module to make room.

        Revision 1.5 Tue Oct 9 15:03:18 2007
        Version 35-PPP-002. Second round of testing..

        Revision 1.4 Fri Sep 21 20:33:19 2007
        -Fixed disconnection time when a DPF is configured.
        -Removed code to free space.

        Revision 1.3 Mon Sep 17 18:41:35 2007
        DPF related modifications

        Revision 1.2 Tue Aug 28 16:43:18 2007
        Corrected bit access in DPF status message.

        Revision 1.1 Mon Aug 27 22:26:33 2007
        *** empty log message ***

       \endif
*/
/******************************************************************************/
#include "Events.h"
#include "DPF.h"
#include "can.h"
#include "apu.h"
#include "main.h"
#include "ice_pnl.h"            // Interface to heating and cooling control
#include "ViewMan.h"
#include "nvmem.h"
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "rtclock.h"
#include "Datalog.h"
#include "jmain.h"
#include "timers.h"

// DPF Variables
DPF_INFO DPF_Info;
DPF_DIAG DPF_Diag;
DPF_TIMERS DPF_Timers;
unsigned char DPF_State;
uint16 DM1_u16NewFlags;
uint16 DM1_u16PastFlags;

// Function prototypes
static void Service_Regenning(void);
static void Service_Regen_Done(void);
static void Start_Regen(unsigned char);
static void Process_Raw_Exhst_Press(void);
static void Process_Raw_Exhst_Temp(void);
#ifdef EXT_MEM
static void Record_DPF_Status(unsigned char device);
#endif // EXT_MEM
static void DPF_fnRestoreLoads( void );

uint8 DPF_u8DpfAccess;


// Functions

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Initialize_DPF_Variables -->
       \brief This routine is called when:
                    -Reset
                    -When APU is turned on or off
                    -When DPF is disconnected

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Initialize_DPF_Variables (void)
{
    DPF_Timers.Wait_For_ACK = 0;
    DPF_Timers.MSG_Display = 0;
    DPF_Timers.One_Hour = 60;
    DPF_Timers.APU_Cooldown_Timer = 0;

    DPF_Info.EXHST_Press = NO_DATA;
    DPF_Info.EXHST_Press_To_View = NO_DATA;
    DPF_Info.EXHST_Press_To_Average = 0;
    DPF_Info.EXHST_Temp = NO_DATA;
    DPF_Info.EXHST_Temp_To_View = NO_DATA_WORD;
    DPF_Info.EXHST_Temp_To_Average = 0;
    DPF_Info.Number_Of_Samples = 0;
    DPF_Info.Required = DPF_REGEN_REQUIRED_OK;
    DPF_Info.Gen_Voltage = VOLTAGE_LOW;
    DPF_Info.Frozen_Filter = 0;
    DPF_Info.Faults = NO_FAULT;
    DPF_Info.Clean_Filter= CLEAN;

    DCP_Configuration = nvfGetNumber(NV_ID_DCP_CONFIG, DCP_Configuration);	
    DPF_Diag.bInstalled = (DCP_Configuration & DPF_INSTALLED);
    DPF_Diag.bPresent = FALSE;
    if ( DPF_Diag.bInstalled )
        DPF_Timers.Disconnection = DISCONNECT_DETECTION_TIME;
    else 
        DPF_Timers.Disconnection = 0;

    DPF_Diag.Record = 0;
    DPF_Diag.Last_Regen_Command = END_REGEN;
    DPF_Diag.Number_No_ACK_Commands = 0;
    DPF_Diag.bDeferRegen = FALSE;
    DPF_Diag.bRegenCmdSent = FALSE;
    DPF_Diag.bNeedToRegen = FALSE;
    DPF_Diag.bNeedToCool = FALSE;

    DPF_State = DPF_eST_STANDBY;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Reset_DPF_Flags -->
       \brief This routine is called when there is no need for a complete 
              initialization of all variables but just some of the flags.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Reset_DPF_Flags(void)
{
    DPF_Diag.Emergency_Regen_Flag = FALSE;
    DPF_Diag.Last_Regen_Command = END_REGEN;
    DPF_Diag.Number_No_ACK_Commands = 0;
    DPF_Diag.bRegenCmdSent = FALSE;
    DPF_Diag.bNeedToRegen = FALSE;
    DPF_Diag.bNeedToCool = FALSE;
    
    DPF_Timers.APU_Cooldown_Timer = 0;

    if (DPF_Timers.MSG_Display == 0)
        DPF_State = DPF_eST_STANDBY;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Update_DPF_Status -->
       \brief This routine is called when a new J1939 DPF status message is received
              It compares each piece of information contained in this message and 
              if there is a change it generates an event related to the parameter.

              This routine also records pressure and temperature values into the datalog
              memory if present. To do this, it gathers 16 samples of these readings, 
              averages them and then logs them to the external datalog memory.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Update_DPF_Status (j1939_RxMsg_Struct *message)
{
    unsigned char temp_byte;
    unsigned char required_bits;
    unsigned char fault_bits;
    unsigned char frozen_bit;
    unsigned char gen_v_bit;
    unsigned char clean_bit;
    unsigned char exhst_p;
    unsigned char exhst_t;
    unsigned char progress_bits;
    unsigned char change = FALSE;

    Add_Event(EV_DPF_STAT_RCVD);

    // Extract the DPF values of interest
    // DPF Status byte
    temp_byte = message->Data[ 0 ];
    required_bits = temp_byte & DPF_REGEN_REQUIRED_BITS;
    fault_bits = ( temp_byte & FAULT_BITS ) >> 2;
    frozen_bit = ( ( temp_byte & FILTER_FROZEN ) != 0 );
    gen_v_bit = ( ( temp_byte & GEN_VOLTAGE_BIT ) != 0 );
    clean_bit = ( ( temp_byte & CLEAN_FLTR_BIT ) != 0 );

    // DPF Pressure and Temperature
    exhst_p = message->Data[ 1 ];
    exhst_t = message->Data[ 2 ];
    
    // DPF Regenneration progress
    progress_bits = message->Data[ 3 ] >> 6;

    // Handle changed values
    if ( DPF_Info.EXHST_Press != exhst_p )
    {
        DPF_Info.EXHST_Press = exhst_p;
        Process_Raw_Exhst_Press();
    }

    if ( DPF_Info.EXHST_Temp != exhst_t )
    {
        DPF_Info.EXHST_Temp = exhst_t;
        Process_Raw_Exhst_Temp();
    }

    // Regen Required bits
    if ( DPF_Info.Required != required_bits )
    {
        change = TRUE;
        DPF_Info.Required = required_bits;

        switch ( DPF_Info.Required )
        {
        case DPF_REGEN_REQUIRED_SOON:
            Add_Event( EV_REGEN_SOON );
            break;
        case DPF_REGEN_REQUIRED_NOW:
            Add_Event( EV_REGEN_NOW );
            break;
        case DPF_REGEN_REQUIRED_UNKNOWN:    // fall through
        case DPF_REGEN_REQUIRED_OK:
            Add_Event( EV_REGEN_IDLE );
            break;
        }
    }
    
    // Regen progress bits
    if ( DPF_Info.Progress != progress_bits )
    {
        change = TRUE;
        DPF_Info.Progress = progress_bits;
        
        switch ( DPF_Info.Progress )
        {
        case DPF_REGEN_IS_DONE_OK:
        case DPF_REGEN_IS_DONE_NOT_OK :
            DPF_Info.Regenning = FALSE;
            Add_Event( EV_REGEN_DONE );
            break;
        case DPF_REGEN_IS_ACTIVE_NORMAL:
        case DPF_REGEN_IS_ACTIVE_EMERG:
            DPF_Info.Regenning = TRUE;
            Add_Event( EV_REGENNING );
            break;
        }
    }

    if ( DPF_Info.Faults != fault_bits )
    {
        change = TRUE;
        DPF_Info.Faults = fault_bits ;
    }

    if ( DPF_Info.Frozen_Filter != frozen_bit )
    {
        change = TRUE;
        DPF_Info.Frozen_Filter = frozen_bit;
    }

    if ( DPF_Info.Gen_Voltage != gen_v_bit )
    {
        change = TRUE;
        DPF_Info.Gen_Voltage = gen_v_bit;
    }
    
    if ( DPF_Info.Clean_Filter != clean_bit )
    {
        change = TRUE;
        DPF_Info.Clean_Filter = clean_bit;
        if ((DPF_Info.Clean_Filter == CLEAN)
            && (DPF_Diag.Record & MUST_CLEAN_FILTER))
        {
            Add_Event (EV_FLTR_CLEANED);
        }

        if ((DPF_Info.Clean_Filter == DIRTY)
            && ((DPF_Diag.Record & MUST_CLEAN_FILTER) == 0))
        {
            Add_Event (EV_CLEAN_FLTR);
        }
    }
#ifdef EXT_MEM
    if (Datalog.Logging_On)
    {
        if (DPF_Info.Number_Of_Samples < 16)
        {
            DPF_Info.EXHST_Press_To_Average += DPF_Info.EXHST_Press;
            DPF_Info.EXHST_Temp_To_Average += DPF_Info.EXHST_Temp;
            DPF_Info.Number_Of_Samples++;
            if (DPF_Info.Number_Of_Samples == 16)
            {
                // Average and add to long log
                DPF_Info.EXHST_Press_To_Average >>= 4;
                DPF_Info.EXHST_Temp_To_Average >>= 4;
                Add_To_Long_Record(DPF_Info.EXHST_Press_To_Average, DPF_Info.EXHST_Temp_To_Average);
                DPF_Info.Number_Of_Samples = 0;
                DPF_Info.EXHST_Press_To_Average = 0;
                DPF_Info.EXHST_Temp_To_Average = 0;
            }
        }
        else // We should never get here
        {
            DPF_Info.Number_Of_Samples = 0;
            DPF_Info.EXHST_Press_To_Average = 0;
            DPF_Info.EXHST_Temp_To_Average = 0;
        }
    }

    if ( change )
    {
        Record_DPF_Status(EXTERNAL);
    }
#endif // EXT_MEM
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_State_Machine -->
       \brief This routine is called from main loop if there is an event to attend to.
              It has two switch statmenet:
                - First one is based on the events that are not state specific
                - Second one is based on the events that are state specific

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DPF_State_Machine (unsigned char event)
{
    switch (event)
    {
        // Events that are applicable in all states
        case EV_DPF_STAT_RCVD:
            // DPF_Diag.bInstalled is initialized at startup based on 
            // installation Configuration (DCP_Configuration & DPF_INSTALLED)
            if ( !DPF_Diag.bInstalled )
            {
                // If DPF is not installed but being detected on CAN/J1939
                Install_DPF();
                VWM_fnRequestView (VWM_eVIEW_DPF_DETECTED, BACKLIGHT_ON);
            }
            // Clear DPF lost flag and its flag to shutdown the engine
            if (DPF_Diag.Record & MISSING)
            {
                Record_Fault (SPN_DPF_DISCONNECTED, FMI_DPF_DISCONNECTED, CLEAR);
                Initialize_DPF_Variables();
            }
            DPF_Diag.bPresent = TRUE;
            APU_Critical_Shutdown_Can_Clear();
            // Reset the timer again to detect disconnection
            DPF_Timers.Disconnection = DISCONNECT_DETECTION_TIME;
            // Clear alert screen (it may not have existed but it won't hurt to clear)
            VWM_fnRemoveView(VWM_eVIEW_DPF_DISCONNECT_ALERT);
        break;

        case EV_DPF_DISCONNECTED:
            DPF_Diag.bPresent = FALSE;
            // A DPF disconnection fault will be issued if DPF is installed 
            if ( DPF_Diag.bInstalled )
            {
                DPF_Diag.Record |= MISSING;
                Record_Fault( SPN_DPF_DISCONNECTED, FMI_DPF_DISCONNECTED, SET );
                
                // Only make alert and shutdown if APU is running
                if ( APU_ON_FLAG )
                {
                    APU_Critical_ShutDown();
                    VWM_fnRemoveView( VWM_eVIEW_DPF_DETECTED );
                    VWM_fnRequestView( VWM_eVIEW_DPF_DISCONNECT_ALERT, BACKLIGHT_ON );
                }
            }
        break;

        case EV_ACK_ERROR:
            APU_Critical_ShutDown();
            VWM_fnRequestView (VWM_eVIEW_DPF_ACK_ERROR_ENGINE_SHUT_DOWN, BACKLIGHT_ON);
            Record_Fault (SPN_DPF_DISCONNECTED, FMI_DPF_ABNORMAL_FREQ, SET);
        break;

        case EV_REGEN_SOON:
            DPF_State = DPF_eST_REGEN_SOON;
            DPF_Diag.bNeedToRegen = TRUE;
			APU_fnLoadControl( TRUE );					// added to stop alternator, forcing apu in Run3 mode to ignore interlock
        break;

        case EV_REGEN_NOW:
        case EV_USER_REGEN:
            DPF_State = DPF_eST_REGEN_NOW;
            DPF_Diag.bNeedToRegen = TRUE;
            if ( ! APU_Diag.APU_Warmed_Up )
            {
                DPF_Diag.bDeferRegen = TRUE;
            }
            else
            {
                if ( DPF_Diag.Emergency_Regen_Flag )
                    Start_Regen( EMERGENCY_REGEN );
                else
                    Start_Regen( START_REGEN_NORMAL );
            }
        break;

        case EV_APU_WARMUP:
            if ( DPF_Diag.bDeferRegen )
            {
                Add_Event( EV_REGEN_NOW );
                DPF_Diag.bDeferRegen = FALSE;
            }
            APU_Diag.APU_Warmed_Up = TRUE;
        break;

        case EV_REGENNING:
            Service_Regenning();
        break;

        case EV_REGEN_DONE:
            if (DPF_State == DPF_eST_REGENNING)
                Service_Regen_Done();
            else
                Abort_Regen();
        break;

        case EV_APU_NO_REGEN_FOR_TOO_LONG:
            VWM_fnRequestView (VWM_eVIEW_DPF_APU_NO_REGEN_FOR_TOO_LONG, BACKLIGHT_ON );
        break;

        case EV_UPDATE_APU_HOUR_WITHOUT_REGEN:
            (void) nvfPutNumber (NV_ID_APU_NO_REGEN_HOUR, DPF_Diag.APU_Hours_Without_Regen);
            
        break;

        case EV_CLEAN_FLTR:
            DPF_Diag.Record |= MUST_CLEAN_FILTER;
            VWM_fnRequestView (VWM_eVIEW_DPF_CLEANING_REQUIRED, BACKLIGHT_ON);
        break;

        case EV_FLTR_CLEANED:
            DPF_Diag.Record &= ~MUST_CLEAN_FILTER;
            VWM_fnRemoveView(VWM_eVIEW_DPF_CLEANING_REQUIRED);
        break;

        case EV_DPF_ONE_HR:
            // This is the time to remind the user of the following issues
            // They may appear sooner than one hour after they have first 
            // been detected. The reason is that the one hour timer is started 
            // at reset and not at the time of each fault detection
            // These events are ordered by their importance and priority
            if (APU_ON_FLAG)
            {
                if (DPF_Diag.Record & MISSING)
                {
                    VWM_fnRequestView (VWM_eVIEW_DPF_DISCONNECT_ALERT, BACKLIGHT_ON);
                }
                if (DPF_Diag.Record & MUST_CLEAN_FILTER)
                {
                    VWM_fnRequestView (VWM_eVIEW_DPF_CLEANING_REQUIRED, BACKLIGHT_ON);
                }
                if (DPF_Diag.Record & ( EXHST_PRESS_SNSR_FAIL | EXHST_PRESS_SNUB_PLUG ) )
                {
                    VWM_fnRequestView (VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL, BACKLIGHT_ON);
                }
                if (DPF_Diag.Record & EXHST_TEMP_SNSR_FAIL)
                {
                    VWM_fnRequestView (VWM_eVIEW_DPF_EXHST_TEMP_SNSR_FAIL, BACKLIGHT_ON);
                }
#ifdef EXT_MEM
                if (DCP_Diag.EXT_Memory_Fault)
                {
                    VWM_fnRequestView (VWM_eVIEW_DCP_EXT_MEMORY_FAULT, BACKLIGHT_ON);
                }
#endif // EXT_MEM
            }

            DPF_Timers.One_Hour = 60;	// To remind again in one hour
        break;

        case EV_DPF_8_HR:
            Add_Event( EV_REGEN_NOW );
            DPF_Timers.Eight_Hour = 8;
        break;

// DM1 messages:

        case EV_DPF_DM1_CLEARED:
            // Clear only the faults that were generated by DM1 message
            DPF_Diag.Record &= ~(EXHST_TEMP_SNSR_FAIL + EXHST_TEMP_HIGH + 
                                 HEATER_FAIL + HEATER_TIMEOUT + 
                                 EXHST_PRESS_HIGH + EXHST_PRESS_SNSR_FAIL +
                                 EXHST_PRESS_SNUB_PLUG +
                                 GEN_LOW_VOLTAGE + GEN_FREQ_FAULT + TWO_FAILURES);
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_EXTREME_PRESS:
        // If there is a frozen filter condition, we want to perform an emergency regen
        // to warm up the system. We also want the extreme pressure to be ignored during
        // this regen to avoid an engine shutdown.
            if ((DPF_Info.Frozen_Filter) && 
                (APU_ON_FLAG) && 
                (DPF_Diag.Emergency_Regen_Flag == FALSE))
            {
                Set_Emergency_Regen_Flag();
                Add_Event (EV_USER_REGEN);
            }
            if (!(DPF_Diag.Record & EXHST_PRESS_HIGH))
            {
                if (DPF_Diag.Emergency_Regen_Flag == FALSE)
                {
                    // NOTE: In case of an emergency regen this fault will not be recorded
                    // in datalog and DPF_Diag.Record. The reason is that if the 
                    // flag at DPF_Diag is set, this part of the code will not be executed
                    // and the appropriate actions that need to be taken in presence of this
                    // fault will be misssed.
                    DPF_Diag.Record |= EXHST_PRESS_HIGH;
                    DPF_fnTurnOffLoads();
                    APU_Critical_ShutDown();
                    VWM_fnRequestView (VWM_eVIEW_DPF_EXTREME_PRESS_ENGINE_SHUT_DOWN, BACKLIGHT_ON);
                    Record_Fault(SPN_EXHST_PRESS, FMI_EXTREME_PRESS, SET);
                }
            }
        break;

        case EV_EXTREME_PRESS_CLR:
            DPF_Diag.Record &= ~EXHST_PRESS_HIGH;
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_EXHST_PRESS_SNSR_FAIL:
            // If pressure sensor failure is new
            if ( ! ( DPF_Diag.Record & EXHST_PRESS_SNSR_FAIL ) )
            {
                // If not in emergency regen
                if ( DPF_Diag.Emergency_Regen_Flag == FALSE )
                {
                    // Add fault to record and alert user
                    DPF_Diag.Record |= EXHST_PRESS_SNSR_FAIL;
                    VWM_fnRequestView( VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL, BACKLIGHT_ON );
                    Record_Fault( SPN_EXHST_PRESS, FMI_PRESS_SNSR_FAIL, SET );
                    
                    // If we are not in eight hour timer mode yet
                    // or pressure sensor regen did not complete
                    if ( ( DPF_Timers.Eight_Hour == 0 ) 
                      || ( DPF_Diag.Press_Snsr_Wants_Regen ) )
                    {
                        // Start a regen now
                        Add_Event( EV_REGEN_NOW );
                        // Flag the need for a complete regen after pressure sensor failed
                        DPF_Diag.Press_Snsr_Wants_Regen = TRUE;
                        DPF_Diag.Consecutive_Good_Regens = 0;
                        // Add fault flag to indicate service required
                        DPF_Diag.Record |= EXHST_PRESS_SNSR_FAIL_ONCE;
                        // And start eight hour timer
                        DPF_Timers.Eight_Hour = 8;
                    }
                }

                // If there is also a temperature sensor failure
                if ( DPF_Diag.Record & EXHST_TEMP_SNSR_FAIL )
                {
                    DPF_Diag.Record |= TWO_FAILURES;
                    Service_Two_Failures();
                }
            }
        break;

        case EV_EXHST_PRESS_SNUB_PLUG:
            // If pressure sensor failure is new
            if ( ! ( DPF_Diag.Record & EXHST_PRESS_SNUB_PLUG ) )
            {
                // If not in emergency regen
                if ( DPF_Diag.Emergency_Regen_Flag == FALSE )
                {
                    // Add fault to record and alert user
                    DPF_Diag.Record |= EXHST_PRESS_SNUB_PLUG;
                    VWM_fnRequestView( VWM_eVIEW_DPF_EXHST_PRESS_SNSR_FAIL, BACKLIGHT_ON );
                    Record_Fault( SPN_EXHST_PRESS, FMI_PRESS_SNUBBER_PLUG, SET );
                    
                    // If we are not in eight hour timer mode yet
                    // or pressure sensor regen did not complete
                    if ( ( DPF_Timers.Eight_Hour == 0 ) 
                      || ( DPF_Diag.Press_Snsr_Wants_Regen ) )
                    {
                        // Start a regen now
                        Add_Event( EV_REGEN_NOW );
                        // Flag the need for a complete regen after pressure sensor failed
                        DPF_Diag.Press_Snsr_Wants_Regen = TRUE;
                        DPF_Diag.Consecutive_Good_Regens = 0;
                        // Add fault flag to indicate service required
                        DPF_Diag.Record |= EXHST_PRESS_SNSR_FAIL_ONCE;
                        // And start eight hour timer
                        DPF_Timers.Eight_Hour = 8;
                    }
                }

                // If there is also a temperature sensor failure
                if ( DPF_Diag.Record & EXHST_TEMP_SNSR_FAIL )
                {
                    DPF_Diag.Record |= TWO_FAILURES;
                    Service_Two_Failures();
                }
            }
        break;

        case EV_EXHST_PRESS_SNSR_CLR:
            // Clear exhaust pressure sensor fail flag
            DPF_Diag.Record &= ~EXHST_PRESS_SNSR_FAIL;
            // If snubber plugged fault is also clear
            if ( ( DPF_Diag.Record & EXHST_PRESS_SNUB_PLUG ) == 0 )
            {
                // Clear the Two Failures condition
                DPF_Diag.Record &= ~TWO_FAILURES;
                APU_Critical_Shutdown_Can_Clear();
            }
        break;

        case EV_EXHST_PRESS_SNUB_PLUG_CLR:
            // Clear snubber plugged fault
            DPF_Diag.Record &= ~EXHST_PRESS_SNUB_PLUG;
            // If exhaust pressure sensor fail is also clear
            if ( ( DPF_Diag.Record & EXHST_PRESS_SNSR_FAIL ) == 0 )
            {
                // Clear the Two Failures condition
                DPF_Diag.Record &= ~TWO_FAILURES;
                APU_Critical_Shutdown_Can_Clear();
            }
        break;

        case EV_EXHST_PRESS_HIGH_POST_REGEN:
            if (!(DPF_Diag.Record & EXHST_PRESS_HIGH_POST_REGEN))
            {
                if (DPF_Diag.Emergency_Regen_Flag == FALSE)
                {
                    DPF_Diag.Record |= EXHST_PRESS_HIGH_POST_REGEN;
                    VWM_fnRequestView( VWM_eVIEW_DPF_EXHST_PRESS_HIGH_POST_REGEN, BACKLIGHT_ON );
                    Record_Fault( SPN_EXHST_PRESS, FMI_PRESS_HIGH_POST_REGEN, SET );
                    APU_Diag.Noncritical_Shutdown = TRUE;
                }
            }
        break;

        case EV_EXHST_PRESS_HIGH_POST_REGEN_CLR:
            DPF_Diag.Record &= ~EXHST_PRESS_HIGH_POST_REGEN;
        break;
        
        case EV_EXHST_TEMP_SNSR_FAIL:
            if (!(DPF_Diag.Record & EXHST_TEMP_SNSR_FAIL))
            {
                DPF_Diag.Record |= EXHST_TEMP_SNSR_FAIL;
                VWM_fnRequestView (VWM_eVIEW_DPF_EXHST_TEMP_SNSR_FAIL, BACKLIGHT_ON);
                Record_Fault(SPN_EXHST_TEMP, FMI_EXHST_TEMP_SNSR_FAIL, SET);
                // If there is also a pressure sensor failure
                if (DPF_Diag.Record & EXHST_PRESS_SNSR_FAIL)
                {
                    DPF_Diag.Record |= TWO_FAILURES;
                    Service_Two_Failures();
                }
            }
        break;

        case EV_EXHST_TEMP_SNSR_CLR:
            DPF_Diag.Record &= ~(EXHST_TEMP_SNSR_FAIL + TWO_FAILURES);
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_EXHST_TEMP_HIGH:
            if (!(DPF_Diag.Record & EXHST_TEMP_HIGH))
            {
                if (DPF_Diag.Emergency_Regen_Flag == FALSE)
                {
                    DPF_Diag.Record |= EXHST_TEMP_HIGH;
                    APU_Critical_ShutDown();
                    VWM_fnRequestView (VWM_eVIEW_DPF_EXHST_TEMP_HIGH, BACKLIGHT_ON);
                    Record_Fault(SPN_EXHST_TEMP, FMI_EXHST_TEMP_HIGH, SET);
                }
            }
        break;

        case EV_EXHST_TEMP_HIGH_CLR:
            DPF_Diag.Record &= ~EXHST_TEMP_HIGH;
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_HEATER_TIMEOUT:
            if (!(DPF_Diag.Record & HEATER_TIMEOUT))
            {
                if (DPF_Diag.Emergency_Regen_Flag == FALSE)
                {
                    DPF_Diag.Record |= HEATER_TIMEOUT;
                    APU_Critical_ShutDown();
                    VWM_fnRequestView (VWM_eVIEW_DPF_HEATER_FAIL, BACKLIGHT_ON);
                    Record_Fault(SPN_HEATER_FAIL, FMI_HEATER_TIMEOUT, SET);
                }
            }
        break;

        case EV_HEATER_TIMEOUT_CLR:
            DPF_Diag.Record &= ~HEATER_TIMEOUT;
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_HEATER_FAIL:
            if (!(DPF_Diag.Record & HEATER_FAIL))
            {
                if (DPF_Diag.Emergency_Regen_Flag == FALSE)
                {
                    DPF_Diag.Record |= HEATER_FAIL;
                    APU_Critical_ShutDown();
                    VWM_fnRequestView (VWM_eVIEW_DPF_HEATER_FAIL, BACKLIGHT_ON);
                    Record_Fault(SPN_HEATER_FAIL, FMI_HEATER_FAIL, SET);
                }
            }
        break;

        case EV_HEATER_FAIL_CLR:
            DPF_Diag.Record &= ~HEATER_FAIL;
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_LOW_VOLTAGE:
            if (!(DPF_Diag.Record & GEN_LOW_VOLTAGE))
            {
                DPF_Diag.Record |= GEN_LOW_VOLTAGE;
                APU_Critical_ShutDown();
                if ( APU_ON_FLAG )	//if APU is already OFF then do not run. Prevents faulty DPF messages.
                    { 
                    VWM_fnRequestView (VWM_eVIEW_DPF_LOW_VOLTAGE_ENGINE_SHUT_DOWN, BACKLIGHT_ON);
                    Record_Fault(SPN_GEN_VOLTAGE, FMI_LOW_VOLTAGE, SET);
                    }
            }
        break;

        case EV_LOW_VOLTAGE_CLR:
            DPF_Diag.Record &= ~GEN_LOW_VOLTAGE;
            APU_Critical_Shutdown_Can_Clear();
        break;

        case EV_FREQ_FAULT:
            if ( ! ( DPF_Diag.Record & GEN_FREQ_FAULT ) )
            {
                DPF_Diag.Record |= GEN_FREQ_FAULT;
                APU_Critical_ShutDown();
                if ( APU_ON_FLAG )	//if APU is already OFF then do not run. Prevents faulty DPF messages.
                    { 
                    VWM_fnRequestView( VWM_eVIEW_DPF_FREQ_FAULT_ENGINE_SHUT_DOWN, BACKLIGHT_ON );
                    Record_Fault( SPN_GEN_FREQ, FMI_FREQ_FAULT, SET );
                    }
            }
        break;

        case EV_FREQ_FAULT_CLR:
            DPF_Diag.Record &= ~GEN_FREQ_FAULT;
        break;

// End of DM1 messages
#ifdef EXT_MEM
        case EXT_MEMORY_FAULT:
            DCP_Diag.EXT_Memory_Fault = TRUE;
            VWM_fnRequestView (VWM_eVIEW_DCP_EXT_MEMORY_FAULT, BACKLIGHT_ON);
        break;
#endif // EXT_MEM

        case EV_APU_COOL_NOW:
            DPF_Diag.bNeedToCool = FALSE;
            // Do not restore loads if APU is going off now
            if ( APU_Diag.APU_Off_Requested )
            {
                APU_Diag.APU_Off_Now = TRUE;
            }
            else
            {
                // Restore full loads
                DPF_fnRestoreLoads();
            }
        break;

        default:
        break;
    }
    switch (DPF_State)
    {
        // Events that are applicable in particular states
        case DPF_eST_REGEN_SOON:
            switch (event)
            {
                case EV_APU_GOING_OFF:
                    Add_Event( EV_REGEN_NOW );
                break;
            }
        break;

        case DPF_eST_REGEN_NOW:
            switch (event)
            {
                case EV_ACK_RCVD:
                    DPF_Timers.Wait_For_ACK = 0;
                    DPF_Diag.Number_No_ACK_Commands = 0;
                break;

                case EV_DPF_NO_ACK:
                    if ( DPF_Diag.Number_No_ACK_Commands < MAX_NUMBER_NO_ACK_COMMAND )
                    {
                        Start_Regen( DPF_Diag.Last_Regen_Command );
                        DPF_Diag.Number_No_ACK_Commands++;
                    }
                    else
                    {
                        Add_Event( EV_ACK_ERROR );
                    }
                break;

                case EV_REGENNING:
                    Service_Regenning();
                break;

                case EV_APU_GOING_OFF:
                    Add_Event( EV_REGEN_NOW );
                break;

                default:
                break;
            }
        break;

        case DPF_eST_REGEN_DONE:
            switch (event)
            {
                case EV_DPF_MSG_DISPLAY:
                    Clear_DPF_Screen();
                    DPF_State = DPF_eST_STANDBY;
                break;
            
                default:
                break;
            }
        break;

        default:
        break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Service_Regenning -->
       \brief This routine is called when there is a transition to regen in progress 
              read from J1939 DPF status message.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Service_Regenning(void)
{
    // This must happen only when APU is on
    if (APU_ON_FLAG)
    {
        DPF_State = DPF_eST_REGENNING;
        DPF_Diag.bNeedToCool = TRUE;
        DPF_Timers.APU_Cooldown_Timer = 0;
        Clear_DPF_Screen();
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Service_Regen_Done -->
       \brief This routine is called when there is a transition to regen OK/Done 
              read from J1939 DPF status message and if a regen was in progress.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Service_Regen_Done(void)
{
    DPF_State = DPF_eST_REGEN_DONE;
    
    // If the regen was successful
    if ( DPF_Info.Progress == DPF_REGEN_IS_DONE_OK )
    {
        // Count good regen
        if ( DPF_Diag.Consecutive_Good_Regens < 255 )
        {
            DPF_Diag.Consecutive_Good_Regens++;
        }

        // Flag that the pressure sensor regen is done
        DPF_Diag.Press_Snsr_Wants_Regen = FALSE;
        
        // If there were 2 consecutive good regens
        // and the exhaust pressure sensor is okay
        if ( ( DPF_Diag.Consecutive_Good_Regens >= 2 )
          && ( ( DPF_Diag.Record & ( EXHST_PRESS_SNSR_FAIL | EXHST_PRESS_SNUB_PLUG ) ) == 0 ) )
        {
            // Clear the exhaust sensor pressure fail once flag
            DPF_Diag.Record &= ~EXHST_PRESS_SNSR_FAIL_ONCE;
            // And clear the eight hour timer
            DPF_Timers.Eight_Hour = 0;
        }
    }
    else
    {
        // Regen did not complete properly
        // Reset count of consecutive good regens
        DPF_Diag.Consecutive_Good_Regens = 0;
    }
    
    DPF_Timers.MSG_Display = DPF_MSG_DISPLAY_TIME;
    // Restore loads unless the APU is going off
    if ( ! APU_Diag.APU_Off_Requested )
    {
        // Restore HVAC loads
        ICE_fnLoadControl( FALSE );
    }
    DPF_Diag.Last_Regen_Command = END_REGEN;
    DPF_Diag.bRegenCmdSent = FALSE;
    DPF_Diag.Emergency_Regen_Flag = FALSE;
    DPF_Diag.bNeedToRegen = FALSE;
    Clear_DPF_Screen();
    // Start APU cooldown time before releasing for shutdown
    DPF_Timers.APU_Cooldown_Timer = DPF_COOL_DOWN_TIME;
    DPF_Diag.APU_Minutes_Without_Regen = 0;
    DPF_Diag.APU_Hours_Without_Regen = 0;
    (void) nvfPutNumber (NV_ID_APU_NO_REGEN_HOUR, DPF_Diag.APU_Hours_Without_Regen);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Abort_Regen -->
       \brief This routine is called when:
                    APU has (or must) shutdown for any reason
                    The current status from DPF is gone back to normal
                    The half hour forced regeneration time is up

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Abort_Regen (void)
{
    unsigned char data[1];
    
    Clear_DPF_Screen();

    if ((DPF_State == DPF_eST_REGENNING) || 
        (DPF_Diag.Last_Regen_Command == START_REGEN) || 
        (DPF_Diag.Last_Regen_Command == EMERGENCY_REGEN)||
        (DPF_Diag.Last_Regen_Command == START_REGEN_NORMAL))
    {
        DPF_State = DPF_eST_REGEN_ABORTED;
        DPF_Timers.MSG_Display = DPF_MSG_DISPLAY_TIME;
        // Start APU cooldown time before releasing for shutdown
        DPF_Timers.APU_Cooldown_Timer = DPF_COOL_DOWN_TIME;
        DPF_Diag.Last_Regen_Command = ABORT_REGEN;
        data[0] = DPF_Diag.Last_Regen_Command;
        // Send command to DPF controller
        Tx_PGN65301(data);
    }
    else if (DPF_Timers.MSG_Display == 0)
    {
        DPF_State = DPF_eST_STANDBY;
    }
    DPF_fnRestoreLoads();
    DPF_Diag.Last_Regen_Command = END_REGEN;
    DPF_Diag.bRegenCmdSent = FALSE;
    DPF_Diag.bNeedToRegen = FALSE;
    DPF_Diag.Emergency_Regen_Flag = FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Start_Regen -->
       \brief This routine is called when:
                    -Five minute countdown is up in REGEN NOW
                    -No ACK is received for previously sent command
                    -User requested regen
                    -APU is going off in REGEN SOON
                    -APU is going off when in five minute countdown
                    -Forced regen when exhaust pressure sensor fails 
                                        and every 8 hours after that

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Start_Regen(unsigned char command)
{
    unsigned char data[1];

    // Do not send regen command if apu off has been requested and we are
    // actually ending a regen (dpf needs to cool)
    if ( ( APU_Diag.APU_Off_Requested ) && ( DPF_Diag.bNeedToCool ) )
        return;

    // Send a regen command only if the DPF is not in a regen and the APU is on
    if ( ( ! DPF_Info.Regenning ) && ( APU_ON_FLAG ) )
    {
        Clear_DPF_Screen();
        DPF_State = DPF_eST_REGEN_NOW;
        DPF_fnTurnOffLoads();
        DPF_Diag.Last_Regen_Command = command;
        data[0] = command;
        // Send command to DPF controller
        Tx_PGN65301(data);
        DPF_Diag.bRegenCmdSent = TRUE;
        DPF_Timers.Wait_For_ACK = COMMAND_ACK_TIME;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_DM1_Tick -->
       \brief This routine is called from the seconds timer.
              Every two seconds the DM1 flags are checked to see if any
              have been dropped from activity.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DPF_DM1_Tick( void )
{
    static uint8 u8Ticks;
    uint16 u16Removed;
    
    u8Ticks++;
    
    // After 2 seconds
    if ( u8Ticks > 1 )
    {
        // Restart timer
        u8Ticks = 0;
        
        // The lack of incoming DM1 may be the result of DPF disconnection,
        // We do not want to clear the faults if this is the case
        if ( DPF_Timers.Disconnection < ( DISCONNECT_DETECTION_TIME - 1 ) )
        {
            // Do not check for DM1 flag removal
            return;
        }
        
        // Determine bits that are set in Past DM1 and not in New DM1
        u16Removed = ( DM1_u16PastFlags ^ DM1_u16NewFlags ) & DM1_u16PastFlags;
        
        // If any DM1 flag is removed
            // Generate Clear Event
        
        if ( u16Removed & DM1_EXHST_TEMP_SNSR_FAIL )
            Add_Event (EV_EXHST_TEMP_SNSR_CLR);
            
        if ( u16Removed & DM1_EXHST_PRESS_EXTREME )
            Add_Event (EV_EXTREME_PRESS_CLR);
            
        if ( u16Removed & DM1_EXHST_PRESS_SNSR_FAIL )
            Add_Event (EV_EXHST_PRESS_SNSR_CLR);
            
        if ( u16Removed & DM1_EXHST_PRESS_SNUB_PLUG )
            Add_Event (EV_EXHST_PRESS_SNUB_PLUG_CLR);
            
        if ( u16Removed & DM1_HEATER_FAIL )
            Add_Event (EV_HEATER_FAIL_CLR);
            
        if ( u16Removed & DM1_GEN_LOW_VOLTAGE )
            Add_Event (EV_LOW_VOLTAGE_CLR);
                    
        if ( u16Removed & DM1_EXHST_TEMP_HIGH )
            Add_Event (EV_EXHST_TEMP_HIGH_CLR);
            
        if ( u16Removed & DM1_EXHST_PRESS_HIGH_POST_REGEN )
            Add_Event (EV_EXHST_PRESS_HIGH_POST_REGEN_CLR);
            
        if ( u16Removed & DM1_HEATER_TIMEOUT )
            Add_Event (EV_HEATER_TIMEOUT_CLR);
            
        if ( u16Removed & DM1_GEN_FREQ_FAULT )
            Add_Event( EV_FREQ_FAULT_CLR );
            
        // Save New DM1 flags to Past DM1 flags
        DM1_u16PastFlags = DM1_u16NewFlags;
        
        // Clear New DM1 flags
        DM1_u16NewFlags = 0;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_DM1_Faults -->
       \brief This routine is called from J1939 message reception regular check.
              The SPN and FMI's are extracted from DM1 messages that may contain
              more than one fault in them. 

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DPF_DM1_Faults(unsigned long spn, unsigned char fmi)
{
    // Do not capture DPF related faults if the APU is not on
    if ( APU_Diag.APU_On_Off_Flag == OFF )
        return;

    switch (spn)
    {
        case SPN_HEATER_FAIL:
            if (fmi == FMI_HEATER_FAIL)
            {
                DM1_u16NewFlags |= DM1_HEATER_FAIL;
                Add_Event(EV_HEATER_FAIL);
            }
            else if (fmi == FMI_HEATER_TIMEOUT)
            {
                DM1_u16NewFlags |= DM1_HEATER_TIMEOUT;
                Add_Event(EV_HEATER_TIMEOUT);
            }
        break;

        case SPN_EXHST_TEMP:
            if (fmi == FMI_EXHST_TEMP_SNSR_FAIL)
            {
                DM1_u16NewFlags |= DM1_EXHST_TEMP_SNSR_FAIL;
                Add_Event(EV_EXHST_TEMP_SNSR_FAIL);
            }
            else if (fmi == FMI_EXHST_TEMP_HIGH)
            {
                DM1_u16NewFlags |= DM1_EXHST_TEMP_HIGH;
                Add_Event(EV_EXHST_TEMP_HIGH);
            }
        break;

        case SPN_EXHST_PRESS:
            if (fmi == FMI_EXTREME_PRESS)
            {
                DM1_u16NewFlags |= DM1_EXHST_PRESS_EXTREME;
                Add_Event(EV_EXTREME_PRESS);
            }
            else if (fmi == FMI_PRESS_SNSR_FAIL)
            {
                DM1_u16NewFlags |= DM1_EXHST_PRESS_SNSR_FAIL;
                Add_Event(EV_EXHST_PRESS_SNSR_FAIL);
            }
            else if (fmi == FMI_PRESS_SNUBBER_PLUG)
            {
                DM1_u16NewFlags |= DM1_EXHST_PRESS_SNUB_PLUG;
                Add_Event(EV_EXHST_PRESS_SNUB_PLUG);
            }
            else if (fmi == FMI_PRESS_HIGH_POST_REGEN )
            {
                DM1_u16NewFlags |= DM1_EXHST_PRESS_HIGH_POST_REGEN;
                Add_Event(EV_EXHST_PRESS_HIGH_POST_REGEN);
            }
/*			
            else if (fmi == FMI_INVALID_DATA);
            else if (fmi == FMI_ABNORMAL_RATE);
            else if (fmi == FMI_CONDITIONS_EXIST);
*/
        break;

        case SPN_GEN_VOLTAGE:
            if (fmi == FMI_LOW_VOLTAGE)
            {
                DM1_u16NewFlags |= DM1_GEN_LOW_VOLTAGE;
                Add_Event(EV_LOW_VOLTAGE);
            }
        break;

        case SPN_GEN_FREQ:
            if ( fmi == FMI_FREQ_FAULT )
            {
                DM1_u16NewFlags |= DM1_GEN_FREQ_FAULT;
                Add_Event( EV_FREQ_FAULT );
            }
        break;

        default:
            
        break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_Acknowledgement_Received -->
       \brief This routine is called from J1939 message reception regular check.
              This must be received when DCP has sent a command to DPF.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DPF_Acknowledgement_Received(j1939_RxMsg_Struct *message)
{
    if (message->Data[0] == POSITIVE_ACK)
    {
        Add_Event (EV_ACK_RCVD);
    }	
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Process_Raw_Exhst_Press -->
       \brief This routine is called when a new exhaust pressure value is 
              received in the DPF status message. This routine converts the
              raw number to a processed number that takes into account the resolution, 
              offset and the unit of display. This value then can be displayed to user.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Process_Raw_Exhst_Press(void)
{
    // The resolution is 1 mbar/count and the unit of display is also mbar
    if (DPF_Info.EXHST_Press <= VALID_CAN_DATA)
    {
        DPF_Info.EXHST_Press_To_View = DPF_Info.EXHST_Press;
    }

    else
        DPF_Info.EXHST_Press_To_View = NO_DATA;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Process_Raw_Exhst_Temp -->
       \brief This routine is called when a new exhaust temperature value is 
              received in the DPF status message. This routine converts the
              raw number to a processed number that takes into account the resolution, 
              offset and the unit of display. This value then can be displayed to user.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Process_Raw_Exhst_Temp(void)
{
    // The resolution is 2 C/count and the unit of display is C
    if (DPF_Info.EXHST_Temp <= VALID_CAN_DATA)
    {
        DPF_Info.EXHST_Temp_To_View = DPF_Info.EXHST_Temp * EXHST_TEMP_RESOLUTION;
        DPF_Info.EXHST_Temp_To_View += EXHST_TEMP_OFFSET;
    }

    else
        DPF_Info.EXHST_Temp_To_View = NO_DATA_WORD;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Record_Fault -->
       \brief This routine is called when a DPF related fault is detected. The 
              fault has to be recorded in fault memory (onboard) and log memory
              (external) devices. The flag shows which one of these devices is 
              the intended destination.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Record_Fault (unsigned long spn, unsigned char fmi, unsigned char flag)
{
unsigned char Data[8];
unsigned char hours;
#ifdef OBM
    // Prepare data to be recorded at internal memory
    Data[4] = (unsigned char)(spn & 0x000000FF);
    Data[5] = (unsigned char) ((spn >> 8) & 0x000000FF);
    Data[6] = (unsigned char) ((spn >> 16) & 0x00000007);
	Data[6] += (fmi << 3);
	
	hours = timers.APU_ONTmr/60;
	if(hours == 0)
		runTimeSinceLastStart = (unsigned char)(timers.APU_ONTmr & 0x00FF);		 // if run time since last start < 60 min
	else	// if run time since start >= 60 min: set bit 6 = "1", store hours int bits 5-2, and quarters into bits 1-0
		runTimeSinceLastStart = 0x40 | (hours << 2) | ((timers.APU_ONTmr % 60) / 15);
	Data[7] = runTimeSinceLastStart;

    if (flag == SET)
        Data[7] |= 0x80;        // set the bit to show that the fault has been set
        
    Data[0] = (rtgClock.rtxMinutes << 6);
    Data[0] += rtgClock.rtxSeconds;

    Data[1] = (rtgClock.rtxMinutes >> 2);
    Data[1] += (rtgClock.rtxHours << 4);
    Data[2] = (rtgClock.rtxHours >> 4);
    Data[2] += (rtgClock.rtxDate << 1);	
	Data[2] += (rtgClock.rtxMonth  << 6);
    Data[3] = (rtgClock.rtxMonth >> 2);
    Data[3] += (rtgClock.rtxYear  << 2);

    OBM_fnAddRecord(Data,1);
#endif // OBM

#ifdef EXT_MEM
    Data[0] = FAULT_RECORD;
    Data[1] = (unsigned char)(spn & 0x000000FF);
    Data[2] = (unsigned char) ((spn >> 8) & 0x000000FF);
    Data[3] = (unsigned char) ((spn >> 16) & 0x000000FF);
    if (flag == SET)
        Data[3] |= 0x80;        // set the bit to show that the fault has been set
    Data[4] = fmi;

    // Write to external memory
    if ( Datalog.Ext_Mem_Present )
    {
        Record_Real_Time_And_Date(EXTERNAL);
        if ( EXM_fnLogData( Data, 5 ) )
            Increment_Sync_Insertion_Counter();
    }
#endif // EXT_MEM
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Record_DPF_Status -->
       \brief This routine is called when a parameter in DPF status message is 
              changed. The device parameter shows which memory device is 
              the intended destination.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
#ifdef EXT_MEM
static void Record_DPF_Status(unsigned char device)
{
unsigned char status;
unsigned char Data[8];

    // Prepare data to be recorded
    Data[0] = DPF_STATUS_RECORD;
    
    // Older regen status definitions
    if ( DPF_Info.Regenning )
        status = 0x03;      // Regen in progress
    else if ( DPF_Info.Required == DPF_REGEN_REQUIRED_NOW )
        status = 0x02;      // Regen now
    else if ( DPF_Info.Required == DPF_REGEN_REQUIRED_SOON )
        status = 0x01;      // Regen soon
    else
        status = 0x00;      // Regen complete
        
    status += (DPF_Info.Faults << 2);
    
    if ( DPF_Info.Gen_Voltage )
        status += GEN_VOLTAGE_BIT;
    if ( DPF_Info.Clean_Filter )
        status += CLEAN_FLTR_BIT;
    
    Data[1] = status;

    if ( ( device == EXTERNAL ) && ( Datalog.Ext_Mem_Present ) )
    {
        Record_Real_Time_And_Date(EXTERNAL);
        // Write to external memory
        if ( EXM_fnLogData( Data, 2 ) )
            Increment_Sync_Insertion_Counter();
    }
}
#endif // EXT_MEM
/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_fnTurnOffLoads -->
       \brief This routine is called when a critical shutdown of APU is necessary
              and when a regeneration is about to start.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void DPF_fnTurnOffLoads( void )
{
    ICE_fnLoadControl( TRUE );
    APU_fnLoadControl( TRUE );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: DPF_fnRestoreLoads -->
       \brief This routine is called when a critical fault is cleared or
              or a regeneration is completed or abondoned.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void DPF_fnRestoreLoads( void )
{
    ICE_fnLoadControl( FALSE );
    APU_fnLoadControl( FALSE );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Install_DPF -->
       \brief This routine is called when a DPF is detected an user confirmation 
              is requested and that the user has pushed the menu key. It is also 
              called through menu item that installs the DPF.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Install_DPF (void) 
{
    DPF_Diag.bInstalled = TRUE;
    DCP_Configuration |= DPF_INSTALLED;
    DPF_Timers.Disconnection = DISCONNECT_DETECTION_TIME;
    (void) nvfPutNumber (NV_ID_DCP_CONFIG, DCP_Configuration);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Uninstall_DPF -->
       \brief This routine is called through menu item that installs the DPF.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Uninstall_DPF (void)
{
    DPF_Diag.bInstalled = FALSE;
    DPF_Diag.bPresent = FALSE;
    DPF_Diag.Record &= ~MISSING;
    DCP_Configuration &= ~DPF_INSTALLED;
    DPF_Timers.Disconnection = 0;
    APU_Critical_Shutdown_Can_Clear();
    (void) nvfPutNumber (NV_ID_DCP_CONFIG, DCP_Configuration);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Clear_DPF_Screen -->
       \brief This routine is called when a reset of DPF state is performed 
              and the screen needs to be cleared. Sometimes some of these 
              messages need to stay on the screen in which case the calling
              routine must take care of it. 

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Clear_DPF_Screen(void)
{
    VWM_fnRemoveView (VWM_eVIEW_DPF_HOLD_ON_OFF_BUTTON);
    VWM_fnRemoveView (VWM_eVIEW_DPF_TURN_APU_ON);

}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Set_Emergency_Regen_Flag -->
       \brief This routine is called through menu item. This allows the APU to 
              to stay on in the fault cases that it would normally been shutdown.
              This is to allow the user to manually perform a regeneration in hopes
              of clearing the fault that had caused the APU shutdown. After one 
              round of usage this flag will be cleared.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Set_Emergency_Regen_Flag(void)
{
    DPF_Diag.Emergency_Regen_Flag = TRUE;
    APU_Diag.Crtical_Shutdown = FALSE;
    APU_Diag.Emergency_Regen_Sent_Once = FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Service_Two_Failures -->
       \brief This routine is called when there are pressure AND temperature 
              sensor failures at the same time. The system will then have tobe 
              shutdown. 

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Service_Two_Failures(void)
{
    DPF_fnTurnOffLoads();
    APU_Critical_ShutDown();
    VWM_fnRequestView (VWM_eVIEW_DPF_TWO_FAILURES_SHUT_DOWN, BACKLIGHT_ON);
}
