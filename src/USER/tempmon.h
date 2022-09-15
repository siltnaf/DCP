/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file tempmon.h
       \brief Header file defining the interface to the Temperature Monitor Extended mode State Machine Module

<!-- DESCRIPTION: -->
    To provide the interface to Temperature Monitor state machine.

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/tempmon.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.3  2014/04/16 14:07:10  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.2  2013/09/19 13:07:45  blepojevic
          Version with improved temperature measurement in the range of low temperatures and  cold start operation. In case of missing thermistor APU will not go to cold start (starter engaged longer, 15 sec). Introduced reset of DPF faults in menu option Faults. Also fixed bugs in the software, and made many improvements

          Revision 1.1  2009/10/14 14:47:30  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.4  2008-11-14 19:27:37  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.3  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.2  2007-08-01 20:18:06  nallyn
          Modified commenting for Doxygen.

          Revision 1.1  2007-07-05 20:10:13  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.13  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.

          Revision 1.12  2007/02/26 17:49:52  nallyn
          Added the ability to have two more timers for a total of 6 at the expense of battery monitor and comfort monitor by defining EXTRA_TIMERS

          Revision 1.11  2006/02/08 20:24:09  nallyn
          Changes based on code review
          Eliminated magic numbers

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers
          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

#ifndef TEMPMON_H
#define TEMPMON_H

/* ---------- D e f i n i t i o n s ---------- */
#define TM_MINIMUM_RUN      ( 10 )      //!< Minimum run time in minutes

#define TM_CELSIUS_INVALID  ( 99 )

enum tmthermostat
{
    TM_SEARCHING,
    TM_HEATING,
    TM_COOLING
};

enum tempUnit
{
    CELSIUS,
    FAHRENHEIT
};

typedef signed char TM_CELSIUS;

struct tmzVadCnv
{
    TM_CELSIUS  tmxC;           //!< StartC - first Celsius value in the range
    int         tmxVad;         //!< StartVad - first AtoD reading in a range
    signed char tmxFactor;      //!< factor - scaling factor in the range
};

#ifndef MM32F0133C7P		//? Add
//! Interpolation table for converting an AtoD reading to a Celsius temperature
struct tmzVadCnv tmhVadTable[] =
{
    // C, VAD,  Factor ( ( v2-v1) / (c2-c1 ) )
    {-40, 996, -20 },
    {-30, 976, -38 },
    {-20, 938, -62 },
    {-10, 876, -89 },
    { 2,  769, -111 },
    { 18, 592, -113 },
    { 28, 479, -105 },
    { 38, 374, -92 },
    { 46, 300, -79 },
    { 55, 229, -59 },
    { 65, 170, -39 }
};
#else
extern struct tmzVadCnv tmhVadTable[11];
#endif

/* ---------- G l o b a l   D a t a ---------- */

extern bit tmgTempMonRun;
extern char tmgThermo;

extern char tmgTMAccess;            // Allow user access to feature setting
extern bit tmgTMEnable;             // TempMon feature enabled
extern bit tmgTMActive;             // TempMon is active (enabled and toggle on)
extern bit tmgTMMode;               // TempMon Mode (enabled and duration not zero
extern TM_CELSIUS tmgTMTemp;        // Temperature set point
extern TM_CELSIUS tmgTMRange;       // Temperature range set point
extern char tmgFSRange;             // Fan speed range
extern char tmgFSSelected;          // Fan speed selected
extern char tmgTMDuration;          // Duration

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

signed int tmfVtoCF( int v, char F );
char tmfThermostat( char thermostat, TM_CELSIUS setpoint, unsigned char *pCounter );
void tmfSetMode( void );

#ifndef EXTRA_TIMERS
void tmfTempMon( void );
#endif // EXTRA_TIMERS

#endif  // TEMPMON_H
