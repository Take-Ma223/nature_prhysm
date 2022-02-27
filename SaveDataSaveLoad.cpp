#include"SaveDataSaveLoad.h"
#include"DxLib.h"
#include<sys/stat.h>

void writeSaveData(SAVEDATA saveData){
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/SaveData.dat", L"wb");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", L"save_data/SaveData.dat");
		printfDx(L"エラー番号:%d", error);
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
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
		return -1;
	}
	else {
		fread(saveData, sizeof(*saveData), 1, fp);//前回のオプション読み込み
		fclose(fp);
		return 0;
	}


	return 0;
}
