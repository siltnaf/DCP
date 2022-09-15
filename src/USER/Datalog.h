/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file Datalog.h
       \brief Interface for data logging manager

<!-- DESCRIPTION: -->
       Interface for data logging manager

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/Datalog.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.6  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

         Revision 1.5 Fri Dec 21 20:36:43 2007 
         - Added new DM1s for generator frequency measurement timeout, voltage low and high, and generator short or open circuit.
         - Changed the APU record to contain APU total number of starts instead of cranks. This is a two byte data.
         - Increased the length of DM1 message to 34 to be able to receive up to 8 faults currently designed in the application.
         - Added a small print out to the bottom of the home screen to show : Regen soon, Regenerating, and APU Cooling down.

         Revision 1.4 Tue Oct 16 20:40:22 2007 
         Version 35-DCP-004. External memory error correction.

         Revision 1.3 Fri Sep 21 20:52:21 2007 
         *** empty log message ***

         Revision 1.2 Tue Sep 18 19:23:59 2007 
         Moved defines from OnBoardNvmem.h

         Revision 1.1 Mon Sep 17 18:43:50 2007 
         *** empty log message ***

   \endif
*/
/******************************************************************************/

#ifndef DATALOG_H
#define DATALOG_H

#define SYNC_BYTE  0xAA
#define MAX_LONG_LOG_LENGTH 100


#define RTC_RECORD              0
#define APU_RECORD_NEW          1   // Start byte is increased to 2 bytes and it contains the total number of APU starts since SW update
#define APU_RECORD              2
#define DPF_STATUS_RECORD       3
#define CCU_RECORD              4
#define LONG_RECORD             127
#define FAULT_RECORD            128

#define RTC_RECORD_LEN          5
#define APU_RECORD_NEW_LEN      11
#define APU_RECORD_LEN          10
#define DPF_STATUS_RECORD_LEN   5
#define CCU_RECORD_LEN          2
//#define LONG_RECORD_LEN       NOT DEFINED, must be read from the record itself
#define FAULT_RECORD_LEN        5


#define INTERNAL    0
#define EXTERNAL    1

typedef struct
{
    unsigned char Logging_On        :1;
    unsigned char Ext_Mem_Present   :1;
    unsigned char Sync_Insertion_Counter;
    unsigned char Index;
}DATALOG;

extern DATALOG Datalog;

// Prototypes
void Init_Datalogging (void);
void Start_Datalog(void);
void Stop_Datalog(void);
void Add_To_Long_Record(unsigned int pressure, unsigned int temperature);
void Increment_Sync_Insertion_Counter(void);

#endif  // DATALOG_H
