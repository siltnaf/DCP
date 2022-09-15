/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file battmon.h
       \brief Header file for Battery voltage monitor 

<!-- DESCRIPTION: -->
    Provide the interface to using the Battery Voltage Monitor

<!-- NOTES: -->

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

          $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/battmon.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
       
          $Log: not supported by cvs2svn $
          Revision 1.1  2008/11/14 19:27:36  blepojevic
          No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

          Revision 1.3  2008-11-14 19:27:36  gfinlay
          - Added file header comment blocks for several files that were missing these.
          - Added debug uart support with serial.c and serial.h

          Revision 1.2  2007-08-01 19:58:36  nallyn
          Modified commenting for Doxygen

          Revision 1.1  2007-07-05 20:10:12  gfinlay
          Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

          Revision 1.14  2007/04/03 18:44:42  gfinlay
          Placing ES35-000-04A files back at the head of the main truck or baseline - we will subsequently place the multi-timer or 10-timer versions as branches off the main baseline so that we can continue multi-timer version development on a parallel branch. The multi-timer stuff was committed or merged with the main trunk originally - this was a mistake.

          Revision 1.13  2007/02/26 17:23:11  nallyn
          Added the ability to not include this module with the definition of EXTRA_TIMERS

          Revision 1.12  2006/02/08 20:10:28  nallyn
          Changes based on code review
          Changed the way the battery voltage is read and decoded

          Revision 1.11  2006/02/08 19:39:08  nallyn
          Review Code with updated file headers

          Revision 1.10  2006/01/24 16:51:25  nallyn
          Added voltage calibration

          Revision 1.9  2005/12/20 18:04:46  nallyn
          November 27 2005
       
       \endif
*/
/******************************************************************************/

#ifndef BATTMON_H
#define BATTMON_H

/* ---------- D e f i n i t i o n s ---------- */

/* ---------- G l o b a l   D a t a ---------- */

#ifndef EXTRA_TIMERS
extern char bmgBMAccess;        
extern bit BM_RUN;             
extern int BMON_VOLTAGE;     
extern bit volt_monitor;        
extern char bmgVoltAccess;
extern signed char bmgVOffset;
#endif // EXTRA_TIMERS

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

#ifndef EXTRA_TIMERS
int battery_voltage( void );
void battery_monitor(void);
#endif // EXTRA_TIMERS

#endif //BATTMON_H
