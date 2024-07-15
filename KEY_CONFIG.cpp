#include "KEY_CONFIG.h"
#include "Asset.h"
#include "AppContext.h"
#include "ActivityContext.h"
#include "CONSTANT_VALUE.h"
#include "GetNowCount_d.h"
#include "Get_Key_State.h"
#include "ShowFps.h"
#include "NPVsync.h"
#include "show_something.h"
#include "KeyConfigValidator.h"
#include "KeyConfigSaveLoad.h"
#include "NPLoadSoundMem.h"
#include "DxLibUtil.h"


void KEY_CONFIG(int Button[3][4], int Button_Shutter, int* Key, char* Buf, ANDROID_CONTROLLER* AC, Config config, Option* option, IR_SETTING* ir)
{
	Asset asset;//�g���摜�Z�b�g
	//�R���e�L�X�g
	AppContext appContext = AppContext(NULL, option, &config);
	ActivityContext context = ActivityContext(&appContext, &asset);

	double GAME_passed_time = 0, GAME_start_time = 0, LOOP_passed_time = 1;
	double time_cash = 0;

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->op.theme.toString());
	int H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	int SH_ERROR = NPLoadFxSoundMem(L"sound/no.wav", option);

	auto releaseResource = [&]() {
		DeleteGraph(H_BG);
		DeleteGraph(SH_ERROR);
	};

	struct Position {
		int x = 0;
		int y = 0;
	};
	Position settingKeyPosition;
	settingKeyPosition.y = 2;
	int brightness = 0;
	bool isKeyConfigSetting = true;//�L�[�R���t�B�O�����ǂ����̃t���O

	ChangeFont(NATURE_PRHYSM_FONT);
	SetFontThickness(9);
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(26);

	GAME_start_time = GetNowCount_d(config);
	Get_Key_State(Buf, Key, AC);

	NPVsync npVsync = NPVsync(config.Vsync, config.Fps);
	while (1) {
		appContext.updateTime();

		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//�o�ߎ��Ԍv�Z
		LOOP_passed_time = GAME_passed_time - time_cash;//1���[�v�ɂ����������Ԃ��Z�o
		time_cash = GAME_passed_time;


		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		Get_Key_State(Buf, Key, AC);

		if (ProcessMessage() != 0) {//�~�ŉ�ʏI��
			dxLibFinishProcess();
			return;
		}

		if (Key[KEY_INPUT_ESCAPE] == 1) {//ESC�ŉ�ʏI��
			isKeyConfigSetting = false;
		}


		for (int i = 0; i < 256; i++) {
			if (Key[i] == 1) {
				if (KeyConfigValidator::isValidKey(i)) {
					Button[settingKeyPosition.y][settingKeyPosition.x] = i;
					settingKeyPosition.x++;
					if (settingKeyPosition.x >= 4) {
						settingKeyPosition.x = 0;
						settingKeyPosition.y--;
						if (settingKeyPosition.y <= -1) {
							//�L�[�z�u�ۑ�
							KeyConfigSave(Button, &Button_Shutter);
							KeyConfigLoad(Button, &Button_Shutter);

							SetDrawBright(255, 255, 255);
							int width1;
							wchar_t str2[64];
							sprintfDx(str2, L"�L�[�z�u�ݒ��ۑ����܂���");
							width1 = GetDrawStringWidth(str2, wcslen(str2));
							ShowExtendedStrFit(640, 500, str2, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));
							ScreenFlip();
							Sleep(2000);
							isKeyConfigSetting = false;
						}
					}
					break;
				}
				else {
					PlaySoundMem(SH_ERROR, DX_PLAYTYPE_BACK, TRUE);
				}


			}
		}

		if (!isKeyConfigSetting)break;


		//Draw
		SetDrawScreen(appContext.baseHandle.getHandle());
		ClearDrawScreen();//�O���t�B�b�N��������

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		DrawGraph(0, 0, H_BG, TRUE);//�w�i
		brightness = int(255 * (1 + sin((double)GAME_passed_time / 600)) / 2);

		int configUiX = 640;
		int configUiY = 460;

		wchar_t* str1 = L"\"�ݒ肷��L�[�����ɉ����Ă�������(Esc�ŃL�����Z��)\"";
		int width1 = GetDrawStringWidth(str1, wcslen(str1));
		ShowExtendedStrFit(configUiX, configUiY - 120, str1, width1, 1280, config, GetColor(255, 255, 255), GetColor(0, 0, 0));


		int boxSize = 40;
		DrawBoxWithLine(configUiX - 80, configUiY, configUiX - 40, configUiY + boxSize, settingKeyPosition.y == 0 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
		DrawBoxWithLine(configUiX - 40, configUiY, configUiX,      configUiY + boxSize, settingKeyPosition.y == 0 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
		DrawBoxWithLine(configUiX,      configUiY, configUiX + 40, configUiY + boxSize, settingKeyPosition.y == 0 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);
		DrawBoxWithLine(configUiX + 40, configUiY, configUiX + 80, configUiY + boxSize, settingKeyPosition.y == 0 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(50, 50, 255), 150);

		configUiY += boxSize;
		DrawBoxWithLine(configUiX - 40, configUiY, configUiX,      configUiY + boxSize, settingKeyPosition.y == 1 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
		DrawBoxWithLine(configUiX,      configUiY, configUiX + 40, configUiY + boxSize, settingKeyPosition.y == 1 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
		DrawBoxWithLine(configUiX + 40, configUiY, configUiX + 80, configUiY + boxSize, settingKeyPosition.y == 1 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);
		DrawBoxWithLine(configUiX - 80, configUiY, configUiX - 40, configUiY + boxSize, settingKeyPosition.y == 1 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(50, 255, 50), 150);

		configUiY += boxSize;
		DrawBoxWithLine(configUiX - 80, configUiY, configUiX - 40, configUiY + boxSize, settingKeyPosition.y == 2 && settingKeyPosition.x == 0 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
		DrawBoxWithLine(configUiX - 40, configUiY, configUiX,      configUiY + boxSize, settingKeyPosition.y == 2 && settingKeyPosition.x == 1 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
		DrawBoxWithLine(configUiX,      configUiY, configUiX + 40, configUiY + boxSize, settingKeyPosition.y == 2 && settingKeyPosition.x == 2 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);
		DrawBoxWithLine(configUiX + 40, configUiY, configUiX + 80, configUiY + boxSize, settingKeyPosition.y == 2 && settingKeyPosition.x == 3 ? GetColor(brightness, brightness, brightness) : GetColor(255, 50, 50), 150);


		npVsync.wait(LOOP_passed_time);

		ScreenFlip();
		clsDx();

	}

	Get_Key_State(Buf, Key, AC);//�Ō�ɉ������L�[�̃t���O������������̂�h�~

	releaseResource();
	return;

}


