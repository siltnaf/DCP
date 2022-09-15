/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file j1939.c
       \brief SAE J1939/NMEA2000 functions to construct and process network and application messages.

<!-- DESCRIPTION: -->
       SAE J1939/NMEA2000 functions to construct and process network and application messages.
       Author: Martial Bilodeau
       Date: February 22, 2005
  

<!-- NOTES: -->
       \note
           Modification: A00 => First Release based on J1939.c revision A11 at Megatech Electro
                                This version was modified to include the reception and transmission
                                of Fast Packet. By adding this the driver can be used for J1939
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
       Copyright © 2005 Teleflex, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/j1939.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.3  2014/04/16 14:07:08  blepojevic
       Added diagnostic of APU and CCU

       Revision 1.2  2013/09/19 13:07:44  blepojevic
       Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.4  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.3 Fri Nov 30 18:52:07 2007
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

         Revision 1.2 Mon Aug 27 22:19:08 2007 
         CAN communication working.

         Revision 1.1 Thu Jul 5 20:10:12 2007 
         Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

   \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s -------------- */
#include "j1939.h"
#include "std.h"
#include "can.h"

/* ---------- I n t e r n a l   D e f i n i t i o n s ------ */

#define  J1939_RESERVED_QUEUE_SPACE            2     // Used to define how many element in the Queue
                                                     // are reserved for the J1939 Process.
                                                     // This reserved space is used to transmit all the kind
                                                     // of acknowledge, request, response to Multi Packet.
                                                     // This assition was done to avoid that the J1939 RX Mailbox
                                                     // Fill completely the Mailbox.

#define  J1939_MAX_PF_PDU1FORMAT           239    // Maximum value that PDU format can have (EF)

// Request message parameters
#define  J1939_REQUEST_PGN                 59904  // 0x00EA00  PDU1 format
#define  J1939_REQUEST_PRIORITY            6      // Default priority
#define  J1939_GLOBAL_ADDRESS              255    // Global destination address

// Acknowledgment message parameters
#define  J1939_ACKNOWLEDGMENT_PGN          59392  // 0x00E800  PDU1 format
#define  J1939_ACKNOWLEDGMENT_PRIORITY     6      // Default priority
#define  J1939_GROUP_FUNCTION_VALUE        0      // Group function of PGN being acknowledged
enum     J1939_ACKNOWLEDGMENT_STATE      { POSITIVE_ACKNOWLEDGMENT,
                                           NEGATIVE_ACKNOWLEDGMENT,
                                           PGN_SUPPORTED_ACCESS_DENIED,
                                           PGN_SUPPORTED_CANNOT_RESPOND };

// Address claimed message parameters
#define  J1939_ADDRESS_CLAIM_ENABLED       TRUE   // Address claim after power on self test
#define  J1939_ADDRESS_CLAIM_PGN           60928  // 0x00EE00  PDU1 format
#define  J1939_ADDRESS_CLAIM_PRIORITY      6      // Default priority

#ifdef NMEA2000
#define  J1939_ADDRESS_CLAIM_NULL_ADDRESS  254
#define  J1939_ADDRESS_CLAIM_MAX_ADDRESS   251
#else 
#define  J1939_ADDRESS_CLAIM_NULL_ADDRESS  254
#define  J1939_ADDRESS_CLAIM_MAX_ADDRESS   254
#endif

// Multi Packet Transmission Message Parameter
#define J1939_MULTI_PACKET_TPCM_PGN        60416   // PGN used for the Transport Protocol Connection Management
#define J1939_MULTI_PACKET_PRIORITY        7       // Priority Status of the Multi Packet Message
#define J1939_MULTI_PACKET_TPDT_PGN        60160   // PGN Used for the Data Transfert
#define J1939_MULTI_PACKET_TPCMRTS         16      // Control Byte Value for TP.CM_RTS
#define J1939_MULTI_PACKET_TPCMCTS         17      // Control Byte value for TP.CM_CTS
#define J1939_MULTI_PACKET_ENOFMSGACK      19      // Control Byte value for TP.CM_EndOfMsgACK
#define J1939_MULTI_PACKET_TPCONNABORT     255     // Control Byte Value for TP.Conn_Abort
#define J1939_MULTI_PACKET_TPCMBAM         32      // Control Byte Value for TP.CM_BAM

// Proprietary A message parameters
#define  J1939_PROPRIETARY_A_PGN           61184  // 0x00EF00  PDU1 format
#define  J1939_PROPRIETARY_A_PRIORITY      6      // Default priority

// Proprietary B message parameters
#define  J1939_PROPRIETARY_B18_PGN         65304  // 0x00FF18  PDU2 format
#define  J1939_PROPRIETARY_B18_PRIORITY    6      // Default priority

// Proprietary B message parameters
#define  J1939_PROPRIETARY_B_PRIORITY      	6

// Prprietary B APU module
#define  J1939_PROP_B_APU_STATUS_PGN		65280

// Prprietary B SPM module
#define  J1939_PROP_B_PWR_SOURCE_INFO_PGN			65290
#define  J1939_PROP_B_LOAD_INFO_PGN					65291
#define  J1939_PROP_B_PWR_SOURCE_MANAGEMENT_PGN		65292
#define  J1939_PROP_B_LOAD_MANAGEMENT_PGN			65293

// Prprietary B DPF module
#define  J1939_PROP_B_DPF_STATUS_PGN		65300
#define  J1939_PROP_B_DPF_COMMAND_PGN		65301

// Real time
#define  J1939_REAL_TIME_PGN		65254

// Diagnostic messages
#define J1939_DM1_PGN 		65226
#define J1939_DM3_PGN 		65228

// Commanded address message parameters - requires the BAM form of the transport protocol
#define  J1939_COMMANDED_ADDRESS_PGN       65240  // 0x00FED8  PDU2 format
#define  J1939_COMMANDED_ADDRESS_PRIORITY  6      // Default priority

// Transmitted signal ranges for ASCII
#define  J1939_ASCII_MIN_VALID_RANGE       0x01
#define  J1939_ASCII_MAX_VALID_RANGE       0xFE
#define  J1939_ASCII_ERROR_INDICATOR       0x00
#define  J1939_ASCII_NOT_AVAILABLE         0xFF

// Transmitted signal ranges for 1-byte
#define  J1939_1BYTE_MIN_VALID_RANGE       0x00
#define  J1939_1BYTE_MAX_VALID_RANGE       0xFA
#define  J1939_1BYTE_ERROR_INDICATOR       0xFE
#define  J1939_1BYTE_NOT_AVAILABLE         0xFF

// Transmitted signal ranges for 2-byte
#define  J1939_2BYTE_MIN_VALID_RANGE       0x0000
#define  J1939_2BYTE_MAX_VALID_RANGE       0xFAFF
#define  J1939_2BYTE_ERROR_INDICATOR       0xFE00
#define  J1939_2BYTE_NOT_AVAILABLE         0xFF00

// Transmitted signal ranges for 4-byte
#define  J1939_4BYTE_MIN_VALID_RANGE       0x00000000
#define  J1939_4BYTE_MAX_VALID_RANGE       0xFAFFFFFF
#define  J1939_4BYTE_ERROR_INDICATOR       0xFE000000
#define  J1939_4BYTE_NOT_AVAILABLE         0xFF000000

// Fast Packet Timing Adjustment
#define FAST_PACKET_RX_TIME_OUT      75     // Maximum time between the reception of 2 Fast Packet of the same message
#define FAST_PACKET_TX_INTER_FRAME    0     // Time between the transmission of 2 packet in step of 10 mS

/***********************************************************************************************/
/******************************* Private Macro Definitions *************************************/
/***********************************************************************************************/

/***********************************************************************************************/
/******************************* Private Structure Definitions ********************************/
/***********************************************************************************************/

// To avoid problem with compiler compatibility, each BitFields needs to be grouped
// by Byte.  Thats why the next Fields is construct in that ways !!!

typedef struct j1939_NAME_Fields_Struct {    
  unsigned char  IdentityNumber_Byte1;
  unsigned char  IdentityNumber_Byte2;
  struct{
    unsigned char  IdentityNumber_Byte3     :5;
    unsigned char  ManufacturerCode_Byte3   :3;
  }Byte3;
  unsigned char  ManufacturerCode_Byte4;
  struct{
    unsigned char  EcuInstance              :3;
    unsigned char  DeviceFunctionInstance   :5;
  }Byte5;
  unsigned char  DeviceFunction;
  struct{
    unsigned char  Reserved                 :1;
    unsigned char  VehicleSystem            :7;
  }Byte7;
  struct{
    unsigned char  VehicleSystemInstance    :4;
    unsigned char  IndustryGroup            :3;
    unsigned char  ArbitraryAddressCapable  :1;
  }Byte8;
} j1939_NAME_Fields_Struct_Type;             // Bit Mapping is OK for Atmel

/*************************** J1939 Protocol Structure Definitions *****************************/

typedef union j1939_Id_Struct {              
  unsigned long  Identifier;     // 29-bits
  struct {
    unsigned char SourceAddress;
    unsigned char PduSpecific;
    unsigned char PduFormat;
    struct{
      unsigned char DataPage       :1;
      unsigned char Reserved       :1;
      unsigned char Priority       :3;
      unsigned char UnusedMSB      :3;
    }BitField;
  } IdField;
} j1939_Id_Struct_Type;                      // Bit Mapping is OK for Atmel


typedef struct j1939_SingleFrame_Message_Struct {      
  j1939_Id_Struct_Type           MsgId;              // 29-bits message identifier
  unsigned char                  ByteCntr;           // Single frame data byte counter
  unsigned char                 *BytePtr;            // Single frame data byte pointer
} j1939_SingleFrame_Message_Struct_Type;             // Bit Mapping is OK  for Atmel

typedef struct j1939_Request_Message_Struct {          
  unsigned char  PduSpecific;
  unsigned char  PduFormat;
  struct{
    unsigned char  DataPage         :1;
    unsigned char  Reserved         :1;
    unsigned char  UnusedMSB        :6;
  }BitField;
} j1939_Request_Message_Struct_Type;                  // Bit Mapping is OK for atmel 


// This structure is a modified version of the regular J1939 29 bits identifier
// By using this structure to pass the information between the application and
// the J1939 driver, the processing is reduce and the code is less complicated

typedef union j1939MailBox_ID_Struct {              
  unsigned long  Identifier;     
  struct {
    unsigned char PduSpecific;
    unsigned char PduFormat;
    struct{
      unsigned char DataPage       :1;
      unsigned char Reserved       :1;
      unsigned char UnusedMSB      :6;
    }BitField;
    unsigned char Unused;
  } IdField;
}j1939MailBox_ID_Struct_Type;                      // Bit Mapping is OK for atmel



typedef struct j1939_Acknowledgment_Message_Struct {
  unsigned char  AckStatus;
  unsigned char  GroupFnct;
  unsigned char  Byte3;
  unsigned char  Byte4;
  unsigned char  Byte5;
  unsigned char  PduSpecific;
  unsigned char  PduFormat;
  struct{
    unsigned char  DataPage         :1;
    unsigned char  UnusedMSB        :7;
  }BitField;
} j1939_Acknowledgment_Message_Struct_Type;    // Used by the Address Claim Process, Bit Mapping is OK

/******************************* Grafcet Structure Definitions ********************************/

typedef struct j1939_Address_Claim_State_Variables_Struct {
  unsigned   IdleState           :1;
  unsigned   StartRandomDelay    :1;
  unsigned   Start250MsDelay     :1;
  unsigned   DelayComplete       :1;
  unsigned   SelectStartAddress  :1;
  unsigned   FetchNextMyAddress  :1;
  unsigned   CommandedAddress    :1;
  unsigned   AddressClaimed      :1;
  unsigned   CannotClaimAddress  :1;
  unsigned   IWin                :1;
  unsigned   ILose               :1;
  unsigned   CanMsgSent          :1;
  unsigned   NoMessage           :1;
  unsigned   RequestPgn          :1;
} j1939_Address_Claim_State_Variables_Struct_Type;   // Bit Mapping is not important

/************* Multi Packet Session Structure Definition *************/
typedef struct j1939_RxMultiPacket_State_Variables_Struct {
  unsigned char   InProgress    :1;   // Reception of MultiPacket (0=Idle, 1=InProgress)
  unsigned char   DelayTimer;         // Delay timer use to determine if a Multi Packet session
                                      // needs to be abort.
  unsigned char   DA;                 // Destination address of the MultiPacket message 
                                      // It can take only two values, Global(255) or specific(MyAddress)
  unsigned char   Originator;         // Source Address of the Data Originator
  unsigned long   PGN;                // PGN of the requested data with Reserved and DataPage
  unsigned int    Msg_Size;           // Total Message Size, Number of bytes
  unsigned int    Trunk_Size;         // When "Trunk_Size" of Bytes was received, the data are no more
                                      // written in the Buffer. But the session continue. The Data
                                      // are simply not proceed. This feature can be usefull for
                                      // Multi Packet PGN with variable length like FMI Code.
  unsigned char   Num_Byte_Rx;        // Number of received bytes. Use to determine if the message
                                      // was received completely.
  unsigned char   Packet_Size;        // Total Number of Packet
  unsigned char   Sequence_Number;    // Counter use to keep track of the packet sequence number (1-255)
  unsigned char   Max_Num_Packet_CTS; // Maximum number of packets that can be received before 
                                      // sending a CTS. This number is the smallest one between
                                      // the Originator and the Responder
  unsigned char   Num_Packet_CTS;     // The Sequence number is used to determine if it's the
                                      // time to transmit a CTS following Max_Num_Packet_CTS value
                                      // Num_Packet_CTS is loaded with Max_Num_Packet_CTS and decrement 
                                      // at the reception of each packet. When it reach ZERO, CTS is send.

  unsigned char   *Byte_Pgm_Ptr;      // Pointer to the Byte that need to be Program
  j1939_RxMsg_Struct *RxMsgPtr;       // Pointer to the Receive Mailbox. Value == NULL when not used
}j1939_RxMultiPacket_State_Variables_Struct_Type;    // Bit Mapping is not important

typedef struct j1939_TxMultiPacket_State_Variables_Struct {
  unsigned char   InProgress    :1;   // Reception of MultiPacket (0=Idle, 1=InProgress)
  unsigned char   DelayTimer;         // Delay timer use to determine if a Multi Packet session
                                      // needs to be abort.
  unsigned char   NextTxTimer;        // Used to determine when the next Data Transfert is needed
  unsigned char   DA;                 // Destination address of the MultiPacket message
                                      // It can take any values between 0 and 255 ( 255 = Global Session)
  unsigned char   Originator;         // Source Address of the Data Originator (It correspond to my address)
  unsigned long   PGN;                // PGN of the transmitted Data with Reserved and DataPage
  unsigned int    Msg_Size;           // Total Message Size, Number of bytes
  unsigned char   Packet_Size;        // Total Number of Packet
  unsigned char   Sequence_Number;    // Counter use to keep track of the packet sequence number (1-255)
  unsigned char   Max_Num_Packet_CTS; // Maximum number of packets that can be transmit before 
                                      // reception of a CTS. This number is the smallest one between
                                      // the Originator and the Responder
  unsigned char   Num_Packet_CTS;     // The Sequence number is used to determine if it's the
                                      // time to receive a CTS following Max_Num_Packet_CTS value
                                      // Num_Packet_CTS is loaded with Max_Num_Packet_CTS and decrement
                                      // at the reception of each packet. When it reach ZERO, CTS is send.
  unsigned char   *Data_Pgm_Ptr;      // Pointer to the Beginning of the Data that needs to be transmitted
  j1939_TxMsg_Struct *TxMsgPtr;    // Pointer to the Transmit Mailbox. Value == NULL when not used
}j1939_TxMultiPacket_State_Variables_Struct_Type;    // Bit Mapping is not important


/************* Fast Packet Session Structure Definition *************/
typedef struct j1939_RxFastPacket_State_Variables_Struct {
  unsigned char   InProgress    :1;   // Reception of FastPacket (0=Idle, 1=InProgress)
  unsigned char   SeqCounter;         // Sequence Counter used to distinguish different Fast Packet session
  unsigned char   Frame_Counter;      // Number of frame received
  unsigned char   DelayTimer;         // Delay timer use to determine if a Multi Packet session
                                      // needs to be abort.
  unsigned char   Num_Byte_Rx;        // Number of received bytes. Use to determine if the message
                                      // was received completely.
  unsigned char   *Byte_Pgm_Ptr;      // Pointer to the next Byte that need to be Program
  j1939_RxMsg_Struct *RxMsgPtr;       // Fixed pointer to the J1939 Rx Mailbox element
                                      // The pointer is initialise by J1939_Initialisation
}j1939_RxFastPacket_State_Variables_Struct_Type;    // Bit Mapping is not important

typedef struct j1939_TxFastPacket_State_Variables_Struct {
  unsigned char   InProgress    :1;   // Reception of FastPacket (0=Idle, 1=InProgress)
  unsigned char   SeqCounter;         // This counter is increment each the PGN is transmitted
  unsigned char   TxFrame_Counter;    // Number of the last frame transmitted
  unsigned char   NextTxTimer;        // Used to determine when the next Data Transfert is needed

  unsigned char   Num_Byte_Tx;        // Number of received bytes. Use to determine if the message
                                      // was received completely.

  unsigned char   *Byte_Tx_Ptr;      // Pointer to the next Byte that need to Sent
  j1939_TxMsg_Struct *TxMsgPtr;       // Fixed pointer to the J1939 Rx Mailbox element
                                      // The pointer is initialise by J1939_Initialisation
}j1939_TxFastPacket_State_Variables_Struct_Type;    // Bit Mapping is not important

/***********************************************************************************************/
/******************************* Global Variable Definitions **********************************/
/***********************************************************************************************/

#if J1939_COMMANDED_ADDRESS_ENABLED
  UINT1  Commanded;
#endif

// Status of the J1939 application 
j1939_Status_Struct j1939_Status;


/***********************************************************************************************/
/******************************* Private Variable Definitions **********************************/
/***********************************************************************************************/

// Pointer to the RX and TX J1939 Mailbox
static j1939_Parameter_Struct *j1939_Parameter;

static j1939_Address_Claim_State_Variables_Struct_Type    AddrClaim;

static void (*Dll_G7_AddressClaim)(void);
static UINT2  j1939_FreeRunTmr;              
static UINT2  j1939_DelayTmr;

static UINT1  MyAddress;       // Used Locally by the address claim process. When the address claim process
                               // is finished J1939_Status is updated if needed.
static UINT1  Answered;

#if SUPPORT_MULTIPACKET
// Use to keep track of the Multi Packet session states and variables
static j1939_RxMultiPacket_State_Variables_Struct_Type    RxMultiPacket[J1939_NUMBER_RX_MULTI_PACKET_SESSION];
static j1939_TxMultiPacket_State_Variables_Struct_Type    TxMultiPacket[J1939_NUMBER_TX_MULTI_PACKET_SESSION];
#endif //SUPPORT_MULTIPACKET

// Use to keep track of the Multi Packet session states and variables
#if SUPPORT_FAST_PACKET_RX
static j1939_RxFastPacket_State_Variables_Struct_Type    RxFastPacket[J1939_NUMBER_RX_FAST_PACKET_SESSION];
#endif //SUPPORT_FAST_PACKET_RX

#if SUPPORT_FAST_PACKET_TX
static j1939_TxFastPacket_State_Variables_Struct_Type    TxFastPacket[J1939_NUMBER_TX_FAST_PACKET_SESSION];
#endif //SUPPORT_FAST_PACKET_TX

static char Timer_10ms;   
static char Timer_100ms;   

/***********************************************************************************************/
/************************************ Private functions ***************************************/
/***********************************************************************************************/

static void  j1939_TxManagerTask(void);
static void  j1939_RxManagerTask(void);
static UINT4 j1939_ExtractPGN(UINT4 Id);

static UINT1 PGN_Tx_ISOAddressClaim(UINT1 DA, UINT1 SA);
static UINT1 PGN_Tx_ISOAcknowledgment(UINT1 DA, UINT1 SA, UINT4 PGN, UINT1 GroupFnct, UINT1 Status);
static UINT1 PGN_Tx_ISORequest(UINT1 DA, UINT1 SA, UINT4 PGN);

static void  PGN_Rx_ISOAddressClaim(can_Msg_Struct *MessagePtr);
//static void  PGN_Rx_ISOAcknowledgment(can_Msg_Struct *MessagePtr);
static void  PGN_Rx_ISORequest(can_Msg_Struct *MessagePtr);

static UINT1 Dll_TxSingleFrameMessage(j1939_SingleFrame_Message_Struct_Type *Message);
//static UINT1 Dll_RxSingleFrameMessage(j1939_SingleFrame_Message_Struct_Type *Message);
static UINT1 Dll_GetRandomTimeDelay(UINT1 *NameFld);

static UINT1  DLL_Fill_j1939RxMailbox(can_Msg_Struct *Msg_Can, unsigned long pgn, UINT1 SA, UINT1 DA );
static UINT1  Dll_SetTx_j1939TxMailbox(UINT4 PGN_Number);
static void   DLL_Update_TxCounter(void);
static void   DLL_Upd_AgeingCtr_j1939RxMailbox(void);

#if SUPPORT_MULTIPACKET
static j1939_RxMsg_Struct* DLL_Check_j1939RxMailbox_MultiPacket(unsigned long pgn, unsigned char SA);
#endif

// Fast Packet Transmission and reception
#if SUPPORT_FAST_PACKET_RX
static unsigned char Dll_RxFastPacketMsg(can_Msg_Struct *Msg_Can, j1939_RxMsg_Struct *Msg_j1939, UINT1 SA, UINT1 DA );
#endif //SUPPORT_FAST_PACKET_RX
#if SUPPORT_FAST_PACKET_TX
static unsigned char Dll_TxFastPacket_Init(j1939_TxMsg_Struct *Msg_j1939);
static void Dll_TxFastPacket(void);
#endif //SUPPORT_FAST_PACKET_TX
static void Dll_FastPacket_TimeOut_10mS(void);

#if SUPPORT_MULTIPACKET
// Multi Packet PGN Reception and Transission
static void  PGN_Rx_TP_CM_RTS(can_Msg_Struct *MessagePtr, UINT1 SA );
static void  PGN_Rx_TP_Conn_Abort(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr);
static UINT1 PGN_Tx_TP_Conn_Abort(UINT1 Second_Node_Addr, UINT1 Abort_Reason, unsigned long PGN);
static UINT1 PGN_Tx_TP_CM_CTS(j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR);
static UINT1 PGN_Tx_TP_CM_EndOfMsgACK(j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR);
static void  PGN_Rx_TP_CM_BAM(can_Msg_Struct *MessagePtr, UINT1 Originator_Address);
static void  PGN_Rx_DataTransfert(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr, UINT1 Dest_Addr);
static UINT1 PGN_Tx_TP_CM_RTS( j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR);
static UINT1 PGN_Tx_TP_CM_BAM( j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR);
static void  PGN_Rx_TP_CM_CTS(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr);
static UINT1 PGN_Tx_DataTransfert(j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR);
static void PGN_Rx_TP_CM_EndOfMsgACK(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr);

// Others Multi Packet usefull Function
static UINT1 Dll_TxMultiPacket_Init( j1939_TxMsg_Struct *Msg_j1939);
static void  Dll_MultiPacket_TimeOut(void);
static void  Dll_MultiPacket_Transmit(void);
static void  Dll_MultiPacket_UpdCtr(void);
#endif //SUPPORT_MULTIPACKET

// Address claim grafcet states and transitions
static void  Dll_S0_AddressClaim_Idle(void);
static void  Dll_S1_AddressClaim_RandomDelay(void);
static void  Dll_S2_AddressClaim_Transmit(void);
static void  Dll_S3_AddressClaim_RxCanFrame(void);
static void  Dll_S4_AddressClaim_FetchNextMyAddress(void);
static void  Dll_S5_AddressClaim_RxCanFrame(void);
static void  Dll_S6_AddressClaim_RandomDelay(void);
static void  Dll_S7_AddressClaim_Transmit(void);
static void  Dll_S8_AddressClaim_RxCanFrame(void);
static void  Dll_S9_AddressClaim_Transmit(void);

static void  Dll_T0_AddressClaim_Idle(void);
static void  Dll_T1_AddressClaim_RandomDelay(void);
static void  Dll_T2_AddressClaim_Transmit(void);
static void  Dll_T3_AddressClaim_RxCanFrame(void);
static void  Dll_T4_AddressClaim_FetchNextMyAddress(void);
static void  Dll_T5_AddressClaim_RxCanFrame(void);
static void  Dll_T6_AddressClaim_RandomDelay(void);
static void  Dll_T7_AddressClaim_Transmit(void);
static void  Dll_T8_AddressClaim_RxCanFrame(void);
static void  Dll_T9_AddressClaim_Transmit(void);


/***********************************************************************************************
* FUNCTION   : j1939_Initialisation
*
* DESCRIPTION: 
*
* INPUTS     : N/A
*
* OUTPUTS    : N/A
*           
* COMMENTS   :
***********************************************************************************************/
void j1939_Initialisation(j1939_Parameter_Struct *Parameter)
{

  unsigned char i,j;
  #if SUPPORT_MULTIPACKET
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;   
  #endif //SUPPORT_MULTIPACKET

  #if SUPPORT_FAST_PACKET_RX
  j1939_RxMsg_Struct *j1939_RxMsgPtr;
  #endif //SUPPORT_FAST_PACKET_RX
  #if SUPPORT_FAST_PACKET_TX
  j1939_TxMsg_Struct *j1939_TxMsgPtr;
  #endif //SUPPORT_FAST_PACKET_TX

  // Initialisation of the RX and TX J1939 Mailbox Pointer
  j1939_Parameter = Parameter;

  // Initialisation of the J1939 Statue
  j1939_Status.AddClaim = 0;
  j1939_Status.CAN_BusOK = 1;   // For now this Flag is not treated
  
  MyAddress = J1939_ADDRESS_CLAIM_NULL_ADDRESS;
  Answered  = J1939_ADDRESS_CLAIM_NULL_ADDRESS;

  #if J1939_COMMANDED_ADDRESS_ENABLED
  Commanded = J1939_ADDRESS_CLAIM_NULL_ADDRESS;
  #endif

  // Check and fix the memory if it has been corrupted
  
  if (*j1939_Parameter->PreferedSA < J1939_ADDRESS_CLAIM_MAX_ADDRESS)
  {
    // Default J1939 system node device address for this node
    MyAddress = *j1939_Parameter->PreferedSA;
  }

  // Initialisation of the J1939 Statue
  j1939_Status.AddClaim = 0;
  j1939_Status.CAN_BusOK = 1;   // For now this Flag is not treated
  j1939_Status.SA = MyAddress;  

  #if SUPPORT_MULTIPACKET
  // Initialise the Multi Packet Process Structure
  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];
    RxMultiPacket_PTR->InProgress = 0;
    RxMultiPacket_PTR->RxMsgPtr = NULL;
  }               
  #endif //SUPPORT_MULTIPACKET

  // Initialise the Fast Packet Process Structure 
  #if SUPPORT_FAST_PACKET_RX
  j1939_RxMsgPtr = j1939_Parameter->RxMailbox;
  j=0;
  for (i=0;i<j1939_Parameter->RxMailboxSize;i++)
  {
     if(j1939_RxMsgPtr->Pgn_Type == 1)
     {
        RxFastPacket[j].RxMsgPtr = j1939_RxMsgPtr;
        j++;
     }
     j1939_RxMsgPtr++;
  }
  #endif //SUPPORT_FAST_PACKET_RX
  #if SUPPORT_FAST_PACKET_TX
  j1939_TxMsgPtr = j1939_Parameter->TxMailbox;
  j=0;
  for (i=0;i<j1939_Parameter->TxMailboxSize;i++)
  {
     if(j1939_TxMsgPtr->Pgn_Type == 1)
     {
        TxFastPacket[j].TxMsgPtr = j1939_TxMsgPtr;
        j++;
     }
     j1939_TxMsgPtr++;
  }
  #endif //SUPPORT_FAST_PACKET_TX

  // Grafcets initialization
  if(MyAddress != J1939_ADDRESS_CLAIM_NULL_ADDRESS)
     Dll_G7_AddressClaim = Dll_S0_AddressClaim_Idle;
  else
     Dll_G7_AddressClaim   = Dll_S6_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* FUNCTION   :  j1939_ProcessRefresh
*
* DESCRIPTION: This function is called by the application and is used to refresh
*              the different J1939 process.
*
* INPUTS     : N/A
*
* OUTPUTS    : N/A
*           
* COMMENTS   : 
***********************************************************************************************/
void j1939_ProcessRefresh(void)
{
  Dll_G7_AddressClaim();        // Update the Address Claim Grafcet

  // Each 10 mS, update the transmit Trigger Counter
  if(Timer_10ms >= 10)
  {
    Timer_10ms -= 10;
    DLL_Update_TxCounter();     // Go check if J1939 Message needs to be send
    #if SUPPORT_MULTIPACKET
    Dll_MultiPacket_UpdCtr();   // Update the Time-Out and Transmit MultiPacket Counter
    #endif //SUPPORT_MULTIPACKET
    Dll_FastPacket_TimeOut_10mS();
  }

  // Each 100 mS, update the transmit Trigger Counter 
  if(Timer_100ms >= 100)   
  {
    DLL_Upd_AgeingCtr_j1939RxMailbox();
    Timer_100ms -= 100;
  }

  #if SUPPORT_MULTIPACKET
  Dll_MultiPacket_TimeOut();    // Check if a timeOut has occurs for the Multi Packet session
  Dll_MultiPacket_Transmit();   // Check if it's the time to transmit a Multi Packet
  #endif //SUPPORT_MULTIPACKET

  #if SUPPORT_FAST_PACKET_TX
  Dll_TxFastPacket();           // Check if a Fast message needs to be sent
  #endif //SUPPORT_FAST_PACKET_TX

  j1939_RxManagerTask();        // Read received messages
  j1939_TxManagerTask();        // Broadcast periodic messages
}


/***********************************************************************************************
* FUNCTION   :  j1939_TicksTimerTask
*
* DESCRIPTION: This function needs to be called each 1ms by an interrupt Timer. This time base
*              base is used by the address claim process. This function is also used to refresh
*              the transmit trigger counter each 10 mS
*
* INPUTS     : N/A
*
* OUTPUTS    : N/A
*           
* COMMENTS   : This base time should be increase to 1 mS
*   
***********************************************************************************************/
void j1939_TicksTimerTask(void)
{  
  Timer_10ms++;   // Increment the 10mS sub time base
  Timer_100ms++;  // Increment the 100mS sub Time Base

  // Interrupt base J1939 protocol timer free running counter increment
  j1939_FreeRunTmr++;
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_ISOAddressClaim
*
* DESCRIPTION: Request address claim at a specific destination address
*
* INPUTS     : DA - Destination address (0..255)
*              SA - Source address (0..251)
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : 
***********************************************************************************************/
static UINT1 PGN_Tx_ISOAddressClaim(UINT1 DA, UINT1 SA)
{
  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  j1939_NAME_Fields_Struct_Type   MyName;
  unsigned int Temp;

  // First, Built my own Name using the Name Fields define by the High Application Level Soft
  MyName.Byte8.ArbitraryAddressCapable = j1939_Parameter->Name_Fields.Self_Cfg_Addr & 0x01;
  MyName.Byte8.IndustryGroup           = j1939_Parameter->Name_Fields.Industry_Group & 0x07;
  MyName.Byte8.VehicleSystemInstance   = j1939_Parameter->Name_Fields.Vehicle_Sys_Inst & 0x0F;
  MyName.Byte7.VehicleSystem           = j1939_Parameter->Name_Fields.Vehicle_System & 0x7F;
  MyName.Byte7.Reserved                = j1939_Parameter->Name_Fields.Reserved;
  MyName.DeviceFunction                = j1939_Parameter->Name_Fields.Function;
  MyName.Byte5.DeviceFunctionInstance  = j1939_Parameter->Name_Fields.Function_Instance & 0x1F;
  MyName.Byte5.EcuInstance             = j1939_Parameter->Name_Fields.ECU_Instance & 0x07;
  Temp                                 = j1939_Parameter->Name_Fields.Manufacturer_Code << 5;
  MyName.ManufacturerCode_Byte4        = _HIBYTE(Temp);
  MyName.Byte3.ManufacturerCode_Byte3  = _LOBYTE(j1939_Parameter->Name_Fields.Manufacturer_Code) & 0x07;
  MyName.Byte3.IdentityNumber_Byte3    = _LOBYTE(_HIWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte2          = _HIBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte1          = _LOBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));  

  // Data to be transmited
  message.BytePtr  = (UINT1 *) (&MyName);
  message.ByteCntr = sizeof(j1939_NAME_Fields_Struct_Type);


  // J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_ADDRESS_CLAIM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_ADDRESS_CLAIM_PRIORITY;
  message.MsgId.IdField.BitField.Reserved = 0;
  message.MsgId.IdField.PduSpecific       = DA;
  message.MsgId.IdField.SourceAddress     = SA;

  // Valid PGN frame transmission state trap...
  if (Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }

  // Fail to transmit PGN frame...
  return (FALSE);
}


/***********************************************************************************************
* FUNCTION   : PGN_Rx_ISOAddressClaim
*
* DESCRIPTION: Respond to the received PGN message ISO address claim request from a specific
*              network device
*
* INPUTS     : MessagePtr - J1939 message structure pointer
*
* OUTPUTS    : N/A
*           
* COMMENTS   : Address claim procedures variables IWIN and ILOSE are set by this function
*              if we have source addresses contention for more then one network device
*              who claim this address.
***********************************************************************************************/
static void PGN_Rx_ISOAddressClaim (can_Msg_Struct *MessagePtr)
{
  j1939_Id_Struct_Type           *NmeaID  = (j1939_Id_Struct_Type *)(&MessagePtr->Id);
  j1939_NAME_Fields_Struct_Type   MyName;
  UINT1                          *MyNamePtr;
  UINT1                          *RxNamePtr;
  UINT1                           NameCnt;
  UINT2                           Temp;

  // First, Built my own Name using the Name Fields define by the High Application Level Soft
  MyName.Byte8.ArbitraryAddressCapable = j1939_Parameter->Name_Fields.Self_Cfg_Addr & 0x01;
  MyName.Byte8.IndustryGroup           = j1939_Parameter->Name_Fields.Industry_Group & 0x07;
  MyName.Byte8.VehicleSystemInstance   = j1939_Parameter->Name_Fields.Vehicle_Sys_Inst & 0x0F;
  MyName.Byte7.VehicleSystem           = j1939_Parameter->Name_Fields.Vehicle_System & 0x7F;
  MyName.Byte7.Reserved                = j1939_Parameter->Name_Fields.Reserved;
  MyName.DeviceFunction                = j1939_Parameter->Name_Fields.Function;
  MyName.Byte5.DeviceFunctionInstance  = j1939_Parameter->Name_Fields.Function_Instance & 0x1F;
  MyName.Byte5.EcuInstance             = j1939_Parameter->Name_Fields.ECU_Instance & 0x07;
  Temp                                 = j1939_Parameter->Name_Fields.Manufacturer_Code << 5;
  MyName.ManufacturerCode_Byte4        = _HIBYTE(Temp);
  MyName.Byte3.ManufacturerCode_Byte3  = _LOBYTE(j1939_Parameter->Name_Fields.Manufacturer_Code) & 0x07;
  MyName.Byte3.IdentityNumber_Byte3    = _LOBYTE(_HIWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte2          = _HIBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte1          = _LOBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));  
  
  // PGN address claim message reception flag reset
  AddrClaim.NoMessage = FALSE;

  // Devices address claim contention trap...
  if (NmeaID->IdField.SourceAddress == MyAddress)
  {
    // Set data byte pointers to MSB byte of NAME fields
    // (MSB of a structure have the same alignment in little and big endian)
    RxNamePtr  = (UINT1 *)(&MessagePtr->Data[0]);
    MyNamePtr  = (UINT1 *)(&MyName);
    RxNamePtr += sizeof(j1939_NAME_Fields_Struct_Type);
    MyNamePtr += sizeof(j1939_NAME_Fields_Struct_Type);
    NameCnt    = sizeof(j1939_NAME_Fields_Struct_Type);

    // J1939 address claim NAME field chalenge loop...
    while (NameCnt--)
    {
      // Next MSB byte...
      RxNamePtr--;
      MyNamePtr--;

      // NAME fileds MSB byte chalenge trap...
      if (*MyNamePtr != *RxNamePtr)
      {
        // The lowest value have the higest priority and WIN the chalenge
        if (*MyNamePtr < *RxNamePtr) AddrClaim.IWin  = TRUE;
        else                   AddrClaim.ILose = TRUE;
        // Address claim chalenge loop exit trap...
        break;
      }
    }
  }
}


/***********************************************************************************************
* FUNCTION   : PGN_Tx_ISOAcknowledgment
*
* DESCRIPTION: Transmit an acknowlegment to the device who sent a specific command or request.
*
* INPUTS     : DA        - Destination address (0..255)
*              SA        - Source address (0..251)
*              PGN       - Parameter Group Number requested
*              GroupFnct - Group function of PGN being acknowledged
*              Status    - Acknowledgment status to be transmit (0..3)
*
* OUTPUTS    : TRUE      - Single frame message sent to Tx queue
*              FALSE     - Tx queue full, no message sent
*           
* COMMENTS   : 
***********************************************************************************************/
static UINT1 PGN_Tx_ISOAcknowledgment(UINT1 DA, UINT1 SA, UINT4 PGN, UINT1 GroupFnct, UINT1 Status)
{
  // Local J1939 single frame message structures
  j1939_SingleFrame_Message_Struct_Type     message;
  j1939_Acknowledgment_Message_Struct_Type  ackData;
  j1939_Id_Struct_Type                      idToAck;

  // 24-bits PGN number to be acknowledged
  idToAck.Identifier  =  PGN<< 8;
  idToAck.Identifier &= 0x03FFFF00;

  // J1939 acknowlegment data bytes
  ackData.AckStatus   = Status;
  ackData.GroupFnct   = GroupFnct;
  ackData.Byte3       = 0xFF;
  ackData.Byte4       = 0xFF;
  ackData.Byte5       = 0xFF;
  ackData.PduSpecific = idToAck.IdField.PduSpecific;
  ackData.PduFormat   = idToAck.IdField.PduFormat;
  ackData.BitField.DataPage    = idToAck.IdField.BitField.DataPage;
  ackData.BitField.UnusedMSB   = 0;

  // Data to be transmited
  message.BytePtr  = (UINT1 *)(&ackData);
  message.ByteCntr = sizeof(ackData);

  // J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_ACKNOWLEDGMENT_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_ACKNOWLEDGMENT_PRIORITY;
  message.MsgId.IdField.BitField.Reserved = 0;
  message.MsgId.IdField.PduSpecific   = DA;
  message.MsgId.IdField.SourceAddress = SA;

  // Return PGN frame transmission state...
  return ( Dll_TxSingleFrameMessage(&message) );
}


/***********************************************************************************************
* FUNCTION   : PGN_Rx_ISOAcknowledgment
*
* DESCRIPTION: Receive an acknowlegment from the device we have requested PGN information.
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*
* OUTPUTS    : N/A
*
* COMMENTS   : If the PGN requested is not supported by the device, we will receive a NACK.
*              This is only true if the request was sent to a specific address.
***********************************************************************************************/
/*
void PGN_Rx_ISOAcknowledgment(can_Msg_Struct *MessagePtr)
{
  // To be defined...
}
*/

/***********************************************************************************************
* FUNCTION   : PGN_Tx_ISORequest
*
* DESCRIPTION: Request the transmission of a PGN from a network device or devices.
*
* INPUTS     : DA    - Destination address (0..255)
*              SA    - Source address (0..251)
*              PGN   - Parameter Group Number being requested
*
* OUTPUTS    : TRUE  - Single frame message sent to Tx queue
*              FALSE - Tx queue full, no message sent
*           
* COMMENTS   : 
***********************************************************************************************/
static UINT1 PGN_Tx_ISORequest(UINT1 DA, UINT1 SA, UINT4 PGN)
{
  // Local J1939 single frame message structures
  j1939_SingleFrame_Message_Struct_Type  message;
  j1939_Request_Message_Struct_Type      reqData;
  j1939_Id_Struct_Type                   reqPgn;

  // 24-bits PGN number being requested
  reqPgn.Identifier  = PGN << 8;
  reqPgn.Identifier &= 0x03FFFF00;

  // J1939 request data bytes
  reqData.PduSpecific        = reqPgn.IdField.PduSpecific;
  reqData.PduFormat          = reqPgn.IdField.PduFormat;
  reqData.BitField.DataPage  = reqPgn.IdField.BitField.DataPage;
  reqData.BitField.Reserved  = reqPgn.IdField.BitField.Reserved;
  reqData.BitField.UnusedMSB = 0;

  // Data to be transmited
  message.BytePtr  = (UINT1 *)(&reqData);
  message.ByteCntr = sizeof(reqData);

  // J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_REQUEST_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_REQUEST_PRIORITY;
  message.MsgId.IdField.BitField.Reserved = 0;
  message.MsgId.IdField.PduSpecific       = DA;
  message.MsgId.IdField.SourceAddress     = SA;
  
  if(PGN == J1939_ADDRESS_CLAIM_PGN) 
  {
     AddrClaim.NoMessage  = FALSE;
     AddrClaim.RequestPgn = TRUE;
     // PGN transmition is allready supported by "G7_AddressClaim" grafcet...
     //  ...do nothing here
  }
  else if (DA == 0xFF)
  {
     // If the PGN is defined, set the transmission. A module have to answer to his own request
     (void) Dll_SetTx_j1939TxMailbox(PGN);
  }

  // Return PGN frame transmission state...
  return ( Dll_TxSingleFrameMessage(&message) );
}


/***********************************************************************************************
* FUNCTION   : PGN_Rx_ISORequest
*
* DESCRIPTION: Request the transmission of a PGN from a network specific device or
*              global devices.
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*
* OUTPUTS    : N/A
*           
* COMMENTS   : 
***********************************************************************************************/
static void PGN_Rx_ISORequest(can_Msg_Struct *MessagePtr)
{
  // Local PGN pointers
  j1939_Request_Message_Struct_Type *PgnRequest = (j1939_Request_Message_Struct_Type *)(&MessagePtr->Data[0]);

  // Local J1939 message ID structure
  union {
    j1939_Id_Struct_Type  IdStruct;
    UINT4                 PgnNumber;
  } j1939;

  // Get J1939 specific network device address who sent the message to be answered
  j1939.IdStruct.Identifier = MessagePtr->Id;
  Answered = j1939.IdStruct.IdField.SourceAddress;

  // Extract requested 24-bits PGN number from J1939 single frame data field
  j1939.IdStruct.IdField.PduSpecific        = PgnRequest->PduSpecific;
  j1939.IdStruct.IdField.PduFormat          = PgnRequest->PduFormat;
  j1939.IdStruct.IdField.BitField.DataPage  = PgnRequest->BitField.DataPage;
  j1939.IdStruct.IdField.BitField.Reserved  = PgnRequest->BitField.Reserved;
  j1939.IdStruct.IdField.BitField.Priority  = 0;
  j1939.IdStruct.IdField.BitField.UnusedMSB = 0;
  j1939.IdStruct.Identifier        >>= 8;

  // All messages can be transmited if device is able to claim an address
  if (AddrClaim.ILose == FALSE)
  {
    // Software arbitration trap of supported PGN number to transmit...
    switch (j1939.PgnNumber)
    {
      case J1939_ADDRESS_CLAIM_PGN:
        // J1939 request for PGN address claim message
        AddrClaim.NoMessage  = FALSE;
        AddrClaim.RequestPgn = TRUE;
        // PGN transmition is allready supported by "G7_AddressClaim" grafcet...
        //  ...do nothing here
        break;

      case J1939_REQUEST_PGN:
      case J1939_ACKNOWLEDGMENT_PGN:
        PGN_Tx_ISOAcknowledgment(Answered, MyAddress, j1939.PgnNumber, J1939_GROUP_FUNCTION_VALUE, PGN_SUPPORTED_ACCESS_DENIED);
        break;


      default:
        // Check in the J1939 TX Mailbox if the requested PGN is supported
        // If yes, set the single transmission Flag. If the PGN is Periodic, set the 
        // transmission Flag anyways.
        // If the message is not supported, Send a Negative Acknowledgement
        if ( Dll_SetTx_j1939TxMailbox(j1939.PgnNumber) == FALSE)
            PGN_Tx_ISOAcknowledgment(Answered, MyAddress, j1939.PgnNumber, J1939_GROUP_FUNCTION_VALUE, NEGATIVE_ACKNOWLEDGMENT);
        break;
    }
  }

  // Only this message can be transmited if device cannot claim an address
  else if (j1939.PgnNumber == J1939_ADDRESS_CLAIM_PGN)  // PGN:59904
  {
    // J1939 request for PGN address claim message
    AddrClaim.NoMessage  = FALSE;
    AddrClaim.RequestPgn = TRUE;
    // PGN transmition is already supported by "G7_AddressClaim" grafcet - do nothing here!
  }
}

#if SUPPORT_MULTIPACKET
/***********************************************************************************************
* FUNCTION   : PGN_Rx_TP_CM_RTS
*
* DESCRIPTION: Reception of a "Request to send Message" sent by the Data Originator
*              
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*              SA - Source Address of the Originator
*
* OUTPUTS    : 
*           
* COMMENTS   : Connection Mode Request To Send (TP.CM_RTS): Destination Specific
*              Control Byte = 16
*
*              The function check if it's possible to start a Multi Packet receiving session
*              If Yes, the "MultiPacket" state variable is update and the session is started.
***********************************************************************************************/
static void PGN_Rx_TP_CM_RTS(can_Msg_Struct *MessagePtr, UINT1 SA )
{

  // Local Structure use to analyse the informations included in the Data Frame for TP.CM_RTS 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMRTS_Struct{
    unsigned char   Control_Byte;           
    unsigned char   Msg_Size_LSB;
    unsigned char   Msg_Size_MSB;
    unsigned char   Packet_Size;
    unsigned char   Max_Num_Packet_CTS;   // Maximum number of Packet that can be sent by the originator
                                          // before that a CTS was send by the receiver
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMRTS_Struct_Type;

  unsigned int Msg_Size;
  unsigned char i,Conn_Abort_Sent = 0;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;   
  j1939MailBox_ID_Struct_Type PGN_Requested;  

  j1939_TPCMRTS_Struct_Type *DataFieldPtr = (j1939_TPCMRTS_Struct_Type *)(&MessagePtr->Data[0]);

  // Extract requested 24-bits PGN number from J1939 Data Field
  PGN_Requested.IdField.Unused             = 0;
  PGN_Requested.IdField.PduSpecific        = DataFieldPtr->PduSpecific;
  PGN_Requested.IdField.PduFormat          = DataFieldPtr->PduFormat;
  PGN_Requested.IdField.BitField.DataPage  = DataFieldPtr->BitField.DataPage;
  PGN_Requested.IdField.BitField.Reserved  = DataFieldPtr->BitField.Reserved;  
  PGN_Requested.IdField.BitField.UnusedMSB = 0;


  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];

    if ( (RxMultiPacket_PTR->InProgress == 1) &&
         (SA == RxMultiPacket_PTR->Originator) &&
         (PGN_Requested.Identifier!=RxMultiPacket_PTR->PGN) )
    {
      // It's not possible to open a new Rx Session with this Node
      Conn_Abort_Sent = 1;
      (void) PGN_Tx_TP_Conn_Abort(SA, 1, PGN_Requested.Identifier);
      break;        
    }
    else if ( (RxMultiPacket_PTR->InProgress == 1) &&
              (SA == RxMultiPacket_PTR->Originator) &&
              (PGN_Requested.Identifier==RxMultiPacket_PTR->PGN) )
    {
      // A Rx session for this PGN is already open with this Node, Restart it !!!!
      RxMultiPacket_PTR->InProgress = 0;
      RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 0;
      break;        
    }
    else
      RxMultiPacket_PTR = NULL;
  }

  // If needed, Try to find a free Receive Multipacket Session
  if (RxMultiPacket_PTR == NULL)  
  {
      for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
      {
        RxMultiPacket_PTR = &RxMultiPacket[i]; 
        if(RxMultiPacket_PTR->InProgress == 0)
          break;
        else
          RxMultiPacket_PTR = NULL;
      }
      // If no Free session was find, send a Comm Abort
      if (RxMultiPacket_PTR == NULL)
      {
        (void) PGN_Tx_TP_Conn_Abort(SA, 1, PGN_Requested.Identifier);
        Conn_Abort_Sent = 1;
      }
  }

  if ((Conn_Abort_Sent == 0) && (RxMultiPacket_PTR != NULL))
  {
    // Check if the PGN can be received by the J1939 Receive MailBox

     _LOBYTE(Msg_Size) = DataFieldPtr->Msg_Size_LSB;  // Determine the size of the message
     _HIBYTE(Msg_Size) = DataFieldPtr->Msg_Size_MSB;

     // Try to find a VALID Free J1939 Rx Mailbox for the new Message.
#ifndef MM32F0133C7P		//? Add
     //RxMultiPacket_PTR->RxMsgPtr = DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,SA);
		//New XC8 compiler reported error, so changed to:
     j1939_RxMsg_Struct* RxMsgPtr = DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,SA);
		 RxMultiPacket_PTR->RxMsgPtr = RxMsgPtr;
#else
		 RxMultiPacket_PTR->RxMsgPtr = DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,SA);
#endif

     
     if (RxMultiPacket_PTR->RxMsgPtr != NULL)
     {
        // Check if the Truncation is allowed 
        if(RxMultiPacket_PTR->RxMsgPtr->Trunk == 1)
           RxMultiPacket_PTR->Trunk_Size = RxMultiPacket_PTR->RxMsgPtr->BufferSize;
        // The Truncation is not allowed, we have to verify that the Mailbox is Big
        // enough to received the message.
        else if ( RxMultiPacket_PTR->RxMsgPtr->BufferSize < Msg_Size)
           RxMultiPacket_PTR->RxMsgPtr = NULL; 
        else
           RxMultiPacket_PTR->Trunk_Size = Msg_Size;             
     }

     if (RxMultiPacket_PTR->RxMsgPtr != NULL)
     {
        RxMultiPacket_PTR->DA = MyAddress;                 // The multi Packet session is destination Specific 
        RxMultiPacket_PTR->Originator = SA;                // Keep track of the Originator Address
        RxMultiPacket_PTR->PGN = PGN_Requested.Identifier; // Keep track of the PGN that is transmitted
        RxMultiPacket_PTR->Msg_Size = Msg_Size;            // Keep Track of the message size (Total number of bytes)
        RxMultiPacket_PTR->Packet_Size = DataFieldPtr->Packet_Size;  // Keep Track of the number of Packet in the message

        // Define how many packet can be receive before than the receiver send CTS.
        // EX: If the Originator have a lowest capability value => use it 
        if(DataFieldPtr->Max_Num_Packet_CTS < J1939_MAX_NUMBER_PACKET_BEFOFE_CTS)
          RxMultiPacket_PTR->Max_Num_Packet_CTS = DataFieldPtr->Max_Num_Packet_CTS;
        else 
          RxMultiPacket_PTR->Max_Num_Packet_CTS = J1939_MAX_NUMBER_PACKET_BEFOFE_CTS;

        RxMultiPacket_PTR->Num_Packet_CTS = RxMultiPacket_PTR->Max_Num_Packet_CTS;
        RxMultiPacket_PTR->Sequence_Number = 1;
        RxMultiPacket_PTR->Num_Byte_Rx = 0;    // Reset the number of bytes received
        
        RxMultiPacket_PTR->RxMsgPtr->MsgSize = Msg_Size;     // Set the size of the message in the J1939
        RxMultiPacket_PTR->Byte_Pgm_Ptr = RxMultiPacket_PTR->RxMsgPtr->Data;  // The Byte Programming Pointer should
                                                                              // need to pointed on the Good J1939 Mailbox
                                                                              // Element

        if( PGN_Tx_TP_CM_CTS(RxMultiPacket_PTR)==FALSE )  // If it's not possible to send the CTS
          RxMultiPacket_PTR->RxMsgPtr = NULL;             // Don't start the Multi Packet session
        else
        {    
          // The PGN is supported by the J1939 Mailbox, and the CTS was sent
          // finish to initialised Register, prepare the first reception
          RxMultiPacket_PTR->InProgress = 1;                // Multi Packet session is now in Progress
          RxMultiPacket_PTR->RxMsgPtr->Full = J1939_EMPTY;  // Clear The Flag Full in the J1939 MailBox 
          RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 1;
          RxMultiPacket_PTR->RxMsgPtr->Org_Addr = SA;
          RxMultiPacket_PTR->RxMsgPtr->Dst_Addr = MyAddress; 
          
          // Start a timer of 1.25 S. If the timer elapsed before than a first packet is received
          // The J1939 driver have to send a Connection Abort Message.
          RxMultiPacket_PTR->DelayTimer = 125;  
        }
     }

     else   // The PGN is not supported 
     {
        // The PGN is not supported, Send a communication abort with Abort Reason = 255 
        (void) PGN_Tx_TP_Conn_Abort(SA, 255, PGN_Requested.Identifier);       
     }
  }
}

/***********************************************************************************************
* FUNCTION   : PGN_Rx_TP_Conn_Abort
*
* DESCRIPTION: Reception of Connection Abort Message sent by the Originator
*              If the Originator wants to Abort the message that is currently
*              processed by the Multi Packet session, the session is stop.
*              
* INPUTS     : MessagePtr - Single frame message structure pointer
*              Originator_Addr - Address of the Data Originator
*
* INPUTS     : 
*
* OUTPUTS    : 
*           
* COMMENTS   : Connection Abort (TP.Conn_Abort): Destination Specific
*              Control Byte = 255
***********************************************************************************************/
static void PGN_Rx_TP_Conn_Abort(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr)
{

  // Local Structure use to receive the informations TP.Conn_Abort 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAbort_Struct{
    unsigned char   Control_Byte;           // Control Byte = 255 for Conn Abort     
    unsigned char   Reason;                 // Connection Abort Reason
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   Not_Used2;              // Should be filed with FF
    unsigned char   Not_Used3;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMAbort_Struct_Type;

  unsigned char i;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;        
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;        

  j1939_TPCMAbort_Struct_Type *DATA = (j1939_TPCMAbort_Struct_Type *)(&MessagePtr->Data[0]);

  j1939MailBox_ID_Struct_Type PGN_Requested;  

  // Extract requested 24-bits PGN number from J1939 Data Field
  PGN_Requested.IdField.Unused             = 0;
  PGN_Requested.IdField.PduSpecific        = DATA->PduSpecific;
  PGN_Requested.IdField.PduFormat          = DATA->PduFormat;
  PGN_Requested.IdField.BitField.DataPage  = DATA->BitField.DataPage;
  PGN_Requested.IdField.BitField.Reserved  = DATA->BitField.Reserved;  
  PGN_Requested.IdField.BitField.UnusedMSB = 0;

  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];

    if ( (RxMultiPacket_PTR->InProgress == 1) && 
         (RxMultiPacket_PTR->Originator == Originator_Addr) &&
         (RxMultiPacket_PTR->PGN == PGN_Requested.Identifier) )
    {
      RxMultiPacket_PTR->InProgress = 0;   // Stop the Multi Packet session
      RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 0;
    }
  }

  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &TxMultiPacket[i];

    if ( (TxMultiPacket_PTR->InProgress == 1) && 
         (TxMultiPacket_PTR->DA == Originator_Addr) &&
         (TxMultiPacket_PTR->PGN == PGN_Requested.Identifier) &&
         (TxMultiPacket_PTR->DA != 0xFF) )
    {
      TxMultiPacket_PTR->InProgress = 0;
      TxMultiPacket_PTR->TxMsgPtr->TxInProgress = 0;
    }
  }
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_Conn_Abort
*
* DESCRIPTION: Transmission of Connection Abort Message message
*              
* INPUTS     : Second_Node_Addr - Address the second node of the MultiPacket session
*              Abort Reason - Reason Why Multi Packet session need to be abort (See J1939-21)
*              PGN - Parameter Group Number of the Multi Packet session
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent           
*           
* COMMENTS   : Connection Abort (TP.Conn_Abort): Destination Specific
*
***********************************************************************************************/
static UINT1 PGN_Tx_TP_Conn_Abort(UINT1 Second_Node_Addr, UINT1 Abort_Reason, unsigned long PGN)
{

  // Local Structure use to send the informations TP.Conn_Abort 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAbort_Struct{
    unsigned char   Control_Byte;           // Control Byte = 255 for Conn Abort     
    unsigned char   Reason;                 // Connection Abort Reason
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   Not_Used2;              // Should be filed with FF
    unsigned char   Not_Used3;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMAbort_Struct_Type;

  j1939_TPCMAbort_Struct_Type DATA;
  
  // Pointer to the Pgn Number included in the MultiPacket State Struct
  j1939MailBox_ID_Struct_Type *PGN_Number = (j1939MailBox_ID_Struct_Type*)(&PGN);   

  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  // Start by Building the DATA Frame
  DATA.Control_Byte = 255;
  DATA.Reason = Abort_Reason;
  DATA.Not_Used1 = 0xFF;
  DATA.Not_Used2 = 0xFF;
  DATA.Not_Used3 = 0xFF;
  DATA.PduSpecific = PGN_Number->IdField.PduSpecific;
  DATA.PduFormat = PGN_Number->IdField.PduFormat;
  DATA.BitField.DataPage = PGN_Number->IdField.BitField.DataPage;
  DATA.BitField.Reserved = PGN_Number->IdField.BitField.Reserved;
  DATA.BitField.UnusedMSB = PGN_Number->IdField.BitField.UnusedMSB;

  message.BytePtr  = (UINT1 *) (&DATA.Control_Byte);
  message.ByteCntr = sizeof(j1939_TPCMAbort_Struct_Type);

  // Second, Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPCM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = Second_Node_Addr;          
  message.MsgId.IdField.SourceAddress     = MyAddress;

  // Valid PGN frame transmission state trap...
  if ( Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }
  // Fail to transmit PGN frame...
  return (FALSE);
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_CM_CTS
*
* DESCRIPTION: Transmission of CTS message in response to an accepted RTS
*              
* INPUTS     : Pointer to the current MultiPacket State structure to build the message
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : Connection Abort (TP.CM_CTS): Destination Specific
*
***********************************************************************************************/
static UINT1 PGN_Tx_TP_CM_CTS(j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR)
{

  // Local Structure use to transmit the informations TP.CM_CTS 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMCTS_Struct{
    unsigned char   Control_Byte;           // Control Byte = 17 for CTS     
    unsigned char   Max_Num_Packet_CTS;     // Max number of packet that can be sent
    unsigned char   Next_Packet_Number;     // Next packet number to be sent
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   Not_Used2;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMCTS_Struct_Type;

  j1939_TPCMCTS_Struct_Type DATA;
  
  // Pointer to the Pgn Number included in the MultiPacket State Struct
  j1939MailBox_ID_Struct_Type *PGN_Number = (j1939MailBox_ID_Struct_Type*)(&(RxMultiPacket_PTR->PGN));   

  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  // Start by Building the DATA Frame
  DATA.Control_Byte = 17;
  DATA.Max_Num_Packet_CTS = RxMultiPacket_PTR->Max_Num_Packet_CTS;
  DATA.Next_Packet_Number = RxMultiPacket_PTR->Sequence_Number;
  DATA.Not_Used1 = 0xFF;
  DATA.Not_Used2 = 0xFF;
  DATA.PduSpecific = PGN_Number->IdField.PduSpecific;
  DATA.PduFormat = PGN_Number->IdField.PduFormat;
  DATA.BitField.DataPage = PGN_Number->IdField.BitField.DataPage;
  DATA.BitField.Reserved = PGN_Number->IdField.BitField.Reserved;
  DATA.BitField.UnusedMSB = PGN_Number->IdField.BitField.UnusedMSB;

  message.BytePtr  = (UINT1 *) (&DATA.Control_Byte);
  message.ByteCntr = sizeof(j1939_TPCMCTS_Struct_Type);

  // Second, Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPCM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = RxMultiPacket_PTR->Originator;
  message.MsgId.IdField.SourceAddress     = RxMultiPacket_PTR->DA;

  // Valid PGN frame transmission state trap...
  if (Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }
  // Fail to transmit PGN frame...
  return (FALSE);
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_CM_EndOfMsgACK
*
* DESCRIPTION: Transmission of CTS message in response to a good RTS
*              
* INPUTS     : Pointer to the current MultiPacket State structure to build the message
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : End Of Message Acknowledgement (TP.CM_EndOfMsgAck) : Destination Specific
*              Control Byte = 19
*
***********************************************************************************************/
static UINT1 PGN_Tx_TP_CM_EndOfMsgACK(j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR)
{
  // Local Structure use to send the informations TP.CM.EndOfMsgAck 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAck_Struct{
    unsigned char   Control_Byte;           // Control Byte = 19 for End of Msg Ack
    unsigned char   Msg_Size_LSB;
    unsigned char   Msg_Size_MSB;
    unsigned char   Packet_Size;
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMAck_Struct_Type;

  j1939_TPCMAck_Struct_Type DATA;
  
  // Pointer to the Pgn Number included in the MultiPacket State Struct
  j1939MailBox_ID_Struct_Type *PGN_Number = (j1939MailBox_ID_Struct_Type*)(&(RxMultiPacket_PTR->PGN));   

  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  // Start by Building the DATA Frame
  DATA.Control_Byte = 19;
  DATA.Msg_Size_LSB = _LOBYTE(RxMultiPacket_PTR->Msg_Size);
  DATA.Msg_Size_MSB = _HIBYTE(RxMultiPacket_PTR->Msg_Size);
  DATA.Packet_Size = RxMultiPacket_PTR->Packet_Size;
  DATA.Not_Used1 = 0xFF;
  DATA.PduSpecific = PGN_Number->IdField.PduSpecific;
  DATA.PduFormat = PGN_Number->IdField.PduFormat;
  DATA.BitField.DataPage = PGN_Number->IdField.BitField.DataPage;
  DATA.BitField.Reserved = PGN_Number->IdField.BitField.Reserved;
  DATA.BitField.UnusedMSB = PGN_Number->IdField.BitField.UnusedMSB;

  message.BytePtr  = (UINT1 *) (&DATA.Control_Byte);
  message.ByteCntr = sizeof(j1939_TPCMAck_Struct_Type);

  // Second, Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPCM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = RxMultiPacket_PTR->Originator;
  message.MsgId.IdField.SourceAddress     = RxMultiPacket_PTR->DA;

  // Valid PGN frame transmission state trap...
  if (Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }
  // Fail to transmit PGN frame...
  return (FALSE);
}


/***********************************************************************************************
* FUNCTION   : PGN_Rx_TP_CM_BAM
*
* DESCRIPTION: Reception of Broadcast Announce Message sent by the Originator
*              
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*              Originator_Address - Source Address of the Originator
*
* OUTPUTS    : 
*           
* COMMENTS   : Broadcast Announcement Message (TP.CM_BAM): Global Destination
*                 Control Byte = 32 
***********************************************************************************************/
static void PGN_Rx_TP_CM_BAM(can_Msg_Struct *MessagePtr, UINT1 Originator_Address)
{

  // Local Structure use to analyse the informations included in the Data Frame for TP.CM_BAM 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMBAM_Struct{
    unsigned char   Control_Byte;           
    unsigned char   Msg_Size_LSB;
    unsigned char   Msg_Size_MSB;
    unsigned char   Packet_Size;
    unsigned char   Not_Used1;     // Should be loaded with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMBAM_Struct_Type;

  unsigned int Msg_Size;
  unsigned char i,Session_Abort = 0;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;        
  j1939MailBox_ID_Struct_Type PGN_Requested;  

  j1939_TPCMBAM_Struct_Type *DataFieldPtr = (j1939_TPCMBAM_Struct_Type *)(&MessagePtr->Data[0]);

  // Extract requested 24-bits PGN number from J1939 Data Field
  PGN_Requested.IdField.Unused             = 0;
  PGN_Requested.IdField.PduSpecific        = DataFieldPtr->PduSpecific;
  PGN_Requested.IdField.PduFormat          = DataFieldPtr->PduFormat;
  PGN_Requested.IdField.BitField.DataPage  = DataFieldPtr->BitField.DataPage;
  PGN_Requested.IdField.BitField.Reserved  = DataFieldPtr->BitField.Reserved;  
  PGN_Requested.IdField.BitField.UnusedMSB = 0;


  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];

    if ( (RxMultiPacket_PTR->InProgress == 1) &&
         (Originator_Address == RxMultiPacket_PTR->Originator) )
    {
      // The Data Originator wants to start a Broadcast Multipacket session
      // But one session is already Open. Stop the current session.
      RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 0;
      RxMultiPacket_PTR->InProgress = 0;
      break;  
    }                      
    else
      RxMultiPacket_PTR = NULL;
  }

  // If needed, Try to find a free Receive Multipacket Session
  if (RxMultiPacket_PTR == NULL)  
  {
      for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
      {
        RxMultiPacket_PTR = &RxMultiPacket[i]; 
        if(RxMultiPacket_PTR->InProgress == 0)
          break;
        else
          RxMultiPacket_PTR = NULL;
      }
  }

  // If the session doesn't need to be abort 
  if ( (RxMultiPacket_PTR != NULL) && (Session_Abort == 0) )   
  {
    // Check if the PGN can be received by the J1939 Receive MailBox
     _LOBYTE(Msg_Size) = DataFieldPtr->Msg_Size_LSB;  // Determine the size of the message
     _HIBYTE(Msg_Size) = DataFieldPtr->Msg_Size_MSB;

     // Try to find a VALID Free J1939 Rx Mailbox for the new Message.
#ifndef MM32F0133C7P		//? Add
     //RxMultiPacket_PTR->RxMsgPtr = DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,Originator_Address);
	 //New XC8 compiler reported error, so changed to:
     j1939_RxMsg_Struct*  RxMsgPtr =  DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,Originator_Address);
     RxMultiPacket_PTR->RxMsgPtr = RxMsgPtr;
#else
     RxMultiPacket_PTR->RxMsgPtr = DLL_Check_j1939RxMailbox_MultiPacket(PGN_Requested.Identifier,Originator_Address);
#endif

     if (RxMultiPacket_PTR->RxMsgPtr != NULL)
     {
        // Check if the Truncation is allowed 
        if(RxMultiPacket_PTR->RxMsgPtr->Trunk == 1)
           RxMultiPacket_PTR->Trunk_Size = RxMultiPacket_PTR->RxMsgPtr->BufferSize;
        // The Truncation is not allowed, we have to verify that the Mailbox is Big
        // enough to received the message.
        else if ( RxMultiPacket_PTR->RxMsgPtr->BufferSize < Msg_Size)
           RxMultiPacket_PTR->RxMsgPtr = NULL; 
        else
           RxMultiPacket_PTR->Trunk_Size = Msg_Size;             
     }

     if (RxMultiPacket_PTR->RxMsgPtr != NULL)
     {
        // The PGN is supported by the J1939 Mailbox
         
        // initialised Register, prepare the first reception
        // and send the CTS

        RxMultiPacket_PTR->InProgress = 1;                  // Multi Packet session is now in Progress
        RxMultiPacket_PTR->DA = 255;                        // The multi Packet session is Global 
        RxMultiPacket_PTR->Originator = Originator_Address; // Keep track of the Originator Address
        RxMultiPacket_PTR->PGN = PGN_Requested.Identifier;  // Keep track of the PGN that is transmitted
        RxMultiPacket_PTR->Msg_Size = Msg_Size;             // Keep Track of the message size (Total number of bytes)
        RxMultiPacket_PTR->Packet_Size = DataFieldPtr->Packet_Size;  // Keep Track of the number of Packet in the message

        RxMultiPacket_PTR->Sequence_Number = 1;
        RxMultiPacket_PTR->Num_Byte_Rx = 0;    // Reset the number of bytes received
        
        RxMultiPacket_PTR->RxMsgPtr->Full = J1939_EMPTY;  // Clear The Flag Full in the J1939 MailBox 
        RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 1;
        RxMultiPacket_PTR->RxMsgPtr->Org_Addr = Originator_Address;
        RxMultiPacket_PTR->RxMsgPtr->Dst_Addr = 0xFF; 

        RxMultiPacket_PTR->RxMsgPtr->MsgSize = Msg_Size;                      // Set the size of the message in the J1939
        RxMultiPacket_PTR->Byte_Pgm_Ptr = RxMultiPacket_PTR->RxMsgPtr->Data;  // The Byte Programming Pointer should
                                                                              // need to pointed on the Good J1939 Mailbox
                                                                              // Element

        // Start a timer of 750 mS. If the timer elapsed before 
        // than a first packet is received, the session is abort.
        RxMultiPacket_PTR->DelayTimer = 75;  
     }                                                        
  }
}

/***********************************************************************************************
* FUNCTION   : PGN_Rx_DataTransfert
*
* DESCRIPTION: Reception of a Multi Packet DATA
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*              Originator_Addr - Address of the Data Originator
*              Dest_Addr - Destination address of the Data, this parameter is needed
*                          in order to process Destination Specific and Global communication
*
* OUTPUTS    : 
*           
* COMMENTS   : Transport Protocol - Data Transfert (TP.DT)
***********************************************************************************************/
static void PGN_Rx_DataTransfert(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr, UINT1 Dest_Addr)
{
  // Local Structure use to receive the informations TP.DT 
  // See J1939-21 for reference
  typedef struct j1939_TPDT_Struct{
    unsigned char   SequenceNumber;               
    unsigned char   DATA[7];
  }j1939_TPDT_Struct_Type;

  unsigned char i,j;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;   

  j1939_TPDT_Struct_Type *DataPtr = (j1939_TPDT_Struct_Type *)(&MessagePtr->Data[0]);

  // Find the corresponding Multi-Packet session
  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  {
    RxMultiPacket_PTR = &RxMultiPacket[i];
    // Check if a Multi Packet session is in Progress AND if this Data are related to the current session
    if ( (RxMultiPacket_PTR->InProgress == 1) &&
         (RxMultiPacket_PTR->Originator == Originator_Addr) &&
         (RxMultiPacket_PTR->DA == Dest_Addr) )
      break;
    else
      RxMultiPacket_PTR = NULL;
   }

  if ( RxMultiPacket_PTR != NULL )
  {
    // Check if this Packet correspond to the one that is waited by the MP session
    if(RxMultiPacket_PTR->Sequence_Number == DataPtr->SequenceNumber)
    {
      // Check if it's the last Packet of Data
      if(RxMultiPacket_PTR->Sequence_Number == RxMultiPacket_PTR->Packet_Size)
      {
        j = 0;
        for (i=RxMultiPacket_PTR->Num_Byte_Rx;i<RxMultiPacket_PTR->Msg_Size;i++)  // Copy the remaining byte in the Memory
        {
          if(RxMultiPacket_PTR->Num_Byte_Rx < RxMultiPacket_PTR->Trunk_Size)
            *(RxMultiPacket_PTR->Byte_Pgm_Ptr) = DataPtr->DATA[j];
          RxMultiPacket_PTR->Byte_Pgm_Ptr++;
          RxMultiPacket_PTR->Num_Byte_Rx++;
          j++; 
        }
        if(RxMultiPacket_PTR->DA != 255)                    // If the session is destination specific
          (void) PGN_Tx_TP_CM_EndOfMsgACK(RxMultiPacket_PTR);      // Send the End of Message Acknowledgement

          RxMultiPacket_PTR->RxMsgPtr->Full = J1939_FULL;   // The J1939 Mailbox is now FULL
          RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 0;
          RxMultiPacket_PTR->RxMsgPtr->Ageing_Ctr = 0;      // Refresh the Ageing Counter
          RxMultiPacket_PTR->InProgress = 0; 
      }
      // This Packet is not the last One
      else
      {
        for (i=0;i<7;i++)   // Copy the 7 Byte of Data in the Memory
        {
          if(RxMultiPacket_PTR->Num_Byte_Rx < RxMultiPacket_PTR->Trunk_Size)
            *(RxMultiPacket_PTR->Byte_Pgm_Ptr) = DataPtr->DATA[i];
          RxMultiPacket_PTR->Byte_Pgm_Ptr++;   // Increment the Data Pointer
          RxMultiPacket_PTR->Num_Byte_Rx++;    // Increment the number of byte received
        }
        RxMultiPacket_PTR->Sequence_Number++;  // Increment the sequence number to be able to receive next packet

        RxMultiPacket_PTR->DelayTimer = 75;    // Time Out before new reception = 750 mS
        if (RxMultiPacket_PTR->DA != 255)      // If the message is not Global, check if it's the time to transmit CTS
        {
          if (--RxMultiPacket_PTR->Num_Packet_CTS == 0)
          {
            // It's the time to transmit a CTS
            RxMultiPacket_PTR->Num_Packet_CTS = RxMultiPacket_PTR->Max_Num_Packet_CTS;
            (void) PGN_Tx_TP_CM_CTS(RxMultiPacket_PTR);
            RxMultiPacket_PTR->DelayTimer = 125;   // Time Out before new reception = 1250 mS
          }
        }          
      }
    }
  }  
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_CM_RTS
*
* DESCRIPTION: Transmission of a RTS to start a Tx MultiPacket session
*
* INPUTS     : TxMultiPacket_PTR - Pointer to the 
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : 
***********************************************************************************************/  
static UINT1 PGN_Tx_TP_CM_RTS( j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR)
{

  // Local Structure use to send the informations TP.CM_RTS 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAbort_Struct{
    unsigned char   Control_Byte;          // Control Byte = 16 for RTS  
    unsigned char   Msg_Size_LSB;          // Message Size (2 Bytes)
    unsigned char   Msg_Size_MSB;
    unsigned char   Num_Packet;            // Total Number of packet
    unsigned char   Max_Num_Packet_CTS;    // Max number of packet that can be sent     
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMRTS_Struct_Type;

  j1939_TPCMRTS_Struct_Type DATA;

  // Pointer to the Pgn Number included in the MultiPacket State Struct
  j1939MailBox_ID_Struct_Type *PGN_Number = (j1939MailBox_ID_Struct_Type*)(&(TxMultiPacket_PTR->PGN));   

  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  // Start by Building the DATA Frame
  DATA.Control_Byte = 16;
  DATA.Msg_Size_LSB = _LOBYTE(TxMultiPacket_PTR->Msg_Size);
  DATA.Msg_Size_MSB = _HIBYTE(TxMultiPacket_PTR->Msg_Size);
  DATA.Num_Packet   = TxMultiPacket_PTR->Packet_Size;
  DATA.Max_Num_Packet_CTS = TxMultiPacket_PTR->Max_Num_Packet_CTS;

  DATA.PduSpecific = PGN_Number->IdField.PduSpecific;
  DATA.PduFormat = PGN_Number->IdField.PduFormat;
  DATA.BitField.DataPage = PGN_Number->IdField.BitField.DataPage;
  DATA.BitField.Reserved = PGN_Number->IdField.BitField.Reserved;
  DATA.BitField.UnusedMSB = PGN_Number->IdField.BitField.UnusedMSB;

  message.BytePtr  = (UINT1 *) (&DATA.Control_Byte);
  message.ByteCntr = sizeof(j1939_TPCMRTS_Struct_Type);

  // Second, Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPCM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = TxMultiPacket_PTR->DA;
  message.MsgId.IdField.SourceAddress     = TxMultiPacket_PTR->Originator;

  // Valid PGN frame transmission state trap...
  if (Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }
  // Fail to transmit PGN frame...
  return (FALSE);
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_CM_BAM
*
* DESCRIPTION: Transmission of a BAM to start a Global Tx MultiPacket session
*
* INPUTS     : TxMultiPacket_PTR - Pointer to the 
*
* OUTPUTS    : 
*           
* COMMENTS   : 
***********************************************************************************************/  
static UINT1 PGN_Tx_TP_CM_BAM( j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR)
{

  // Local Structure use to send the informations TP.CM_BAM 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAbort_Struct{
    unsigned char   Control_Byte;          // Control Byte = 32 for BAM  
    unsigned char   Msg_Size_LSB;          // Message Size (2 Bytes)
    unsigned char   Msg_Size_MSB;
    unsigned char   Num_Packet;            // Total Number of packet
    unsigned char   Unused;                // Unused     
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMBAM_Struct_Type;

  j1939_TPCMBAM_Struct_Type DATA;

  // Pointer to the Pgn Number included in the MultiPacket State Struct
  j1939MailBox_ID_Struct_Type *PGN_Number = (j1939MailBox_ID_Struct_Type*)(&(TxMultiPacket_PTR->PGN));   

  // Local J1939 single frame message structure
  j1939_SingleFrame_Message_Struct_Type  message;

  // Start by Building the DATA Frame
  DATA.Control_Byte = 32;
  DATA.Msg_Size_LSB = _LOBYTE(TxMultiPacket_PTR->Msg_Size);
  DATA.Msg_Size_MSB = _HIBYTE(TxMultiPacket_PTR->Msg_Size);
  DATA.Num_Packet   = TxMultiPacket_PTR->Packet_Size;
  DATA.Unused       = 0xFF;

  DATA.PduSpecific = PGN_Number->IdField.PduSpecific;
  DATA.PduFormat = PGN_Number->IdField.PduFormat;
  DATA.BitField.DataPage = PGN_Number->IdField.BitField.DataPage;
  DATA.BitField.Reserved = PGN_Number->IdField.BitField.Reserved;
  DATA.BitField.UnusedMSB = PGN_Number->IdField.BitField.UnusedMSB;

  message.BytePtr  = (UINT1 *) (&DATA.Control_Byte);
  message.ByteCntr = sizeof(j1939_TPCMBAM_Struct_Type);

  // Second, Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPCM_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = TxMultiPacket_PTR->DA;    // Should be 0xFF
  message.MsgId.IdField.SourceAddress     = TxMultiPacket_PTR->Originator;

  // Valid PGN frame transmission state trap...
  if (Dll_TxSingleFrameMessage(&message) == TRUE)
  {
    // Success PGN frame transmission exit...
    return (TRUE);
  }
  // Fail to transmit PGN frame...
  return (FALSE);
}

/***********************************************************************************************
* FUNCTION   : PGN_Rx_TP_CM_CTS
*
* DESCRIPTION: Reception of a CTS probably in response to a RTS
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*              Originator_Addr - Address of the CTS Originator
*
* OUTPUTS    : 
*           
* COMMENTS   : 
***********************************************************************************************/  
static void PGN_Rx_TP_CM_CTS(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr)
{

  // Local Structure use to receive the informations TP.CM_CTS 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMCTS_Struct{
    unsigned char   Control_Byte;           // Control Byte = 17 for CTS     
    unsigned char   Max_Num_Packet_CTS;     // Max number of packet that can be sent
    unsigned char   Next_Packet_Number;     // Next packet number to be sent
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   Not_Used2;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMCTS_Struct_Type;

  unsigned char i;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;        

  j1939_TPCMCTS_Struct_Type *DATA = (j1939_TPCMCTS_Struct_Type *)(&MessagePtr->Data[0]);

  j1939MailBox_ID_Struct_Type PGN_Requested;  

  // Extract requested 24-bits PGN number from J1939 Data Field
  PGN_Requested.IdField.Unused             = 0;
  PGN_Requested.IdField.PduSpecific        = DATA->PduSpecific;
  PGN_Requested.IdField.PduFormat          = DATA->PduFormat;
  PGN_Requested.IdField.BitField.DataPage  = DATA->BitField.DataPage;
  PGN_Requested.IdField.BitField.Reserved  = DATA->BitField.Reserved;  
  PGN_Requested.IdField.BitField.UnusedMSB = 0;

  // Check which Tx Mailbox is corresponding to this CTS
  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &(TxMultiPacket[i]);
    if( (TxMultiPacket_PTR->InProgress == 1) &&
        (PGN_Requested.Identifier == TxMultiPacket_PTR->PGN) &&
        (Originator_Addr == TxMultiPacket_PTR->DA) &&
        (Originator_Addr != 255) )
      break;
    else
      TxMultiPacket_PTR = NULL;
  }

  if(TxMultiPacket_PTR != NULL)
  {
     if(DATA->Next_Packet_Number == 0)
     {
        // If (Next Packet Number == 0), The Reponder wants to hold the connection open
        // but cannot receive any packet right now. A maximum of 500 mS later, the responder
        // should an other CTS to keep the connection Hold or to ask for an others packet.
        TxMultiPacket_PTR->DelayTimer  = 50;   // Session Abort scheduled in 500mS  
        TxMultiPacket_PTR->NextTxTimer = 60;   // Should be higher to avoir Data Transfert
        // Don't update the sequence number it's useless
        TxMultiPacket_PTR->Max_Num_Packet_CTS = DATA->Max_Num_Packet_CTS; // This value is determined by the Reception Node
        TxMultiPacket_PTR->Num_Packet_CTS = DATA->Max_Num_Packet_CTS;
     }
     else
     {
        TxMultiPacket_PTR->DelayTimer  = 20;    // Next Packet needs to be sent < 200 mS  
        TxMultiPacket_PTR->NextTxTimer = 5;     // Next Tx scheduled in 50 mS
        TxMultiPacket_PTR->Sequence_Number = DATA->Next_Packet_Number;    // This value is determined by the Reception Node
        TxMultiPacket_PTR->Max_Num_Packet_CTS = DATA->Max_Num_Packet_CTS; // This value is determined by the Reception Node
        TxMultiPacket_PTR->Num_Packet_CTS = DATA->Max_Num_Packet_CTS;
     }
  }
  // Else don't do anything
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_DataTransfert
*
* DESCRIPTION: Transmission of Packet of Data
*
* INPUTS     : TxMultiPacket_PTR - Pointer to the Tx Multi Packet Session
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : 
***********************************************************************************************/  
static UINT1 PGN_Tx_DataTransfert(j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR)
{
  // Local Structure use to Transmit the informations TP.DT 
  // See J1939-21 for reference
  typedef struct j1939_TPDT_Struct{
    unsigned char   SequenceNumber;               
    unsigned char   Data[7];
  }j1939_TPDT_Struct_Type;

  j1939_TPDT_Struct_Type DATA;
  unsigned char i;
  unsigned int Num_Byte_Tx;
  unsigned char *Data_Ptr;        // Local Pointer to the Data that needs to be transmitted
  j1939_SingleFrame_Message_Struct_Type  message;  // Local J1939 single frame message structure

  // Determine the Data Pointer Position
  Data_Ptr = TxMultiPacket_PTR->Data_Pgm_Ptr;
  Data_Ptr += ( (TxMultiPacket_PTR->Sequence_Number - 1) * 7);

  // Fill the Message Data  
  DATA.SequenceNumber = TxMultiPacket_PTR->Sequence_Number;
  // Check if it's the last packet of Data
  if(TxMultiPacket_PTR->Sequence_Number == TxMultiPacket_PTR->Packet_Size)
  {
    // It's the last Packet of Data !!!!
    Num_Byte_Tx = ( (TxMultiPacket_PTR->Sequence_Number -1) * 7);
    for(i=0;i<7;i++)
    {
      if(Num_Byte_Tx < TxMultiPacket_PTR->Msg_Size)
      {
        DATA.Data[i] = *Data_Ptr;
        Num_Byte_Tx++;
        Data_Ptr++;
      }
      else
        DATA.Data[i] = 0xFF;   // Fill the remaining Bytes with 0xFF
    }  
  }
  else
  {
    // It's not the last Packet of Data !!!
    for(i=0;i<7;i++)
    {
      DATA.Data[i] = *Data_Ptr;
      Data_Ptr++;
    }
  }

  message.BytePtr  = (UINT1 *) (&DATA.SequenceNumber);
  message.ByteCntr = sizeof(j1939_TPDT_Struct_Type);

  // Build the J1939 29-bit identifier fields
  message.MsgId.Identifier                = J1939_MULTI_PACKET_TPDT_PGN << 8;
  message.MsgId.IdField.BitField.Priority = J1939_MULTI_PACKET_PRIORITY;
  message.MsgId.IdField.PduSpecific       = TxMultiPacket_PTR->DA;
  message.MsgId.IdField.SourceAddress     = TxMultiPacket_PTR->Originator;
  
  // Send the Packet on the CAN Network. If the Message is not placed on the Queue CAN
  // don't do anything. The driver will try to send it again at the next Tread
  if( Dll_TxSingleFrameMessage(&message) == TRUE) // Send the Data Transfert Message
  {
    // Check if the Message is Global
    if(TxMultiPacket_PTR->DA == 255)
    {
      // Messsage is Global, Send the next Packet in 100 mS, don't wait for CTS
      TxMultiPacket_PTR->DelayTimer =  20;   // Tx Time Out set at 200mS
      TxMultiPacket_PTR->NextTxTimer = 5;    // Next Transmission schedule in 50 mS
      TxMultiPacket_PTR->Num_Packet_CTS = TxMultiPacket_PTR->Max_Num_Packet_CTS;  // Reception of CTS not needed, Set at Maximum        
      if(TxMultiPacket_PTR->Sequence_Number >= TxMultiPacket_PTR->Packet_Size)    // Check if it's the last Packet
      {
        TxMultiPacket_PTR->InProgress = 0;
        if(TxMultiPacket_PTR->TxMsgPtr != NULL)
        {
          TxMultiPacket_PTR->TxMsgPtr->TxInProgress = 0;
          TxMultiPacket_PTR->TxMsgPtr->TxReq = J1939_IDLE;
          TxMultiPacket_PTR->TxMsgPtr = NULL;
        }
      }
      TxMultiPacket_PTR->Sequence_Number++;                        // Increase the sequence Number for next Packet
    }
    else
    {
      // The message is destination Specific !!!
      // Check if we have to wait for a CTS, send New Data or wait for End Of Msg Ack
      if( --TxMultiPacket_PTR->Num_Packet_CTS == 0 )
      {
        // Yes we have to wait for a CTS before sending new Message
        TxMultiPacket_PTR->DelayTimer =  125;   // Delay maximum to receive a CTS
        TxMultiPacket_PTR->NextTxTimer = 135;   // Need to be set Higher to avoid transmission
        // Don't update the sequence Number, it's useless
        TxMultiPacket_PTR->Num_Packet_CTS = TxMultiPacket_PTR->Max_Num_Packet_CTS;
      }
      else if(TxMultiPacket_PTR->Sequence_Number == TxMultiPacket_PTR->Packet_Size)
      {
        // The last Packet of Data was Sent, Set the Timing to Wait for End of Msg Ack
        TxMultiPacket_PTR->DelayTimer =  125;   // Delay maximum to receive a CTS
        TxMultiPacket_PTR->NextTxTimer = 135;   // Need to be set Higher to avoid transmission
        TxMultiPacket_PTR->Sequence_Number ++;
      }
      else
      {
        // CTS or End Of Msg Ack are not Needed => Prepare the Next Data Transfert
        TxMultiPacket_PTR->DelayTimer =  20;   // Time Out need to be Higher that NextTxTimer
        TxMultiPacket_PTR->NextTxTimer = 5;    // Next Transmission schedule in 150 mS
        TxMultiPacket_PTR->Sequence_Number ++;
      } 
    }
    return(TRUE);
  }
  else
    return(FALSE);
}

/***********************************************************************************************
* FUNCTION   : PGN_Tx_TP_CM_EndOfMsgACK
*
* DESCRIPTION: Reception of a End of Message acknowledge
*
* INPUTS     : MessagePtr - Single frame message structure pointer
*              Originator_Addr - Address of the End Of Msg ACK Originator
*
* OUTPUTS    : 
*           
* COMMENTS   : 
***********************************************************************************************/  
static void PGN_Rx_TP_CM_EndOfMsgACK(can_Msg_Struct *MessagePtr, UINT1 Originator_Addr)
{

  // Local Structure use to send the informations TP.CM.EndOfMsgAck 
  // This structure is used to simplify the Byte manipulation
  // See J1939-21 for reference
  typedef struct j1939_TPCMAck_Struct{
    unsigned char   Control_Byte;           // Control Byte = 19 for End of Msg Ack
    unsigned char   Msg_Size_LSB;
    unsigned char   Msg_Size_MSB;
    unsigned char   Packet_Size;
    unsigned char   Not_Used1;              // Should be filed with FF
    unsigned char   PduSpecific;
    unsigned char   PduFormat;
    struct{
      unsigned char DataPage  :1;
      unsigned char Reserved  :1;
      unsigned char UnusedMSB :6;
    }BitField;
  }j1939_TPCMAck_Struct_Type;

  unsigned char i;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;        
  j1939_TPCMAck_Struct_Type *DataPtr = (j1939_TPCMAck_Struct_Type *)(&MessagePtr->Data[0]);
  j1939MailBox_ID_Struct_Type PGN_Requested;  


  // Extract 24-bits PGN number
  PGN_Requested.IdField.Unused             = 0;
  PGN_Requested.IdField.PduSpecific        = DataPtr->PduSpecific;
  PGN_Requested.IdField.PduFormat          = DataPtr->PduFormat;
  PGN_Requested.IdField.BitField.DataPage  = DataPtr->BitField.DataPage;
  PGN_Requested.IdField.BitField.Reserved  = DataPtr->BitField.Reserved;  
  PGN_Requested.IdField.BitField.UnusedMSB = 0;

  // Check which Tx Mailbox is corresponding to this End of Msg Ack
  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &(TxMultiPacket[i]);
    if( (TxMultiPacket_PTR->InProgress == 1) &&
        (PGN_Requested.Identifier == TxMultiPacket_PTR->PGN) &&
        (Originator_Addr == TxMultiPacket_PTR->DA) &&
        (Originator_Addr != 255) &&
        (TxMultiPacket_PTR->Sequence_Number >= TxMultiPacket_PTR->Packet_Size) )
      break;
    else
      TxMultiPacket_PTR = NULL;
  }

  if(TxMultiPacket_PTR !=NULL)
  {
    TxMultiPacket_PTR->InProgress = 0;
    TxMultiPacket_PTR->TxMsgPtr->TxInProgress = 0;
    TxMultiPacket_PTR->TxMsgPtr->TxReq = J1939_IDLE;
  }
}


/***********************************************************************************************
* FUNCTION   : Dll_TxMultiPacket_Init
*
* DESCRIPTION: Initialisation of a MultiPacket transmission
*
* INPUTS     : Msg_J1939 - Pointer to the J1939 Mailbox Element that need to be transmitted
*              Msg_Flash - 
*
* OUTPUTS    : TRUE  - Single frame PGN sent to Tx queue
*              FALSE - Tx queue full, no PGN sent
*           
* COMMENTS   : 
***********************************************************************************************/  
static UINT1 Dll_TxMultiPacket_Init( j1939_TxMsg_Struct *Msg_j1939)
{
  
  unsigned char i;
  unsigned int Temp;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;   

  // Check Tx MultiPacket session is free
  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &(TxMultiPacket[i]);
    if (TxMultiPacket_PTR->InProgress == 0) 
      break;
    else
      TxMultiPacket_PTR = NULL;
  }
  
  // Be certain that a MultiPacket-transmission is not already in progress with this node
  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    if( (TxMultiPacket[i].InProgress == 1) && (TxMultiPacket[i].DA == Msg_j1939->Dest_Addr) )
       TxMultiPacket_PTR = NULL;
  }

  if (TxMultiPacket_PTR != NULL)    // A transmission of J1939 PGN is needed
  {
     // Fill the MultiPacket State Variables with the Good Information

     TxMultiPacket_PTR->DA = Msg_j1939->Dest_Addr;      // Destination Address
     TxMultiPacket_PTR->Originator = MyAddress;         // Data Originator
     TxMultiPacket_PTR->PGN = Msg_j1939->PGN;           // PGN
     TxMultiPacket_PTR->Msg_Size = Msg_j1939->DataSize; // Message Size     
     Temp = (UINT1) (Msg_j1939->DataSize / 7);          // Determine the number of packets that needs to be sent
     if( (Msg_j1939->DataSize % 7) != 0 )
        Temp++;
     TxMultiPacket_PTR->Packet_Size = (UINT1)Temp;
     TxMultiPacket_PTR->Sequence_Number = 1;            // Sequence Number = 1, the first sequence
     TxMultiPacket_PTR->Max_Num_Packet_CTS = J1939_MAX_NUMBER_PACKET_BEFOFE_CTS;
     TxMultiPacket_PTR->Num_Packet_CTS     = J1939_MAX_NUMBER_PACKET_BEFOFE_CTS;

     // If the Message is Destination Specific
     if(TxMultiPacket_PTR->DA != 255)
     {
        TxMultiPacket_PTR->DelayTimer  = 125;  
        TxMultiPacket_PTR->NextTxTimer = 135;           
        if( PGN_Tx_TP_CM_RTS(TxMultiPacket_PTR) == TRUE)
        {
           TxMultiPacket_PTR->InProgress = 1;
           Msg_j1939->TxInProgress = 1;
           TxMultiPacket_PTR->TxMsgPtr = Msg_j1939;
           TxMultiPacket_PTR->Data_Pgm_Ptr = Msg_j1939->Data;
           return (TRUE);
        }
     }
     else // The message is Destination Global
     {
        TxMultiPacket_PTR->DelayTimer  = 20;    // Time Out delay
        TxMultiPacket_PTR->NextTxTimer = 5;    // Delay between 2 packets is set at 50ms           
        if( PGN_Tx_TP_CM_BAM(TxMultiPacket_PTR) == TRUE)
        {
           TxMultiPacket_PTR->InProgress = 1;
           Msg_j1939->TxInProgress = 1;
           TxMultiPacket_PTR->TxMsgPtr = Msg_j1939;
           TxMultiPacket_PTR->Data_Pgm_Ptr = Msg_j1939->Data;                   
           return(TRUE);
        }
     }
  }
  return(FALSE);
}
#endif //SUPPORT_MULTIPACKET

/***********************************************************************************************
* FUNCTION   : Dll_TxSingleFrameMessage
*
* DESCRIPTION: 
*
* INPUTS     : Pgn - J1939 single frame PGN control structure pointer
*
* OUTPUTS    : TRUE  - Single frame PGN sended to Tx queue or MSCAN
*              FALSE - Tx queue full or no MSCAN Tx buffer, no PGN sending
*           
* COMMENTS   : 
***********************************************************************************************/
static UINT1 Dll_TxSingleFrameMessage(j1939_SingleFrame_Message_Struct_Type *Message)
{
    can_Msg_Struct *mailboxPtr;
    UINT1                   *mailboxDataPtr;

    // Get MSCAN TX queue mailbox message pointer
    mailboxPtr = can_WriteTxQueue();

    // MSCAN TX queue mailbox message trap (queue not full)
    if (mailboxPtr != NULL)
    {
        // Put J1939 message directly into TX queue mailbox message
        mailboxPtr->Id         = Message->MsgId.Identifier;
        mailboxPtr->Ide   = 1;   // Extended ID Type
        mailboxPtr->Rtr   = 0;
        mailboxPtr->Dlc   = Message->ByteCntr;

        // Set MSCAN data destination pointer 
        mailboxDataPtr = &mailboxPtr->Data[0];

        // Copy J1939 message data into TX queue mailbox message
        while (Message->ByteCntr--)
        {
            *mailboxDataPtr++ = *Message->BytePtr++;
        }
         // TX mailbox message pushed to queue
         can_PushTxQueue();
    }

    if (mailboxPtr == NULL) return (FALSE); // No sending, Tx queue full
    else                    return (TRUE);  // TX message pushed to queue
}


/***********************************************************************************************
* FUNCTION   : Dll_RxSingleFrameMessage
*
* DESCRIPTION: This function fill the pointed J1939 message structure fields
*
* INPUTS     : MsgObjPtr - J1939 message structure pointer
*                          All fields is empty except MSCAN node index reference
*
* OUTPUTS    : TRUE  - RxMsg pulled from queue
*              FALSE - Rx queue is empty, no message pulled
*           
* COMMENTS   :
***********************************************************************************************/
/*
static UINT1 Dll_RxSingleFrameMessage(j1939_SingleFrame_Message_Struct_Type *Message)
{
    can_Msg_Struct *mailboxPtr;
    UINT1                  *mailboxDataPtr;

    // Get MSCAN RX queue mailbox message pointer
    mailboxPtr = can_ReadRxQueue();

    // MSCAN RX queue mailbox message trap (queue is not empty)
    if (mailboxPtr != NULL)
    {

        switch (mailboxPtr->Ide)
        {
            case 0:
                // For now theses messages are ignored
                break;

            case 1:
                
                // Set MSCAN data source pointer 
                mailboxDataPtr = &mailboxPtr->Data[0];

                // Fill J1939 message directly from RX queue mailbox message
                Message->MsgId.Identifier = mailboxPtr->Id;
                Message->ByteCntr = mailboxPtr->Dlc;

                // Fill J1939 message data from RX queue mailbox message
                while (mailboxPtr->Dlc--)
                {
                    *Message->BytePtr++ = *mailboxDataPtr++;
                }

                // RX mailbox message pulled from queue
                can_PullRxQueue();

                break;
        }
    }
    else
    {
        // No RX queue mailbox message available (queue is empty)
    }

    if (mailboxPtr == NULL) return (FALSE); // No more message, RX queue empty
    else                    return (TRUE);  // RX message pulled from queue
}
*/

/***********************************************************************************************
* FUNCTION   : Dll_GetRandomTimeDelay
*
* DESCRIPTION: 
*
* INPUTS     : NameFld  -  J1939 name field structure pointer
*
* OUTPUTS    : Time     -  J1939 random time delay (0..153ms)
*           
* COMMENTS   : 
***********************************************************************************************/
static UINT1 Dll_GetRandomTimeDelay(UINT1 *NameFld)
{
  UINT1  i   = sizeof(j1939_NAME_Fields_Struct_Type);
  UINT1  Sum = 0;
  UINT2  Time;

  // Keep only last byte of the NAME field bytes summation
  while (i--) Sum += *NameFld++;
  // J1939 random time adjust [ms]
  Time = (Sum * 3) / 5;

  return (UINT1)(Time);
}


/***********************************************************************************************
* STEP       :  Dll_S0_AddressClaim_Idle
*
* DESCRIPTION:  STATE 0  -  This is the idle state. The state machine is waiting to be started.
*               In this J1939 device we would transistion from idle state after succesfull
*               completion of power on self test. The machine stay in state 0 as long as the
*               state variable "IdleState" is true. When this variable going to false the
*               action "StartRandomDelay" is completed and go to state 1.
***********************************************************************************************/
static void Dll_S0_AddressClaim_Idle(void)
{
  // Address Claim Procedures
  AddrClaim.IdleState          = !J1939_ADDRESS_CLAIM_ENABLED;
  AddrClaim.StartRandomDelay   = FALSE;
  AddrClaim.Start250MsDelay    = FALSE;
  AddrClaim.DelayComplete      = FALSE;
  AddrClaim.SelectStartAddress = FALSE;
  AddrClaim.CommandedAddress   = FALSE;
  AddrClaim.FetchNextMyAddress = FALSE;
  AddrClaim.AddressClaimed     = FALSE;
  AddrClaim.CannotClaimAddress = FALSE;
  AddrClaim.IWin               = FALSE;
  AddrClaim.ILose              = FALSE;
  AddrClaim.CanMsgSent         = FALSE;
  AddrClaim.NoMessage          = FALSE;
  AddrClaim.RequestPgn         = FALSE;

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T0_AddressClaim_Idle;
}


/***********************************************************************************************
* TRANSITION :  Dll_T0_AddressClaim_Idle
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T0_AddressClaim_Idle(void)
{
  if (                                              // T0A
     (AddrClaim.IdleState == FALSE)                 // End of power on self test
     ) Dll_G7_AddressClaim = Dll_S1_AddressClaim_RandomDelay;
                                                        // T0B
  else Dll_G7_AddressClaim = Dll_T0_AddressClaim_Idle;  // Stay idle, wait to be started...
}


/***********************************************************************************************
* STEP       :  Dll_S1_AddressClaim_RandomDelay
*
* DESCRIPTION:  STATE 1  -  The action StartRandomDelay starts a timer with a random time out
*               period. ISO11783-5:2001(E) specifies how this random delay period is calculated.
*               Completing the action StartRandomDelay sets the state variable "DelayComplete"
*               to false. This state constinuously tests the timer that was started during the
*               transition from state 0 to state 1. If the state variable "DelayComplete" is
*               false the machine will stay in state 1. When DelayComplete is true present state
*               is forced to 2, so on the state transition diagram there is no forward slash
*               after the state variable "DelayComplete".
***********************************************************************************************/
static void Dll_S1_AddressClaim_RandomDelay(void)
{

  j1939_NAME_Fields_Struct_Type   MyName;
  unsigned int                       Temp;

  // First, Built my own Name using the Name Fields define by the High Application Level Soft
  MyName.Byte8.ArbitraryAddressCapable = j1939_Parameter->Name_Fields.Self_Cfg_Addr & 0x01;
  MyName.Byte8.IndustryGroup           = j1939_Parameter->Name_Fields.Industry_Group & 0x07;
  MyName.Byte8.VehicleSystemInstance   = j1939_Parameter->Name_Fields.Vehicle_Sys_Inst & 0x0F;
  MyName.Byte7.VehicleSystem           = j1939_Parameter->Name_Fields.Vehicle_System & 0x7F;
  MyName.Byte7.Reserved                = j1939_Parameter->Name_Fields.Reserved;
  MyName.DeviceFunction                = j1939_Parameter->Name_Fields.Function;
  MyName.Byte5.DeviceFunctionInstance  = j1939_Parameter->Name_Fields.Function_Instance & 0x1F;
  MyName.Byte5.EcuInstance             = j1939_Parameter->Name_Fields.ECU_Instance & 0x07;
  Temp                                 = j1939_Parameter->Name_Fields.Manufacturer_Code << 5;
  MyName.ManufacturerCode_Byte4        = _HIBYTE(Temp);
  MyName.Byte3.ManufacturerCode_Byte3  = _LOBYTE(j1939_Parameter->Name_Fields.Manufacturer_Code) & 0x07;
  MyName.Byte3.IdentityNumber_Byte3    = _LOBYTE(_HIWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte2          = _HIBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte1          = _LOBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));  

  // Address Claim Procedures
  AddrClaim.SelectStartAddress = TRUE;
  AddrClaim.DelayComplete      = FALSE;
  AddrClaim.StartRandomDelay   = TRUE;

  // Random timeout delay timer log time
  j1939_DelayTmr = j1939_FreeRunTmr + Dll_GetRandomTimeDelay((UINT1 *) (&MyName));

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T1_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* TRANSITION :  Dll_T1_AddressClaim_RandomDelay
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T1_AddressClaim_RandomDelay(void)
{
  // Random timeout delay timer overrun trap...
  if (j1939_FreeRunTmr >= j1939_DelayTmr) AddrClaim.DelayComplete = TRUE;

  if (                                              // T1A
     (AddrClaim.DelayComplete == TRUE)              // End of J1939 random delay time
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;
                                                               // T1B
  else Dll_G7_AddressClaim = Dll_T1_AddressClaim_RandomDelay;  // Wait end of delay...
}


/***********************************************************************************************
* STEP       :  Dll_S2_AddressClaim_Transmit
*
* DESCRIPTION:  STATE 2  -  In this state we transmit the address claim message. The detailled
*               structure of an address claim message is described in ISO11783-5. Basically we
*               are broadcasting to all other J1939 devices that we (this device) desires
*               to claim (use) the address contained within the address claim message. There are
*               two possible transitions from state 2.
*
*                    I) If the message was sent without error by the CAN controller
*                   II) The message was NOT sent without error by the CAN controller
*
*               The CAN controller device driver will set the state variable "CanMsgSent" true
*               or false. If the message was sent without error, we complete the action
*               "Start250MsDelay" and force the present state to state 3. If the message was not
*               sent without error, we complete the action StartRandomDelay and force the
*               present state to state 1.
***********************************************************************************************/
static void Dll_S2_AddressClaim_Transmit(void)
{
  // Address Claim Procedures
  AddrClaim.StartRandomDelay   = FALSE;
  AddrClaim.Start250MsDelay    = FALSE;
  AddrClaim.AddressClaimed     = FALSE;
  AddrClaim.CannotClaimAddress = FALSE;

  #if J1939_COMMANDED_ADDRESS_ENABLED
  if (AddrClaim.CommandedAddress == TRUE)
  {
    // Address Claim Procedures
    AddrClaim.SelectStartAddress = FALSE;
    AddrClaim.FetchNextMyAddress = FALSE;
    // Update to request commanded address
    MyAddress = Commanded;
  }
  #endif

  // Transmit address claim message to all network devices
  AddrClaim.CanMsgSent = PGN_Tx_ISOAddressClaim(J1939_GLOBAL_ADDRESS, MyAddress);

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T2_AddressClaim_Transmit;
}


/***********************************************************************************************
* TRANSITION :  Dll_T2_AddressClaim_Transmit
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T2_AddressClaim_Transmit(void)
{
  if (                                              // T2A
     (AddrClaim.CanMsgSent == TRUE)                 // Message transmited without error
     ) Dll_G7_AddressClaim = Dll_S3_AddressClaim_RxCanFrame;

  else if (                                         // T2B
     (AddrClaim.CanMsgSent == FALSE)                // Message NOT trasmited
     ) Dll_G7_AddressClaim = Dll_S1_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* STEP       :  Dll_S3_AddressClaim_RxCanFrame
*
* DESCRIPTION:  STATE 3  -  This state wait for a response to the address claim messsage sent
*               in state 2. The device may only respond to the address claim messages in state 3.
*               Successfull claiming of an address by an device consists of sending an address
*               claim message for the address to be claimed and not receiving contending claims
*               from other ECUs for 250ms.
*
*               The 250ms time out period was initiated during the trasition from state 2 to
*               state 3. We wait in state 3 to allow other devices on the network to challenge
*               our address claim.
*
*               The NAME field is a 64-bit field. Details of the NAME field can be found in J1939
*               Standard and ISO11783. For the purpose of address claim we treat the NAME
*               field as a 64-bit unsigned integer, the smaller this integer, the higher is the
*               priority. It is NEVER permitted to have two or more devices on the network with
*               identical NAME field.
*
*               A device that have previously claimed the address we wish to claim AND has a
*               higher priority will chalenge us by sending and address claim message containing
*               the address in dispute. It is also possible that another device that is powered
*               on during this 250ms period may try to claim the same address we wish to claim.
*               In either case we receive and process address claim messages that have the same
*               source address at the one we wish to claim. We process this received message in
*               state 3 and determine if the other device has a higher priority, or lower
*               priority then ourselves, this is done by comparing our 64-bit NAME field to the
*               other devices NAME field.
*
*               If we process a received address claim message and the 64-bit NAME field of this
*               received address claim message is numericaly less than our own NAME field, then
*               the state variable "ILose" is set to true. This is because we have lost the
*               address claim chalenge and complete FetchNextMyAddress action in state 4.
*
*               If we process a received address claim message and the 64-bit NAME field of this
*               received address claim message is numericaly greather than our own NAME field,
*               then the state variable "IWin" is set to true. This is because we have won the
*               address claim chalenge. We transition to state 2 so we can again send our
*               address claim message and start the 250ms time out period. Though we won the
*               challenge, by specification we must wait 250ms in order to start using this
*               claimed address for messages other than address claim or request for address
*               claim.
*
*               The action Start250MsDelay starts a time with a fixed timeout period of 250ms.
*               ISO11783-5 specifies this fixed delay. Completing the action Start250MsDelay
*               negates the state variable "DelayComplete". When the 250ms delay times out we
*               then go to state 5.
***********************************************************************************************/
static void Dll_S3_AddressClaim_RxCanFrame(void)
{
   // Address Claim Procedures
  AddrClaim.IWin      = FALSE;
  AddrClaim.ILose     = FALSE;
  AddrClaim.NoMessage = FALSE;

  // One shoot non-retrigable fixed 250ms timeout delay
  if (AddrClaim.Start250MsDelay == FALSE)
  {
    // Address Claim Procedures
    AddrClaim.DelayComplete   = FALSE;
    AddrClaim.Start250MsDelay = TRUE;

    // Log time of fixed 250ms timeout delay timer
    j1939_DelayTmr = j1939_FreeRunTmr + 250;
  }

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T3_AddressClaim_RxCanFrame;
}


/***********************************************************************************************
* TRANSITION : Dll_T3_AddressClaim_RxCanFrame
*
* DESCRIPTION: Wait to receive address claim messages from other network devices (3-bit XOR set)
*              If received message address claim win chalenge -> IWin == TRUE;
*              If received message address claim lose chalenge -> ILose == TRUE;
*              Not either address claim or PGN request for address address -> NoMessage == TRUE;
************************************************************************************************/
static void Dll_T3_AddressClaim_RxCanFrame(void)
{
  // Fixed 250ms timeout delay timer overrun trap...
  if (j1939_FreeRunTmr >= j1939_DelayTmr) AddrClaim.DelayComplete = TRUE;

  if (                                              // T3A
     (AddrClaim.IWin == TRUE)                       // Address claim message received and...
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;  //  ...win chalenge

  else if (                                         // T3B
     (AddrClaim.ILose == TRUE)                      // Address claim message received and...
     ) Dll_G7_AddressClaim = Dll_S4_AddressClaim_FetchNextMyAddress;  //  ...lose chalenge

  else if (                                         // T3D
     (AddrClaim.DelayComplete == TRUE)              // End of ISO11783 fixed 250ms timeout delay
     ) Dll_G7_AddressClaim = Dll_S5_AddressClaim_RxCanFrame;

  #if J1939_COMMANDED_ADDRESS_ENABLED
  else if (                                         // T3E
     (AddrClaim.CommandedAddress == TRUE)           // Commanded address message received
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;
  #endif
                                                              // T3F
  else Dll_G7_AddressClaim = Dll_T3_AddressClaim_RxCanFrame;  // Wait Rx message...
}


/***********************************************************************************************
* STEP       :  Dll_S4_AddressClaim_FetchNextMyAddress
*
* DESCRIPTION:  STATE 4  -  This where we try and generate a new address. The source address SA
*               that we are trying to claim is an 8-bit value, so there are 256 possible
*               addresses (0..255) except address 252, 253, 254, 255 that are reserved. So the
*               possible addresses we may claim are 0 through 251 for a total of 252 addresses.
*
*               ISO11783-5 does not specify any particular algorithm that a device must use in
*               order to choose a source address. However it is recommended that once a source
*               address is claimed that the device attempt to claim this same address when the
*               device is again powered down then powered on. The idea being that once a network
*               is operating and all of the device have a source address, there will be a minimum
*               of thrashing on the network whenever a device is powered on.
*
*               ISO11783-5 does specify that in the event a device cannot find an address to
*               claim it may NOT participate in network communications except to respond to a
*               request for address claim message.
*
*               In state 4 we generate a new source address by adding 1 to the present source
*               address, if the new source address is greather than 251 we are out of address
*               and must give up to state 6 and completing the action StartRandomDelay. If the
*               new source address is less than 252 we transition to state 2 where we send an
*               address claim message.
***********************************************************************************************/
static void Dll_S4_AddressClaim_FetchNextMyAddress(void)
{
 
  // Address Claim Procedures
  AddrClaim.Start250MsDelay = FALSE;

  // Next source address to be claimed except J1939 reserved source addresses
  if (++MyAddress >= J1939_ADDRESS_CLAIM_MAX_ADDRESS) MyAddress = 0;

  // Source address rollover check
  AddrClaim.FetchNextMyAddress = (MyAddress != *j1939_Parameter->PreferedSA) ? TRUE : FALSE;

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T4_AddressClaim_FetchNextMyAddress;
}


/***********************************************************************************************
* TRANSITION :  Dll_T4_AddressClaim_FetchNextMyAddress
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T4_AddressClaim_FetchNextMyAddress(void)
{
  if (                                              // T4A
     (AddrClaim.FetchNextMyAddress == TRUE)         // Valid new source address
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;

  else if (                                         // T4B
     (AddrClaim.FetchNextMyAddress == FALSE)        // No more possible source address
     ) Dll_G7_AddressClaim = Dll_S6_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* STEP       :  Dll_S5_AddressClaim_RxCanFrame
*
* DESCRIPTION:  STATE 5  -  This state means we have succesfully claimed an address on the
*               J1939 network. We gave the other devices 250ms to challenge our claimed
*               address and either no other device challenged our claim or we won the claim.
*               At this point the device is free to transmit message that pertain to it's normal
*               operation. Any message that is NOT either address claim or request for address
*               claim, is designated as "NoMessage", no processing is done for messages that are
*               defined as NoMessage. The only way to transition out of state 5 is to receive
*               either an address claim message or receive a request for address claim message.
*
*               While in state 5 it is possible for other devices to be powered on, these devices
*               may send address claim messages with the same source address SA that we have
*               claimed. So we compare the received NAME field to our own NAME field just like
*               state 3. The result of this compare will set one of the state variables "IWin"
*               or "ILose" to true.
*
*               We may also receive a request for address claim message, in which no comparison
*               of NAME field is required, just send an address claim message.
***********************************************************************************************/
static void Dll_S5_AddressClaim_RxCanFrame(void)
{
  // Address Claim Procedures
  AddrClaim.IWin            = FALSE;
  AddrClaim.ILose           = FALSE;
  AddrClaim.NoMessage       = FALSE;
  AddrClaim.RequestPgn      = FALSE;
  AddrClaim.Start250MsDelay = FALSE;
  AddrClaim.AddressClaimed  = TRUE;

  // Advise the Application that the Address Claim is finished 
  // If needed updated the current source address for the next Power-Up
  j1939_Status.AddClaim = TRUE;   
  if (MyAddress != j1939_Status.SA)
    j1939_Status.SA = MyAddress;

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T5_AddressClaim_RxCanFrame;
}


/***********************************************************************************************
* TRANSITION :  Dll_T5_AddressClaim_RxCanFrame
*
* DESCRIPTION: Receive address claim messages from other network devices (4-bit XOR set) 
*              If received message address claim win challenge -> IWin == TRUE;
*              If received message address claim lose challenge -> ILose == TRUE;
*              If received message request for address claim -> RequestPgn == TRUE;
*              Not either address claim or PGN request for address claim -> NoMessage == TRUE;
***********************************************************************************************/
static void Dll_T5_AddressClaim_RxCanFrame(void)
{
  if (                                              // T5A
    ((AddrClaim.RequestPgn == TRUE) ||              // Receive a request PGN for address claim
     (AddrClaim.IWin == TRUE))                      // Receive address claim message and...
     ) Dll_G7_AddressClaim = Dll_S9_AddressClaim_Transmit;  //  ...win chalenge

  else if (                                         // T5B
     (AddrClaim.ILose == TRUE)                      // Receive address claim message and...
     ) Dll_G7_AddressClaim = Dll_S4_AddressClaim_FetchNextMyAddress;  //  ...lose chalenge

  #if J1939_COMMANDED_ADDRESS_ENABLED
  else if (                                         // T5D
     (AddrClaim.CommandedAddress == TRUE)           // Commanded address message received
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;
  #endif
                                                              // T5E
  else Dll_G7_AddressClaim = Dll_T5_AddressClaim_RxCanFrame;  // Wait Rx message...
}


/***********************************************************************************************
* STEP       :  Dll_S6_AddressClaim_RandomDelay
*
* DESCRIPTION:  STATE 6  -  The transition from state 4 to state 6 means we have fail to
*               succesfully claim a source address SA. In accordance with ISO11783-5 we wait a
*               random delay then transmit a can not claim source address message.The can not
*               claim message is an address claim message with the source address SA field equal
*               to 254. According to ISO11783-5, a device which cannot claim a source address
*               may only to respond for address claim messages.
*
*               The only way to attempt further address claiming is to cycle the device power.
*               In state 6 we wait for the random time out delay initiated during the trasition
*               from state 4 to state 6. When the delay is completed we transition to state 7.
***********************************************************************************************/
static void Dll_S6_AddressClaim_RandomDelay(void)
{

  j1939_NAME_Fields_Struct_Type      MyName;
  UINT2                              Temp;

  // First, Built my own Name using the Name Fields define by the High Application Level Soft
  MyName.Byte8.ArbitraryAddressCapable = j1939_Parameter->Name_Fields.Self_Cfg_Addr & 0x01;
  MyName.Byte8.IndustryGroup           = j1939_Parameter->Name_Fields.Industry_Group & 0x07;
  MyName.Byte8.VehicleSystemInstance   = j1939_Parameter->Name_Fields.Vehicle_Sys_Inst & 0x0F;
  MyName.Byte7.VehicleSystem           = j1939_Parameter->Name_Fields.Vehicle_System & 0x7F;
  MyName.Byte7.Reserved                = j1939_Parameter->Name_Fields.Reserved;
  MyName.DeviceFunction                = j1939_Parameter->Name_Fields.Function;
  MyName.Byte5.DeviceFunctionInstance  = j1939_Parameter->Name_Fields.Function_Instance & 0x1F;
  MyName.Byte5.EcuInstance             = j1939_Parameter->Name_Fields.ECU_Instance & 0x07;
  Temp                                 = j1939_Parameter->Name_Fields.Manufacturer_Code << 5;
  MyName.ManufacturerCode_Byte4        = _HIBYTE(Temp);
  MyName.Byte3.ManufacturerCode_Byte3  = _LOBYTE(j1939_Parameter->Name_Fields.Manufacturer_Code) & 0x07;
  MyName.Byte3.IdentityNumber_Byte3    = _LOBYTE(_HIWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte2          = _HIBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));
  MyName.IdentityNumber_Byte1          = _LOBYTE(_LOWORD(j1939_Parameter->Name_Fields.Identity_Number));  

  // Address Claim Procedures
  AddrClaim.DelayComplete    = FALSE;
  AddrClaim.StartRandomDelay = TRUE;

  // Random timeout delay timer log time
  j1939_DelayTmr = j1939_FreeRunTmr + Dll_GetRandomTimeDelay((UINT1 *) (&MyName));

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T6_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* TRANSITION :  Dll_T6_AddressClaim_RandomDelay
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T6_AddressClaim_RandomDelay(void)
{
  // Random timeout delay timer overrun trap...
  if (j1939_FreeRunTmr >= j1939_DelayTmr) AddrClaim.DelayComplete = TRUE;

  if (                                              // T6A
     (AddrClaim.DelayComplete == TRUE)              // End of j1939 random delay time
     ) Dll_G7_AddressClaim = Dll_S7_AddressClaim_Transmit;
                                                               // T6B
  else Dll_G7_AddressClaim = Dll_T6_AddressClaim_RandomDelay;  // Wait end of delay...
}


/***********************************************************************************************
* STEP       :  Dll_S7_AddressClaim_Transmit
*
* DESCRIPTION:  STATE 7  -  We transmit an address claim message with source address SA equal
*               to 254. By definition this is called a cannot claim source address message. If
*               the CAN controller sent the message, we transition to state 8, other wise we
*               start a random delay and go back to state 6 to wait for the random delay period
*               to time out.
***********************************************************************************************/
static void Dll_S7_AddressClaim_Transmit(void)
{
  // Address Claim Procedures
  AddrClaim.StartRandomDelay = FALSE;

  // This node can not claim an address...
  // Set this device address to J1939 null address
  MyAddress = J1939_ADDRESS_CLAIM_NULL_ADDRESS;

  // Transmit address claim message to all network devices
  AddrClaim.CanMsgSent = PGN_Tx_ISOAddressClaim(J1939_GLOBAL_ADDRESS, MyAddress);

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T7_AddressClaim_Transmit;
}


/***********************************************************************************************
* TRANSITION :  Dll_T7_AddressClaim_Transmit
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T7_AddressClaim_Transmit(void)
{
  if (                                              // T7A
     (AddrClaim.CanMsgSent == TRUE)                 // Message transmited without error
     ) Dll_G7_AddressClaim = Dll_S8_AddressClaim_RxCanFrame;

  else if (                                         // T7B
     (AddrClaim.CanMsgSent == FALSE)                // Message NOT trasmited
     ) Dll_G7_AddressClaim = Dll_S6_AddressClaim_RandomDelay;
}


/***********************************************************************************************
* STEP       :  Dll_S8_AddressClaim_RxCanFrame
*
* DESCRIPTION:  STATE 8  -  We process received messages in this state. In accordance with
*               ISO11783-5, the ONLY message this device may respond to is a request for source
*               address SA message, ALL other messages are ignored. If a request message is
*               received, we initiate a random delay period and made transition to state 6 to
*               wait for the period to expire, then transtion to state 7 to transmit a can not
*               claim address message.
***********************************************************************************************/
static void Dll_S8_AddressClaim_RxCanFrame(void)
{
  // Address Claim Procedures
  AddrClaim.RequestPgn         = FALSE;
  AddrClaim.CannotClaimAddress = TRUE;

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T8_AddressClaim_RxCanFrame;
}


/***********************************************************************************************
* TRANSITION :  Dll_T8_AddressClaim_RxCanFrame
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T8_AddressClaim_RxCanFrame(void)
{
  if (                                              // T8A
     (AddrClaim.RequestPgn == TRUE)                 // Receive a request PGN for address claim
     ) Dll_G7_AddressClaim = Dll_S6_AddressClaim_RandomDelay;

  #if J1939_COMMANDED_ADDRESS_ENABLED
  else if (                                         // T8B
     (AddrClaim.CommandedAddress == TRUE)           // Commanded address message received
     ) Dll_G7_AddressClaim = Dll_S2_AddressClaim_Transmit;
  #endif
                                                              // T8C
  else Dll_G7_AddressClaim = Dll_T8_AddressClaim_RxCanFrame;  // Wait request PGN or commanded address
}


/***********************************************************************************************
* STEP       :  Dll_S9_AddressClaim_Transmit
*
* DESCRIPTION:  STATE 9  -  This state simply transmit an address claim message containing our
*               claimed address. There are two reasons that would cause a transtion to state 9.
*
*                    I) We received an address claim message with an address that matches our
*                       own claimed address and 64-bit NAME field of the received address claim
*                       message is numerically greather than our own NAME field, so we are
*                       reasserting our claimed address.
*                   II) We receive a request for address claimed message, so we are simply
*                       responding with our NAME field and address.
*
*               Now it is possible that the CAN controller did not succesfully send the message
*               in state 9, so in this case we go to a random delay, otherwise we go to state 5.
***********************************************************************************************/
static void Dll_S9_AddressClaim_Transmit(void)
{
  // Transmit address claim message to all network devices
  AddrClaim.CanMsgSent = PGN_Tx_ISOAddressClaim(J1939_GLOBAL_ADDRESS, MyAddress);

  // Grafcet control pointer return
  Dll_G7_AddressClaim = Dll_T9_AddressClaim_Transmit;
}


/***********************************************************************************************
* TRANSITION :  Dll_T9_AddressClaim_Transmit
*
* DESCRIPTION:  
***********************************************************************************************/
static void Dll_T9_AddressClaim_Transmit(void)
{
  if (                                              // T9A
     (AddrClaim.CanMsgSent == TRUE)                 // Message transmited without error
     ) Dll_G7_AddressClaim = Dll_S5_AddressClaim_RxCanFrame;

  else if (                                         // T9B
     (AddrClaim.CanMsgSent == FALSE)                // Message NOT trasmited
     ) Dll_G7_AddressClaim = Dll_S1_AddressClaim_RandomDelay;
}

#if SUPPORT_MULTIPACKET
/***********************************************************************************************
* FUNCTION   : DLL_MultiPacket_TimeOut
*
* DESCRIPTION: When a Multi Packet session is in progress many timing have to be
*              respected. If a time out occurs, the session needs to be aborted
*              In addition, if the session is destination specific the function 
*              send a Connection Abort Message.
*
* INPUTS     : None, the function works only with the structure MultiPacket
*
* OUTPUTS    : MultiPacket structure is modified and Conn Abort can be sent
*           
* COMMENTS   : 
*
***********************************************************************************************/
static void Dll_MultiPacket_TimeOut(void)
{
  unsigned char i;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;

  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];

    if (RxMultiPacket_PTR->InProgress == 1)    // If Multi Packet session is in progress
    {
      if(RxMultiPacket_PTR->DelayTimer == 0)   // If a TimeOut has occurs
      {        
        if(RxMultiPacket_PTR->DA != 255)   // If the Session is Destination Specific
          PGN_Tx_TP_Conn_Abort(RxMultiPacket_PTR->Originator,3,RxMultiPacket_PTR->PGN);
        RxMultiPacket_PTR->InProgress = 0;
        RxMultiPacket_PTR->RxMsgPtr->Rx_InProgress = 0;
      } 
    }
  }

  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &TxMultiPacket[i];

    if (TxMultiPacket_PTR->InProgress == 1)    // If Multi Packet session is in progress
    {
      if(TxMultiPacket_PTR->DelayTimer == 0)   // If a TimeOut has occurs
      {        
        if(TxMultiPacket_PTR->DA != 255)   // If the Session is Destination Specific
          PGN_Tx_TP_Conn_Abort(TxMultiPacket_PTR->DA,3,TxMultiPacket_PTR->PGN);
        TxMultiPacket_PTR->InProgress = 0;
        TxMultiPacket_PTR->TxMsgPtr->TxInProgress = 0;
        // Don't clear the TXReq in the J1939 MailBox. The message was not sent completely
      } 
    }
  }
}
#endif //SUPPORT_MULTIPACKET

#if SUPPORT_MULTIPACKET
/***********************************************************************************************
* FUNCTION   : Dll_MultiPacket_Transmit
*
* DESCRIPTION: This function check all the possible Tx Multi Packet Session
*              If one of the session is active and DataTransfert is needed
*              PGN_Tx_DataTransfert is called.
*
* INPUTS     : None, the function works only with the structure MultiPacket
*
* OUTPUTS    : None
*           
* COMMENTS   : 
*
***********************************************************************************************/
static void Dll_MultiPacket_Transmit(void)
{
  unsigned char i;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;

  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &TxMultiPacket[i];

    if ( (TxMultiPacket_PTR->InProgress == 1) &&   // If TxMulti Packet session is in progress
         (TxMultiPacket_PTR->NextTxTimer == 0) )
        (void) PGN_Tx_DataTransfert(TxMultiPacket_PTR);
  }
}
#endif //SUPPORT_MULTIPACKET

#if SUPPORT_MULTIPACKET
/***********************************************************************************************
* FUNCTION   : Dll_MultiPacket_UpdCtr
*
* DESCRIPTION: This function check all the possible Multi Packet Session (Tx or Rx)
*              If an active Multi-Packet session is finded, the Time-Out and Transmit
*              counter are decrement. 
*
* INPUTS     : None, the function works only with the structure MultiPacket
*
* OUTPUTS    : None
*           
* COMMENTS   : 
*
***********************************************************************************************/
static void Dll_MultiPacket_UpdCtr(void)
{
  unsigned char i;
  j1939_RxMultiPacket_State_Variables_Struct_Type *RxMultiPacket_PTR;
  j1939_TxMultiPacket_State_Variables_Struct_Type *TxMultiPacket_PTR;

  for (i=0;i<J1939_NUMBER_RX_MULTI_PACKET_SESSION;i++)
  { 
    RxMultiPacket_PTR = &RxMultiPacket[i];

    if (RxMultiPacket_PTR->InProgress == 1)    // If Multi Packet session is in progress
    {
      if(RxMultiPacket_PTR->DelayTimer != 0)   // Decrement the Time-Out Counter untill 0 is reach
         RxMultiPacket_PTR->DelayTimer--;
    }
  }

  for (i=0;i<J1939_NUMBER_TX_MULTI_PACKET_SESSION;i++)
  { 
    TxMultiPacket_PTR = &TxMultiPacket[i];

    if (TxMultiPacket_PTR->InProgress == 1)    // If Multi Packet session is in progress
    {
       if(TxMultiPacket_PTR->DelayTimer!=0)    // Decrement the Time-Out Counter untill 0 is reach
          TxMultiPacket_PTR->DelayTimer--;
       if(TxMultiPacket_PTR->NextTxTimer!=0)   // Decrement the Transmit Counter untill 0 is reach
          TxMultiPacket_PTR->NextTxTimer--;
    }
  }
}
#endif //SUPPORT_MULTIPACKET

#if SUPPORT_FAST_PACKET_RX
/***********************************************************************************************
  Function   : Dll_RxFastPacketMsg

  Description: This function needs to be called when a fast packet message is received
               All the reception process is process in this function.
               
 OUTPUTS    : TRUE  - The Message was process and updated
                                             
              FALSE - The Message was not process

***********************************************************************************************/
static unsigned char Dll_RxFastPacketMsg(can_Msg_Struct *Msg_Can, j1939_RxMsg_Struct *Msg_j1939, UINT1 SA, UINT1 DA )
{
   unsigned char i;
   unsigned char FrameCounter;
   unsigned char SequenceCounter;
   unsigned char TotalMsgSize;
   unsigned char ReturnStatus = 0;
   j1939_RxFastPacket_State_Variables_Struct_Type *RxFastPacketPtr;
   
   RxFastPacketPtr = &RxFastPacket[0];

   FrameCounter = Msg_Can->Data[0] & 0b00011111;
   SequenceCounter =  (Msg_Can->Data[0] & 0b11100000) >> 5;
   
   // Find the Rx Fast Packet session corresponding to this PGN
   for (i=0;i<J1939_NUMBER_RX_FAST_PACKET_SESSION;i++)
   {
      if(RxFastPacketPtr->RxMsgPtr == Msg_j1939)
         break;
      else
         RxFastPacketPtr++;
   }
   
  // If the Reception is not already active and First frame of a fast packet message
  // Also check if the J1939 Mailbox can be Overwrite
  // Also check if the Originator Filter is used
   if( ((Msg_j1939->Rx_InProgress == 0) && (FrameCounter == 0)) && 
       ((Msg_j1939->No_OverWrite == 0) || ( (Msg_j1939->No_OverWrite == 1) && (Msg_j1939->Full == 0) ) ) &&
       ((Msg_j1939->Org_Filter == 0xFF) || (Msg_j1939->Org_Filter == SA)) )
   {
      TotalMsgSize = Msg_Can->Data[1];
      if(Msg_j1939->BufferSize >= TotalMsgSize)
      {
         Msg_j1939->MsgSize = TotalMsgSize;
         Msg_j1939->Org_Addr = SA;
         Msg_j1939->Dst_Addr = DA; 

         if(TotalMsgSize <= 6)  
         {
            // The message contains only one Frame, update J1939 Rx Mailbox right now
            Msg_j1939->Full = 1;
            Msg_j1939->Ageing_Ctr = 0;
            for(i=0;i<TotalMsgSize;i++)
               Msg_j1939->Data[i] = Msg_Can->Data[i+2];
         }
         else
         {
            // The message contains more than one Frame
            Msg_j1939->Full = 0;
            Msg_j1939->Rx_InProgress = 1;
            RxFastPacketPtr->InProgress = 1;
            RxFastPacketPtr->Frame_Counter = FrameCounter;   
            RxFastPacketPtr->SeqCounter = SequenceCounter;
            RxFastPacketPtr->DelayTimer = FAST_PACKET_RX_TIME_OUT;    // Time-Out will occurs if the next frame is not receive in 750 mS
            RxFastPacketPtr->Num_Byte_Rx = 6;
            RxFastPacketPtr->Byte_Pgm_Ptr = &Msg_j1939->Data[0];
            for(i=0;i<6;i++)
            {
               *RxFastPacketPtr->Byte_Pgm_Ptr = Msg_Can->Data[i+2];
               RxFastPacketPtr->Byte_Pgm_Ptr++;
            }
         }
         ReturnStatus = 1;
      }
   }

   // If the reception is already active and it's not the first Frame
   else if((Msg_j1939->Rx_InProgress == 1) && (FrameCounter != 0))  
   {
      if((RxFastPacketPtr->SeqCounter == SequenceCounter) && (SA == Msg_j1939->Org_Addr))
      {
         if(FrameCounter == (RxFastPacketPtr->Frame_Counter+1))
         {
            RxFastPacketPtr->Frame_Counter++;
            RxFastPacketPtr->DelayTimer = FAST_PACKET_RX_TIME_OUT;    // Time-Out will occurs if the next frame is not receive in 750 mS
            for(i=0;((i<7) && (RxFastPacketPtr->Num_Byte_Rx < Msg_j1939->MsgSize));i++)
            {
               *RxFastPacketPtr->Byte_Pgm_Ptr = Msg_Can->Data[i+1];
               RxFastPacketPtr->Byte_Pgm_Ptr++;
               RxFastPacketPtr->Num_Byte_Rx++;
            }
            if((RxFastPacketPtr->Num_Byte_Rx) == Msg_j1939->MsgSize)
            {
               Msg_j1939->Rx_InProgress = 0;
               RxFastPacketPtr->InProgress = 0;
               Msg_j1939->Ageing_Ctr = 0;
               Msg_j1939->Full = 1;
            }
         }
         else // The sequence of message is not respected, discard the session
         {
            Msg_j1939->Rx_InProgress = 0;
            RxFastPacketPtr->InProgress = 0;
         }
         ReturnStatus = 1;
         
      }
   }
   return(ReturnStatus);
}
#endif //SUPPORT_FAST_PACKET_RX

#if SUPPORT_FAST_PACKET_TX
/***********************************************************************************************
  Function   : Dll_TxFastPacket_Init

  Description: This function needs to be called when the transmission of a fast packet is
               started.
               
 OUTPUTS    : TRUE  - The Message was process and updated
                                             
              FALSE - The Message was not process

***********************************************************************************************/
static unsigned char Dll_TxFastPacket_Init(j1939_TxMsg_Struct *Msg_j1939)
{
   unsigned char i;
   j1939_TxFastPacket_State_Variables_Struct_Type *TxFastPacketPtr;
   j1939_SingleFrame_Message_Struct_Type message;
   unsigned char Data[8];
 
   TxFastPacketPtr = &TxFastPacket[0];

   // Find the Tx Fast Packet session corresponding to this PGN
   for (i=0;i<J1939_NUMBER_TX_FAST_PACKET_SESSION;i++)
   {
      if(TxFastPacketPtr->TxMsgPtr == Msg_j1939)
         break;
      else
         TxFastPacketPtr++;
   }

   TxFastPacketPtr->TxFrame_Counter = 0;
   TxFastPacketPtr->NextTxTimer = FAST_PACKET_TX_INTER_FRAME;
   TxFastPacketPtr->Num_Byte_Tx = 0;
   TxFastPacketPtr->Byte_Tx_Ptr = Msg_j1939->Data;          

   Data[0] = ((TxFastPacketPtr->SeqCounter & 0b00000111) << 5);
   Data[1] = (unsigned char) Msg_j1939->DataSize;
   for(i=0;(i<6) && (i<Msg_j1939->DataSize);i++)
   {
      Data[i+2] = *TxFastPacketPtr->Byte_Tx_Ptr;
      TxFastPacketPtr->Byte_Tx_Ptr++;
      TxFastPacketPtr->Num_Byte_Tx++;
   }

   message.BytePtr  = &Data[0];
   message.ByteCntr = TxFastPacketPtr->Num_Byte_Tx + 2;
   // J1939 29-bit identifier fields
   message.MsgId.Identifier                = Msg_j1939->PGN << 8;
   message.MsgId.IdField.BitField.Priority = (Msg_j1939->Priority & 0x07);
   message.MsgId.IdField.SourceAddress     = MyAddress;
   if(message.MsgId.IdField.PduFormat < 240)
      message.MsgId.IdField.PduSpecific = Msg_j1939->Dest_Addr;

   // Valid PGN frame transmission state trap...
   if (Dll_TxSingleFrameMessage(&message) == TRUE)
   {
      if(Msg_j1939->DataSize > 6)
      {
         TxFastPacketPtr->InProgress = 1;
         Msg_j1939->TxInProgress = 1;
      }
      return TRUE;
   }
   return FALSE;  
}
#endif //SUPPORT_FAST_PACKET_TX

#if SUPPORT_FAST_PACKET_TX
/***********************************************************************************************
* FUNCTION   : Dll_TxFastPacket
*
* DESCRIPTION: This function go through all the Rx Fast Packet session.
*              The function transmit the scheduled fast packet if needed
*
* INPUTS     : None, the function works only with the structure FastPacket
*
* OUTPUTS    : None
*           
* COMMENTS   : 
*
***********************************************************************************************/
static void Dll_TxFastPacket(void)
{
   unsigned char i;
   j1939_SingleFrame_Message_Struct_Type message;
   j1939_TxMsg_Struct *Msg_j1939;
   unsigned char Data[8];

   j1939_TxFastPacket_State_Variables_Struct_Type *TxFastPacketPtr;

   TxFastPacketPtr = &TxFastPacket[0];
   
   // Go through all Fast-Packet session and transmit packet if needed
   for (i=0;i<J1939_NUMBER_TX_FAST_PACKET_SESSION;i++)
   {
      if((TxFastPacketPtr->InProgress == 1) && (TxFastPacketPtr->NextTxTimer == 0))
      {
         if((UINT1)can_Tx_Queue_Free_Space() > J1939_RESERVED_QUEUE_SPACE)
         {
            Msg_j1939 = TxFastPacketPtr->TxMsgPtr;

            TxFastPacketPtr->TxFrame_Counter++;
            TxFastPacketPtr->NextTxTimer = FAST_PACKET_TX_INTER_FRAME;

            Data[0] = ((TxFastPacketPtr->SeqCounter & 0b00000111) << 5) | (TxFastPacketPtr->TxFrame_Counter & 0b00011111);
            for(i=0;(i<7) && (TxFastPacketPtr->Num_Byte_Tx < Msg_j1939->DataSize);i++)
            {
               Data[i+1] = *TxFastPacketPtr->Byte_Tx_Ptr;
               TxFastPacketPtr->Byte_Tx_Ptr++;
               TxFastPacketPtr->Num_Byte_Tx++;
            }
            for(;i<7;i++)
               Data[i+1] = 0xFF;

             message.BytePtr  = &Data[0];  
             message.ByteCntr = i + 1;  
             // J1939 29-bit identifier fields  
             message.MsgId.Identifier                = Msg_j1939->PGN << 8;  
             message.MsgId.IdField.BitField.Priority = (Msg_j1939->Priority & 0x07);  
             message.MsgId.IdField.SourceAddress     = MyAddress;  
             if(message.MsgId.IdField.PduFormat < 240)  
                message.MsgId.IdField.PduSpecific = Msg_j1939->Dest_Addr;  

            // Valid PGN frame transmission state trap...  
            if (Dll_TxSingleFrameMessage(&message) == TRUE)  
            {  
               if(TxFastPacketPtr->Num_Byte_Tx == Msg_j1939->DataSize)  
               {  
                  TxFastPacketPtr->InProgress = 0;  
                  TxFastPacketPtr->SeqCounter = (TxFastPacketPtr->SeqCounter + 1) & 0b00000111;  
                  Msg_j1939->TxInProgress = 0;  
                  Msg_j1939->TxReq = 0;  
               }  
            }  
         }
      }
      TxFastPacketPtr++;
   }
}
#endif //SUPPORT_FAST_PACKET_TX

//***********************************************************************************************
// FUNCTION   : Dll_FastPacket_TimeOut_10mS
//
// DESCRIPTION: This function go through all the Fast Packet session.
//              For the RX Fast Packet the function decrement the time-out counter
//              and check if the session needs to be aborted
//              For the TX FastPacket, the next transmission scheduler timer is
//              decrement if needed 
//  
//
// INPUTS     : None, the function works only with the structure FastPacket
//
// OUTPUTS    : None
//           
// COMMENTS   : 
//*
//**********************************************************************************************
static void Dll_FastPacket_TimeOut_10mS(void)
{
   unsigned char i;

   j1939_RxFastPacket_State_Variables_Struct_Type *RxFastPacketPtr;
   j1939_TxFastPacket_State_Variables_Struct_Type *TxFastPacketPtr;

   #if SUPPORT_FAST_PACKET_RX   
   RxFastPacketPtr = &RxFastPacket[0];
   for (i=0;i<J1939_NUMBER_RX_FAST_PACKET_SESSION;i++)
   {
      if(RxFastPacketPtr->InProgress == 1)
      {
         if(RxFastPacketPtr->DelayTimer != 0)
            RxFastPacketPtr->DelayTimer--;
         if(RxFastPacketPtr->DelayTimer == 0)  // Check if the session needs to be aborted
         {
            RxFastPacketPtr->InProgress = 0;
            RxFastPacketPtr->RxMsgPtr->Rx_InProgress = 0;
         }
      }
      RxFastPacketPtr++;
   }
   #endif //SUPPORT_FAST_PACKET_RX   

   #if SUPPORT_FAST_PACKET_TX   
   TxFastPacketPtr = &TxFastPacket[0];
   // Update the transmit Fast-Packet timer and transmit packet if needed
   for (i=0;i<J1939_NUMBER_TX_FAST_PACKET_SESSION;i++)
   {
      if((TxFastPacketPtr->InProgress == 1) && (TxFastPacketPtr->NextTxTimer > 0))
         TxFastPacketPtr->NextTxTimer--;

      TxFastPacketPtr++;
   }
   #endif //SUPPORT_FAST_PACKET_TX   

}




/**********************************************************************************************/
/******************************                                  ******************************/
/******************************    J1939-71 APPLICATION LAYER    ******************************/
/******************************                                  ******************************/
/**********************************************************************************************/


/***********************************************************************************************
  Function   : j1939_TxManagerTask
  Description: Send all the message (Periodic or Single ) to TX queue. 
               The message are specified by the High application level in the J1939_TxMsg_Struct.

***********************************************************************************************/
static void j1939_TxManagerTask(void)
{
   typedef union{              
     unsigned long  L;     
     struct {
       unsigned char B0;
       unsigned char B1;
       unsigned char B2;
       unsigned char B3;
     } Byte;
   } RequestPGN_Struct;  // Bit Mapping is OK for Atmel         

   RequestPGN_Struct RequestPgnNumber;  // Used only when the Mailbox contains Request PGN Msg

  j1939_SingleFrame_Message_Struct_Type message;
  j1939_TxMsg_Struct   *Msg_j1939;
  unsigned char i;
 
  i = j1939_Parameter->TxMailboxSize;
  Msg_j1939 = j1939_Parameter->TxMailbox;     // Local Pointer to the Tx Mailbox

  // If the module does not have a fixed address it cannot send
  // normal messages until it has successfully claimed an address
  if (AddrClaim.AddressClaimed == TRUE)      
  {
    while (i > 0)   // Check the complete J1939 TX Mailbox and send the message if needed
    {
      // Check how many element are free in the Queue CAN. Two element in the Queue CAN are reserved
      // for the communication Hand Shaking and Process. Almost two element in the Queue CAN
      // needs to be reserved for this. The Cast is used to compensated for a Bug with the Compiler
      if ( (UINT1)can_Tx_Queue_Free_Space() <= J1939_RESERVED_QUEUE_SPACE )
        break;    // The Queue is too loaded to add new element => STOP

      if( (Msg_j1939->TxReq ==  J1939_REQUEST) &&   // If this Message needs to be transmitted
          (Msg_j1939->TxInProgress == 0) )
      {
        #if SUPPORT_FAST_PACKET_TX   
        if(Msg_j1939->Pgn_Type == 1)       // Check for Fast Packet Message
           (void) Dll_TxFastPacket_Init(Msg_j1939);
        #else
        if(0);        
        #endif //SUPPORT_FAST_PACKET_TX   

        #if SUPPORT_MULTIPACKET
        else if(Msg_j1939->DataSize > 8)        // Check the Size of the Message
        {
          // This needs the Multi Packet Protocol
          if( Dll_TxMultiPacket_Init(Msg_j1939) == TRUE)
            Msg_j1939->TxInProgress = 1;
        }
        #endif //SUPPORT_MULTIPACKET

        else if(Msg_j1939->PGN == J1939_REQUEST_PGN)
        {
           RequestPgnNumber.Byte.B3 = 0;
           RequestPgnNumber.Byte.B2 = *(Msg_j1939->Data+2);
           RequestPgnNumber.Byte.B1 = *(Msg_j1939->Data+1);
           RequestPgnNumber.Byte.B0 = *(Msg_j1939->Data);

           if(PGN_Tx_ISORequest(Msg_j1939->Dest_Addr, MyAddress, RequestPgnNumber.L) == TRUE)
             Msg_j1939->TxReq = J1939_IDLE;   // Success PGN frame transmission, Clear Transmit Flag
        }
        else
        {
          // The message size is lower than 8 Bytes, Simply transmit it as a single MSG      
          message.BytePtr  = Msg_j1939->Data;
          message.ByteCntr = (UINT1) Msg_j1939->DataSize;
          // J1939 29-bit identifier fields
          message.MsgId.Identifier                = Msg_j1939->PGN << 8;
          message.MsgId.IdField.BitField.Priority = (Msg_j1939->Priority & 0x07);
          message.MsgId.IdField.SourceAddress     = MyAddress;
          if(message.MsgId.IdField.PduFormat < 240)
            message.MsgId.IdField.PduSpecific = Msg_j1939->Dest_Addr;

          // Valid PGN frame transmission state trap...
          if (Dll_TxSingleFrameMessage(&message) == TRUE)
          {
            Msg_j1939->TxReq = J1939_IDLE;   // Success PGN frame transmission, Clear Transmit Flag
          }
          else
            break;  // The Mailbox is Full, stop to Fill it
        }
      }
      i--;
      Msg_j1939++; 
    }
  }
}

/***********************************************************************************************
  Function   : j1939_RxManagerTask
  Description: Read all messages received in the RX queue.
***********************************************************************************************/

static void j1939_RxManagerTask(void)
{
    can_Msg_Struct     *message;    // Message Queue Element Pointer
    j1939_Id_Struct_Type     id;    // CAN Message id 
    unsigned long           pgn;        
    unsigned char           dsr;
    static unsigned char    Last_Call_Status = 0;  // 0 = At last call of the fct the queue can was empty completely
                                                   // 1 = At last call the process was stop before ending.
                                                   //     The can message was keep in the can queue to let the 
                                                   //     application process the J1939 Mailbox. The message is keep
                                                   //     in the queue for only one thread. 
    #if SUPPORT_MULTIPACKET
    unsigned char Control_Byte;  // Used if the PGN received is Connection Management (60416)
    #endif
    
    // Scan the Received Data Queue
    for (dsr=0; dsr<(j1939_Parameter->Queue->Rx.Size); dsr++)
    {
        message = can_ReadRxQueue();
        if (message == NULL)
            break;              // No more message to read

        // Adddress claim "NoMessage" variable is always TRUE default message reception...
        //  ...except for PGN address claim or ISO request for PGN address claim messages
        //  ...then this flag set be FALSE (see below)
        AddrClaim.NoMessage = TRUE;

        id.Identifier = message->Id;
        pgn = j1939_ExtractPGN(id.Identifier);


        /******************* J1939 PDU1 Format - specific messages ****************************/
        // Specific network device address arbitration trap...
        if (   (id.IdField.PduFormat <= J1939_MAX_PF_PDU1FORMAT)
               && ( (id.IdField.PduSpecific == MyAddress) 
                    || (id.IdField.PduSpecific == J1939_GLOBAL_ADDRESS) ) )
        {
            // Remove destination address from specific PGN number 
            pgn ^= id.IdField.PduSpecific;

            // All messages can be received if device is able to claim an address
            if (AddrClaim.ILose == FALSE)
            {
                // Supported addressable destination PGN numbers arbitration trap...
                switch (pgn)
                {
                    case J1939_REQUEST_PGN:
                        PGN_Rx_ISORequest(message);
                        break;
                    //case J1939_ACKNOWLEDGMENT_PGN:
                    //    PGN_Rx_ISOAcknowledgment(message);
                    //    break;
                    case J1939_ADDRESS_CLAIM_PGN:
                        PGN_Rx_ISOAddressClaim(message);
                        if(DLL_Fill_j1939RxMailbox(message,pgn,id.IdField.SourceAddress,id.IdField.PduSpecific) == 0)
                           message = NULL;  // This CAN message needs to be written in the J1939 Mailbox but there is no
                                            // free space to do it. Stop to check the CAN queue untill next call to let the application
                                            // update the J1939 Mailbox. 
                        break;
                    #if SUPPORT_MULTIPACKET
                    case J1939_MULTI_PACKET_TPCM_PGN:
                        Control_Byte = message->Data[0];      // Determine which type of Connection Message
                        if( (Control_Byte == J1939_MULTI_PACKET_TPCMRTS) &&
                                 (id.IdField.PduSpecific == MyAddress) )
                          PGN_Rx_TP_CM_RTS(message,id.IdField.SourceAddress);
                        else if( (Control_Byte == J1939_MULTI_PACKET_TPCONNABORT) &&
                                 (id.IdField.PduSpecific == MyAddress) )
                          PGN_Rx_TP_Conn_Abort(message,id.IdField.SourceAddress);
                        else if(Control_Byte == J1939_MULTI_PACKET_TPCMBAM)
                          PGN_Rx_TP_CM_BAM(message,id.IdField.SourceAddress);                         
                        else if( (Control_Byte == J1939_MULTI_PACKET_TPCMCTS) &&
                                 (id.IdField.PduSpecific == MyAddress) )
                          PGN_Rx_TP_CM_CTS(message,id.IdField.SourceAddress);
                        else if( (Control_Byte == J1939_MULTI_PACKET_ENOFMSGACK) &&
                                 (id.IdField.PduSpecific == MyAddress) )
                          PGN_Rx_TP_CM_EndOfMsgACK(message,id.IdField.SourceAddress); 
                        break;
                    case J1939_MULTI_PACKET_TPDT_PGN:
                        PGN_Rx_DataTransfert(message,id.IdField.SourceAddress,id.IdField.PduSpecific);
                        break;
                    #endif //SUPPORT_MULTIPACKET
                    default:
                        // Go check if this message is supported and process it if needed
                        // In any case, the module doesn't answer 
                        if(DLL_Fill_j1939RxMailbox(message,pgn,id.IdField.SourceAddress,id.IdField.PduSpecific) == 0)
                           message = NULL;  // This CAN message needs to be written in the J1939 Mailbox but there is no
                                            // free space to do it. Stop to check the CAN queue untill next call to let the application
                                            // update the J1939 Mailbox. 
                        break;
                }
            }
            else if (pgn == J1939_REQUEST_PGN)
            {
                // Only this message can be received if device cannot claim an address
                PGN_Rx_ISORequest(message);
            }
        }

        //------------------- J1939 PDU2 Format - global messages ------------------------------
        // All messages can be received if device is able to claim an address
        else if( (AddrClaim.AddressClaimed == TRUE) && (id.IdField.PduFormat > J1939_MAX_PF_PDU1FORMAT) )
        {
            // Go check if this message is supported and process it if needed 
            if(DLL_Fill_j1939RxMailbox(message,pgn,id.IdField.SourceAddress,0xFF) == 0)
               message = NULL;  // This CAN message needs to be written in the J1939 Mailbox but there is no
                                // free space to do it. Stop to check the CAN queue untill next call to let the application
                                // update the J1939 Mailbox. 
        }
        if(message != NULL)
        {
           can_PullRxQueue();
           Last_Call_Status = 0;
        }
        else if(Last_Call_Status == 1)
        {
           // This can message was not process by the application fast enough.
           // To avoid can queue overrun, the can message is delete.
           can_PullRxQueue();
           Last_Call_Status = 0;
        }
        else
        {
           // Stop to process the queue can. The can message is leave in the queue CAN for 1 tread.
           // If at the next call of the function the J1939 mailbox is still full, the can message will
           // be flushed. If not the message is written in the queue can as usual.
           Last_Call_Status = 1;
           break;
        }
    }
}


/***********************************************************************************************
  Function   : j1939_ExtractPGN
  Description: Extract the J1939 parameter group number (PGN) from the 29-bit CAN identifier.
                                         D P P P P P P P P P P P P P P P P S S S S S S S S
               CAN ID    :       P P P R P F F F F F F F F S S S S S S S S A A A A A A A A
                                         D P P P P P P P P P P P P P P P P
               J1939 PGN : 0 0 0 0 0 0 R P F F F F F F F F S S S S S S S S
***********************************************************************************************/

static UINT4 j1939_ExtractPGN(UINT4 Id)
{
    return ((Id >> 8) & 0x0003FFFF);
}



/***********************************************************************************************
  Function   : DLL_Fill_j1939RxMailbox
  Description: The function check if the received PGN is supported by the application.
               If the J1939 Rx Mailbox is big enough, the message is transfered

 OUTPUTS    : TRUE  - The Message is not defined in the Mailbox OR was update correctly
                                             
              FALSE - The Message is define in the MailBox but can not be update since
                      all the Buffer for this PGN are already FULL

***********************************************************************************************/
static UINT1 DLL_Fill_j1939RxMailbox(can_Msg_Struct *Msg_Can, unsigned long pgn, UINT1 SA, UINT1 DA )
{
    j1939_RxMsg_Struct       *Msg_j1939;
    unsigned char i,j;
    unsigned char Status = 1;  
    
    Msg_j1939 = j1939_Parameter->RxMailbox;     

    for(i=0; i<j1939_Parameter->RxMailboxSize; i++)
    {
        if(pgn == (Msg_j1939->PGN & 0x0003FFFF))
        {
           // This PGN is defined in the MailBox
           Status = 0;

           if(Msg_j1939->Pgn_Type == 1)
           {
              #if SUPPORT_FAST_PACKET_RX   
              Status = 1;
              if(Dll_RxFastPacketMsg(Msg_Can, Msg_j1939,SA, DA))
                 break; 
              #endif //SUPPORT_FAST_PACKET_RX                 
           }

           else if( (Msg_j1939->Rx_InProgress == 0) &&
                    ( (Msg_j1939->No_OverWrite == 0) || 
                      ( (Msg_j1939->No_OverWrite == 1) && (Msg_j1939->Full == 0) ) ) )
           {
              // Verify the Data Originator Filter
              if( (Msg_j1939->Org_Filter == 0xFF) || (Msg_j1939->Org_Filter == SA) )
              {                          
                 if(Msg_j1939->BufferSize >= Msg_Can->Dlc) // Check if the reception buffer is big enough
                 {
                     // The Message can be receive
                     Status = 1;
    
                     Msg_j1939->MsgSize = Msg_Can->Dlc;         
                     for(j=0; j<Msg_Can->Dlc; j++)                // Transfer the Data
                         Msg_j1939->Data[j] = Msg_Can->Data[j];
                     Msg_j1939->Full = J1939_FULL;                // This buffer is now Full
                     Msg_j1939->Dst_Addr = DA;
                     Msg_j1939->Org_Addr = SA;
                     Msg_j1939->Ageing_Ctr = 0;                   // Reset the Ageing Counter
                 }
                 break;  // Stop the Loop For, the message buffer was find
              }
           }
        }
        Msg_j1939++;
    }
    return(Status);           
}

#if SUPPORT_MULTIPACKET
/***********************************************************************************************
  Function   : DLL_Check_j1939RxMailbox_MultiPacket
  Description: The function check if the received PGN is supported by the application.

  INPUTS :     pgn      - The PGN Number that needs to be checked


  OUTPUTS    : If the PGN is supported 
                        AND
               If The reception of the PGN is not in progresss 
                        AND
               IF (The PGN can Be overwrite OR The buffer is Empty)
               
               The function return a pointer to Mailbox element
               and if not it return NULL
           
  COMMENTS   : 
***********************************************************************************************/
static j1939_RxMsg_Struct* DLL_Check_j1939RxMailbox_MultiPacket(unsigned long pgn, unsigned char SA)
{
  j1939_RxMsg_Struct       *Msg_j1939;
  unsigned char i;
    
  Msg_j1939 = j1939_Parameter->RxMailbox;     

  for(i=0; i<j1939_Parameter->RxMailboxSize; i++)
  {
    if( pgn == Msg_j1939->PGN )
    {
        if(Msg_j1939->Rx_InProgress == 0)
        {
           if( (Msg_j1939->Full != 1) || (Msg_j1939->No_OverWrite != 1) )
           {
             // Verify the Data Originator Filter
             if( (Msg_j1939->Org_Filter == 0xFF) || (Msg_j1939->Org_Filter == SA) )
                return Msg_j1939;            // Stop the Loop For, the message buffer was find
           }
        }
    }
    Msg_j1939++;
  }     
  return NULL;            
}
#endif //SUPPORT_MULTIPACKET
/***********************************************************************************************
  Function   : DLL_Upd_AgeingCtr_j1939RxMailbox
  Description: Increase the Ageing Counter of all the element in the j1939RX Mailbox
               The Ageing Counter is stack at 255.
***********************************************************************************************/
static void DLL_Upd_AgeingCtr_j1939RxMailbox(void)
{
  j1939_RxMsg_Struct       *Msg_j1939;
  unsigned char i;  
    
  Msg_j1939 = j1939_Parameter->RxMailbox;     

  for(i=0; i<j1939_Parameter->RxMailboxSize; i++)
  {
    if(Msg_j1939->Ageing_Ctr < 255)
      Msg_j1939->Ageing_Ctr++;  
    Msg_j1939++;
  }           
}

/***********************************************************************************************
  Function   : DLL_Update_TxCounter
  Description: This function is used to update the different counter used for the periodic
               transmission of PGN. This counter are a part of the j1939_TxMsg structure              

***********************************************************************************************/
static void DLL_Update_TxCounter(void)
{
  j1939_TxMsg_Struct   *Msg_j1939;
  unsigned char i;
  
  i = j1939_Parameter->TxMailboxSize;
  Msg_j1939 = j1939_Parameter->TxMailbox;      // Local Pointer to the Tx Mailbox

  while (i > 0)
  {
    if(Msg_j1939->Period == J1939_PERIODIC)    // Check if it is a periodic message
    {
      if(Msg_j1939->Cntr != 0)                 // Decrement Counter if it is higher than 0
        Msg_j1939->Cntr--;
      if(Msg_j1939->Cntr == 0)
      {
        Msg_j1939->TxReq = J1939_REQUEST;      // IF counter is elapsed (zero), set transmit request
        Msg_j1939->Cntr = Msg_j1939->CntrSet;  // flag and reload counter set value
      }     
    }
    i--;
    Msg_j1939++;
  }
}

/***********************************************************************************************
* FUNCTION   : Dll_SetTx_j1939TxMailbox
*
* DESCRIPTION : This function check the J1939 Tx Mailbox to see if the requested PGN
*               is supported. If yes, a single transmission Flag is set. 
*
* INPUTS     : PGN Number
*
* OUTPUTS    : TRUE  - The PGN is suppported and the transmission will occurs
*              FALSE - PGN is not Supported
*           
* COMMENTS   : 
***********************************************************************************************/

static UINT1 Dll_SetTx_j1939TxMailbox(UINT4 PGN_Number)
{
  j1939_TxMsg_Struct   *Msg_j1939;
  unsigned char i;
  unsigned char j = FALSE;
  
  i = j1939_Parameter->TxMailboxSize;
  Msg_j1939 = j1939_Parameter->TxMailbox;     // Local Pointer to the Tx Mailbox

  while (i > 0)   // Check the complete J1939 TX Mailbox
  {
      // Note: The next IF contains a AND. This is used to remove
      //       the Priority field from the identifier
      if(PGN_Number == (Msg_j1939->PGN & 0x0003FFFF))   
      {
        Msg_j1939->TxReq = J1939_REQUEST;           // The PGN is supported, Set the Request
        j = TRUE;                                   // Set the Return Flag to TRUE
      }
      i--;
      Msg_j1939++;     
  }
  return (j);   // Return TRUE or FALSE depending if the PGN will be tramsmitted or not
}

/**** END ****/
