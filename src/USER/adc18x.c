/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file adc18x.c
       \brief Driver file for the ADC unit

<!-- DESCRIPTION: -->
	Driver file for the ADC unit - designed for PIC18F extended instruction set
	controllers.

<!-- NOTES: -->
       \if ShowHist
       Copyright � 2006 Teleflex Canada, Inc. All rights reserved.
	    This source file and the information contained in it are confidential and 
	    proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in 
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

       CHANGE HISTORY:

	    $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/adc18x.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	    $Log: not supported by cvs2svn $
	    Revision 1.3  2014/04/16 14:07:07  blepojevic
	    Added diagnostic of APU and CCU
	
	    Revision 1.2  2013/09/19 13:07:42  blepojevic
	    Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements
	
	    Revision 1.1  2007/09/21 20:34:02  blepojevic
	    No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	    Revision 1.3  2007-09-21 20:34:02  msalimi
	    No change

	    Revision 1.2  2007-07-05 21:28:30  gfinlay
	    Fixed problem with file header comment block - it was causing an error in Doxygen with the code directive

	    Revision 1.1  2007-07-05 20:10:11  gfinlay
	    Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
//?#include <pic18.h> //Pic CPU header file
#include "adc18x.h"
#include "adcx.h"				//? Add

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

// Direction Register definitions
#if defined(_18F8585) || defined(_18F8680)
    #define TRISAN15    TRISH7
    #define TRISAN14    TRISH6
    #define TRISAN13    TRISH5
    #define TRISAN12    TRISH4
    #define MAXCHANNELS 16
#else
    #define MAXCHANNELS 12
#endif

#define TRISAN11    TRISF6
#define TRISAN10    TRISF5
#define TRISAN9     TRISF4
#define TRISAN8     TRISF3
#define TRISAN7     TRISF2
#define TRISAN6     TRISF1
#define TRISAN5     TRISF0
#define TRISAN4     TRISA5
#define TRISAN3     TRISA3
#define TRISAN2     TRISA2
#define TRISAN1     TRISA1
#define TRISAN0     TRISA0

//ADCON2 Options
#define AD_LEFT_JUSTIFIED   0x00    // Left justified number
#define AD_RIGHT_JUSTIFIED  0x80    // Right justified number

// A/D Acquisition Time
#define TAD0                0x00    
#define TAD2                0x08
#define TAD4                0x10
#define TAD6                0x18
#define TAD8                0x20
#define TAD12               0x28
#define TAD16               0x30
#define TAD20               0x38

// A/D Conversion clock select
#define FOSC2               0x00                      
#define FOSC8               0x01
#define FOSC32              0x02
#define FOSC4               0x04
#define FOSC16              0x05
#define FOSC64              0x06
#define FRC                 0x07

#define INPUT   1
#define OUTPUT  0
#define ADGO    GODONE  

/* ---------- I n t e r n a l   D a t a ---------- */
uint8 u8ADCON1Copy = 0;

/* ---------- G l o b a l   D a t a ---------- */


/* ----------------- F u n c t i o n s ----------------- */

 
/******************************************************************************/
/*!

<!-- FUNCTION NAME: InitialiseADC -->
       \brief Initialises the ADC channels 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  u8NumChannels    The number of A/D channels to be used. See data sheet
        for details on combinations for A/D ports.
<!-- RETURNS: -->
      \return 0 on failure, 1 on success.

*/
/******************************************************************************/
int8 InitialiseADC ( uint8 u8NumChannels ) 
{
#ifndef MM32F0133C7P		//? Add
    if ( u8NumChannels >= MAXCHANNELS )
    {
        return 0;
    }
    
    switch ( u8NumChannels )
    {
#if defined(_18F8585) || defined(_18F8680)
        case 15:
            TRISAN15 = INPUT;
        case 14:
            TRISAN14 = INPUT;
        case 13:
            TRISAN13 = INPUT;
        case 12:
            TRISAN12 = INPUT;
#endif
        case 11:
            TRISAN11 = INPUT;
        case 10:
            TRISAN10 = INPUT;
        case 9:
            TRISAN9 = INPUT;
        case 8:
            TRISAN8 = INPUT;
        case 7:
            TRISAN7 = INPUT;
        case 6:
            TRISAN6 = INPUT;
        case 5:
            TRISAN5 = INPUT;
        case 4:
            TRISAN4 = INPUT;
        case 3:
            TRISAN3 = INPUT;
        case 2:
            TRISAN2 = INPUT;
        case 1:
            TRISAN1 = INPUT;
        case 0:
            TRISAN0 = INPUT;
            break;
        default:
            break;

    }
    
    ADCON2 = 0x00;
    ADCON2 = AD_RIGHT_JUSTIFIED | TAD0 | FOSC64;
    u8ADCON1Copy = ( (~(u8NumChannels + 1) ) & 0x0f ); 
    ADCON1 = u8ADCON1Copy;
#else		///?OK
		ADCx_Init();
#endif

    return 1;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: SetADCChannel -->
       \brief Reads the ADC level input on a specified ADC channel.

<!-- PURPOSE: -->
       Takes in an 10-bit ADC channel number indicating channel to be read next by Read_ADC().

<!-- PARAMETERS: -->
       @param[in]  ADC_Channel   The channel to be read by Read_ADC()
<!-- RETURNS: -->
      \return 1 if ADC_Channel is within range  0 to MAXCHANNELS-1, or
              0 if the ADC_Channel is out of range.

*/
/******************************************************************************/
int8 SetADCChannel(unsigned char ADC_Channel)
{
#ifndef MM32F0133C7P		//? Add
    if ( ADC_Channel >= MAXCHANNELS )
    {
        return 0;
    }
        
    // Rewrite ADCON1 register for noisy environments
    ADCON1 = u8ADCON1Copy;
    
    // Selecting ADC channel
    ADCON0 = (ADC_Channel << 2) + 0x01; // Enable ADC
#else		///?OK
		SetADCxChannel(ADC_Channel);
#endif
		
    return 1;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Read_ADC -->
       \brief Reads the ADC level input on a specified 10-bit ADC channel.

<!-- PURPOSE: -->
  Approximate sampling time = 76.8us
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return A 10-bit number that signifies the level on the selected channel

*/
/******************************************************************************/
uint16 Read_ADC(void)
{
    uint16 ADC_VALUE = 0;
    uint8 x;

#ifndef MM32F0133C7P		//? Add
    ADIF = 0;              // Resetting the ADC interupt bit

    ADGO = 1;              // Staring the ADC process
    while(!ADIF) continue; // Wait for conversion complete

    x = ADRESL;            // Getting HSB of CCP1
    ADC_VALUE = ADRESH;    // Getting LSB of CCP1
    //x = x >> 6;
    ADC_VALUE = ADC_VALUE << 8;
    ADC_VALUE += x;
#else		///?OK
		ADC_VALUE = GetSingleChannelValue();
		//UART_printf("ADC = %04X\n\r", ADC_VALUE);
#endif
#ifndef MM32F0133C7P		//? Add
    return (ADC_VALUE);    // Return the value of the ADC process
#else
		return (ADC_VALUE >> 2);    // Return the value of the ADC process
#endif
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ReadADCeight -->
       \brief Reads the ADC level input for a specified 8-bit ADC channel

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return An 8-bit number that contains the level of the selected 8-bit ADC channnel

*/
/******************************************************************************/
uint8 ReadADCeight(void)
{
#ifndef MM32F0133C7P		//? Add
    volatile unsigned char x;
    ADIF = 0;              // Resetting the ADC interupt bit

    ADGO = 1;              // Staring the ADC process 
    while(!ADIF) continue; // Wait for conversion complete

    x = ADRESH;            // Getting MSB of CCP1

    return (x);            // Return the value of the ADC process
#endif
}
