#ifndef _STRUCT_ANDROID_CONTROLLER
#define _STRUCT_ANDROID_CONTROLLER

typedef struct ANDROID_CONTROLLER {//��p�R���g���[��Android�ł̍\����
	int port = 4000;
	int NetUDPHandle;
	char StrBuf[256] = "0";
	char Android_Key[256];//�L�[�������(0:�����Ă��� 1:������Ă���)
	int Button[3][4];//�L�[�R���t�B�O�̔z��̐擪�A�h���X
} STRUCT_ANDROID_CONTROLLER;

#endif