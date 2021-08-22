#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_OP.h"
#include"STRUCT_IR_SETTING.h"

#ifndef _TITLE
#define _TITLE

extern void TITLE(int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, CONFIG config, OPTION* option, IR_SETTING* ir);
extern void OPEN_COVER(int, CONFIG);//使ってない
extern void CLOSE_COVER(int,CONFIG, OPTION*);

#endif