/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file jmain.c
       \brief Use of J1939 stack in the DCP application

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

       CHANGE HISTORY:
           $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/jmain.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
  
           $Log: not supported by cvs2svn $
           Revision 1.3  2014/08/25 19:31:34  blepojevic
           Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

           Revision 1.2  2014/06/27 17:35:27  blepojevic
           Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

           Revision 1.1  2008/12/09 18:58:30  blepojevic
           No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

           Revision 1.6  2008-11-14 19:27:36  gfinlay
           - Added file header comment blocks for several files that were missing these.
           - Added debug uart support with serial.c and serial.h

            Revision 1.5 Tue Oct 28 15:54:28 2008 
            - APU can now turn on by pushing the APU ON/OFF key even if there is a critical shutdown. APU will not turn on with automatic functions if there is a critical shutdown reason. When APU is turned on with a critical shutdown, the error will be cleared in RAM so that it is triggered again if it still exists. This will lead to extra records in the faults memory.
            - Removed the display of APU shutdown reason after ESC/RESET key push.
            - Changed the design of sending a regeneration command message to DPF. This message is now a periodic message being sent out every second. In idle conditions the command will be zero, meaning that no action is required. When a regeneration or abort of regeneration is required, the appropriate command code will be sent. An acknowledgement is still required for this command. If the ACK is not received after 60 retrials, the APU will be shutdown displaying an "ACKNOWLEDG ERROR".

            Revision 1.4 Tue Mar 25 19:14:38 2008 
            - Put the EXT_MEM compile switch for all the memory logging activities that are not needed for normal operation.
            - Cancelled the five minutes warning period before going to regen. This period will be in effet only if the user has requested a regen using the menu AND that APU is off. This period then will serve as a warmup period for APU.
            - Cancelled user confirmation request for installing DPF when it first is detected. It will now install the DPF as soon as it is detected without user input or confirmation. Menu key will now clear popup messages if they are displayed.
            - Cancelled DPF "INSTALL" option in the "SYSTEM CONFIG" menu.
            - APU cooldown period is reduced to 2 minutes.
            - DPF related faults won't be captured when APU is off.
            - Freed code space by removing redundant code.
            - Faults are not displayed in DPF menu anymore.
            - DM1 faults will be cleared in 3 seconds if DPF is still sending status messages every one second.

            Revision 1.3 Thu Feb 21 20:08:04 2008 
            - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
            - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
            - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

            Revision 1.2 Fri Dec 21 20:36:44 2007 
            - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
            - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
            - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
            - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

            Revision 1.1 Fri Nov 30 18:53:35 2007 
            *** empty log message ***

       \endif
*/
/******************************************************************************/
#include "jmain.h"
#include "DPF.h"

/* -------- Function Prototypes -------- */
static void RxHandle_PGN65300(void);
static void RxHandle_PGN65226(void);
static void RxHandle_PGN59392(void);
static void Process_SPN_FMI(j1939_RxMsg_Struct *message, unsigned char index);

//  ************** Local Variables  **************
unsigned char PreferedSA;

can_Msg_Struct J1939Rx_Queue[J1939RX_QUEUE_SIZE];
can_Msg_Struct J1939Tx_Queue[J1939TX_QUEUE_SIZE];
can_Queue_Struct J1939Queue;

// ************** Rx Buffers **************
unsigned char RxPgn_65300[8];
unsigned char RxPgn_65226[DM1_MESSAGE_LENGTH];
unsigned char RxPgn_59392[8];
//unsigned char RxPgn_65263[50];

//  ************** J1939 Rx mailbox  **************
j1939_RxMsg_Struct J1939_RxMailbox[RXMAILBOX_SIZE]={
// Full Trunk Rx_InProg No_Over PgnType  PGN   Ageing_Ctr MsgSize Org_Addr Org_Filter Dst_Addr BufferSize DataBuffer
   0,   0,    0,        1,      0,       65300, 255,       0,      0,          0xFF,      0,       8,       RxPgn_65300,	// DPF Status
   0,   0,    0,        1,      0,       65226, 255,       0,      0,          0xFF,      0, DM1_MESSAGE_LENGTH,      RxPgn_65226,	// DM1
   0,   0,    0,        1,      0,       59392, 255,       0,      0,          0xFF,      0,       8,       RxPgn_59392,	// ACK
   //0,   0,    0,        1,      0,       65263, 255,       0,      0,          0xFF,      0,      50,       RxPgn_65263
};
// NOTE: IF Fast Packet message (NMEA2000) are include in the J1939 Rx Mailbox, do not forget to modify J1939.h

//  ************** Tx Buffers  **************
unsigned char TxPgn_65301[8]={0,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA};
unsigned char TxPgn_65254[8]={NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA,NO_DATA};
unsigned char TxPgn_65280[8]={0,0,0,0,NO_DATA,NO_DATA,0,0};
//unsigned char TxPgn_61184[24]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};

//  ************** J1939 Tx mailbox  **************
j1939_TxMsg_Struct J1939_TxMailbox[TXMAILBOX_SIZE]={
// TxReq  Period  TxInProg PgnType  CntrSet  Cntr   PGN         Priority    Dest_Addr  DataSize     Data
   0,     0,      0,		0,     	0,  	0,      65301,  	6,		    0xFF,     	8,      	TxPgn_65301,	// Regeneration command (DPF)
   0,     1,      0,		0,     	100,  	100,    65280,  	6,		    0xFF,     	8,      	TxPgn_65280,	// APU Status
   0,     1,      0,        0,      100,	100,    65254,  	6,          0xFF,     	8,      	TxPgn_65254, 	// Real time
   //0,     0,      0,        0,      0,      0,    61184,    6,      0x19,       19,         TxPgn_61184, 
};

// J1939 Communication Parameter Initialisation
j1939_Parameter_Struct J1939_Parameter={
   &J1939Queue,                  // Pointer to the CAN queue used by the J1939 driver
   J1939_RxMailbox,              // RxMailbox Address
   RXMAILBOX_SIZE,               // RxMailbox Size
   J1939_TxMailbox,              // TxMailbox Address
   TXMAILBOX_SIZE,               // TxMailbox Size
   (unsigned char*) &PreferedSA, // Prefered Source Address

   // Definition of the J1939 Name Fields
   MY_ARBITRARY_ADDRESS_CAPABILITY,   
   MY_INDUSTRY_GROUP,                 
   MY_SYSTEM_INSTANCE,                
   MY_DEVICE_CLASS,                   
   MY_RESERVED_FIELD,                 
   MY_DEVICE_FUNCTION,                
   MY_DEVICE_INSTANCE_UPPER,          
   MY_DEVICE_INSTANCE_LOWER,          
   MY_MANUFACTURER_CODE,              
   MY_IDENTITY_NUMBER                
};


/******************************************************************************/
/*!

<!-- FUNCTION NAME: Initialize_J1939_Variables -->
       \brief This routine is called once at startup.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Initialize_J1939_Variables(void)
{
    // Initialisation of the RX and TX J1939 Queue and driver
    J1939Queue.Rx.Buffer = &J1939Rx_Queue[0];
    J1939Queue.Rx.Size = J1939RX_QUEUE_SIZE;
    J1939Queue.Tx.Buffer = &J1939Tx_Queue[0];
    J1939Queue.Tx.Size = J1939TX_QUEUE_SIZE;
    can_InitQueue(&J1939Queue);              // Initialisation of the CAN Software Queue
    can_InitHardware();                      // Initialise the CAN Hardware

    // Initialise information for the J1939 Driver
    // Normally the next three value are loaded with value as defined in the production Process
    PreferedSA =  MY_PREFERRED_ADDRESS;         
    J1939_Parameter.Name_Fields.ECU_Instance = 0;
    J1939_Parameter.Name_Fields.Identity_Number = MY_IDENTITY_NUMBER;

    j1939_Initialisation(&J1939_Parameter);  
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Check_J1939_Mailboxex -->
       \brief This routine is called once at every main loop. It looks for 
              incoming J1939 messages that are defined in this application. 
              If so, then it calls the related handling routine.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Check_J1939_Mailboxex(void)
{
    if(J1939_RxMailbox[0].Full == 1)
    {
       // Call handling function ...
       RxHandle_PGN65300();
       J1939_RxMailbox[0].Full = 0;
    }
    if(J1939_RxMailbox[1].Full == 1)
    {
       // Call handling function ...
       RxHandle_PGN65226();
       J1939_RxMailbox[1].Full = 0;
    }
    if(J1939_RxMailbox[2].Full == 1)
    {
       // Call handling function ...
       RxHandle_PGN59392();
       J1939_RxMailbox[2].Full = 0;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: RxHandle_PGN65300 -->
       \brief This routine is called when a J1939 message with this PGN has been
              received.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void RxHandle_PGN65300(void)
{
    Update_DPF_Status (&J1939_RxMailbox[DPF_STATUS_INDEX]);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: RxHandle_PGN65226 -->
       \brief This routine is called whenever a DM1 message is received.
              It is assumed that the DM1 message will contain only up to 4
              faults. 

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void RxHandle_PGN65226(void)
{
    unsigned char i;
    j1939_RxMsg_Struct *message;

    message = &J1939_RxMailbox[DM1_INDEX];

    for (i= 5 ; i < DM1_MESSAGE_LENGTH; i += 4)
        Process_SPN_FMI(message, i);

    // Clear the buffer
    for (i= 0 ; i < DM1_MESSAGE_LENGTH; i++)
        message->Data[i] = 0;
}

static void Process_SPN_FMI(j1939_RxMsg_Struct *message, unsigned char index)
{
    unsigned long spn;
    unsigned char fmi;

    // Only conversion method version 4 is supported here
    if ((message->Data[index] & CONV_METHOD_BIT) == CM_VERSION4)
    {
        spn = ((message->Data[index - 1] & SPN_MSB_BITS) >> 5);
        spn <<= 8;
        spn += message->Data[index - 2];
        spn <<= 8;
        spn += message->Data[index - 3];

        fmi = ((message->Data[index - 1] & FMI_BITS));

        DPF_DM1_Faults(spn, fmi);
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: RxHandle_PGN59392 -->
       \brief This routine is called when a J1939 message with this PGN has been
              received.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void RxHandle_PGN59392(void)
{
    j1939_RxMsg_Struct *message;

    message = &J1939_RxMailbox[ACK_INDEX];

    if (message->Org_Addr == DPF_SOURCE_ADDRESS)
    {
        DPF_Acknowledgement_Received(message);
    }
}

/*
void J1939_CANTx_Process(void)
{

}
*/

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Tx_PGN65301 -->
       \brief This routine is called when a command needs to be sent to DPF. 

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Tx_PGN65301(unsigned char *data)
{
    j1939_TxMsg_Struct *TxMailbox;

    TxMailbox = &J1939_Parameter.TxMailbox[REGEN_COMMAND_INDEX];
    TxMailbox->Data[0] = data[0];

    TxMailbox->TxReq = J1939_REQUEST;	// TRUE
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Tx_PGN65280 -->
       \brief This routine is called every second to send out an APU status message.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Tx_PGN65280(unsigned char *data)
{
    j1939_TxMsg_Struct *TxMailbox;

    TxMailbox = &J1939_Parameter.TxMailbox[APU_STATUS_INDEX];

    TxMailbox->TxReq = J1939_REQUEST;	// TRUE
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Tx_PGN65254 -->
       \brief This routine is called every second to send out a real time & date message.

<!-- PURPOSE: -->

<!-- PARAMETERS: -->

<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Tx_PGN65254(unsigned char *data)
{
    j1939_TxMsg_Struct *TxMailbox;

    TxMailbox = &J1939_Parameter.TxMailbox[REAL_TIME_INDEX];

    TxMailbox->TxReq = J1939_REQUEST;	// TRUE
}