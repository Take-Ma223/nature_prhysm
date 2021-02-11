#include"SongCacheSaveLoad.h"
#include"STRUCT_Song.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_OP.h"


void writeCacheToMusic(SongCache* Cache, Song* Music, int song_number, int difficulty) {
	int i = 0;
	Music[song_number].bpm_suggested[difficulty] = Cache->bpm_suggested;

	Music[song_number].total_note[difficulty] = Cache->total_note;

	for (i = 0; i < 9; i++) {
		Music[song_number].LocalNotesAmount[difficulty][i] = Cache->LocalNotesAmount[i];
		Music[song_number].ColorNotesAmount[difficulty][i] = Cache->ColorNotesAmount[i];
	}
	for (i = 0; i < 2; i++) {
		Music[song_number].global[i][difficulty] = Cache->global[i];//降水量(平均)
		Music[song_number].local[i][difficulty] = Cache->local[i];//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
		Music[song_number].color[i][difficulty] = Cache->color[i];//色変化度
		Music[song_number].longNote[i][difficulty] = Cache->longNote[i];//LN率
		Music[song_number].unstability[i][difficulty] = Cache->unstability[i];//大気不安定度(ソフラン度)
		Music[song_number].chain[i][difficulty] = Cache->chain[i];//縦連度
	}

	return;
}

void writeMusicToCache(SongCache *Cache, Song* Music, int song_number, int difficulty) {
	int i = 0;

	Cache->bpm_suggested = Music[song_number].bpm_suggested[difficulty];
	Cache->total_note = Music[song_number].total_note[difficulty];

	for (i = 0; i < 9; i++) {
		Cache->LocalNotesAmount[i] = Music[song_number].LocalNotesAmount[difficulty][i];
		Cache->ColorNotesAmount[i] = Music[song_number].ColorNotesAmount[difficulty][i];
	}
	for (i = 0; i < 2; i++) {
		Cache->global[i] = Music[song_number].global[i][difficulty];//降水量(平均)
		Cache->local[i] = Music[song_number].local[i][difficulty];//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
		Cache->color[i] = Music[song_number].color[i][difficulty];//色変化度
		Cache->longNote[i] = Music[song_number].longNote[i][difficulty];//LN率
		Cache->unstability[i] = Music[song_number].unstability[i][difficulty];//大気不安定度(ソフラン度)
		Cache->chain[i] = Music[song_number].chain[i][difficulty];//縦連度
	}
	
	return;
}

void SaveSongCache(SongCache Cache, Song *Music, int song_number, int difficulty) {
	FILE* fp = 0;
	errno_t error = 0;
	wchar_t str[256];

	sprintfDx(str, L"cache/%s", Music[song_number].KindFolder);
	CreateDirectory(str, NULL);//キャッシュ保存用の種類ディレクトリ(official)作成

	CreateDirectory(Music[song_number].CacheFolder, NULL);//キャッシュ保存用の曲ディレクトリ作成

	sprintfDx(str, L"%s/%d.dat", Music[song_number].CacheFolder, difficulty);

	error = _wfopen_s(&fp, str, L"wb");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", str);
		printfDx(L"エラー番号:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&Cache, sizeof(Cache), 1, fp);
	fclose(fp);
	return;
}


int LoadSongCache(SongCache* Cache, Song* Music, int song_number, int difficulty) {

	//Cache初期化
	Cache->writeTime.dwLowDateTime = 0;
	Cache->writeTime.dwHighDateTime = 0;
	int i = 0;
	Cache->total_note = 0;

	for (i = 0; i < 9; i++) {
		Cache->LocalNotesAmount[i] = 0;
		Cache->ColorNotesAmount[i] = 0;
	}
	for (i = 0; i < 2; i++) {
		Cache->global[i] = 0;//降水量(平均)
		Cache->local[i] = 0;//局所降水量(0~1に正規化された色重みづけ配置タイミングの標準偏差 小さいほど局所難)
		Cache->color[i] = 0;//色変化度
		Cache->longNote[i] = 0;//LN率
		Cache->unstability[i] = 0;//大気不安定度(ソフラン度)
		Cache->chain[i] = 0;//縦連度
	}



	FILE* fp = 0;
	errno_t error = 0;

	wchar_t str[256];
	sprintfDx(str, L"%s/%d.dat", Music[song_number].CacheFolder, difficulty);

	error = _wfopen_s(&fp, str, L"rb");
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
		return 1;
	}
	else {
		fread(Cache, sizeof(*Cache), 1, fp);//読み込み
		fclose(fp);
		return 0;
	}

	return 0;
}