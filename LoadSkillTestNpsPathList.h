#include"STRUCT_Song.h"

#ifndef _LOAD_SKILL_TEST_NPS_PATH_LIST
#define _LOAD_SKILL_TEST_NPS_PATH_LIST

void LoadSkillTestNpsPathList(int SongNumberList[4], int DifficultyList[4], wchar_t *FileName, Song *Music, int NumberOfSongs);

int FindSongNumber(wchar_t *name, Song *Music, int NumberOfSongs);

#endif