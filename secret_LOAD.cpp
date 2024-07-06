#include"secret_LOAD.h"
#include"STRUCT_RESULT.h"
#include"CONSTANT_VALUE.h"

int secret_LOAD(Song *Music, int song_number) {//隠し曲設定ファイルの読み込み
	//(隠し曲かどうかを見る) 隠し曲なら1を返す 通常曲は0を返す
	if (SECRET_NOT_USE == 1) {//隠し曲を最初から全解禁するなら
		return 0;//全て通常曲にする
	}

	wchar_t path[255];
	int fp;

	sprintfDx(path, L"%s/secret.dat", Music[song_number].FolderPath);

	wchar_t sharp1[255];
	wchar_t sharp2[255];


	fp = FileRead_open(path);
	if (fp == 0) {//無かったら戻る
		Music[song_number].secret = UnlockState::Normal;//隠し曲ではない
		return 0;
	}
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし


	wchar_t str[10];


	while (FileRead_gets(str, 10, fp) != -1) {//一行取得(\nは入らない)
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", sharp1, _countof(sharp1), sharp2, _countof(sharp2));
		if (wcscmp(L"#SECRET", sharp1) == 0) {
			if (wcscmp(L"1", sharp2) == 0) {//1なら隠し曲にする
				Music[song_number].secret = UnlockState::Secret;//隠し曲
				FileRead_close(fp);
				return 1;
			}
			else {
				Music[song_number].secret = UnlockState::Normal;//隠し曲ではない
				FileRead_close(fp);
				return 0;
			}
		}
	}

	FileRead_close(fp);
	return 0;//何も見つからなかったら通常曲に
}

void secret_LOAD_contents(Song *Music, int song_number, SECRET *secret) {//隠し曲設定ファイルの読み込み(内容を渡す)
	wchar_t path[255];
	int fp;

	sprintfDx(path, L"%s/secret.dat", Music[song_number].FolderPath);

	wchar_t sharp1[255];
	wchar_t sharp2[255];





	fp = FileRead_open(path);
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし

	wchar_t str[256];
	sharp2[0] = L'\0';

	sprintfDx(secret->BGM, L"\0");//専用BGMパスを初期化
	secret->beep_alarm = 0;

	while (FileRead_gets(str, 256, fp) != -1) {//一行取得(\nは入らない)
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", sharp1, _countof(sharp1), sharp2, _countof(sharp2));
		

		if (wcscmp(L"#COLOR", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら白
				secret->Color = GetColor(255, 255, 255);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				secret->Color = GetColor(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx("%d\n", Music[song_number].StrColor[difficulty]);
		}

		if (wcscmp(L"#SHADOWCOLOR", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら白
				secret->ShadowColor = GetColor(255, 255, 255);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				secret->ShadowColor = GetColor(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx("%d\n", Music[song_number].StrColor[difficulty]);
		}

		if (wcscmp(L"#ANNOUNCE", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルト警報文
				sprintfDx(secret->Announce, L"警報　大雨警報が発表されています。　厳重に警戒してください。");
			}
			else {
				sprintfDx(secret->Announce, L"%s", sharp2);
			}
			//printfDx("%d\n", Music[song_number].StrColor[difficulty]);
		}


		if (wcscmp(L"#BGM", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトBGM
				sprintfDx(secret->BGM, L"\0");
			}
			else {
				sprintfDx(secret->BGM, L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
			//printfDx("%d\n", Music[song_number].StrColor[difficulty]);
		}

		if (wcscmp(L"#ALARM", sharp1) == 0) {
			if (wcscmp(L"1", sharp2) == 0) {//1なら事前アラーム鳴らす
				secret->beep_alarm = 1;
			}
			else {//1以外,何も書かれていなかったら事前アラーム鳴らさない
				secret->beep_alarm = 0;
			}
			//printfDx("%d\n", Music[song_number].StrColor[difficulty]);
		}


		if (wcscmp(L"#END", sharp1) == 0) {
			break;
		}

		sharp2[0] = L'\0';

	}

	FileRead_close(fp);

	return;
}

void secret_save_LOAD(SECRET *secret,int secret_song_number, wchar_t *path) {//隠し曲のセーブファイル確認
	FILE *fp = 0;
	RESULT res;
	errno_t error = 0;
	wchar_t filename[255];

	wchar_t save_filename[8][17] = {
		L"result_sunny",
		L"result_cloudy",
		L"result_rainy",
		L"result_thunder",
		L"result_sunny_r",
		L"result_cloudy_r",
		L"result_rainy_r",
		L"result_thunder_r"
	};

	secret->get_song_number[secret_song_number] = 0;//デフォルトは非解禁
	//クリア済みのセーブファイルがあれば解禁済みにする

	for (int i = 0; i <= 7; i++) {
		sprintfDx(filename, L"%s/%s.dat", path, save_filename[i]);
		error = _wfopen_s(&fp, filename, L"rb");
		if (error != 0) {//スコアファイルが見つからなかった

		}
		else {
			fread(&res, sizeof(res), 1, fp);//セーブデータ読み込み
			//FAILEDでもPLAYでもないクリア状態を見つけた
			if (res.clear != 0 && res.clear != -1)secret->get_song_number[secret_song_number] = 1;//解禁済み;
			fclose(fp);
		}
	}

	return;
}

void secret_all_get(SECRET *secret) {//隠し曲が全解禁されているか確認
	int all_get = 1;
	for (int j = 0; j <= secret->total - 1; j++) {
		all_get = all_get && secret->get_song_number[j];
	}
	secret->all_get = all_get;//全解禁されていたらall_getは1

	return;
}