#include"STRUCT_CONFIG.h"
#include"ConfigLoad.h"
#include"CONSTANT_VALUE.h"
#include"DxLib.h"

void ConfigLoad(Config *config) {//Configƒtƒ@ƒCƒ‹‚Ì“Ç‚Ýž‚Ý
	wchar_t path[255];
	int fp;

	sprintfDx(path, L"save_data/config.dat");

	wchar_t sharp1[255];
	wchar_t sharp2[255];

	fp = FileRead_open(path);
	FileRead_seek(fp, 2, SEEK_SET);//BOM”ò‚Î‚µ

	wchar_t str[256];
	sharp2[0] = L'\0';

	while (FileRead_gets(str, 256, fp) != -1) {//ˆêsŽæ“¾(\n‚Í“ü‚ç‚È‚¢)
		swscanf_s(str, L"%[^:\n]:%[^:\n]\n", sharp1, _countof(sharp1), sharp2, _countof(sharp2));

		if (wcscmp(L"VSYNC", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Vsync = _wtoi(Val);//”’lŠi”[

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
			config->VsyncOffsetCompensation = _wtoi(Val);//”’lŠi”[

			if (config->VsyncOffsetCompensation == 1 && config->Vsync == TRUE) {//Vsync‚ªON‚Ì‚Æ‚«‚Ì‚Ý—LŒø
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
			config->Fps = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"SHOW_FPS", sharp1) == 0) {
			wchar_t Val[3];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ShowFps = _wtoi(Val);//”’lŠi”[

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
			config->ShowStrShadow = _wtoi(Val);//”’lŠi”[

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
			config->UseHiPerformanceTimer = _wtoi(Val);//”’lŠi”[

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
			config->SongSelectRowNumber = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"#END", sharp1) == 0) {
			break;
		}

		if (wcscmp(L"DISPLAY_TIMING_OFFSET", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->DisplayTimingOffset = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"SHOW_DEBUG", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->ShowDebug = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"LOCAL", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Local = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"USE_PY", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->UsePy = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"FULLSCREEN", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->FullScreen = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"SOUND_OUTPUT_TYPE", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->SoundOutputType = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"WASAPI_EXCLUSIVE", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->WasapiExclusive = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"ASIO_DRIVER", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->AsioDriver = _wtoi(Val);//”’lŠi”[
		}

		if (wcscmp(L"BUFFER", sharp1) == 0) {
			wchar_t Val[9];
			swscanf_s(sharp2, L"%[^,\n]",
				Val, _countof(Val));
			config->Buffer = _wtoi(Val);//”’lŠi”[
		}


		if (wcscmp(L"#END", sharp1) == 0) {
			break;
		}

		sharp2[0] = L'\0';

	}

	if (config->Vsync == TRUE) {
		config->Fps = 60;
	}

	FileRead_close(fp);

	return;
}