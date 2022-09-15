/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file battmon.c
       \brief Battery voltage monitor 

<!-- DESCRIPTION: -->
    Monitor the battery voltage and start the APU if required.

<!-- NOTES: -->
       \note
       \code
           State Machine for battery monitoring process
           can assume one of three states:
              1: bm_off - battery monitor off
              2: bm_standby - battery monitor enabled, apu off
              3: bm_run - battery monitor enabled, apu on
           external variables:
           monitor : variable determined in menu state machine
           for enabling and disabling Battery monitor
           BATTERY_VOLTS : flag indicating that the voltage is
           low:
             1 = low voltage
             0 = normal voltage
           internal variables:
           bmTimeOut : associated with bmSTimer() to determine
           the length of time to run the APU
       \endcode
       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/battmon.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
    
       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.3  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.2  2007-08-01 19:43:53  nallyn
       Added commenting for Doxygen

       Revision 1.1  2007-07-05 20:10:12  gfinlay
       Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

       Revision 1.15  2007-06-01 17:43:03  msalimi
       Dale added comments.

       Revision 1.14  2007/04/03 18:44:42  gfinlay
       Placing ES35-000-04A files back at the head of the main truck or baseline
       - we will subsequently place the multi-timer or 10-timer versions as
       branches off the main baseline so that we can continue multi-timer version
       development on a parallel branch. The multi-timer stuff was committed or
       merged with the main trunk originally - this was a mistake.

       Revision 1.13  2007/02/26 17:49:52  nallyn
       Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

       Revision 1.12  2006/02/08 20:10:28  nallyn
       Changes based on code review
       Changed the way the battery voltage is read and decoded

       Revision 1.11  2006/02/08 19:39:08  nallyn
       Review Code with updated file headers
       Revision 1.10  2006/01/24 16:51:25  nallyn
       Added voltage calibration

       Revision 1.9  2005/12/20 18:04:46  nallyn
       November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "battmon.h"            // Interface to Battery Voltage Monitor
#include "main.h"               // Common definitions
#include "apu.h"                // Interface to APU State Machine
#include "lcd_grp1.h"           // Interface to LCD
#include "fonts.h"              // Interface to LCD fonts
#include "timers.h"             // Interface to software counter timers
#include "iconblinker.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
#ifndef EXTRA_TIMERS

#define CHARGE_RUN_TIME      5399   //!<seconds to run for battery charge during battery monitor
#define VOLTAGE_CHECK_TIME   599    //!<seconds to check battery

//!Possible states for the battery voltage
enum monitorState
{
    msNormal,   //!< Battery voltage is normal
    msLow       //!< Battery voltage is low
};

/* ---------- I n t e r n a l   D a t a ---------- */

/* ---------- G l o b a l   D a t a ---------- */

char bmgBMAccess;           //!< enable battery monitor user access
bit BM_RUN;                 //!< battery monitor variable
int BMON_VOLTAGE;           //!< (118-4)*103/24;//479;
bit volt_monitor;           //!< Enable battery monitor
char bmgVoltAccess;         //!< Enable voltage adjust user access
signed char bmgVOffset;     //!< Voltage Offset


/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: battery_voltage -->
       \brief Function to scale AtoD reading of the battery to a number representing
        the battery voltage in tenths of volts.
        
<!-- PURPOSE: -->
    This function has a configurable compensation to account for diode drops 
    and other losses.
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Battery voltage in tenths of volts. 

*/
/******************************************************************************/
int battery_voltage( void )
{
    return ( ( ( BATTERY_VOLTAGE * 24 ) / 100 ) + bmgVOffset );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: battery_low -->
       \brief Compare battery voltage against the battery monitor set point.

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return TRUE if the battery voltage is below the set point, or
      FALSE if the battery voltage is not low

*/
/******************************************************************************/
static bit battery_low( void )
{
    if ( battery_voltage() < BMON_VOLTAGE )
        return TRUE;
    else
        return FALSE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: battery_monitor -->
    \brief Battery voltage Monitor state machine to be called from the main loop.

<!-- PURPOSE: -->
    This function debounces and checks if the battery voltage is too low and
    will attempt to start and run the APU to recharge the battery.
    This function also controls the Battery Monitor icon.

<!-- NOTES: -->
    \note 
    \code
    Sets BM_RUN to indicate that battery monitor wants the APU to run.
    The Battery Monitor states are:
        off      - not enabled by the user (menu)
        on       - enabled by user
        standby  - ready to run but battery voltage is normal or has not
                 been low for more than the check time
        starting - request that the APU start and set run time
        run      - wait for timer to expire or some other reason to give up APU control
    \endcode

<!-- PARAMETERS: none -->
<!-- RETURNS: -->
    \return Void

*/
/******************************************************************************/

void battery_monitor(void)
{
    static char lowVoltCounter = 0;                     // Filter input counter
    static enum monitor_state b_monitor = monitor_off;  // State variable
    static enum monitorState voltageState = msNormal;   // Battery Normal of Low

    // If the counter is not at maximum and the battery is low
    if ( ( lowVoltCounter < 0xff ) && battery_low() )
    {
        // Count up the timer to debounce (time filter) it
        lowVoltCounter += 1;
    }
    // If the counter is not at minimum and the battery is not low
    else if ( ( lowVoltCounter > 0 ) && !battery_low() )
    {
        // Count down the timer to debounce (time filter) it
        lowVoltCounter -= 1;
    }

    switch (b_monitor)
    {
        case monitor_off: //0
            BM_RUN = 0;
            iconControl( ICON_BATTERY, ICON_OFF );
            if (volt_monitor) //If the monitor process is enabled by the user...
            {
                b_monitor = monitor_on; //...goto bm_on state
            }
            break;
        case monitor_on: //1
            iconControl( ICON_BATTERY, ICON_ON );   //Display the battery icon
            
            //If the monitor process is disabled by the user...
            if (!volt_monitor)
            {
                b_monitor = monitor_off; //...goto bm_off state
            }
            else
            {
                timers.bmTmr = VOLTAGE_CHECK_TIME; //bmSTimer(VOLTAGE_CHECK_TIME);
                b_monitor = monitor_standby;
            }
            break;
        case monitor_standby: //2
            BM_RUN = 0;
            //If the monitor process is disabled by the user...
            if (!volt_monitor)
            {
                b_monitor = monitor_off;
            }
            else
            {
                switch (voltageState)
                {
                    case msNormal:
                        iconControl( ICON_BATTERY, ICON_ON );
                        if (lowVoltCounter > 200)
                        {
                            timers.bmTmr = VOLTAGE_CHECK_TIME;
                            voltageState = msLow;
                        }
                        break;
                    case msLow:
                        iconControl( ICON_BATTERY, ICON_BLINK_ASYMETRIC_ON );
                        if (lowVoltCounter < 10)
                        {
                            voltageState = msNormal;
                        }
                        else if (!timers.bmTmr)
                        {
                            b_monitor = monitor_starting;
                            timers.bmTmr = 5;
                        }
                        break;
                }
            }
            break;
        case monitor_starting: //3
            iconControl( ICON_BATTERY, ICON_BLINK_FAST );
            //If the monitor process is disabled by the user...
            if (!volt_monitor)
            {
                b_monitor = monitor_off;
            }
            else if (lowVoltCounter < 10)
            {
                b_monitor = monitor_standby;
            }
            else if (!timers.bmTmr)
            {
                BM_RUN = 1;
                timers.bmTmr = CHARGE_RUN_TIME;
                b_monitor = monitor_run;
            }
            break;
        case monitor_run: //4
            BM_RUN = 1;
            // If there is no higher priority
            if ( apuPriority( APU_ON_BY_BATTMON ) )
                iconControl( ICON_BATTERY, ICON_BLINK );
            else
                iconControl( ICON_BATTERY, ICON_ON );
                
            //If the monitor process is disabled by the user
            // or switched out of AUTO mode
            if (!volt_monitor || !AUTO)
            {
                b_monitor = monitor_off;
            }
            else if (timers.bmTmr == 0)
            {
                b_monitor = monitor_standby;
                timers.bmTmr = VOLTAGE_CHECK_TIME;
            }
            break;
        default:
            b_monitor = monitor_off;
            break;
    }
}

#endif // EXTRA_TIMERS
