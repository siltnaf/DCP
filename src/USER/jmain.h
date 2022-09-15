/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file jmain.h
       \brief Header file for j1939 application layer for DCP2

<!-- DESCRIPTION: -->
       Header file for j1939 application layer for DCP2

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/jmain.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.3  2014/08/25 19:31:35  blepojevic
       Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

       Revision 1.2  2014/06/27 17:35:27  blepojevic
       Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.4  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.3 Tue May 6 18:47:35 2008 
         Added hardware level filter to CAN operation to receive only messages with 29 bit identifier.

         Revision 1.2 Fri Dec 21 20:36:44 2007 
         - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
         - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
         - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
         - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

         Revision 1.1 Fri Nov 30 18:54:04 2007 
         *** empty log message ***

   \endif
*/
/******************************************************************************/

#ifndef JMAIN_H
#define JMAIN_H

#include "can.h"
#include "j1939.h"
#include "main.h"

/*************************** J1939 definition and initialisation ****************************/
#define J1939RX_QUEUE_SIZE  20  // 6
#define J1939TX_QUEUE_SIZE  4

//***** J1939 identification parameters

// My default application source address as defined by J1939 table B2
#define  MY_PREFERRED_ADDRESS	             0x80     // DCP preferred address 128
//#define  MY_PREFERRED_ADDRESS	             0x18     // For debug only

// My application NAME fields
#define  MY_ARBITRARY_ADDRESS_CAPABILITY   TRUE     // Can use arbitrary SA to resolve a conflict
#define  MY_INDUSTRY_GROUP                 1        // J1939 TABLE B1: 1 = On-Highway Equipment
#define  MY_SYSTEM_INSTANCE                0        // First instance
#define  MY_DEVICE_CLASS                   0        // J1939 Table B12, Non Specific System
#define  MY_RESERVED_FIELD                 0        // Reserved by SAE, should be set to 0
#define  MY_DEVICE_FUNCTION                60        // J1939 Table B11, Auxiliary Power Unit = 1
#define  MY_DEVICE_INSTANCE_UPPER          0        // First instance 
#define  MY_DEVICE_INSTANCE_LOWER          0        // First instance (Port = 0, Center = 1, Starboard = 2)
#define  MY_MANUFACTURER_CODE              1850     // TeleFlex inc.
#define  MY_IDENTITY_NUMBER                0x1FFFFF // Teleflex Production serial number 

// FMI constant definitions
#define FMI_ABOVE_NORMAL		0
#define FMI_INVALID_DATA		2
#define	FMI_V_ABOVE_NORMAL		3
#define FMI_V_BELOW_NORMAL		4
#define FMI_I_BELOW_NORMAL		5
#define FMI_I_ABOVE_NORMAL		6
#define FMI_BAD_DEVICE			12
#define FMI_ABNORMAL_RATE		10
#define FMI_CONDITIONS_EXIST	31

#define RXMAILBOX_SIZE  3
#define TXMAILBOX_SIZE  3

#define DPF_STATUS_INDEX	0
#define DM1_INDEX			1
#define ACK_INDEX			2

#define DM1_MESSAGE_LENGTH  34  // Supporting up to 8 faults

#define SPN_MSB_BITS		0xE0
#define FMI_BITS			0x1F
#define CONV_METHOD_BIT		0x80
#define CM_VERSION4			0
                                            
#define REGEN_COMMAND_INDEX	0
#define APU_STATUS_INDEX  	1
#define REAL_TIME_INDEX		2


/*************************** J1939 Usefull Function ****************************************/

void Initialize_J1939_Variables(void);
void Check_J1939_Mailboxex(void);
void Tx_PGN65301(unsigned char *data);
void Tx_PGN65280(unsigned char *data);
void Tx_PGN65254(unsigned char *data);

//void J1939_CANTx_Process(void);

#endif

         
