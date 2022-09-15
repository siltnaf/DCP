/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file coldstart.c
       \brief Module to provided functionality for Coldstart Extended Mode

<!-- DESCRIPTION: -->
       This module includes the functionality for the Coldstart Extended Mode state machine 
       and all supporting routines. 

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/coldstart.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/04/16 14:07:08  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.2  2013/09/19 13:07:43  blepojevic
	      Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements
	
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.4  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.3  2007-11-30 18:52:07  msalimi
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

	      Revision 1.2  2007-08-01 19:59:09  nallyn
	      Added commenting for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.16  2007-06-01 17:43:29  msalimi
	      Dale added comments.

	      Revision 1.15  2007/04/03 18:44:42  gfinlay
	      Placing ES35-000-04A files back at the head of the main truck or baseline
          - we will subsequently place the multi-timer or 10-timer versions as
          branches off the main baseline so that we can continue multi-timer version
          development on a parallel branch. The multi-timer stuff was committed or
          merged with the main trunk originally - this was a mistake.
	
	      Revision 1.14  2007/02/26 17:49:52  nallyn
	      Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS
        	
              Revision 1.13  2007/02/23 18:11:28  nallyn
	      Changed from 2 timers to 4 timers for testing purposes
	
	      Revision 1.12  2006/02/08 20:11:47  nallyn
	      Changes based on code review
	      Updated the temperature interpolation routine
	
	      Revision 1.11  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "coldstart.h"
#include "main.h"
#include "apu.h"                // Interface to APU State Machine
#include "iconblinker.h"
#include "timers.h"
#include "fonts.h"
#include <stdio.h>
#include "lcd_grp1.h"
#include "tempmon.h"		// To include temperature lookup table
#include "menu.h"

#ifdef USE_COLDSTART
/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
//! seconds to check if it is cold - 10 minutes
#define COLD_CHECK_TIME     600 
//!Seconds to wait after engine starts before turning on a load - 2 minutes
#define TURN_ON_LOAD_DELAY  120
//!Seconds in a 10th of an hour - 6 minutes 
#define TENTH_OF_HOUR       360 

//!Possibles states for temperature monitor
enum monitorState
{
    msNormal,   //!<Temperature is normal
    msLow       //!<Temperature is cold
};

/* ---------- I n t e r n a l   D a t a ---------- */

//!Default ColdStart configuration parameters (can be changed by user (menu))
gtCOLD_START tCOLD_START =
{
    1,      //!< enable cold monitor user access
    0,      //!< cold start selection state variable
    60,     //!< time to run unit when cold started
    -10,    //!< default start temp in C
    872     //!< voltage corresponding to start temp
};

bit bColdMonitor;       //!< ColdStart is turned on by user (menu)
bit bCmRun;             //!< ColdStart wants APU to run
bit bCmRunHighHeat;     //!< ColdStart high heat control

/* ---------- G l o b a l   D a t a ---------- */
//!State variable for controlling the heat when temperature monitor is active
static char heatOn = 0;
#endif // USE_COLDSTART

/* ----------------- F u n c t i o n s ----------------- */

#ifdef USE_COLDSTART        

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfCtoVad -->
       Converts Celsius temperature to Voltage AtoD value

<!-- PURPOSE: -->
    This function is used to convert given temperature in Celsius to an
    AtoD reading 

<!-- NOTES: -->
     \note 
    The AtoD to Celsius conversion has a negative slope.  A higher AtoD
    value means a lower temperature.

<!-- PARAMETERS: -->
       @param[in]  temp    Temperature for the Cold Start set point
<!-- RETURNS: -->
      \return AtoD value to match the Cold Start temperature set point

*/
/******************************************************************************/

unsigned int tmfCtoVad( signed char temp )
{
    unsigned char i;
    unsigned int Vad;

    if (temp > CSTART_HIGH_TEMP) 	temp = CSTART_HIGH_TEMP;			// this temperature is limited by menu options or content of EEPROM
    if (temp < CSTART_LOW_TEMP) 	temp = CSTART_LOW_TEMP;				// but just in case we have limits here in function

    for(i=0; i < (unsigned char) DIM(tmhVadTable)-1; i++)
    {
       if (temp <= tmhVadTable[i+1].tmxC)
       {
           Vad = tmhVadTable[i+1].tmxVad + ((temp - tmhVadTable[i+1].tmxC ) * tmhVadTable[i].tmxFactor)/10;
           break;
       }
    }
    return Vad;
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: fncTemperature -->
       \brief  line description of function.

<!-- PURPOSE: -->
    Compare an AtoD input against the cold start set point voltage.  The
    temperature/voltage conversion has a negative slope.  A higher voltage
    means a lower temperature. If temperature > 996 (actually lower than -40C
	return 0 to disable cold start function because it indicates missing thermistor.

<!-- PARAMETERS: -->
       @param[in]  tmp     An AtoD reading (from TEMPERATURE port) (voltage)
<!-- RETURNS: -->
      \return 1 (TRUE) if temperature is below (voltage is above) the set point,
    or, 0 (FALSE) if temperature is not below (voltage is not above) set point.

*/
/******************************************************************************/
bit fncTemperature(uint16 tmp)
{
 	if (( tmp > tCOLD_START.uiStartTempVoltage ) && (tmp <= 996))
 	{
 		return( 1 );
 	}
	return( 0 );
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: fncTurnOnHeat -->
       \brief Brief one line description of function.

<!-- PURPOSE: -->
    This function is used to delay the request for high heat output from the
    CCU until the APU has had time to warm up and can handle the load.
    This function is intended to be called from in the run state of the
    Cold Start Monitor state machine.

<!-- NOTES: -->
     \note 
     Modifies bCmRunHighHeat to indicate if the CCU should turn on high heat
    Requires the APU to be on and producing AC power (HVAC_ENABLE is TRUE)
    before starting the high heat delay timer.

<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fncTurnOnHeat(void)
{
	switch(heatOn)
	{
		case 0:
			bCmRunHighHeat = 0;
			if( HVAC_ENABLE )
			{
				timers.cmhTmr = TURN_ON_LOAD_DELAY; //seconds - 2 minutes
				heatOn = 1;
			}
			break;
		case 1:
			if( !HVAC_ENABLE )
			{
				heatOn = 0;
			}
			else if( timers.cmhTmr == 0 )
			{
				heatOn = 2;
			}
			break;
		case 2:
			bCmRunHighHeat = 1;
			break;
		default:
			heatOn = 0;
			break;
	}
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: cold_start_monitor -->
       \brief Cold Start monitor state machine to be called from the main loop.

<!-- PURPOSE: -->
    This function debounces and checks if the temperature is too low and
    will attempt to start and run the APU to keep it from getting too cold.
    This function also controls the Cold Start icon.

<!-- NOTES: -->
     \note 
        Sets cmRun to indicate that cold start wants the APU to run.
        Sets cmRunHighHeat to indicate that cold start wants the CCU to produce
        heat heat (to load the APU).
    \code
    The Cold Start Monitor states are:
        off      - not enabled by the user (menu)
        on       - enabled by user but APU is not runable
        standby  - ready to run but temperature is normal or delay timer running
        starting - request that the APU start and set cold start run time
        run      - wait for timer to expire or some other reason to give up
                  APU control
    The APU is runable if the the APU is not locked out, the APU is in
    AUTO mode (not run by user), and the breakers are not tripped.
    \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Temperature counter - number of ticks the temperature has been too low

*/
/******************************************************************************/
char cold_start_monitor(void)
{
 	static char temperatureCounter = 0;
    static enum monitor_state c_monitor = monitor_off;
 	static enum monitorState coldState = msNormal;
    char runable;

    // If the counter is not at maximum and the temperature is too cold
 	if( temperatureCounter < 0xff && fncTemperature( TEMPERATURE ) )
 	{
        // Count up the timer to debounce (time filter) it
 		temperatureCounter += 1;
 	}
    // If the counter is not a minimum and temperature is not too cold
	else if( temperatureCounter > 0 && !fncTemperature( TEMPERATURE) )
	{
        // Count down the timer to debounce (time filter) it
		temperatureCounter -= 1;
	}

    // Is the APU runable?    
    runable = ( !apuFaultLockout && AUTO && !BREAKER_TRIP );
    
    // State machine for handling the cold start process
    switch (c_monitor)
    {
        case monitor_off: //0
            bCmRun = 0;         // Do not request that APU should run
            bCmRunHighHeat = 0; // bit to control the enabling of high heat
            // If the monitor process is enabled by the user...            
            if ( bColdMonitor )
            {
                c_monitor = monitor_on; //...goto monitor on state
            }
            iconControl( ICON_COLDSTART, ICON_OFF ); //Erase the cold start icon
            break;
        case monitor_on: //1
            iconControl( ICON_COLDSTART, ICON_ON );//Display the cold start icon
            // If the monitor process is disabled by the user...            
            if ( !bColdMonitor )
            {
                c_monitor = monitor_off; //...next state monitor_off state
            }
            else if ( runable )
            {
                timers.cmTmr = COLD_CHECK_TIME; //Set the timer for 10 minutes
                c_monitor = monitor_standby;    // next state monitor standby
            }
            break;
        case monitor_standby: //2
            bCmRun = 0;         // Do not request that APU should run
            if ( !bColdMonitor || !runable )
            {
                c_monitor = monitor_off;
            }
            else
            {
                switch (coldState)
                {
                    case msNormal:
                        iconControl( ICON_COLDSTART, ICON_ON );
                        if (temperatureCounter > 200)
                        {
                            coldState = msLow;
                            timers.cmTmr = COLD_CHECK_TIME; //Set for 10 minutes
                        }
                        break;
                    case msLow:
                        iconControl( ICON_COLDSTART, ICON_BLINK_ASYMETRIC_ON );
                        if (temperatureCounter < 10)
                        {
                            coldState = msNormal;
                        }
                        else if (timers.cmTmr == 0)
                        {
                            c_monitor = monitor_starting;
                            timers.cmTmr = 5;
                        }
                        break;
                }
            }
            break;
        case monitor_starting: //3
            bCmRunHighHeat = 0;
            iconControl( ICON_COLDSTART, ICON_BLINK_FAST );
            if ( !bColdMonitor || !runable )
            {
                c_monitor = monitor_off;
            }
            else if (temperatureCounter < 10)
            {
                c_monitor = monitor_standby;
            }
            else if (timers.cmTmr == 0)
            {
                bCmRun = 1;     // Request that APU should run
                heatOn = 0;     //set initial state of fncTurnOnHeat() to 0
                c_monitor = monitor_run;
                timers.cmTmr
                    = (unsigned int)tCOLD_START.uiColdRunTime * TENTH_OF_HOUR;
            }
            break;
        case monitor_run: //4
            bCmRun = 1;     // Request that APU should run
            // If there is no higher priority
            if ( apuPriority( APU_ON_BY_COLDSTART ) )
                iconControl( ICON_COLDSTART, ICON_BLINK );
            else
    			iconControl( ICON_COLDSTART, ICON_ON );
                
            if (!bColdMonitor || !runable || !AUTO)
            {
                c_monitor = monitor_off;
            }
            else if (timers.cmTmr == 0)
            {
                c_monitor = monitor_standby;
                timers.cmTmr = COLD_CHECK_TIME;
            }
            else
            {
             	fncTurnOnHeat();
			}
            break;
        default:
            c_monitor = monitor_off;
            break;
    }
    return( temperatureCounter );
}

#endif
