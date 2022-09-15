/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file nvmem.c
       \brief Access to Non-Volatile Memory

<!-- DESCRIPTION: -->
    To provide functions for saving and restoring data to and from
    some form of non-volatile memory device.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/nvmem.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:38  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.2  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.12  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.

          Revision 1.11  2007/02/26 17:49:52  nallyn
          Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "nvmem.h"              // Interface to Non-Volatile Memory storage
#include "main.h"               // Common definitions
#include "eeprom.h"             // Interface to PIC EEPROM
#include "delay18.h"            // for DelayMs()


/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

#define NV_BYTE2_XOR    0xCC

/* ---------- I n t e r n a l   D a t a ---------- */


/* ---------- G l o b a l   D a t a ---------- */


/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfGetByte -->
       \brief Gets the value of a byte from EEPROM

<!-- PURPOSE: -->
       Gets the value of the byte specified by the ID from EEPROM. There are three 
       redundant copies of each byte stored in EEPROM. The first copy is stored as is, the 
       second copy is stored XOR'd with 0xCC, and the third is stored with bits inverted.
<!-- PARAMETERS: -->
       @param[in]  id      index into array of byte values to retrieve (3 bytes for each ID index)
       @param[in]  deflt   default value for parameter to return if the EEPROM is blank
<!-- RETURNS: -->
      \return Value of byte retrieved for the corresponding ID

*/
/******************************************************************************/
static NV_BYTE nvfGetByte( NV_ID id, NV_BYTE deflt )
{
    NV_BYTE value1, value2, value3;
    unsigned char addr;

    addr = NV_EE_FIRST_ADDR + id * NV_EE_STORE_MULT;

    // Byte 3 is stored inverted so a blank eeprom can be detected
    value1 =  EEPROM_GetChar( addr   );  
    value2 =  EEPROM_GetChar( addr + 1 ) ^ NV_BYTE2_XOR;
    value3 = ~EEPROM_GetChar( addr + 2 );
    
    if ( ( value1 == value2 ) || ( value1 == value3 ) )
    {
        return( value1 );
    }
    else if ( value2 == value3 )
    {
        return( value2 );
    }

    return( deflt );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfPutByte -->
       \brief Write the value of a byte at a location in EEPROM specified by ID

<!-- PURPOSE: -->
       Detailed description of function
<!-- PARAMETERS: -->
       @param[in]  id      Specifies the index into the EEPROM area for byte value (3 bytes per ID)
       @param[in]  value   Specifies the value of the byte to store
<!-- RETURNS: -->
      \return TRUE if the byte was stored correctly, or FALSE if the byte was not stored correctly

*/
/******************************************************************************/
static NV_BOOL nvfPutByte( NV_ID id, NV_BYTE value )
{
    NV_BYTE temp;
    unsigned char addr;

    addr = NV_EE_FIRST_ADDR + id * NV_EE_STORE_MULT;

    // Byte 3 is stored inverted so a blank eeprom can be detected
    EEPROM_PutChar( addr,      value );
    EEPROM_PutChar( addr + 1,  value ^ NV_BYTE2_XOR );
    EEPROM_PutChar( addr + 2, ~value );

    DelayMs( 20 );

    temp = nvfGetByte( id, 0 );
    if ( temp == value )
    {
        return( TRUE );
    }
    else
    {
        return( FALSE );
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfGetTemperature -->
       \brief Get the stored temperature from EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      index into array of byte values to retrieve (3 bytes for each ID index)
       @param[in]  deflt   default value for temperature to return if the EEPROM is blank
<!-- RETURNS: -->
      \return Signed 8-bit Temperature value

*/
/******************************************************************************/
signed char nvfGetTemperature( NV_ID id, signed char deflt )
{
    return nvfGetByte( id, deflt );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfPutTemperature -->
       \brief Put the temperature to the proper location in EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      Specifies the index into the EEPROM area for temp value (3 bytes per ID)
       @param[in]  value   Specifies the temperature value to store
<!-- RETURNS: -->
      \return TRUE if the temperature byte was stored correctly, or FALSE if not stored correctly

*/
/******************************************************************************/
char nvfPutTemperature( NV_ID id, signed char value )
{
    return nvfPutByte( id, value );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfGetVoltage -->
       \brief Get the stored voltage from EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      index into array of byte values to retrieve (3 bytes for each ID index)
       @param[in]  deflt   default value for voltage to return if the EEPROM is blank
<!-- RETURNS: -->
      \return Unsigned 8-bit voltage value

*/
/******************************************************************************/
unsigned char nvfGetVoltage( NV_ID id, unsigned char deflt )
{
    return nvfGetByte( id, deflt );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfPutVoltage -->
       \brief Put the voltage to the proper location in EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      Specifies the index into the EEPROM area for voltage value (3 bytes per ID)
       @param[in]  value   Specifies the voltage value to store
<!-- RETURNS: -->
      \return TRUE if the voltage byte was stored correctly, or FALSE if not stored correctly

*/
/******************************************************************************/
char nvfPutVoltage( NV_ID id, unsigned char value )
{
    return nvfPutByte( id, value );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfGetNumber -->
       \brief Get the stored number from EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      index into array of byte values to retrieve (3 bytes for each ID index)
       @param[in]  deflt   default value for number to return if the EEPROM is blank
<!-- RETURNS: -->
      \return Unsigned 8-bit number

*/
/******************************************************************************/
unsigned char nvfGetNumber( NV_ID id, unsigned char deflt )
{
    return nvfGetByte( id, deflt );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfPutNumber -->
       \brief Put the unsigned 8-bit number to the proper location in EEPROM 

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      Specifies the index into the EEPROM area for number (3 bytes per ID)
       @param[in]  value   Specifies the number value to store
<!-- RETURNS: -->
      \return TRUE if the number byte was stored correctly, or FALSE if not stored correctly

*/
/******************************************************************************/
char nvfPutNumber( NV_ID id, unsigned char value )
{
    return nvfPutByte( id, value );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfPutTime -->
       \brief Stored the 3 byte time value to EEPROM

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      Base ID of time value ID in EEPROM
       @param[in]  time    Time structure containing 1 byte each for day of week, hours and minutes
<!-- RETURNS: -->
      \return TRUE if the time was stored properly or FALSE if the time was not stored

*/
/******************************************************************************/
NV_BOOL nvfPutTime( NV_ID id, TKZTIME time )
{
    char result;
    
    result = TRUE;
    
    if ( !nvfPutNumber( id, time.tkxDayOfWeek ) )
        result = FALSE;
        
    if ( !nvfPutNumber( id+1, time.tkxHours ) )
        result = FALSE;
        
    if ( !nvfPutNumber( id+2, time.tkxMinutes ) )
        result = FALSE;
    
    return result;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: nvfGetTime -->
       \brief Retrieve the time value from EEPROM

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  id      Base ID of time value ID in EEPROM
       @param[in]  time    Pointer to Time structure buffer containing 1 byte each for day of week, hours and minutes
       @param[in]  day     Day of week
       @param[in]  hour    Hour
       @param[in]  minute  Minute
<!-- RETURNS: -->
      \return Void. Updated value for the retrieved time in the TKZTIME structure supplied.

*/
/******************************************************************************/
void nvfGetTime( NV_ID id, TKZTIME *time, char day, char hour, char minute )
{
    time->tkxDayOfWeek = nvfGetNumber( id,   day    );
    time->tkxHours     = nvfGetNumber( id+1, hour   );
    time->tkxMinutes   = nvfGetNumber( id+2, minute );
}
