/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file maindata.c
       \brief Data declarations for Control Panel software

<!-- DESCRIPTION: -->
<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/maindata.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.3  2014/07/25 15:24:16  blepojevic
	      Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection
	
	      Revision 1.2  2014/04/16 14:07:09  blepojevic
	      Added diagnostic of APU and CCU
	
	      Revision 1.1  2009/07/31 17:01:42  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.10  2008-11-14 19:27:37  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.9  2007-12-21 20:36:44  msalimi
	      - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
	      - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
	      - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
	      - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

	      Revision 1.8  2007-11-30 18:52:08  msalimi
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

	      Revision 1.7  2007-10-23 19:01:51  msalimi
	      Corrected spelling error.

	      Revision 1.6  2007-09-21 20:52:21  msalimi
	      *** empty log message ***

	      Revision 1.5  2007-09-17 18:41:35  msalimi
	      DPF related modifications

	      Revision 1.4  2007-08-27 22:19:08  msalimi
	      CAN communication working.

	      Revision 1.3  2007-08-02 14:20:18  nallyn
	      Corrected the comments around cool_timer and key_pressed

	      Revision 1.2  2007-08-01 20:11:05  nallyn
	      Added comments for Doxygen

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.12  2006/02/08 20:20:15  nallyn
	      Changes based on code review.
	      Removed unused code
	
	      Revision 1.11  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include "main.h"
#include "defaults.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
/******************************************************
    I/O Definitions
*******************************************************/

/*** ***analog inputs*** ***/
//!ADC input of Battery Voltage
volatile int BATTERY_VOLTAGE;
//!ADC input of CCU feedback for presence of VAC and breaker trips
volatile int AC_BR1_BR2_SERV;     //PORTA 0
//!ADC input of APU feedback for glowing and faults 
volatile int OIL_OH_WAIT;      //PORTA 1
//bank1 volatile int PWR_MONITOR; //PORTA 2
//!ADC input for cabin ambient temperature
volatile int TEMPERATURE;      //PORTA 3
//unused analog          //PORTA 5

/*** ***Digital I/O*** ***/
//backlight - see lcd_grph.h      //PORTA 4
//LCD - CS1B - see lcd_grph.h     //PORTB 0

//Keypad rows and columns - see keypad.h and keypad.c
//bit R3   @ PORTBIT(PORTB, 2);
//bit R2   @ PORTBIT(PORTB, 3);
//bit R1   @ PORTBIT(PORTB, 4);
//bit C3   @ PORTBIT(PORTB, 5);
//bit C2   @ PORTBIT(PORTB, 6);
//bit C1   @ PORTBIT(PORTB, 7);

//bit buzzer     @ PORTBIT(PORTB, 1);
//LCD - RD - see lcd_grph.h       //PORTC 0
#ifndef MM32F0133C7P		//? Add
//!Unused
bit HEAT_AC         @ PORTBIT(PORTC, 1);
//!Unused
bit FAN_LOW_HIGH    @ PORTBIT(PORTC, 2);
#endif

//SCL - see i2c.h           //PORTC 3
//SDA - see i2c.h           //PORTC 4
//LCD - RS - see lcd_grph.h       //PORTC 5
//unused digital            //PORTC 6
//LCD - /RST - see lcd_grph.h        //PORTC 7
//LCD - DATA - see lcd_grph.h        //PORTD 0-7
//LCD - /WR - see lcd_grph.h         //PORTE 0

//APU - alternator off control - see apu.c  // PORTE 5
//APU - engine start control - see apu.c  // PORTE 6
//APU - engine on control - see apu.c  // PORTE 7

/* ---------- I n t e r n a l   D a t a ---------- */
/* ---------- G l o b a l   D a t a ---------- */
/**** Control Panel Variables ****/
//!Counter for air conditioning
char cool_timer = 0; 
//!Flag indicating whether any key has been pressed in the last loop of the program
//! - valid for one program loop
bit key_pressed;
//!Ambient temperature (copy of TEMPERATURE) used to compare set points with
//!the current cabin temperature
unsigned int AMBIENT_TEMP = 0;
#ifndef MM32F0133C7P		//? Add
//!Used to store the setpoint temperature as ADC counts to compare to the 
//!ambient temperature
bank1 unsigned int DECODE_TEMP = 0;
//!The set point value of temperature - set by the user when in heating or
//!air conditioning mode
bank1 unsigned char TEMP;
#else
//!Used to store the setpoint temperature as ADC counts to compare to the 
//!ambient temperature
unsigned int DECODE_TEMP = 0;
//!The set point value of temperature - set by the user when in heating or
//!air conditioning mode
unsigned char TEMP;
#endif
//!HVAC Off key flag
bit HVAC_OFF_BTN = 0;
//!On/Off key flag
bit ON_OFF_BTN = 0;
//!Start key flag
bit START_BTN;

//!HVAC Breaker tripped flag
bit BREAKER_TRIP;
//!A/C (air conditioning) key flag
bit AC_BTN;
//!Heat key flag
bit HEAT_BTN;
//!Flag indicating whether the outlet should be enabled or not
bit OUTLET;
//!Flag indicating whether or not Shore power is enabled
bit SHORE_POWER;

//!Fan key flag
bit FAN_BTN;
//!Temperature up (right arrow) key flag
bit TEMP_UP_BTN;
//!Temperature down (left arrow) key flag
bit TEMP_DOWN_BTN;
//!Esc/Reset key flag
bit BRESET_BTN;
//!Right arrow key flag - set by the same input as the TEMP_UP_BTN flag
bit RIGHT_BTN;
//!Left arrow key flag - set by the same input as the TEMP_DOWN_BTN flag
bit LEFT_BTN;
//!Menu key flag
bit MENU_BTN;
//!Flag to indicatate when both the Menu key and the Esc/Reset key have been pressed
bit MENU_AND_ESC_BTN;
//!Flag used to inform the CCU functions that there is a temporary break in
//!the VAC to the CCU
#ifndef MM32F0133C7P		//? Add
bank1 bit PAUSE;
#else
bit PAUSE;
#endif

// Battery voltage monitor
// See battmon.h

// Maximum runtime
// See timekeep.h

// Cold temperature start data
// Also see coldstart.h

// Timer 1 and Timer 2 data
// See timekeep.h

/**** APU Variables ****/
//!Variable used to display APU runtime hours
long APU_HOURS = 0;
//!Variable used to display APU runtime since last start hours and minutes (if less than 1 hour every minute, if more every 15 minutes)
unsigned char runTimeSinceLastStart = 0;
//!Variable holding the number of APU starts since the first time the variable is initiated (at SW update)
unsigned int APU_Start_Total;
//!Flag indicating whether or not the APU is on 
bit APU_ON_FLAG;
//bank1 char waitx = 0;
//!Flag indicating whether or not the APU is in the glow state
bit WAIT;
//!Unused
bit APU_RUN;
//!Flag indicating whether or not there is an engine fault
bit ENGINE_FAULT;
//!Flag indicating whether or not there is an engine fault oil pressure
bit ENGINE_FAULT_OIL;
//!Flag indicating whether or not there is an engine fault collant overtemperature
bit ENGINE_FAULT_TEMP;
//!Flag indicating whether or not there is an engine fault alternator feedback
bit ENGINE_FAULT_ALTFB;
//!Flag indicating whether or not there is an engine interlock
bit ENGINE_INTRLCK;
//!Flag indicating whether or not there is an previous engine fault oil pressure
bit engineFaultOilPrev;
//!Flag indicating whether or not there is an previous engine fault collant overtemperature
bit engineFaultTempPrev;
//!Flag indicating whether or not there is an previous engine fault alternator feedback
bit engineFaultAltFbPrev;
//!Flag indicating whether or not the APU has stopped running
bit APU_STOPPED;
//!Flags to indicate whether the APU was started by an auto-start function 
bit AUTO;
//!Flag to indicate if the APU is running in manual mode
bit MANUAL;
//!Flag to prevent auto functions from working when an APU fault has occured
bit apuFaultLockout;
//bit OIL,oil1;
//{376,397,417,438,459,480,501,522,

/**** HVAC Variables ****/
//!Variable for encoding the output to the CCU to control the fan and outlet
unsigned char fan_low_high;
//!Variable for encoding the output to the CCU to control the air conditioning
//!and heat
unsigned char heat_ac;
//!Flag to indicate whether or not VAC is present
bit AC_PWR;
//!Flag holding the input from the CCU regarding the presence of VAC
bit ac_pwr1;
//!Flag to indicate wheater the main breaker is tripped
bit mainBrakerTripped;
//!Flag to indicate whether or not the HVAC breaker has tripped
bit BR1;
//!Flag holding the input from the CCU regarding the state of the HVAC breaker 
bit b1;
//!Flag to indicate whether or not the outlet/high heat breaker has tripped
bit BR2;
//!Flag holding the input from the CCU regarding the state of the outlet/high
//!heat breaker
bit b2;
//!Flag to indicate that HVAC functions are enabled based on VAC or the APU
//!running
bit HVAC_ENABLE = 0;
//!Flag indicating that one of the HVAC functions (heat A/C or fan) is currently
//!turned on
char HVAC_ON;
//!Flag to control the output of the fan low signal to the CCU
bit FAN_LOW = 0;
//!Flag to control the fan high signal to the CCU
bit FAN_HIGH = 0;
//!Flag to control the heat signal to the CCU
bit HEAT_CONT = 0;
//!Flag to enable the heat to the CCU
bit HEAT_ENABLE = 0;
//!Flag to control the A/C signal to the CCU
bit AC_CONT = 0;
//!Variable to indicate which mode the fan is in (off, low or high)
char FAN_FLAG = 0;
//!Variable to indicate to the thermostat control whether or not heat mode
//!is active
char HEAT_FLAG = 0;
//!Variable to indicate to the thermostat control whether or not A/C mode
//!is active
char AC_FLAG = 0;
//!Variable to signal the APU module whether or not a cold start is required
char COLD = 0;

//!Variable storing the password for the menu system
int user_password = DEFAULT_PASSWORD;
//!Variable storing the configuration of the DCP
unsigned char DCP_Configuration;

/******************************************************
    State Machine Definitions
*******************************************************/
//!State variable for the thermostatic control module
enum t_state THERMO_STATE=TH_ST_INIT;
//!State variable for the A/C, heat control module
enum ac_h_f_state AC_HEAT_STATE=OFF_STATE;
//!State variable storing the last state of the A/C, heat control module
enum ac_h_f_state last_state;
//!State variable for fan control in a/c mode 
enum ac_h_state ac_state=low;
//!State variable for fan control in heat mode
enum ac_h_state heat_state=low;
//!state variable for BREAKER_STATE
char breaker;
//!State variable for the APU control module   
enum a_state APU_STATE=ST_APU_OFF;
//!State variable for the outlet control module
enum o_state OUTLET_STATE=ST_OUTLET_OFF;
//!State variable for VAC detection
enum ac_state AC_PWR_STATE = ST_AC_PWR_OFF;
//!State variable for breaker 1 detection
enum b_state B1STATE=ST_OFF;
//!State variable for breaker 2 detection
enum b_state B2STATE=ST_OFF;

/* ----------------- F u n c t i o n s ----------------- */


