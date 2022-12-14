/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file TX16FULL.H
       \brief Header file defining bitmaps for 16x16 LCD Icons - autogenerated code

<!-- DESCRIPTION: -->
<!-- NOTES: -->

       \if ShowHist
       Copyright ? 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

     CHANGE HISTORY:

	      $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/TX16FULL.H,v 1.1 2015-06-30 20:41:22 blepojevic Exp $

	      $Log: not supported by cvs2svn $
	      Revision 1.2  2010/12/01 22:44:24  dharper
	      Fixes version number displayed in DCP3 software to Rev C from Rev A.  this is because REV B still had REV A listed as the version.
	      This revision also contains chages to the main page of the display so that time and current ambient temp. are displayed.
	      This is because REV B was never checked in.
	
	      Revision 1.1  2008/11/14 19:27:36  blepojevic
	      No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A
	
	      Revision 1.3  2008-11-14 19:27:36  gfinlay
	      - Added file header comment blocks for several files that were missing these.
	      - Added debug uart support with serial.c and serial.h

	      Revision 1.2  2007-08-01 20:20:48  nallyn
	      Added comment for Doxygen.

	      Revision 1.1  2007-07-05 20:10:11  gfinlay
	      Added baseline for new DCP2 driver control panel (CAN-based) that uses the PIC 18F6680.

	      Revision 1.2  2006/02/08 19:39:08  nallyn
	      Review Code with updated file headers
	
       \endif
*/
/******************************************************************************/

/*O 8 8 16 32  TX16FULL.H   LCDIcon FileDescriptor: Do not edit or move */
/* Put Your Comments Here */
//!The 16 byte/character font set
const sUnit16 suFont16[64] = 
{
{{'\0', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x01', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1B,    /*  ...@@.@@  */
(BYTE) 0x1B,    /*  ...@@.@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x02', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x3C,    /*  ..@@@@..  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x3C,    /*  ..@@@@..  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x03', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x40,    /*  .@......  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x40,    /*  .@......  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x40,    /*  .@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x04,    /*  .....@..  */
}}
,
{{'\x04', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xFF,    /*  @@@@@@@@  */
(BYTE) 0xFF,    /*  @@@@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xFF,    /*  @@@@@@@@  */
(BYTE) 0xFF,    /*  @@@@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x05', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x06,    /*  .....@@.  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x19,    /*  ...@@..@  */
(BYTE) 0x18,    /*  ...@@...  */
}}
,
{{'\x06', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x90,    /*  @..@....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0E,    /*  ....@@@.  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x13,    /*  ...@..@@  */
(BYTE) 0x0E,    /*  ....@@@.  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x11,    /*  ...@...@  */
}}
,
{{'\x07', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x3C,    /*  ..@@@@..  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x08', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x09', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x0a', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x0D,    /*  ....@@.@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0D,    /*  ....@@.@  */
(BYTE) 0x01,    /*  .......@  */
}}
,
{{'\x0b', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
}}
,
{{'\x0c', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x40,    /*  .@......  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x0d', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x01,    /*  .......@  */
}}
,
{{'\x0e', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x0f', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x0E,    /*  ....@@@.  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x10', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'\x11', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
}}
,
{{'\x12', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x1E,    /*  ...@@@@.  */
(BYTE) 0x13,    /*  ...@..@@  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{'\x13', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'\x14', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x02,    /*  ......@.  */
(BYTE) 0x12,    /*  ...@..@.  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x12,    /*  ...@..@.  */
}}
,
{{'\x15', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'\x16', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x98,    /*  @..@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'\x17', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x18', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'\x19', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x19,    /*  ...@@..@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x07,    /*  .....@@@  */
}}
,
{{'\x1a', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x1b', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x40,    /*  .@......  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x1c', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x1d', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x04,    /*  .....@..  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x1e', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\x1f', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1B,    /*  ...@@.@@  */
(BYTE) 0x1B,    /*  ...@@.@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{' ', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x48,    /*  .@..@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x13,    /*  ...@..@@  */
(BYTE) 0x12,    /*  ...@..@.  */
(BYTE) 0x13,    /*  ...@..@@  */
(BYTE) 0x01,    /*  .......@  */
}}
,
{{'!', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x02,    /*  ......@.  */
(BYTE) 0x02,    /*  ......@.  */
(BYTE) 0x02,    /*  ......@.  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
}}
,
{{'\"', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'#', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0C,    /*  ....@@..  */
}}
,
{{'$', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x07,    /*  .....@@@  */
}}
,
{{'%', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{'&', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'\'', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x19,    /*  ...@@..@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
}}
,
{{'(', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
}}
,
{{')', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'*', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'+', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{',', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{'-', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
}}
,
{{'.', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
}}
,
{{'/', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x07,    /*  .....@@@  */
}}
,
{{'0', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'1', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF0,    /*  @@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x7F,    /*  .@@@@@@@  */
(BYTE) 0x4F,    /*  .@..@@@@  */
}}
,
{{'2', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{'3', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x30,    /*  ..@@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0E,    /*  ....@@@.  */
}}
,
{{'4', 8, 16},{
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'5', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x0F,    /*  ....@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x0F,    /*  ....@@@@  */
}}
,
{{'6', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x03,    /*  ......@@  */
}}
,
{{'7', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x07,    /*  .....@@@  */
}}
,
{{'8', 8, 16},{
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x60,    /*  .@@.....  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x06,    /*  .....@@.  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x06,    /*  .....@@.  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x18,    /*  ...@@...  */
}}
,
{{'9', 8, 16},{
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{':', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x88,    /*  @...@...  */
(BYTE) 0xC8,    /*  @@..@...  */
(BYTE) 0x78,    /*  .@@@@...  */
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1C,    /*  ...@@@..  */
(BYTE) 0x1E,    /*  ...@@@@.  */
(BYTE) 0x13,    /*  ...@..@@  */
(BYTE) 0x11,    /*  ...@...@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{';', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'<', 8, 16},{
(BYTE) 0x38,    /*  ..@@@...  */
(BYTE) 0x70,    /*  .@@@....  */
(BYTE) 0xE0,    /*  @@@.....  */
(BYTE) 0xC0,    /*  @@......  */
(BYTE) 0x80,    /*  @.......  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x01,    /*  .......@  */
(BYTE) 0x03,    /*  ......@@  */
(BYTE) 0x07,    /*  .....@@@  */
(BYTE) 0x0E,    /*  ....@@@.  */
(BYTE) 0x1C,    /*  ...@@@..  */
}}
,
{{'=', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0x08,    /*  ....@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0xF8,    /*  @@@@@...  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x1F,    /*  ...@@@@@  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'>', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x06,    /*  .....@@.  */
(BYTE) 0x0C,    /*  ....@@..  */
(BYTE) 0x18,    /*  ...@@...  */
(BYTE) 0x10,    /*  ...@....  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
}}
,
{{'?', 8, 16},{
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x00,    /*  ........  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
(BYTE) 0x20,    /*  ..@.....  */
}}
};
