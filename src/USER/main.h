/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file main.h
       \brief Header file for DCP driver control panel application main entry point routine

<!-- DESCRIPTION: -->
<!-- NOTES: -->

       \if ShowHist
       Copyright � 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/main.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/07/25 15:24:16  blepojevic
	      Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection
	
	      Revision 1.2  2014/04/16 14:07:09  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.1  2009/07/31 17:00:06  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.12  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.11  2007-12-21 20:36:44  msalimi
	      - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
	      - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
	      - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
	      - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

	      Revision 1.10  2007-11-30 18:52:08  msalimi
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

	      Revision 1.9  2007-10-26 18:42:14  msalimi
	      Added 5 minute cooldown for APU after DPF regeneration.
	      Changed DPF variable menu to show all variables in one screen.
	      Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
	      Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

	      Revision 1.8  2007-10-23 19:03:57  msalimi
	      Corrected spelling error.

	      Revision 1.7  2007-10-15 16:57:08  msalimi
	      Added variable to hold the memory error.

	      Revision 1.6  2007-10-09 15:03:18  msalimi
	      Version 35-PPP-002. Second round of testing..

	      Revision 1.5  2007-09-21 20:52:21  msalimi
	      *** empty log message ***

	      Revision 1.4  2007-09-17 18:41:35  msalimi
	      DPF related modifications

	      Revision 1.3  2007-08-27 22:19:08  msalimi
	      CAN communication working.

	      Revision 1.2  2007-08-01 20:10:35  nallyn
	      Commented out unused code. Added comments for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.12  2006/02/08 20:19:39  nallyn
	      Changes based on code review.
	      removed unused code
	
	      Revision 1.11  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/


#ifndef MAIN_H
#define MAIN_H
//?#include <pic18.h>
#include "tfxstddef.h"
#include "uart.h"
#include "delay.h"

/* ---------- D e f i n i t i o n s ------------------------ */
#define NO_DATA			0xFF
#define NO_DATA_WORD	0xFFFF

#define VOLTAGE_OFFSET   8

#define MENU_PAUSE      120// 30    changed to two minutes

// Hours are sometimes kept in decimal tenths of hours
// Convert decimal hours to minutes
#define HOUR_TENTHS_TO_MINUTES( x ) ( ( x ) * 6 )

// Get dimension of array
// Divide total size of array in bytes by size of an array element
#define DIM( x )        ( sizeof( x ) / sizeof( x[0] ) )

// constants for unsigned char DCP_Configuration
#define DPF_INSTALLED		0X01
//#define FREE				0X02
//#define FREE				0X04
//#define FREE				0X08
//#define FREE				0X10
//#define FREE				0X20
//#define FREE				0X40
//#define FREE				0X80

typedef struct
{
	unsigned char EXT_Memory_Fault		:1;
} DCP_DIAG;

// Version string
extern const char version[];

/******************************************************
    I/O Definitions
*******************************************************/

#define   PORTBIT(adr, bit) ((unsigned)(&adr)*8+(bit))

/*** ***analog inputs*** ***/
extern volatile int BATTERY_VOLTAGE;
extern volatile int AC_BR1_BR2_SERV;    // PORTA 0
extern volatile int OIL_OH_WAIT;        // PORTA 1
//bank1 volatile int PWR_MONITOR;       // PORTA 2
extern volatile int TEMPERATURE;        // PORTA 3
//unused analog                         // PORTA 5

/*** ***Digital I/O*** ***/
//backlight - see lcd_grph.h            // PORTA 4
//LCD - CS1B - see lcd_grph.h           // PORTB 0
//extern bit R3; //Keypad rows and columns.  PORTB 2
//extern bit R2;                          // PORTB 3
//extern bit R1;                          // PORTB 4
//extern bit C3;                          // PORTB 5
//extern bit C2;                          // PORTB 6
//extern bit C1;                          // PORTB 7

//extern bit buzzer;
//LCD - RD - see lcd_grph.h             // PORTC 0
//extern bit HEAT_AC;
//extern bit FAN_LOW_HIGH;

//SCL - see i2c.h                       // PORTC 3
//SDA - see i2c.h                       // PORTC 4
//LCD - RS - see lcd_grph.h             // PORTC 5
//unused digital                        // PORTC 6
//LCD - /RST - see lcd_grph.h           // PORTC 7
//LCD - DATA - see lcd_grph.h           // PORTD 0-7
//LCD - /WR - see lcd_grph.h            // PORTE 0


/******************************************************
    Global Variable Definitions
*******************************************************/

/**** Control Panel Variables ****/
extern char cool_timer; // counter for air conditioning
extern bit key_pressed;
extern unsigned int AMBIENT_TEMP;
#ifndef MM32F0133C7P		//? Add
extern bank1 unsigned int DECODE_TEMP;
extern bank1 unsigned char TEMP;
#else
extern unsigned int DECODE_TEMP;
extern unsigned char TEMP;
#endif
extern bit HVAC_OFF_BTN;
extern bit ON_OFF_BTN;
extern bit START_BTN;

extern bit BREAKER_TRIP;
extern bit AC_BTN;
extern bit HEAT_BTN;
extern bit OUTLET;
extern bit SHORE_POWER;

extern bit FAN_BTN;
extern bit TEMP_UP_BTN;
extern bit TEMP_DOWN_BTN;
extern bit BRESET_BTN;
extern bit RIGHT_BTN;
extern bit LEFT_BTN;
extern bit MENU_BTN;
extern bit MENU_AND_ESC_BTN;
#define RESET_BTN  BRESET_BTN
#ifndef MM32F0133C7P		//? Add
extern bank1 bit PAUSE;
#else
extern bit PAUSE;
#endif
extern unsigned char DCP_Configuration;

/****** Menu Variables ******/

// Battery voltage monitor - moved to battmon.h

// Maximum runtime - moved to timekeep.h

// cold start variables

/**** APU Variables ****/
extern long APU_HOURS;
extern unsigned char runTimeSinceLastStart;
extern unsigned int APU_Start_Total;
extern bit APU_ON_FLAG;
//extern bank1 char waitx;
extern bit WAIT;
extern bit APU_RUN;
extern bit ENGINE_FAULT;
extern bit ENGINE_FAULT_OIL;
extern bit ENGINE_FAULT_TEMP;
extern bit ENGINE_FAULT_ALTFB;
extern bit ENGINE_INTRLCK;
extern bit engineFaultOilPrev;
extern bit engineFaultTempPrev;
extern bit engineFaultAltFbPrev;
extern bit mainBrakerTripped;
extern bit APU_STOPPED;
extern bit AUTO,MANUAL;
extern bit apuFaultLockout;
//extern bit OIL,oil1;

/**** HVAC Variables ****/
extern unsigned char fan_low_high;
extern unsigned char heat_ac;
extern bit ac_pwr1, AC_PWR, BR1, BR2, b1, b2;
//extern bit AC_PWR;
extern bit HVAC_ENABLE;
extern char HVAC_ON;
extern bit FAN_LOW;
extern bit FAN_HIGH;
extern bit HEAT_CONT;
extern bit HEAT_ENABLE;
extern bit AC_CONT;
extern char FAN_FLAG;
extern char HEAT_FLAG;
extern char AC_FLAG;
extern char COLD;
extern int user_password;

/******************************************************
    State Machine Definitions
*******************************************************/
//Control
//enum c_state {ST_ALL_OFF,ST_FAN_LOW,ST_FAN_HIGH,ST_HEAT_LOW,ST_HEAT_HIGH,ST_AC_LOW,ST_AC_HIGH,ST_BREAKER_TRIPPED,ST_BREAKER_RESET,ST_PAUSE};
//enum c_state CONTROL_STATE;
//enum c_state last_state;
//ThermoStat
enum t_state {TH_ST_INIT,TH_OFF,TH_ST_HOLD,TH_ST_UP,TH_ST_DOWN};
extern enum t_state THERMO_STATE;

//!Possible AC & Heat Flags states
enum ac_h_f_state {OFF_STATE,AC_STATE,HEAT_STATE,BREAKER_STATE,PAUSE_STATE,UNLOAD_STATE};
extern enum ac_h_f_state AC_HEAT_STATE;
extern enum ac_h_f_state last_state;
//!Possible AC and Heat states
enum ac_h_state {low,high};
extern enum ac_h_state ac_state;
extern enum ac_h_state heat_state;
extern char breaker;   //!<state variable for BREAKER_STATE
//enum ac_h_f_state {ST_AC_HEAT_OFF, ST_ACF_LOW,ST_ACF_HIGH,ST_HEATF_LOW,ST_HEATF_HIGH};
//extern enum ac_h_f_state AC_HEAT_FLAG_STATE;

//!Possible Fan Flags states
enum f_f_state {ST_FANF_OFF,ST_FANF_LOW,ST_FANF_HIGH,ST_FANF_UNLOAD};

//!Possible Apu states
enum a_state {ST_APU_OFF,ST_SHORE_POWER,ST_APU_VERSION,ST_CHECK_LOCK,
    ST_APU_GLOW,ST_APU_LOCK_OUT,ST_APU_COVER,ST_APU_START,
	ST_COLD_START,ST_APU_WAIT,ST_APU_RESTART,ST_APU_CRANK_LIMIT,
    ST_APU_RUN,ST_APU_STOPPING,ST_APU_STOPPED,ST_APU_FAULT};
extern enum a_state APU_STATE;

//!Possible Outlet states
enum o_state {ST_OUTLET_OFF,ST_OUTLET_ON,ST_OUTLET_TRIPPED,
    HEAT_TRIPPED,OUTLET_TRIPPED,ST_OUTLET_RESET,ST_OUTLET_UNLOAD};
extern enum o_state OUTLET_STATE;

//!Possible states for monitoring modules
enum monitor_state {monitor_off,monitor_on,monitor_standby,monitor_starting,monitor_run};
//!Possible VAC states
enum ac_state {ST_AC_PWR_OFF,ST_AC_PWR_OFF_WAIT,ST_AC_PWR_ON,ST_PAUSE_WAIT,ST_AC_PWR_PAUSE,ST_AC_ON_WAIT};
extern enum ac_state AC_PWR_STATE;

//!Possible breaker states
enum b_state {ST_OFF,ST_TRIPPING,ST_TRIPPED};
extern enum b_state B1STATE;
extern enum b_state B2STATE;

//!Possible cold temperature states
enum getCold {unPressed,pressing,pressed,unPressing};

extern DCP_DIAG DCP_Diag;

//! System Configuration
extern uint8 SYS_u8SysCfgAccess;

#endif
