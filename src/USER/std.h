/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file std.h
       \brief Header file for standard embedded macros and utility functions such as byte swapping

<!-- DESCRIPTION: -->
       Header file for standard embedded macros and utility functions such as byte swapping

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

       $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/std.h,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

       $Log: not supported by cvs2svn $
       Revision 1.1  2008/11/14 19:27:38  blepojevic
       No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

       Revision 1.2  2008-11-14 19:27:37  gfinlay
       - Added file header comment blocks for several files that were missing these.
       - Added debug uart support with serial.c and serial.h


   \endif
*/
/******************************************************************************/
#ifndef __STD_H
#define __STD_H


#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif

#ifdef TASKING_COMPILER
#define BIT     unsigned int
#else
#define BIT     unsigned short
#endif

#ifndef BOOL
//#define BOOL    int			///?OK
#endif
#define CHAR8   char
#define CHAR16  unsigned short
#define FLOAT32 float
#define FLOAT64 double
#define FLOAT80 long
#define INT8    char
#define INT16   short
#define INT32   long
#define INT64   long
#define UINT8   unsigned char
#define UINT16  unsigned short
#define UINT32  unsigned long
#define UINT64  unsigned long

#define UINT1   unsigned char       /* 8 bits, not signed   */
#define UINT2   unsigned short      /* 16 bits, not signed  */
#define UINT4   unsigned long       /* 32 bits, not signed  */
#define SINT1   signed char         /* 8 bits, signed       */
#define SINT2   signed short        /* 16 bits, signed      */
#define SINT4   signed long         /* 32 bits, signed      */
#define TYPEBIT unsigned int        /* 1 bit, for bit field */


//--- Read HighByte and LowByte of an 16bits integer-constant (Little-Endian uC)
#define _HIBYTE_CTE(Cte)  (unsigned char)((unsigned short)(Cte) >> 8)
#define _LOBYTE_CTE(Cte)  (unsigned char)(Cte)

//--- Read HighWord and LowWord of an 32bits integer-constant (Little-Endian uC)
#define _LOWORD_CTE(Cte)  (unsigned short)((unsigned long)(Cte) >> 16)
#define _HIWORD_CTE(Cte)  (unsigned short)(Cte)

//--- Read HighByte and LowByte of an 16bits integer-variable (Little-Endian uC)
#define _LOBYTE(Var)  (*(unsigned char *)&Var)
#define _HIBYTE(Var)  (*((unsigned char *)&Var + 1))

//--- Read HighWord and LowWord of a 32bits long-variable (Little-Endian uC)
#define _LOWORD(Var)  (*(unsigned short *)&Var)
#define _HIWORD(Var)  (*((unsigned short *)&Var + 1))

unsigned short std_Swap16(unsigned short X);
unsigned long std_Swap32(unsigned long X);
signed short std_Swap16s(signed short X);
signed long std_Swap32s(signed long X);

#endif

