/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file keypad.c
       \brief Module for crosspoint keypad input device driver

<!-- DESCRIPTION: -->
       This module provides a function to perform scanning of 
       a crosspoint keypad matrix and to generate keypress events.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/keypad.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

          $Log: not supported by cvs2svn $
          Revision 1.2  2014/07/25 15:24:16  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.1  2009/07/31 16:53:30  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
    
          Revision 1.5  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.4  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:06:10  nallyn
          Added commenting for Doxygen

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.1  2006/02/08 19:06:38  nallyn
          Modularized code by taking functions out of headers and placing them in .c files
          Updated file headers to match proposed Teleflex coding standard
    
      \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s -------------- */
//?#include <pic18.h>
#include "main.h"
#include "keypad.h"
#include "keypad-timers.h"
//#include "maindata.c"
#include "delay18.h"
#include "Events.h"
#include "gpio.h"				//? Add

/* ---------- I n t e r n a l   D e f i n i t i o n s ------ */
//!Delay time for keypad scaning, used with DelayUs()
#define KEY_DELAY		5
//!Debounce time for individual keys. Used with the 10 ms tick  
#define DEBOUNCE		3
//!Rebounce time for the individual keys. Used with the 10 ms tick
#define REBOUNCE		0
//!Repeat delay for the temperature up and down keys. Used with the 10 ms tick
#define TEMPERATURE_REPEAT_DELAY	20
//!Repeat time for the temperature up and down keys. Used with the 10 ms tick
#define TEMPERATURE_REPEAT_TIME	1
//!Repeat delay for up and down keys. Used with the 10 ms tick
#define KEY_REPEAT_DELAY	30
//!Repeat time for up and down keys. Used with the 10 ms tick
#define KEY_REPEAT_TIME	10

//!States that each key can have
enum button_state {UN_PRESSED,PRESSING,PRESSED,STILL_PRESSED};

static bit keyOn;       //!<local working variable bit size to save code space

/* ---------- I n t e r n a l   D a t a -------------------- */
//!Number of rows in the keypad matrix
#define NUM_ROWS    3
//!Number of columns in the keypad matrix
#define NUM_COLUMNS 3

//!Array holding the current state of each of the keys in the keypad array
static char aKeyPad[NUM_ROWS][NUM_COLUMNS]; 

#ifndef MM32F0133C7P		//? Add
//!No longer used
bit R3   @ PORTBIT(PORTB, 0); //Keypad rows and columns.
//!No longer used
bit R2   @ PORTBIT(PORTB, 1); //inputs
//!No longer used
bit R1   @ PORTBIT(PORTB, 2);
//!No longer used
bit C3   @ PORTBIT(PORTB, 3); //outputs
//!No longer used
bit C2   @ PORTBIT(PORTB, 4);
//!No longer used
bit C1   @ PORTBIT(PORTB, 5);
#endif

/* ---------- G l o b a l   D a t a ------------------------ */
/* ---------- F u n c t i o n s ---------------------------- */
/******************************************************************************/
/*!

<!-- FUNCTION NAME: KPD_fnInitKeypad -->
       \brief Initializes the port for the keypad rows and columns

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void KPD_fnInitKeypad(void)
{
#ifndef MM32F0133C7P		//? Add
    TRISB = 0xC7;
#else		///?OK
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2;					//PB2-0 Input, Pull-up
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fnScanKeypad -->
       \brief Performs a column by column scan to see which rows are turned on

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fnScanKeyPad(void)
{
    char column = 0;
    char row = 0;
#ifndef MM32F0133C7P		//? Add
    char copy = 0;

    for ( column = 0; column < NUM_COLUMNS; column++ )
    {
        PORTB |= 0xF8;
        PORTB &= ( 0xFF & ~( 1 << (column + NUM_ROWS) ) );
        DelayUs(KEY_DELAY);
        copy = ~PORTB;
#else		///?OK
    u16 copy = 0;
		u16 PortB;

		PortB = ReadOutputLatch(GPIOB) | 0x0038;					//Set PB3-5 = 1
	
    for ( column = 0; column < NUM_COLUMNS; column++ )
    {
	      PortB &= ( 0xFFFF & ~( 1 << (column + NUM_ROWS) ) );
				//UART_printf("---> PortB = %d %04x\n\r", column, PortB);
				WritePort(GPIOB, PortB);
        DelayUs(KEY_DELAY);
				copy = ~ReadPort(GPIOB);
				PortB |= 0x0038;															//Set PB3-5 = 1
				WritePort(GPIOB, PortB);
#endif
        for ( row = 0; row < NUM_ROWS; row++ )
        {
            if ( ( copy & ( 1 << row ) ) != 0 )
            {
                aKeyPad[row][column] = 1;
								//UART_printf("---> aKeyPad[%d][%d] = 1\n\r", row, column);
            }
            else
            {
                aKeyPad[row][column] = 0;
								//UART_printf("---> aKeyPad[%d][%d] = 0\n\r", row, column);
            }
        }
    }
}


/******************************************************

        R0          R1          R2
C0    on/off      start        a/c

C1    tempup      tempdn       fan

C2     off        reset        heat

******************************************************/
#define ON_OFF_BTN()    aKeyPad[0][0]
#define TEMP_UP_BTN()   aKeyPad[0][1]
#define HVAC_OFF_BTN()  aKeyPad[0][2]
#define START_BTN()     aKeyPad[1][0]
#define TEMP_DN_BTN()   aKeyPad[1][1]
#define ESC_RESET_BTN() aKeyPad[1][2]
#define AC_BTN()        aKeyPad[2][0]
#define FAN_BTN()       aKeyPad[2][1]
#define HEAT_BTN()      aKeyPad[2][2]

//!State variable for the On/Off key
enum button_state OOButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: on_off_button -->
       \brief Update the state of the On-Off button

<!-- PURPOSE: -->
       Updates the state of the OOButton variable by scanning R1 C1 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void on_off_button(void)
{
// 	C2 = 1;
//	C3 = 1;
//	C1 = 0; // Scanning column 1
//	DelayUs(KEY_DELAY);
//    keyOn = FALSE;
//	if (!R1)// Row 1 (On/Off Button for APU)
//	{
//		keyOn = aKeyPad[R0][C0];
//		key_pressed = keyOn;
//	}
//  C1 = 1;
    
    keyOn = ON_OFF_BTN();
    key_pressed |= keyOn;
    switch(OOButton)
    {
        case UN_PRESSED:
            ON_OFF_BTN = 0;
            START_BTN = 0;
            if(!kptimers.oobTmr && keyOn)
            {
                OOButton = PRESSING;
                kptimers.oobTmr = DEBOUNCE;//oobTimer(DEBOUNCE);
            }
            break;
        case PRESSING:
            if(!kptimers.oobTmr)
            {
                if(keyOn)
                {
                    OOButton = PRESSED;
                    kptimers.oobTmr = DEBOUNCE;//oobTimer(5);
                }
                else
                {
                    OOButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            ON_OFF_BTN = 1;
//			Add_Event(EV_ON_OFF_KEY);
            START_BTN = 0;
            if(keyOn)
            {
                OOButton = STILL_PRESSED;
                kptimers.oobTmr = 250;//oobTimer(250);
            }
            else
            {
                OOButton = UN_PRESSED;
                kptimers.oobTmr = DEBOUNCE;//oobTimer(5);
            }
            
            break;
        case STILL_PRESSED:
            ON_OFF_BTN = 0;
            if(!kptimers.oobTmr)
            {
                START_BTN = 1;
            }
            if(!keyOn)
            {
                OOButton = UN_PRESSED;
                kptimers.oobTmr = DEBOUNCE;//oobTimer(5);
            }
            else
            {
                OOButton = STILL_PRESSED;
            }
            break;
        default:
            OOButton = UN_PRESSED;
            break;
    }
}

//!State variable for the Start key - which is the same as the On/Off key
enum button_state SButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: start_button -->
       \brief Update the state of the start button

<!-- PURPOSE: -->
       Updates the state of the SButton variable by scanning R1 C2 of the keypad
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void start_button(void)
{
    
    keyOn = START_BTN();
    key_pressed |= keyOn;
    
    switch(SButton)
    {
        case UN_PRESSED:
            //START_BTN = 0;
            MENU_BTN = 0;
            if(!kptimers.sbTmr && keyOn)
            {
                SButton = PRESSING;
                kptimers.sbTmr = DEBOUNCE;//sbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.sbTmr)
            {
                if(keyOn)
                {
                    SButton = PRESSED;

                }
                else
                {
                    SButton = UN_PRESSED;
                }
                kptimers.sbTmr = DEBOUNCE;//sbTimer(5);
            }
            break;
        case PRESSED:
            //START_BTN = 1;
            MENU_BTN = 1;
//			Add_Event(EV_MENU_KEY);
            if(keyOn)
            {
                SButton = STILL_PRESSED;
            }
            else
            {
                SButton = UN_PRESSED;
            }
            kptimers.sbTmr = DEBOUNCE;//sbTimer(5);
            break;
        case STILL_PRESSED:
            //START_BTN = 1;
            MENU_BTN = 0;
            if(!keyOn)
            {
                SButton = UN_PRESSED;
            }
            break;
        default:
            SButton = UN_PRESSED;
            break;
    }
}

//!State variable for the fan key
enum button_state FButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: fan_button -->
       \brief Update the state of the fan button

<!-- PURPOSE: -->
       Updates the state of the FButton variable by scanning R2 C3 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void fan_button(void)
{
    
    keyOn = FAN_BTN();
    key_pressed |= keyOn;
    
    switch(FButton)
    {
        case UN_PRESSED:
            FAN_BTN = 0;
            if(!kptimers.fbTmr && keyOn)
            {
                FButton = PRESSING;
                kptimers.fbTmr = DEBOUNCE;//fbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.fbTmr)
            {
                if(keyOn)
                {
                    FButton = PRESSED;
                    kptimers.fbTmr = DEBOUNCE;//fbTimer(5);
                }
                else
                {
                    FButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            FAN_BTN = 1;
//			Add_Event(EV_FAN_KEY);
            if(keyOn)
            {
                FButton = STILL_PRESSED;
            }
            else
            {
                FButton = UN_PRESSED;
            }
            kptimers.fbTmr = DEBOUNCE;//fbTimer(5);
            break;
        case STILL_PRESSED:
            FAN_BTN = 0;
            if(!keyOn)
            {
                FButton = UN_PRESSED;
            }
            break;
        default:
            FButton = UN_PRESSED;
            break;
    }
}

//!State variable for the A/C key
enum button_state ACButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ac_button -->
       \brief Update the state of the A/C button

<!-- PURPOSE: -->
       Updates the state of the ACButton variable by scanning R1 C3 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void ac_button(void)
{
    
    keyOn = AC_BTN();
    key_pressed |= keyOn;

    switch(ACButton)
    {
        case UN_PRESSED:
            AC_BTN = 0;
            if(!kptimers.acbTmr && keyOn)
            {
                ACButton = PRESSING;
                kptimers.acbTmr = DEBOUNCE;//acbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.acbTmr)
            {
                if(keyOn)
                {
                    ACButton = PRESSED;
                    kptimers.acbTmr = DEBOUNCE;//acbTimer(5);
                }
                else
                {
                    ACButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            AC_BTN = 1;
//			Add_Event(EV_AC_KEY);
            if(keyOn)
            {
                ACButton = STILL_PRESSED;
            }
            else
            {
                ACButton = UN_PRESSED;
            }
            kptimers.acbTmr = DEBOUNCE;//acbTimer(5);
            break;
        case STILL_PRESSED:
            AC_BTN = 0;
            if(!keyOn)
            {
                ACButton = UN_PRESSED;
            }
            break;
        default:
            ACButton = UN_PRESSED;
            break;
    }
}

//!State variable for the heat key
enum button_state HButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: heat_button -->
       \brief Update the state of the Heat button

<!-- PURPOSE: -->
       Updates the state of the HButton variable by scanning R3 C3 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void heat_button(void)
{
    keyOn = HEAT_BTN();
    key_pressed |= keyOn;
        
    switch(HButton)
    {
        case UN_PRESSED:
            HEAT_BTN = 0;
            if(!kptimers.hbTmr && keyOn)
            {
                HButton = PRESSING;
                kptimers.hbTmr = DEBOUNCE;//hbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.hbTmr)
            {
                if(keyOn)
                {
                    HButton = PRESSED;
                    kptimers.hbTmr = DEBOUNCE;//	hbTimer(5);
                }
                else
                {
                    HButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            HEAT_BTN = 1;
//			Add_Event(EV_HEAT_KEY);
            if(keyOn)
            {
                HButton = STILL_PRESSED;
            }
            else
            {
                HButton = UN_PRESSED;
            }
            kptimers.hbTmr = DEBOUNCE;//hbTimer(5);
            break;
        case STILL_PRESSED:
            HEAT_BTN = 0;
            if(!keyOn)
            {
                HButton = UN_PRESSED;
            }
            break;
        default:
            HButton = UN_PRESSED;
            break;
    }
}


//!Variable used for repeating the temperature down key
char repeatTD;
//!Variable used for repeating the left key - same physical key as the
//!temperature down key
char repeatL;
//!State variable for the temperture down key
enum button_state TDButton = UN_PRESSED;
//!State variable for the left key - same physical key as the temperature down key
enum button_state LButton = UN_PRESSED;


/******************************************************************************/
/*!

<!-- FUNCTION NAME: temp_down_button -->
       \brief Update the state of the Temp Down button

<!-- PURPOSE: -->
       Updates the state of the TDButton and LButton variables by scanning R2 C2 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void temp_down_button(void)
{
    keyOn = TEMP_DN_BTN();
    key_pressed |= keyOn;

    switch(TDButton)
    {
        case UN_PRESSED:
            TEMP_DOWN_BTN = 0;
            if(keyOn)
            {
                TDButton = PRESSING;
                kptimers.tdbTmr = DEBOUNCE;//tdbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.tdbTmr)
            {
                if(keyOn)
                {
                    TDButton = PRESSED;
                    //tdbTimer(1);
                }
                else
                {
                    TDButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            TEMP_DOWN_BTN = 1;
            //TDButton = UN_PRESSED;
            if(keyOn)
            {
                TDButton = STILL_PRESSED;
                repeatTD = 0;
                kptimers.tdbTmr = TEMPERATURE_REPEAT_DELAY;//tdbTimer(20);
            }
            else
            {
                TDButton = UN_PRESSED;
                //tdbTimer(1);
            }
            break;
        case STILL_PRESSED:			
            if(keyOn)
            {
                switch(repeatTD)
                {
                    case 0:
                        TEMP_DOWN_BTN = 0;
                        if(!kptimers.tdbTmr)
                        {
                            repeatTD = 1;
                            //tdbTimer(2);
                        }
                        break;
                    case 1:
                        TEMP_DOWN_BTN = 1;
                        repeatTD = 0;
                        kptimers.tdbTmr = TEMPERATURE_REPEAT_TIME;//tdbTimer(1);
                        break;
                    default:
                        repeatTD = 0;
                        break;
                }
            }
            else
            {
                TEMP_DOWN_BTN = 0;
                TDButton = UN_PRESSED;
            }
            break;
        default:
            TEMP_DOWN_BTN = 0;
            TDButton = UN_PRESSED;
            break;
    }
    
    switch(LButton)
    {
        case UN_PRESSED:
            LEFT_BTN = 0;
            if(!kptimers.lbTmr && keyOn)
            {
                LButton = PRESSING;
                kptimers.lbTmr = DEBOUNCE;//lbTimer(DEBOUNCE);
            }
            break;
        case PRESSING:
            if(!kptimers.lbTmr)
            {
                if(keyOn)
                {
                    LButton = PRESSED;
                }
                else
                {
                    LButton = UN_PRESSED;
                    //lbTimer(REBOUNCE);
                }
            }
            break;
        case PRESSED:
            LEFT_BTN = 1;
//			Add_Event(EV_LEFT_ARROW_KEY);
            if(keyOn)
            {
                LButton = STILL_PRESSED;
                //LEFT_BTN = 0;
                repeatL = 0;
                kptimers.lbTmr = KEY_REPEAT_DELAY;//lbTimer(40);
            }
            else
            {
                LButton = UN_PRESSED;
                kptimers.lbTmr = DEBOUNCE;//lbTimer(REBOUNCE);
            }
            break;
        case STILL_PRESSED:			
            if(keyOn)
            {
                switch(repeatL)
                {
                    case 0:
                        LEFT_BTN = 0;
                        if(!kptimers.lbTmr)
                        {
                            repeatL = 1;
                        }
                        break;
                    case 1:
                        LEFT_BTN = 1;
                        repeatL = 0;
                        kptimers.lbTmr = KEY_REPEAT_TIME;//lbTimer(10);
                        break;
                    default:
                        repeatL = 0;
                        break;
                }
            }
            else
            {
                LEFT_BTN = 0;
                LButton = UN_PRESSED;
                kptimers.lbTmr = REBOUNCE;//lbTimer(REBOUNCE);
            }
            break;
        default:
            LEFT_BTN = 0;
            LButton = UN_PRESSED;
            kptimers.lbTmr = REBOUNCE;//lbTimer(REBOUNCE);
            break;
    }	
}

//!Variable used for repeating the temperature up key
char repeatTU;
//!Variable used for repeating the right key - same physical key as the 
//!temperature up key
char repeatR;
//!State variable for the temperature up key
enum button_state TUButton = UN_PRESSED;
//!State variable for the right key - same physical key as the temperture up key
enum button_state RButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: temp_up_button -->
       \brief Update the state of the Temp Up button

<!-- PURPOSE: -->
       Updates the state of the TUButton and RButton variables by scanning R2 C1 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void temp_up_button(void)
{
    keyOn = TEMP_UP_BTN();
    key_pressed |= keyOn;
    
    switch(TUButton)
    {
        case UN_PRESSED:
            TEMP_UP_BTN = 0;
            if(keyOn)
            {
                TUButton = PRESSING;
                kptimers.tubTmr = DEBOUNCE;//tubTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.tubTmr)
            {
                if(keyOn)
                {
                    TUButton = PRESSED;
                    //tubTimer(1);
                }
                else
                {
                    TUButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            TEMP_UP_BTN = 1;
            if(keyOn)
            {
                TUButton = STILL_PRESSED;
                repeatTU = 0;
                kptimers.tubTmr = TEMPERATURE_REPEAT_DELAY;//tubTimer(20);
            }
            else
            {
                TUButton = UN_PRESSED;
                //kptimers.tubTmr = 1;//tubTimer(1);
            }
            break;
        case STILL_PRESSED:
            
            if(keyOn)
            {
                switch(repeatTU)
                {
                    case 0:
                        TEMP_UP_BTN = 0;
                        if(!kptimers.tubTmr)
                        {
                            repeatTU = 1;
                        }
                        break;
                    case 1:
                        TEMP_UP_BTN = 1;
                        repeatTU = 0;
                        kptimers.tubTmr = TEMPERATURE_REPEAT_TIME;//tubTimer(1);
                        break;
                    default:
                        repeatTU = 0;
                        break;
                }
            }
            else
            {
                TEMP_UP_BTN = 0;
                TUButton = UN_PRESSED;
            }
            break;
        default:
            TEMP_UP_BTN = 0;
            TUButton = UN_PRESSED;
            break;
    }
    
    switch(RButton)
    {
        case UN_PRESSED:
            RIGHT_BTN = 0;
            if(!kptimers.rbTmr && keyOn)
            {
                RButton = PRESSING;
                kptimers.rbTmr = DEBOUNCE;//rbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.rbTmr)
            {
                if(keyOn)
                {
                    RButton = PRESSED;
                    //rbTimer(1);
                }
                else
                {
                    RButton = UN_PRESSED;
                    //rbTimer(REBOUNCE);
                }
            }
            break;
        case PRESSED:
            RIGHT_BTN = 1;
//			Add_Event(EV_RIGHT_ARROW_KEY);
            if(keyOn)
            {
                RButton = STILL_PRESSED;
                //RIGHT_BTN = 0;
                repeatR = 0;
                kptimers.rbTmr = KEY_REPEAT_DELAY;//rbTimer(40);
            }
            else
            {
                RButton = UN_PRESSED;
                kptimers.rbTmr = REBOUNCE;//rbTimer(REBOUNCE);
            }
            break;
        case STILL_PRESSED:

            if(keyOn)
            {
                switch(repeatR)
                {
                    case 0:
                        RIGHT_BTN = 0;
                        if(!kptimers.rbTmr)
                        {
                            repeatR = 1;
                        }
                        break;
                    case 1:
                        RIGHT_BTN = 1;
                        repeatR = 0;
                        kptimers.rbTmr = KEY_REPEAT_TIME;//rbTimer(10);
                        break;
                    default:
                        repeatR = 0;
                        break;
                }
            }
            else
            {
                RIGHT_BTN = 0;
                RButton = UN_PRESSED;
                kptimers.rbTmr = REBOUNCE;//rbTimer(REBOUNCE);
            }
            break;
        default:
            RIGHT_BTN = 0;
            RButton = UN_PRESSED;
            kptimers.rbTmr = REBOUNCE;//rbTimer(REBOUNCE);
            break;
    }    	
}

//!State variable for the HVAC off key
enum button_state OButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: off_button -->
       \brief Update the state of the HVAC Off button

<!-- PURPOSE: -->
       Updates the state of the OButton variable by scanning R3 C1 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void off_button(void)
{
    keyOn = HVAC_OFF_BTN();
    key_pressed |= keyOn;

    switch(OButton)
    {
        case UN_PRESSED:
            HVAC_OFF_BTN = 0;
            if(!kptimers.obTmr && keyOn)
            {
                OButton = PRESSING;
                kptimers.obTmr = DEBOUNCE;//obTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.obTmr)
            {
                if(keyOn)
                {
                    OButton = PRESSED;
                    kptimers.obTmr = DEBOUNCE;//obTimer(5);
                }
                else
                {
                    OButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            HVAC_OFF_BTN = 1;
//			Add_Event(EV_HVAC_OFF_KEY);
            if(keyOn)
            {
                OButton = STILL_PRESSED;
            }
            else
            {
                OButton = UN_PRESSED;
            }
            kptimers.obTmr = DEBOUNCE;//obTimer(5);
            break;
        case STILL_PRESSED:
            HVAC_OFF_BTN = 0;
            if(!keyOn)
            {
                OButton = UN_PRESSED;
            }
            break;
        default:
            OButton = UN_PRESSED;
            break;
    }
}

//!State variable for the esc/reset key
enum button_state BRButton = UN_PRESSED;

/******************************************************************************/
/*!

<!-- FUNCTION NAME: breset_button -->
       \brief Update the state of the Breaker Reset button

<!-- PURPOSE: -->
       Updates the state of the BRButton variable by scanning R3 C2 of the keypad
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void breset_button(void)
{
    static char reset = 0;

    keyOn = ESC_RESET_BTN();
    key_pressed |= keyOn;

    switch(BRButton)
    {
        case UN_PRESSED:
            BRESET_BTN = 0;
            if(!kptimers.brbTmr && keyOn)
            {
                BRButton = PRESSING;
                kptimers.brbTmr = DEBOUNCE;//brbTimer(DEBOUNCE);
            }
            break;

        case PRESSING:
            if(!kptimers.brbTmr)
            {
                if(keyOn)
                {
                    BRButton = PRESSED;
                    kptimers.brbTmr = DEBOUNCE;//brbTimer(5);
                }
                else
                {
                    BRButton = UN_PRESSED;
                }
            }
            break;
        case PRESSED:
            BRESET_BTN = 1;
            Add_Event(EV_ESC_KEY);
            if(keyOn)
            {
                BRButton = STILL_PRESSED;
            }
            else
            {
                BRButton = UN_PRESSED;
            }
            kptimers.brbTmr = DEBOUNCE;//brbTimer(5);
            if ( SButton == STILL_PRESSED )
                MENU_AND_ESC_BTN = 1;
            break;
        case STILL_PRESSED:
            BRESET_BTN = 0;
            MENU_AND_ESC_BTN = 0;
            if(!keyOn)
            {
                BRButton = UN_PRESSED;
            }
            break;
        default:
            BRButton = UN_PRESSED;
            break;
    }

    if(!reset)
      reset = 1;
    else
      reset = 0;
}


/******************************************************************************/
/*!

<!-- FUNCTION NAME: KPD_fnKeypad -->
       \brief Function which scans all the keys of the keypad

<!-- PURPOSE: -->
       Updates the state variables corresponding to each button on the 
       crosspoint matrix keypad.
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void KPD_fnKeypad(void)
{
    fnScanKeyPad();    
    on_off_button();		// APU On/Off
    temp_up_button();		// Thermostat Temperature Up
    off_button();			// HVAC Off
    start_button();			// APU Start
    temp_down_button();		// Thermostat Temperature Down
    breset_button();		// Breaker Reset
    ac_button();			// Air Conditioner
     fan_button();			// Fan
    heat_button();			// Heat
}
