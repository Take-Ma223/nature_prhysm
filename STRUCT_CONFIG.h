#ifndef _STRUCT_CONFIG
#define _STRUCT_CONFIG

enum SoundOutputType {
	DirectSound,
	WASAPI,
	ASIO
};

enum WasapiExclusive {
	NotExclusive,
	Exclusive
};

typedef struct Config {//�R���t�B�O�f�[�^
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
	int UsePy = 0;//util�t�H���_�̃v���O������py�t�@�C�����g�p����

	int SoundOutputType = 0;
	int WasapiExclusive = 0;
	int AsioDriver = 0;
	int Buffer = 256;
	int ComPort = 0;
	int Editable = 0;
} Config;

#endif