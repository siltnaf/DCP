/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file rtclock.c
       \brief Module providing driver functions to access RTC Real-time Clock device

<!-- DESCRIPTION: -->
    To provide functions for access the Real-time Clock device

<!-- NOTES: -->
       \note
       \code
            GLOBAL FUNCTIONS:
                rtfpoll             - Poll for completion of reads and writes
                rtfEnableWrite      - Send write enable message to RTC device
                rtfSetClock         - Send time to RTC device
                rtfRequestClock     - Start a read request for time to RTC device

            INTERNAL FUNCTIONS:
                rtfFromBcd          - Convert Binary Coded Decimal to a number
                rtfToBcd            - Convert a number to Binary Coded Decimal
                rtfSaveReadData     - Store data from read into global data
       \endcode

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/rtclock.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2009/01/09 18:49:44  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.12  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.11  2008-03-26 23:06:10  gfinlay
          Fixed mistake in the function parameter descriptions (doxygen) for the rtfSetClock routine

          Revision 1.10  2008-02-21 20:08:04  msalimi
          - Changed DPF temperature resolution to 3 (from 2) and offset to 0 (from 200).
          - Changed RTC calls to ssfI2CWriteTo and ssfI2CReadFrom to if statements. The routine will return if the call comes back as false.
          - Fixed DM1 processing routine bug. The index was not being incremented and used properly.

          Revision 1.9  2008-01-31 20:54:29  msalimi
          -Added feature to clear DPF missing fault when DPF is uninstalled.
          -Made external and on-board memory applications independant of each other.

          Revision 1.8  2007-12-20 18:19:06  msalimi
          - Fixed triggering, clearing and logging of DPF Lost.
          - Added a feature to not trigger the extreme pressure fault during the 5 minutes countdown to a regen.
          - Fixed using the invalid 520192 SPN.
          - Changed the routine ssfI2CReadFrom to return a value (true or false)
          - Changed monitoring of a successfull read from OBM to look for ssgSuccess instead of ssgDone.
          - Changed the format of OBM records to include time and the fault in one same record.
          - Added a menu item to display the historical faults.
          - OBM related changes are excludable with OBM compile option.

          Revision 1.7  2007-11-30 18:52:08  msalimi
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

          Revision 1.6  2007-10-15 16:28:29  msalimi
          Excluded on board memory module to make room.

          Revision 1.5  2007-10-09 15:03:18  msalimi
          Version 35-PPP-002. Second round of testing..

          Revision 1.4  2007-09-21 20:40:30  msalimi
          Added J1939 real time and date data formatting.

          Revision 1.3  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.2  2007-08-01 20:16:30  nallyn
          Added and modified commenting for Doxygen.

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.11  2006/02/08 20:23:06  nallyn
          Changes based on code review
          Eliminated magic numbers

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

/* ---------- D e f i n i t i o n   F i l e s ---------- */

#include "rtclock.h"            // Interface to real-time clock
#include "main.h"               // Common definitions
#include "syncserial.h"         // Interface to Synchronous Serial
#include "OnBoardNvmem.h"
#include "ExtNvmem.h"
#include "Datalog.h"
#include "uart.h"

/* -------- I n t e r n a l   D e f i n i t i o n s -------- */

// I2C Node addresses for X1226 device - see the datasheet
#define RT_CLK_DEVICE           0xDF
#define RT_EE_DEVICE            0xAF

#define RT_CLK_STATUS_ADDR      0x03F
#define RT_CLK_RTC_ADDR         0x030
#define RT_CLK_CONTROL_ADDR     0x010
#define RT_CLK_ALARM1_ADDR      0x008
#define RT_CLK_ALARM0_ADDR      0x000

#define RT_BUF_LEN              8

#define RT_CLK_SECONDS_OFFSET   0
#define RT_CLK_MINUTES_OFFSET   1
#define RT_CLK_HOURS_OFFSET     2
#define RT_CLK_DATE_OFFSET      3
#define RT_CLK_MONTH_OFFSET     4
#define RT_CLK_YEAR_OFFSET      5
#define RT_CLK_WEEK_DAY_OFFSET  6
#define RT_CLK_YEAR200_OFFSET   7
#define RT_CLK_MILITARY_HOUR_MASK   0x3F

//!Possible polling states
enum poll_states
{
    RT_POLL_IDLE,
    RT_POLL_WRITE_RESPONSE,
    RT_POLL_READ_RESPONSE,
    RT_POLL_WRITE_ENABLE_2,
    RT_POLL_WRITE_ENABLE_COMPLETE
};

//! Buffer and control flags for real-time clock access
typedef struct rtzCtrl
{
    char rtxPending;
    char rtxBusy;
    int  rtxLocation;
    char rtxCount;
    char rtxBuffer[ RT_BUF_LEN ];
} RTZCTRL;

/* ---------- I n t e r n a l   D a t a ---------- */

static char rthPollState;       // Controls access to Serial Device

static RTZCTRL rthWriteCtrl;    // Buffer for queueing a write
static RTZCTRL rthReadCtrl;     // Buffer for queueing a read
static bool rthWrEnPending;     // Flag to save a request for write enable
static char rthWrEnBuf[ 1 ];    // Buffer for write enable commands


/* ---------- G l o b a l   D a t a ---------- */

RTZTIME rtgClock;
extern unsigned char TxPgn_65254[8];

/* ----------------- F u n c t i o n s ----------------- */

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfFromBcd -->
       \brief Convert 2 digit bcd time value to decimal

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  bcd   2-digit time value (secs, mins or hours) stored as 
       high digit in MS 4-bit nibble, and low digit in LS 4-bit nibble
<!-- RETURNS: -->
      \return 2-digit time value (secs, mins or hours) in decimal 

*/
/******************************************************************************/
static char rtfFromBcd( char bcd )
{
    return ( ( ( bcd >> 4 ) * 10 ) + ( bcd & 0x0F ) );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfToBcd -->
       \brief Convert 2 digit decimal time value to bcd

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  val     Decimal value to convert to bcd
<!-- RETURNS: -->
      \return bcd equivalent for time value

*/
/******************************************************************************/
static char rtfToBcd( char val )
{
    return ( ( (  val / 10 ) << 4 ) + ( val % 10 ) );
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfSaveReadData -->
       \brief Convert time data read from EEPROM from bcd to decimal and store in rtgClock

<!-- PURPOSE: -->
      Updates the value of global rtgClock with time data read from EEPROM. 
      Conversion from bcd to decimal for seconds, minutes and hours is done as appropriate.
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void rtfSaveReadData( void )
{
    // If address is the real-time clock  registers
    if ( rthReadCtrl.rtxLocation == RT_CLK_RTC_ADDR )
    {
        // If 24 hour mode is set assume all was set
        if ( ( rthReadCtrl.rtxBuffer[ RT_CLK_HOURS_OFFSET ]
                 & ~RT_CLK_MILITARY_HOUR_MASK ) != 0 )
        {
            // Translate time data from device
            rtgClock.rtxSeconds =
                rtfFromBcd( rthReadCtrl.rtxBuffer[ RT_CLK_SECONDS_OFFSET ] );
            rtgClock.rtxMinutes =
                rtfFromBcd( rthReadCtrl.rtxBuffer[ RT_CLK_MINUTES_OFFSET ] );
            rtgClock.rtxHours =
                rtfFromBcd( rthReadCtrl.rtxBuffer[ RT_CLK_HOURS_OFFSET ] &
                                                    RT_CLK_MILITARY_HOUR_MASK );
            rtgClock.rtxDayOfWeek =
                rthReadCtrl.rtxBuffer[RT_CLK_WEEK_DAY_OFFSET ];
            rtgClock.rtxDate =
                rtfFromBcd( rthReadCtrl.rtxBuffer[RT_CLK_DATE_OFFSET]);
            rtgClock.rtxMonth =
                rtfFromBcd( rthReadCtrl.rtxBuffer[RT_CLK_MONTH_OFFSET]);
            rtgClock.rtxYear =
                rtfFromBcd( rthReadCtrl.rtxBuffer[RT_CLK_YEAR_OFFSET]);
                
            // J1939 data formatting
            TxPgn_65254[0] = (rtgClock.rtxSeconds * 4);
            TxPgn_65254[1] = rtgClock.rtxMinutes;
            TxPgn_65254[2] = rtgClock.rtxHours;
            TxPgn_65254[3] = rtgClock.rtxMonth;
            TxPgn_65254[4] = (rtgClock.rtxDate * 4);
            TxPgn_65254[5] = (rtgClock.rtxYear + 15);
            // End of J1939 data formatting 

            rtgClock.rtxReady = TRUE;
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfPoll -->
       \brief Poll for pending updates for reading from or writing to RTC clock over I2C

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void rtfPoll( void )
{
    // If the serial driver has completed last command
    if ( ssgDone )
    {
        switch ( rthPollState )
        {
            // Waiting for a request
            case RT_POLL_IDLE:
            {
                // If there is a write enable request pending
                if ( rthWrEnPending )
                {
                    // Set WEL bit of clock control register first
                    rthWrEnBuf[ 0 ] = 0x02;
                    
                    // Send first command of RTC write enable to driver
                    if ( ! ( ssfI2CWriteTo( RT_CLK_DEVICE,
                                            RT_CLK_STATUS_ADDR,
                                            rthWrEnBuf,
                                            1 )))
                        return;
                               
                    // Wait for first write to finish, then do second write
                    rthWrEnPending = FALSE;
                    rthPollState = RT_POLL_WRITE_ENABLE_2;
                }
                // If there is a pending write request
                else if ( rthWriteCtrl.rtxPending )
                {
                    // Pass request to driver
                    if (!(ssfI2CWriteTo( RT_CLK_DEVICE,
                                   rthWriteCtrl.rtxLocation,
                                   rthWriteCtrl.rtxBuffer,
                                   rthWriteCtrl.rtxCount)))
                        return;
                                   
                    // Remove the pending flag
                    rthWriteCtrl.rtxPending = FALSE;
                    
                    // Wait for response to write
                    rthPollState = RT_POLL_WRITE_RESPONSE;
                }
                // If there is a pending read request
                else if ( rthReadCtrl.rtxPending )
                {
                    // Pass request to driver
                    if (!(ssfI2CReadFrom( RT_CLK_DEVICE,
                                    rthReadCtrl.rtxLocation,
                                    rthReadCtrl.rtxBuffer,
                                    rthReadCtrl.rtxCount )))
                        return;
                                   
                    // Remove the pending flag
                    rthReadCtrl.rtxPending = FALSE;
                    
                    // Wait for response to read
                    rthPollState = RT_POLL_READ_RESPONSE;
                }
                // else nothing to do
            }
            break;

            // Waiting for response from driver; last request was a write
            case RT_POLL_WRITE_RESPONSE:
                // Done flag from driver
                // Mark buffer as not busy
                rthWriteCtrl.rtxBusy = FALSE;
                
                // Write is complete
                rthPollState = RT_POLL_IDLE;
            break;
                           
            // Waiting for response from driver; last request was a read
            case RT_POLL_READ_RESPONSE:
                // Done flag from driver
                // Mark transfer as done
                
                // If success from driver
                //if ( ssgSuccess )
                {
                    // Save the read response data
                    rtfSaveReadData();
                }
                
                // Free the buffer for another read
                rthReadCtrl.rtxBusy = FALSE;
                
                // Read is complete
                rthPollState = RT_POLL_IDLE;
            break;
            
            // Wait for completion of first write enable command
            // Send second command of RTC write enable to driver
            case RT_POLL_WRITE_ENABLE_2:
            {
                // Set RWEL and WEL bits of clock control register
                rthWrEnBuf[ 0 ] = 0x06;
                    
                // Pass request to driver
                if (!(ssfI2CWriteTo( RT_CLK_DEVICE,
                               RT_CLK_STATUS_ADDR,
                               rthWrEnBuf,
                               1)))
                    return;
                               
                // Wait for enable completion
                rthPollState = RT_POLL_WRITE_ENABLE_COMPLETE;
                
            }
            break;

            // Wait for completion of second write enable command
            case RT_POLL_WRITE_ENABLE_COMPLETE:
            {
                // Write enable is complete, go to idle processing
                rthPollState = RT_POLL_IDLE;
            }
            break;
            
            default:
                rthPollState = RT_POLL_IDLE;
                break;
        }
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfEnableWrite -->
       \brief Enable writes to RTC clock

<!-- PURPOSE: -->
<!-- PARAMETERS: none-->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void rtfEnableWrite( void )
{
    // Save the request for write enable for later processing
    rthWrEnPending = TRUE;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfSetClock -->
       \brief Set the value stored on the RTC device using day/hour/minutes/date/month/year
        as provided by parameters

<!-- PURPOSE: -->
<!-- PARAMETERS: -->
       @param[in]  day     Day in decimal  (1-7)
       @param[in]  hours   Hour in decimal (0-23)
       @param[in]  minutes Minutes in decimal (0-59)
       @param[in]  date    Date (day of week) in decimal (1-7)
       @param[in]  month   Month in decimal (1-12)
       @param[in]  year    Year in decimal
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void rtfSetClock( char day, char hours, char minutes, char date, char month, char year )
{
    // If the write buffer is not busy
    if ( rthWriteCtrl.rtxBusy == FALSE )
    {
        // Set up values
        rthWriteCtrl.rtxBuffer[ 0 ] = 0;                    // seconds
        rthWriteCtrl.rtxBuffer[ 1 ] = rtfToBcd( minutes );  // minutes
        rthWriteCtrl.rtxBuffer[ 2 ] = rtfToBcd( hours );    // hours
        rthWriteCtrl.rtxBuffer[ 2 ] |= 0x80;                // 24 hour mode
        rthWriteCtrl.rtxBuffer[ 3 ] = rtfToBcd(date);       // date
        rthWriteCtrl.rtxBuffer[ 4 ] = rtfToBcd(month);      // month
        rthWriteCtrl.rtxBuffer[ 5 ] = rtfToBcd(year);       // year
        rthWriteCtrl.rtxBuffer[ 6 ] = day;                  // day of week
        rthWriteCtrl.rtxBuffer[ 7 ] = 0x20;                 // century 20
        
        // Set number of bytes to write
        rthWriteCtrl.rtxCount = 8;
        
        // Set to write to RTC data locations
        rthWriteCtrl.rtxLocation = RT_CLK_RTC_ADDR;
        
        // Buffer is busy, request is pending
        rthWriteCtrl.rtxBusy = TRUE;
        rthWriteCtrl.rtxPending = TRUE;
    }
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: rtfRequestClock -->
       \brief Request is queued to read from RTC clock

<!-- PURPOSE: -->
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return TRUE if RTC read is queued, or FALSE if unable to queue request

*/
/******************************************************************************/
void rtfRequestClock( void )
{
    // If the read buffer is not busy
    if ( !rthReadCtrl.rtxBusy )
    {
        // Set number of bytes to read
        rthReadCtrl.rtxCount = RT_BUF_LEN;
        
        // Set to read from RTC data locations
        rthReadCtrl.rtxLocation = RT_CLK_RTC_ADDR;
        
        // Set buffer to busy, request pending
        rthReadCtrl.rtxBusy = TRUE;
        rthReadCtrl.rtxPending = TRUE;
    }
}

void Record_Real_Time_And_Date (unsigned char device)
{
#ifdef EXT_MEM
unsigned char Data[8];

    if (device == EXTERNAL)
    {
        // Prepare data to be recorded at internal memory
        Data[0] = RTC_RECORD;

        Data[1] = (rtgClock.rtxMinutes << 6);
        Data[1] += rtgClock.rtxSeconds;

        Data[2] = (rtgClock.rtxMinutes >> 2);
        Data[2] += (rtgClock.rtxHours << 4);

        Data[3] = (rtgClock.rtxHours >> 4);
        Data[3] += (rtgClock.rtxDate << 1);
        Data[3] += (rtgClock.rtxMonth << 6);

        Data[4] = (rtgClock.rtxMonth >> 2);
        Data[4] += (rtgClock.rtxYear << 2);

        if ( ( Datalog.Ext_Mem_Present ) && ( device == EXTERNAL ) )
        {
            // Write to external memory
            if ( EXM_fnLogData( Data, 5 ) )
                Increment_Sync_Insertion_Counter();
        }
    }
#endif // EXT_MEM
}
