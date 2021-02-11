#include"ScreenShot.h"
#include <time.h>
#include"DxLib.h"

void ScreenShot(int SH_SHUTTER_SIGNAL, int SH_SHUTTER) {//�ėp�X�N���[���V���b�g�֐�
	errno_t error;
	wchar_t ScreenShotFileName[256] = L"screenshot/SongSelect";//�X�N���[���V���b�g�̃t�@�C����
	time_t now;//���ݎ���(�O���j�b�W�W����)
	struct tm tm_now;//���ԍ\����

	now = time(NULL);//�����擾
	error = _localtime64_s(&tm_now, &now);

	swprintf_s(ScreenShotFileName, L"screenshot/ScreenShot_%d_%d_%d_%d_%d_%d.png", tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);//�p�X���܂񂾍ŏI�I�ȕۑ���

	PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
	SaveDrawScreenToPNG(0, 0, 1280, 720, ScreenShotFileName, 0);
	PlaySoundMem(SH_SHUTTER, DX_PLAYTYPE_BACK, TRUE);



}