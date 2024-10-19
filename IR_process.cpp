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


int LoadTargetScore(wchar_t* saveFolderPass) {
	int targetScore = 0;
	FILE* fp = 0;
	errno_t error = 0;

	wchar_t passStr[512] = L"";
	sprintfDx(passStr, L"%s/IRdata/TargetScore", saveFolderPass);


	error = _wfopen_s(&fp, passStr, L"rb");
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
	}
	else {
		fread(&targetScore, sizeof(targetScore), 1, fp);//TargetScore読み込み
		fclose(fp);
	}

	return targetScore;
}

int LoadConnectionState() {
	int ConnectState = 0;
	FILE* fp = 0;
	errno_t error = 0;

	wchar_t passStr[512] = L"save_data/tmp/ConnectState";

	error = _wfopen_s(&fp, passStr, L"rb");
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
	}
	else {
		fread(&ConnectState, sizeof(ConnectState), 1, fp);//ConnectState読み込み
		fclose(fp);
	}

	return ConnectState;
}


void IRgetID(Config config) {
	//初回起動時にIR idの取得
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/getid.exe",
	L"python3 util/getid.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local --run %s", command[config.UsePy], RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         --run %s", command[config.UsePy], RUN_PASS);
	}

	if (stat("save_data/IR_id.dat", &statBuf) == 0) {//ファイル存在確認
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

void IRsave(wchar_t* npsPath, wchar_t* folderPath, RESULT res, int difficulty, int season, BOOL rainbow, BOOL onlyOption, Config config) {
	//送信用スコアの保存
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/save.exe",
	L"python3 util/save.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local \"%s\" \"%s\" %d %d %s %s %d %d %d %d %d %d %d %d %d %d --run %s", command[config.UsePy],
			npsPath, folderPath, difficulty, season, rainbow ? L"--rainbow" : L"", onlyOption ? L"--onlyOption" : L"", res.clear, res.rank, res.score, res.sky_perfect, res.perfect, res.good, res.miss, res.min_miss, res.max_combo, res.play_count, RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         \"%s\" \"%s\" %d %d %s %s %d %d %d %d %d %d %d %d %d %d --run %s", command[config.UsePy],
			npsPath, folderPath, difficulty, season, rainbow ? L"--rainbow" : L"", onlyOption ? L"--onlyOption" : L"", res.clear, res.rank, res.score, res.sky_perfect, res.perfect, res.good, res.miss, res.min_miss, res.max_combo, res.play_count, RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	printfDx(L"IR用スコアを保存しています\n");

}

void IRsend(IR_SETTING* ir, wchar_t* npsPath, wchar_t* folderPath, int difficulty, int rainbow, Config config) {
	if (ir->IR_Enable == FALSE)return;//IRに参加しない場合は送信しない

	//スコアの送信
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/send.exe",
	L"python3 util/send.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local \"%s\" \"%s/IRdata/%d%s\" --run %s", command[config.UsePy],
			npsPath, folderPath, difficulty, rainbow ? L"_r" : L"", RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         \"%s\" \"%s/IRdata/%d%s\" --run %s", command[config.UsePy],
			npsPath, folderPath, difficulty, rainbow ? L"_r" : L"", RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void IRview(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, Config config) {
	//スコアランキングの表示
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/view.exe",
	L"python3 util/view.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local \"%s\" %s --run %s", command[config.UsePy],
			npsPath, rainbow ? L"--rainbow" : L"", RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         \"%s\" %s --run %s", command[config.UsePy],
			npsPath, rainbow ? L"--rainbow" : L"", RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	//WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void viewSkillRadarRanking(Config config) {
	//スキルレーダーランキングの表示

	if (config.Local == 1) {
		HINSTANCE ret = ShellExecute(NULL, L"open", L"http://nature-prhysm.main.jp/ranking/skill_radar_ranking.php?sort=robustness", NULL, NULL, SW_SHOW);

	}
	else {
		HINSTANCE ret = ShellExecute(NULL, L"open", L"http://nature-prhysm.main.jp/ranking/skill_radar_ranking.php?sort=robustness", NULL, NULL, SW_SHOW);

	}
}

void getTargetScore(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, int mode, int score, int rivalID, Config config) {
	//ターゲットスコアの取得
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/getTargetScore.exe",
	L"python3 util/getTargetScore.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local \"%s\" \"%s\" %s %d %d %d --run %s", command[config.UsePy],
			npsPath, folderPath, rainbow ? L"--rainbow" : L"", mode, score, rivalID, RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         \"%s\" \"%s\" %s %d %d %d --run %s", command[config.UsePy],
			npsPath, folderPath, rainbow ? L"--rainbow" : L"", mode, score, rivalID, RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void connectionConfirm(Config config) {
	//サーバー接続状態の取得
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = { 
	L"util/connectionConfirm.exe",
	L"python3 util/connectionConfirm.py"
	};

	if (config.Local == 1) {
		sprintfDx(passbuf, L"%s --local --run %s", command[config.UsePy], RUN_PASS);
	}
	else {
		sprintfDx(passbuf, L"%s         --run %s", command[config.UsePy], RUN_PASS);
	}

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}


void addRadarListItem(wchar_t* npsPath, int global, int local, int chain, int unstability, int streak, int color, Config config)
{
	//サーバー接続状態の取得
	struct stat statBuf;
	wchar_t passbuf[512] = L"";

	wchar_t* command[2] = {
	L"util/add_radar_list_item.exe",
	L"python3 util/add_radar_list_item.py"
	};


	sprintfDx(passbuf, L"%s \"%s\" %d %d %d %d %d %d --run %s", command[config.UsePy], 
		npsPath,
		global,
		local,
		chain,
		unstability,
		streak,
		color,
		RUN_PASS);
	

	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);//終了を待つ
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}