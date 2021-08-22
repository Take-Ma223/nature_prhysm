#include"STRUCT_IR_SETTING.h"
#include"STRUCT_RESULT.h"
#include"STRUCT_Song.h"

#ifndef _IRSETTINGSAVELOAD
#define _IRSETTINGSAVELOAD


void SaveIRSetting(IR_SETTING ir);
void LoadIRSetting(IR_SETTING* ir);
void IRgetID();
void IRsave(wchar_t* npsPath, wchar_t* folderPath, RESULT res, int difficulty, int season, BOOL rainbow);
void IRsend(IR_SETTING* ir, wchar_t* npsPath, wchar_t* folderPath, int difficulty, int rainbow);
void IRview(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow);

#endif