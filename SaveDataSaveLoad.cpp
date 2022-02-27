#include"SaveDataSaveLoad.h"
#include"DxLib.h"
#include<sys/stat.h>

void writeSaveData(SAVEDATA saveData){
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/SaveData.dat", L"wb");
	if (error != 0) {
		printfDx(L"%s�̃I�[�v���Ɏ��s���܂���.\n", L"save_data/SaveData.dat");
		printfDx(L"�G���[�ԍ�:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&saveData, sizeof(saveData), 1, fp);
	fclose(fp);
	return;
}

int loadSaveData(SAVEDATA* saveData){
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/SaveData.dat", L"rb");
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
		return -1;
	}
	else {
		fread(saveData, sizeof(*saveData), 1, fp);//�O��̃I�v�V�����ǂݍ���
		fclose(fp);
		return 0;
	}


	return 0;
}
