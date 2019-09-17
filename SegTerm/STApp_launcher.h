#ifndef __STAPP_LAUNCHER_H__
#define __STAPP_LAUNCHER_H__

#define APPNAMES static const char * appName(int i) { switch (i) {
#define APPNAME(i,n) case i: return n;
#define ENDNAMES default: return (const char *)0; } }

#define REGAPPS static void runApp(int i) { switch (i) {
#define REGAPP(i,n) case i: if (n##_setup()) { while (n##_loop()); n##_quit(); } break;
#define ENDAPPS } }

bool launcher_setup();
bool launcher_loop();
void launcher_quit();

#endif
