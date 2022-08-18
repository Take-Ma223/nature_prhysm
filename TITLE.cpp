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
#include"IR_process.h"
#include"KeyConfig.h"
#include "Image.h"
#include "Asset.h"

using namespace std;

void TITLE(int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, CONFIG config, OPTION *option, IR_SETTING *ir) {
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
	int SH_ERROR;
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


	const int STATE_PRESS_ANY_KEY = 0;
	const int STATE_SELECT_MENU = 1;
	const int STATE_IR_SETTING = 2;
	const int STATE_KEY_CONFIG = 3;


	int stat = STATE_PRESS_ANY_KEY;//画面状態変数

	const int MENU_GAME_START = 0;
	const int MENU_IR_SETTING = 1;
	const int MENU_KEY_CONFIG = 2;

	int menuSelectStat = MENU_GAME_START;

	const int IRMENU_PARTICIPATE = 0;
	const int IRMENU_PLAYER_NAME = 1;
	const int IRMENU_SET_RIVAL_ID = 2;
	const int IRMENU_EXIT = 3;
	int IRWarningFlag = 0;//注意書き表示中のフラグ
	int IRMenuStat = IRMENU_PARTICIPATE;

	const int KEY_CONFIG_MENU_SETTING = 0;
	const int KEY_CONFIG_MENU_EXIT = 1;
	int keyConfigSettingFlag = 0;//キーコンフィグ中かどうかのフラグ
	struct Position {
		int x = 0;
		int y = 0;
	};

	Position settingKeyPosition;
	settingKeyPosition.y = 2;

	int KeyConfigMenuStat = KEY_CONFIG_MENU_SETTING;

	Asset asset;//使う画像セット

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	H_CLOUD = LoadGraph(L"img/cloud.png");

	H_TITLE_LOGO = LoadGraph(L"img/title_logo.png");

	
	Image buttonBB = Image(asset.img(L"img/button_BB.png"), &GAME_passed_time, 600, 300, 255);
	auto buttonAnimation = [&] {
		buttonBB.clearAllEvent();
		buttonBB.visible.eON();
		buttonBB.X.eSet(600);
		buttonBB.X.eChange(point(600, Abs), point(1200,Abs), Converter(Linear), 0, 5000);
		buttonBB.X.eSet(1200, 5000);


		buttonBB.action.eSet(0,[&]{
			PlaySoundMem(SH_SHUTTER, DX_PLAYTYPE_BACK, TRUE);
			});
		buttonBB.action.eSet(1000,[&]{
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			});

		buttonBB.playAll();
	};

	auto buttonPlay = [&] {
		buttonBB.playAll();
		buttonBB.X.setStartTimeFromRange(0.5);
	};

	auto buttonStop = [&] {
		buttonBB.stopAll();
	};

	auto buttonResume = [&] {
		buttonBB.resumeAll();
	};

	auto buttonReverse = [&] {
		buttonBB.reverseAll();
	};

	auto buttonSpeed2 = [&] {

		buttonBB.setPlaySpeedAll(2);
	};

	auto buttonSpeed05 = [&] {

		buttonBB.setPlaySpeedAll(0.5);
	};

	auto buttonLoop = [&] {

		buttonBB.setLoopAll(TRUE);
	};
	

	SH_START = LoadSoundMem(L"sound/nature_prhysm_jingle.wav");
	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");

	SH_BGM = LoadSoundMem(L"sound/nature_prhysm_theme.ogg");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");
	SH_ERROR = LoadSoundMem(L"sound/no.wav");
	/*
	SH_TEST[0] = LoadSoundMem(L"sound/1.wav", 1);
	SH_TEST[1] = LoadSoundMem(L"sound/2.wav", 1);
	SH_TEST[2] = LoadSoundMem(L"sound/3.wav", 1);
	*/


	ChangeFont(L"メイリオ");
	SetFontThickness(9);
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
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

		
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_0] == 1) {
				buttonPlay();
			}
		}

		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_1]==1) {
				buttonAnimation();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_2]) {
				buttonStop();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_3]) {
				buttonResume();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_4]==1) {
				buttonReverse();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_5]==1) {
				buttonSpeed05();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_6] == 1) {
				buttonSpeed2();
			}
		}
		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[KEY_INPUT_7] == 1) {
				buttonLoop();
			}
		}
		

		if (stat == STATE_PRESS_ANY_KEY) {
			if (Key[Button[0][0]] || Key[Button[0][1]] || Key[Button[0][2]] || Key[Button[0][3]] ||
				Key[Button[1][0]] || Key[Button[1][1]] || Key[Button[1][2]] || Key[Button[1][3]] ||
				Key[Button[2][0]] || Key[Button[2][1]] || Key[Button[2][2]] || Key[Button[2][3]] || Key[KEY_INPUT_RETURN]) {
				stat = STATE_SELECT_MENU;
			}
		}
		else if (stat == STATE_SELECT_MENU) {
			if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {
				if (menuSelectStat == MENU_GAME_START) {
					start = 1;
					PlaySoundMem(SH_START, DX_PLAYTYPE_BACK, TRUE);
				}
				else if(menuSelectStat == MENU_IR_SETTING){
					stat = STATE_IR_SETTING;
				}
				else if (menuSelectStat == MENU_KEY_CONFIG) {
					stat = STATE_KEY_CONFIG;
				}
			}
			else if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
				menuSelectStat -= 1;
				if (menuSelectStat < MENU_GAME_START)menuSelectStat = MENU_GAME_START;
			}
			else if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
				menuSelectStat += 1;
				if (menuSelectStat > MENU_KEY_CONFIG)menuSelectStat = MENU_KEY_CONFIG;
			}


		}
		else if(stat == STATE_IR_SETTING){
			if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {
				if (IRMenuStat == IRMENU_PLAYER_NAME) {
					DrawBoxWithLine(640 - 240, 470, 640 + 240, 510, GetColor(100, 100, 100));

					wchar_t* str1 = L"\"注意:インターネットランキングで公開されるため個人情報は入力しないでください\"";
					int width1 = GetDrawStringWidth(str1, wcslen(str1));
					ShowExtendedStrFit(640, 430, str1, width1, 1280, config,GetColor(255, 255, 255), GetColor(0, 0, 0));					

					wchar_t strbuf[17]=L"";
					KeyInputString(410, 480,
						16, strbuf,
						TRUE);
					stat = STATE_IR_SETTING;

					sprintfDx(ir->plaeyerName, L"%s", strbuf);

					SaveIRSetting(*ir);
					LoadIRSetting(ir);
					
					wchar_t str2[64];
					sprintfDx(str2, L"%sに変更しました", strbuf);
					width1 = GetDrawStringWidth(str2, wcslen(str2));
					ShowExtendedStrFit(640, 530, str2, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));
					ScreenFlip();
					Sleep(2000);



				}
				else if (IRMenuStat == IRMENU_SET_RIVAL_ID) {
					DrawBoxWithLine(640 - 240, 470, 640 + 240, 510, GetColor(100, 100, 100));

					wchar_t* str1 = L"\"ライバルに設定したいプレーヤーのIDを半角数字で入力してください\"";
					int width1 = GetDrawStringWidth(str1, wcslen(str1));
					ShowExtendedStrFit(640, 430, str1, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));

					wchar_t strbuf[17] = L"";
					KeyInputString(410, 480,
						16, strbuf,
						TRUE);
					stat = STATE_IR_SETTING;

					ir->rivalID = atoiDx(strbuf);

					SaveIRSetting(*ir);
					LoadIRSetting(ir);

					wchar_t str2[64];
					sprintfDx(str2, L"RIVAL ID:%dに設定しました", ir->rivalID);
					width1 = GetDrawStringWidth(str2, wcslen(str2));
					ShowExtendedStrFit(640, 530, str2, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));
					ScreenFlip();
					Sleep(2000);
				}
				else if (IRMenuStat == IRMENU_PARTICIPATE) {
					int width1;
					wchar_t str[64];
					

					if (ir->IR_Enable == FALSE) {
						ir->IR_Enable = TRUE;
						sprintfDx(str, L"\"参加する\"に設定を変更しました");
					}
					else {
						ir->IR_Enable = FALSE;
						sprintfDx(str, L"\"参加しない\"に設定を変更しました");
					}

					SaveIRSetting(*ir);
					LoadIRSetting(ir);


					width1 = GetDrawStringWidth(str, wcslen(str));
					ShowExtendedStrFit(640, 500, str, width1, 1280, config,GetColor(255, 255, 255), GetColor(0, 0, 0));
					ScreenFlip();
					Sleep(2000);
				}
				else if (IRMenuStat == IRMENU_EXIT) {
					stat = STATE_SELECT_MENU;
				}
			}
			else if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
				IRMenuStat -= 1;
				if (IRMenuStat < IRMENU_PARTICIPATE)IRMenuStat = IRMENU_PARTICIPATE;
			}
			else if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
				IRMenuStat += 1;
				if (IRMenuStat > IRMENU_EXIT)IRMenuStat = IRMENU_EXIT;

			}
		}
		else if (stat == STATE_KEY_CONFIG) {
			if (keyConfigSettingFlag == 0) {
				if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {
					if (KeyConfigMenuStat == KEY_CONFIG_MENU_SETTING) {
						if (keyConfigSettingFlag == 0) {
							settingKeyPosition.x = 0;
							settingKeyPosition.y = 2;
							keyConfigSettingFlag = 1;
						}
					}
					else if (KeyConfigMenuStat == KEY_CONFIG_MENU_EXIT) {
						stat = STATE_SELECT_MENU;
					}
				}
				else if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
					KeyConfigMenuStat -= 1;
					if (KeyConfigMenuStat < KEY_CONFIG_MENU_SETTING)KeyConfigMenuStat = KEY_CONFIG_MENU_SETTING;
				}
				else if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
					KeyConfigMenuStat += 1;
					if (KeyConfigMenuStat > KEY_CONFIG_MENU_EXIT)KeyConfigMenuStat = KEY_CONFIG_MENU_EXIT;
				}
			}
			else {
				for (i = 0; i < 256; i++) {
					if (Key[i] == 1) {
						if (isValidKey(i)) {
							Button[settingKeyPosition.y][settingKeyPosition.x] = i;
							settingKeyPosition.x++;
							if (settingKeyPosition.x >= 4) {
								settingKeyPosition.x = 0;
								settingKeyPosition.y--;
								if (settingKeyPosition.y <= -1) {
									//キー配置保存
									KeyConfigSave(Button, &Button_Shutter);
									KeyConfigLoad(Button, &Button_Shutter);

									SetDrawBright(255, 255, 255);
									int width1;
									wchar_t str2[64];
									sprintfDx(str2, L"キー配置設定を保存しました");
									width1 = GetDrawStringWidth(str2, wcslen(str2));
									ShowExtendedStrFit(640, 500, str2, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));
									ScreenFlip();
									Sleep(2000);
									keyConfigSettingFlag = 0;
								}
							}
							break;
						}
						else {
							PlaySoundMem(SH_ERROR, DX_PLAYTYPE_BACK, TRUE);
						}


					}
				}
					
				
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

		if (stat == STATE_PRESS_ANY_KEY) {
			if ((int(GAME_passed_time) - 100) % 480 <= 370 && start == 0) {
				wchar_t* str = L"\"Press Any Key\"";
				int width = GetDrawStringWidth(str, wcslen(str));
				ShowExtendedStrFit(640, 590, str, width, 300, config, GetColor(255, 255, 255), GetColor(0, 0, 0));
				//DrawString(502, 622, L"\"Press Any Key\"", GetColor(0, 0, 0));
				//DrawString(500, 620, L"\"Press Any Key\"", GetColor(255, 255, 255));
			}
		}
		else if(stat == STATE_SELECT_MENU){
			SetDrawBright(255,255,255);

			wchar_t* str1 = L"\"GAME START\"";
			int width1 = GetDrawStringWidth(str1, wcslen(str1));
			ShowExtendedStrFit(640, 590, str1, width1, 300, config, menuSelectStat == MENU_GAME_START ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str2 = L"\"IR SETTING\"";
			int width2 = GetDrawStringWidth(str2, wcslen(str2));
			ShowExtendedStrFit(640, 620, str2, width2, 300, config, menuSelectStat == MENU_IR_SETTING ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str3 = L"\"KEY CONFIG\"";
			int width3 = GetDrawStringWidth(str3, wcslen(str3));
			ShowExtendedStrFit(640, 650, str3, width3, 300, config, menuSelectStat == MENU_KEY_CONFIG ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));


		}
		else if (stat == STATE_IR_SETTING) {
			SetDrawBright(255, 255, 255);

			wchar_t str1[21];
			if (ir->IR_Enable == TRUE) {
				sprintfDx(str1, L"\"インターネットランキング参加:する\"");
			}
			else {
				sprintfDx(str1, L"\"インターネットランキング参加:しない\"");
			}

			int width1 = GetDrawStringWidth(str1, wcslen(str1));
			ShowExtendedStrFit(640, 590, str1, width1, 500, config, IRMenuStat == IRMENU_PARTICIPATE ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str2 = L"\"PLAYER NAME変更\"";
			int width2 = GetDrawStringWidth(str2, wcslen(str2));
			ShowExtendedStrFit(640, 620, str2, width2, 300, config, IRMenuStat == IRMENU_PLAYER_NAME ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str3 = L"\"RIVAL ID 設定\"";
			int width3 = GetDrawStringWidth(str3, wcslen(str3));
			ShowExtendedStrFit(640, 650, str3, width3, 300, config, IRMenuStat == IRMENU_SET_RIVAL_ID ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str4 = L"\"戻る\"";
			int width4 = GetDrawStringWidth(str4, wcslen(str4));
			ShowExtendedStrFit(640, 680, str4, width4, 300, config, IRMenuStat == IRMENU_EXIT ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));


			wchar_t* str5 = L"―――注意―――";
			int width5 = GetDrawStringWidth(str5, wcslen(str5));
			ShowExtendedStrFit(640, 315, str5, width5, 800, config,GetColor(255, 255, 255), GetColor(0, 0, 0));

			wchar_t* str6 = L"インターネットランキングに参加すると";
			int width6 = GetDrawStringWidth(str6, wcslen(str6));
			ShowExtendedStrFit(640, 345, str6, width6, 800, config,GetColor(255, 255, 255), GetColor(0, 0, 0));

			wchar_t* str7 = L"プレイヤー名、プレイした譜面の情報、スコアがサーバーに送信されます";
			int width7 = GetDrawStringWidth(str7, wcslen(str7));
			ShowExtendedStrFit(640, 375, str7, width7, 1000, config,GetColor(255, 255, 255), GetColor(0, 0, 0));


		}
		else if (stat == STATE_KEY_CONFIG) {
			SetDrawBright(255, 255, 255);

			wchar_t* str1 = L"\"キー設定を変更する\"";
			int width1 = GetDrawStringWidth(str1, wcslen(str1));
			ShowExtendedStrFit(640, 590, str1, width1, 300, config, KeyConfigMenuStat == KEY_CONFIG_MENU_SETTING ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			wchar_t* str2 = L"\"戻る\"";
			int width2 = GetDrawStringWidth(str2, wcslen(str2));
			ShowExtendedStrFit(640, 620, str2, width2, 300, config, KeyConfigMenuStat == KEY_CONFIG_MENU_EXIT ? GetColor(255, 255, 255) : GetColor(100, 100, 100), GetColor(0, 0, 0));

			if (keyConfigSettingFlag == 1) {
				wchar_t* str1 = L"\"設定するキーを順に押してください\"";
				int width1 = GetDrawStringWidth(str1, wcslen(str1));
				ShowExtendedStrFit(640, 400, str1, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));

				DrawBoxWithLine(640 - 80, 440, 640 - 40, 480,	settingKeyPosition.y == 0 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
				DrawBoxWithLine(640 - 40, 440, 640, 480,		settingKeyPosition.y == 0 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
				DrawBoxWithLine(640, 440, 640 + 40, 480,		settingKeyPosition.y == 0 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
				DrawBoxWithLine(640 + 40, 440, 640 + 80, 480,	settingKeyPosition.y == 0 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);

				DrawBoxWithLine(640 - 40, 480, 640, 520,		settingKeyPosition.y == 1 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
				DrawBoxWithLine(640, 480, 640 + 40, 520,		settingKeyPosition.y == 1 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
				DrawBoxWithLine(640 + 40, 480, 640 + 80, 520,	settingKeyPosition.y == 1 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
				DrawBoxWithLine(640 - 80, 480, 640 - 40, 520,	settingKeyPosition.y == 1 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);

				DrawBoxWithLine(640 - 80, 520, 640 - 40, 560,	settingKeyPosition.y == 2 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
				DrawBoxWithLine(640 - 40, 520, 640, 560,		settingKeyPosition.y == 2 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
				DrawBoxWithLine(640, 520, 640 + 40, 560,		settingKeyPosition.y == 2 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
				DrawBoxWithLine(640 + 40, 520, 640 + 80, 560,	settingKeyPosition.y == 2 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
			}
		}

		buttonBB.draw();

		SetDrawBright(brightness, brightness, brightness);
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

int isValidKey(int key) {


	if (key == KEY_INPUT_ESCAPE)return FALSE;
	if (key == KEY_INPUT_LSHIFT)return FALSE;
	if (key == KEY_INPUT_LCONTROL)return FALSE;
	if (key == KEY_INPUT_RSHIFT)return FALSE;
	if (key == KEY_INPUT_RCONTROL)return FALSE;
	if (key == KEY_INPUT_F1)return FALSE;
	if (key == KEY_INPUT_F2)return FALSE;
	if (key == KEY_INPUT_F3)return FALSE;
	if (key == KEY_INPUT_F4)return FALSE;
	if (key == KEY_INPUT_F5)return FALSE;
	if (key == KEY_INPUT_F6)return FALSE;
	if (key == KEY_INPUT_F7)return FALSE;
	if (key == KEY_INPUT_F8)return FALSE;
	if (key == KEY_INPUT_F9)return FALSE;
	if (key == KEY_INPUT_F10)return FALSE;
	if (key == KEY_INPUT_F11)return FALSE;
	if (key == KEY_INPUT_F12)return FALSE;
	if (key == KEY_INPUT_RETURN)return FALSE;
	if (key == KEY_INPUT_NUMPADENTER)return FALSE;
	if (key == KEY_INPUT_UP)return FALSE;
	if (key == KEY_INPUT_DOWN)return FALSE;
	if (key == KEY_INPUT_LEFT)return FALSE;
	if (key == KEY_INPUT_RIGHT)return FALSE;

	return TRUE;
}