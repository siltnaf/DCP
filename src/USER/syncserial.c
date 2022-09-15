/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file syncserial.c
       \brief Synchronous Serial driver for PIC18 

<!-- DESCRIPTION: -->
    This file provides an I2C driver for the PIC 18 synchronous serial port.
    So far the only driver only supports I2C.

      \code
             
        GLOBAL FUNCTIONS:
          ssfIsr              - Interrupt Service Routine
          ssfI2CInit          - Initialize port for I2C protocol
          ssfI2CReadFrom      - Start a READ process for I2C protocol data
          ssfI2CWriteTo       - Start a WRITE process for I2C protocol data 
      \endcode

<!-- NOTES: -->
       \note This file only supports the I2C protocol at this time.

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/syncserial.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.9  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.8  2008-03-25 19:14:38  msalimi
          - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
          - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
          - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
          - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
          - APU cooldown period is reduced to 2 minutes.
          - DPF related faults won't be captured when APU is off.
          - Freed code space by removing redundant code.
          - Faults are not displayed in DPF menu anymore.
          - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

          Revision 1.7  2008-02-21 20:08:04  msalimi
          - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
          - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
          - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

          Revision 1.6  2008-01-07 22:59:34  msalimi
          Fixed memory error problem. It was coming up when the memory board was not installed as the software was still trying to write to it.

          Revision 1.5  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.4  2007-11-30 18:52:08  msalimi
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

          Revision 1.3  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.2  2007-08-01 20:17:04  nallyn
          Added and modified commenting for Doxygen.

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "syncserial.h"         // Interface to Syncronous Serial
#include "main.h"               // Common definitions
#include "tfxstddef.h"
#include "menu.h"
#include "i2c.h"						//? Add


/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
//!Syncronous serial status
typedef enum ss_status
{
    SS_UNINITIALIZED = 0,   //!< Not Initialized
    SS_IDLE,                //!< Not doing nothing
    SS_WAIT_START,          //!< Wait for START condition
    SS_WAIT_ACK1,           //!< Wait for first ACK (address)
    SS_WAIT_ACK2,           //!< Wait for second ACK (location high byte)
    SS_WAIT_ACK3,           //!< Wait for second ACK (location low byte)
    SS_WAIT_RX_START,       //!< Wait for START condition when setting up receive
    SS_WAIT_RX_ACK,         //!< Wait for ACK of address when setting up receive 
    SS_RX_ACTIVE,           //!< Waiting for data to be received
    SS_RX_DONE,             //!< Data receive has filled buffer
    SS_WAIT_TX_ACK,         //!< Wait for ACK of address when setting up transmit 
    SS_TX_DONE,             //!< Data transmit has emptied buffer
    SS_TX_DONE1,
    SS_TX_DONE2,
    SS_FAULT                //!< Transfer aborted
} SSSTATUS;


/* ---------- I n t e r n a l   D a t a ---------- */

char ssState = SS_UNINITIALIZED;

char ssDevice;
int  ssLocation;
char *ssBufPtr;
char ssBufLen;
unsigned char ssRx;

/* ---------- G l o b a l   D a t a ---------- */
//!Current operation is done
#ifndef MM32F0133C7P		//? Add
unsigned char ssgDone;
#else
unsigned char ssgDone = TRUE;
#endif
//!Current operation is successful
unsigned char ssgSuccess;
unsigned char ssgAck = FALSE;

/* ----------------- F u n c t i o n s ----------------- */

// Interrupt handler for Synchronous Serial Port
/******************************************************************************/
/*!

<!-- FUNCTION NAME: ssfIsr -->
       \brief I2C Interrupt Service Routine

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void ssfIsr( void )
{
#ifndef MM32F0133C7P		//? Add
    if ( ( WCOL ) || ( SSPOV ) )
    {
        ssState = SS_FAULT;
    }
    
    switch ( ssState )
    {
        // Waiting for response after setting START condition
        case SS_WAIT_START:
        {
            // Set the slave node's WRITE address
            SSPBUF = ssDevice & 0xFE;
            
            // Wait for ACK from slave
            ssState = SS_WAIT_ACK1;
        }
        break;
            
        // Waiting for ACK after sending address
        case SS_WAIT_ACK1:
        {
			 if(!ACKSTAT)
           {
           	ssgAck = TRUE;
           }
           else
           {
           	ssgAck = FALSE;
           }
            // Write the high byte of device location
            SSPBUF = ( ssLocation >> 8 );
            
            // Wait for ACK from slave
            ssState = SS_WAIT_ACK2;
        }
        break;
        
        // Waiting for ACK after sending high byte of location
        case SS_WAIT_ACK2:
        {
            // Write the low byte of device location
            SSPBUF = ssLocation;
            
            // If we are to receive from the device
            if ( ssRx )
            {
                // Wait for ACK from slave when receiving
                ssState = SS_WAIT_ACK3;
            }
            else
            {
                // Wait for ACK from slave when transmitting
                ssState = SS_WAIT_TX_ACK;
            }
        }
        break;
        
        // Waiting for ACK after sending low byte of location
        case SS_WAIT_ACK3:
        {
            // Set another START condition
            RSEN = 1;
       
            // Wait for ACK from slave
            ssState = SS_WAIT_RX_START;
        }
        break;
        
        case SS_WAIT_RX_START:
        {
            // Write the slave node's READ address
            SSPBUF = ssDevice | 0x01;
            
            // Wait for ACK from slave
            ssState = SS_WAIT_RX_ACK;
        }
        break;
        
        case SS_WAIT_RX_ACK:
        {
            // Set receive enable
            RCEN = 1;
            
            // Set to active receiving
            ssState = SS_RX_ACTIVE;
        }
        break;
        
        // If we are waiting to receive data
        case SS_RX_ACTIVE:
        {
            // If data ready
            if ( BF )
            {
                // Put data into our rx buffer and count down length
                *ssBufPtr++ = SSPBUF;
                ssBufLen--;
                
                // If count is full
                if ( ssBufLen == 0 )
                {
                    // Do not ACK last data received
                    ACKDT = 1;
                    ACKEN = 1;
                }
                else
                {
                    // ACK the data received
                    ACKDT = 0;
                    ACKEN = 1;
                }
            }
            else
            {
                // If count is full
                if ( ssBufLen == 0 )
                {
                    // Assert stop condition
                    PEN = 1;
                    ssState = SS_RX_DONE;
                }
                else
                {
                    // Enable receive of next 
                    RCEN = 1;
                }
            }
        }
        break;
        
        case SS_RX_DONE:
        {
            // Stop Condition interrupt
            // Disable interrupt
            SSPIE = 0;
            
            // We are done
            ssState = SS_IDLE;
            
            // Receive was finished successfully;
            ssgSuccess = TRUE;
            ssgDone = TRUE;
#ifdef OBM
            // OBM record reading finished
            Set_OBM_Read_Done_Flags();
#endif // OBM
        }
        break;
        
        // Waiting for ACK after sending address or data
        case SS_WAIT_TX_ACK:
        {
            // If count is full
            if ( ssBufLen == 0 )
            {
                // Assert stop condition
                PEN = 1;
                ssState = SS_TX_DONE;
            }
            else
            {
                // Get data from our tx buffer and count down length
                SSPBUF = *ssBufPtr++;
                ssBufLen--;
            }
        }
        break;
        
        case SS_TX_DONE:
        {
            // Stop Condition
            // Disable interrupt
            SSPIE = 0;
            
            // We are done
            ssState = SS_IDLE;
            
            // Transmit was finished successfully;
            ssgSuccess = TRUE;
            ssgDone = TRUE;
        }
        break;
        
        
        default:
            // Stop synchronous serial interrupts
            SSPIE = 0;
            
            // We are done
            ssState = SS_IDLE;
            
            // Request is done but failed
            ssgSuccess = FALSE;
            ssgDone = TRUE;
            break;
    }

    // Clear interrrupt
    SSPIF = 0;
#else		///?OK
		//NOP
#endif
}

// Initialize Synchronous Serial Port for I2C usage
/******************************************************************************/
/*!

<!-- FUNCTION NAME: ssfI2CInit -->
       \brief Initialize port for I2C protocol

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void ssfI2CInit( void )
{
#ifndef MM32F0133C7P		//? Add
    // Set data direction for SS bits of i/o port C
	TRISC3 = 1;
	TRISC4 = 1;
    
    // Set bits 3-0 of SSPCON1 to b1000 (0x08) for Master Mode,
    // It is okay to clear the other bits for now
    SSPCON1 = 0x08;
    
    // Assuming master mode, set baud rate into SS address register
    // Assuming 20 MHz use 12 (0x0c) to set rate of 384.6 kHz
    SSPADD = 0x0c;
    
    // Enable Synchronous Serial hardware
    SSPEN = 1;
    
    // Ready for transmit or receive data
    ssState = SS_IDLE;
    
    // Command was finished successfully;
    ssgSuccess = TRUE;
    ssgDone = TRUE;
#else
		I2C_NVIC_EepromInit();
#endif
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ssfI2CReadFrom -->
       \brief Start a READ process for I2C protocol data

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  device   Device id on I2C bus
       @param[in]  location location of data to read on device
       @param[in]  bufPtr   Pointer to buffer into which data will be stored
       @param[in]  bufLen   Length of target buffer where data will be stored
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
unsigned char ssfI2CReadFrom( char device, int location, char *bufPtr, char bufLen )
{
#ifndef MM32F0133C7P		//? Add
    ssDevice = device;
    ssLocation = location;
    ssBufPtr = bufPtr;
    ssBufLen = bufLen;
    ssRx = TRUE;
    
    // Clear the fault state
    if ( ssState == SS_FAULT )
    {
        BF = 0;
        ssState = SS_IDLE;
    }
    
    // Only start data request if we are idle
    if ( ssState == SS_IDLE )
    {
        // New request started, not success, not done
        ssgSuccess = FALSE;
        ssgDone = FALSE;
        
        // Set to wait for Start interrupt
        ssState = SS_WAIT_START;
        
        // Enable START
        SEN = 1;
        
        // Enable Synchronous Serial interrupts
        SSPIE = 1;
        return TRUE;
    }
    else
    {
        // New request failed to start, no success, but done
        ssgSuccess = FALSE;
        ssgDone = TRUE;
        return FALSE;
    }
#else		///?OK
				I2C_SetDeviceAddr(I2C1, device);
				EEPROM_ReadBuff_Sub2B(I2C1, location, bufPtr, bufLen);
        return TRUE;
#endif
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: ssfI2CWriteTo -->
       \brief Start a WRITE process for I2C protocol data 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  device   Device id on I2C bus
       @param[in]  location location of data to write on I2C device
       @param[in]  bufPtr   Pointer to buffer from which data will be written to I2C
       @param[in]  bufLen   Length of target buffer from which data will be written
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
unsigned char ssfI2CWriteTo( char device, int location, char *bufPtr, char bufLen )
{
#ifndef MM32F0133C7P		//? Add
    ssDevice = device;
    ssLocation = location;
    ssBufPtr = bufPtr;
    ssBufLen = bufLen;
    ssRx = FALSE;
    
    // Clear the fault state
    if ( ssState == SS_FAULT )
    {
        BF = 0;
        ssState = SS_IDLE;
    }
    
    // Only start data request if we are idle
    if ( ssState == SS_IDLE )
    {
        // New request started, not success, not done
        ssgSuccess = FALSE;
        ssgDone = FALSE;
        
        // Set to wait for Start interrupt
        ssState = SS_WAIT_START;
    
        // Enable START
        SEN = 1;
    
        // Enable Synchronous Serial interrupts
        SSPIE = 1;
        return TRUE;
    }
    else
    {
        // New request failed to start, no success, but done
        ssgSuccess = FALSE;
        ssgDone = TRUE;
        return FALSE;
    }
#else		///?OK
				I2C_SetDeviceAddr(I2C1, device);
				EEPROM_WriteBuff_Sub2B(I2C1, location, bufPtr, bufLen);
        return TRUE;
#endif
}

