#include"STRUCT_ANDROID_CONTROLLER.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_OP.h"
#include"STRUCT_IR_SETTING.h"
#include"STRUCT_Song.h"

#ifndef _TITLE
#define _TITLE

extern void TITLE(Song* Music, int* NumberOfSongs, int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, Config config, Option* option, IR_SETTING* ir);
extern void OPEN_COVER(int, Config);//使ってない
extern void CLOSE_COVER(int,Config, Option*);

#endif