#include"STRUCT_LIST.h"
#include"STRUCT_SONG.h"
#include"STRUCT_SECRET.h"
#include"STRUCT_OP.h"
#include"STRUCT_CONFIG.h"
#include"STRUCT_IR_SETTING.h"

#ifndef _LOAD
#define _LOAD

extern void LOAD(LIST *, Song *, int *, SECRET *, SkillTestList *, OPTION *op, CONFIG config, IR_SETTING* ir);
extern void SearchThemeSkin(OPTION* op);
extern void SearchNoteSkin(OPTION *op);
extern void SearchHitSoundSkin(OPTION *op);

#endif