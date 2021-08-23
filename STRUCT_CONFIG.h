#ifndef _STRUCT_CONFIG
#define _STRUCT_CONFIG

typedef struct CONFIG {//�R���t�B�O�f�[�^
	int FullScreen = 0;
	int Vsync = 1;
	int VsyncOffsetCompensation = 1;
	int Fps = 60;
	int ShowFps = 0;
	int ShowStrShadow = 1;
	int UseHiPerformanceTimer = 1;
	int SongSelectRowNumber = 15;
	int DisplayTimingOffset = 0;//�f�B�X�v���C�̒x���␳�l
	int ShowDebug = 1;//�f�����[�h (1�̎��Ƀv���C��ʂ�DEBUG MODE���\������Ȃ��悤�ɂȂ�)
	int Local = 0;//�\�P�b�g�ʐM�̐ڑ���IP�A�h���X�����[�J���ɂ���
} CONFIG;

#endif