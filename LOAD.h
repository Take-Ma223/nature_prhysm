#include"STRUCT_LIST.h"
#include"STRUCT_SONG.h"
#include"STRUCT_SECRET.h"
#include"STRUCT_OP.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_IR_SETTING.h"

#ifndef _LOAD
#define _LOAD

extern void LOAD(LIST *, Song *, int *, SECRET *, SkillTestList *, Option *op, Config config, IR_SETTING* ir);
extern void SearchThemeSkin(Option* op);
extern void SearchNoteSkin(Option *op);
extern void SearchHitSoundSkin(Option *op);
extern void SearchNoteTextSkin(Option* op);

vector<wstring> getSkinFolderList(wstring path);

#endif