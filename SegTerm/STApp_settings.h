#ifndef __STAPP_SETTINGS_H__
#define __STAPP_SETTINGS_H__

#define SETTING_BRIGHTNESS  0
#define SETTING_FONT        1
#define SETTING_LOWERCASE   2
#define SETTING_DOT_MODE    3
#define SETTING_LOCAL_ECHO  4
#define SETTING_CURSOR      5
#define SETTING_DATE        6
#define SETTING_TIME        7
#define SETTING_FIRST_DAY   8
#define SETTING_ABOUT       9
#define SETTING_MAX        10

#define SETTING_ACTION_NOOP        0
#define SETTING_ACTION_REDRAW_ITEM 1
#define SETTING_ACTION_REDRAW_MENU 2
#define SETTING_ACTION_EXIT        3

bool settings_setup();
bool settings_loop();
void settings_quit();

#endif
