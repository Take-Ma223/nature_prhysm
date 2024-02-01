#pragma once
#include "STRUCT_IR_SETTING.h"
#include "STRUCT_ANDROID_CONTROLLER.h"
#include "STRUCT_CONFIG.h"
#include "STRUCT_OP.h"

#ifndef _KEY_CONFIG
#define _KEY_CONFIG

extern void KEY_CONFIG(int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, Config config, Option* option, IR_SETTING* ir);


#endif