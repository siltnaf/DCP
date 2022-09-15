/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file syncserial.h
       \brief Header file defining the interface to Synchronous Serial driver for PIC18

<!-- DESCRIPTION: -->
    This file provides the interface to the synchronous serial driver
    for a PIC 18. So far the only driver only supports I2C.
         \code
           GLOBAL FUNCTIONS:
             ssfIsr              - Interrupt Service Routine
             ssfI2CInit          - Initialize port for I2C protocol
             ssfI2CReadFrom      - Start a READ process for I2C protocol data
             ssfI2CWriteTo       - Start a WRITE process for I2C protocol data 
         \endcode

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/syncserial.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.5  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.4  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.3  2007-11-30 18:52:08  msalimi
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

          Revision 1.2  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

#ifndef SYNCSERIAL_H
#define SYNCSERIAL_H

/* ---------- D e f i n i t i o n s ---------- */

    
/* ---------- G l o b a l   D a t a ---------- */

extern unsigned char ssgDone;
extern unsigned char ssgSuccess;
extern unsigned char ssgAck;

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

void ssfIsr( void );
void ssfI2CInit( void );
unsigned char ssfI2CReadFrom( char device, int location, char *bufPtr, char bufLen );
unsigned char ssfI2CWriteTo( char device, int location, char *bufPtr, char bufLen );


#endif


