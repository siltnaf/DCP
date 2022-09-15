/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file ice_pnl.h
       \brief Header file for CCU Climate Control state machine module

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

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/ice_pnl.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.5  2014/08/25 19:31:34  blepojevic
          Stepped back, changed debounce time for fault detection to 2 sec. Check coninuously during this 2 sec is the same fault present and record it. Also is returned delay 10/5 sec to have recorded fault Main Breaker Tripped.

          Revision 1.4  2014/07/25 15:24:16  blepojevic
          Added option to disable heaters and RESET faults removed to menu SYSTEM OPTION menu to allow password protection

          Revision 1.3  2014/06/27 17:35:27  blepojevic
          Increased delay from 5 sec to 10 sec to have recorded fault Main Breaker Tripped. Also is changed delay when AC power was present and disappeared from 1 Sec to 5 sec. This version had added 2 new CAN messages for APU and CCU faults broadcasted every 1 sec

          Revision 1.2  2014/04/16 14:07:08  blepojevic
          Added diagnostic of APU and CCU

          Revision 1.1  2010/01/12 23:31:50  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

            Revision 1.7  2010-01-12 15:29  echu
          - changed comfort monitor to output high heat.

          Revision 1.6  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.5  2007-10-26 18:42:14  msalimi
          Added 5 minute cooldown for APU after DPF regeneration.
          Changed DPF variable menu to show all variables in one screen.
          Cancelled the recording of the temperature and pressure in DPF Status to the external memory.
          Changed the recording of the CCU record to the external memory to be performed every minute rather than when there is a change in the reading.

          Revision 1.4  2007-09-21 20:52:21  msalimi
          *** empty log message ***

          Revision 1.3  2007-09-17 18:41:35  msalimi
          DPF related modifications

          Revision 1.2  2007-08-27 22:19:08  msalimi
          CAN communication working.

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.10  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005

       \endif
*/
/******************************************************************************/

#ifndef ICE_PNL_H
#define ICE_PNL_H

#include "tfxstddef.h"     // Teleflex Standard definitions

/* ---------- D e f i n i t i o n s ---------- */

    
/* ---------- G l o b a l   D a t a ---------- */


/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

// SPN definitions for CCU faults
//AC_PWR main breaker(35A) - SPN Generator Circuit Breaker Status
#define SPN_CCU_MAIN_BR_TRIPPED   3545
#define FMI_CCU_MAIN_BR_TRIPPED     0

//BR1 - hvac breaker (20A) - SPN Utility Circuit Breaker Status
#define SPN_CCU_BR1_TRIPPED   	3546
#define FMI_CCU_BR1_TRIPPED     0

//BR2 - outlet breaker (20A) - SPN Auxiliary AC Breaker Tripped
#define SPN_CCU_BR2_TRIPPED	5869
#define FMI_CCU_BR2_TRIPPED   	31

void decode_ac_br1_br2(volatile int adInputValue);
void thermostat(void);
void outlet(void);
void ac_heat_flag(void);
void ICE_fnDrawThermostat( void );
void ICE_fnDrawFan( void );
void fan_flag(void);
char check_hvac_on(void);
void ICE_fnLoadControl( bool bLoadsOff );
void Record_CCU_Status_External (void);
void Enable_Heaters(bool enable);

#endif  // ICE_PNL_H
