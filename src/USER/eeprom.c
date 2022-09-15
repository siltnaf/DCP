/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file eeprom.c
       \brief Routines for reading and writing to EEPROM

<!-- DESCRIPTION: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:
	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/eeprom.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.2  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.1  2007-07-05 20:10:12  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.10  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */
//?#include <pic18.h>
#include "eeprom.h"
#include "i2c.h"				//? Add

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */
/* ---------- I n t e r n a l   D a t a ---------- */
/* ---------- G l o b a l   D a t a ---------- */
/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_GetInt -->
       \brief Read a 16-bit integer from EEPROM

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  addr Unsigned 8-bit integer which is offset of location in EEPROM to read
<!-- RETURNS: -->
      \return 16-bit integer read from the desired location

*/
/******************************************************************************/
unsigned int EEPROM_GetInt(unsigned char addr)
{
	unsigned int tempInt;

	tempInt = eeprom_read(addr);
	tempInt = (tempInt<<8)+eeprom_read(addr+1);
	return(tempInt);
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_PutInt -->
       \brief Write a long value to EEPROM

<!-- PURPOSE: -->
       LSByte is in the higher EEPROM address
<!-- PARAMETERS: -->
       @param[in]  addr    Location (8-bit offset) where the value will be written
       @param[in]  value   The unsigned int value to be written
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void EEPROM_PutInt(unsigned char addr,unsigned int value)
{
	eeprom_write(addr+1,(value&0xFF));
	eeprom_write(addr,((value>>8)&0xFF));
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_GetChar -->
       \brief Read a 8 bit char value from EEPROM

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  addr    Unsigned 8-bit integer which is offset of location in EEPROM to read
<!-- RETURNS: -->
      \return 8-bit char value read from the desired location

*/
/******************************************************************************/
unsigned char EEPROM_GetChar(unsigned char addr)
{
	return ( eeprom_read(addr) );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: EEPROM_PutChar -->
       \brief Write a 8 bit char value to EEPROM

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  addr    Location (8-bit offset) where the value will be written
       @param[in]  value   The unsigned 8-bit char value to be written
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void EEPROM_PutChar(unsigned char addr, unsigned char value)
{
	eeprom_write(addr,value);
}

