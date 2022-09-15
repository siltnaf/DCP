/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file defaults.h
       \brief Header file which provides defines for configuration defaults

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/defaults.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

 * 
 * RTC_75512 APU Driver control panel - increase password length to 4 digits, November 13, 2019, Hannon
 *  TFX_PASSWORD  to 8068  - Initial Carrier Revision.
 * 
          $Log: not supported by cvs2svn $
          Revision 1.4  2014/07/25 15:24:15  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.3  2014/04/16 14:07:08  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.2  2013/09/19 13:07:43  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.1  2010/03/11 19:30:37  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    

            Revision 2.0  2010-03-09   echu
          - Changed DEFAULT_TEMP_MON_DURATION to 8 hours from 0 hours so that the new CM mods are in effect.	      

          Revision 1.9  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.8  2008-02-06 16:29:56  msalimi
          - Corrected low voltage SPN. This was 5 and now is 4.

          Revision 1.7  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.6  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.5  2007-10-17 18:35:36  msalimi
          Set all timers on by default.

          Revision 1.4  2007-10-15 16:49:54  msalimi
          Changed timer defaults to ON for extra timers version.

          Revision 1.3  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.2  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.7  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.
    
              Revision 1.6  2007/02/27 01:20:17  nallyn
              Added 4 more timers for a total of 10 timers. The timers function differently
          than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
          timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
          depending on the temperture selected:
          18-20: High AC
          21-23: Low AC
          24: HVAC Off
          25-27: Low Heat
          28-30: High Heat
    
    Revision 1.5  2007/02/26 17:24:46  nallyn
    Added defaults for timer times and type (12 vs 24)
    
    Revision 1.4  2007/02/23 18:11:28  nallyn
    Changed from 2 timers to 4 timers for testing purposes
    
          Revision 1.3  2006/02/09 22:09:12  nallyn
          Updated the default values for clock and voltage calibration access
    
          Revision 1.2  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers
    
       \endif
*/
/******************************************************************************/

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include "timekeep.h"

#define VOLTS( n )	( (int)(n * 10) )
#define HOURS( n )	( (int)(n * 10) )
#define DEGREES_C( c )	( c )
#define DEGREES_F( c )	((int)( ( ( c * 9 ) / 5 ) + 32 ))

// Password protection/User accessable
#define TFX_PASSWORD        8068 

    #define DEFAULT_PASSWORD    		000
    #define DEFAULT_CLOCK_ACCESS		ON
    #define DEFAULT_24HR_CLOCK          ON
    
    #define DEFAULT_BATT_MON_VOLTS		VOLTS( 11.8 )
    #define DEFAULT_BATT_MON_ENABLE		OFF
    #define DEFAULT_BATT_MON_ACCESS		ON
    #define DEFAULT_VOLT_CAL_ACCESS		ON
    
    #define DEFAULT_SYS_CFG_ACCESS      ON
    #define DEFAULT_DIAG_ACCESS         ON
    #define DEFAULT_SPM_ACCESS          ON
    #define DEFAULT_DPF_ACCESS          ON

    #define DEFAULT_COLD_START_TEMP		DEGREES_C( -10 )
    #define DEFAULT_COLD_START_TIME		HOURS( 2.0 )
    #define DEFAULT_COLD_START_ENABLE	OFF
    #define DEFAULT_COLD_START_ACCESS	ON

    #define DEFAULT_MAX_RUN_TIME		HOURS( 8.0 )
    #define DEFAULT_MAX_RUN_ENABLE		OFF
    #define DEFAULT_MAX_RUN_ACCESS		ON

    #define DEFAULT_TEMP_MON_TEMP		DEGREES_C( 21 )
    #define DEFAULT_TEMP_MON_RANGE		DEGREES_C( 0 )
    #define DEFAULT_FAN_SPEED_RANGE		FAN_SPEED_HIGH_ONLY
    #define DEFAULT_TEMP_MON_DURATION	HOURS( 8.0 )
    #define DEFAULT_TEMP_MON_ENABLE		OFF
    #define DEFAULT_TEMP_MON_ACCESS		ON

    #define DEFAULT_TIMER1_TEMP         DEGREES_C( 26 )
    #define DEFAULT_TIMER1_DURATION     HOURS( 2.0 )
    #define DEFAULT_HEAT_ENABLE   	    ON
#ifdef EXTRA_TIMERS
    #define DEFAULT_TIMER1_ENABLE       ON
#else // EXTRA_TIMERS
    #define DEFAULT_TIMER1_ENABLE       OFF
#endif // EXTRA_TIMERS
    #define DEFAULT_TIMER1_ACCESS		ON
    #define DEFAULT_TIMER1_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER1_HOUR         0
    #define DEFAULT_TIMER1_MINUTE       0

    #define DEFAULT_TIMER2_TEMP         DEGREES_C( 29 )
    #define DEFAULT_TIMER2_DURATION     HOURS( 2.0 )
#ifdef EXTRA_TIMERS
    #define DEFAULT_TIMER2_ENABLE       ON
#else // EXTRA_TIMERS
    #define DEFAULT_TIMER2_ENABLE       OFF
#endif // EXTRA_TIMERS
    #define DEFAULT_TIMER2_ACCESS		ON
    #define DEFAULT_TIMER2_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER2_HOUR         1
    #define DEFAULT_TIMER2_MINUTE       59
    
    #define DEFAULT_TIMER3_TEMP         DEGREES_C( 26 )
    #define DEFAULT_TIMER3_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER3_ENABLE       ON
    #define DEFAULT_TIMER3_ACCESS       ON
    #define DEFAULT_TIMER3_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER3_HOUR         4
    #define DEFAULT_TIMER3_MINUTE       48
    
    #define DEFAULT_TIMER4_TEMP         DEGREES_C( 29 )
    #define DEFAULT_TIMER4_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER4_ENABLE       ON
    #define DEFAULT_TIMER4_ACCESS       ON
    #define DEFAULT_TIMER4_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER4_HOUR         6
    #define DEFAULT_TIMER4_MINUTE       47
#ifdef EXTRA_TIMERS
    #define DEFAULT_TIMER5_TEMP         DEGREES_C( 26 )
    #define DEFAULT_TIMER5_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER5_ENABLE       ON
    #define DEFAULT_TIMER5_ACCESS       ON
    #define DEFAULT_TIMER5_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER5_HOUR         9
    #define DEFAULT_TIMER5_MINUTE       36
    
    #define DEFAULT_TIMER6_TEMP         DEGREES_C( 29 )
    #define DEFAULT_TIMER6_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER6_ENABLE       ON
    #define DEFAULT_TIMER6_ACCESS       ON
    #define DEFAULT_TIMER6_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER6_HOUR         11
    #define DEFAULT_TIMER6_MINUTE       35

    #define DEFAULT_TIMER7_TEMP         DEGREES_C( 26 )
    #define DEFAULT_TIMER7_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER7_ENABLE       ON
    #define DEFAULT_TIMER7_ACCESS       ON
    #define DEFAULT_TIMER7_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER7_HOUR         14
    #define DEFAULT_TIMER7_MINUTE       24

    #define DEFAULT_TIMER8_TEMP         DEGREES_C( 29 )
    #define DEFAULT_TIMER8_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER8_ENABLE       ON
    #define DEFAULT_TIMER8_ACCESS       ON
    #define DEFAULT_TIMER8_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER8_HOUR         16
    #define DEFAULT_TIMER8_MINUTE       23
    
    #define DEFAULT_TIMER9_TEMP         DEGREES_C( 26 )
    #define DEFAULT_TIMER9_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER9_ENABLE       ON
    #define DEFAULT_TIMER9_ACCESS       ON
    #define DEFAULT_TIMER9_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER9_HOUR         19
    #define DEFAULT_TIMER9_MINUTE       12
    
    #define DEFAULT_TIMER10_TEMP         DEGREES_C( 29 )
    #define DEFAULT_TIMER10_DURATION     HOURS( 2.0 )
    #define DEFAULT_TIMER10_ENABLE       ON
    #define DEFAULT_TIMER10_ACCESS       ON
    #define DEFAULT_TIMER10_DAY          TK_EVERYDAY
    #define DEFAULT_TIMER10_HOUR         21
    #define DEFAULT_TIMER10_MINUTE       11
#endif // EXTRA_TIMERS  

#endif //DEFAULTS_H
