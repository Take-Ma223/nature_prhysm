#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_ANDROID_CONTROLLER.h"

void Get_Key_State(char *Buf, int *Key, ANDROID_CONTROLLER *AC) {//Key[]にキーの押下状態を入れる
	int i;
	
	GetHitKeyStateAll(Buf);//Bufにキーの押下状態が入る(離していたら全て0)

	int row = 0;
	int column = 0;
	int push = 0;

	int res = 0;
	while (CheckNetWorkRecvUDP(AC->NetUDPHandle) == TRUE) {//キーの押下情報が溜まっているなら全て読み込む
		res = NetWorkRecvUDP(AC->NetUDPHandle, NULL, &(AC->port), AC->StrBuf, 256, FALSE);
		//printfDx("Str:%c%c%c", AC->StrBuf[0], AC->StrBuf[1], AC->StrBuf[2]);
		if (AC->StrBuf[0] == 'R') {
			row = 2;
		}
		else if (AC->StrBuf[0] == 'G') {
			row = 1;
		}
		else if (AC->StrBuf[0] == 'B') {
			row = 0;
		}
		wchar_t str[3];

		//アスキーコードを数値に変換
		sprintfDx(str, L"%c", AC->StrBuf[1]);
		column = _wtoi(str);
		sprintfDx(str, L"%c", AC->StrBuf[2]);
		push = _wtoi(str);


		AC->Android_Key[AC->Button[row][column]] = push;

	}

	for (int i = 0; i <= 2; i++) {
		for (int j = 0; j <= 3; j++) {
			if (AC->Android_Key[AC->Button[i][j]] == 1) {//Android側で押されている状態だったら
				Buf[AC->Button[i][j]] = 1;//こちら側でも押している状態にする
			}
		}
	}

	


	for (i = 0; i <= 255; i++) {
		if (Buf[i] == 0) {//今は押されていない
			if (Key[i] <= 0)Key[i] = 0;//離され続けてる
			if (Key[i] >  0)Key[i] = -1;//離された瞬間
		}
		else {//今は押されている
			Key[i]++;//
		}
	}

	return;
}