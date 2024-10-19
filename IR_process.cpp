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
		printfDx(L"%s�̃I�[�v���Ɏ��s���܂���.\n", L"save_data/IR_Setting.dat");
		printfDx(L"�G���[�ԍ�:%d", error);
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
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
	}
	else {
		fread(ir, sizeof(*ir), 1, fp);//�O��̃I�v�V�����ǂݍ���
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
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
	}
	else {
		fread(&targetScore, sizeof(targetScore), 1, fp);//TargetScore�ǂݍ���
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
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
	}
	else {
		fread(&ConnectState, sizeof(ConnectState), 1, fp);//ConnectState�ǂݍ���
		fclose(fp);
	}

	return ConnectState;
}


void IRgetID(Config config) {
	//����N������IR id�̎擾
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

	if (stat("save_data/IR_id.dat", &statBuf) == 0) {//�t�@�C�����݊m�F
		//_wsystem(passbuf);
	}
	else {
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;
		BOOL CPres = ::CreateProcessW(NULL, passbuf, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		printfDx(L"IR id���擾���Ă��܂�\n");
	}
}

void IRsave(wchar_t* npsPath, wchar_t* folderPath, RESULT res, int difficulty, int season, BOOL rainbow, BOOL onlyOption, Config config) {
	//���M�p�X�R�A�̕ۑ�
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
	WaitForSingleObject(pi.hProcess, INFINITE);//�I����҂�
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	printfDx(L"IR�p�X�R�A��ۑ����Ă��܂�\n");

}

void IRsend(IR_SETTING* ir, wchar_t* npsPath, wchar_t* folderPath, int difficulty, int rainbow, Config config) {
	if (ir->IR_Enable == FALSE)return;//IR�ɎQ�����Ȃ��ꍇ�͑��M���Ȃ�

	//�X�R�A�̑��M
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
	//�X�R�A�����L���O�̕\��
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
	//WaitForSingleObject(pi.hProcess, INFINITE);//�I����҂�
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void viewSkillRadarRanking(Config config) {
	//�X�L�����[�_�[�����L���O�̕\��

	if (config.Local == 1) {
		HINSTANCE ret = ShellExecute(NULL, L"open", L"http://nature-prhysm.main.jp/ranking/skill_radar_ranking.php?sort=robustness", NULL, NULL, SW_SHOW);

	}
	else {
		HINSTANCE ret = ShellExecute(NULL, L"open", L"http://nature-prhysm.main.jp/ranking/skill_radar_ranking.php?sort=robustness", NULL, NULL, SW_SHOW);

	}
}

void getTargetScore(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, int mode, int score, int rivalID, Config config) {
	//�^�[�Q�b�g�X�R�A�̎擾
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
	WaitForSingleObject(pi.hProcess, INFINITE);//�I����҂�
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

void connectionConfirm(Config config) {
	//�T�[�o�[�ڑ���Ԃ̎擾
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
	WaitForSingleObject(pi.hProcess, INFINITE);//�I����҂�
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}


void addRadarListItem(wchar_t* npsPath, int global, int local, int chain, int unstability, int streak, int color, Config config)
{
	//�T�[�o�[�ڑ���Ԃ̎擾
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
	WaitForSingleObject(pi.hProcess, INFINITE);//�I����҂�
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}