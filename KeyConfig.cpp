#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"KeyConfig.h"

void KeyConfig(int Button[3][4],int *Button_Shutter) {
	int fp;
	wchar_t ButtonName[64];
	wchar_t KeyName[16];

	//_wfopen_s(&fp, L"KeyConfig.dat", L"r");

	fp = FileRead_open(L"save_data/KeyConfig.dat");
	if (fp == 0) {//無かったら戻る
		return;
	}
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし

	wchar_t str[16];
	while (FileRead_gets(str, 16, fp) != -1){//一行取得
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", ButtonName, _countof(ButtonName), KeyName, _countof(KeyName));
		/*
		printfDx("%s\n",ButtonName);
		ScreenFlip();
		Sleep(10);
		*/

		if (wcscmp(ButtonName, L"R1") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[2][0] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"R2") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[2][1] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"R3") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[2][2] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"R4") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[2][3] = KeyNameToInt(KeyName);
			}
		}

		if (wcscmp(ButtonName, L"G1") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[1][0] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"G2") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[1][1] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"G3") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[1][2] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"G4") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[1][3] = KeyNameToInt(KeyName);
			}
		}

		if (wcscmp(ButtonName, L"B1") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[0][0] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"B2") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[0][1] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"B3") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[0][2] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"B4") == 0) {
			if (KeyNameToInt(KeyName) != -1) {
				Button[0][3] = KeyNameToInt(KeyName);
			}
		}
		if (wcscmp(ButtonName, L"ScreenShot") == 0) {//廃止
			if (KeyNameToInt(KeyName) != -1) {
				*Button_Shutter = KeyNameToInt(KeyName);
			}
		}
		*Button_Shutter = KEY_INPUT_SYSRQ;//スクショはPrtScキー
	}
	FileRead_close(fp);
}

int KeyNameToInt(wchar_t *KeyName) {
	if (wcscmp(KeyName, L"BACK") == 0)return KEY_INPUT_BACK;    	// バックスペースキー
	if (wcscmp(KeyName, L"TAB") == 0)return KEY_INPUT_TAB; 	 // タブキー
	if (wcscmp(KeyName, L"RETURN") == 0)return KEY_INPUT_RETURN; 	 // エンターキー

	if (wcscmp(KeyName, L"LSHIFT") == 0)return KEY_INPUT_LSHIFT; 	// 左シフトキー
	if (wcscmp(KeyName, L"RSHIFT") == 0)return KEY_INPUT_RSHIFT; 	// 右シフトキー
	if (wcscmp(KeyName, L"LCONTROL") == 0)return KEY_INPUT_LCONTROL; 	// 左コントロールキー
	if (wcscmp(KeyName, L"RCONTROL") == 0)return KEY_INPUT_RCONTROL; 	// 右コントロールキー
	if (wcscmp(KeyName, L"ESCAPE") == 0)return KEY_INPUT_ESCAPE; 	// エスケープキー
	if (wcscmp(KeyName, L"SPACE") == 0)return KEY_INPUT_SPACE; 	// スペースキー
	if (wcscmp(KeyName, L"PGUP") == 0)return KEY_INPUT_PGUP; 	// ＰａｇｅＵＰキー
	if (wcscmp(KeyName, L"PGDN") == 0)return KEY_INPUT_PGDN; 	// ＰａｇｅＤｏｗｎキー
	if (wcscmp(KeyName, L"END") == 0)return KEY_INPUT_END; 	// エンドキー
	if (wcscmp(KeyName, L"HOME") == 0)return KEY_INPUT_HOME; 	// ホームキー
	if (wcscmp(KeyName, L"LEFT") == 0)return KEY_INPUT_LEFT; 	// 左キー
	if (wcscmp(KeyName, L"UP") == 0)return KEY_INPUT_UP;     	// 上キー
	if (wcscmp(KeyName, L"RIGHT") == 0)return KEY_INPUT_RIGHT; 	// 右キー
	if (wcscmp(KeyName, L"DOWN") == 0)return KEY_INPUT_DOWN;	// 下キー
	if (wcscmp(KeyName, L"INSERT") == 0)return KEY_INPUT_INSERT; 	// インサートキー
	if (wcscmp(KeyName, L"DELETE") == 0)return KEY_INPUT_DELETE;	// デリートキー

	if (wcscmp(KeyName, L"MINUS") == 0)return KEY_INPUT_MINUS;	// －キー
	if (wcscmp(KeyName, L"YEN") == 0)return KEY_INPUT_YEN;     // ￥キー
	if (wcscmp(KeyName, L"PREVTRACK") == 0)return KEY_INPUT_PREVTRACK;	// ＾キー
	if (wcscmp(KeyName, L"PERIOD") == 0)return KEY_INPUT_PERIOD;  // ．キー
	if (wcscmp(KeyName, L"SLASH") == 0)return KEY_INPUT_SLASH;	// ／キー
	if (wcscmp(KeyName, L"LALT") == 0)return KEY_INPUT_LALT;    // 左ＡＬＴキー
	if (wcscmp(KeyName, L"RALT") == 0)return KEY_INPUT_RALT;	// 右ＡＬＴキー
	if (wcscmp(KeyName, L"SCROLL") == 0)return KEY_INPUT_SCROLL; 	// ScrollLockキー
	if (wcscmp(KeyName, L"SEMICOLON") == 0)return KEY_INPUT_SEMICOLON; 	// ；キー
	if (wcscmp(KeyName, L"COLON") == 0)return KEY_INPUT_COLON;   // ：キー
	if (wcscmp(KeyName, L"LBRACKET") == 0)return KEY_INPUT_LBRACKET; 	// ［キー
	if (wcscmp(KeyName, L"RBRACKET") == 0)return KEY_INPUT_RBRACKET;	// ］キー
	if (wcscmp(KeyName, L"AT") == 0)return KEY_INPUT_AT;      // ＠キー
	if (wcscmp(KeyName, L"BACKSLASH") == 0)return KEY_INPUT_BACKSLASH; 	// ＼キー
	if (wcscmp(KeyName, L"COMMA") == 0)return KEY_INPUT_COMMA;// ，キー
	if (wcscmp(KeyName, L"CAPSLOCK") == 0)return KEY_INPUT_CAPSLOCK; 	// CaspLockキー
	if (wcscmp(KeyName, L"PAUSE") == 0)return KEY_INPUT_PAUSE;// PauseBreakキー

	if (wcscmp(KeyName, L"NUMPAD0") == 0)return KEY_INPUT_NUMPAD0;  // テンキー０
	if (wcscmp(KeyName, L"NUMPAD1") == 0)return KEY_INPUT_NUMPAD1;	 // テンキー１
	if (wcscmp(KeyName, L"NUMPAD2") == 0)return KEY_INPUT_NUMPAD2;	 // テンキー２
	if (wcscmp(KeyName, L"NUMPAD3") == 0)return KEY_INPUT_NUMPAD3;	 // テンキー３
	if (wcscmp(KeyName, L"NUMPAD4") == 0)return KEY_INPUT_NUMPAD4;	 // テンキー４
	if (wcscmp(KeyName, L"NUMPAD5") == 0)return KEY_INPUT_NUMPAD5;	 // テンキー５
	if (wcscmp(KeyName, L"NUMPAD6") == 0)return KEY_INPUT_NUMPAD6;	 // テンキー６
	if (wcscmp(KeyName, L"NUMPAD7") == 0)return KEY_INPUT_NUMPAD7;	 // テンキー７
	if (wcscmp(KeyName, L"NUMPAD8") == 0)return KEY_INPUT_NUMPAD8;	 // テンキー８
	if (wcscmp(KeyName, L"NUMPAD9") == 0)return KEY_INPUT_NUMPAD9;	 // テンキー９
	if (wcscmp(KeyName, L"MULTIPLY") == 0)return KEY_INPUT_MULTIPLY; 	// テンキー＊キー
	if (wcscmp(KeyName, L"ADD") == 0)return KEY_INPUT_ADD;      // テンキー＋キー
	if (wcscmp(KeyName, L"SUBTRACT") == 0)return KEY_INPUT_SUBTRACT; 	// テンキー－キー
	if (wcscmp(KeyName, L"DECIMAL") == 0)return KEY_INPUT_DECIMAL;   // テンキー．キー
	if (wcscmp(KeyName, L"DIVIDE") == 0)return KEY_INPUT_DIVIDE;	  // テンキー／キー
	if (wcscmp(KeyName, L"NUMPADENTER") == 0)return KEY_INPUT_NUMPADENTER;	// テンキーのエンターキー

	if (wcscmp(KeyName, L"F1") == 0)return KEY_INPUT_F1; 	// Ｆ１キー
	if (wcscmp(KeyName, L"F2") == 0)return KEY_INPUT_F2;	// Ｆ２キー
	if (wcscmp(KeyName, L"F3") == 0)return KEY_INPUT_F3;	// Ｆ３キー
	if (wcscmp(KeyName, L"F4") == 0)return KEY_INPUT_F4;	// Ｆ４キー
	if (wcscmp(KeyName, L"F5") == 0)return KEY_INPUT_F5;	// Ｆ５キー
	if (wcscmp(KeyName, L"F6") == 0)return KEY_INPUT_F6;	// Ｆ６キー
	if (wcscmp(KeyName, L"F7") == 0)return KEY_INPUT_F7;	// Ｆ７キー
	if (wcscmp(KeyName, L"F8") == 0)return KEY_INPUT_F8;	// Ｆ８キー
	if (wcscmp(KeyName, L"F9") == 0)return KEY_INPUT_F9;	// Ｆ９キー
	if (wcscmp(KeyName, L"F10") == 0)return KEY_INPUT_F10; 	// Ｆ１０キー
	if (wcscmp(KeyName, L"F11") == 0)return KEY_INPUT_F11; 	// Ｆ１１キー
	if (wcscmp(KeyName, L"F12") == 0)return KEY_INPUT_F12; 	// Ｆ１２キー

	if (wcscmp(KeyName, L"A") == 0)return KEY_INPUT_A; 	// Ａキー
	if (wcscmp(KeyName, L"B") == 0)return KEY_INPUT_B;	// Ｂキー
	if (wcscmp(KeyName, L"C") == 0)return KEY_INPUT_C;	// Ｃキー
	if (wcscmp(KeyName, L"D") == 0)return KEY_INPUT_D;	// Ｄキー
	if (wcscmp(KeyName, L"E") == 0)return KEY_INPUT_E;	// Ｅキー
	if (wcscmp(KeyName, L"F") == 0)return KEY_INPUT_F;	// Ｆキー
	if (wcscmp(KeyName, L"G") == 0)return KEY_INPUT_G;	// Ｇキー
	if (wcscmp(KeyName, L"H") == 0)return KEY_INPUT_H;	// Ｈキー
	if (wcscmp(KeyName, L"I") == 0)return KEY_INPUT_I;	// Ｉキー
	if (wcscmp(KeyName, L"J") == 0)return KEY_INPUT_J;	// Ｊキー
	if (wcscmp(KeyName, L"K") == 0)return KEY_INPUT_K;	// Ｋキー
	if (wcscmp(KeyName, L"L") == 0)return KEY_INPUT_L;	// Ｌキー
	if (wcscmp(KeyName, L"M") == 0)return KEY_INPUT_M;	// Ｍキー
	if (wcscmp(KeyName, L"N") == 0)return KEY_INPUT_N;	// Ｎキー
	if (wcscmp(KeyName, L"O") == 0)return KEY_INPUT_O;	// Ｏキー
	if (wcscmp(KeyName, L"P") == 0)return KEY_INPUT_P;	// Ｐキー
	if (wcscmp(KeyName, L"Q") == 0)return KEY_INPUT_Q;	// Ｑキー
	if (wcscmp(KeyName, L"R") == 0)return KEY_INPUT_R;	// Ｒキー
	if (wcscmp(KeyName, L"S") == 0)return KEY_INPUT_S;	// Ｓキー
	if (wcscmp(KeyName, L"T") == 0)return KEY_INPUT_T;	// Ｔキー
	if (wcscmp(KeyName, L"U") == 0)return KEY_INPUT_U;	// Ｕキー
	if (wcscmp(KeyName, L"V") == 0)return KEY_INPUT_V;	// Ｖキー
	if (wcscmp(KeyName, L"W") == 0)return KEY_INPUT_W;	// Ｗキー
	if (wcscmp(KeyName, L"X") == 0)return KEY_INPUT_X;	// Ｘキー
	if (wcscmp(KeyName, L"Y") == 0)return KEY_INPUT_Y;	// Ｙキー
	if (wcscmp(KeyName, L"Z") == 0)return KEY_INPUT_Z;	// Ｚキー
	if (wcscmp(KeyName, L"0") == 0)return KEY_INPUT_0; 	// ０キー
	if (wcscmp(KeyName, L"1") == 0)return KEY_INPUT_1;	// １キー
	if (wcscmp(KeyName, L"2") == 0)return KEY_INPUT_2;	// ２キー
	if (wcscmp(KeyName, L"3") == 0)return KEY_INPUT_3;	// ３キー
	if (wcscmp(KeyName, L"4") == 0)return KEY_INPUT_4;	// ４キー
	if (wcscmp(KeyName, L"5") == 0)return KEY_INPUT_5;	// ５キー
	if (wcscmp(KeyName, L"6") == 0)return KEY_INPUT_6;	// ６キー
	if (wcscmp(KeyName, L"7") == 0)return KEY_INPUT_7;	// ７キー
	if (wcscmp(KeyName, L"8") == 0)return KEY_INPUT_8;	// ８キー
	if (wcscmp(KeyName, L"9") == 0)return KEY_INPUT_9;	// ９キー

	return -1;//どれにも当てはまらなかったからエラー
}