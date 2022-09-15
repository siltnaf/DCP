/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file OnBoardNvmem.h
       \brief Header file defining interface to On Board Non-Volatile Memory

<!-- DESCRIPTION: -->

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/OnBoardNvmem.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
    
       $Log: not supported by cvs2svn $
       Revision 1.2  2014/04/16 14:07:07  blepojevic
       Added diagnostic of APU and CCU

       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.6  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.5  2008-04-02 19:50:10  msalimi
       - Fixed onboard memory reading bug.
       - There is no one hour recall of faults if APU is off.
       - Fault viewing menu rolls over from the end to the beginning of the faults and vice versa with arrow keys.
       - DPF lost fault will not be detected if APU is off.

       Revision 1.4  2008-03-26 22:41:14  gfinlay
       Fixed file header comment block so it parses properly for Doxygen, and so it retrieves the file history log properly with CVS.

       \endif
*/
/******************************************************************************/
#ifndef ONBOARDNVMEM_H
#define ONBOARDNVMEM_H


#include "tfxstddef.h"
/* ----------G l o b al  D e f i n i t i o n s ---------- */
#define RECORD_SIZE	  		8      // Length of record in bytes (1-62)

#define BUFFER_SIZE	   		5      // Number of Buffered Records 

#define MEM1_PAGES				510			// Number of pages (64 bytes) used by memory space 1 
                                // remaining pages are used by memory space 2.

#define MEM1_SIZE					MEM1_PAGES *64

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

char OBM_fnInit(uint8 u8MemSel);
char OBM_fnAddRecord(uint8 *puData,uint8 u8MemSel);
void OBM_fnReset(uint8 u8MemSel);
uint16 OBM_fnGetTotalRecords(uint8 u8MemSel);
char OBM_fnGetRecord(unsigned char *pu8Data, uint16 u16RecordNum,uint8 u8MemSel);
unsigned char OBM_fnCheckBusy(void);
void OBM_fnProcess(void);
unsigned char Check_Read_Checksum(unsigned char * pu8Data);
void nvmUpdate(void);

#endif  // ONBOARDNVMEM_H
