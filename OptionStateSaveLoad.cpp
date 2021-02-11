#include"OptionStateSaveLoad.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_OP.h"

void SaveOptionState(OP option) {
	FILE *fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"score/OptionState.dat", L"wb");
	if (error != 0) {
		printfDx(L"%s�̃I�[�v���Ɏ��s���܂���.\n", L"score/OptionState.dat");
		printfDx(L"�G���[�ԍ�:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwrite(&option, sizeof(option), 1, fp);
	fclose(fp);
	return;
}


void LoadOptionState(OPTION *option) {
	FILE *fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"score/OptionState.dat", L"rb");
	if (error != 0) {//�t�@�C��������Ȃ�������
		//�������Ȃ�
	}
	else {
		fread(&(option->op), sizeof(option->op), 1, fp);//�O��̃I�v�V�����ǂݍ���
		fclose(fp);
	}

	//�ُ�Ȓl�����߂�
	int i = 0;
	for (i = 0; i < option->OPTION_NUM; i++) {
		if (*(option->ArrayValue[i]) < 0 || *(option->ArrayValue[i]) > * (option->ArrayOptionNum[i]) - 1) {//�I�v�V�����̍ŏ��A�ő�l�𒴂��Ă�����0�ɂ���
			*option->ArrayValue[i] = 0;
		}

	}


	return;
}