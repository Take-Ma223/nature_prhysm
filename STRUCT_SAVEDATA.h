#ifndef _SAVEDATA
#define _SAVEDATA

typedef struct SAVEDATA {//�Z�[�u�f�[�^�\����
	unsigned int totalBootCount = 1;//���N����
	unsigned int totalPlayCount = 0;//���v���C�Ȑ�
	unsigned int totalHighScore = 0;//���n�C�X�R�A(���K)
	unsigned int totalHighScoreRainbow = 0;//���n�C�X�R�A(��)
	unsigned int totalHitNotes = 0;//�@���������̐�
}SAVEDATA;

#endif