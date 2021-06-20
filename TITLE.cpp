#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"TITLE.h"
#include"Get_Key_State.h"
#include"show_something.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"GetNowCount_d.h"
#include"ShowFps.h"
#include"ScreenShot.h"
#include"STRUCT_OP.h"
#include<string>
using namespace std;

void TITLE(int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, CONFIG config, OPTION *option) {
	int H_BG;//背景画像
	int H_CLOUD;//雲画像
	int H_TITLE_LOGO;//ロゴ画像

	//int H_PFC = LoadGraph(L"img/PERFECT_FULLCOMBO.png");
	//int H_PFC_BRIGHT = LoadGraph(L"img/PERFECT_FULLCOMBO_BRIGHT.png");
	//int HM_PFC = LoadMask(L"img/PERFECT_FULLCOMBO_MASK.png");

	int SH_START;//スタート音
	int SH_CLOSE;
	int SH_CLOSED;
	int SH_BGM;
	int SH_SHUTTER;
	int SH_SHUTTER_SIGNAL;
	//int SH_TEST[3];

	int j = 0;
	int k[3] = { 0,0,0 };
	double be[3] = { 3,1,1 };

	double GAME_passed_time = 0, GAME_start_time = 0;
	double LOOP_passed_time = 1;
	double CounterRemainTime = 0;//カウンターの値を1msずつ変動するための時間
	double time_cash = 0;
	int push_time[3] = { -50000,-50000,-50000 };
	int brightness = 0;
	int start = 0;//最初のボタンを押したフラグ
	double LOGO_draw_alpha = 1;//ロゴの透明度
	int i = 0;
	XY pos_cloud;//雲の座標

	double BackDrawCounter = 0;//背景をうっすらと表示させるためのカウンタ
	double OtherDrawCounter = 0;
	BOOL BGMPlay = 0;//BGM再生したかのフラグ(0:してない 1:した)


	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	H_CLOUD = LoadGraph(L"img/cloud.png");

	H_TITLE_LOGO = LoadGraph(L"img/title_logo.png");

	SH_START = LoadSoundMem(L"sound/nature_prhysm_jingle.wav");
	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");

	SH_BGM = LoadSoundMem(L"sound/nature_prhysm_theme.ogg");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");
	/*
	SH_TEST[0] = LoadSoundMem(L"sound/1.wav", 1);
	SH_TEST[1] = LoadSoundMem(L"sound/2.wav", 1);
	SH_TEST[2] = LoadSoundMem(L"sound/3.wav", 1);
	*/


	ChangeFont(L"メイリオ");
	SetFontThickness(1);
	ChangeFontType(DX_FONTTYPE_EDGE);
	SetFontSize(26);
	

	GAME_start_time = GetNowCount_d(config);
	Get_Key_State(Buf, Key, AC);
	while (1) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}


		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		Get_Key_State(Buf, Key, AC);

		if (start == 0) {
			if (Key[Button[0][0]] || Key[Button[0][1]] || Key[Button[0][2]] || Key[Button[0][3]] ||
				Key[Button[1][0]] || Key[Button[1][1]] || Key[Button[1][2]] || Key[Button[1][3]] ||
				Key[Button[2][0]] || Key[Button[2][1]] || Key[Button[2][2]] || Key[Button[2][3]] || Key[KEY_INPUT_RETURN]) {
				start = 1;
				PlaySoundMem(SH_START, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		if (Key[Button_Shutter] == 1) {//スクリーンショット
			ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
		}
		/*
		if (Key[KEY_INPUT_1] == 1 || GAME_passed_time >= k[0]*be[0] * 689.655172){
		StopSoundMem(SH_TEST[0]);
		ChangeVolumeSoundMem(255, SH_TEST[0]);
		SetCurrentPositionSoundMem(int(44100*((double)k[0]*be[0] * 60/87)), SH_TEST[0]);
		k[0]++;
		PlaySoundMem(SH_TEST[0], DX_PLAYTYPE_BACK, FALSE);
		push_time[0] = GAME_passed_time;
		//j = (j + 1) % 2;
		}
		if (GAME_passed_time - push_time[0] >= be[0] * 689.655172) {
		ChangeVolumeSoundMem(0, SH_TEST[0]);
		//StopSoundMem(SH_TEST[j]);
		}

		if (Key[KEY_INPUT_2] == 1 || GAME_passed_time >= k[1] * be[1] * 689.655172) {
		StopSoundMem(SH_TEST[1]);
		ChangeVolumeSoundMem(255, SH_TEST[1]);
		SetCurrentPositionSoundMem(int(44100 * ((double)k[1] * be[1] * 60 / 87)), SH_TEST[1]);
		k[1]++;
		PlaySoundMem(SH_TEST[1], DX_PLAYTYPE_BACK, FALSE);
		push_time[1] = GAME_passed_time;
		//j = (j + 1) % 2;
		}
		if (GAME_passed_time - push_time[1] >= be[1] * 689.655172) {
		ChangeVolumeSoundMem(0, SH_TEST[1]);
		//StopSoundMem(SH_TEST[j]);
		}
		if (Key[KEY_INPUT_3] == 1 || GAME_passed_time >= k[2] * be[2] * 689.655172) {
		StopSoundMem(SH_TEST[2]);
		ChangeVolumeSoundMem(255, SH_TEST[2]);
		SetCurrentPositionSoundMem(int(44100 * ((double)k[2] * be[2] * 60 / 87)), SH_TEST[2]);
		k[2]++;
		PlaySoundMem(SH_TEST[2], DX_PLAYTYPE_BACK, FALSE);
		push_time[2] = GAME_passed_time;
		//j = (j + 1) % 2;
		}
		if (GAME_passed_time - push_time[2] >= be[2] * 689.655172) {
		ChangeVolumeSoundMem(0, SH_TEST[2]);
		//StopSoundMem(SH_TEST[j]);
		}

		/*
		//前に鳴ってた音
		if (GAME_passed_time - push_time <=100) {
		ChangeVolumeSoundMem(0, SH_TEST[j]);
		//ChangeVolumeSoundMem(255 * (pow((100 - (GAME_passed_time - push_time)),0.00001) / 3.5), SH_TEST[j]);

		}
		if (GAME_passed_time - push_time >= 100) {
		StopSoundMem(SH_TEST[j]);
		}

		//今鳴ってる音
		if (GAME_passed_time - push_time >= be * 689.655172) {
		ChangeVolumeSoundMem(0, SH_TEST[j]);
		//ChangeVolumeSoundMem(255 * (pow((be * 689 + 100 - (GAME_passed_time - push_time)), 0.00001) / 3.5), SH_TEST[(j + 1) % 2]);

		}



		*/

		brightness = int(255 * (1 + sin((double)GAME_passed_time / 600)) / 2);

		if (start == 1 && LOGO_draw_alpha == 0) break;

		//Draw
		ClearDrawScreen();
		SetDrawBright(255, 255, 255);


		for (i = 0; i < CRTBuf; i++) {
			if (BackDrawCounter < 255) {
				BackDrawCounter += 0.05;
				if (BackDrawCounter >= 255)BackDrawCounter = 255;
			}

			if (OtherDrawCounter < 255) {
				OtherDrawCounter += 0.05;
				if (OtherDrawCounter >= 255)OtherDrawCounter = 255;
			}

			if (start == 1) {

				if (LOGO_draw_alpha >= 0) {
					LOGO_draw_alpha -= 0.0018;
				}
				if (LOGO_draw_alpha < 0) {
					LOGO_draw_alpha = 0;
				}

			}
		}
	
	

		
		//printfDx(L"BackDrawCounter:%f\n", BackDrawCounter);
		//printfDx(L"OtherDrawCounter:%f\n", OtherDrawCounter);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(BackDrawCounter));
		DrawGraph(0, 0, H_BG, TRUE);//背景

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(LOGO_draw_alpha * OtherDrawCounter));
		show_cloud(H_CLOUD, &pos_cloud, 1, LOOP_passed_time);//雲
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		SetDrawBright(brightness, brightness, brightness);


		//LOGO_draw_alpha = 0;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(LOGO_draw_alpha * OtherDrawCounter));
		DrawGraph(0, 0, H_TITLE_LOGO, TRUE);//ロゴ
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		/*
		DrawGraph(0, 0, H_PFC, TRUE);
		CreateMaskScreen();
		FillMaskScreen(1);
		DrawMask(0, 0, HM_PFC, DX_MASKTRANS_NONE);
		DrawGraph(0, 50-(int(GAME_passed_time*0.1) % 100), H_PFC_BRIGHT, TRUE);
		DeleteMaskScreen();
		*/


		if ((int(GAME_passed_time) - 100) % 480 <= 370 && start == 0) {
			DrawString(502, 622, L"\"Press Any Key\"", GetColor(0, 0, 0));
			//DrawString(498, 618, "\"Press Any Key\"", GetColor(0, 0, 0));
			//DrawString(502, 618, "\"Press Any Key\"", GetColor(0, 0, 0));
			//DrawString(498, 622, "\"Press Any Key\"", GetColor(0, 0, 0));
			DrawString(500, 620, L"\"Press Any Key\"", GetColor(255, 255, 255));
		}

		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000/config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		SetDrawBright(255, 255, 255);
		ScreenFlip();
		clsDx();

		//BGM再生
		if (BGMPlay == 0 && GAME_passed_time >= 1500) {
			PlaySoundMem(SH_BGM, DX_PLAYTYPE_BACK, TRUE);
			BGMPlay = 1;
		}
		ChangeVolumeSoundMem(int(LOGO_draw_alpha*LOGO_draw_alpha * 255), SH_BGM);

		//Sleep(1);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255, 255, 255);
	CLOSE_COVER(1, config, option);

	InitGraph();//メモリ開放
	InitSoundMem();//
	return;
}


void OPEN_COVER(int difficulty,CONFIG config) {
	int H_COVER = 0;//カバー画像
	int H_COVER_MIDDLE;//中心カバー
	int H_BG;
	double c_m_draw_counter = 0;//中心カバー描画カウンター(0:Close 1:Open)
	double GAME_passed_time = 0;
	double GAME_start_time = 0;
	double LOOP_passed_time = 1;
	double time_cash = 0;
	int i = 0;

	if (difficulty == 1)H_COVER = LoadGraph(L"img/cover_sunny.png");//難易度によってカバー変更
	if (difficulty == 2)H_COVER = LoadGraph(L"img/cover_cloudy.png");
	if (difficulty == 3)H_COVER = LoadGraph(L"img/cover_rainy.png");
	H_COVER_MIDDLE = LoadGraph(L"img/cover_middle.png");
	H_BG = LoadGraph(L"img/bg.png");

	GAME_start_time = GetNowCount_d(config);
	while (1) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}

		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		//Draw
		ClearDrawScreen();
		DrawGraph(0, 0, H_BG, TRUE);//背景
		DrawGraph(int(-320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 0), 0, H_COVER, TRUE);//カバー表示
		DrawGraph(int(320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 960), 0, H_COVER, TRUE);//右側
		DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
		if (c_m_draw_counter > 1) {
			InitGraph();//メモリ開放
			InitSoundMem();//
			return;
		}
		for (i = 0; i < LOOP_passed_time; i++) {
			if (c_m_draw_counter <= 1) {
				c_m_draw_counter += 0.001;
			}
		}
		Sleep(1);
		ScreenFlip();
	}
}

void CLOSE_COVER(int difficulty, CONFIG config, OPTION* option) {
	int H_COVER = 0;//カバー画像
	int H_COVER_MIDDLE;//中心カバー
	int H_BG;
	int SH_CLOSE;
	int SH_CLOSED;
	int beepflag = 0;
	double c_m_draw_counter = 1;//中心カバー描画カウンター(0:Close 1:Open)
	double GAME_passed_time = 0;
	double GAME_start_time = 0;
	double LOOP_passed_time = 1;
	double time_cash = 0;
	int i = 0;

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
	if (difficulty == 1)H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());//難易度によってカバー変更
	if (difficulty == 2)H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
	if (difficulty == 3)H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());

	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");

	GAME_start_time = GetNowCount_d(config);
	while (1) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}

		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		//Draw
		ClearDrawScreen();
		DrawGraph(0, 0, H_BG, TRUE);//背景
		DrawGraph(int(-320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 0), 0, H_COVER, TRUE);//カバー表示
		DrawGraph(int(320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 960), 0, H_COVER, TRUE);//右側
		DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
		if (c_m_draw_counter < 0) {
			PlaySoundMem(SH_CLOSED, DX_PLAYTYPE_BACK, TRUE);
			for (i = 0; i <= 1500; i++) {//1.5s待つ
				if (ProcessMessage() != 0) {
					DxLib_End();
					exit(0);
					return;
				}
				Sleep(1);
			}

			InitGraph();//メモリ開放
			InitSoundMem();//
			return;
		}
		for (i = 0; i < LOOP_passed_time; i++) {
			if (c_m_draw_counter >= 0) {
				c_m_draw_counter -= 0.0015;
			}
		}

		//SOUND
		if (beepflag == 0) {
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			beepflag = 1;
		}

		i = 0;
		while (LOOP_passed_time + i < 0) {//最低10ms待ってやる(カクつかないように)
			Sleep(1);
			i++;

			//printfDx(L"i:%d\n", i);
			//ScreenFlip();
		}
		Sleep(1);
		ScreenFlip();
		clsDx();
	}
}