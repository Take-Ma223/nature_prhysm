#include"STRUCT_LIST.h"
#include"STRUCT_Song.h"

#ifndef _FOLDER_INSERT
#define _FOLDER_INSERT

extern void folder_insert(LIST *song_folder, int song_number, int difficulty, Song *Music);
extern void folder_insert_season(LIST *song_folder, int song_number, Song *Music);
extern void folder_insert_radar(LIST* song_folder, int song_number, int difficulty, Song* Music);

extern void reset_folder_skill_radar(LIST* song_folder);
extern void folder_insert_skill_radar(LIST* song_folder, int song_number, int difficulty, FolderIndex index, int kind);

#endif