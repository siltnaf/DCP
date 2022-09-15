/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file tfxstddef.h
       \brief Teleflex standard definitions

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

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/tfxstddef.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	    $Log: not supported by cvs2svn $
	    Revision 1.1  2008/08/21 15:43:12  blepojevic
	    No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	    Revision 1.4  2008-08-21 15:43:12  msalimi
	    - Added feature to turn on the backlight when fault or important messages are being displayed.
	    - Modified the feature that cleared displayed messages with ANY button press to ESC/RESET button.
	    - Cancelled reception of three DM1s: high frequency, low frequency and high voltage.
	    - Added a feature to shutdown the APU in case of high exhaust temperature fault. this fault is ignored during emergency regeneration.

	    Revision 1.3  2007-08-27 22:19:08  msalimi
	    CAN communication working.

	    Revision 1.2  2007-07-05 21:29:05  gfinlay
	    Added proper new format for Doxygen file header comment block

	    Revision 1.1  2007-07-05 20:10:13  gfinlay
	    Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	    Revision 1.3  2006/04/07 20:53:43  nallyn
	    Updated NULL definition

	    Revision 1.2  2006/04/07 17:40:46  nallyn
	    1. always.h is removed from project per Code Review
	    2. Other files added as new to project as a result of Code Review
	    
	    Revision 1.1  2006/03/27 22:17:40  nallyn
	    first commit
	    
	    Revision 1.2  2006/03/25 01:21:19  nallyn
	    added ENABLE and DISABLE defines
	    
	    Revision 1.1  2006/03/22 00:25:21  nallyn
	    First commit of files
       \endif
*/
/******************************************************************************/

#ifndef TFXSTDDEF_H
#define TFXSTDDEF_H

#ifdef MM32F0133C7P			//? Add
/* ---------- D e f i n i t i o n   F i l e s ---------- */
#include	"types.h"
#endif

/* --------------- D e f i n i t i o n s --------------- */
//?typedef bit             uint1;
//typedef unsigned char   bit;				//? Add
typedef signed char     int8;
typedef unsigned char   uint8;
typedef signed short    int16;
typedef unsigned short  uint16;
typedef signed long     int32;
typedef unsigned long   uint32;

#ifndef  __cplusplus
#ifndef MM32F0133C7P		//? Add
typedef unsigned char   bool;
#endif
#endif

#ifndef NULL
#define NULL    (void *) 0
#endif

#ifndef FALSE

#define FALSE   0
#define TRUE    1

#define NO      FALSE
#define YES     TRUE

#define OFF     FALSE
#define ON      TRUE

#ifndef MM32F0133C7P		//? Add
#define SET		TRUE
#define CLEAR	FALSE

#define ENABLE	TRUE
#define DISABLE	FALSE
#else
#define CLEAR	FALSE
#endif

#endif

#define BACKLIGHT_ON    1
#define BACKLIGHT_OFF   0


// Used for bit shifting to get upper components of larger data type
#define U8_BIT_COUNT        8       // Number of bits in a uint8 data type
#define U16_BIT_COUNT       16      // Number of bits in a uint16 data type
#define U32_BIT_COUNT       32      // Number of bits in a uint32 data type

#ifndef MM32F0133C7P		//? Add
#define UINT8_MAX           255
#define UINT16_MAX          65535
#define UINT32_MAX          4294967295L
#define INT8_MAX            127
#define INT8_MIN            -128
#define INT16_MAX           32767
#define INT16_MIN           -32768
#define INT32_MAX           2147483647L
#define INT32_MIN           -2147483648L
#endif

#define U16_UPPER_U8(x)     ( (uint8) ( x >> U8_BIT_COUNT ) )
#define U16_LOWER_U8(x)     ( (uint8) x )
#define U32_UPPER_U16(x)    ( (uint16) ( x >> U16_BIT_COUNT ) )
#define U32_LOWER_U16(x)    ( (uint16) x )

// Get dimension of array
// Divide total size of array in bytes by size of an array element
#define DIM( x )        	( sizeof( x ) / sizeof( x[0] ) )

/* --------------- G l o b a l   D a t a --------------- */

// None - no associated .c files

/* ---------- F u n c t i o n   P r o t o t y p e s ---------- */

// None - no associated .c files

/* ------- F u n c t i o n   P r o t o t y p e s ------- */

#endif //TFXSTDDEF_H
