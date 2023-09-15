//LED操作関数群

#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"LED_operation.h"
#include"STRUCT_NOTE.h"


void flash_LED(HANDLE hComm, NOTE note, STATE *LED_state, int i, DWORD *dwWritten, OVERLAPPED *ovl) {//コントローラのLEDを光らせる
	char str[5];
	char num[2];
	//_itoa(i, num, 10);
	sprintf_s(num, "%d", i);

	if (note.color_init == NoteColor::R) {//R
		LED_state->R[i] = 10;
	}
	if (note.color_init == NoteColor::R) {//G
		LED_state->G[i] = 10;
	}
	if (note.color_init == NoteColor::R) {//B
		LED_state->B[i] = 10;
	}

	strcpy_s(str, num);



	if (note.color_init == NoteColor::R) {
		strcat_s(str, "R");
	}
	if (note.color_init == NoteColor::G) {
		strcat_s(str, "G");
	}
	if (note.color_init == NoteColor::B) {
		strcat_s(str, "B");
	}
	if (note.color_init == NoteColor::Y) {
		strcat_s(str, "Y");
	}
	if (note.color_init == NoteColor::C) {
		strcat_s(str, "C");
	}
	if (note.color_init == NoteColor::M) {
		strcat_s(str, "M");
	}
	if (note.color_init == NoteColor::W) {
		strcat_s(str, "W");
	}
	if (note.color_init == NoteColor::K) {
		strcat_s(str, "K");
	}
	if (note.color_init == NoteColor::F) {
		strcat_s(str, "F");
	}

	if (note.group == 0) {//単ノート
		strcat_s(str, "0");
	}
	if (note.group == 1) {//LN始点
		strcat_s(str, "1");
	}
	if (note.group == 2) {//LN終点
		strcat_s(str, "2");
	}



	//(str, "\n");


	WriteFile(hComm, str, (DWORD)strlen(str), dwWritten, NULL);
}

void turn_off_LED(HANDLE hComm, STATE *LED_state, DWORD *dwWritten, OVERLAPPED *ovl) {//コントローラのLEDを消す
	char str[5];
	char num[2];
	int i = 0;
	int DataSize = 0;
	//COMSTAT Comstat;
	//DWORD dwErrorMask;

	for (i = 1; i <= 4; i++) {
		sprintf_s(num, "%d", i);
		strcpy_s(str, num);
		strcat_s(str, "R");
		strcat_s(str, "2");

		/*
		DataSize = strlen(str); //データのサイズ取得
		do {
			ClearCommError(hComm, &dwErrorMask, &Comstat);
			ProcessMessage();
		} while (
			((8 - Comstat.cbOutQue) <= DataSize) ||  // 送信バッファが送信データを書き込める容量だけあるかチェック
														 //　　　MAX：送信バッファーサイズ
			(Comstat.fCtsHold == FALSE)                 // CTSがtrue（相手側が受信不可）なので送信不可　
														 //     （ハードウェアフロー制御をおこなう場合）
			);    // 相手側が受信可能でまた送信バッファーもデータにサイズに対して書き込める容量が空いている場合
				 // ループを抜ける
		*/




		WriteFile(hComm, str, (DWORD)strlen(str), dwWritten, NULL);
	}
}
