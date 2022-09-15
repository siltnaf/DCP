/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file tempmon.c
       \brief Temperature Monitor Extended mode State Machine Module

<!-- DESCRIPTION: -->
    To provide the Temperature Monitor State Machine and support code.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/tempmon.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.4  2014/07/25 15:24:17  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.3  2014/04/16 14:07:10  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.2  2013/09/19 13:07:45  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.1  2009/10/14 15:32:06  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.3  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.2  2007-08-01 20:17:36  nallyn
          Modified commenting for Doxygen.

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.14  2007-06-01 17:44:20  msalimi
          Dale added comments.

          Revision 1.13  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline
          - we will subsequently place the multi-timer or 10-timer versions as
          branches off the main baseline so that we can continue multi-timer version
          development on a parallel branch. The multi-timer stuff was committed or
          merged with the main trunk originally - this was a mistake.

          Revision 1.12  2007/02/26 17:49:52  nallyn
          Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

          Revision 1.11  2006/02/08 20:24:09  nallyn
          Changes based on code review
          Eliminated magic numbers

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "tempmon.h"            // Interface to Temperature Monitor
#include "main.h"               // Common definitions
#include "apu.h"                // Interface to APU State Machine
#include "timers.h"             // Interface to software counter timers
#include "iconblinker.h"        // Interface to Icon definitions


/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

#define SETTLE_TIME         ( 0 )   // Time in minutes

/*
    The temperature as determined from the voltage is linear in regions.
    The voltage is read by the A to D (Vad). StartC and StartVad are the
    temperature and A to D reading at the start of the region.
    
    inter = (100 *( Vad - StartVad )) / factor
    Celsius = StartC + inter/10;
    Fahrenheit = ((StartC * 9) / 5) + ((inter * 9) / (5 * 10)) + 32;
*/

/* ---------- I n t e r n a l   D a t a ---------- */

#ifdef MM32F0133C7P			//? Add
//! Interpolation table for converting an AtoD reading to a Celsius temperature
struct tmzVadCnv tmhVadTable[] =
{
    // C, VAD,  Factor ( ( v2-v1) / (c2-c1 ) )
    {-40, 996, -20 },
    {-30, 976, -38 },
    {-20, 938, -62 },
    {-10, 876, -89 },
    { 2,  769, -111 },
    { 18, 592, -113 },
    { 28, 479, -105 },
    { 38, 374, -92 },
    { 46, 300, -79 },
    { 55, 229, -59 },
    { 65, 170, -39 }
};
#endif

/* ---------- G l o b a l   D a t a ---------- */

TM_CELSIUS tmgTMTemp;       //!< Temperature set point
TM_CELSIUS tmgTMRange;      //!< Temperature range set point
char tmgFSRange;            //!< Fan speed range
char tmgFSSelected;         //!< Fan speed selected
char tmgTMDuration;         //!< Duration
char tmgTMAccess;           //!< Do not require password
bit tmgTMEnable;            //!< TempMon is enabled by user (menu)
bit tmgTMActive;            //!< TempMon is active (enabled and toggle on)
bit tmgTMMode;              //!< TempMon Mode (enabled and duration not zero

bit tmgTempMonRun;          //!< TempMon wants APU to run
char tmgThermo;             //!< TempMon heating/cooling control

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfVtoCF -->
       \brief Convert a Voltage (AtoD reading) to a Celsius or Fahrenheit temperature.

<!-- PURPOSE: -->

<!-- NOTES: -->
     \note 
    The conversion is based on a linear interpolation of known points.  The
    number of entries in the interpolation table was largely based on the
    amount of code memory that could be allotted to it.

<!-- PARAMETERS: -->
       @param[in]  v       AtoD reading
       @param[in]  F       Fahrenheit (1) or Celsius (0)
<!-- RETURNS: -->
      \return Temperature in Celsius or Fahrenheit

*/
/******************************************************************************/
signed int tmfVtoCF( int v, char T )
{
    int i;
    TM_CELSIUS c;
    int inter;
	signed int f;
    
    for ( i = 0; i < (signed int) DIM( tmhVadTable ) - 1; i++ )
    {
        if ( v > tmhVadTable[ i + 1 ].tmxVad )
        {
            // Check is divisor would be 0
            if ( tmhVadTable[ i ].tmxFactor == 0 )
            {
                // Return invalid temperature
                return TM_CELSIUS_INVALID;
            }
                
            c = tmhVadTable[ i ].tmxC;
            inter = (100 * ( v - tmhVadTable[ i ].tmxVad )) / tmhVadTable[ i ].tmxFactor;
            if (T == CELSIUS)
            {   
                c += inter/10;
				return c;
            }
            else if (T == FAHRENHEIT)
            {
                f = ((c * 90) + (inter * 9)) / 50 + 32 ;    // Fahrenheit temperature
				return f;
            }
            // Return interpolated temperature
			return c;
        }
    }
    
    // Return invalid temperature
    return TM_CELSIUS_INVALID;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfInRange -->
       \brief Check if the measured temperature is close to the temperature set point.

<!-- PURPOSE: -->
    The thermostat field (heating/cooling control) determines the nature of
    the comparison:

    \code
        If SEARCHING then a temperature 2 degrees above or 2 degrees below the
        setpoint is out of range;
        If HEATING a temperature 1 degree above the setpoint is out of range;
        if COOLING a temperature 1 degree below the setpoint is out of range.
    \endcode
    
    A temperature out of range usually means that heating or cooling is
    required.

<!-- NOTES: -->
     \note 
    The SEARCHING and HEATING or COOLING rules mean that:

    \code
        if the temperature is 2 degrees below the setpoint then heating
        is requested until the temperature is 1 degree above the set point.
        
        if the temperature is 2 degrees above the setpoint then cooling
        is requested until the temperature is 1 degree below the set point.
    \endcode

    This is done to prevent a heating cycle from triggering a cooling cycle
    or a cooling cycle from triggering a heating cycle.

<!-- PARAMETERS: -->
       @param[in]  thermostat The heating/cooling control for the set point
       @param[in]  celsius    The temperature (Celsius) being sensed
       @param[in]  setpoint   The temperature (C) that is trying to be maintained

<!-- RETURNS: -->
      \return TRUE if the temperature is out of range, or FALSE if the temperature 
      is close enough to the set point.

*/
/******************************************************************************/
char tmfInRange( char thermostat, TM_CELSIUS celsius, TM_CELSIUS setpoint )
{
    switch ( thermostat )
    {
        case TM_HEATING:
            // If above test point
            if ( celsius >= setpoint + 1 )
                // Heating not needed
                return FALSE;
            else
                // Heating needed
                return TRUE;
            
        case TM_COOLING:
            // If below test point
            if ( celsius <= setpoint - 1 )
                // Cooling not needed
                return FALSE;
            else
                // Cooling needed
                return TRUE;
        
        case TM_SEARCHING:
        default:
            if ( celsius >= setpoint + 2 )
                // Too high
                return FALSE;
            else if ( celsius <= setpoint - 2 )
                // Too low
                return FALSE;
            else
                // In range
                return TRUE;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfThermostat -->
       \brief Determine if heating or cooling is required

<!-- PURPOSE: -->
    This function reads the temperature input AtoD and debounces (time filters)
    the input and determines if heating or cooling should be provided.

<!-- PARAMETERS: -->
       @param[in]  thermo   The previous heating/cooling control
       @param[in]  setpoint A temperature (Celsius) setting to be maintained
       @param[in]  pCounter A pointer to a tick counter used for debouncing

<!-- RETURNS: -->
      \return a heating/cooling control value, one of:
      \code
        TM_HEATING if heat is required,
        TM_COOLING is cooling is required,
        TM_SEARCHING if neither heat or cooling is required.
      \endcode


*/
/******************************************************************************/
char tmfThermostat( char thermo, TM_CELSIUS setpoint, unsigned char *pCounter )
{
    TM_CELSIUS celsius;
    unsigned char counter;
    
    // Get local copy of counter
    counter = *pCounter;

    // Grab the temperature and convert to celsius
    celsius = (TM_CELSIUS)tmfVtoCF( TEMPERATURE, CELSIUS );
    
    // If temperature conversion fails
    if ( celsius == TM_CELSIUS_INVALID )
    {
        // Go to a non active state
        return TM_SEARCHING;
    }

    // If in range
    if ( tmfInRange( thermo, celsius, setpoint ) )
    {
        // count down
        if ( counter > 0 )
            counter--;
    }
    else
    {
        // count up
        if ( counter < 255 )
            counter++;
    }
    
    // If out of range too long, actually not needed heating (nor cooling)
    if ( counter > 240 )
    {
        switch ( thermo )
        {
            case TM_HEATING:
                // Start temperature settle timer and change state
                timers.settleTmr = SETTLE_TIME;  // minutes
                thermo = TM_SEARCHING;
                counter = 0;
                break;
            
            case TM_COOLING:
                // Start temperature settle timer and change state
                timers.settleTmr = SETTLE_TIME;  // minutes
                thermo = TM_SEARCHING;
                counter = 0;
                break;
        
            case TM_SEARCHING:
            default:
                if ( timers.settleTmr == 0 )
                {
                    if ( celsius > setpoint )
                        thermo = TM_COOLING;
                    else
                        thermo = TM_HEATING;
                        
                    // Start with fresh counter after state change
                    counter = 0;
                }
                break;
        }
        
    }

    // Pass back counter value
    *pCounter = counter;
    return thermo;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfSetMode -->
       \brief Set temperature monitor mode

<!-- PURPOSE: -->
    This function sets the temperature monitoring mode flag and the active
    flag depending on if the feature is enabled and the duration setting.

<!-- PARAMETERS: none-->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void tmfSetMode( void )
{
    char ModeSave;
    
    ModeSave = tmgTMMode;
    
    // TM Mode is when enabled and duration is not zero
    // Set global mode flag
    tmgTMMode = tmgTMEnable && ( tmgTMDuration != 0 );
    
    // If not TM Mode
    if ( ! tmgTMMode )
    {
        // Active is same as enable (as before)
        tmgTMActive = tmgTMEnable;
    }
    // If mode has changed    
    else if ( tmgTMMode != ModeSave )
    {
        tmgTMActive = FALSE;
        timers.tmRunTmr = 0;
    }
}

#ifndef EXTRA_TIMERS
/******************************************************************************/
/*!

<!-- FUNCTION NAME: tmfTempMon -->
       \brief This is the temperature monitor state machine to be called from the main loop.

<!-- PURPOSE: -->
    This function checks the temperature against the set point to deterimine
    if heating or cooling is required and will change the temperature monitor
    state machine to coordinate the running of the APU. This function also 
    controls the Temperature Monitor icon.

<!-- NOTES: -->
     \note 
     \code
    The temperature monitor state machine is made to look similar to the
    battery monitor and cold start state machines.
    The states are:
        off      - not enabled by the user (menu)
        on       - enabled by user but APU is not runable
        standby  - ready to run but temperature is not out of range
        starting - request that the APU start
        run      - wait for temperature to reach set point or some other
                  reason to give up APU control
    
    
    The APU is runable if the the APU is not locked out, the APU is in
    AUTO mode (not run by user), and the breakers are not tripped.

    tmfTempMon sets tmgTempMonRun to indicate that the temperature monitor 
    wants the APU to run.

    tmfTempMon sets tmgThermo to indicate that the temperature monitor 
    wants the CCU to provide heating or cooling.
     \endcode

<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void tmfTempMon( void )
{
    static unsigned char temperatureCounter = 0;
    static enum monitor_state t_monitor = monitor_off;
    char iconCtrl;
    char runable;

    // Test the heating/cooling control
    tmgThermo = tmfThermostat( tmgThermo, tmgTMTemp, &temperatureCounter );
    
    runable = ( !apuFaultLockout && AUTO && !BREAKER_TRIP );
    
    // Default to Icon On
    iconCtrl = ICON_ON;
        
    // State machine for handling the temperature monitor process
    switch ( t_monitor )
    {
        case monitor_off: //0
            // Tempmon should not be asking APU to run
            tmgTempMonRun = FALSE;
            apuClearMinRun();
            // If the monitor process is enabled by the user...            
            if ( tmgTMActive )
            {
                t_monitor = monitor_on; //...goto monitor on state
            }
            iconCtrl = ICON_OFF;
            break;
            
        case monitor_on: //1
            // Tempmon should not be asking APU to run
            tmgTempMonRun = FALSE;
            // If the monitor process is disabled by the user...            
            if ( !tmgTMActive )
            {
                t_monitor = monitor_off; //...next state monitor_off state
            }
            else if ( runable )
            {
                t_monitor = monitor_standby;    // next state monitor standby
            }
            break;
        case monitor_standby: //2
            // Tempmon should not be asking APU to run
            tmgTempMonRun = FALSE;
            if ( !tmgTMActive || !runable )
            {
                t_monitor = monitor_off;
            }
            else
            {
                // If minimum run is active
                if ( apuPriority( APU_ON_BY_MINIMUM_RUN ) )
                {
                    // Icon should be blinking
                    iconCtrl = ICON_BLINK_SLOW;
                }
            
                // If thermo control is active
                //if ( tmgThermo != TM_SEARCHING )
				if ( tmgThermo == TM_COOLING || ((tmgThermo == TM_HEATING) && HEAT_ENABLE))
                {
                    // Start up
                    t_monitor = monitor_starting;
                }
            }
            break;
        case monitor_starting: //3
            // Tempmon is active
            tmgTempMonRun = TRUE;
            t_monitor = monitor_run;
            break;
        case monitor_run: //4
            // If there is no higher priority
            if ( apuPriority( APU_ON_BY_TEMPMON ) )
                iconCtrl = ICON_BLINK;

            if (!tmgTMActive || !runable || !AUTO)
            {
                // Should be off
                t_monitor = monitor_off;
            }
            // If thermo control is not active
            else if ( tmgThermo == TM_SEARCHING )
            {
                // Go back to standby
                t_monitor = monitor_standby;
            }
            break;
        default:
            t_monitor = monitor_off;
            break;
    }
    
    // Call to control the icon
    iconControl( ICON_TEMPMON, iconCtrl );
}
#endif // EXTRA_TIMERS
