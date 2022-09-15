/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file can.h
       \brief CAN controller data link functions for PIC18F4680

<!-- DESCRIPTION: -->
       CAN controller data link functions for PIC18F4680
       Author: Martial Bilodeau
       Date: April 26, 2007

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/can.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.2  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.1 Thu Jul 5 20:10:12 2007 
         Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

   \endif
*/
/******************************************************************************/

#ifndef _CAN_H
#define _CAN_H

/*************************************** Public Types *****************************************/

typedef struct {
  unsigned       BusOff :1;   // 
  unsigned char  DiscardMsgCntr;
} can_Ctrl_Status_def;        // CAN Controler Status

typedef struct {
  unsigned       Dlc :4;          // Data length count [0-8]
  unsigned       Ide :1;          // Extended ID bit [0=11bits 1=29bits]
  unsigned       Srr :1;          // Fixed recessive bit for extended format
  unsigned       Rtr :1;          // Remote transmission request [0=DataFrame 1=RemoteFrame]
  unsigned long  Id;              // Right justificated standard or extended ID
  unsigned char  Data[8];         // Frame data [up to 8 bytes]
} can_Msg_Struct;                 // CAN Message Queue Element Structure

typedef struct {
  struct {
    unsigned char  OvrCntr;       // Rx Queue overrun roll-over counter
    unsigned char  ReadIdx;       // Rx Queue read index
    unsigned char  WriteIdx;      // Rx Queue write index
    unsigned char  Size;          // Rx Queue element size [0-255]
    can_Msg_Struct *Buffer;       // Rx Queue buffer pointer (first indexed message)
  } Rx;                           // Rx Queue structure
  struct {
    unsigned char  OvrCntr;       // Tx Queue overrun roll-over counter
    unsigned char  ReadIdx;       // Tx Queue read index
    unsigned char  WriteIdx;      // Tx Queue write index
    unsigned char  Size;          // Tx Queue element size [0-255]
    can_Msg_Struct *Buffer;       // Tx Queue buffer pointer (first indexed message)
  } Tx;                           // Tx Queue structure
} can_Queue_Struct;               // CAN Messages Tx/Rx Queue Structure


/************************************* Public Variables ***************************************/

extern can_Ctrl_Status_def can_Ctrl_Status;

/************************************* Public Functions ***************************************/

void can_InitHardware(void);
void can_InitQueue(can_Queue_Struct *Queue);
void can_process(void);
void CANIT_handler(void);

void can_PullRxQueue(void);
void can_PullTxQueue(void);
void can_PushRxQueue(void);
void can_PushTxQueue(void);

can_Msg_Struct *can_ReadRxQueue(void);
can_Msg_Struct *can_ReadTxQueue(void);
can_Msg_Struct *can_WriteRxQueue(void);
can_Msg_Struct *can_WriteTxQueue(void);

unsigned char can_Rx_Queue_Free_Space(void);
unsigned char can_Tx_Queue_Free_Space(void);


#endif  /* _CAN_H */
