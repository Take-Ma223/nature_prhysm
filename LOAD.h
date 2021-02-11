#include"STRUCT_LIST.h"
#include"STRUCT_SONG.h"
#include"STRUCT_SECRET.h"
#include"STRUCT_OP.h"
#include"STRUCT_CONFIG.h"

#ifndef _LOAD
#define _LOAD

extern void LOAD(LIST *, Song *, int *, SECRET *, SkillTestList *, OPTION *op, CONFIG config);
extern void SearchNoteSkin(OPTION *op);
extern void SearchHitSoundSkin(OPTION *op);

#endif