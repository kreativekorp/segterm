#ifndef __STAPP_CALENDAR_H__
#define __STAPP_CALENDAR_H__

// How long to press button 2 to exit.
#define CAL_LONG_PRESS   1200

// How long to display another month before going back to this month.
#define CAL_TODAY_AFTER 10000

bool calendar_setup();
bool calendar_loop();
void calendar_quit();

#endif
