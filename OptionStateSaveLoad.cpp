#include"OptionStateSaveLoad.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_OP.h"

void SaveOptionState(OP option) {
	FILE *fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/OptionState.dat", L"wb");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", L"save_data/OptionState.dat");
		printfDx(L"エラー番号:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&option, sizeof(option), 1, fp);
	fclose(fp);
	return;
}


void LoadOptionState(Option *option) {
	FILE *fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/OptionState.dat", L"rb");
	if (error != 0) {//ファイル見つからなかったら
		//何もしない
	}
	else {
		fread(&(option->op), sizeof(option->op), 1, fp);//前回のオプション読み込み
		fclose(fp);
	}

	//異常な値を収める
	Option defaultOption;
	int i = 0;
	for (i = 0; i < option->OPTION_NUM; i++) {
		if (*(option->ArrayValue[i]) < 0 || *(option->ArrayValue[i]) > * (option->ArrayOptionNum[i]) - 1) {//オプションの最小、最大値を超えていたら0にする
			*option->ArrayValue[i] = *(defaultOption.ArrayValue[i]);
		}

	}


	return;
}