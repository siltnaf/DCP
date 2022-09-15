/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file can.c
       \brief CAN controller data link functions for PIC18F4680

<!-- DESCRIPTION: -->
       Author: Martial Bilodeau
       Date: April 26, 2007

       \if ShowHist
       COPYRIGHT (c) TELEFLEX MEGATECH ELECTRO INC. 2005
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Megatech Electro Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/can.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.7  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.6 Tue May 6 18:47:34 2008 
         Added hardware level filter to CAN operation to receive only messages with 29 bit identifier.

         Revision 1.5 Fri Nov 30 18:52:07 2007 
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

         Revision 1.4 Mon Sep 17 18:39:17 2007 
         DPF related modifications

         Revision 1.3 Mon Aug 27 22:19:08 2007 
         CAN communication working.

         Revision 1.2 Tue Jul 10 22:35:33 2007 
         Updated CAN BRGCONx timing for 40MHz clock.

         Revision 1.1 Thu Jul 5 20:10:12 2007 
         Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

       \endif
*/
/******************************************************************************/

//?#include <pic18.h>
#include "can.h"
#include "std.h"
#include "canx.h"				//? Add

/*************************************** Private Definition ************************************/

/******************************* Private Function Functions ***********************************/

/******************************* Private Variables ********************************************/
can_Queue_Struct *Queue;  // Pointer to the can Queue that needs to be used

/******************************* Public Variables *********************************************/
can_Ctrl_Status_def can_Ctrl_Status;

/***********************************************************************************************
  FUNCTION   : can_InitHardware
                                                                                              
  DESCRIPTION: Initialise CAN controller uC Hardware
          
            
  COMMENTS   : This function needs to be called at uC initialisation

               Precision about the utilisation of the 15 different MOB
               MOB 0 to 13  => Reception, 29Bit, No Filter
               MOB 14 => Transmission, 29 Bit

***********************************************************************************************/
void can_InitHardware(void)
{
#ifndef MM32F0133C7P		//? Add
  can_Ctrl_Status.DiscardMsgCntr = 0;

  // Request the configuration Mode
  CANCON = 0b10000000;

  // Wait for configuration mode confirmation
  while((CANSTAT & 0b10000000) == 0); 

  // Select CAN Mode 2 (FIFO), FIFO High Water Mark = 4
  ECANCON = 0b10000000;  

// Baud rate triplets for 40MHz Fosc (4x PLL enabled) for 10MHz crystal (DCP2 board)
//    Set Baud Rate to 250Kbps, Tq=200ns, bit time = 20Tq, and Sampling point=60%
//    Bit time = 1+3+8+8=20tq, and sampling pt (1+3+8)/(1+3+8+8)=12/20=60%
//    BRGCON1 = 0b00000000;  0x03  // Synchronized Jump = 1xTq, BaudRate Prescaler = 3
//    BRGCON2 = 0b10111010;  0xBA  // PSeg1 = 8Tq, Propagation Time = 3Tq
//    BRGCON3 = 0b00000111;  0x07  // PSeg2 = 8Tq

// Baud rate triplets for 16 MHz crystal (PICDEM CAN-LIN 2 demo board in Megatech)
//    Set Baud Rate to 250Kbps
//    BRGCON1 = 0b00000001;    // Synchronized Jump = 1xTq, BaudRate Prescaler = 1
//    BRGCON2 = 0b10111010;    // PSeg1 = 8Tq, Propagation Time = 3Tq
//    BRGCON3 = 0b00000011;    // PSeg2 = 4Tq

// Baud rate triplets for 25 MHz crystal (PICDEM CAN-LIN 2 demo board in Richmond)
//
// We set PropSeg to 8, PhaseSeg1 to 8 and PhaseSeg2 to 8 which gives the bit
// time 1+8+8+8=25 tq and the sampling point (1+8+8)/(1+8+8+8) = 68%.
// With prescaler 0, we get 500 kbps.
// Also, we use SJW = 2 tq, SAM=1
//
//           BRGCON1      BRGCON2        BRGCON3        In Hex
// 500 kbps: 01 000000    1 0 111 111    0 0 000 111    40 bf 07
// 250 kbps: 01 000001    1 0 111 111    0 0 000 111    41 bf 07
// 125 kbps: 01 000011    1 0 111 111    0 0 000 111    43 bf 07
//
// Set Baud Rate to 250Kbps
  BRGCON1 = 0x03;
  BRGCON2 = 0x3A;	//0xBA;	// change according to errata SEG2PHTS inverted
  BRGCON3 = 0x07;

  //---- Initialisation of 1 transmit Buffer, !!! No initialisation are needed !!!
  // !!! Only one transmit buffer is used !!!

  // Initialisation of 8 receiving Buffer to create the hardware FIFO
  RXB0CON  = 0b00000000;  // Receive only Valid message, Filter 0
  RXB1CON  = 0b00000001;  // Receive only Valid message, Filter 1

  BSEL0    = 0b00000000;  // All Buffer are configured in receive mode
  B0CON    = 0b00000010;  // Receive only Valid message, Filter 2 
  B1CON    = 0b00000011;  // Receive only Valid message, Filter 3
  B2CON    = 0b00000100;  // Receive only Valid message, Filter 4
  B3CON    = 0b00000101;  // Receive only Valid message, Filter 5
  B4CON    = 0b00000110;  // Receive only Valid message, Filter 6
  B5CON    = 0b00000111;  // Receive only Valid message, Filter 7

  // Initialisation of the Filter (Filter 0 to 7 are used)
  // No Initialisation needed since we accept all messages

  RXFCON0  = 0b11111111;   // Filter 0 to 7 are used
  RXFCON1  = 0b00000000;   // Filter 8 to 15 not used

  RXFBCON0 = 0b00010000;   // Association between Filter and receiving Buffer
  RXFBCON1 = 0b00110010;
  RXFBCON2 = 0b01010100;
  RXFBCON3 = 0b01110110;

  MSEL0 = 0b00000000;      // Mask 0 is used for Filter 0 to 7
  MSEL1 = 0b00000000;
#ifndef CAN_ID_FLTR
  RXM0SIDH = 0b00000000;
  RXM0SIDL = 0b00000000;
  RXM0EIDH = 0b00000000;
  RXM0EIDL = 0b00000000; 
#else
// Set receive acceptance mask 0 (MSEL0 and MSEL1 are set so that all filters use mask 0)

  RXM0SIDH = 0b00000000;
  RXM0SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Mask will use messages selected by EXIDEN bit in RXFnSIDL bit 3
  RXM0EIDH = 0b00000000;
  RXM0EIDL = 0b00000000; 
 
// Set receive acceptance filters 0 to 7 (which are configured by RXFBCON0-3 to correspond to buffers 0-7 in FIFO
// Filters 8 to 15 are not used
  RXF0SIDH = 0b00000000;
  RXF0SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF0EIDH = 0b00000000;
  RXF0EIDL = 0b00000000; 

  RXF1SIDH = 0b00000000;
  RXF1SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF1EIDH = 0b00000000;
  RXF1EIDL = 0b00000000; 

  RXF2SIDH = 0b00000000;
  RXF2SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF2EIDH = 0b00000000;
  RXF2EIDL = 0b00000000; 

  RXF3SIDH = 0b00000000;
  RXF3SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF3EIDH = 0b00000000;
  RXF3EIDL = 0b00000000; 

  RXF4SIDH = 0b00000000;
  RXF4SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF4EIDH = 0b00000000;
  RXF4EIDL = 0b00000000; 

  RXF5SIDH = 0b00000000;
  RXF5SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF5EIDH = 0b00000000;
  RXF5EIDL = 0b00000000; 

  RXF6SIDH = 0b00000000;
  RXF6SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF6EIDH = 0b00000000;
  RXF6EIDL = 0b00000000; 

  RXF7SIDH = 0b00000000;
  RXF7SIDL = 0b00001000;   // Set EXIDEN bit to 1 = Filter only accepts extended ID messages
  RXF7EIDH = 0b00000000;
  RXF7EIDL = 0b00000000; 
#endif // CAN_ID_FLTR
  CIOCON = 0b00100000;      // CANTX to VDD when recessive

  // Enable required interrupt
  PIR3  = 0b00000000;      // Reset all the interrupt  
  PIE3  = 0b00000010;      // Enable Receiver interrupts
  IPR3  = 0b00000000;      // All interrupt are Low Priority
  TXBIE = 0b00000000;      // No interrupt for transmit Buffer
  BIE0  = 0b11111111;      // Enable interrupt from all receive buffer

  // Request the Normal Mode
  CANCON = 0b00000000;
#else		///?OK
	CAN_NVIC_Init();
#endif
}

/***********************************************************************************************
  FUNCTION   : can_InitQueue
                                                                                              
  DESCRIPTION: Initialise CAN controller Tx/Rx message queue
          
  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : 
            
  COMMENTS   :
***********************************************************************************************/
void can_InitQueue(can_Queue_Struct *QueuePtr)
{

  Queue = QueuePtr;

  // Reset RxQueue overrun and stored messages counters 
  Queue->Rx.OvrCntr  = 0;

  // Initialise RxQueue messages indexes
  Queue->Rx.ReadIdx  = 0;
  Queue->Rx.WriteIdx = 0;
 
  // Reset TxQueue overrun and stored messages counters 
  Queue->Tx.OvrCntr  = 0;
  
  // Initialise TxQueue messages indexes
  Queue->Tx.ReadIdx  = 0;
  Queue->Tx.WriteIdx = 0;
}

/***********************************************************************************************
  FUNCTION   : can_process
                                                                                              
  DESCRIPTION: Check if Can messages needs to be sent
          
  INPUTS     : 

  OUTPUTS    : 
            
  COMMENTS   : Designed to works with only one MOB configure in Tx 29Bit
***********************************************************************************************/
void can_process(void)
{
   can_Msg_Struct *MsgBoxPtr;
   unsigned char NbData,i,*BytePtr;
	 CanTxMsg gCanTxMsgBuff;		//? Add

   // Check if Can Message needs to be transmitted and if the Can transmitter is not Busy
   MsgBoxPtr = can_ReadTxQueue();

#ifndef MM32F0133C7P		//? Add
   if( (MsgBoxPtr != NULL) && (TXB0REQ == 0))
   {
      if(MsgBoxPtr->Ide)
      {
         // Transmission of Extended Message
      
         // Set the 29 Bit ID Tag
         TXB0EIDL = _LOBYTE(_LOWORD(MsgBoxPtr->Id));
        
         TXB0EIDH = _HIBYTE(_LOWORD(MsgBoxPtr->Id));

         TXB0SIDL = ( (_LOBYTE(_HIWORD(MsgBoxPtr->Id)) & 0b00000011) |
                      (_LOBYTE(_HIWORD(MsgBoxPtr->Id)) & 0b00011100) << 3 |
                      0b00001000 );

         TXB0SIDH = ( (_LOBYTE(_HIWORD(MsgBoxPtr->Id)) & 0b11100000) >> 5 |
                      (_HIBYTE(_HIWORD(MsgBoxPtr->Id)) & 0b00011111) << 3  ); 

         // Transfer the data...
         BytePtr = (unsigned char*)&TXB0D0;   // Pointer to Tx Data Buffer
         NbData = TXB0DLC = MsgBoxPtr->Dlc;   // Set Data Length
         if(MsgBoxPtr->Rtr == 1)              // Check if the message is a RTR
            TXB0DLC |= 0b01000000;
         for( i = 0; i < NbData; i++ )        // Transfer the Data
            BytePtr[i] = MsgBoxPtr->Data[i];

         TXB0CON = 0b00001001;   // Start the transmission, Priority Level 2
      }
      else
      {
      
         // Set the 11 Bit ID Tag
         TXB0SIDL = (_LOBYTE(_LOWORD(MsgBoxPtr->Id)) & 0b00000111) << 5;

         TXB0SIDH = ( (_LOBYTE(_LOWORD(MsgBoxPtr->Id)) & 0b11111000) >> 3 |
                      (_HIBYTE(_LOWORD(MsgBoxPtr->Id)) & 0b00000111) << 5  ); 

         // Transfer the data...
         BytePtr = (unsigned char*)&TXB0D0;   // Pointer to Tx Data Buffer
         NbData = TXB0DLC = MsgBoxPtr->Dlc;   // Set Data Length
         if(MsgBoxPtr->Rtr == 1)              // Check if the message is a RTR
            TXB0DLC |= 0b01000000;
         for( i = 0; i < NbData; i++ )        // Transfer the Data
            BytePtr[i] = MsgBoxPtr->Data[i];

         TXB0CON = 0b00001001;   // Start the transmission, Priority Level 2
      }
      can_PullTxQueue();
   }
#else		///?OK
   //if( (MsgBoxPtr != NULL) && (TXB0REQ == 0))		///?OK
	 if ( (MsgBoxPtr != NULL) && (CAN_GetFlagStatus(CAN1, CAN_SR_TS) == 0) )
   {
		  gCanTxMsgBuff.CANID = MsgBoxPtr->Id;
		  gCanTxMsgBuff.CANIDtype = MsgBoxPtr->Ide;
		  gCanTxMsgBuff.RTR = MsgBoxPtr->Rtr;

			if(MsgBoxPtr->Rtr == 1)					// Check if the message is a RTR
			{
				 gCanTxMsgBuff.DLC = 0;
			}
			else
			{
				 gCanTxMsgBuff.DLC = MsgBoxPtr->Dlc;
			}

		  NbData = MsgBoxPtr->Dlc;   			// Set Data Length
		  for( i = 0; i < NbData; i++ )        // Transfer the Data
		  {
				 gCanTxMsgBuff.Data[i] = MsgBoxPtr->Data[i];
			}

			CAN_SendFrame(&gCanTxMsgBuff);
      can_PullTxQueue();
   }
#endif
}

/***********************************************************************************************
  FUNCTION   : can_PullRxQueue                                                       
                                                                                              
  DESCRIPTION: Increase the ReadIdx value into the RxQueue to release the currrent message
               space if RxQueue is not empty. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : 
            
  COMMENTS   :  
***********************************************************************************************/
void can_PullRxQueue(void)
{
   // Next RxQueue read index
   Queue->Rx.ReadIdx=(Queue->Rx.ReadIdx+1) % Queue->Rx.Size;
}


/***********************************************************************************************
  FUNCTION   : can_PullTxQueue                                                       
                                                                                              
  DESCRIPTION: Increase the ReadIdx value into the TxQueue to release the currrent message
               space if TxQueue is not empty. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : 
            
  COMMENTS   :  
***********************************************************************************************/
void can_PullTxQueue(void)
{
   // Next TxQueue read index
   Queue->Tx.ReadIdx=(Queue->Tx.ReadIdx+1) % Queue->Tx.Size;
}


/***********************************************************************************************
  FUNCTION   : can_PushRxQueue                                                       
                                                                                              
  DESCRIPTION: Increase the WriteIdx value into the RxQueue to validate the current message
               if RxQueue is not full. Points to the next empty location. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : 
            
  COMMENTS   :  
***********************************************************************************************/
void can_PushRxQueue(void)
{
     Queue->Rx.WriteIdx=(Queue->Rx.WriteIdx+1) % Queue->Rx.Size;
}


/***********************************************************************************************
  FUNCTION   : can_PushTxQueue                                                       
                                                                                              
  DESCRIPTION: Increase the WriteIdx value into the TxQueue to validate the current message
               if TxQueue is not full. Points to the next empty location. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : 
            
  COMMENTS   :  
***********************************************************************************************/
void can_PushTxQueue(void)
{
     Queue->Tx.WriteIdx=(Queue->Tx.WriteIdx+1) % Queue->Tx.Size;
}


/***********************************************************************************************
  FUNCTION   : can_ReadRxQueue
                                                                                              
  DESCRIPTION: Return a pointer to the RxQueue current message pointed by the ReadIdx.
               The returned pointer is NULL if the RxQueue is empty.

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : RxMsg    - RxQueue message element structure pointer
            
  COMMENTS   :  
***********************************************************************************************/
can_Msg_Struct *can_ReadRxQueue(void)
{
    if (Queue->Rx.ReadIdx != Queue->Rx.WriteIdx){

      // Get indexed RxQueue message pointer
      return &(Queue->Rx.Buffer[Queue->Rx.ReadIdx]);
   }
   else{
      // RxQueue is empty.
      return NULL;
   }
}


/***********************************************************************************************
  FUNCTION   : can_ReadTxQueue
                                                                                              
  DESCRIPTION: Return a pointer to the TxQueue current message pointed by the ReadIdx.
               The returned pointer is NULL if the TxQueue is empty.

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : TxMsg    - TxQueue message element structure pointer
            
  COMMENTS   :  
***********************************************************************************************/
can_Msg_Struct *can_ReadTxQueue(void)
{

   if (Queue->Tx.ReadIdx != Queue->Tx.WriteIdx){
      
      // Get indexed TxQueue message pointer
      return &(Queue->Tx.Buffer[Queue->Tx.ReadIdx]);
   }
   else{
      // TxQueue is empty.
      return NULL;
   }
}


/***********************************************************************************************
  FUNCTION   : can_WriteRxQueue
                                                                                              
  DESCRIPTION: Returns a pointer to the current free space of the selected RxQueue in the 
               order to allow the user to build its message.
               Returns a null pointer if RxQueue is full. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : RxMsg    - RxQueue message element structure pointer
            
  COMMENTS   :  
***********************************************************************************************/
can_Msg_Struct *can_WriteRxQueue(void)
{
     unsigned char tmpidx;

   // Calculate the next index value.
   tmpidx=(Queue->Rx.WriteIdx+1) % Queue->Rx.Size;

   if (tmpidx != Queue->Rx.ReadIdx){

      // Return requested RxQueue message pointer
      return (&(Queue->Rx.Buffer[Queue->Rx.WriteIdx]));
   }
   else{
      // RxQueue is full
      Queue->Rx.OvrCntr++;
      return NULL;
   }
}


/***********************************************************************************************
  FUNCTION   : can_WriteTxQueue
                                                                                              
  DESCRIPTION: Returns a pointer to the current free space of the selected TxQueue in the 
               order to allow the user to build its message.
               Returns a null pointer if TxQueue is full. 

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : TxMsg    - TxQueue message element structure pointer
            
  COMMENTS   :  
***********************************************************************************************/
can_Msg_Struct *can_WriteTxQueue(void)
{
   unsigned char tmpidx;

   // Calculate the next index value.
   tmpidx=(Queue->Tx.WriteIdx+1) % Queue->Tx.Size;

   if (tmpidx != Queue->Tx.ReadIdx){

      // Return requested TxQueue message pointer
      return (&(Queue->Tx.Buffer[Queue->Tx.WriteIdx]));
   }
   else{
      // TxQueue is full
      Queue->Tx.OvrCntr++;
      return NULL;
   }
}


/***********************************************************************************************
  FUNCTION   : can_Rx_Queue_Free_Space
                                                                                              
  DESCRIPTION: Returns free space message element can be stored in RxQueue

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : RxQueue free space message element
            
  COMMENTS   :  
***********************************************************************************************/
unsigned char can_Rx_Queue_Free_Space(void)
{
   if (Queue->Rx.WriteIdx >= Queue->Rx.ReadIdx)
      return (Queue->Rx.ReadIdx + Queue->Rx.Size - Queue->Rx.WriteIdx - 1);
   else
      return (Queue->Rx.ReadIdx - Queue->Rx.WriteIdx - 1);
}


/***********************************************************************************************
  FUNCTION   : can_Tx_Queue_Free_Space
                                                                                              
  DESCRIPTION: Returns free space message element can be stored in TxQueue

  INPUTS     : Queue    - CAN messages queue structure pointer

  OUTPUTS    : TxQueue free space message element(s)
            
  COMMENTS   :  
***********************************************************************************************/
unsigned char can_Tx_Queue_Free_Space(void)
{
   if (Queue->Tx.WriteIdx >= Queue->Tx.ReadIdx)
      return (Queue->Tx.ReadIdx + Queue->Tx.Size - Queue->Tx.WriteIdx - 1);
   else
      return (Queue->Tx.ReadIdx - Queue->Tx.WriteIdx - 1);
}


/***********************************************************************************************
  FUNCTION   : CANIT_handler
                                                                                              
  DESCRIPTION: This function is called following an interrupt from the CAN  

  INPUTS     : None

  OUTPUTS    : None
            
  COMMENTS   : !!! This vector handles many different interrupt sources !!!
***********************************************************************************************/
void CANIT_handler(void)

{
  unsigned char  BufferNumber,*BytePtr;
  unsigned char i,Temp_ECANCON;
  can_Msg_Struct *MsgBoxPtr;

#ifndef MM32F0133C7P		//? Add
  BufferNumber = CANCON & 0b00001111;   // Find the Buffer that has generated the interrupt (FIFO)
  Temp_ECANCON = ECANCON;               // Keep in memory actual value of ECANCON because it need to be restore at the end
  ECANCON &= 0b11100000;                // Set EWIN4:EWIN0 bit to the good buffer
  ECANCON |= BufferNumber | 0b00010000;
  // Now that EWIN has been set, now we can works on RXB0 Buffer Only
  // It really help to reduce code size

  if(RXB0FUL == 0){
    RXBNIF = 0;                // Reset interrupt Flag
    ECANCON = Temp_ECANCON;    // Restaure previous EWIN bits
    return;                    // This condition should not happen
  }
  
   // Request for a Free Element in the RX Queue
   MsgBoxPtr = can_WriteRxQueue();

   if(MsgBoxPtr == NULL){
      if(can_Ctrl_Status.DiscardMsgCntr!=255)
         can_Ctrl_Status.DiscardMsgCntr++;    // Increment the lost message counter

      RXB0FUL = 0;               // Set Register for a new reception
      RXBNIF = 0;                // Reset interrupt Flag
      ECANCON = Temp_ECANCON;    // Restaure previous EWIN bits
      return;
   }
   
   // Extract RTR flag
   MsgBoxPtr->Rtr  = RXB0RTRR0;

   // Extract Data Length
   MsgBoxPtr->Dlc = RXB0DLC & 0b00001111;

   // Extract IDE flag
   MsgBoxPtr->Ide = RXB0EXID;

   // Extract SRR flag (Always set to 1)
   MsgBoxPtr->Srr = RXB0SRR;     

   if(MsgBoxPtr->Ide == 1){

/********************************** Important Note *******************************
The following registers are volatile and they may change during the follwoing read
operations. It is better to read the register once into a temporary variable and
then perform the operations on it.    
*/
       // Extract 29 Bit CAN ID
      _LOBYTE(_LOWORD(MsgBoxPtr->Id)) = RXB0EIDL;
                                        
      _HIBYTE(_LOWORD(MsgBoxPtr->Id)) = RXB0EIDH;

      _LOBYTE(_HIWORD(MsgBoxPtr->Id)) =  (RXB0SIDL & 0b00000011)  |
                                        ((RXB0SIDL & 0b11100000) >> 3) |
                                        ((RXB0SIDH & 0b00000111) << 5)  ;

      _HIBYTE(_HIWORD(MsgBoxPtr->Id)) = (RXB0SIDH & 0b11111000) >> 3;

    }
    else
    {
       // Extract 11 Bit CAN ID
      _LOBYTE(_LOWORD(MsgBoxPtr->Id)) = ((RXB0SIDL & 0b11100000) >> 5) | 
                                        ((RXB0SIDH & 0b00011111) << 3);

      _HIBYTE(_LOWORD(MsgBoxPtr->Id)) = ((RXB0SIDH & 0b11100000) >> 5); 

      _LOBYTE(_HIWORD(MsgBoxPtr->Id)) = 0;

      _HIBYTE(_HIWORD(MsgBoxPtr->Id)) = 0;
    }

    // Transfer the data...
    BytePtr = (unsigned char*)&RXB0D0;
    for( i = 0; i < 8; i++ )
       MsgBoxPtr->Data[i] = BytePtr[i];

    can_PushRxQueue();         // Push the message into the software queue
    RXB0FUL = 0;               // Set Register for a new reception
    RXBNIF = 0;                // Reset interrupt Flag
    ECANCON = Temp_ECANCON;    // Restaure previous EWIN bits
#else		///?OK
		// Request for a Free Element in the RX Queue
		MsgBoxPtr = can_WriteRxQueue();

		if(MsgBoxPtr == NULL){
			if(can_Ctrl_Status.DiscardMsgCntr!=255)
				can_Ctrl_Status.DiscardMsgCntr++;    // Increment the lost message counter

      return;
		}
   
		// Extract RTR flag
		MsgBoxPtr->Rtr  = gCanPeliRxMsgBuff.RTR;

		// Extract Data Length
		MsgBoxPtr->Dlc = gCanPeliRxMsgBuff.DLC;

		// Extract IDE flag
		MsgBoxPtr->Ide = gCanPeliRxMsgBuff.FF;

		// Extract SRR flag (Always set to 1)
		MsgBoxPtr->Srr = 1;
	 
		// Extract ID flag
		MsgBoxPtr->Id = gCanPeliRxMsgBuff.ID;

		// Transfer the data...
		for( i = 0; i < 8; i++ )
			MsgBoxPtr->Data[i] = gCanPeliRxMsgBuff.Data[i];

		can_PushRxQueue();         // Push the message into the software queue
#endif
}
