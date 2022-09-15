/******************************************************************************/
/*!
<!-- FILE NAME: -->
       \file Events.c
       \brief Event generation and queuing

<!-- DESCRIPTION: -->

        This module is in charge of queuing all the evnets. Other modules only 
        need to call Add_Event and Get_Event routines. Everything else is internal 
        to this module.

       \if ShowHist
       Copyright © 2006 Teleflex Canada, Inc. All rights reserved.
       This source file and the information contained in it are confidential and
       proprietary to Teleflex Canada, Inc. The reproduction or disclosure, in
       whole or in part, to anyone outside of Teleflex without the written
       approval of a Teleflex officer under a Non-Disclosure Agreement is
       expressly prohibited.

       CHANGE HISTORY:
           $Header: /media/sf_Temp/CVSROOT/APU/DieselAPU/DCP3/Events.c,v 1.1 2015-06-30 20:41:22 blepojevic Exp $
  
           $Log: not supported by cvs2svn $
           Revision 1.1  2008/11/14 19:27:36  blepojevic
           No change - DCP3 software transition to GFI CVS server, added tag DCP3-07-02A

           Revision 1.4  2008-11-14 19:27:36  gfinlay
           - Added file header comment blocks for several files that were missing these.
           - Added debug uart support with serial.c and serial.h

         Revision 1.3 Fri Nov 30 18:52:07 2007 
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

         Revision 1.2 Wed Oct 24 15:09:30 2007 
         Removed left over test code. Compile options: UNIT_TEST & MEMORY_TESTING.

         Revision 1.1 Mon Aug 27 22:26:33 2007 
         *** empty log message ***

       \endif
*/
/******************************************************************************/

#include "Events.h"

static char Event_Buffer [EVENT_BUFFER_SIZE];
static char Event_Buffer_Newest;
static char Event_Buffer_Oldest;
// Function prototypes
static void Increment_Event_Buffer_Newest (void);
static void Increment_Event_Buffer_Oldest (void);


/******************************************************************************/
/*!

<!-- FUNCTION NAME: Increment_Event_Buffer_Newest -->
       \brief Increment the pointer pointing to where the new event must be entered
<!-- PURPOSE: -->
       Hndle circular event buffer pointers
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Increment_Event_Buffer_Newest (void)
{
	Event_Buffer_Newest++;
	if (Event_Buffer_Newest >= EVENT_BUFFER_SIZE)
		Event_Buffer_Newest = 0;

	if (Event_Buffer_Newest == Event_Buffer_Oldest)
		Increment_Event_Buffer_Oldest();
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Increment_Event_Buffer_Oldest -->
       \brief Increment the pointer pointing to where the oldest event is
<!-- PURPOSE: -->
       Hndle circular event buffer pointers
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
static void Increment_Event_Buffer_Oldest (void)
{
	Event_Buffer_Oldest++;
	if (Event_Buffer_Oldest >= EVENT_BUFFER_SIZE)
		Event_Buffer_Oldest = 0;
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Add_Event -->
       \brief Adds an event to the event buffer
<!-- PURPOSE: -->
       Hndle circular event buffer events
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return Void

*/
/******************************************************************************/
void Add_Event (unsigned char event)
{
	Event_Buffer[Event_Buffer_Newest] = event;
	Increment_Event_Buffer_Newest();
}

/******************************************************************************/
/*!

<!-- FUNCTION NAME: Get_Event -->
       \brief Retrieves an event from the event buffer
<!-- PURPOSE: -->
       Hndle circular event buffer events
<!-- PARAMETERS: none -->
<!-- RETURNS: -->
      \return unsigned char, this is either the event or a sign that there is no event

*/
/******************************************************************************/
unsigned char Get_Event (void)
{
	unsigned char event;

	if (Event_Buffer_Newest != Event_Buffer_Oldest)
	{
		event = Event_Buffer[Event_Buffer_Oldest];
		Increment_Event_Buffer_Oldest();
		return (event);
	}
	else
	{
		return (NO_EVENT);
	}
}

