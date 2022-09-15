/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file j1939.h
       \brief Header file for Megatech J1939 stack

<!-- DESCRIPTION: -->
       SAE J1939/NMEA2000 functions to construct and process network and application messages.

<!-- NOTES: -->
       \note 
           Author: Martial Bilodeau
           Date: February 22, 2005
           Modification: A00 => First Release based on J1939.c revision A11 at Megatech Electro
                                This version was modified to include the reception and transmission
                                of Fast Packet. Ba adding this the driver can be used for J1939
                                or NMEA2000.

           Modification: A01 => (July 6, 2006): The following Bug was detected and corrected. When the 
                                reception of multi packet PGN was in progress and a sigle
                                frame message was receive for the same PGN, the single frame
                                message was accepted and written over the multi packet message.
                                Now, when a single frame message is receive, the driver check if
                                a multipacket reception of the PGN is in progress.
               
                         A02 => (January 24, 2006): When Multi-Packet message are transmitted
                                the driver have to be certain that only one transmission per
                                destination address is in progress. It means that only one 
                                multi-packet message can be broadcast at the time. Same thing
                                for destination specific transmission.
                                
                                Resolution of a problem of message reception. When more than 1 Mailbox
                                was defined for 1 PGN and ORG Filter was used, only the first MailBox was used.
                                
                                Delay between 2 Multi-Packet is change between 100mS to 50 mS

                                Addition of Option tu support or Not Fast Packet and Multi-Packet.
                                Code size is scale depending on option settings

                         A03 => (April 26, 2007): Adaptation for PIC18F4680


       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/j1939.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.5  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.4 Mon Sep 17 18:41:35 2007 
         DPF related modifications

         Revision 1.3 Mon Aug 27 22:19:08 2007 
         CAN communication working.

         Revision 1.2 Tue Jul 10 22:37:16 2007 
         Remove comment containing lengthy example for sample main code.

         Revision 1.1 Thu Jul 5 20:10:12 2007 
         Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

   \endif
*/
/******************************************************************************/

#ifndef  j1939_H
#define  j1939_H

#include "can.h"
/**************************** Application Specific Parameters *********************************/

#define  SUPPORT_FAST_PACKET_RX   FALSE   // If the driver is used to ommunicate in NMEA2000 configuration
#define  SUPPORT_FAST_PACKET_TX   FALSE  // If the driver is used to ommunicate in NMEA2000 configuration
#define  SUPPORT_MULTIPACKET      TRUE   // If MultiPacket Protocol needs to be supported, Set to true

#define  J1939_COMMANDED_ADDRESS_ENABLED   FALSE      // Commanded address message can alter SA

#define  J1939_MAX_NUMBER_PACKET_BEFOFE_CTS    3      // Maximum number of Packet that can be received 
                                                      // or transmitted between two CTS

#define J1939_NUMBER_RX_MULTI_PACKET_SESSION   2      // Number of Rx Multi Packet Session
#define J1939_NUMBER_TX_MULTI_PACKET_SESSION   1      // Number of TX Multi Packet Session

#define J1939_NUMBER_RX_FAST_PACKET_SESSION   0      // Needs to correspond to number of Fast Packet
                                                     // PGN defines in the J1939 Rx Mailbox
#define J1939_NUMBER_TX_FAST_PACKET_SESSION   0      // Needs to correspond to number of Fast Packet
                                                     // PGN defines in the J1939 Tx Mailbox

/******************************* Public Structure Definitions *******************/

// Structure of the RX Mailbox used between the J1939 and the high application Layer
typedef struct{
  unsigned char   Full  :1;             // [ 0:Empty , 1:Full ]
  unsigned char   Trunk :1;             // [ 0:No Trunk allowed, 1: Trunk of MultiPacket or Fast Packet Message is allowed ]
  unsigned char   Rx_InProgress :1;     // Flag used by the J1939 Driver Only. This flag is used to avoid that
                                        // a Rx PGN was overwriten by a single Packet PGN when a Multi Packet
                                        // reception is in progress;
  unsigned char   No_OverWrite :1;      // If this flag is SET the J1939 driver doesn't update the PGN untill
                                        // that the PGN is not acknowledged (Full = 0)
  unsigned char   Pgn_Type     :1;      // 0 = Regular PGN, 1 = Fast Packet PGN
  unsigned long   PGN;        // PGN with Group Extension (Including DataPage and Reserved)
  unsigned char   Ageing_Ctr; // Aging Counter (Increment each 100ms, Stack at 0xFF, Clear at Msg Reception)
  unsigned int    MsgSize;    // Message Size
  unsigned char   Org_Addr;   // Address of the PGN Originator
  unsigned char   Org_Filter; // 0xFF         => Filter is not used
                              // Others Value => The PGN is dedicated to only one Originator and work as a filter
  unsigned char   Dst_Addr;   // Destination Address (Can by MyAddress or 0xFF when the message is global)
  unsigned int    BufferSize; // Data Buffer Size (Fixed and defined by the application)
  unsigned char   *Data;      // Data Buffer Pointer
}j1939_RxMsg_Struct;       

// Structure of the TX Mailbox used between the J1939 and the high application Layer
typedef struct{
  unsigned char                    TxReq  :1;    // Transmit Request [0:Idle, 1:Request]
  unsigned char                    Period :1;    // Periodic Transmit [0:Single, 1:Periodic]
  unsigned char              TxInProgress :1;    // Flag used when the transmission is Multi
                                                 // Packet. This flag need to initialised at 0;
  unsigned char              Pgn_Type     :1;    // 0 = Regular PGN, 1 = Fast Packet PGN
  unsigned char                    CntrSet;      // Transmit Period Setting (Time Base of 10mS)
  unsigned char                    Cntr;         // Transmit trigger counter
  unsigned long                    PGN;          // Message Identification Character;
  unsigned char                    Priority;     // Priority of the PGN
  unsigned char                    Dest_Addr;    // Destination Address (Needed only if message is in PDU1 Format)
  unsigned int                     DataSize;     // Data Buffer Size
  unsigned char                   *Data;         // Data Buffer Pointer
}j1939_TxMsg_Struct;         

// Communication Parameter between  J1939 and the high application Layer
typedef struct{
  can_Queue_Struct         *Queue;           // Can Queue pointer
  j1939_RxMsg_Struct       *RxMailbox;       // RxMailbox Address
  unsigned char             RxMailboxSize;   // RxMailbox Size
  j1939_TxMsg_Struct       *TxMailbox;       // TxMailbox Address
  unsigned char             TxMailboxSize;   // TxMailbox Size
  unsigned char            *PreferedSA;      // Prefered Source Address (This first address
                                             // that the module will try to claim at Power Up)  
  struct{
    // Parameter used in the J1939 Name Fields (See J1939-81 for details)
    // All the Name parameter have an associated length. Be certain to set 
    // this values lower or equal to the maximum.
    // Ex: The Industry Group is a 3 Bits fields => Max possible values = 7
    unsigned char   Self_Cfg_Addr  :1;  // Self Configurable Address
    unsigned char   Industry_Group;     // Defined by the SAE Commitee, see J1939 Appendix B1
    unsigned char   Vehicle_Sys_Inst;   // Vehicle System Instance (Indicate the occurence of a particular Vehicle System)  
    unsigned char   Vehicle_System;     // Defined by the SAE Commitee, see J1939 Appendix B Table B12
    unsigned char   Reserved       :1;  // Reserved for future assigment (Should be set to Zero)   
    unsigned char   Function;           // Defined by the SAE Commitee, see J1939 Appendix B Table B11
    unsigned char   Function_Instance;  //
    unsigned char   ECU_Instance;       //
    unsigned int    Manufacturer_Code;  // Defined by the SAE Commitee, see J1939 Appendix B Table B10
    unsigned long   Identity_Number;    // Unique Identity Number assigned by the manufacturer
                                        // This number is unique for each module and needs to be assigned 
                                        // by the test Jig. It needs to be save in Flash.
  }Name_Fields;                         
}j1939_Parameter_Struct;                

// Status of the J1939 Application
typedef struct{
   unsigned char  AddClaim  :1;   // Address Claim Process (0:In Progress 1:Done)
   unsigned char  CAN_BusOK :1;   // CAN Bus State (0: CAN Bus is down, 1: CAN Bus is working)
   // Others Flag needed
   unsigned char  SA;             // Current Source Address (Address used after the address claim Challenge)
                                  // As a first try, the module try to claimed the address specified
                                  // in J1939_Name_Struct. This register is the result of the address
                                  // claim challenge.
}j1939_Status_Struct;

/******************************* Public Constants Definition  **********************************/

// Messages Status Codes
#define J1939_IDLE       0
#define J1939_REQUEST    1
#define J1939_SINGLE     0
#define J1939_PERIODIC   1
#define J1939_EMPTY      0
#define J1939_FULL       1


/******************************* Public Variable Definition ***********************************/

// Status of the J1939 application 
extern j1939_Status_Struct j1939_Status;


/******************************* Public Function Prototypes ***********************************/

void  j1939_Initialisation(j1939_Parameter_Struct *Parameter);

void  j1939_ProcessRefresh(void);

void  j1939_TicksTimerTask(void);


#endif 
