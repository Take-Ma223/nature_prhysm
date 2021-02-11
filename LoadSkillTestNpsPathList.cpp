#include"LoadSkillTestNpsPathList.h"
#include"DxLib.h"
#include"ShowErrorName.h"

void LoadSkillTestNpsPathList(int SongNumberList[4], int DifficultyList[4], wchar_t *FileName, Song *Music,int NumberOfSongs) {
	wchar_t path[255];
	int fp;

	sprintfDx(path, L"skill_test/%s", FileName);

	wchar_t sharp1[255];
	wchar_t sharp2[255];
	wchar_t path_buf[4][255];


	fp = FileRead_open(path);
	if (fp == 0) {//無かったら戻る
		return;
	}
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし


	wchar_t str[256];

	while (FileRead_gets(str, 256, fp) != -1) {//一行取得(\nは入らない)
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", sharp1, _countof(sharp1), sharp2, _countof(sharp2));
		if (wcscmp(L"#SONG1", sharp1) == 0) {
			sprintfDx(path_buf[0], L"songs/%s", sharp2);
		}
		if (wcscmp(L"#SONG2", sharp1) == 0) {
			sprintfDx(path_buf[1], L"songs/%s", sharp2);
		}
		if (wcscmp(L"#SONG3", sharp1) == 0) {
			sprintfDx(path_buf[2], L"songs/%s", sharp2);
		}
		if (wcscmp(L"#SONG4", sharp1) == 0) {
			sprintfDx(path_buf[3], L"songs/%s", sharp2);
		}

		if (wcscmp(L"#DIFFICULTY1", sharp1) == 0) {
			DifficultyList[0] = _wtoi(sharp2);
		}
		if (wcscmp(L"#DIFFICULTY2", sharp1) == 0) {
			DifficultyList[1] = _wtoi(sharp2);
		}
		if (wcscmp(L"#DIFFICULTY3", sharp1) == 0) {
			DifficultyList[2] = _wtoi(sharp2);
		}
		if (wcscmp(L"#DIFFICULTY4", sharp1) == 0) {
			DifficultyList[3] = _wtoi(sharp2);
		}
		sharp2[0] = L'\0';
	}

	//曲番号を検索
	
	for (int i = 0; i <= 3; i++) {
		SongNumberList[i] = FindSongNumber(path_buf[i], Music, NumberOfSongs);
	}
	

	FileRead_close(fp);
	return;
}

int FindSongNumber(wchar_t *name, Song *Music, int NumberOfSongs) {
	int i = 0;
	for (i = 0; i <= NumberOfSongs - 1; i++) {
		if (wcscmp(name, Music[i].FolderPath) == 0) {//同じファイルパスを見つけた
			return i;
		}
	}
	return -1;
}


