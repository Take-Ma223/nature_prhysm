#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"KeyConfig.h"

void KeyConfigLoad(int Button[3][4], int* Button_Shutter) {
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
	while (FileRead_gets(str, 16, fp) != -1) {//一行取得
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

void KeyConfigSave(int Button[3][4], int* Button_Shutter) {
	FILE* fp = 0;
	errno_t error = 0;

	error = _wfopen_s(&fp, L"save_data/KeyConfig.dat", L"w,ccs=UNICODE");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", L"save_data/KeyConfig.dat");
		printfDx(L"エラー番号:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}

	fwprintf_s(fp, L"R1:%s\n", IntToKeyName(Button[2][0]));
	fwprintf_s(fp, L"R2:%s\n", IntToKeyName(Button[2][1]));
	fwprintf_s(fp, L"R3:%s\n", IntToKeyName(Button[2][2]));
	fwprintf_s(fp, L"R4:%s\n", IntToKeyName(Button[2][3]));
	fwprintf_s(fp, L"G1:%s\n", IntToKeyName(Button[1][0]));
	fwprintf_s(fp, L"G2:%s\n", IntToKeyName(Button[1][1]));
	fwprintf_s(fp, L"G3:%s\n", IntToKeyName(Button[1][2]));
	fwprintf_s(fp, L"G4:%s\n", IntToKeyName(Button[1][3]));
	fwprintf_s(fp, L"B1:%s\n", IntToKeyName(Button[0][0]));
	fwprintf_s(fp, L"B2:%s\n", IntToKeyName(Button[0][1]));
	fwprintf_s(fp, L"B3:%s\n", IntToKeyName(Button[0][2]));
	fwprintf_s(fp, L"B4:%s\n", IntToKeyName(Button[0][3]));
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"設定したいキーを下からコピー&ペーストして書き換えて下さい\n");
	fwprintf_s(fp, L"BACK        //バックスペースキー\n");
	fwprintf_s(fp, L"TAB         //タブキー\n");
	fwprintf_s(fp, L"RETURN      //エンターキー\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"LSHIFT      //左シフトキー\n");
	fwprintf_s(fp, L"RSHIFT      //右シフトキー\n");
	fwprintf_s(fp, L"LCONTROL    //左コントロールキー\n");
	fwprintf_s(fp, L"RCONTROL    //右コントロールキー\n");
	fwprintf_s(fp, L"ESCAPE      //エスケープキー\n");
	fwprintf_s(fp, L"SPACE       //スペースキー\n");
	fwprintf_s(fp, L"PGUP        //ＰａｇｅＵＰキー\n");
	fwprintf_s(fp, L"PGDN        //ＰａｇｅＤｏｗｎキー\n");
	fwprintf_s(fp, L"END         //エンドキー\n");
	fwprintf_s(fp, L"HOME        //ホームキー\n");
	fwprintf_s(fp, L"LEFT        //左キー\n");
	fwprintf_s(fp, L"UP          //上キー\n");
	fwprintf_s(fp, L"RIGHT       //右キー\n");
	fwprintf_s(fp, L"DOWN        //下キー\n");
	fwprintf_s(fp, L"INSERT      //インサートキー\n");
	fwprintf_s(fp, L"DELETE      //デリートキー\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"MINUS       //－キー\n");
	fwprintf_s(fp, L"YEN         //￥キー\n");
	fwprintf_s(fp, L"PREVTRACK   //＾キー\n");
	fwprintf_s(fp, L"PERIOD      //．キー\n");
	fwprintf_s(fp, L"SLASH       //／キー\n");
	fwprintf_s(fp, L"LALT        //左ＡＬＴキー\n");
	fwprintf_s(fp, L"RALT        //右ＡＬＴキー\n");
	fwprintf_s(fp, L"SCROLL      //ScrollLockキー\n");
	fwprintf_s(fp, L"SEMICOLON   //；キー\n");
	fwprintf_s(fp, L"COLON       //：キー\n");
	fwprintf_s(fp, L"LBRACKET    //［キー\n");
	fwprintf_s(fp, L"RBRACKET    //］キー\n");
	fwprintf_s(fp, L"AT          //＠キー\n");
	fwprintf_s(fp, L"BACKSLASH   //＼キー\n");
	fwprintf_s(fp, L"COMMA       //，キー\n");
	fwprintf_s(fp, L"CAPSLOCK    //CaspLockキー\n");
	fwprintf_s(fp, L"PAUSE       //PauseBreakキー\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"NUMPAD0     //テンキー０\n");
	fwprintf_s(fp, L"NUMPAD1     //テンキー１\n");
	fwprintf_s(fp, L"NUMPAD2     //テンキー２\n");
	fwprintf_s(fp, L"NUMPAD3     //テンキー３\n");
	fwprintf_s(fp, L"NUMPAD4     //テンキー４\n");
	fwprintf_s(fp, L"NUMPAD5     //テンキー５\n");
	fwprintf_s(fp, L"NUMPAD6     //テンキー６\n");
	fwprintf_s(fp, L"NUMPAD7     //テンキー７\n");
	fwprintf_s(fp, L"NUMPAD8     //テンキー８\n");
	fwprintf_s(fp, L"NUMPAD9     //テンキー９\n");
	fwprintf_s(fp, L"MULTIPLY    //テンキー＊キー\n");
	fwprintf_s(fp, L"ADD         //テンキー＋キー\n");
	fwprintf_s(fp, L"SUBTRACT    //テンキー－キー\n");
	fwprintf_s(fp, L"DECIMAL     //テンキー．キー\n");
	fwprintf_s(fp, L"DIVIDE      //テンキー／キー\n");
	fwprintf_s(fp, L"NUMPADENTER //テンキーのエンターキー\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"F1          //Ｆ１キー\n");
	fwprintf_s(fp, L"F2          //Ｆ２キー\n");
	fwprintf_s(fp, L"F3          //Ｆ３キー\n");
	fwprintf_s(fp, L"F4          //Ｆ４キー\n");
	fwprintf_s(fp, L"F5          //Ｆ５キー\n");
	fwprintf_s(fp, L"F6          //Ｆ６キー\n");
	fwprintf_s(fp, L"F7          //Ｆ７キー\n");
	fwprintf_s(fp, L"F8          //Ｆ８キー\n");
	fwprintf_s(fp, L"F9          //Ｆ９キー\n");
	fwprintf_s(fp, L"F10         //Ｆ１０キー\n");
	fwprintf_s(fp, L"F11         //Ｆ１１キー\n");
	fwprintf_s(fp, L"F12         //Ｆ１２キー\n");
	fwprintf_s(fp, L"\n");
	fwprintf_s(fp, L"A           //Ａキー\n");
	fwprintf_s(fp, L"B           //Ｂキー\n");
	fwprintf_s(fp, L"C           //Ｃキー\n");
	fwprintf_s(fp, L"D           //Ｄキー\n");
	fwprintf_s(fp, L"E           //Ｅキー\n");
	fwprintf_s(fp, L"F           //Ｆキー\n");
	fwprintf_s(fp, L"G           //Ｇキー\n");
	fwprintf_s(fp, L"H           //Ｈキー\n");
	fwprintf_s(fp, L"I           //Ｉキー\n");
	fwprintf_s(fp, L"J           //Ｊキー\n");
	fwprintf_s(fp, L"K           //Ｋキー\n");
	fwprintf_s(fp, L"L           //Ｌキー\n");
	fwprintf_s(fp, L"M           //Ｍキー\n");
	fwprintf_s(fp, L"N           //Ｎキー\n");
	fwprintf_s(fp, L"O           //Ｏキー\n");
	fwprintf_s(fp, L"P           //Ｐキー\n");
	fwprintf_s(fp, L"Q           //Ｑキー\n");
	fwprintf_s(fp, L"R           //Ｒキー\n");
	fwprintf_s(fp, L"S           //Ｓキー\n");
	fwprintf_s(fp, L"T           //Ｔキー\n");
	fwprintf_s(fp, L"U           //Ｕキー\n");
	fwprintf_s(fp, L"V           //Ｖキー\n");
	fwprintf_s(fp, L"W           //Ｗキー\n");
	fwprintf_s(fp, L"X           //Ｘキー\n");
	fwprintf_s(fp, L"Y           //Ｙキー\n");
	fwprintf_s(fp, L"Z           //Ｚキー\n");
	fwprintf_s(fp, L"0           //０キー\n");
	fwprintf_s(fp, L"1           //１キー\n");
	fwprintf_s(fp, L"2           //２キー\n");
	fwprintf_s(fp, L"3           //３キー\n");
	fwprintf_s(fp, L"4           //４キー\n");
	fwprintf_s(fp, L"5           //５キー\n");
	fwprintf_s(fp, L"6           //６キー\n");
	fwprintf_s(fp, L"7           //７キー\n");
	fwprintf_s(fp, L"8           //８キー\n");
	fwprintf_s(fp, L"9           //９キー\n");

	fclose(fp);
	return;
}

int KeyNameToInt(wchar_t* KeyName) {
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

wchar_t* IntToKeyName(int KeyNumber) {
	if (KeyNumber == KEY_INPUT_BACK)return L"BACK"; 		 		// バックスペースキー
	if (KeyNumber == KEY_INPUT_TAB)return L"TAB"; 			 	 // タブキー
	if (KeyNumber == KEY_INPUT_RETURN)return L"RETURN"; 		 		 // エンターキー

	if (KeyNumber == KEY_INPUT_LSHIFT)return L"LSHIFT"; 		 	// 左シフトキー
	if (KeyNumber == KEY_INPUT_RSHIFT)return L"RSHIFT"; 		 	// 右シフトキー
	if (KeyNumber == KEY_INPUT_LCONTROL)return L"LCONTROL"; 	 	// 左コントロールキー
	if (KeyNumber == KEY_INPUT_RCONTROL)return L"RCONTROL"; 	 	// 右コントロールキー
	if (KeyNumber == KEY_INPUT_ESCAPE)return L"ESCAPE"; 		 	// エスケープキー
	if (KeyNumber == KEY_INPUT_SPACE)return L"SPACE"; 		 // スペースキー
	if (KeyNumber == KEY_INPUT_PGUP)return L"PGUP"; 		 		// ＰａｇｅＵＰキー
	if (KeyNumber == KEY_INPUT_PGDN)return L"PGDN"; 		 		// ＰａｇｅＤｏｗｎキー
	if (KeyNumber == KEY_INPUT_END)return L"END"; 			 		// エンドキー
	if (KeyNumber == KEY_INPUT_HOME)return L"HOME"; 		 		// ホームキー
	if (KeyNumber == KEY_INPUT_LEFT)return L"LEFT"; 		 		// 左キー
	if (KeyNumber == KEY_INPUT_UP)return L"UP"; 			 	// 上キー
	if (KeyNumber == KEY_INPUT_RIGHT)return L"RIGHT"; 		 // 右キー
	if (KeyNumber == KEY_INPUT_DOWN)return L"DOWN"; 		 		// 下キー
	if (KeyNumber == KEY_INPUT_INSERT)return L"INSERT"; 		 	// インサートキー
	if (KeyNumber == KEY_INPUT_DELETE)return L"DELETE"; 		 		// デリートキー

	if (KeyNumber == KEY_INPUT_MINUS)return L"MINUS"; 		 	// －キー
	if (KeyNumber == KEY_INPUT_YEN)return L"YEN"; 			 	  // ￥キー
	if (KeyNumber == KEY_INPUT_PREVTRACK)return L"PREVTRACK"; 	 		// ＾キー
	if (KeyNumber == KEY_INPUT_PERIOD)return L"PERIOD"; 		 // ．キー
	if (KeyNumber == KEY_INPUT_SLASH)return L"SLASH"; 		 	// ／キー
	if (KeyNumber == KEY_INPUT_LALT)return L"LALT"; 		 	 // 左ＡＬＴキー
	if (KeyNumber == KEY_INPUT_RALT)return L"RALT"; 		 	// 右ＡＬＴキー
	if (KeyNumber == KEY_INPUT_SCROLL)return L"SCROLL"; 		 		// ScrollLockキー
	if (KeyNumber == KEY_INPUT_SEMICOLON)return L"SEMICOLON"; 	 // ；キー
	if (KeyNumber == KEY_INPUT_COLON)return L"COLON"; 		 	 // ：キー
	if (KeyNumber == KEY_INPUT_LBRACKET)return L"LBRACKET"; 	 		// ［キー
	if (KeyNumber == KEY_INPUT_RBRACKET)return L"RBRACKET"; 	 		// ］キー
	if (KeyNumber == KEY_INPUT_AT)return L"AT"; 			   // ＠キー
	if (KeyNumber == KEY_INPUT_BACKSLASH)return L"BACKSLASH"; 	 // ＼キー
	if (KeyNumber == KEY_INPUT_COMMA)return L"COMMA"; 		 	// ，キー
	if (KeyNumber == KEY_INPUT_CAPSLOCK)return L"CAPSLOCK"; 	 	// CaspLockキー
	if (KeyNumber == KEY_INPUT_PAUSE)return L"PAUSE"; 		 	// PauseBreakキー

	if (KeyNumber == KEY_INPUT_NUMPAD0)return L"NUMPAD0"; 		 	 // テンキー０
	if (KeyNumber == KEY_INPUT_NUMPAD1)return L"NUMPAD1"; 		 	// テンキー１
	if (KeyNumber == KEY_INPUT_NUMPAD2)return L"NUMPAD2"; 		 	// テンキー２
	if (KeyNumber == KEY_INPUT_NUMPAD3)return L"NUMPAD3"; 		 	// テンキー３
	if (KeyNumber == KEY_INPUT_NUMPAD4)return L"NUMPAD4"; 		 	// テンキー４
	if (KeyNumber == KEY_INPUT_NUMPAD5)return L"NUMPAD5"; 		 	// テンキー５
	if (KeyNumber == KEY_INPUT_NUMPAD6)return L"NUMPAD6"; 		 	// テンキー６
	if (KeyNumber == KEY_INPUT_NUMPAD7)return L"NUMPAD7"; 		 	// テンキー７
	if (KeyNumber == KEY_INPUT_NUMPAD8)return L"NUMPAD8"; 		 	// テンキー８
	if (KeyNumber == KEY_INPUT_NUMPAD9)return L"NUMPAD9"; 		 	// テンキー９
	if (KeyNumber == KEY_INPUT_MULTIPLY)return L"MULTIPLY"; 	 	// テンキー＊キー
	if (KeyNumber == KEY_INPUT_ADD)return L"ADD"; 			 	  // テンキー＋キー
	if (KeyNumber == KEY_INPUT_SUBTRACT)return L"SUBTRACT"; 	  	// テンキー－キー
	if (KeyNumber == KEY_INPUT_DECIMAL)return L"DECIMAL"; 		 	// テンキー．キー
	if (KeyNumber == KEY_INPUT_DIVIDE)return L"DIVIDE"; 		 // テンキー／キー
	if (KeyNumber == KEY_INPUT_NUMPADENTER)return L"NUMPADENTER"; 	// テンキーのエンターキー

	if (KeyNumber == KEY_INPUT_F1)return L"F1"; 			 	// Ｆ１キー
	if (KeyNumber == KEY_INPUT_F2)return L"F2"; 			 // Ｆ２キー
	if (KeyNumber == KEY_INPUT_F3)return L"F3"; 			 // Ｆ３キー
	if (KeyNumber == KEY_INPUT_F4)return L"F4"; 			 // Ｆ４キー
	if (KeyNumber == KEY_INPUT_F5)return L"F5"; 			 // Ｆ５キー
	if (KeyNumber == KEY_INPUT_F6)return L"F6"; 			 // Ｆ６キー
	if (KeyNumber == KEY_INPUT_F7)return L"F7"; 			 // Ｆ７キー
	if (KeyNumber == KEY_INPUT_F8)return L"F8"; 			 // Ｆ８キー
	if (KeyNumber == KEY_INPUT_F9)return L"F9"; 			 // Ｆ９キー
	if (KeyNumber == KEY_INPUT_F10)return L"F10"; 			 	// Ｆ１０キー
	if (KeyNumber == KEY_INPUT_F11)return L"F11"; 			 	// Ｆ１１キー
	if (KeyNumber == KEY_INPUT_F12)return L"F12"; 			 	// Ｆ１２キー

	if (KeyNumber == KEY_INPUT_A)return L"A"; 			 // Ａキー
	if (KeyNumber == KEY_INPUT_B)return L"B"; 			 	// Ｂキー
	if (KeyNumber == KEY_INPUT_C)return L"C"; 			 	// Ｃキー
	if (KeyNumber == KEY_INPUT_D)return L"D"; 			 	// Ｄキー
	if (KeyNumber == KEY_INPUT_E)return L"E"; 			 	// Ｅキー
	if (KeyNumber == KEY_INPUT_F)return L"F"; 			 	// Ｆキー
	if (KeyNumber == KEY_INPUT_G)return L"G"; 			 	// Ｇキー
	if (KeyNumber == KEY_INPUT_H)return L"H"; 			 	// Ｈキー
	if (KeyNumber == KEY_INPUT_I)return L"I"; 			 	// Ｉキー
	if (KeyNumber == KEY_INPUT_J)return L"J"; 			 	// Ｊキー
	if (KeyNumber == KEY_INPUT_K)return L"K"; 			 	// Ｋキー
	if (KeyNumber == KEY_INPUT_L)return L"L"; 			 	// Ｌキー
	if (KeyNumber == KEY_INPUT_M)return L"M"; 			 	// Ｍキー
	if (KeyNumber == KEY_INPUT_N)return L"N"; 			 	// Ｎキー
	if (KeyNumber == KEY_INPUT_O)return L"O"; 			 	// Ｏキー
	if (KeyNumber == KEY_INPUT_P)return L"P"; 			 	// Ｐキー
	if (KeyNumber == KEY_INPUT_Q)return L"Q"; 			 	// Ｑキー
	if (KeyNumber == KEY_INPUT_R)return L"R"; 			 	// Ｒキー
	if (KeyNumber == KEY_INPUT_S)return L"S"; 			 	// Ｓキー
	if (KeyNumber == KEY_INPUT_T)return L"T"; 			 	// Ｔキー
	if (KeyNumber == KEY_INPUT_U)return L"U"; 			 	// Ｕキー
	if (KeyNumber == KEY_INPUT_V)return L"V"; 			 	// Ｖキー
	if (KeyNumber == KEY_INPUT_W)return L"W"; 			 	// Ｗキー
	if (KeyNumber == KEY_INPUT_X)return L"X"; 			 	// Ｘキー
	if (KeyNumber == KEY_INPUT_Y)return L"Y"; 			 	// Ｙキー
	if (KeyNumber == KEY_INPUT_Z)return L"Z"; 			 	// Ｚキー
	if (KeyNumber == KEY_INPUT_0)return L"0"; 			 	// ０キー
	if (KeyNumber == KEY_INPUT_1)return L"1"; 			 	// １キー
	if (KeyNumber == KEY_INPUT_2)return L"2"; 			 	// ２キー
	if (KeyNumber == KEY_INPUT_3)return L"3"; 			 	// ３キー
	if (KeyNumber == KEY_INPUT_4)return L"4"; 			 	// ４キー
	if (KeyNumber == KEY_INPUT_5)return L"5"; 			 	// ５キー
	if (KeyNumber == KEY_INPUT_6)return L"6"; 			 	// ６キー
	if (KeyNumber == KEY_INPUT_7)return L"7"; 			 	// ７キー
	if (KeyNumber == KEY_INPUT_8)return L"8"; 			 	// ８キー
	if (KeyNumber == KEY_INPUT_9)return L"9"; 			 	// ９キー

	return L"ERROR";//どれにも当てはまらなかったからエラー
}