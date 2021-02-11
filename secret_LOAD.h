#include"DxLib.h"
#include"STRUCT_Song.h"
#include"STRUCT_SECRET.h"

#ifndef _SECRET_LOAD
#define _SECRET_LOAD

extern int secret_LOAD(Song *Music, int song_number);//隠し曲設定ファイルの読み込み
extern void secret_LOAD_contents(Song *Music, int song_number, SECRET *secret);//隠し曲設定ファイルの読み込み(内容を渡す)
extern void secret_save_LOAD(SECRET *secret, int secret_song_number, wchar_t *path);//隠し曲のセーブファイル確認
extern void secret_all_get(SECRET *secret);
#endif