#include"ScreenShot.h"
#include <time.h>
#include"DxLib.h"

void ScreenShot(int SH_SHUTTER_SIGNAL, int SH_SHUTTER) {//汎用スクリーンショット関数
	errno_t error;
	wchar_t ScreenShotFileName[256] = L"screenshot/SongSelect";//スクリーンショットのファイル名
	time_t now;//現在時刻(グリニッジ標準時)
	struct tm tm_now;//時間構造体

	now = time(NULL);//時刻取得
	error = _localtime64_s(&tm_now, &now);

	swprintf_s(ScreenShotFileName, L"screenshot/ScreenShot_%d_%d_%d_%d_%d_%d.png", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);//パスを含んだ最終的な保存名

	PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
	SaveDrawScreenToPNG(0, 0, 1280, 720, ScreenShotFileName, 0);
	PlaySoundMem(SH_SHUTTER, DX_PLAYTYPE_BACK, TRUE);



}