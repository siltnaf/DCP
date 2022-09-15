/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file ice_pnl.c
       \brief CCU Climate Control state machine

<!-- DESCRIPTION: -->
    To provide control for sending signals to start or stop the Air Conditioning, Heater
    and Outlet power

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/ice_pnl.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

          $Log: not supported by cvs2svn $
          Revision 1.7  2014/08/25 19:31:34  blepojevic
          Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

          Revision 1.6  2014/08/11 19:07:43  blepojevic
          Changed debounce time for fault detection to 60 sec. Software made more robust to protect from storing dummy faults.

          Revision 1.5  2014/07/25 15:24:16  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.4  2014/06/27 17:35:27  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.3  2014/04/16 14:07:08  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.2  2013/09/19 13:07:44  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.1  2010/01/12 23:31:50  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
         Revision 1.15  2010-01-12 15:29  echu
          - changed comfort monitor to output high heat.

          Revision 1.14  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.13  2008-08-21 15:43:11  msalimi
          - Added feature to turn on the backlight when fault or important messages are being displayed.
          - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
          - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
          - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

          Revision 1.12  2008-06-03 17:34:27  msalimi
          - ded a message for heat adjustment operation to display "Regenerating" when in regen mode.
          - Cancelled restoring the loads when the sensor pressure and extreme pressure failures are cleared. The loads are now being restored only when regen is finished.

          Revision 1.11  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.10  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.9  2007-11-30 18:52:07  msalimi
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

          Revision 1.8  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.7  2007-10-17 18:34:44  msalimi
          Fixed external memory log problem.

          Revision 1.6  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.5  2007-09-21 20:50:17  msalimi
          Added new data recording to external memory for CCU status.

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:03:28  nallyn
          Added and modified commenting for Doxygen

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.13  2007-06-01 17:43:55  msalimi
          Dale added comments.

          Revision 1.12  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline
              - we will subsequently place the multi-timer or 10-timer versions as
              branches off the main baseline so that we can continue multi-timer version
              development on a parallel branch. The multi-timer stuff was committed or
              merged with the main trunk originally - this was a mistake.
    
              Revision 1.11  2007/02/27 01:20:17  nallyn
          Added 4 more timers for a total of 10 timers. The timers function differently
          than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
          timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
          depending on the temperture selected:
          18-20: High AC
          21-23: Low AC
          24: HVAC Off
          25-27: Low Heat
          28-30: High Heat
    
    Revision 1.10  2007/02/26 17:49:52  nallyn
    Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
    
    Revision 1.9  2007/02/23 18:11:28  nallyn
    Changed from 2 timers to 4 timers for testing purposes
    
          Revision 1.8  2006/02/08 20:16:35  nallyn
          Changes based on code review
          Array bounds checking and removal of unused code
    
          Revision 1.7  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "ice_pnl.h"
#include <stdio.h>              // for printf
#include "main.h"               // Common definitions
#include "timers.h"             // Interface to software counter timers
#include "lcd_grp1.h"           // Interface to LCD
#include "fonts.h"              // Interface to LCD fonts
#include "apu.h"                // Interface to APU State Machine
#include "menu.h"               // Menu definitions
#include "coldstart.h"
#include "tempmon.h"            // Interface to Temperature Monitor
#include "timekeep.h"           // Interface for clock time keeping
#include "iconblinker.h"        // Interface to Icon definitions
#include "ViewMan.h"            // Interface to using View Manager
#include "Datalog.h"
#include "ExtNvmem.h"
#include "rtclock.h"
#include "DPF.h"
#include "nvmem.h"              // Interface to Non-Volatile Memory storage

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#define NO_POWER        0   //!<apuPowerState state
#define DELAY_POWER     1   //!<apuPowerState state
#define FULL_POWER      2   //!<apuPowerState state

#define ADC_RANGE       (1024)
#define ADC_MAX_VALUE   ( ADC_RANGE - 1 )
#define ADC_MIN_VALUE   (0)

#define FAN_HEAT_COOL_HIGH  0
#define FAN_HEAT_COOL_LOW   1

//!Structure for interpreting AtoD channel from the CCU
typedef struct
{
    int adValue;    //!<The raw ADC data
    char acPwr;     //!<VAC input
    char brkrOne;   //!<Breaker 1 input
    char brkrTwo;   //!<Breaker 2 input
} acB1B2;

/* ---------- I n t e r n a l   D a t a ---------- */
//!Table for interpreting AtoD channel from the CCU - where xxxx in adCal(xxxx) is in millivolts
const acB1B2 acBrkrOneBrkrTwo[5] =
{
    {adCal(750),OFF,OFF,OFF},    
    {adCal(2000),ON,OFF,OFF},
    {adCal(3000),ON,OFF,ON},
    {adCal(4000),ON,ON,OFF},
    {adCal(5000),ON,ON,ON}
};

// Is APU providing power?
static bit hvacApuPower;

static enum f_f_state FAN_FLAG_STATE = ST_FANF_OFF;

/* ---------- G l o b a l   D a t a ---------- */
//!Variable used to determine whether, when in heat mode, heat and fan should
//!be turned on based on ambient temperture and set point
char LAST_HEAT = 0;
//!Variable used to determine whether, when in a/c mode, a/c and fan should
//!be turned on based on ambient temperture and set point
#ifndef MM32F0133C7P		//? Add
bank1 char LAST_COOL = 0;
#else
char LAST_COOL = 0;
#endif

extern unsigned char TxPgn_65280[8];

/* ----------------- F u n c t i o n s ----------------- */
#ifdef EXT_MEM
static unsigned char Set_Heat_Cool_State (void);
#endif // EXT_MEM
/******************************************************************************/
/*!
<!-- FUNCTION NAME: decode_ac_br1_br2 -->
       \brief This function is used to interpret the feedback message from the CCU.

<!-- PURPOSE: -->
    The CCU communicates with the DCP using an analog channel. This function
    debounces (time filters) the AtoD input and classfies the input into
    ac power, breaker 1 and breaker 2 messages.

<!-- NOTES: -->
       \note
       \code
          Modifies:
              AC_PWR          - is ac power is available
              PAUSE           - has power levels momentarily dipped
              hvacApuPower    - APU is on and AC power is ready
              HVAC_ENABLE     - APU is on or AC power is available
              BR1             - breaker 1 is open
              BR2             - breaker 2 is open
       \endcode

<!-- PARAMETERS: -->
       @param[in]  adInputValue  AtoD reading from the CCU AC/BR1/BR2 channel
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void decode_ac_br1_br2(volatile int adInputValue)
{
    static uint8 apuPowerState;
    char i;

    // Scan through breaker table    
    for ( i = 0; i < DIM( acBrkrOneBrkrTwo ); i++ )
    {
        // If input is below ad value
        if ( adInputValue <= acBrkrOneBrkrTwo[ i ].adValue )
        {
            // Use the index
            break;
        }
    }
    
    // If index does not fit into table
    if ( i >= DIM( acBrkrOneBrkrTwo ) )
    {
        // Use the highest index
        i = DIM( acBrkrOneBrkrTwo ) - 1;
    }

    // Look up index into the AtoD conversion table and set individual flags
    ac_pwr1 = acBrkrOneBrkrTwo[i].acPwr;
    b1 = acBrkrOneBrkrTwo[i].brkrOne;
    b2 = acBrkrOneBrkrTwo[i].brkrTwo;

    // Check the state of AC power
    switch(AC_PWR_STATE)
    {
       case ST_AC_PWR_OFF:
          AC_PWR = 0;
          PAUSE = 0;
          mainBrakerTripped = FALSE;
          
          if(ac_pwr1)                           // If AC power is sensed
          {
             AC_PWR_STATE = ST_AC_PWR_ON;
          }
          else if (APU_ON_FLAG == TRUE)
          {
             timers.a_cTmr = 10;	
             AC_PWR_STATE = ST_AC_PWR_OFF_WAIT;
          }
          break;
       case ST_AC_PWR_OFF_WAIT:
          AC_PWR = 0;
          PAUSE = 0;
          if(ac_pwr1)                           // If AC power is sensed
          {
             AC_PWR_STATE = ST_AC_PWR_ON;
             mainBrakerTripped = FALSE;
          }
          else if ((APU_ON_FLAG == TRUE) && (mainBrakerTripped == FALSE) && (!timers.a_cTmr) && (ENGINE_FAULT == 0))
          {
             Record_Fault( SPN_CCU_MAIN_BR_TRIPPED, FMI_CCU_MAIN_BR_TRIPPED, SET );
             mainBrakerTripped = TRUE;
             TxPgn_65280[7] |= 0x04;
          } 
          break;
       case ST_AC_PWR_ON:
          AC_PWR = 1;
          PAUSE = 0;
          if(!ac_pwr1 && !APU_ON_FLAG)
          {
             AC_PWR_STATE = ST_AC_PWR_OFF;
          }
          else if(!ac_pwr1)
          {
             timers.a_cTmr = 5;
             AC_PWR_STATE = ST_PAUSE_WAIT;
          }
          else
          {
             AC_PWR_STATE = ST_AC_PWR_ON;
          }
          break;
       case ST_PAUSE_WAIT:
          AC_PWR = 1;
          PAUSE = 0;
          if(!ac_pwr1 && !APU_ON_FLAG)
          {
             AC_PWR_STATE = ST_AC_PWR_OFF;
          }
          else if(ac_pwr1)
          {
             AC_PWR_STATE = ST_AC_PWR_ON;
          }
          else if(!timers.a_cTmr)
          {
             AC_PWR_STATE = ST_AC_PWR_PAUSE;
          }
          else
          {
             AC_PWR_STATE = ST_PAUSE_WAIT;
          }
          break;
       case ST_AC_PWR_PAUSE:
          AC_PWR = 1;
          PAUSE = 1;
          if(!ac_pwr1 && !APU_ON_FLAG)
          {
             AC_PWR_STATE = ST_AC_PWR_OFF;
          }
          else if(ac_pwr1)
          {
             timers.a_cTmr = 10;
             AC_PWR_STATE = ST_AC_ON_WAIT;
             mainBrakerTripped = FALSE;
          }
          else
          {
             if ((mainBrakerTripped == FALSE) && (ENGINE_FAULT == 0) && (!timers.a_cTmr))
             {
                 Record_Fault( SPN_CCU_MAIN_BR_TRIPPED, FMI_CCU_MAIN_BR_TRIPPED, SET );
                 mainBrakerTripped = TRUE;
                 TxPgn_65280[7] |= 0x04;
             }
             AC_PWR_STATE = ST_AC_PWR_PAUSE;
          }
          break;
       case ST_AC_ON_WAIT:
          AC_PWR = 1;
          PAUSE = 1;
          if(!ac_pwr1 && !APU_ON_FLAG)
          {
             AC_PWR_STATE = ST_AC_PWR_OFF;
          }
          else if(!ac_pwr1)
          {
             AC_PWR_STATE = ST_AC_PWR_PAUSE;
          }
          else if(!timers.a_cTmr)
          {
             AC_PWR_STATE = ST_AC_PWR_ON;
          }
          else
          {
             AC_PWR_STATE = ST_AC_ON_WAIT;
          }
          break;
       default:
          AC_PWR_STATE = ST_AC_PWR_OFF;
          break;
    }

    switch( apuPowerState )
    {
        case NO_POWER:
            hvacApuPower = FALSE;
            if ( APU_ON_FLAG && AC_PWR )
            {
                // Start the load delay timer
                timers.apuAcTmr = 30;
                apuPowerState = DELAY_POWER;
            }
            break;
        
        case DELAY_POWER:
            if ( !APU_ON_FLAG || !AC_PWR )
            {
                apuPowerState = NO_POWER;
            }
            else if ( timers.apuAcTmr == 0 )
            {
                hvacApuPower = TRUE;
                apuPowerState = FULL_POWER;
            }
            break;
            
        case FULL_POWER:
            if ( !APU_ON_FLAG || !AC_PWR )
                apuPowerState = NO_POWER;
        
            break;
            
        default:
            apuPowerState = NO_POWER;
            break;
    }
    
    HVAC_ENABLE = (APU_ON_FLAG || AC_PWR);
    
    switch(B1STATE)
    {
       case ST_OFF:
          BR1 = 0;
          if(b1)
          {
             B1STATE = ST_TRIPPING;
             timers.b1Tmr = 200;//b1MsTimer(20);
          }
          break;
       case ST_TRIPPING:
          if(!timers.b1Tmr)//b1TimeOut)
          {
             if(b1)
             {
                B1STATE = ST_TRIPPED;
             }
             else
             {
                B1STATE = ST_OFF;
             }
          }
          break;
       case ST_TRIPPED:
          if (BR1 == 0)
          {
             Record_Fault( SPN_CCU_BR1_TRIPPED, FMI_CCU_BR1_TRIPPED, SET );
             TxPgn_65280[7] |= 0x01;
          }
          BR1 = 1;
          if(!b1)
          {
             B1STATE = ST_OFF;
          }
          break;
       default:
          BR1 = 0;
          break;
    }

    switch(B2STATE)
    {
       case ST_OFF:
          BR2 = 0;
          if(b2)
          {
             B2STATE = ST_TRIPPING;
             timers.b2Tmr = 200;//b2MsTimer(20);
          }
          break;
       case ST_TRIPPING:
          if(!timers.b2Tmr)//b2TimeOut)
          {
             if(b2)
             {
                B2STATE = ST_TRIPPED;
             }
             else
             {
                B2STATE = ST_OFF;
             }
          }
          break;
       case ST_TRIPPED:
          if (BR2 == 0)
          {
             Record_Fault( SPN_CCU_BR2_TRIPPED, FMI_CCU_BR2_TRIPPED, SET );
             TxPgn_65280[7] |= 0x02;
          }
          BR2 = 1;
          if(!b2)
          {
             B2STATE = ST_OFF;
          }
          break;
       default:
          BR2 = 0;
          break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: thermostat -->
       \brief Allows the operator to change the temperature set point for manual cab temperature control.

<!-- PURPOSE: -->
       A temperature gauge is displayed on the LCD screen
       and the user can use the up and down arrow keys to raise or lower the
       temperature set point.  This function also checks the many reasons that
       the manual temperature should not be displayed/adjusted.
    
<!-- NOTES: -->
       \note
       \code
          modifies:
              DECODE_TEMP     - the temperture set point (as an AtoD value)
              AMBIENT_TEMP    - the current temperature (as an AtoD value)
       \endcode

<!-- PARAMETERS: Void -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void thermostat(void)
{
    int16 i16Temp;
    bool  bInView;
    
    DECODE_TEMP = 0;
    AMBIENT_TEMP = 0;
    switch(THERMO_STATE)
    {
       case TH_ST_INIT:
          TEMP = 65;  //Equivalent to 75 F
          THERMO_STATE=TH_OFF;
          break;
       case TH_OFF:
          if((AC_FLAG || HEAT_FLAG) )
          {
             THERMO_STATE=TH_ST_HOLD;
          }
          else
          {
             THERMO_STATE=TH_OFF;
          }
          break;
       case TH_ST_HOLD:
          // If thermostat is in view
          bInView =  VWM_fnQueryView( VWM_eVIEW_MANUAL_THERMOSTAT );
          if ( bInView )
          {
             ICE_fnDrawThermostat();
          }
          
          if(!AC_PWR || (!AC_FLAG && !HEAT_FLAG) )
          {
             THERMO_STATE = TH_OFF;
          }
          else if( ( TEMP_UP_BTN == 1 ) && bInView )
          {
             THERMO_STATE = TH_ST_UP;
          }
          else if ( ( TEMP_DOWN_BTN == 1 ) && bInView )
          {
             THERMO_STATE = TH_ST_DOWN;
          }
          else
          {
             THERMO_STATE = TH_ST_HOLD;
          }
          break;
       case TH_ST_UP:
          TEMP++;//=2;
          if(TEMP >= 115)
             TEMP = 115;
          THERMO_STATE=TH_ST_HOLD;
          break;
       case TH_ST_DOWN:
          TEMP--;//=2;
          if(TEMP <= 15)
             TEMP = 15;
          THERMO_STATE=TH_ST_HOLD;
          break;
       default:
          THERMO_STATE=TH_OFF;
          break;
    }

    AMBIENT_TEMP = TEMPERATURE;

    // Inverse transfer function to convert threshold back from degC to ADC counts
    i16Temp = ( ( -14 * TEMP ) / 10 + 616 );

    // Force i16Temp to limits of ADC range of 0-1023 before converting to unsigned int

    if (i16Temp > ADC_MAX_VALUE)
    {
        i16Temp = ADC_MAX_VALUE;
    }

    if (i16Temp < ADC_MIN_VALUE )
    {
        i16Temp = ADC_MIN_VALUE;
    }

    // Although some TEMP values could make ADC counts -ve, within the TEMP range we are interested the corresponding
    // ADC counts should be a +ve value in 0-1023, so there is not a issue with loss of data due to casting
    DECODE_TEMP = (uint16) i16Temp;
    
}

void ICE_fnDrawThermostat( void )
{
    lcd_puttemp();              // draw temp scale
    lcd_settemp( TEMP, 6 );     // draw indicator
        
    // Note: all strings must have same length
    
    lcd_position(30, 4, large);    

    if ( AC_HEAT_STATE == AC_STATE )
    {
        if ( ac_state == high )
        {
            printf( " A/C HIGH" );
        }
        else if ( ac_state == low )
        {
            printf( " A/C LOW " );
        }
    }
    else if ( AC_HEAT_STATE == HEAT_STATE )
    {
        if ( heat_state == high )
        {
            printf( "HEAT HIGH" );
        }
        else if ( heat_state == low )
        {
            printf( "HEAT LOW " );
        }
    }
    else if ( AC_HEAT_STATE == UNLOAD_STATE )
    {
        lcd_position(16,4,large);
        printf( "REGENERATING" );
    }
    else
    {
        // Erase the label space
        lcd_cla( 30, 105, 4, 5 );
    }
}
    
/******************************************************************************/
/*!

<!-- FUNCTION NAME: heat_enable -->
       \brief This function is used to determine if heating is required when in manual control mode.

<!-- PURPOSE: -->
       Detailed description of function

<!-- NOTES: -->
       \note
    The AtoD to temperature conversion has a negative slope.  A higher AtoD
    value means a lower temperature.

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return 1 (TRUE) if heating is required, or 0 (FALSE) if heating is not required

*/
/******************************************************************************/
char heat_enable(void)
{
    char i;
    
    if(AMBIENT_TEMP >= (DECODE_TEMP + 5))
    {
       i=1;
    }
    else if(AMBIENT_TEMP <= (DECODE_TEMP - 5))
    {
       i=0;
    }
    else if(LAST_HEAT)
    {
       i = 1;
    }
    else
       i = 0;

    LAST_HEAT = i;
    return(i);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ac_enable -->
       \brief This function is used to determine if cooling (air-conditioning) is
       required when in manual control mode.

<!-- PURPOSE: -->
       Detailed description of function

<!-- NOTES: -->
       \note
       The AtoD to temperature conversion has a negative slope.  A higher AtoD
       value means a lower temperature.

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return 1 (TRUE) if the cooling is required, or 0 (FALSE) if cooling is not required.

*/
/******************************************************************************/
char ac_enable(void)
{
    char i;

    if( AMBIENT_TEMP <= (DECODE_TEMP - 5) )
    {
       i = 1;   // TRUE
    }
    else if( AMBIENT_TEMP >= (DECODE_TEMP + 5) )
    {
       i = 0;   // FALSE
    }
    else if( LAST_COOL )
    {
       i = 1;   // TRUE
    }
    else
    {
       i = 0;   // FALSE
    }

    LAST_COOL = i;

    return(i);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: outlet -->
       \brief This functions determines if the ac outlet is allowed to be on. This
    function also controls the outlet icon. 

<!-- PURPOSE: -->
       Detailed description of function
<!-- NOTES: -->
       \note
       \code
          Modifies:
             OUTLET - should the ac outlet be on
       \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void outlet(void)
{
    switch(OUTLET_STATE)
    {
       case ST_OUTLET_OFF:
          iconShow( ICON_OUTLET_OFF );
          OUTLET = 0;
          if(BR2 && AC_PWR)
          {
             OUTLET_STATE = ST_OUTLET_TRIPPED;
          }
          else if((heat_state != high) && AC_PWR && !PAUSE && !APU_STOPPED)
          {
             OUTLET_STATE = ST_OUTLET_ON;
          }
          else
          {
             OUTLET_STATE = ST_OUTLET_OFF;
          }
          break;
       case ST_OUTLET_ON:
          OUTLET = 1;
          iconShow( ICON_OUTLET_ON );
          if(BR2 && AC_PWR)
          {
             OUTLET_STATE = ST_OUTLET_TRIPPED;
          }
          else if((heat_state == high) || !AC_PWR || PAUSE && !APU_STOPPED)
          {
             OUTLET_STATE = ST_OUTLET_OFF;
          }
          else
          {
             OUTLET_STATE = ST_OUTLET_ON;
          }
          break;
       case ST_OUTLET_TRIPPED:
          OUTLET = 0;
          if(heat_state == high)
          {
              OUTLET_STATE = HEAT_TRIPPED;
          }
          else
          {
              OUTLET_STATE = OUTLET_TRIPPED;
          }
          break;
       case HEAT_TRIPPED:
          iconShow( ICON_HEAT_TRIP );
          if(!BR2)
          {
             OUTLET_STATE = ST_OUTLET_RESET;
          }
          break;
       case OUTLET_TRIPPED:
          iconShow( ICON_OUTLET_TRIP );
          if(!BR2)
          {
             OUTLET_STATE = ST_OUTLET_RESET;
          }
          break;
       case ST_OUTLET_RESET:
          OUTLET = 0;
          iconShow( ICON_OUTLET_RESET );
          if(BR2 && AC_PWR)
          {
             OUTLET_STATE = ST_OUTLET_TRIPPED;
          }
          else if(!BR2 && BRESET_BTN)
          {
             OUTLET_STATE = ST_OUTLET_OFF;
          }
          else
          {
             OUTLET_STATE = ST_OUTLET_RESET;
          }
          break;
       case ST_OUTLET_UNLOAD:      // Hold with no load 
          // Outlet load should be off, wait for resume
          break;
       default:
          OUTLET_STATE = ST_OUTLET_OFF;
          break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fans -->
       \brief This function set the fan speed depending on the fan speed setting.

<!-- PURPOSE: -->
       Detailed description of function
<!-- NOTES: -->
       \note
       \code
          modifies:
              FAN_LOW     - should the fan be on low
              FAN_HIGH    - should the fan be on high
       \endcode

<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fans(void)
{
    switch(FAN_FLAG)
    {
       case 0:
          FAN_LOW = OFF;
          FAN_HIGH = OFF;
          break;
       case 1:
          FAN_LOW = ON;
          FAN_HIGH = OFF;
          break;
       case 2:
          FAN_LOW = OFF;
          FAN_HIGH = ON;
          break;
       default:
          FAN_LOW = OFF;
          FAN_HIGH = OFF;
          break;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ac_heat_flag -->
       \brief This function determines if the CCU should be providing heating or
       cooling or neither.

<!-- PURPOSE: -->
       It also can determine if the level of output should be low, high or off.  
       This function is used for manual and automatic control.
       This function allows the user to take manual control of heating or cooling
       and to set low heat, high heat or no heat, or, to set low cooling,
       high cooling or no cooling.
       This function also detemines if the display should show heating or
       cooling or neither.
       This function also shows if there is a breaker trip and allows the
       user to reset the breaker trip indication.

<!-- NOTES: -->
       \note
       \code
          modifies:
              HEAT_CONT       - should heating control be heating
              AC_CONT         - should ac control be cooling
              BREAKER_TRIP    - was a breaker tripped and not reset by user
              FAN_LOW         - should the fan be on low
              FAN_HIGH        - should the fan be on high
              ac_state        - should the cooling be low, high or off
              heat_state      - should the heating be low, high or off
       \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void ac_heat_flag(void)
{
    char i;
    bool  bThermoInView;
    bool  bClockInView;
    
    // If thermostat is in view
    bThermoInView = VWM_fnQueryView( VWM_eVIEW_MANUAL_THERMOSTAT );
    bClockInView  = VWM_fnQueryView( VWM_eVIEW_CLOCK );

    switch(AC_HEAT_STATE)
    {
    case OFF_STATE:
       fans();
       HEAT_CONT = OFF;  //Heat Off
       AC_CONT = OFF;
       VWM_fnRemoveView( VWM_eVIEW_MANUAL_THERMOSTAT );
       bThermoInView = FALSE;

       //****check the following for necessity****
       if(BREAKER_TRIP && APU_ON_FLAG)
       {
          i = 1;
       }
       else
       {
          i = 0;
       }
       BREAKER_TRIP = 0;
       //*****************************************

       AC_FLAG = OFF; 
       HEAT_FLAG = OFF;
       
       if(BR1 || i)   //if breaker tripped or breaker was tripped before APU shutdown
       {
          AC_HEAT_STATE = BREAKER_STATE;
       }
       else if(AC_PWR && HVAC_ENABLE)
       {
       
          //**** Order here is important, ordered by priority
          
          if ( AC_BTN && bClockInView && !tmgTMMode )
          {
             ac_state = low;
             VWM_fnRequestView( VWM_eVIEW_MANUAL_THERMOSTAT, BACKLIGHT_ON );
             AC_HEAT_STATE = AC_STATE;
          }
          else if ( HEAT_BTN && bClockInView && !tmgTMMode )
          {
            if( HEAT_ENABLE)
             {
                 heat_state = low;
                 VWM_fnRequestView( VWM_eVIEW_MANUAL_THERMOSTAT, BACKLIGHT_ON );
                 AC_HEAT_STATE = HEAT_STATE;
             }
             else
             {
                 VWM_fnRequestView( VWM_eVIEW_HEAT_DISABLED, BACKLIGHT_ON );
                 timers.menu3Tmr = 500;
             }
          }
#ifdef USE_COLDSTART        
          else if(bCmRunHighHeat )
          {
             FAN_HIGH = ON;
             HEAT_CONT = ON;
             heat_state = high;
          }
#endif // USE_COLDSTART
          else if( tkgTimer1.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE  )
             {
                 HEAT_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer1.tkxTempSet > 27 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 heat_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
#endif  // EXTRA_TIMERS
             }
          }
          else if( tkgTimer1.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer1.tkxTempSet < 21 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 ac_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
#endif // EXTRA_TIMERS
             }
          }
          else if( tkgTimer2.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer2.tkxTempSet > 27 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 heat_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
#endif  // EXTRA_TIMERS
             }
          }
          else if( tkgTimer2.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer2.tkxTempSet < 21 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 ac_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
#endif // EXTRA_TIMERS
             }
          }
#ifndef USE_COLDSTART
          else if( tkgTimer3.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE  )
             {
                 HEAT_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer3.tkxTempSet > 27 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 heat_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
#endif  // EXTRA_TIMERS
             }
          }
          else if( tkgTimer3.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer3.tkxTempSet < 21 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 ac_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
#endif // EXTRA_TIMERS
             }
          }
          else if( tkgTimer4.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer4.tkxTempSet > 27 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 heat_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
#endif  // EXTRA_TIMERS
             }
          }
          else if( tkgTimer4.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
#ifdef EXTRA_TIMERS
                if ( tkgTimer4.tkxTempSet < 21 )
                {
#endif // EXTRA_TIMERS
                 FAN_HIGH = ON;
                 ac_state = high;
#ifdef EXTRA_TIMERS
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
#endif // EXTRA_TIMERS
             }
          }
#ifdef EXTRA_TIMERS
          else if( tkgTimer5.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE  )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer5.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer5.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer5.tkxTempSet < 21 )
                {
                    FAN_HIGH = ON;
                    ac_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          }
          else if( tkgTimer6.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer6.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer6.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer6.tkxTempSet < 21 )
                {
                    FAN_HIGH = ON;
                    ac_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          }
          else if( tkgTimer7.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer7.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer7.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer7.tkxTempSet < 21 )
                {
                    FAN_HIGH = ON;
                    ac_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          }
          else if( tkgTimer8.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer8.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer8.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer8.tkxTempSet < 21 )
                {
                    FAN_HIGH = ON;
                    ac_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          }      
          else if( tkgTimer9.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer9.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer9.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer9.tkxTempSet < 21 )
                {
                 FAN_HIGH = ON;
                 ac_state = high;
             }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          }   
          else if( tkgTimer10.tkxThermo == TM_HEATING )
          {
             // If using power from APU
             if ( hvacApuPower && HEAT_ENABLE  )
             {
                 HEAT_CONT = ON;
                if ( tkgTimer10.tkxTempSet > 27 )
                {
                    FAN_HIGH = ON;
                    heat_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    heat_state = low;
                }                 
             }
          }
          else if( tkgTimer10.tkxThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {
                 AC_CONT = ON;
                if ( tkgTimer10.tkxTempSet < 21 )
                {
                    FAN_HIGH = ON;
                    ac_state = high;
                }
                else
                {
                    FAN_HIGH = OFF;
                    FAN_LOW = ON;
                    ac_state = low;
                }
             }
          } 
#endif // EXTRA_TIMERS
#endif // USE_COLDSTART
          else if( tmgTempMonRun && (tmgThermo == TM_HEATING) && HEAT_ENABLE  )
          {
             // If using power from APU
             if ( hvacApuPower )
             {  // in comfort monitor, when enabled change of fan speed, and selected low speed
                if ( tmgTMMode && (tmgFSRange == FAN_SPEED_LOW_HIGH) && tmgFSSelected == FAN_HEAT_COOL_LOW  )
                {
                    HEAT_CONT = ON;
                    FAN_LOW = ON;
                    heat_state = low;
                }
                else
                {
                    HEAT_CONT = ON;
                    FAN_HIGH = ON;
                    heat_state = high;
                }
             }
             // If using shore power
             else if ( SHORE_POWER )
             {
                 HEAT_CONT = ON;
                 FAN_LOW = ON;
                 heat_state = low;
             }
          }
          else if ( tmgTempMonRun && tmgThermo == TM_COOLING )
          {
             // If using power from APU
             if ( hvacApuPower )
             {   // in comfort monitor, when enabled change of fan speed, and selected low speed
                 if ( tmgTMMode && (tmgFSRange == FAN_SPEED_LOW_HIGH) && tmgFSSelected == FAN_HEAT_COOL_LOW  )
                 {
                    AC_CONT = ON;
                    FAN_LOW = ON;
                    ac_state = low;
                 }
                 else
                 {
                    AC_CONT = ON;
                    FAN_HIGH = ON;
                    ac_state = high;
                 }
             }
             else if ( SHORE_POWER )
             {
                 AC_CONT = ON;
                 FAN_LOW = ON;
                 ac_state = low;
             }
          }
          else
          {
             // Reset heat state so outlet will be on
             heat_state = low;
             ac_state = low;
          }
          
          // If Outlet/High Heat breaker is tripped
          if ( ( HEAT_CONT && FAN_HIGH )
            && ( OUTLET_STATE != ST_OUTLET_OFF )
            && ( OUTLET_STATE != ST_OUTLET_ON ) )
          {
             // Change to use low heat
             FAN_HIGH = OFF;
             FAN_LOW = ON;
          }          
       }
       break;
    case AC_STATE:    //AC State is for the Air conditioning
             //It can be turned off by:
             //    off button
             //    no ac power
             //    breaker trip
             //    pause due to low voltage
             //    heat button being pushed
       HEAT_CONT = OFF;
       AC_FLAG = 1;
       HEAT_FLAG = 0;
       if(!AC_PWR || HVAC_OFF_BTN || (APU_STOPPED && !SHORE_POWER))
       {
          AC_HEAT_STATE = OFF_STATE;
       }
       else if(BR1)
       {
          breaker = 0;
          AC_HEAT_STATE = BREAKER_STATE;
       }
       else if(PAUSE)
       {
          last_state = AC_HEAT_STATE;
          AC_HEAT_STATE = PAUSE_STATE;
       }
       else if( HEAT_BTN && bThermoInView  )
       {
          if (HEAT_ENABLE)
          {
             heat_state = low;
             AC_HEAT_STATE = HEAT_STATE;
          }
          else
          {
             VWM_fnRequestView( VWM_eVIEW_HEAT_DISABLED, BACKLIGHT_ON );
             timers.menu3Tmr = 500;
          }
       }
       else
       {
          i = ac_enable();
          switch(ac_state)
          {
          case low:   //Air conditioning low
             //AC_FLAG = 1;
             if(i)
             {
                FAN_HIGH = OFF;
                AC_CONT = ON;
                FAN_LOW = ON;
             }
             else
             {
                AC_CONT = OFF;
                fans();
             }
             
             if ( AC_BTN && bThermoInView )
             {
                ac_state = high;
             }
             break;
          case high:  //Air conditioning high
             //AC_FLAG = 2;
             if(i)
             {
                FAN_HIGH = ON;
                AC_CONT = ON;
                FAN_LOW = OFF;
             }
             else
             {
                AC_CONT = OFF;
                fans();
             }

             if ( AC_BTN && bThermoInView )
             {
                ac_state = low;
             }
             break;
          }
       }
       break;
    case HEAT_STATE:  //HEAT state is for heating
             //It can be turned off by:
             //    off button
             //    no ac power
             //    breaker trip
             //    pause due to low voltage
             //    heat button being pushed
       AC_FLAG = 0;
       HEAT_FLAG = 1;
       AC_CONT = OFF;
       if(!AC_PWR || HVAC_OFF_BTN || (APU_STOPPED && !SHORE_POWER))
       {
          heat_state = low;
          AC_HEAT_STATE = OFF_STATE;
       }
       else if(BR1)
       {
          heat_state = low;
          breaker = 0;
          AC_HEAT_STATE = BREAKER_STATE;
       }
       else if(PAUSE)
       {
          heat_state = low;
          last_state = AC_HEAT_STATE;
          AC_HEAT_STATE = PAUSE_STATE;
       }
       else if( AC_BTN && bThermoInView )
       {
          heat_state = low;
          ac_state = low;
          AC_HEAT_STATE = AC_STATE;
       }
       else	if(HEAT_ENABLE)
       {
          i = heat_enable();
          switch(heat_state)
          {
          case low:
             FAN_HIGH = OFF;
             if(i)
             {
                HEAT_CONT = ON;
                FAN_LOW = ON;
             }
             else
             {
                HEAT_CONT = OFF;
                fans();
             }

             if ( HEAT_BTN && !SHORE_POWER && !BR2 && bThermoInView )
             {
                if (HEAT_ENABLE)
                   heat_state = high;
                else
                {
                   VWM_fnRequestView( VWM_eVIEW_HEAT_DISABLED, BACKLIGHT_ON );
                   timers.menu3Tmr = 500;
                }
             }
             break;
          case high:
             FAN_LOW = OFF;
             if(i)
             {
                HEAT_CONT = ON;
                FAN_HIGH = ON;
             }
             else
             {
                HEAT_CONT = OFF;
                fans();
             }

             if ( HEAT_BTN && bThermoInView )
             {
                if(HEAT_ENABLE)
                   heat_state = low;
                else
                {
                   VWM_fnRequestView( VWM_eVIEW_HEAT_DISABLED, BACKLIGHT_ON );
                   timers.menu3Tmr = 500;
                }
             }
             
             if ( OUTLET_STATE == HEAT_TRIPPED )
             {
                heat_state = low;
             }
             break;
          }
       }
       break;
    case BREAKER_STATE:
       FAN_HIGH = OFF;
       FAN_LOW = OFF;
       HEAT_CONT = OFF;
       AC_CONT = OFF;
       BREAKER_TRIP = 1;

       switch(breaker)
       {
       case 0:
          VWM_fnRequestView( VWM_eVIEW_HVAC_BREAKER_TRIP, BACKLIGHT_ON);

          if(!BR1)
          {
             VWM_fnRemoveView( VWM_eVIEW_HVAC_BREAKER_TRIP);
             breaker = 1;
          }
          break;
       case 1:
          
#if 1 // Use this code is user reset is required

          VWM_fnRequestView( VWM_eVIEW_HVAC_BREAKER_RESET, BACKLIGHT_ON );
          
          if ( BR1 )
          {
             breaker = 0;
             VWM_fnRemoveView( VWM_eVIEW_HVAC_BREAKER_RESET );
          }
          
          if ( BRESET_BTN )
          {
             VWM_fnRemoveView( VWM_eVIEW_HVAC_BREAKER_RESET );
             BREAKER_TRIP = 0;
             AC_HEAT_STATE = OFF_STATE;
          }
          
#else  // Use this code if user reset is not required

          BREAKER_TRIP = 0;
          AC_HEAT_STATE = OFF_STATE;
          break;
#endif
       }
       break;
    case PAUSE_STATE:
       FAN_HIGH = OFF;
       FAN_LOW = OFF;
       HEAT_CONT = OFF;
       AC_CONT = OFF;
       VWM_fnRequestView( VWM_eVIEW_UNDER_VOLTAGE, BACKLIGHT_ON );
       
       if(!APU_ON_FLAG)
       {
          VWM_fnRemoveView( VWM_eVIEW_UNDER_VOLTAGE );
          AC_HEAT_STATE = OFF_STATE;
       }
       else if(!PAUSE)
       {
          VWM_fnRemoveView( VWM_eVIEW_UNDER_VOLTAGE );
          AC_HEAT_STATE = last_state;
       }
       break;
       
    case UNLOAD_STATE:       // Hold with no load
        // AC HEAT load should be off, wait for resume
       break;
       
    default:
       AC_HEAT_STATE = OFF_STATE;
       break;
    }

    if (timers.menu3Tmr == 1)	// by purpose not wait time to expire (to simplify software)
    {
       VWM_fnRemoveView( VWM_eVIEW_HEAT_DISABLED);	
       timers.menu3Tmr = 0;
    }
    
    if ( ( HEAT_CONT ) && ( ! VWM_fnQueryView( VWM_eVIEW_MENU ) ) )
    {
        iconShow( ICON_HEAT );
    }
    else
    {
        iconErase( ICON_HEAT );
    }
       
    if ( ( AC_CONT ) && ( ! VWM_fnQueryView( VWM_eVIEW_MENU ) ) )
    {
        iconShow( ICON_COOL );
    }
    else
    {
        iconErase( ICON_COOL );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fan_flag -->
       \brief This function allows the user to set the fan operating level: low, high or off.

<!-- PURPOSE: -->
       It also determines if the fan can be set at all.  If the fan can
       be set by the user, the fan will cycle from off to low to high to off.
       This function also manages the display of the fan icon.
<!-- NOTES: -->
       \note
       \code
          modifies:
              FAN_FLAG    - manual fan level: low, high or off
       \endcode
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fan_flag(void)
{
    switch(FAN_FLAG_STATE)
    {
       case ST_FANF_OFF:
          FAN_FLAG = 0;
          if(AC_PWR && !BREAKER_TRIP && FAN_BTN && !tmgTMMode )
          {
             FAN_FLAG_STATE = ST_FANF_LOW;
             ICE_fnDrawFan();
          }
          else if ( AC_PWR && !BREAKER_TRIP && FAN_BTN && tmgTMMode && tmgFSRange )   
          {   // in comfort monitor, when fan speed change enabled, use FAN button as toggle between HIGH and LOW speed/heat/cool switch
              tmgFSSelected = !tmgFSSelected;
              ICE_fnDrawFan(); 
          }
          else
          {
             FAN_FLAG_STATE = ST_FANF_OFF;
          }
          break;
       case ST_FANF_LOW:
          FAN_FLAG = 1;
          if(!AC_PWR || BREAKER_TRIP || HVAC_OFF_BTN || APU_STOPPED)
          {
             FAN_FLAG_STATE = ST_FANF_OFF;
             ICE_fnDrawFan();
          }
          else if(FAN_BTN)
          {
             FAN_FLAG_STATE = ST_FANF_HIGH;
             ICE_fnDrawFan();
          }
          else
          {
             FAN_FLAG_STATE = ST_FANF_LOW;
          }
          break;
       case ST_FANF_HIGH:
          FAN_FLAG = 2;
          if(!AC_PWR || BREAKER_TRIP || HVAC_OFF_BTN || FAN_BTN || APU_STOPPED)
          {
             FAN_FLAG_STATE = ST_FANF_OFF;
             ICE_fnDrawFan();
          }
          else
          {
             FAN_FLAG_STATE = ST_FANF_HIGH;
          }
          break;
       case ST_FANF_UNLOAD:      // Hold with no load
          // Fan load should be off, wait for resume
          break;
       default:
          FAN_FLAG_STATE = ST_FANF_OFF;
          break;
    }
}
    
//------------------------------------------------------------------------

void ICE_fnDrawFan( void )
{
    // Show Fan
    if ( ( VWM_fnQueryView( VWM_eVIEW_CLOCK ) )
      || ( VWM_fnQueryView( VWM_eVIEW_MANUAL_THERMOSTAT ) ) )
    {
        // Fan display follows Fan Flags not Fan Controls
        if (( FAN_FLAG_STATE == ST_FANF_HIGH ) || ( tmgTMMode && (tmgFSSelected == FAN_HEAT_COOL_HIGH) && (FAN_HIGH == ON) && (FAN_LOW == OFF)))
        {
            lcd_position(112,4,small);
            printf("FAN");
            lcd_position(109,5,small);
            printf("HIGH");
        }
        else if (( FAN_FLAG_STATE == ST_FANF_LOW ) || ( tmgTMMode && (tmgFSSelected == FAN_HEAT_COOL_LOW) && (FAN_LOW == ON)))
        {
            // Clear fan indication area to avoid screen debris
            lcd_cla(109,131,5,5);
            lcd_position(112,4,small);
            printf("FAN");
            lcd_position(112,5,small);
            printf("LOW");
        }
        else
        {
            // Clear fan indication area
            lcd_cla(109,131,4,5);
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: check_hvac_on -->
       \brief Determine if the user has set manual comfort control levels.

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return 1 (TRUE) if manual temperature or fan control is set (not off),
      or 0 (FALSE) if manual control are all set to off.

*/
/******************************************************************************/
char check_hvac_on(void)
{
    if ( (AC_HEAT_STATE == AC_STATE)
      || (AC_HEAT_STATE == HEAT_STATE)
      || (FAN_FLAG != 0) )
    {
       return(1);
    }
    return(0);
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: ICE_fnLoadControl -->
       \brief Control loads for Regeneration

<!-- PURPOSE: -->
       This function removes or resumes CCU load control for DPF regeneration.
       During regeneration all energy loads must be turned off; on the CCU
       AirCon/Heat, Fan and Outlet are energy loads.
<!-- PARAMETERS: -->
       @param[in]  bLoadsOff   TRUE to turn off loads, FALSE to resume loads.
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/

void ICE_fnLoadControl( bool bLoadsOff )
{
    static enum ac_h_f_state oldAcHeatState = OFF_STATE;
    static enum f_f_state    oldFanState = ST_FANF_OFF;
    static enum o_state      oldOutletState = ST_OUTLET_OFF;
    
    // If set to remove loads
    if ( bLoadsOff )
    {
        VWM_fnRemoveView( VWM_eVIEW_MANUAL_THERMOSTAT );
        
        // If not already in unloaded state
        if ( AC_HEAT_STATE != UNLOAD_STATE )
        {
            // Save AC HEAT state and set it to unloaded state
            oldAcHeatState = AC_HEAT_STATE;
            AC_HEAT_STATE = UNLOAD_STATE;
            
            // Save Fan state and set it to unloaded state
            oldFanState = FAN_FLAG_STATE;
            FAN_FLAG_STATE = ST_FANF_UNLOAD;
            
            // Save Outlet state and set it to unloaded state
            oldOutletState = OUTLET_STATE;
            OUTLET_STATE = ST_OUTLET_UNLOAD;
            
            // Turn off energy loads
            HEAT_CONT = OFF;
            AC_CONT = OFF;
            FAN_LOW = OFF;
            FAN_HIGH = OFF;
            OUTLET = OFF;
        }
    }
    else // Set to resume regular loading
    {
        // If in unloaded state
        if ( AC_HEAT_STATE == UNLOAD_STATE )
        {
            // Restore AC Heat, Fan and Outlet states
            AC_HEAT_STATE = oldAcHeatState;
            FAN_FLAG_STATE = oldFanState;
            OUTLET_STATE = oldOutletState;
            
            if ( AC_HEAT_STATE != OFF_STATE )
            {
                VWM_fnRequestView( VWM_eVIEW_MANUAL_THERMOSTAT, BACKLIGHT_ON );
            }
        }
    }
}

/******************************************************************************/
/*  Enable_Heaters - Control Heaters

<!-- PURPOSE: -->
       This function install heaters back to the system.
/******************************************************************************/

void Enable_Heaters(bool enable)
{
    HEAT_ENABLE = enable;
    (void) nvfPutNumber( NV_ID_HEAT_ENABLE, enable );
}


#ifdef EXT_MEM
void Record_CCU_Status_External (void)
{
unsigned char Data[2];

    Record_Real_Time_And_Date (EXTERNAL);

    Data[0] = CCU_RECORD;
    Data[1] = Set_Heat_Cool_State();

    // Write to external memory
    if ( EXM_fnLogData( Data, 2 ) )
        Increment_Sync_Insertion_Counter();
}

static unsigned char Set_Heat_Cool_State (void)
{
unsigned char state;

    state = AC_CONT;
    state += (HEAT_CONT << 1);
    state += (FAN_LOW << 2);
    state += (FAN_HIGH << 3);
    state += (BR1 << 4);
    state += (BR2 << 5);
    return (state);
}
#endif // EXT_MEM
