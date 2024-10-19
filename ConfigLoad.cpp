#include"STRUCT_CONFIG.h"
#include"ConfigLoad.h"
#include"CONSTANT_VALUE.h"
#include"DxLib.h"

void ConfigLoad(Config *config) {//Configファイルの読み込み
	wchar_t path[255];
	int fp;

	sprintfDx(path, L"save_data/config.dat");

	wchar_t sharp1[255];
	wchar_t sharp2[255];

	fp = FileRead_open(path);
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし

	wchar_t str[256];
	sharp2[0] = L'\0';

	wchar_t* noteSymbol[] = {
		L"NOTE_SYMBOL_R",
		L"NOTE_SYMBOL_G",
		L"NOTE_SYMBOL_B",
		L"NOTE_SYMBOL_C",
		L"NOTE_SYMBOL_M",
		L"NOTE_SYMBOL_Y",
		L"NOTE_SYMBOL_W",
		L"NOTE_SYMBOL_K",
		L"NOTE_SYMBOL_F",
	};

	while (FileRead_gets(str, 256, fp) != -1) {//一行取得(\nは入らない)
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", sharp1, _countof(sharp1), sharp2, _countof(sharp2));

		if (wcscmp(L"VSYNC", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Vsync = _wtoi(Val);//数値格納

			if (config->Vsync == 1) {
				config->Vsync = TRUE;
			}
			else {
				config->Vsync = FALSE;
			}
		}

		if (wcscmp(L"VSYNC_OFFSET_COMPENSATION", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->VsyncOffsetCompensation = _wtoi(Val);//数値格納

			if (config->VsyncOffsetCompensation == 1 && config->Vsync == TRUE) {//VsyncがONのときのみ有効
				config->VsyncOffsetCompensation = 16;
			}
			else {
				config->VsyncOffsetCompensation = 0;
			}
		}

		if (wcscmp(L"FPS", sharp1) == 0) {
			wchar_t Val[5];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Fps = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"SHOW_FPS", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ShowFps = _wtoi(Val);//数値格納

			if (config->ShowFps == 1) {
				config->ShowFps = TRUE;
			}
			else {
				config->ShowFps = FALSE;
			}
		}

		if (wcscmp(L"SHOW_STR_SHADOW", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ShowStrShadow = _wtoi(Val);//数値格納

			if (config->ShowStrShadow == 1) {
				config->ShowStrShadow = TRUE;
			}
			else {
				config->ShowStrShadow = FALSE;
			}
		}

		if (wcscmp(L"USE_HIPERFORMANCE_TIMER", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->UseHiPerformanceTimer = _wtoi(Val);//数値格納

			if (config->UseHiPerformanceTimer == 1) {
				config->UseHiPerformanceTimer = TRUE;
			}
			else {
				config->UseHiPerformanceTimer = FALSE;
			}
		}

		if (wcscmp(L"SONG_SELECT_ROW_NUMBER", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->SongSelectRowNumber = _wtoi(Val);//数値格納
		}


		if (wcscmp(L"DISPLAY_TIMING_OFFSET", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->DisplayTimingOffset = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"SHOW_DEBUG", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ShowDebug = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"LOCAL", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Local = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"USE_PY", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->UsePy = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"FULLSCREEN", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->FullScreen = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"SOUND_OUTPUT_TYPE", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->SoundOutputType = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"WASAPI_EXCLUSIVE", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->WasapiExclusive = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"ASIO_DRIVER", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->AsioDriver = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"BUFFER", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Buffer = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"COM_PORT", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ComPort = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"EDITABLE", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Editable = _wtoi(Val);//数値格納
		}

		if (wcscmp(L"USE_AI_PREDICTED_DIFFICULTY", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->UseAiPredictedDifficulty = _wtoi(Val);//数値格納
		}

		for (int i = 0; i < 9; i++) {
			if (wcscmp(noteSymbol[i], sharp1) == 0) {
				wchar_t Val[3];
				swscanf_s(sharp2, L"%[^,\n]",
					Val, _countof(Val));
				config->NoteSymbol[i] = _wtoi(Val);//数値格納

				if (config->NoteSymbol[i] == 1) {
					config->NoteSymbol[i] = TRUE;
				}
				else {
					config->NoteSymbol[i] = FALSE;
				}
			}
		}


		if (wcscmp(L"BASE_FONT", sharp1) == 0) {
			wchar_t Val[256];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->BaseFont = std::wstring(Val);
		}

		//if (wcscmp(L"ALTERNATIVE_FONT", sharp1) == 0) {
		//	wchar_t Val[256];
		//	swscanf_s(sharp2, L"%[^,\n]",
		//		Val, _countof(Val));
		//	config->AlternativeFont = std::wstring(Val);
		//}

		if (wcscmp(L"#END", sharp1) == 0) {
			break;
		}

		sharp2[0] = L'\0';

	}

	if (config->Vsync == TRUE) {
		//ディスプレイのリフレッシュレートを設定する 通常は60 モニタによって120など
		int RefreshRate;
		HDC hdc;

		hdc = GetDC(GetMainWindowHandle());	// デバイスコンテキストの取得
		RefreshRate = GetDeviceCaps(hdc, VREFRESH);	// リフレッシュレートの取得
		ReleaseDC(GetMainWindowHandle(), hdc);	// デバイスコンテキストの解放

		config->Fps = RefreshRate;
	}

	FileRead_close(fp);

	return;
}