#ifndef _STRUCT_ANDROID_CONTROLLER
#define _STRUCT_ANDROID_CONTROLLER

typedef struct ANDROID_CONTROLLER {//専用コントローラAndroid版の構造体
	int port = 4000;
	int NetUDPHandle;
	char StrBuf[256] = "0";
	char Android_Key[256];//キー押下状態(0:離している 1:押されている)
	int Button[3][4];//キーコンフィグの配列の先頭アドレス
} STRUCT_ANDROID_CONTROLLER;

#endif