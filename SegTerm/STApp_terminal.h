#ifndef __STAPP_TERMINAL_H__
#define __STAPP_TERMINAL_H__

// Chosen to prevent dropped characters from the host sending too quickly.
#define TERMINAL_BAUD_RATE 1200

bool terminal_setup();
bool terminal_loop();
void terminal_quit();

#endif
