#include"IR_process.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_IR_SETTING.h"
#include "CONSTANT_VALUE.h"
#include "STRUCT_RESULT.h"
#include "STRUCT_Song.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"

void SaveIRSetting(IR_SETTING ir) {
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/IR_Setting.dat", L"wb");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", L"save_data/IR_Setting.dat");
		printfDx(L"エラー番号:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&ir, sizeof(ir), 1, fp);
	fclose(fp);
	return;
}


void LoadIRSetting(IR_SETTING* ir) {
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/IR_Setting.dat", L"rb");
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
	}
	else {
		fread(ir, sizeof(*ir), 1, fp);//前回のオプション読み込み
		fclose(fp);
	}


	return;
}


void IRgetID(CONFIG config) {
	//初回起動時にIR idの取得
	struct stat statBuf;
	wchar_t passbuf[256];
	if (config.Local == 1) {
		sprintfDx(passbuf, L"python3 util/getid.py --local --run %s", RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"python3 util/getid.py --run %s", RUN_PASS);
	}

	if (stat("save_data/IR_id.dat", &statBuf) == 0) {
		//_wsystem(passbuf);
	}
	else {
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;
		BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		printfDx(L"IR idを取得しています\n");
	}
}

void IRsave(wchar_t* npsPath, wchar_t* folderPath, RESULT res, int difficulty, int season, BOOL rainbow, BOOL onlyOption, CONFIG config) {
	//送信用スコアの保存
	struct stat statBuf;
	wchar_t passbuf[512];
	if (config.Local == 1) {
		sprintfDx(passbuf, L"python3 util/save.py --local \"%s\" \"%s\" %d %d %s %s %d %d %d %d %d %d %d %d %d %d --run %s",
			npsPath, folderPath, difficulty, season, rainbow ? L"--rainbow" : L"", onlyOption ? L"--onlyOption" : L"", res.clear, res.rank, res.score, res.sky_perfect, res.perfect, res.good, res.miss, res.min_miss, res.max_combo, res.play_counter, RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"python3 util/save.py         \"%s\" \"%s\" %d %d %s %s %d %d %d %d %d %d %d %d %d %d --run %s",
			npsPath, folderPath, difficulty, season, rainbow ? L"--rainbow" : L"", onlyOption ? L"--onlyOption" : L"", res.clear, res.rank, res.score, res.sky_perfect, res.perfect, res.good, res.miss, res.min_miss, res.max_combo, res.play_counter, RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	printfDx(L"IR用スコアを保存しています\n");

}

void IRsend(IR_SETTING* ir, wchar_t* npsPath, wchar_t* folderPath, int difficulty, int rainbow, CONFIG config) {
	if (ir->IR_Enable == FALSE)return;//IRに参加しない場合は送信しない

	//スコアの送信
	struct stat statBuf;
	wchar_t passbuf[256];
	if (config.Local == 1) {
		sprintfDx(passbuf, L"python3 util/send.py --local \"%s\" \"%s/IRdata/%d%s\" --run %s", 
			npsPath, folderPath, difficulty, rainbow ? L"_r" : L"", RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"python3 util/send.py         \"%s\" \"%s/IRdata/%d%s\" --run %s",
			npsPath, folderPath, difficulty, rainbow ? L"_r" : L"", RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void IRview(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, CONFIG config) {
	//スコアの送信
	struct stat statBuf;
	wchar_t passbuf[256];
	if (config.Local == 1) {
		sprintfDx(passbuf, L"python3 util/view.py --local \"%s\" %s --run %s",
			npsPath, rainbow ? L"--rainbow" : L"", RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"python3 util/view.py         \"%s\" %s --run %s",
			npsPath, rainbow ? L"--rainbow" : L"", RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}