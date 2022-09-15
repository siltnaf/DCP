/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file nvmem.h
       \brief Header file defining interface to Non-Volatile Memory

<!-- DESCRIPTION: -->
    To provide the interface to Non-Volatile Memory 

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/nvmem.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.4  2014/07/25 15:24:17  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.3  2014/04/16 14:07:09  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.2  2013/09/19 13:07:45  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.1  2009/07/31 17:18:40  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.8  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.7  2008-07-14 16:52:42  msalimi
          Implemented a new feature that displays a warning when the APU has been running without DPF regeneration for more than 50 hours. No other action is taken.

          Revision 1.6  2007-12-21 20:36:44  msalimi
          - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
          - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
          - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
          - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

          Revision 1.5  2007-11-30 18:52:08  msalimi
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

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:15:57  nallyn
          Modified comments for Doxygen.

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.15  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.

          Revision 1.14  2007/02/27 01:20:17  nallyn
          Added 4 more timers for a total of 10 timers. The timers function differently
          than if only 4 timers are enabled. The definition of EXTRA_TIMERS causes the
          timers to enable High AC, Low AC, HVAC Off, Low Heat, or High Heat
          depending on the temperture selected:
          18-20: High AC
          21-23: Low AC
          24: HVAC Off
          25-27: Low Heat
          28-30: High Heat

          Revision 1.13  2007/02/26 17:49:52  nallyn
          Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

          Revision 1.12  2007/02/23 18:11:28  nallyn
          Changed from 2 timers to 4 timers for testing purposes

          Revision 1.11  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.10  2006/01/24 16:51:25  nallyn
          Added voltage calibration

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005
       
       \endif
*/
/******************************************************************************/

#ifndef NVMEM_H
#define NVMEM_H

//?#include <pic18.h>              // common include file
#include "timekeep.h"           // Interface for clock time keeping

/* ---------- D e f i n i t i o n s ---------- */

#define NV_EE_FIRST_ADDR        32              // first bytes used elsewhere
#define NV_EE_STORE_MULT        3               // we triple store bytes

#define EEPROM_SIZE 1024

#define NV_EE_LAST_ADDR         ( EEPROM_SIZE - 1 )
#define NV_EE_USABLE            ( NV_EE_LAST_ADDR - NV_EE_FIRST_ADDR )
#define NV_LAST_ID              ( NV_EE_USABLE / NV_EE_STORE_MULT )

//!Identifiers for storing NV data;
typedef enum
{
    
    NV_ID_BM_ENABLE = 0,
#ifndef EXTRA_TIMERS
    NV_ID_BM_VOLTAGE,
    NV_ID_BM_V_OFFSET,
#endif // EXTRA_TIMERS
    NV_ID_CS_ENABLE,
    NV_ID_CS_TEMP,
    NV_ID_CS_TIME,
#ifndef EXTRA_TIMERS
    NV_ID_TM_ENABLE,
    NV_ID_TM_TEMP,
#endif // EXTRA_TIMERS
    NV_ID_MR_ENABLE,
    NV_ID_MR_TIME,
    NV_ID_TIMER1_ENABLE,
    NV_ID_TIMER1_DUR,
    NV_ID_TIMER1_TEMP,
    NV_ID_TIMER1_TIME,              // day of week
    NV_ID_TIMER1_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER1_TIME_MINUTE_BYTE,  // minutes
    NV_ID_TIMER2_ENABLE,
    NV_ID_TIMER2_DUR,
    NV_ID_TIMER2_TEMP,
    NV_ID_TIMER2_TIME,              // day of week
    NV_ID_TIMER2_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER2_TIME_MINUTE_BYTE,  // minutes
#ifndef USE_COLDSTART
    NV_ID_TIMER3_ENABLE,
    NV_ID_TIMER3_DUR,
    NV_ID_TIMER3_TEMP,
    NV_ID_TIMER3_TIME,              // day of week
    NV_ID_TIMER3_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER3_TIME_MINUTE_BYTE,  // minutes
    NV_ID_TIMER4_ENABLE,
    NV_ID_TIMER4_DUR,
    NV_ID_TIMER4_TEMP,
    NV_ID_TIMER4_TIME,              // day of week
    NV_ID_TIMER4_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER4_TIME_MINUTE_BYTE,  // minutes
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    NV_ID_TIMER5_ENABLE,
    NV_ID_TIMER5_DUR,
    NV_ID_TIMER5_TEMP,
    NV_ID_TIMER5_TIME,              // day of week
    NV_ID_TIMER5_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER5_TIME_MINUTE_BYTE,  // minutes
    NV_ID_TIMER6_ENABLE,
    NV_ID_TIMER6_DUR,
    NV_ID_TIMER6_TEMP,
    NV_ID_TIMER6_TIME,              // day of week
    NV_ID_TIMER6_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER6_TIME_MINUTE_BYTE,  // minutes
    NV_ID_TIMER7_ENABLE,
    NV_ID_TIMER7_DUR,
    NV_ID_TIMER7_TEMP,
    NV_ID_TIMER7_TIME,              // day of week
    NV_ID_TIMER7_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER7_TIME_MINUTE_BYTE,  // minutes
    
    NV_ID_TIMER8_ENABLE,
    NV_ID_TIMER8_DUR,
    NV_ID_TIMER8_TEMP,
    NV_ID_TIMER8_TIME,              // day of week
    NV_ID_TIMER8_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER8_TIME_MINUTE_BYTE,  // minutes
    
    NV_ID_TIMER9_ENABLE,
    NV_ID_TIMER9_DUR,
    NV_ID_TIMER9_TEMP,
    NV_ID_TIMER9_TIME,              // day of week
    NV_ID_TIMER9_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER9_TIME_MINUTE_BYTE,  // minutes
    
    NV_ID_TIMER10_ENABLE,
    NV_ID_TIMER10_DUR,
    NV_ID_TIMER10_TEMP,
    NV_ID_TIMER10_TIME,              // day of week
    NV_ID_TIMER10_TIME_HOUR_BYTE,    // hours
    NV_ID_TIMER10_TIME_MINUTE_BYTE,  // minutes
#else
    NV_ID_BM_ACCESS,
    NV_ID_TM_ACCESS,
#endif // EXTRA_TIMERS
    NV_ID_VOLT_ACCESS,
    NV_ID_CS_ACCESS,
    
    NV_ID_MR_ACCESS,
    NV_ID_TIMER1_ACCESS,
    NV_ID_TIMER2_ACCESS,
#ifndef USE_COLDSTART
    NV_ID_TIMER3_ACCESS,
    NV_ID_TIMER4_ACCESS,
#endif // USE_COLDSTART
#ifdef EXTRA_TIMERS
    NV_ID_TIMER5_ACCESS,
    NV_ID_TIMER6_ACCESS,
    NV_ID_TIMER7_ACCESS,
    NV_ID_TIMER8_ACCESS,
    NV_ID_TIMER9_ACCESS,
    NV_ID_TIMER10_ACCESS,
#endif // EXTRA_TIMERS

    NV_ID_DIAG_ACCESS,
    NV_ID_SYS_CFG_ACCESS,
    NV_ID_SPM_ACCESS,
    NV_ID_DPF_ACCESS,
    NV_ID_CLOCK_ACCESS,
    NV_ID_CLOCK_MODE,
    NV_ID_PASS_BYTE1,
    NV_ID_PASS_BYTE2,
    NV_ID_DCP_CONFIG,
    NV_ID_APU_START_TOTAL,
    NV_ID_APU_NO_REGEN_HOUR,
    NV_ID_TM_DURATION,
    NV_ID_SETUP,
    NV_ID_TEMP_RANGE,			// temperature range in comfort monitor
    NV_ID_FAN_SPEED_RANGE,		// fan speed range in comfort monitor
    NV_ID_HEAT_ENABLE,
    NV_ID_LAST_ID = NV_LAST_ID  // this must be last to avoid too many IDs
};

//! Typedefs for NV data types in case they change
typedef char NV_ID;                 //!< NVMEM Identfier
typedef char NV_BOOL;               //!< Boolean return code
typedef char NV_BYTE;               //!< Basic storage data type
typedef short NV_WORD;              //!< 2 byte storage data type

    
/* ---------- G l o b a l   D a t a ---------- */


/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

signed char nvfGetTemperature( NV_ID id, signed char deflt );
char nvfPutTemperature( NV_ID id, signed char value );
unsigned char nvfGetVoltage( NV_ID id, unsigned char deflt );
char nvfPutVoltage( NV_ID id, unsigned char value );
unsigned char nvfGetNumber( NV_ID id, unsigned char deflt );
char nvfPutNumber( NV_ID id, unsigned char value );
NV_BOOL nvfPutTime( NV_ID id, TKZTIME time );
void nvfGetTime( NV_ID id,
                 TKZTIME *time, 
                 char day, 
                 char hour, 
                 char minute );
                 
#endif  // NVMEM_H
