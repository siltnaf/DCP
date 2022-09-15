/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file ExtNvmem.h
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
       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/ExtNvmem.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
    
       $Log: not supported by cvs2svn $
       Revision 1.2  2010/12/01 22:44:24  dharper
       Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
       This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
       This is because REV B was never checked in.

       Revision 1.1  2008/11/14 19:27:36  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.6  2008-11-14 19:27:36  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h

       Revision 1.5  2008-03-26 22:40:16  gfinlay
       Fixed file header comment block so it parses properly for Doxygen, and so it retrieves the file history log properly with CVS. Also fixed the file name in the header block.

       \endif
*/
/******************************************************************************/
#ifndef EXTNVMEM_H
#define EXTNVMEM_H


#include "tfxstddef.h"
/* ----------G l o b al  D e f i n i t i o n s ---------- */
#define EXT_BUFFER_SIZE	   		20
#define BUFF_ELEM_SIZE		20		 // Size of each buffered element in bytes

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

char EXM_fnInit(void);
char EXM_fnLogData(uint8 *puData,uint8 u8DataLen);
void EXM_fnReset(void);
char EXM_fnCheckBusy(void);
uint32 EXM_fnCheckBytesLeft(void);
char EXM_fnIsConnected(void);
void EXM_fnProcess(void);

#endif  // EXTNVMEM_H
