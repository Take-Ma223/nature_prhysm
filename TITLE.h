#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"
#ifndef _TITLE
#define _TITLE

extern void TITLE(int Button[3][4], int Button_Shutter, int *Key, char *Buf, ANDROID_CONTROLLER *AC, CONFIG config);
extern void OPEN_COVER(int, CONFIG);//使ってない
extern void CLOSE_COVER(int,CONFIG);

#endif