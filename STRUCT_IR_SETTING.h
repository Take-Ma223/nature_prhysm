//�C���^�[�l�b�g�����L���O�ݒ�\����
#include"DxLib.h"
#ifndef _STRUCT_IR_SETTING
#define _STRUCT_IR_SETTING

typedef struct IR_SETTING {//bpm_change
	wchar_t plaeyerName[17] = L"player";//�v���C���[��
	BOOL IR_Enable = TRUE;//IR���g�����ǂ���
	unsigned int rivalID = 0;//���C�o��ID
}IR_SETTING;


#endif