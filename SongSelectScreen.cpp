#include "SongSelectScreen.h"
#include "STRUCT_RESULT.h"
#include "NPLoadSoundMem.h"
#include "folder_insert.h"
#include "SaveDataSaveLoad.h"
#include "GetNowCount_d.h"
#include "WindowTitleSetter.h"
#include "Get_Key_State.h"
#include "ShowFps.h"
#include "IR_process.h"
#include "secret_LOAD.h"
#include "KEY_CONFIG.h"
#include "LearningDataGenarator.h"
#include "ScreenShot.h"
#include "number_ring.h"
#include "SHOW_RESULT.h"
#include "LoadSkillTestNpsPathList.h"
#include "OptionStateSaveLoad.h"
#include "number_digit.h"
#include "GameScreen.h"

void SongSelect::SongSelectScreen::init()
{
	asset;//�g���摜�Z�b�g
	//�R���e�L�X�g
	appContext = AppContext(NULL, option, &config);
	context = ActivityContext(&appContext, &asset);


	list_number_base = *l_n;//�ȃt�H���_�̃\�[�g�z��̃C���f�b�N�X�ɑ΂���ԍ� �I�������Ƃ���Sort���g����list_number�ϊ�����

	list_number = *l_n;//���ۂ̋ȃt�H���_���̔ԍ� �I�������Ƃ���song_number��folder�̕\���g���ĕϊ����� �I�ȉ�ʂɓ���O�Œi�ʔF�肩��߂�Ƃ��ȊO�ɍĎZ�o
	song_number = *s_n;//�I�����Ă���Ȕԍ�
	difficulty = *diff;//�I�����Ă����Փx



	coverAlpha = TransValue(&context);
	coverAlphaOpen = [this] {
		coverAlpha.clearEvent();
		coverAlpha.value = 255;
		coverAlpha.eChange(255, 128, Converter(ConvertMode::Linear), 0, 500);
		coverAlpha.play();
		};
	coverAlphaClose = [this] {
		coverAlpha.clearEvent();
		coverAlpha.value = 128;
		coverAlpha.eChange(128, 255, Converter(ConvertMode::Linear), 0, 500);
		coverAlpha.play();
		};

	viewAlpha = TransValue(&context);
	viewAlpha.value = 0;
	isViewAlphaAnimationRun = false;
	alphaAnimationOnStart = [&]() {
		if (isViewAlphaAnimationRun)return;
		isViewAlphaAnimationRun = true;
		viewAlpha.clearEvent();
		viewAlpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 500);
		viewAlpha.play();

		coverAlphaOpen();
	};


	BGM_play = 0;//BGM�Đ��t���O
	FontHandle = CreateFontToHandle(config.BaseFont.c_str(), 27, 9, DX_FONTTYPE_ANTIALIASING_EDGE, -1, 2);//�t�H���g�n���h��

	GAME_passed_time = 0;
	GAME_start_time = 0;
	LOOP_passed_time = 1;
	CounterRemainTime = 0;//�J�E���^�[�̒l��1ms���ϓ����邽�߂̎���
	time_cash = 0;
	i = 0;
	j = 0;

	CoverClosedTime = 0;//�J�o�[���܂����^�C�~���O

	activityState = FLAG_OPENING_STATE;

	Column = config.SongSelectRowNumber - 1;//�o�i�[�̏c�̗�(RowNumber) -1(���ۂɕ��Ԃ̂�Column+1��) �����ɂ���

	for (i = 0; i <= 20; i++) {//������
		title_color[i] = colorRatio(255, 255, 255);
		title_shadow_color[i] = colorRatio(0, 0, 0);
		H_TITLE_STR[i] = MakeScreen(640, 48, TRUE);
	}

	cycleIndex = [](const int value, const int range) {
		int v = value;
		if (v >= range)v -= range;
		if (v < 0)v += range;
		return v;
	};

	
	titleCycleIndex = TitleCycleIndex(config.SongSelectRowNumber);
	titleStrUpdateFlag = ALL;

	genre_width = 1;//�W���������̕�
	artist_width = 1;//�A�[�e�B�X�g���̕�
	brightness = 0;

	show_bpm = 0;//���ۂɕ\������BPM

	Highscore = (HIGHSCORE*)calloc(SONG_MAX_NUMBER, sizeof(HIGHSCORE));
	HighscoreRival = (HIGHSCORE*)calloc(SONG_MAX_NUMBER, sizeof(HIGHSCORE));



	//���ꂼ��̕����񒷂��i�[
	oi_counter = 0;
	SetFontSize(28);

	option->H_SENT = MakeScreen(640, 48, TRUE);//�I�v�V�����������̉摜�n���h���쐬
	DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);


	for (i = 0; i < OPERATION_INSTRUCTION_NUMBER; i++) {
		H_OI_STR[i] = MakeScreen(640, 100, TRUE);
		SetDrawScreen(H_OI_STR[i]);
		width_ope_ins[i] = GetDrawStringWidth(ope_ins[i], wcslen(ope_ins[i]));
		ShowExtendedStrFitToHandle(370, 1, ope_ins[i], width_ope_ins[i], 532, config, FontHandle);
	}
	SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɖ߂�


	Des1_width = GetDrawStringWidthToHandle(Des1, wcslen(Des1), FontHandle);
	Des2_width = GetDrawStringWidthToHandle(Des2, wcslen(Des2), FontHandle);

	
	for (i = 0; i <= SONG_MAX_NUMBER - 1; i++) {//������
		for (j = 0; j <= 9; j++) {
			Highscore[i].score[j] = 0;
			Highscore[i].rank[j] = 0;
			Highscore[i].clear_state[j] = CLEARTYPE_NO_PLAY;
			Highscore[i].play_count[j] = 0;
			Highscore[i].sky_perfect[j] = 0;//SKY_PERFECT��
			Highscore[i].perfect[j] = 0;//PERFECT��
			Highscore[i].good[j] = 0;//GOOD��
			Highscore[i].miss[j] = 0;//MISS��
			Highscore[i].max_combo[j] = 0;//�ő�R���{��
			Highscore[i].min_miss[j] = -1;//�ŏ��~�X��

			HighscoreRival[i].score[j] = 0;
			HighscoreRival[i].rank[j] = 0;
			HighscoreRival[i].clear_state[j] = CLEARTYPE_NO_PLAY;
			HighscoreRival[i].play_count[j] = 0;
			HighscoreRival[i].sky_perfect[j] = 0;//SKY_PERFECT��
			HighscoreRival[i].perfect[j] = 0;//PERFECT��
			HighscoreRival[i].good[j] = 0;//GOOD��
			HighscoreRival[i].miss[j] = 0;//MISS��
			HighscoreRival[i].max_combo[j] = 0;//�ő�R���{��
			HighscoreRival[i].min_miss[j] = -1;//�ŏ��~�X��
		}
	}

	for (i = 0; i <= NUMBER_OF_COURSES - 1; i++) {
		STclear_state[i] = 0;//0:���v���C 1:�s���i 2:���i
		STscore[i] = 0;//
		STsky_perfect[i] = 0;//��p�[�t�F�N�g��
		STperfect[i] = 0;//
		STgood[i] = 0;//
		STmiss[i] = 0;//
		STrank[i] = 0;//�����N(0:���v���C 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
		STplay_count[i] = 0;//����V�񂾂�
		STmax_combo[i] = 0;
		STmin_miss[i] = -1;
	}

	center.x = 640;
	center.y = 360;
	jacketArea.x = 640;
	jacketArea.y = 528;

	for (i = 0; i <= 255; i++) {//������
		filename[i] = '\0';
	}

	themeStr1 = wstring(L"img/themes/");
	themeStr2 = wstring(option->op.theme.toString());

	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());//��Փx�ɂ���ăJ�o�[�ύX
	H_DIFFICULTY[1] = LoadGraph(L"img/weather/sunny.png");

	H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
	H_DIFFICULTY[2] = LoadGraph(L"img/weather/cloudy.png");

	H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
	H_DIFFICULTY[3] = LoadGraph(L"img/weather/rainy.png");

	H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
	H_DIFFICULTY[4] = LoadGraph(L"img/weather/thunder.png");

	H_DIFFICULTY[5] = LoadGraph(L"img/weather/snowy.png");
	H_DIFFICULTY[6] = LoadGraph(L"img/weather/snowstorm.png");

	H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
	H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());

	H_OPTION = LoadGraph(L"img/cover_option_str.png");
	H_RESULT = LoadGraph(L"img/cover_title_result.png");
	H_RESULT_OBJ = LoadGraph(L"img/cover_result_obj.png");

	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
	H_BANNER_UD = LoadGraph(L"img/banner_ud.png");
	H_BANNER_AREA = LoadGraph(L"img/banner_Area.png");
	H_BANNER_ARTIST = LoadGraph(L"img/banner_Artist.png");
	H_BANNER_UD_SKIN = LoadGraph(L"img/banner_ud_skin.png");
	H_BANNER_BACK = LoadGraph(L"img/banner_back.png");
	H_BANNER_BACK_NE = LoadGraph(L"img/banner_back_ne.png");

	H_OPTION_BANNER = LoadGraph(L"img/option_banner.png");
	H_BANNER_FLAME = LoadGraph(L"img/banner_flame.png");
	H_BANNER_FLAME_SECRET = LoadGraph(L"img/banner_flame_secret.png");
	LoadDivGraph(L"img/banner_d_button.png", 6, 1, 6, 108, 80, H_BANNER_D_BUTTON);
	H_BANNER_SELECT = LoadGraph(L"img/banner_select.png");
	H_BANNER_SEASON[0] = LoadGraph(L"img/banner_spring.png");
	H_BANNER_SEASON[1] = LoadGraph(L"img/banner_summer.png");
	H_BANNER_SEASON[2] = LoadGraph(L"img/banner_autumn.png");
	H_BANNER_SEASON[3] = LoadGraph(L"img/banner_winter.png");

	H_COVER_HIGH_SCORE = LoadGraph(L"img/cover_title_high_score.png");
	H_COVER_RADAR = LoadGraph(L"img/cover_radar.png");
	H_COVER_RADAR_NAME_ROBUSTNESS = LoadGraph(L"img/radar_name_1.png");
	H_COVER_RADAR_NAME_TOTAL = LoadGraph(L"img/radar_name_2.png");


	H_COVER_POP = LoadGraph(L"img/cover_title_pop.png");
	H_COVER_SKILL_TEST_POP = LoadGraph(L"img/cover_title_pop.png");

	H_COVER_STATUS_STR = LoadGraph(L"img/cover_status_str.png");
	H_COVER_STATUS = LoadGraph(L"img/cover_title_status.png");

	LoadDivGraph(L"img/score_number.png", 10, 10, 1, 64, 100, H_SCORE_NUMBER);
	LoadDivGraph(L"img/SmallNumberRed.png", 10, 10, 1, 25, 50, H_BPM_NUMBER_MAX);
	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_BPM_NUMBER_MIN);
	LoadDivGraph(L"img/SmallNumberGreen.png", 10, 10, 1, 25, 50, H_VERSION_NUMBER);
	LoadDivGraph(L"img/SmallNumberBlack.png", 10, 10, 1, 25, 50, H_MAX_CHORDS_NUMBER);


	LoadDivGraph(L"img/RaderNumber.png", 10, 10, 1, 20, 20, H_RADER_NUMBER);

	H_VERSION_DECIMAL = LoadGraph(L"img/decimal.png");;

	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_JUDGE_NUMBER);
	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_MIN_MISS_NUMBER);
	LoadDivGraph(L"img/SmallNumberRed.png", 10, 10, 1, 25, 50, H_MAX_COMBO_NUMBER);
	LoadDivGraph(L"img/SmallNumberBlack.png", 10, 10, 1, 25, 50, H_PLAY_COUNT_NUMBER);



	H_PERCENT = LoadGraph(L"img/PERCENT.png");

	H_FAILED = LoadGraph(L"img/failed.png");
	H_CLEARED_EASY = LoadGraph(L"img/cleared_easy.png");
	H_CLEARED_NORMAL = LoadGraph(L"img/cleared_normal.png");
	H_CLEARED_HARD = LoadGraph(L"img/cleared_hard.png");
	H_CLEARED_SUPER_HARD = LoadGraph(L"img/cleared_super_hard.png");
	LoadDivGraph(L"img/FULL_COMBO.png", 6, 1, 6, 640, 100, H_FULL_COMBO);
	H_PFC = LoadGraph(L"img/PERFECT_FULLCOMBO.png");
	H_NO_PLAY = LoadGraph(L"img/no_play.png");

	H_SKILL_TEST_PASSED = LoadGraph(L"img/���i.png");
	H_SKILL_TEST_FAILED = LoadGraph(L"img/�s���i.png");
	//0:FAILED 1:CLEARED 2:FULL_COMBO 3:no play
	H_RANK[1] = LoadGraph(L"img/F.png");
	H_RANK[2] = LoadGraph(L"img/E.png");
	H_RANK[3] = LoadGraph(L"img/D.png");
	H_RANK[4] = LoadGraph(L"img/C.png");
	H_RANK[5] = LoadGraph(L"img/B.png");
	H_RANK[6] = LoadGraph(L"img/A.png");
	H_RANK[7] = LoadGraph(L"img/S.png");

	H_BUTTON_R = LoadGraph(L"img/button_R.png");
	H_BUTTON_G = LoadGraph(L"img/button_G.png");
	H_BUTTON_B = LoadGraph(L"img/button_B.png");
	H_BUTTON_PRESS = LoadGraph(L"img/button_press.png");

	H_R = LoadGraph(L"img/R.png");


	H_BPM_MINMAX_STR = LoadGraph(L"img/BPM_minmax_str.png");
	H_VER_MAX_CHORDS_STR = LoadGraph(L"img/VER_MAX_CHORDS_str.png");

	H_BPM_SLASH = LoadGraph(L"img/slash.png");

	H_DARKNESS = LoadGraph(L"img/BlackWall.png");

	H_CURSOR = LoadGraph(L"img/cursor.png");

	//NOTE�v���r���[�摜�ǂݍ���
	note_folder = option->op.note.toString();
	sprintfDx(strcash, L"img/notes/%s/%s.png", note_folder.c_str(), ReadNameRGB[1]);
	H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
	sprintfDx(strcash, L"img/notes/%s/%s.png", note_folder.c_str(), ReadNameRGB[8]);
	H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);

	note_symbol_folder = option->op.noteSymbol.toString();
	sprintfDx(strcash, L"img/note_symbol/%s/%s.png", note_symbol_folder.c_str(), ReadNameRGB[1]);
	H_OPTION_NOTE_SYMBOL_PREVIEW[0] = LoadGraph(strcash);
	sprintfDx(strcash, L"img/note_symbol/%s/%s.png", note_symbol_folder.c_str(), ReadNameRGB[8]);
	H_OPTION_NOTE_SYMBOL_PREVIEW[1] = LoadGraph(strcash);


	//�O���f�[�V�����摜�̗p��
	screenHandle = MakeScreen(640, 48, TRUE);
	int err = SetDrawScreen(screenHandle);
	// ��Z�ς݃A���t�@�p�A���t�@�u�����h�̃u�����h���[�h�ɐݒ肷��
	SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);
	err = DrawExtendGraph(0, 6, 640, 44,
		context.getAsset()->img(L"img/gradation.png").getHandle(),
		true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	SH_CLOSE = NPLoadFxSoundMem(L"sound/close.wav", option);
	SH_CLOSED = NPLoadFxSoundMem(L"sound/closed.wav", option);
	SH_OPEN = NPLoadFxSoundMem(L"sound/open.wav", option);
	SH_NO = NPLoadFxSoundMem(L"sound/no.wav", option);
	SH_SONG_SELECT = NPLoadFxSoundMem(L"sound/song_select.wav", option);
	SH_DIFFICULTY_SELECT = NPLoadFxSoundMem(L"sound/difficulty_select.wav", option);
	SH_SONG = NPLoadBgmSoundMem(Music[song_number].wavpath[difficulty], option);//�ȃf���p����
	
	SH_FOLDER_SELECT = NPLoadFxSoundMem(L"sound/folder_select.wav", option);
	SH_ALARM = NPLoadBgmSoundMem(L"sound/alarm.wav", option);
	SH_SHUTTER = NPLoadFxSoundMem(L"sound/shutter.wav", option);
	SH_SHUTTER_SIGNAL = NPLoadFxSoundMem(L"sound/shutter_signal.wav", option);

	hit_sound_folder = option->op.hitSound.toString();
	sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", hit_sound_folder.c_str());
	SH_OPTION_HITSOUND_PREVIEW = NPLoadFxSoundMem(strcash, option, 1);//HIT SOUND�v���r���[�����ǂݍ���

	updateVolume = [&]() {
		const int fxVolume = option->op.fxSoundVol.getVolume();
		const int bgmVolume = option->op.bgmSoundVol.getVolume();

		ChangeVolumeSoundMem(fxVolume, SH_CLOSE);
		ChangeVolumeSoundMem(fxVolume, SH_CLOSED);
		ChangeVolumeSoundMem(fxVolume, SH_OPEN);
		ChangeVolumeSoundMem(fxVolume, SH_NO);
		ChangeVolumeSoundMem(fxVolume, SH_SONG_SELECT);
		ChangeVolumeSoundMem(fxVolume, SH_DIFFICULTY_SELECT);
		ChangeVolumeSoundMem(bgmVolume, SH_SONG);
		ChangeVolumeSoundMem(fxVolume, SH_FOLDER_SELECT);
		ChangeVolumeSoundMem(bgmVolume, SH_ALARM);
		ChangeVolumeSoundMem(fxVolume, SH_SHUTTER);
		ChangeVolumeSoundMem(fxVolume, SH_SHUTTER_SIGNAL);
		ChangeVolumeSoundMem(fxVolume, SH_OPTION_HITSOUND_PREVIEW);
		ChangeVolumeSoundMem(bgmVolume, SH_BGM);

		};

	H_JACKET = LoadGraph(Music[song_number].jacketpath[difficulty]);//�W���P�b�g�̓ǂݍ���
	H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath[difficulty]);//�W���P�b�g�̓ǂݍ���




	
	//�i�ʔF�胊�U���g�ǂݍ���
	for (i = 0; i <= NUMBER_OF_COURSES - 2; i++) {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}

		//�ǂݍ��ރt�@�C����������
		sprintfDx(filename, L"save_data/skill_test/score/%s.dat", STList->title[i]);

		error = _wfopen_s(&fp, filename, L"rb");
		if (error != 0) {//�X�R�A�t�@�C����������Ȃ������珉�v���C
			firstplay = 1;
		}
		else {
			firstplay = 0;//�X�R�A�t�@�C��������
		}

		before_STRes.save_data_version = 0;
		before_STRes.max_combo = 0;
		before_STRes.sky_perfect = 0;
		before_STRes.min_miss = 0;

		if (firstplay == 0) {
			fread(&before_STRes, sizeof(before_STRes), 1, fp);

			STscore[i] = before_STRes.score;
			STrank[i] = before_STRes.rank;
			STclear_state[i] = before_STRes.clear;
			STplay_count[i] = before_STRes.play_count;
			STsky_perfect[i] = before_STRes.sky_perfect;//SKY_PERFECT��
			STperfect[i] = before_STRes.perfect;//PERFECT��
			STgood[i] = before_STRes.good;//GOOD��
			STmiss[i] = before_STRes.miss;//MISS��
			STmax_combo[i] = before_STRes.max_combo;//�ő�R���{
			STmin_miss[i] = before_STRes.min_miss;//�ŏ��~�X

			PlayCountSum += before.play_count;//�v���C���[�f�[�^�̑��v���C�J�E���g
			fclose(fp);
		}
	}


	//�X�R�A�ǂݍ���
	for (int side = 0; side <= 1; side++) {//�v���C���[�X�R�A(side=0),���܂��X�R�A(side=1)�̓ǂݍ���
		for (i = 0; i <= *NumberOfSongs - 1; i++) {//�Ȕԍ�
			for (j = 0; j <= 9; j++) {//��Փx
				if (ProcessMessage() != 0) {
					dxLibFinishProcess();
					return;
				}

				if (side == 0) {
					wcscpy_s(filename, Music[i].SaveFolder);
					wcscat_s(filename, L"/result_");
				}
				else if (side == 1) {
					wcscpy_s(filename, Music[i].RivalSaveFolder);
					wcscat_s(filename, L"/result_");
				}

				if (j == 1)wcscat_s(filename, L"sunny.dat");
				if (j == 2)wcscat_s(filename, L"cloudy.dat");
				if (j == 3)wcscat_s(filename, L"rainy.dat");
				if (j == 4)wcscat_s(filename, L"thunder.dat");

				if (j == 5)wcscat_s(filename, L"sunny_r.dat");
				if (j == 6)wcscat_s(filename, L"cloudy_r.dat");
				if (j == 7)wcscat_s(filename, L"rainy_r.dat");
				if (j == 8)wcscat_s(filename, L"thunder_r.dat");

				error = _wfopen_s(&fp, filename, L"rb");
				if (error != 0) {//�X�R�A�t�@�C����������Ȃ������珉�v���C
					firstplay = 1;
				}
				else {
					firstplay = 0;//�X�R�A�t�@�C��������
				}

				before.save_data_version = 0;
				before.max_combo = 0;
				before.sky_perfect = 0;
				before.min_miss = 0;

				if (firstplay == 0) {
					fread(&before, sizeof(before), 1, fp);
				}
				/*
				if (j <= 4 && firstplay == 0) {
					if (ScoreData_Authentication(before, i, j, Music) == -1) {//�Ⴄ�ȁA��Փx�̕��ʃf�[�^�������疳������(�ȒP�ȕs���h�~)
						firstplay = 1;
						fclose(fp);
					}
				}
				if (j >= 5 && firstplay == 0) {//j>=5�͓��I�v�V�����̂Ƃ�
					if (ScoreData_Authentication(before, i, j, Music) == -1) {//�Ⴄ�ȁA��Փx�̕��ʃf�[�^�������疳������(�ȒP�ȕs���h�~)
						firstplay = 1;
						fclose(fp);
					}
				}
				*/

				if (firstplay == 0) {
					if (side == 0) {
						Highscore[i].score[j] = before.score;
						Highscore[i].rank[j] = before.rank;
						Highscore[i].clear_state[j] = before.clear;

						Highscore[i].play_count[j] = before.play_count;
						Highscore[i].sky_perfect[j] = before.sky_perfect;//SKY_PERFECT��
						Highscore[i].perfect[j] = before.perfect;//PERFECT��
						Highscore[i].good[j] = before.good;//GOOD��
						Highscore[i].miss[j] = before.miss;//MISS��
						if (before.save_data_version >= RESULT_DATA_VERSION_MAX_COMBO) {
							Highscore[i].max_combo[j] = before.max_combo;//�ő�R���{���L�^�o�[�W�����ȍ~�̃Z�[�u�f�[�^�Ȃ�ǂݍ���
						}
						if (before.save_data_version >= RESULT_DATA_VERSION_MIN_MISS) {
							Highscore[i].min_miss[j] = before.min_miss;//�ŏ��~�X�L�^�o�[�W�����ȍ~�̃Z�[�u�f�[�^�Ȃ�ǂݍ���
							if (before.clear == CLEARTYPE_PLAY && Highscore[i].min_miss[j] == 0) {//MISS 0�Ȃ�PLAY��Ԃł��t���R���{��\������悤�ɂ���
								Highscore[i].clear_state[j] = CLEARTYPE_FULL_COMBO;
							}
							if (before.clear == CLEARTYPE_PLAY && Highscore[i].min_miss[j] == 0 && Highscore[i].good[j] == 0) {//GOOD 0�Ȃ�PLAY��Ԃł�PERFECT��\������悤�ɂ���
								Highscore[i].clear_state[j] = CLEARTYPE_PERFECT;
							}
						}
						else {//����ȑO�̃Z�[�u�f�[�^
							if (Highscore[i].clear_state[j] == CLEARTYPE_FULL_COMBO || Highscore[i].clear_state[j] == CLEARTYPE_PERFECT) {//�t���R���ς݂Ȃ�
								Highscore[i].min_miss[j] = 0;
							}
							else if (Highscore[i].clear_state[j] == CLEARTYPE_EASY_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_HARD_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_SUPER_HARD_CLEARED) {//�N���A�ς�
								Highscore[i].min_miss[j] = Highscore[i].miss[j];
							}
							else {
								Highscore[i].min_miss[j] = -1;
							}
						}

						PlayCountSum += before.play_count;//�v���C���[�f�[�^�̑��v���C�J�E���g
						fclose(fp);
					}
					else if (side == 1) {
						HighscoreRival[i].score[j] = before.score;
						HighscoreRival[i].rank[j] = before.rank;
						HighscoreRival[i].clear_state[j] = before.clear;
						HighscoreRival[i].play_count[j] = before.play_count;
						HighscoreRival[i].sky_perfect[j] = before.sky_perfect;//SKY_PERFECT��
						HighscoreRival[i].perfect[j] = before.perfect;//PERFECT��
						HighscoreRival[i].good[j] = before.good;//GOOD��
						HighscoreRival[i].miss[j] = before.miss;//MISS��
						if (before.save_data_version >= RESULT_DATA_VERSION_MAX_COMBO) {
							HighscoreRival[i].max_combo[j] = before.max_combo;//�ő�R���{���L�^�o�[�W�����ȍ~�̃Z�[�u�f�[�^�Ȃ�ǂݍ���
						}

						if (before.save_data_version >= RESULT_DATA_VERSION_MIN_MISS) {
							HighscoreRival[i].min_miss[j] = before.min_miss;//�ŏ��~�X�L�^�o�[�W�����ȍ~�̃Z�[�u�f�[�^�Ȃ�ǂݍ���
							if (HighscoreRival[i].min_miss[j] == 0) {//MISS 0�Ȃ�PLAY��Ԃł��t���R���{��\������悤�ɂ���
								HighscoreRival[i].clear_state[j] = CLEARTYPE_FULL_COMBO;
							}
							if (HighscoreRival[i].min_miss[j] == 0 && HighscoreRival[i].good[j] == 0) {//GOOD 0�Ȃ�PLAY��Ԃł�PERFECT��\������悤�ɂ���
								HighscoreRival[i].clear_state[j] = CLEARTYPE_PERFECT;
							}
						}
						else {//����ȑO�̃Z�[�u�f�[�^
							if (HighscoreRival[i].clear_state[j] == CLEARTYPE_FULL_COMBO || HighscoreRival[i].clear_state[j] == CLEARTYPE_PERFECT) {//�t���R���ς݂Ȃ�
								HighscoreRival[i].min_miss[j] = 0;
							}
							else if (HighscoreRival[i].clear_state[j] == CLEARTYPE_EASY_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_HARD_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_SUPER_HARD_CLEARED) {//�N���A�ς�
								HighscoreRival[i].min_miss[j] = HighscoreRival[i].miss[j];
							}
							else {
								HighscoreRival[i].min_miss[j] = -1;
							}
						}
						fclose(fp);
					}
				}
				//fclose(fp);
			}
		}
	}


	

	for (int diff = 1; diff < 5; diff++) {//��Փx
		for (j = 0; j < folder->folder_c[FolderIndex::ALL_SONGS]; j++) {
			if (folder->folder[FolderIndex::ALL_SONGS][j].kind == 0 && Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].exist[diff] == 1) {//�u�t�H���_�I���ɖ߂�v�ł͂Ȃ����݂��镈��
				auto global = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].global[1][diff];
				auto local = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].local[1][diff];
				auto chain = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].chain[1][diff];
				auto unstability = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].unstability[1][diff];
				auto streak = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].longNote[1][diff];
				auto color = Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].color[1][diff];
				auto total = global + local + chain + unstability + streak + color;

				auto score = Highscore[folder->folder[FolderIndex::ALL_SONGS][j].song_number].score[diff];

				radar_skill_list.global.add((double)score / 10000 * global, j, diff);
				radar_skill_list.local.add((double)score / 10000 * local, j, diff);
				radar_skill_list.chain.add((double)score / 10000 * chain, j, diff);
				radar_skill_list.unstability.add((double)score / 10000 * unstability, j, diff);
				radar_skill_list.streak.add((double)score / 10000 * streak, j, diff);
				radar_skill_list.color.add((double)score / 10000 * color, j, diff);
				radar_skill_list.robustness.add((double)score / 10000 * total, j, diff);

			}
		}
	}

	skill_radar_result.global = radar_skill_list.global.average();
	skill_radar_result.local = radar_skill_list.local.average();
	skill_radar_result.chain = radar_skill_list.chain.average();
	skill_radar_result.unstability = radar_skill_list.unstability.average();
	skill_radar_result.streak = radar_skill_list.streak.average();
	skill_radar_result.color = radar_skill_list.color.average();
	skill_radar_result.robustness = radar_skill_list.robustness.average();

	//�X�L�����[�_�[�Ώە��ʂ̃t�H���_�ǉ�
	reset_folder_skill_radar(folder);
	for (const auto& i : radar_skill_list.global.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_GLOBAL, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_GLOBAL, 1);

	for (const auto& i : radar_skill_list.local.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_LOCAL, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_LOCAL, 1);

	for (const auto& i : radar_skill_list.chain.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_CHAIN, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_CHAIN, 1);

	for (const auto& i : radar_skill_list.unstability.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_UNSTABILITY, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_UNSTABILITY, 1);

	for (const auto& i : radar_skill_list.streak.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_STREAK, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_STREAK, 1);

	for (const auto& i : radar_skill_list.color.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_COLOR, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_COLOR, 1);

	for (const auto& i : radar_skill_list.robustness.list) {
		if (i.score == 0)continue;
		folder_insert_skill_radar(folder, i.song_number, i.difficulty, FolderIndex::SKILL_RADAR_ROBUSTNESS, 0);
	}
	folder_insert_skill_radar(folder, *NumberOfSongs, 1, FolderIndex::SKILL_RADAR_ROBUSTNESS, 1);

	


	//�t�H���_���̐��т��Z�o
	for (int rainbow = 0; rainbow < 2; rainbow++) {
		int count = 0;

		for (i = 0; i < NUMBER_OF_FOLDERS; i++) {
			if (folder->FolderKind[i] == FOLDER_KIND_NORMAL) {//��Փx���ɎZ�o
				for (int diff = 1; diff < 5; diff++) {//��Փx
					//FolderScoreRaderBuf FolderScoreRaderBuf;
					count = 0;
					for (j = 0; j < folder->folder_c[i]; j++) {
						if (folder->folder[i][j].kind == 0 && Music[folder->folder[i][j].song_number].exist[diff] == 1) {//�u�t�H���_�I���ɖ߂�v�ł͂Ȃ����݂��镈��
							FolderScore[rainbow][i][diff].AverageScore += Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4];
							double ScoreRate = (double)Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4] / 10000;

							FolderScore[rainbow][i][diff].clearState[clearStateConverter(Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4])]++;
							FolderScore[rainbow][i][diff].rank[Highscore[folder->folder[i][j].song_number].rank[diff + rainbow * 4]]++;

							if (FolderScore[rainbow][i][diff].ClearType > Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4]) {//�t�H���_�ɂ��镈�ʂ̒��ň�ԒႢ�N���A��Ԃ����̃t�H���_�̃N���A��Ԃɂ���
								FolderScore[rainbow][i][diff].ClearType = Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4];
							}

							count++;

						}
					}
					if (count != 0)FolderScore[rainbow][i][diff].AverageScore = (double)FolderScore[rainbow][i][diff].AverageScore / count;

				}

			}
			else if (folder->FolderKind[i] == FOLDER_KIND_DIFFICULTY) {//�t�H���_�P�ʂŎZ�o

				//FolderScoreRaderBuf FolderScoreRaderBuf;
				count = 0;



				for (j = 0; j < folder->folder_c[i]; j++) {
					int diff = folder->folder[i][j].difficulty;

					if (folder->folder[i][j].kind == 0 && Music[folder->folder[i][j].song_number].exist[diff] == 1) {//�u�t�H���_�I���ɖ߂�v�ł͂Ȃ����݂��镈��
						FolderScore[rainbow][i][1].AverageScore += Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4];
						double ScoreRate = (double)Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4] / 10000;

						FolderScore[rainbow][i][1].clearState[clearStateConverter(Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4])]++;
						FolderScore[rainbow][i][1].rank[Highscore[folder->folder[i][j].song_number].rank[diff + rainbow * 4]]++;

						if (FolderScore[rainbow][i][1].ClearType > Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4]) {//�t�H���_�ɂ��镈�ʂ̒��ň�ԒႢ�N���A��Ԃ����̃t�H���_�̃N���A��Ԃɂ���
							FolderScore[rainbow][i][1].ClearType = Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4];
						}

						count++;

					}
				}
				if (count != 0)FolderScore[rainbow][i][1].AverageScore = (double)FolderScore[rainbow][i][1].AverageScore / count;


				for (int diff = 2; diff <= 4; diff++) {//��Փx�Œ�t�H���_�̓t�H���_�I�����ɂǂ̕��ʓ�Փx��I��ł��Ă������l��\�����邽�߂ɒl���R�s�[
					FolderScore[rainbow][i][diff].AverageScore = FolderScore[rainbow][i][1].AverageScore;

					FolderScore[rainbow][i][diff].ClearType = FolderScore[rainbow][i][1].ClearType;

					for (j = 0; j < 9; j++)FolderScore[rainbow][i][diff].clearState[j] = FolderScore[rainbow][i][1].clearState[j];
					for (j = 0; j < 8; j++)FolderScore[rainbow][i][diff].rank[j] = FolderScore[rainbow][i][1].rank[j];
				}

			}
			//�t�H���_�̃����N���� (0:���v���C 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
			for (int diff = 1; diff < 5; diff++) {//��Փx
				if (FolderScore[rainbow][i][diff].AverageScore >= RANK_S_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_S;
				}
				else if (FolderScore[rainbow][i][diff].AverageScore >= RANK_A_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_A;
				}
				else if (FolderScore[rainbow][i][diff].AverageScore >= RANK_B_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_B;
				}
				else if (FolderScore[rainbow][i][diff].AverageScore >= RANK_C_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_C;
				}
				else if (FolderScore[rainbow][i][diff].AverageScore >= RANK_D_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_D;
				}
				else if (FolderScore[rainbow][i][diff].AverageScore >= RANK_E_SCORE) {
					FolderScore[rainbow][i][diff].folderRank = RANK_E;
				}
				else {
					FolderScore[rainbow][i][diff].folderRank = RANK_F;
				}
				if (FolderScore[rainbow][i][diff].AverageScore == 0) {
					FolderScore[rainbow][i][diff].folderRank = RANK_NONE;
				}

			}
		}
	}


	//�Z�[�u�f�[�^�ǂݍ���

	if (loadSaveData(&saveData) == -1) {
		//����쐬��
		saveData.totalPlayCount = PlayCountSum;
		saveData.totalHitNotes = PlayCountSum * 500;//�b��Œ@�������������i�[
		if (saveData.totalPlayCount >= SAVE_DATA_VALUE_LIMIT)saveData.totalPlayCount = SAVE_DATA_VALUE_LIMIT;
		if (saveData.totalHitNotes >= SAVE_DATA_VALUE_LIMIT)saveData.totalHitNotes = SAVE_DATA_VALUE_LIMIT;
	}


	saveData.totalHighScore = 0;
	saveData.totalHighScoreRainbow = 0;
	for (int diff = 1; diff < 5; diff++) {//��Փx
		for (j = 0; j < folder->folder_c[FolderIndex::ALL_SONGS]; j++) {
			if (folder->folder[FolderIndex::ALL_SONGS][j].kind == 0 && Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].exist[diff] == 1) {//�u�t�H���_�I���ɖ߂�v�ł͂Ȃ����݂��镈��
				saveData.totalHighScore += Highscore[folder->folder[FolderIndex::ALL_SONGS][j].song_number].score[diff];
				saveData.totalHighScoreRainbow += Highscore[folder->folder[FolderIndex::ALL_SONGS][j].song_number].score[diff + 4];
			}
		}
	}
	if (saveData.totalHighScore >= SAVE_DATA_VALUE_LIMIT)saveData.totalHighScore = SAVE_DATA_VALUE_LIMIT;
	if (saveData.totalHighScoreRainbow >= SAVE_DATA_VALUE_LIMIT)saveData.totalHighScoreRainbow = SAVE_DATA_VALUE_LIMIT;
	writeSaveData(saveData);


	for (int rainbow = 0; rainbow < 2; rainbow++) {//���R�Ǘ��Z�p�Ҍ���t�H���_�Ń����N�A�N���A��Ԃ�\�����Ȃ��悤�v���C��Ԉ����ɂ���
		for (int diff = 1; diff < 5; diff++) {
			FolderScore[rainbow][FolderIndex::SKILL_TEST_NUMBER][diff].ClearType = CLEARTYPE_PLAY;
		}
	}

	//�ȃ��X�g�̃\�[�g
	for (int SortKind = 0; SortKind < option->op.sort.itemCount() + 10; SortKind++) {//�\�[�g��ނ̐������p��[*][][][][]
		SortList.push_back(vector<vector<vector<vector<SortSongListIndex>>>>());
		for (int FolderInd = 0; FolderInd < NUMBER_OF_FOLDERS; FolderInd++) {//�\�[�g��ނ��ƂɃt�H���_�̐���ǉ�[][*][][][]
			SortList[SortKind].push_back(vector<vector<vector<SortSongListIndex>>>());

			for (int RainbowInd = 0; RainbowInd < 2; RainbowInd++) {//�t�H���_�����X�g���ɐ��K�A���̃\�[�g�Ώۂ̒l��ǉ�[][][*][][]
				SortList[SortKind][FolderInd].push_back(vector<vector<SortSongListIndex>>());
				for (int DifficultyInd = 0; DifficultyInd < 4; DifficultyInd++) {//���K�A���̃\�[�g�Ώۖ��ɓ�Փx��ǉ�[][][][*][] �~���m���t�H���_�ł͂ǂ���g���Ă�����
					SortList[SortKind][FolderInd][RainbowInd].push_back(vector<SortSongListIndex>());

					for (int ListInd = 0; ListInd < folder->folder_c[FolderInd]; ListInd++) {//�t�H���_�̐����ƂɃt�H���_�����X�g����ǉ�[][][][][*]
						SortList[SortKind][FolderInd][RainbowInd][DifficultyInd].push_back(SortSongListIndex());

						SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].index = ListInd;
						if (SortKind == (int)OptionItem::Sort::DEFAULT) {
							SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = ListInd;
						}
						else if (SortKind == (int)OptionItem::Sort::LEVEL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Music[folder->folder[FolderInd][ListInd].song_number].level[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Music[folder->folder[FolderInd][ListInd].song_number].level[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::SCORE) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].score[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].score[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::CLEAR_STATE) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].clear_state[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].clear_state[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MIN_MISS) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].min_miss[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].min_miss[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::PLAY_COUNT) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].play_count[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].play_count[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::RADAR_TOTAL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value =
									(int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][folder->folder[FolderInd][ListInd].difficulty] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][folder->folder[FolderInd][ListInd].difficulty] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][folder->folder[FolderInd][ListInd].difficulty] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][folder->folder[FolderInd][ListInd].difficulty] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][folder->folder[FolderInd][ListInd].difficulty] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];


							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value =
									(int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][DifficultyInd + 1] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][DifficultyInd + 1] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][DifficultyInd + 1] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][DifficultyInd + 1] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][DifficultyInd + 1] +
									(int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][DifficultyInd + 1];

							}
						}
						else if (SortKind == (int)OptionItem::Sort::GLOBAL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::LOCAL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::CHAIN) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::UNSTABILITY) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::STREAK) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::COLOR) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind >= (int)OptionItem::Sort::RED_DENSITY && SortKind <= (int)OptionItem::Sort::RAINBOW_DENSITY) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].ColorNotesAmount[RainbowInd][folder->folder[FolderInd][ListInd].difficulty][SortKind - (int)OptionItem::Sort::RED_DENSITY];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].ColorNotesAmount[RainbowInd][DifficultyInd + 1][SortKind - (int)OptionItem::Sort::RED_DENSITY];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MAX_BPM) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmax[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmax[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MIN_BPM) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmin[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmin[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MAX_CHORDS) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].maxChords[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].maxChords[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::VERSION) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//�~���m���ʃt�H���_�ł͓�Փx���w�肵�Ă���
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].version[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].version[DifficultyInd + 1];
							}
						}
					}
				}
			}
		}
	}

	//�\�[�g����
	for (int SortKind = 0; SortKind < option->op.sort.itemCount(); SortKind++) {//�\�[�g�̎d���̐������p��[*][][][][]
		for (int FolderInd = 0; FolderInd < NUMBER_OF_FOLDERS; FolderInd++) {//�\�[�g��ނ��ƂɃt�H���_�̐���ǉ�[][*][][][]

			for (int RainbowInd = 0; RainbowInd < 2; RainbowInd++) {//�t�H���_�����X�g���ɐ��K�A���̃\�[�g�Ώۂ̒l��ǉ�[][][][*][]
				for (int DifficultyInd = 0; DifficultyInd < 4; DifficultyInd++) {//���K�A���̃\�[�g�Ώۖ��ɓ�Փx��ǉ�[][][][][*] �~���m���t�H���_�ł͂ǂ���g���Ă�����


					stable_sort(&SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][0],
						&SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][folder->folder_c[FolderInd] - 1]);

				}
			}

		}
	}

	//2��ڈȍ~�̑I�ȉ�ʂɖ߂��Ă����Ƃ��̂��߂�list_number_base��list_number�Z�o(�i�ʔF�肩��߂��Ă����Ƃ��͍s��Ȃ�)
	if (folder->selected_folder != FolderIndex::SKILL_TEST_NUMBER) {
		//�\�[�g���ʂ��O�ƕς���Ă����̂��߂ɁA���̋Ȕԍ��Ɠ����ԍ��A������Փx���w��list_number_base��T��
		int now_song_number = song_number;
		int now_difficulty = difficulty;
		int notSortedIndex = 0;//�\�[�g�O�̃��X�g�̃C���f�b�N�X���w��
		for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {//�\�[�g��̃��X�g�̃C���f�b�N�X���w��
			//�\�[�g�O�̃��X�g�̃C���f�b�N�X�ɕϊ�
			notSortedIndex = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.getIndex() == (int)OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index;

			if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY) {//��Փx�ʃt�H���_��I��ł������͋Ȕԍ��Ɠ�Փx�������Ă��邩�m�F����
				//�O��I��ł����Ȕԍ��Ɠ�Փx����v���Ă����炱�̃\�[�g�O�̃��X�g�̃C���f�b�N�X��������
				if (now_song_number ==
					folder->folder[folder->selected_folder][notSortedIndex].song_number

					&&

					now_difficulty ==
					folder->folder[folder->selected_folder][notSortedIndex].difficulty
					) {


					list_number_base = SearchListNumberBase;
				}
			}
			else if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_NORMAL) {//�ʏ�t�H���_��I��ł������͋Ȕԍ����������Ă��邩�m�F����
				//�O��I��ł����Ȕԍ�����v���Ă����炱�̃\�[�g�O�̃��X�g�̃C���f�b�N�X��������
				if (now_song_number ==
					folder->folder[folder->selected_folder][notSortedIndex].song_number) {


					list_number_base = SearchListNumberBase;
				}
			}
		}

		//list_number_base�����������̂�list_number�Z�o
		list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.getIndex() == (int)OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;
	}

	if (PlayCountSum <= 2) {//3��ڂ̃v���C�ォ��t�H���_�I���ɖ߂���Ȃ��Ȃ�
		*SelectingTarget = SELECTING_FOLDER;
	}


	StrAnnounce = announce_str(*StageCount, PlayCountSum);
	AnnounceWidth =//�A�i�E���X������
		GetDrawStringWidthToHandle(//���������擾
			StrAnnounce, //�A�i�E���X���o��
			wcslen(StrAnnounce),//�����悤�ɂ��̕������������Ƃ��ē���Ă��
			FontHandle);


	int rank_point = result_rank_buf[0] + result_rank_buf[1] + result_rank_buf[2];
	if (secret->total != 0 && secret->song_appear_number == -1) {//�B���Ȃ������đO�ɔ��������B���ȉ��o���������Ă���(-1)�Ƃ�

		if ((rank_point >= 5 * 3) || SECRET_ALWAYS_APPEAR) {//�ߋ�3��̃����N������B(�l��5)�ȏ�ŉB���ȉ��o��
			if (secret->all_get == 1) {
				secret->secret_song_appear_number = GetRand(secret->total - 1);//�����Ȃ̉B���Ȕԍ��������_���ɑI��œ����
				secret->song_appear_number = secret->song_number[secret->secret_song_appear_number];//�Ȕԍ��ɕϊ�
			}
			else {//�S�ȉ��ւ��Ă��Ȃ�
				do {
					secret->secret_song_appear_number = GetRand(secret->total - 1);//�����Ȃ̉B���Ȕԍ��������_���ɑI��œ����
					if (secret->get_song_number[secret->secret_song_appear_number] == 0) {//�����ւ̉B���ȋȔԍ��Ȃ炻��ɂ���
						secret->song_appear_number = secret->song_number[secret->secret_song_appear_number];//�Ȕԍ��ɕϊ�
						break;
					}
				} while (1);//�����ւ̋Ȕԍ���I�Ԃ܂ł�蒼��
			}
			secret_LOAD_contents(Music, secret->song_appear_number, secret);//�A�i�E���X���̓ǂݍ���(secret->Announce�Ɋi�[)

			secret->Announce_width =
				GetDrawStringWidthToHandle(//���������擾
					secret->Announce, //�A�i�E���X���o��
					wcslen(secret->Announce),//�����悤�ɂ��̕������������Ƃ��ē���Ă��
					FontHandle);

			if (secret->all_get == 0)*SelectingTarget = SELECTING_FOLDER;//�S���ւ��Ă��Ȃ��Ƃ��t�H���_�I����Ԃɂ���(�A�i�E���X���������邽��)
		}
	}



	//�A�i�E���X�p������摜�쐬
	if (secret->song_appear_number != -1) {//�B���ȏo�����o��
		H_ANNOUNSE = MakeScreen(secret->Announce_width + 20, 100, TRUE);
		SetDrawScreen(H_ANNOUNSE);
		ShowExtendedStrFitToHandle((secret->Announce_width + 20) / 2, 1, secret->Announce, secret->Announce_width, secret->Announce_width + 20, config, FontHandle, secret->Color, secret->ShadowColor);
		SetDrawScreen(DX_SCREEN_BACK);
	}
	else {
		H_ANNOUNSE = MakeScreen(AnnounceWidth + 20, 100, TRUE);
		SetDrawScreen(H_ANNOUNSE);
		ShowExtendedStrFitToHandle((AnnounceWidth + 20) / 2, 1, StrAnnounce, AnnounceWidth, AnnounceWidth + 20, config, FontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));
		SetDrawScreen(DX_SCREEN_BACK);
	}




	if (secret->song_appear_number == -1) {
		if (rank_point == 14) {//�����N�̍��v�����x14�������� (S����A���Ŏ��ƂȂ� BBC�ł��Ȃ�)
			SH_BGM = NPLoadBgmSoundMem(L"sound/song_select_bgm2.ogg", option);//�I��BGM�̓ǂݍ���
		}
		else if (rank_point >= 12) {//A���A���܂���BBE,CCC��
			SH_BGM = NPLoadBgmSoundMem(L"sound/song_select_bgm3.ogg", option);//�I��BGM�̓ǂݍ���

		}
		else {
			SH_BGM = NPLoadBgmSoundMem(L"sound/song_select_bgm.ogg", option);//�I��BGM�̓ǂݍ���
		}
		
		H_CLOUD = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cloud.png")).c_str());
	}
	else if (secret->song_appear_number != -1) {//�B���ȉ��o��
		if (wcscmp(L"\0", secret->BGM) == 0) {//�����w�肳��Ă��Ȃ�������
			SH_BGM = NPLoadBgmSoundMem(L"sound/song_select_secret_bgm.ogg", option);//�B���ȑI��BGM�̓ǂݍ���
		}
		else {
			SH_BGM = NPLoadBgmSoundMem(secret->BGM, option);//��p�I��BGM�̓ǂݍ���
		}
		H_CLOUD = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cloud_black.png")).c_str());//�B���ȉ��o�Ȃ獕���_
	}

	if (secret->song_appear_number != -1) {//�B���ȉ��o��
		if (wcscmp(L"\0", secret->BGM) != 0 && Music[secret->song_appear_number].secret == UnlockState::Secret) {//�����։B���Ȃ̏o�����Ő�pBGM����������
			BGM_continue = 1;//BGM��炵������
		}
		else {
			BGM_continue = 0;//BGM�̓t�H���_�I����ʂ̂ݗ���
		}
	}
	else {
		BGM_continue = 0;//BGM�̓t�H���_�I����ʂ̂ݗ���
	}


	if (!option->op.color.isRainbow()) {//���I�v�V�����I���t���O�̐ݒ�
		select_rainbow = 0;
	}
	if (option->op.color.isRainbow()) {
		select_rainbow = 4;
	}

	ChangeFont(config.BaseFont.c_str());
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(28);
	SetFontThickness(9);

	//����։B���ȂŃA���[���炷�ݒ�̋Ȃ͏o�����ɃA���[����炷
	if (secret->get_song_number[secret->secret_song_appear_number] == 0 && secret->beep_alarm == 1) {
		//NMC�ً}����\��
		H_NMC = LoadGraph(L"img/NMC.png");
		DrawGraph(0, 0, H_NMC, TRUE);
		ScreenFlip();

		//0.1�b�҂�
		Sleep(100);

		PlaySoundMem(SH_ALARM, DX_PLAYTYPE_BACK, TRUE);
		while (CheckSoundMem(SH_ALARM) == 1) {//�Ȃ�I���܂őҋ@
			if (ProcessMessage() != 0) {//�~�ŃQ�[���I��
				dxLibFinishProcess();
				return;
			}
		}
	}

	if (*SelectingTarget == SELECTING_COURSE)optionListView.setSkillTestMode(true);

	GAME_start_time = GetNowCount_d(config);
	Announse_show_time_base = GetNowCount() + 1500;//�A�i�E���X�\���̊����

	auto flag = ShowFlag(); flag.version = true; flag.autoPlay = true; flag.scoreRanking = true; flag.keyConfig = true;
	WindowTitleSetter::setText(flag);
}



void SongSelect::SongSelectScreen::updateModel()
{
	appContext.updateTime();
	coverAlpha.process();

	if (ProcessMessage() != 0) {//�~�ŏI��
		dxLibFinishProcess();
		return;
	}

	//BGM�Đ�
	if (BGM_play == 0 && GAME_passed_time >= 1000) {
		PlaySoundMem(SH_BGM, DX_PLAYTYPE_LOOP, TRUE);
		BGM_play = 1;
	}
	if (activityState != FLAG_END_FUNCTION_STATE) {//�V���b�^�[�܂����炱���ł�BGM�̉��ʒ����͍s��Ȃ�
		int bgmVolume = option->op.bgmSoundVol.getVolume();
		ChangeVolumeSoundMem(int(bgm_vol_ratio * bgmVolume), SH_BGM);
	}

	//CALC
	GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//�o�ߎ��Ԍv�Z
	LOOP_passed_time = GAME_passed_time - time_cash;//1���[�v�ɂ����������Ԃ��Z�o
	time_cash = GAME_passed_time;

	CounterRemainTime -= int(CounterRemainTime);
	CounterRemainTime += LOOP_passed_time;
	CRTBuf = int(CounterRemainTime);

	Get_Key_State(Buf, Key, AC);

	ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

	alphaAnimationOnStart();

	if (activityState == FLAG_SELECT_STATE) {
		int PressFrame = int(25.0 * (17.0 / LOOP_passed_time));//�{�^�����������ăJ�[�\��������������悤�ɂȂ�܂ł̃t���[��
		if (PressFrame <= 0)PressFrame = 1;//0�ȉ��ɂ͂��Ȃ�

		//printfDx(L"%d\n", PressFrame);
		if (Key[KEY_INPUT_ESCAPE] == 1 && activityState != FLAG_CLOSING_STATE && activityState != FLAG_END_FUNCTION_STATE) {//ESC�ŉ�ʏI��
			activityState = FLAG_CLOSING_STATE;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			coverAlphaClose();
			*isBackToTitle = true;
			//dxLibFinishProcess();
		}

		if (Key[KEY_INPUT_F1] == 1) {//�f�o�b�O���[�hONOFF
			if (*debug == 0) {
				*debug = 1;
				PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
			}
			else {
				*debug = 0;
				PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
			}
		}

		if (Key[KEY_INPUT_F2] == 1) {//�X�R�A�đ��M&�����L���O�\��
			if (*SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {//���݂��镈�ʂ�I��ł��鎞
				PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
				IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color.isRainbow(), config);
				IRview(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, option->op.color.isRainbow(), config);
			}
			else if (*SelectingTarget == SELECTING_FOLDER) {
				//�X�L�����[�_�[�����L���O���J��
				PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
				viewSkillRadarRanking(config);
			}
			//if (ScoreShowMode == 0) {
			//	ScoreShowMode = 1;
			//	PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
			//}
			//else {
			//	ScoreShowMode = 0;
			//	PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
			//}
		}

		if (Key[KEY_INPUT_F3] == 1) {//�L�[�R���t�B�O���
			KEY_CONFIG(Button, Button_Shutter, Key, Buf, AC, config, option, ir);
		}

		if (Key[KEY_INPUT_F9] == 1 && SEND_EXIST_SCORE_TO_IR) {//�����X�R�A�𑗐M
			RESULT IrScore;
			IrScore.difficulty = difficulty;
			IrScore.clear = Highscore[song_number].clear_state[difficulty + select_rainbow];
			IrScore.rank = Highscore[song_number].rank[difficulty + select_rainbow];
			IrScore.score = Highscore[song_number].score[difficulty + select_rainbow];
			IrScore.sky_perfect = Highscore[song_number].sky_perfect[difficulty + select_rainbow];
			IrScore.perfect = Highscore[song_number].perfect[difficulty + select_rainbow];
			IrScore.good = Highscore[song_number].good[difficulty + select_rainbow];
			IrScore.miss = Highscore[song_number].miss[difficulty + select_rainbow];
			IrScore.min_miss = Highscore[song_number].min_miss[difficulty + select_rainbow];
			IrScore.max_combo = Highscore[song_number].max_combo[difficulty + select_rainbow];
			IrScore.play_count = Highscore[song_number].play_count[difficulty + select_rainbow];

			IRsave(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, IrScore, difficulty, Music[song_number].season[difficulty], option->op.color.isRainbow(), 0, config);
			IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color.isRainbow(), config);
		}

		if (Key[KEY_INPUT_F4] == 1) {
			LearningDataGenarator predDataGenerator = LearningDataGenarator();
			predDataGenerator.openFile();
			predDataGenerator.writeData(&Music[song_number], difficulty);
		}

		if (Key[Button_Shutter] == 1) {//�X�N���[���V���b�g
			ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
		}


		//�I�v�V�������ONOFF����(���̏������s�����t���[���ł͑��̑�����󂯕t���Ȃ�)
		if (OptionOpen == 0 && (Key[Button[0][0]] == 1 || Key[Button[2][0]] == 1)) {//�I�v�V�������ON
			OptionOpen = 1;//�I�v�V�����I�����[�h�ɂ���
			optionListView.show();
			PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
			time_base_str = int(GetNowCount_d(config));
		}
		else if (OptionOpen == 1 && (Key[Button[0][0]] == 1 || Key[Button[2][0]] == 1 || Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1)) {//�I�v�V�������OFF
			OptionOpen = 0;//�I�ȃ��[�h�ɂ���
			optionListView.hide();
			PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
			time_base_str = int(GetNowCount_d(config));
		}
		else {//���̏������s��Ȃ������t���[��

			if (Key[Button[0][3]] == 1 || Key[Button[2][3]] == 1) {//���U���g���ONOFF
				if (ShowResultFlag == 0) {
					ShowResultFlag = 1;//���U���g�\�����[�h�ɂ���
					PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					ShowResultFlag = 0;//�I�ȃ��[�h�ɂ���
					PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
				}
			}


			if (OptionOpen == 0 && *SelectingTarget == SELECTING_SONG) {//�I�v�V�����o���ĂȂ��ċȑI�����[�h�̂Ƃ�
				if ((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
					&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1)) {//�㉺���������Ńt�H���_�Z���N�g�ɖ߂�
					*SelectingTarget = SELECTING_FOLDER;
					StopSoundMem(SH_SONG);
					PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
					song_play_counter = 1;
					jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
					jacket_alpha = 0;
					widthCalcFlag = 1;
					titleStrUpdateFlag = ALL;
				}
				else {
					if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
						if (difficulty != 1) {
							PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

							if ((wcscmp(Music[song_number].wavpath[difficulty], Music[song_number].wavpath[difficulty - 1]) != 0//�p�X���Ⴄ
								|| (Music[song_number].demostart[difficulty] != Music[song_number].demostart[difficulty - 1]))//���ꂩ�f���X�^�[�g���Ⴄ
								|| ((Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty - 1]))) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);
								song_play_counter = 1;
							}

							if (
								(wcscmp(Music[song_number].jacketpath[difficulty], Music[song_number].jacketpath[difficulty - 1]) != 0)
								|| (Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty - 1])
								) {
								jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
								jacket_alpha = 0;
							}
							widthCalcFlag = 1;
							titleStrUpdateFlag = ALL;
							//�ʓ�Փx�Ō���list_number�Ɠ����ԍ����w��list_number_base��T��
							int now_list_number = list_number;
							for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
								if (now_list_number == SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1 - 1][SearchListNumberBase].index) {
									list_number_base = SearchListNumberBase;
								}
							}
						}
						difficulty--;
						if (difficulty > 4)difficulty = 4;
						if (difficulty < 1)difficulty = 1;
					}

					if (Key[Button[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {
						if (difficulty != 4) {
							PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

							if ((wcscmp(Music[song_number].wavpath[difficulty], Music[song_number].wavpath[difficulty + 1]) != 0
								|| (Music[song_number].demostart[difficulty] != Music[song_number].demostart[difficulty + 1]))
								|| ((Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty + 1]))) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);
								song_play_counter = 1;
							}
							if (
								(wcscmp(Music[song_number].jacketpath[difficulty], Music[song_number].jacketpath[difficulty + 1]) != 0)
								|| (Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty + 1])
								) {
								jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
								jacket_alpha = 0;
							}
							widthCalcFlag = 1;
							titleStrUpdateFlag = ALL;
							//�ʓ�Փx�Ō���list_number�Ɠ����ԍ����w��list_number_base��T��
							int now_list_number = list_number;
							for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
								if (now_list_number == SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty + 1 - 1][SearchListNumberBase].index) {
									list_number_base = SearchListNumberBase;
								}
							}


						}
						difficulty++;
						if (difficulty > 4)difficulty = 4;
						if (difficulty < 1)difficulty = 1;
					}

					if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
						StopSoundMem(SH_SONG);
						//DeleteSoundMem(SH_SONG);
						int beforeSongNumber = song_number;
						int beforeDifficulty = difficulty;
						int beforeDifficultyShouldBe = folder->folder[folder->selected_folder][list_number].difficulty;

						list_number_base--;
						list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

						list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base].index;

						song_number = folder->folder[folder->selected_folder][list_number].song_number;

						titleStrUpdateFlag = TOP;
						if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//���x���ʃt�H���_�̎��Łu�t�H���_�I���ɖ߂�v����Ȃ��Ƃ��͓�Փx���ς��Ă���
							difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
							bool isNeedAllTitleStrUpdate = Music[beforeSongNumber].exist[beforeDifficulty] == false || beforeDifficulty != beforeDifficultyShouldBe;
							if (isNeedAllTitleStrUpdate) {
								titleStrUpdateFlag = ALL;
							}
						}

						bn_draw_counter--;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						song_play_counter = 1;

						jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
						jacket_alpha = 0;
						widthCalcFlag = 1;
						titleCycleIndex.minus();
					}
					else {
						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);
								int beforeSongNumber = song_number;
								int beforeDifficulty = difficulty;
								int beforeDifficultyShouldBe = folder->folder[folder->selected_folder][list_number].difficulty;

								list_number_base--;
								list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

								list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base].index;
								song_number = folder->folder[folder->selected_folder][list_number].song_number;

								titleStrUpdateFlag = TOP;
								if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//���x���ʃt�H���_�̎��Łu�t�H���_�I���ɖ߂�v����Ȃ��Ƃ��͓�Փx���ς��Ă���
									difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
									bool isNeedAllTitleStrUpdate = Music[beforeSongNumber].exist[beforeDifficulty] == false || beforeDifficulty != beforeDifficultyShouldBe;
									if (isNeedAllTitleStrUpdate) {
										titleStrUpdateFlag = ALL;
									}
								}

								bn_draw_counter--;
								roll_counter = 1;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								song_play_counter = 1;

								jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
								jacket_alpha = 0;
								widthCalcFlag = 1;
								titleCycleIndex.minus();
							}
						}
					}


					if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
						StopSoundMem(SH_SONG);
						//DeleteSoundMem(SH_SONG);
						int beforeSongNumber = song_number;
						int beforeDifficulty = difficulty;
						int beforeDifficultyShouldBe = folder->folder[folder->selected_folder][list_number].difficulty;

						list_number_base++;
						list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

						list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base].index;
						song_number = folder->folder[folder->selected_folder][list_number].song_number;

						titleStrUpdateFlag = BOTTOM;
						if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//���x���ʃt�H���_�̎��Łu�t�H���_�I���ɖ߂�v����Ȃ��Ƃ��͓�Փx���ς��Ă���
							difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
							bool isNeedAllTitleStrUpdate = Music[beforeSongNumber].exist[beforeDifficulty] == false || beforeDifficulty != beforeDifficultyShouldBe;
							if (isNeedAllTitleStrUpdate) {
								titleStrUpdateFlag = ALL;
							}
						}

						bn_draw_counter++;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						song_play_counter = 1;

						jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
						jacket_alpha = 0;
						widthCalcFlag = 1;
						titleCycleIndex.plus();
					}
					else {
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);

								int beforeSongNumber = song_number;
								int beforeDifficulty = difficulty;
								int beforeDifficultyShouldBe = folder->folder[folder->selected_folder][list_number].difficulty;

								list_number_base++;
								list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

								list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base].index;
								song_number = folder->folder[folder->selected_folder][list_number].song_number;

								titleStrUpdateFlag = BOTTOM;
								if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//���x���ʃt�H���_�̎��Łu�t�H���_�I���ɖ߂�v����Ȃ��Ƃ��͓�Փx���ς��Ă���
									difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
									bool isNeedAllTitleStrUpdate = Music[beforeSongNumber].exist[beforeDifficulty] == false || beforeDifficulty != beforeDifficultyShouldBe;
									if (isNeedAllTitleStrUpdate) {
										titleStrUpdateFlag = ALL;
									}
								}

								bn_draw_counter++;
								roll_counter = 1;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								song_play_counter = 1;

								jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
								jacket_alpha = 0;
								widthCalcFlag = 1;
								titleCycleIndex.plus();
							}
						}
					}


					if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//�Ȍ���
						if (folder->folder[folder->selected_folder][list_number].kind == 0) {//���e���u�t�H���_�I���ɖ߂�v����Ȃ��āu�ȁv�̂Ƃ�
							if ((Music[song_number].exist[difficulty] == 1 && Music[song_number].secret != UnlockState::Secret)//���̓�Փx�����݂��ĉB���Ȃł͂Ȃ�
								|| (Music[song_number].exist[difficulty] == 1 && Music[song_number].secret == UnlockState::Secret && secret->song_appear_number == song_number)//���ꂩ�B���Ȃŏo���ΏۂɂȂ��Ă���Ƃ�
								) {
								activityState = FLAG_CLOSING_STATE;
								PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
								coverAlphaClose();
							}
							else {
								PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);//�I�ȕs��
							}

						}
						else {//�u�t�H���_�I���ɖ߂�v�����肵���Ƃ�
							*SelectingTarget = SELECTING_FOLDER;

							StopSoundMem(SH_SONG);

							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							song_play_counter = 1;

							jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
							jacket_alpha = 0;
							widthCalcFlag = 1;
							titleStrUpdateFlag = ALL;
							Get_Key_State(Buf, Key, AC);//�������̌��菈���ɔ������Ȃ��悤�ɂ�����x�L�[���͔c������
						}
					}
				}
			}

			if (OptionOpen == 0 && *SelectingTarget == SELECTING_COURSE) {//�i�ʔF��I�����[�h�̂Ƃ�
				if ((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
					&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1)
					|| Key[KEY_INPUT_BACK] >= 1
					) {//�㉺���������Ńt�H���_�Z���N�g�ɖ߂�
					*SelectingTarget = SELECTING_FOLDER;
					PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
					widthCalcFlag = 1;
					optionListView.setSkillTestMode(false);
					titleStrUpdateFlag = ALL;
				}
				else {
					if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
						list_number--;
						list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
						list_number_base = list_number;
						bn_draw_counter--;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						widthCalcFlag = 1;
						titleStrUpdateFlag = TOP;
						titleCycleIndex.minus();
					}
					else {
						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								list_number--;
								list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
								list_number_base = list_number;
								bn_draw_counter--;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								titleStrUpdateFlag = TOP;
								titleCycleIndex.minus();
							}
						}
					}

					if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
						list_number++;
						list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
						list_number_base = list_number;
						bn_draw_counter++;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						widthCalcFlag = 1;
						titleStrUpdateFlag = BOTTOM;
						titleCycleIndex.plus();
					}
					else {
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								list_number++;
								list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
								list_number_base = list_number;
								bn_draw_counter++;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								titleStrUpdateFlag = BOTTOM;
								titleCycleIndex.plus();

							}
						}
					}

					if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//�R�[�X����
						if (STList->Kind[list_number] != 2) {//���e���u�t�H���_�I���ɖ߂�v����Ȃ��āu�R�[�X�v�̂Ƃ�
							activityState = FLAG_CLOSING_STATE;
							PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
							coverAlphaClose();
						}
						else {//�u�t�H���_�I���ɖ߂�v�����肵���Ƃ�
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							*SelectingTarget = SELECTING_FOLDER;
							widthCalcFlag = 1;
							titleStrUpdateFlag = ALL;
							Get_Key_State(Buf, Key, AC);//�������̌��菈���ɔ������Ȃ��悤�ɂ�����x�L�[���͔c������
							optionListView.setSkillTestMode(false);
						}
					}
				}
			}


			if (OptionOpen == 0 && *SelectingTarget == SELECTING_FOLDER) {//�t�H���_�I�����[�h�̂Ƃ�

				if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
					if (difficulty != 1) {
						PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
						widthCalcFlag = 1;
						titleStrUpdateFlag = ALL;
					}
					difficulty--;
					if (difficulty > 4)difficulty = 4;
					if (difficulty < 1)difficulty = 1;
				}

				if (Key[Button[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {
					if (difficulty != 4) {
						PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
						widthCalcFlag = 1;
						titleStrUpdateFlag = ALL;
					}
					difficulty++;
					if (difficulty > 4)difficulty = 4;
					if (difficulty < 1)difficulty = 1;
				}


				if (!((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
					&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1))
					) {//�㉺�����������Ă��瓮�����Ȃ�
					if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {

						folder->selected_folder--;
						folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);

						bn_draw_counter--;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);


						widthCalcFlag = 1;
						titleStrUpdateFlag = TOP;
						titleCycleIndex.minus();
					}
					else {
						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								folder->selected_folder--;
								folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);
								bn_draw_counter--;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								titleStrUpdateFlag = TOP;
								titleCycleIndex.minus();
								//time_base_str = GetNowCount_d(config);
							}
						}
					}

					if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
						folder->selected_folder++;
						folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);

						bn_draw_counter++;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);


						widthCalcFlag = 1;
						titleStrUpdateFlag = BOTTOM;
						titleCycleIndex.plus();
					}
					else {
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								folder->selected_folder++;
								folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);
								bn_draw_counter++;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								titleStrUpdateFlag = BOTTOM;
								titleCycleIndex.plus();
								//time_base_str = GetNowCount_d(config);
							}
						}
					}

				}

				if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//�t�H���_����
					if (folder->folder_c[folder->selected_folder] != 0) {//���̃t�H���_�ɋȂ����݂�����(�Ȑ���0����Ȃ��Ƃ�)
						PlaySoundMem(SH_FOLDER_SELECT, DX_PLAYTYPE_BACK, TRUE);

						if (folder->FolderKind[folder->selected_folder] != FOLDER_KIND_SKILL_TEST) {//�i�ʔF��ȊO�̃t�H���_���J�����Ƃ�
							*SelectingTarget = SELECTING_SONG;
							list_number_base = 0;

							list_number = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base].index;

							song_number = folder->folder[folder->selected_folder][list_number].song_number;

							if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY) {//���x���ʃt�H���_���J�������͓�Փx���ς��Ă���
								difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
							}

						}
						else {//�i�ʔF��t�H���_���J������
							list_number = 0;//�R�[�X�I��ԍ���0�� song_number�͏������K�v����
							list_number_base = list_number;
							*SelectingTarget = SELECTING_COURSE;
							optionListView.setSkillTestMode(true);

						}


					}
					else {
						PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);//�I�ȕs��
					}

					song_play_counter = 1;
					jacket_show_counter = 1;//�W���P�b�g�̓ǂݍ���
					jacket_alpha = 0;

					widthCalcFlag = 1;
					titleStrUpdateFlag = ALL;
				}
			}

			if (OptionOpen == 1) {//�I�v�V�����I����ʂ̎�
				auto changeRow = [&](bool isSelectUp) {
					if (isSelectUp)option_select--;
					else option_select++;
					option_select = number_ring((int)option_select, option->OPTION_NUM - 1);//OPTION_NUM���߂�
					DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);

					if (option_select > OptionShowEnd) {
						OptionShowEnd = option_select;
						OptionShowStart = option_select - (OptionShowAmount - 1);
					}
					else if (option_select < OptionShowStart) {
						OptionShowEnd = option_select + (OptionShowAmount - 1);
						OptionShowStart = option_select;
					}

					PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
					time_base_str = int(GetNowCount_d(config));

					if (isSelectUp)optionListView.moveToSelectUp();
					else optionListView.moveToSelectDown();

					};

				if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
					changeRow(true);
				}
				if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] > PressFrame) {
					if (roll_counter == 0) {
						changeRow(true);
						roll_counter = 1;
					}
				}
				if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
					changeRow(false);
				}
				if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] > PressFrame) {
					if (roll_counter == 0) {
						changeRow(false);
						roll_counter = 1;
					}
				}

				//���������
				auto deleteCoverImage = [&]() {
					DeleteGraph(H_BG);
					DeleteGraph(H_BG);
					DeleteGraph(H_COVER[1]);
					DeleteGraph(H_COVER[2]);
					DeleteGraph(H_COVER[3]);
					DeleteGraph(H_COVER[4]);
					DeleteGraph(H_COVER_SKILL_TEST);
					DeleteGraph(H_COVER_OPTION);
					DeleteGraph(H_COVER_MIDDLE);
					};

				auto changeOption = [&](int indexStep) {
					time_base_str = int(GetNowCount_d(config));
					OptionValueChange(option, option_select, indexStep);
					DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);
					if (option_select != (int)OptionItem::Name::HITSOUND)PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

					if (option_select == (int)OptionItem::Name::COLOR || option_select == (int)OptionItem::Name::SORT) {
						//����list_number�Ɠ����ԍ����w��list_number_base��T��
						int now_list_number = list_number;
						for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
							if (now_list_number == SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][SearchListNumberBase].index) {
								list_number_base = SearchListNumberBase;
							}
						}
						widthCalcFlag = 1;//�\�[�g��ނ�ύX�����Ƃ��͋Ȗ��摜�ĕ`��
						titleStrUpdateFlag = ALL;
					}

					if (option_select == (int)OptionItem::Name::NOTE) {
						DeleteGraph(H_OPTION_NOTE_PREVIEW[0]);
						DeleteGraph(H_OPTION_NOTE_PREVIEW[1]);
						sprintfDx(strcash, L"img/notes/%s/%s.png", option->op.note.toString().c_str(), ReadNameRGB[1]);
						H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
						sprintfDx(strcash, L"img/notes/%s/%s.png", option->op.note.toString().c_str(), ReadNameRGB[8]);
						H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);
					}
					if (option_select == (int)OptionItem::Name::NOTE_SYMBOL) {
						DeleteGraph(H_OPTION_NOTE_SYMBOL_PREVIEW[0]);
						DeleteGraph(H_OPTION_NOTE_SYMBOL_PREVIEW[1]);
						sprintfDx(strcash, L"img/note_symbol/%s/%s.png", option->op.noteSymbol.toString().c_str(), ReadNameRGB[1]);
						H_OPTION_NOTE_SYMBOL_PREVIEW[0] = LoadGraph(strcash);
						sprintfDx(strcash, L"img/note_symbol/%s/%s.png", option->op.noteSymbol.toString().c_str(), ReadNameRGB[8]);
						H_OPTION_NOTE_SYMBOL_PREVIEW[1] = LoadGraph(strcash);
					}
					if (option_select == (int)OptionItem::Name::HITSOUND) {
						DeleteSoundMem(SH_OPTION_HITSOUND_PREVIEW);
						sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->op.hitSound.toString().c_str());
						SH_OPTION_HITSOUND_PREVIEW = NPLoadHitSoundMem(strcash, option);//HIT SOUND�v���r���[�����ǂݍ���
						PlaySoundMem(SH_OPTION_HITSOUND_PREVIEW, DX_PLAYTYPE_BACK, TRUE);
					}
					if (option_select == (int)OptionItem::Name::THEME) {
						deleteCoverImage();
						wstring themeStr1(L"img/themes/");
						wstring themeStr2(option->op.theme.toString());
						H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
						H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());
						H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
						H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
						H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
						H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
						H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());
						H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
						optionListView.setCoverImage();
					}

					if (option_select == (int)OptionItem::Name::HITSOUNDVOL) {
						updateVolume();
					}

					if (option_select == (int)OptionItem::Name::FXSOUNDVOL) {
						updateVolume();
					}

					if (option_select == (int)OptionItem::Name::BGMSOUNDVOL) {
						updateVolume();
					}

					if (indexStep == -1)optionListView.pushLeftArrow();
					if (indexStep == 1)optionListView.pushRightArrow();

					optionListView.updateSelectedOptionItem();
					};


				if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
					changeOption(-1);
				}

				if (Key[Button[1][0]] > PressFrame || Key[KEY_INPUT_LEFT] > PressFrame) {//�����������Ƃ�
					if (roll_counter == 0) {
						changeOption(-1);
						roll_counter = 1;
					}
				}

				if (Key[Button[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {
					changeOption(1);
				}


				if (Key[Button[1][3]] > PressFrame || Key[KEY_INPUT_RIGHT] > PressFrame) {//�����������Ƃ�
					if (roll_counter == 0) {
						changeOption(1);
						roll_counter = 1;
					}
				}
			}

		}

		//�����{�^���𗣂��Ă�����I�v�V�����̖��摜��񊈐��ɂ���
		if (Key[Button[1][0]] == 0 && Key[KEY_INPUT_LEFT] == 0) {
			optionListView.releaseLeftArrow();
		}
		if (Key[Button[1][3]] == 0 && Key[KEY_INPUT_RIGHT] == 0) {
			optionListView.releaseRightArrow();
		}

		if (!option->op.color.isRainbow()) {//���I�v�V�����I���t���O�̐ݒ�
			select_rainbow = 0;
		}
		if (option->op.color.isRainbow()) {
			select_rainbow = 4;
		}

		for (i = 0; i < CRTBuf; i++) {
			if (roll_counter > 0) {
				roll_counter -= 0.01;
			}
			if (roll_counter < 0) {
				roll_counter = 0;
			}
		}
	}

	//printfDx(L"LOOP2:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);



	for (i = 0; i < CRTBuf; i++) {
		if (bn_draw_counter > 0) {
			bn_draw_counter -= bn_draw_counter / 140;
		}
		if (bn_draw_counter < 0) {
			bn_draw_counter -= bn_draw_counter / 140;
		}
		if (fabs(bn_draw_counter * 1.0) <= 0.01 && bn_draw_counter != 0) {
			bn_draw_counter = 0;
		}



		if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number && *SelectingTarget == SELECTING_SONG) {//�B���Ȃ̓��[�_�[,�O���t��\�����Ȃ�
			DRShowTotal += 0.01 * ((double)0 - DRShowTotal);
			DRShowGlobal += 0.01 * ((double)0 - DRShowGlobal);
			DRShowLocal += 0.01 * ((double)0 - DRShowLocal);
			DRShowChain += 0.01 * ((double)0 - DRShowChain);
			DRShowUnstability += 0.01 * ((double)0 - DRShowUnstability);
			DRShowStreak += 0.01 * ((double)0 - DRShowStreak);
			DRShowColor += 0.01 * ((double)0 - DRShowColor);

			for (j = 0; j < 9; j++) {
				ShowLocalNotesGraph[j] += 0.01 * (0 - ShowLocalNotesGraph[j]);
				ShowColorNotesGraph[j] += 0.01 * (0 - ShowColorNotesGraph[j]);
			}
		}
		else {//�ʏ�ȁA���֋Ȃ̓��[�_�[,�O���t�\��
			int R = 0;
			if (option->op.color.isRainbow())R = 1;

			int total =
				Music[song_number].global[R][difficulty] +
				Music[song_number].local[R][difficulty] +
				Music[song_number].chain[R][difficulty] +
				Music[song_number].unstability[R][difficulty] +
				Music[song_number].longNote[R][difficulty] +
				Music[song_number].color[R][difficulty];

			if (*SelectingTarget == SELECTING_SONG) {
				DRShowGlobal += 0.01 * ((double)Music[song_number].global[R][difficulty] - DRShowGlobal);
				DRShowLocal += 0.01 * ((double)Music[song_number].local[R][difficulty] - DRShowLocal);
				DRShowChain += 0.01 * ((double)Music[song_number].chain[R][difficulty] - DRShowChain);
				DRShowUnstability += 0.01 * ((double)Music[song_number].unstability[R][difficulty] - DRShowUnstability);
				DRShowStreak += 0.01 * ((double)Music[song_number].longNote[R][difficulty] - DRShowStreak);
				DRShowColor += 0.01 * ((double)Music[song_number].color[R][difficulty] - DRShowColor);
				DRShowTotal += 0.01 * ((double)total - DRShowTotal);
			}
			else if (*SelectingTarget == SELECTING_FOLDER) {
				DRShowGlobal += 0.01 * (skill_radar_result.global - DRShowGlobal);
				DRShowLocal += 0.01 * (skill_radar_result.local - DRShowLocal);
				DRShowChain += 0.01 * (skill_radar_result.chain - DRShowChain);
				DRShowUnstability += 0.01 * (skill_radar_result.unstability - DRShowUnstability);
				DRShowStreak += 0.01 * (skill_radar_result.streak - DRShowStreak);
				DRShowColor += 0.01 * (skill_radar_result.color - DRShowColor);
				DRShowTotal += 0.01 * (skill_radar_result.robustness - DRShowTotal);
			}


			for (j = 0; j < 9; j++) {
				ShowLocalNotesGraph[j] += 0.01 * (Music[song_number].LocalNotesAmount[R][difficulty][j] - ShowLocalNotesGraph[j]);
				ShowColorNotesGraph[j] += 0.01 * (Music[song_number].ColorNotesAmount[R][difficulty][j] - ShowColorNotesGraph[j]);
			}
		}
		if (DRShowGlobal < 0)DRShowGlobal = 0;
		if (DRShowLocal < 0)DRShowLocal = 0;
		if (DRShowChain < 0)DRShowChain = 0;
		if (DRShowUnstability < 0)DRShowUnstability = 0;
		if (DRShowStreak < 0)DRShowStreak = 0;
		if (DRShowColor < 0)DRShowColor = 0;
		if (DRShowTotal < 0)DRShowTotal = 0;

		for (j = 0; j < 9; j++) {
			if (ShowLocalNotesGraph[j] < 0)ShowLocalNotesGraph[j] = 0;
			if (ShowColorNotesGraph[j] < 0)ShowColorNotesGraph[j] = 0;
		}

		if (song_play_counter >= 0) {
			song_play_counter -= 0.002;
		}

		if (jacket_show_counter >= 0) {
			jacket_show_counter -= 0.005;
		}

		if (button_draw_counter <= 1) {
			button_draw_counter += 0.0004;
		}
		if (button_draw_counter > 1) {
			button_draw_counter = 0;
		}

		if (OptionOpen == 1) {//
			if (option_draw_counter <= 1) {
				option_draw_counter += 0.003;
			}
			if (option_draw_counter > 1) {
				option_draw_counter = 1;
			}
		}
		if (OptionOpen == 0) {
			if (option_draw_counter >= 0) {
				option_draw_counter -= 0.003;
			}
			if (option_draw_counter < 0) {
				option_draw_counter = 0;
			}
		}
		if (ShowResultFlag == 1) {
			if (result_draw_counter <= 1) {
				result_draw_counter += 0.003;
			}
			if (result_draw_counter > 1) {
				result_draw_counter = 1;
			}
		}
		if (ShowResultFlag == 0) {
			if (result_draw_counter >= 0) {
				result_draw_counter -= 0.003;
			}
			if (result_draw_counter < 0) {
				result_draw_counter = 0;
			}
		}

		if (jacket_alpha >= 1) {
			jacket_alpha = 1;
		}
		if (jacket_alpha < 1 && CheckHandleASyncLoad(H_JACKET) == FALSE && jacket_show_counter == -2) {
			if (Music[song_number].secret != UnlockState::Secret || secret->song_appear_number == song_number) {
				//�B���Ȃł͂Ȃ��܂��͉B���Ȃ̏o�����̎��̂݃W���P�b�g��\������
				jacket_alpha += 0.003;
			}
		}

		if (*SelectingTarget == SELECTING_FOLDER || *SelectingTarget == SELECTING_COURSE) {//�t�H���_,�R�[�X�Z���N�g��
			if (BGM_continue == 0) {
				if (bgm_vol_ratio < 1) {
					bgm_vol_ratio += 0.001;
				}
				if (bgm_vol_ratio >= 1) {
					bgm_vol_ratio = 1;
				}
			}
		}


		if (*SelectingTarget == SELECTING_SONG) {//�ȑI������BGM�̉���0�ɂȂ߂炩�Ɉڍs
			if (BGM_continue == 0) {
				if (bgm_vol_ratio > 0) {
					bgm_vol_ratio -= 0.01;
				}
				if (bgm_vol_ratio <= 0) {
					bgm_vol_ratio = 0;
				}
			}
		}
	}


	if (*SelectingTarget == SELECTING_SONG) {
		if (song_play_counter < 0 && song_play_counter != -1 && song_play_counter != -2 && song_play_counter != -3 && BGM_continue == 0) {//��p�I��BGM�̂��関���։B���Ȃ̏o�����ȊO�ŋȓǂݍ���
			song_play_counter = -1;
			SetUseASyncLoadFlag(TRUE);

			SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);
			DeleteSoundMem(SH_SONG);
			SH_SONG = NPLoadBgmSoundMem(Music[song_number].wavpath[difficulty], option);

			SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

			SetUseASyncLoadFlag(FALSE);
		}
	}

	if (*SelectingTarget == SELECTING_SONG) {
		if (jacket_show_counter < 0 && jacket_show_counter != -1 && jacket_show_counter != -2) {//�W���P�b�g�ǂݍ���
			jacket_show_counter = -1;
			jacket_alpha = 0;
			SetUseASyncLoadFlag(TRUE);
			DeleteGraph(H_JACKET);
			DeleteGraph(H_JACKET_GAUSS);
			H_JACKET = LoadGraph(Music[song_number].jacketpath[difficulty]);//�W���P�b�g�̓ǂݍ���
			H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath[difficulty]);//�ڂ������̃W���P�b�g�̓ǂݍ���
			SetUseASyncLoadFlag(FALSE);
		}
	}

	//ProcessMessage();

	if (*SelectingTarget == SELECTING_SONG) {
		if (CheckHandleASyncLoad(SH_SONG) == FALSE && song_play_counter == -1) {//�Ȃ̓ǂݍ��݂��I�������̂ōĐ�
			song_play_counter = -2;
			SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
			if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�炳�Ȃ�
				//�炳�Ȃ�
			}
			else {//�B���Ȃł͂Ȃ�(���֍ς�)�Ȃ̂Ŗ炷
				PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
			}
		}

		//�Ȃ̍Đ����I�������烋�[�v�Đ�
		if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�炳�Ȃ�
			//�炳�Ȃ�
		}
		else {//�B���Ȃł͂Ȃ�(���֍ς�)�Ȃ̂Ŗ炷
			if (song_play_counter == -2 && CheckSoundMem(SH_SONG) == 0) {
				SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
				PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
			}
		}

	}

	brightness = int(155 + 100 * (1 + sin((double)GAME_passed_time / 300)) / 2);

	if (activityState == FLAG_END_FUNCTION_STATE) {
		//�f���̃t�F�[�h�A�E�g
		if (demo_vol > 0) {
			demo_vol = 1.0 - ((GAME_passed_time - CoverClosedTime) / 1200);
		}
		if (demo_vol < 0) {
			demo_vol = 0;
		}
		int bgmVolume = option->op.bgmSoundVol.getVolume();

		ChangeVolumeSoundMem(int(demo_vol * bgmVolume), SH_SONG);//�f���̉��ʂ͋Ȍ����Ƀt�F�[�h�A�E�g
		if (BGM_continue == 1) {

			ChangeVolumeSoundMem(int(demo_vol * bgmVolume), SH_BGM);//BGM�L��B���ȏo������BGM���t�F�[�h�A�E�g
		}
		if (*SelectingTarget == SELECTING_COURSE || *SelectingTarget == SELECTING_FOLDER) {
			ChangeVolumeSoundMem(int(demo_vol * bgmVolume), SH_BGM);//�i�ʃR�[�X�A�ȃt�H���_��I��ł��鎞��BGM���t�F�[�h�A�E�g
		}
	}

	if (activityState == FLAG_END_FUNCTION_STATE && GAME_passed_time - CoverClosedTime >= 1200) {//�ȑI��

		for (i = 0; i <= 40; i++) {
			/*
			if (ProcessMessage() != 0 || Key[KEY_INPUT_ESCAPE] == 1 && activityState != FLAG_CLOSING_STATE) {//ESC�ŃQ�[���I��
				dxLibFinishProcess();
				return;
			}
			*/
			Sleep(1);
		}


		DeleteFontToHandle(FontHandle);//�t�H���g�n���h���폜
		InitSoundMem();
		InitGraph();//�O���t�B�b�N�������J��


		if (*isBackToTitle == false) {//�^�C�g���ɖ߂�ꍇ���t��ʂɂ͑J�ڂ��Ȃ�
			if (*SelectingTarget == SELECTING_SONG) {//�ȑI���̎�

				int gauge_buf = (int)option->op.gauge.getIndex();//���I��ł���Q�[�W��ނ�ۑ�
				int AllowExit = 1;//�r���ޏo�\��
				if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number == song_number) {//�B���ȏo�����ɂ��̉B���Ȃ�I�������̂�
					//debug������
					*debug = 0;
					//�Q�[�W��ތ���
					if (SECRET_GAUGE_CHANGE_OFF == 0) {

						int rank_point = result_rank_buf[0] + result_rank_buf[1] + result_rank_buf[2];


						if (rank_point == 5 * 3) {//�S��B�����N(rank_point==15)
							option->op.gauge.setIndex((int)OptionItem::Gauge::FC_ATTACK);//����FC ATTACK�Q�[�W
						}
						else if (rank_point < 6 * 3) {//���ς�A�����N��菬�����l(rank_point==16~17)
							option->op.gauge.setIndex((int)OptionItem::Gauge::SUPER_HARD);//����SUPER HARD�Q�[�W
						}
						else if (rank_point < 7 * 3) {//���ς�S�����N��菬�����l(rank_point==18~20)
							option->op.gauge.setIndex((int)OptionItem::Gauge::HARD);//����HARD�Q�[�W
						}
						else {//�S��S�����N(rank_point==21)
							option->op.gauge.setIndex((int)OptionItem::Gauge::NORMAL);//����NORMAL�Q�[�W
						}
					}

					AllowExit = 0;
				}

				int CourseCombo = 0;
				int CourseMaxCombo = 0;

				int TryCount = 0;//�g���C��(�N�C�b�N���g���C���邽�тɉ��Z)
				do {
					TryCount++;
					escape = 0;

					//�I�Ȕԍ��Ɠ�Փx��n���ăQ�[����ʂ�
					auto game_screen = Game::GameScreen(
						song_number,difficulty,&res,
						&escape, option, &retryAble,
						debug, Music, Button, Button_Shutter, Key, Buf, secret->song_appear_number != -1, AC,
						config, ir,
						0, NULL, &CourseCombo, &CourseMaxCombo, AllowExit
					);
					game_screen.run();

					Get_Key_State(Buf, Key, AC);
				} while ((Key[Button[0][0]] >= 1 || Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[Button[0][3]] >= 1)
					&& (Key[Button[1][0]] >= 1 || Key[Button[1][1]] >= 1 || Key[Button[1][2]] >= 1 || Key[Button[1][3]] >= 1)
					&& (Key[Button[2][0]] >= 1 || Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[Button[2][3]] >= 1)
					&& (retryAble == 1)
					&& (secret->song_appear_number != song_number || Music[song_number].secret != UnlockState::Secret)//���g���C�ł���Ƃ��̏���(�B���ȉ��o���̖����։B���ȃv���C�̓��g���C�ł��Ȃ�)
					);

				RESULT STResDummy;
				//���t�I���㏈��
				if (escape == 0
					|| (*debug == 0
						&& escape == 1
						&& secret->song_appear_number == song_number
						&& secret->get_song_number[secret->secret_song_appear_number] == 0)) {
					//�r���Ŕ����o���Ă��Ȃ�,�܂��͔����o���ĂĂ���f�o�b�O���[�h�ł��ꂪ�B���ȉ��o���̖����։B���ȂȂ�N���A���s�����ɂ���

					if (escape == 1 && secret->song_appear_number == song_number) {//�B���ȃv���C�̔����o���̓����N��F��
						res.rank = 1;
					}

					SHOW_RESULT(res, option, song_number, difficulty, debug, Music, Button, Button_Shutter, Key, Buf, AC, TryCount,
						STList,
						list_number, config, ir);//���ʔ��\
					if (*debug == 0) {
						if (secret->secret_song_appear_number == -1) {//�ʏ�v���C�ŉB���ȉ��o�ɂȂ��Ă��Ȃ��Ƃ�
							result_rank_buf[*result_count] = res.rank;//�����N��ۑ�

							(*result_count)++;
							if (*result_count == SECRET_SONG_APPEAR_CYCLE) {
								(*result_count) = 0;
							}
						}
						if (secret->secret_song_appear_number != -1) {//�B���ȉ��o�̌�
							*result_count = 0;
							for (i = 0; i <= SECRET_SONG_APPEAR_CYCLE - 1; i++) {
								result_rank_buf[i] = 0;//�����N�o�b�t�@��������
							}

							if (res.clear >= CLEARTYPE_EASY_CLEARED && song_number == secret->song_appear_number) {//�ΏۋȂ��N���A���Ă���
								//���֏����X�V
								secret->get_song_number[secret->secret_song_appear_number] = 1;//���֍ς݂ɂ���
								Music[secret->song_appear_number].secret = UnlockState::Unlocked;
								//�S���ւ������m�F
								secret_all_get(secret);//�S���ւ���Ă���Ȃ�all_get��1
							}
							//�B���ȉ��o�I��
							secret->secret_song_appear_number = -1;
							secret->song_appear_number = -1;
						}
					}
					option->op.gauge.setIndex((int)(OptionItem::Gauge)gauge_buf);//�Q�[�W�����Ƃɖ߂�
				}

			}
			else if (*SelectingTarget == SELECTING_COURSE) {//�i�ʔF��R�[�X�I���̎�
				//*debug = 0;//debug������
				//�Ώۋȃp�X�ǂݍ���

				//�I�v�V�����𒼂�
				int gauge_buf = (int)option->op.gauge.getIndex();//��Ŗ߂����߂̃o�b�t�@

				option->op.gauge.setIndex((int)OptionItem::Gauge::SKILL_TEST);

				if (option->op.lane.getIndex() != (int)OptionItem::Lane::MIRROR) {//MIRROR�ȊO��I�񂾂Ƃ��͐��K�ɂ���
					option->op.lane.setIndex((int)OptionItem::Lane::NONE);
				}


				option->op.color.setIndex((int)OptionItem::Color::NONE);



				//��{�Ȃ���I�v�V�����ɂ���
				if (STList->Kind[list_number] == 0) {
					option->op.color.setIndex((int)OptionItem::Color::RAINBOW);
				}

				int SongNumberList[4];//�Ȕԍ����X�g
				int DifficultyList[4];//��Փx���X�g

				LoadSkillTestNpsPathList(SongNumberList, DifficultyList, STList->fliepath[list_number], Music, *NumberOfSongs);//nps�t�@�C���p�X�ǂݍ���
				RESULT STRes;//�i�ʔF�胊�U���g
				STRes.clear = 1;//�����l�͍��i�ɂ��Ă���

				RESULT Result[4];//�e�Ȃ̃��U���g

				double GaugeVal = 100;

				int CourseCombo = 0;
				int CourseMaxCombo = 0;
				int AllowExit = 1;

				int playedSongIndex = 0;
				//1~4�Ȗ�
				for (i = 0; i <= 3; i++) {
					int TryCount = 1;//�g���C��

					//�I�Ȕԍ��Ɠ�Փx��n���ăQ�[����ʂ�
					auto game_screen = Game::GameScreen(
						SongNumberList[i], DifficultyList[i],
						&Result[i], &escape, option, &retryAble,
						debug, Music, Button, Button_Shutter, Key, Buf, 0, AC, config, ir,
						i + 1, &GaugeVal,
						&CourseCombo, &CourseMaxCombo, AllowExit
					);
					game_screen.run();

					if (escape == 1) {//�v���C�̔����o���̓����N��F�AFAILED��
						Result[i].rank = 1;
						Result[i].clear = CLEARTYPE_FAILED;
					}
					if (escape == 0) {//���������ĂȂ�
						SHOW_RESULT(Result[i], option, SongNumberList[i], DifficultyList[i], debug, Music, Button, Button_Shutter, Key, Buf, AC, TryCount,
							STList,
							list_number,
							config,
							ir,
							i + 1);//���ʔ��\
					}

					if (Result[i].clear >= CLEARTYPE_FULL_COMBO && Music[SongNumberList[i]].secret == UnlockState::Secret) {//�B���Ȃ��t���R���{�ȏ�ŃN���A����
						//���֏����X�V
						int secretSongNnumber = 0;
						//�B���Ȕԍ��̊m�F
						for (j = 0; j < secret->total; j++) {
							if (secret->song_number[j] == SongNumberList[i]) {
								secretSongNnumber = j;
							}
						}
						secret->get_song_number[secretSongNnumber] = 1;//���֍ς݂ɂ���
						Music[SongNumberList[i]].secret = UnlockState::Unlocked;
						//�S���ւ������m�F
						secret_all_get(secret);//�S���ւ���Ă���Ȃ�all_get��1
					}

					playedSongIndex = i;
					if (Result[i].clear == CLEARTYPE_FAILED) {//���̋ȂŔ����o�������i�ʏI��
						STRes.clear = 0;
						break;
					}
				}

				if (escape == 0) {//���������ĂȂ�
					//�i�ʔF�胊�U���g�̕ۑ�
					STRes.save_data_version = RESULT_DATA_VERSION;
					for (i = 0; i <= 3; i++) {
						STRes.sky_perfect += Result[i].sky_perfect;
						STRes.perfect += Result[i].perfect;
						STRes.good += Result[i].good;
						STRes.miss += Result[i].miss;
						STRes.score += Result[i].score;
						STRes.rank += Result[i].rank;
					}

					STRes.max_combo = CourseMaxCombo;

					if (STRes.score >= 38000) {//S
						STRes.rank = 7;
					}
					else if (STRes.score >= 36000) {//A
						STRes.rank = 6;
					}
					else if (STRes.score >= 32000) {//B
						STRes.rank = 5;
					}
					else if (STRes.score >= 32000) {//C
						STRes.rank = 4;
					}
					else if (STRes.score >= 28000) {//D
						STRes.rank = 3;
					}
					else if (STRes.score >= 24000) {//E
						STRes.rank = 2;
					}
					else {//F
						STRes.rank = 1;
					}

					if (STRes.rank <= 0)STRes.rank = 1;//F�ȉ��ɂ͂��Ȃ�




					//�i�ʂ̃��U���g�\��


					SHOW_RESULT(STRes, option, SongNumberList[playedSongIndex], DifficultyList[playedSongIndex], debug, Music, Button, Button_Shutter, Key, Buf, AC, 0,
						STList,
						list_number,
						config,
						ir,
						SHOW_SKILL_TEST_RESULT);//���ʔ��\
				}
				option->op.gauge.setIndex((int)(OptionItem::Gauge)gauge_buf);//�Q�[�W�����ɖ߂�
			}
		}

		*l_n = list_number_base;//�O�I�񂾃t�H���_���̃��X�g��I��ł����Ԃɂ���
		*s_n = song_number;//�O�I�񂾋Ȃ�I��ł����Ԃɂ���
		*diff = difficulty;//�O�I�񂾓�Փx��I��ł����Ԃɂ���
		//*opt = Option->op;//�O�I�񂾃I�v�V������I�����Ă����Ԃɂ���

		//�v���C�I�v�V�����̕ۑ�
		OP op_buf = option->op;//�I�v�V�����ۑ��p
		if (op_buf.gauge.getIndex() == (int)OptionItem::Gauge::SKILL_TEST) {//�i�ʃQ�[�W�̎���NORMAL�Q�[�W�ŕۑ�
			op_buf.gauge.setIndex((int)OptionItem::Gauge::NORMAL);
		}
		SaveOptionState(op_buf);

		free(Highscore);
		free(HighscoreRival);
		exitScreen();
		return;//�֐��I��
	}

	//printfDx(L"LOOP3:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

	//�\�����镡���̃W�������A�A�[�e�B�X�g�A�Ȗ����̌v�Z
	if (*SelectingTarget == SELECTING_SONG) {
		if (widthCalcFlag == 1) {//�㉺���E�̃L�[�������ċ�,��Փx���ς���Ă�����
			for (i = 0; i <= Column; i++) {//Column/2�����S
				//title_width[i] = GetDrawStringWidth(Music[number_ring(list_number + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1)].title[difficulty],wcslen(Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].title[difficulty]));
				int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

				int list_number_buf = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base_buf].index;

				int sn_buf = folder->folder[folder->selected_folder]
					[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number;//list_number�̏�񂩂�Ȕԍ����Z�o


				int difficulty_buf = difficulty;

				if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && (i != (Column / 2))) {//��Փx�ʃt�H���_�̎��͑I��łȂ��Ȃ͓�Փx�Œ�ŕ\�����邽�߂Ƀ��[�v����difficulty�̒l���t�H���_�ł̎w��l�ɕύX
					difficulty_buf = folder->folder[folder->selected_folder]
						[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].difficulty;//list_number�̏�񂩂�Ȕԍ����Z�o
				}

				season_banner_buf[i] = Music[sn_buf].season[difficulty_buf];//�G�߂̃o�i�[����̂��߂̒l���o�b�t�@�Ɋi�[

				if (Music[sn_buf].secret != UnlockState::Secret) {//�B���Ȃł͂Ȃ��A���ꂩ�B���Ȃł����֍ς݂̋Ȃ̂Ƃ��͒ʏ�\��
					//�^�C�g��
					title_buf[i] =
						Music[sn_buf].title[difficulty_buf];//�Ȗ��̐擪�|�C���^��z��Ɋi�[
					title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//�F��z��Ɋi�[

					title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//�F��z��Ɋi�[
				}
				else {//�B����(secret=1)
					if (secret->song_appear_number == sn_buf) {//�B���ȏo���ΏۂɂȂ��Ă���
						//�^�C�g��
						title_buf[i] =
							Music[sn_buf].title[difficulty_buf];//�Ȗ��̐擪�|�C���^��z��Ɋi�[
						title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//�F��z��Ɋi�[
						title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//�F��z��Ɋi�[
					}
					else {//�B���ȏo���ΏۂɂȂ��Ă��Ȃ�
						if (Music[sn_buf].exist[difficulty_buf] == 1) {//���ʂ����݂���Ȃ�

							title_buf[i] =
								secret_str;//�Ȗ��̐擪�|�C���^��z��Ɋi�[
							title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//�F��z��Ɋi�[
							title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//�F��z��Ɋi�[
						}
						else {
							//�^�C�g��
							title_buf[i] =
								Music[sn_buf].title[difficulty_buf];//�Ȗ��̐擪�|�C���^��z��Ɋi�[
							title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//�F��z��Ɋi�[
							title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//�F��z��Ɋi�[
						}
					}


				}
				int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
				if (titleStrUpdateFlag == TOP) {
					if (index != cycleIndex(0 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}
				if (titleStrUpdateFlag == BOTTOM) {
					if (index != cycleIndex(config.SongSelectRowNumber - 1 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}
				title_width[index] =
					GetDrawStringWidthToHandle(//���������擾
						title_buf[i], //�^�C�g�����o��
						wcslen(title_buf[i]),//�����悤�ɂ��̕������������Ƃ��ē���Ă��
						FontHandle);

				SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
				SetDrawScreen(H_TITLE_STR[index]);//�Ȗ��摜��`��Ώۂ�
				ClearDrawScreen();//�O�ɕ`����Ă�������������
				//�摜�Ƃ��Ĉꎞ�I�ɕ`��
				ShowExtendedStrFitToHandle2(320, 4,
					title_buf[i],
					title_width[index],
					620, config,
					FontHandle,
					title_color[i],
					title_shadow_color[i]);//�Ȗ��`��

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				GraphBlend(H_TITLE_STR[index], screenHandle, 255, DX_GRAPH_BLEND_DODGE);//�����Ă�

				SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɖ߂�
				SetDrawMode(DX_DRAWMODE_NEAREST);



			}

			//�W�������A�A�[�e�B�X�g��width�̌v�Z
			if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�???�ɂ���
				genre_width = GetDrawStringWidthToHandle(secret_str, wcslen(secret_str), FontHandle);
				artist_width = GetDrawStringWidthToHandle(secret_str, wcslen(secret_str), FontHandle);
			}
			else {
				genre_width = GetDrawStringWidthToHandle(Music[song_number].genre[difficulty], wcslen(Music[song_number].genre[difficulty]), FontHandle);
				artist_width = GetDrawStringWidthToHandle(Music[song_number].artist[difficulty], wcslen(Music[song_number].artist[difficulty]), FontHandle);
			}
			widthCalcFlag = 0;



		}
	}

	//�i�ʔF��R�[�X�����̌v�Z
	if (*SelectingTarget == SELECTING_COURSE) {
		if (widthCalcFlag == 1) {//�㉺���E�̃L�[�������ċ�,��Փx���ς���Ă�����
			for (i = 0; i <= Column; i++) {//Column/2�����S
				//title_width[i] = GetDrawStringWidth(Music[number_ring(list_number + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1)].title[difficulty],wcslen(Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].title[difficulty]));
				int sn_buf = number_ring(list_number + (i - Column / 2), NUMBER_OF_COURSES - 1);//list_number�̏�񂩂�Ȕԍ����Z�o

				title_buf[i] = STList->title[sn_buf];
				title_color[i] = STList->Color[sn_buf];//�F��z��Ɋi�[
				title_shadow_color[i] = STList->ShadowColor[sn_buf];//�F��z��Ɋi�[

				int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
				if (titleStrUpdateFlag == TOP) {
					if (index != cycleIndex(0 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}
				if (titleStrUpdateFlag == BOTTOM) {
					if (index != cycleIndex(config.SongSelectRowNumber - 1 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}

				title_width[index] =
					GetDrawStringWidthToHandle(//���������擾
						title_buf[i], //�^�C�g�����o��
						wcslen(title_buf[i]),//�����悤�ɂ��̕������������Ƃ��ē���Ă��
						FontHandle);

				SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
				SetDrawScreen(H_TITLE_STR[index]);//�i�ʔF��R�[�X���摜��`��Ώۂ�
				ClearDrawScreen();//�O�ɕ`����Ă�������������
				//�摜�Ƃ��Ĉꎞ�I�ɕ`��
				ShowExtendedStrFitToHandle2(320, 4,
					title_buf[i],
					title_width[index],
					620, config,
					FontHandle,
					title_color[i],
					title_shadow_color[i]);//�Ȗ��`��

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɖ߂�
				SetDrawMode(DX_DRAWMODE_NEAREST);
				GraphBlend(H_TITLE_STR[index], screenHandle, 255, DX_GRAPH_BLEND_DODGE);//�����Ă�
			}

			widthCalcFlag = 0;
		}
	}

	//�t�H���_�����̌v�Z
	if (*SelectingTarget == SELECTING_FOLDER) {
		if (widthCalcFlag == 1) {//�㉺���E�̃L�[�������ċ�,��Փx���ς���Ă�����
			for (i = 0; i <= Column; i++) {//Column/2�����S
				int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
				if (titleStrUpdateFlag == TOP) {
					if (index != cycleIndex(0 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}
				if (titleStrUpdateFlag == BOTTOM) {
					if (index != cycleIndex(config.SongSelectRowNumber - 1 + titleCycleIndex.index, config.SongSelectRowNumber))continue;
				}

				folder_name_width[index] = GetDrawStringWidthToHandle(folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)],
					wcslen(folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)]), FontHandle);
				//printfDx(L"i:%d num:%d ring:%d\n",i, song_number + (i - 8), number_ring(song_number + (i - 8), NumberOfSongs-1));


				SetDrawScreen(H_TITLE_STR[index]);//�t�H���_���摜��`��Ώۂ�
				ClearDrawScreen();//�O�ɕ`����Ă�������������
				//�摜�Ƃ��Ĉꎞ�I�ɕ`��

				ShowExtendedStrFitToHandle2(320, 4,
					folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)],
					folder_name_width[index],
					620, config,
					FontHandle, colorRatio(255, 255, 255), colorRatio(0, 0, 0));//�t�H���_���\��

				SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɖ߂�
				GraphBlend(H_TITLE_STR[index], screenHandle, 255, DX_GRAPH_BLEND_DODGE);//�����Ă�

			}
			widthCalcFlag = 0;
		}
	}

	show_bpm = (Music[song_number].bpmmax[difficulty] + Music[song_number].bpmmin[difficulty]) / 2 + (sin(0.002 * GetNowCount_d(config))) * ((double)Music[song_number].bpmmax[difficulty] - Music[song_number].bpmmin[difficulty]);//���ۂɕ\������BPM������
	if (show_bpm >= Music[song_number].bpmmax[difficulty]) {
		show_bpm = Music[song_number].bpmmax[difficulty];
	}
	if (show_bpm <= Music[song_number].bpmmin[difficulty]) {
		show_bpm = Music[song_number].bpmmin[difficulty];
	}

	if (*SelectingTarget == SELECTING_SONG) {
		if (ScoreShowMode == 0) {
			number_digit(int(Highscore[song_number].score[difficulty + select_rainbow] + 0.5), score_digit, 5);//�n�C�X�R�A�������ƂɊi�[
		}
		else if (ScoreShowMode == 1) {
			number_digit(int(HighscoreRival[song_number].score[difficulty + select_rainbow] + 0.5), score_digit, 5);//�n�C�X�R�A�������ƂɊi�[
		}
	}
	else if (*SelectingTarget == SELECTING_FOLDER) {//�n�C�X�R�A�������ƂɊi�[
		int R = option->op.color.isRainbow();
		number_digit(int(FolderScore[R][folder->selected_folder][difficulty].AverageScore + 0.5), score_digit, 5);
	}
	else if (*SelectingTarget == SELECTING_COURSE) {//�n�C�X�R�A�������ƂɊi�[
		number_digit(int(STscore[list_number] + 0.5), score_digit, 5);
	}

	//printfDx(L"%d\n",score[song_number][difficulty]);
	number_digit(int(show_bpm + 0.5), bpm_digit, 5);//BPM�������ƂɊi�[


	if (*SelectingTarget == SELECTING_COURSE) {//�i��
		number_digit(int(STList->pop[list_number]), level_digit, 3);//���x���������ƂɊi�[
		number_digit(int(STList->bpmmin[list_number]), bpm_min_digit, 5);//�ŏ�BPM�������ƂɊi�[
		number_digit(int(STList->bpmmax[list_number]), bpm_max_digit, 5);//�ő�BPM�������ƂɊi�[
	}
	else {
		number_digit(int(Music[song_number].level[difficulty]), level_digit, 3);//���x���������ƂɊi�[
		number_digit(int(Music[song_number].bpmmin[difficulty]), bpm_min_digit, 5);//�ŏ�BPM�������ƂɊi�[
		number_digit(int(Music[song_number].bpmmax[difficulty]), bpm_max_digit, 5);//�ő�BPM�������ƂɊi�[
	}


	//printfDx(L"LOOP4:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

	
}




void SongSelect::SongSelectScreen::updateView()
{
	//DRAW
	SetDrawScreen(appContext.baseHandle.getHandle());
	ClearDrawScreen();//�O���t�B�b�N��������
	DrawGraph(0, 0, H_BG, TRUE);//�w�i�\��
	show_cloud(H_CLOUD, &pos_cloud, 1, LOOP_passed_time);//�B���ȉ��o���Ȃ甒���_�\��

	viewAlpha.process();
	double viewAlphaRatio = (double)viewAlpha.value / 255;


	if (CheckHandleASyncLoad(H_JACKET) == FALSE && CheckHandleASyncLoad(H_JACKET_GAUSS) == FALSE && jacket_show_counter == -1) {//���ʂƂڂ����p�ǂ�����ǂݍ��݊������Ă���ڂ�������
		jacket_show_counter = -2;
		GetGraphSize(H_JACKET, &jacketSize.x, &jacketSize.y);//�T�C�Y���擾
		GetGraphSize(H_JACKET_GAUSS, &jacketGaussSize.x, &jacketGaussSize.y);//�T�C�Y���擾
		//GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_DOWN_SCALE, 4);
		GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_GAUSS, 16, 4000);



		if (jacketArea.x - jacketSize.x >= jacketArea.y - jacketSize.y) {//�G���A�ɑ΂��ďc��
			jacketScale = (double)jacketArea.y / jacketSize.y;
			//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
		}
		else {//�G���A�ɑ΂��ĉ���
			jacketScale = (double)jacketArea.x / jacketSize.x;
			//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
		}

		if (jacketArea.x - jacketGaussSize.x >= jacketArea.y - jacketGaussSize.y) {//�ڂ������W���P�b�g�摜���G���A�ɑ΂��ďc��
			jacketGaussScale = (double)jacketArea.x / jacketGaussSize.x;
			//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
		}
		else {//�G���A�ɑ΂��ĉ���
			jacketGaussScale = (double)jacketArea.y / jacketGaussSize.y;
			//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
		}

	}

	//printfDx(L"%d %d\n", CheckHandleASyncLoad(H_JACKET), jacket_show_counter);
	SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A��
	if (CheckHandleASyncLoad(H_JACKET) == FALSE && jacket_show_counter == -2) {//�ڂ����������Ă���摜�\��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * jacket_alpha));

		//�ڂ������W���P�b�g�Ō��Ԃ𖄂߂�
		SetDrawArea(320, 96, 960, 624);//�`��\�G���A���w��
		if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�\�����Ȃ�
			//�\�����Ȃ�
		}
		else {
			//�ڂ������W���P�b�g
			DrawExtendGraph(int(center.x - jacketGaussScale * (jacketGaussSize.x / 2) - 1),
				int(center.y - jacketGaussScale * (jacketGaussSize.y / 2) - 1),
				int(center.x + jacketGaussScale * (jacketGaussSize.x / 2) + 1),
				int(center.y + jacketGaussScale * (jacketGaussSize.y / 2) + 1), H_JACKET_GAUSS, FALSE);
			//���̃W���P�b�g
			DrawExtendGraph(int(center.x - jacketScale * (jacketSize.x / 2)), int(center.y - jacketScale * (jacketSize.y / 2)), int(center.x + jacketScale * (jacketSize.x / 2)), int(center.y + jacketScale * (jacketSize.y / 2)), H_JACKET, FALSE);
		}
		SetDrawArea(0, 0, 1280, 720);//�`��\�G���A�����ɖ߂�
	}
	SetDrawMode(DX_DRAWMODE_NEAREST);//�o�C���j�A����߂�

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((flash) * 100));
	//jacket_show_counter == -1


	for (i = 0; i <= Column; i++) {//��ʐ^�񒆗̈�́u�t�H���_���A�Ȗ��v�̌��ɒu���o�i�[�̕\��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100 - int(100 * jacket_alpha));
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(155 * jacket_alpha));
		int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

		int list_number_buf = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base_buf].index;


		if (*SelectingTarget == SELECTING_SONG) {
			if (Music[folder->folder[folder->selected_folder][number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number].exist[difficulty]
				== 1//�����̕��ʂ����݂���Ƃ�
				//Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].exist[difficulty]
				) {
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK, TRUE);//�o�i�[(���݂��镈��)
			}
			else {
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK_NE, TRUE);//�o�i�[(���݂��Ȃ�����)
			}
		}

		if (*SelectingTarget == SELECTING_FOLDER) {
			DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK, TRUE);//�o�i�[
		}

		if (*SelectingTarget == SELECTING_SONG) {
			if (season_banner_buf[i] != 0) {
				//���G�߂���Ȃ��Ȃ�
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150 - int(150 * jacket_alpha));
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_SEASON[season_banner_buf[i] - 1], TRUE);
				//�o�i�[(�G�߂̖͗l)�ɏ����t����
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
				if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100 - int(100 * jacket_alpha));
			}
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100 - int(100 * jacket_alpha));
		DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_FLAME, TRUE);//�o�i�[�t���[��

	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	//printfDx(L"LOOP5:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
	DrawGraph(640 - 16 + int(((double)1 - button_draw_counter) * 288) + 32, 360 - 16, H_BUTTON_G, TRUE);//���^��G�{�^��
	DrawGraph(640 - 16 + int(((double)button_draw_counter - 1) * 288) - 32, 360 - 16, H_BUTTON_G, TRUE);//�E�^��G�{�^��
	if (Key[Button[1][2]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(640 - 16 + int(((double)1 - button_draw_counter) * 288) + 32, 360 - 16, H_BUTTON_PRESS, TRUE);//���^��G�{�^��
	}
	if (Key[Button[1][1]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(640 - 16 + int(((double)button_draw_counter - 1) * 288) - 32, 360 - 16, H_BUTTON_PRESS, TRUE);//�E�^��G�{�^��
	}

	//printfDx(L"LOOP5.1:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);


	//�L�[����w���\��
	for (i = 1; i <= 2; i++) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(0 + 90 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_B, TRUE);//B�{�^��
		if (Key[Button[0][i]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
			DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(0 + 90 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_PRESS, TRUE);//B�{�^��
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(688 - 90 - 7 + ((double)button_draw_counter - 1) * 32), H_BUTTON_R, TRUE);//R�{�^��
		if (Key[Button[2][i]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
			DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(688 - 90 - 7 + ((double)button_draw_counter - 1) * 32), H_BUTTON_PRESS, TRUE);//R�{�^��
		}
	}

	//SetFontSize(28);

	if (*SelectingTarget == SELECTING_SONG) {//�Ȗ��\��
		for (i = 0; i <= Column; i++) {
			int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

			int list_number_buf = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base_buf].index;

			int sn_buf = folder->folder[folder->selected_folder]
				[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number;//list_number�̏�񂩂�Ȕԍ����Z�o

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(255 * jacket_alpha));//�I�����Ă�Ȗ��ȊO��\���A�j���[�V����

			//����������߂鏈�����d��
			//title_buf[i];�Ȗ��̐擪�|�C���^���z��Ɋi�[����Ă���
			int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
			DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[index], TRUE);


			if ((secret->song_appear_number == sn_buf) && (Music[sn_buf].secret == UnlockState::Secret)) {//�B���Ȃ��o���ΏۂɂȂ��Ă���
				//Attack the Secret Song!�̕\��
				SetDrawBright(brightness, brightness, brightness);//�_��
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(255 * jacket_alpha));
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_FLAME_SECRET, TRUE);
				SetDrawBright(255, 255, 255);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
		}

		//�����ɑI�𒆂̋Ȗ���\��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255.0 * jacket_alpha));//�W���P�b�g�\���Ɠ����ɕ\��
		int index = cycleIndex(Column / 2 + titleCycleIndex.index, config.SongSelectRowNumber);
		DrawGraph(320, int(336 + (12 - Column / 2) * 48) - 48 + 48.0 * jacket_alpha, H_TITLE_STR[index], TRUE);

		int list_number_base_buf = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);
		int list_number_buf = SortList[(int)option->op.sort.getIndex()][folder->selected_folder][option->op.color.isRainbow()][difficulty - 1][list_number_base_buf].index;
		int sn_buf = folder->folder[folder->selected_folder]
			[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number;//list_number�̏�񂩂�Ȕԍ����Z�o

		if ((secret->song_appear_number == sn_buf) && (Music[sn_buf].secret == UnlockState::Secret)) {//�B���Ȃ��o���ΏۂɂȂ��Ă���
			//Attack the Secret Song!�̕\��
			SetDrawBright(brightness, brightness, brightness);//�_��
			DrawGraph(320, int(336 + (12 - Column / 2) * 48) - 48 + 48.0 * jacket_alpha, H_BANNER_FLAME_SECRET, TRUE);
			SetDrawBright(255, 255, 255);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	if (*SelectingTarget == SELECTING_COURSE) {//�i�ʔF��R�[�X���\��
		for (i = 0; i <= Column; i++) {
			int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
			DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[index], TRUE);
		}
	}


	if (*SelectingTarget == SELECTING_FOLDER) {//�t�H���_���\��
		for (i = 0; i <= Column; i++) {
			int index = cycleIndex(i + titleCycleIndex.index, config.SongSelectRowNumber);
			DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[index], TRUE);
		}
		/*
		for (i = 0; i <= Column; i++) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(168 * jacket_alpha));

			ShowExtendedStrFitToHandle(640, int(7 + 336 + (bn_draw_counter + i - Column / 2) * 48), folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)], folder_name_width[i], 620, FontHandle);//�t�H���_���\��
		}
		*/
	}

	//printfDx(L"LOOP5.3:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	DrawGraph(320, 0, H_BANNER_UD, TRUE);//�㉺�o�i�[

	SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
	//�W�������A�A�[�e�B�X�g�\��
	if (*SelectingTarget == SELECTING_SONG) {
		if (Music[song_number].genre[difficulty][0] != 0) {//�W������������
			if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�???�ɂ���
				ShowExtendedStrFitToHandle(640, int(8 + 336 + (0 + 3 - 20 / 2) * 48), secret_str, genre_width, 500, config, FontHandle);//�W�������\���u???�v
			}
			else {
				ShowExtendedStrFitToHandle(640, int(8 + 336 + (0 + 3 - 20 / 2) * 48), Music[song_number].genre[difficulty], genre_width, 500, config, FontHandle);//�W�������\��
			}
			DrawGraph(320, 0, H_BANNER_AREA, TRUE);//AREA�����\��
		}
		if (Music[song_number].artist[difficulty][0] != 0) {//�A�[�e�B�X�g������
			if (Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) {//�B���ȂȂ�???�ɂ���
				ShowExtendedStrFitToHandle(640, int(1 + 336 + (0 + 4 - 20 / 2) * 48), secret_str, artist_width, 480, config, FontHandle);//�A�[�e�B�X�g�\���u???�v
			}
			else {
				ShowExtendedStrFitToHandle(640, int(1 + 336 + (0 + 4 - 20 / 2) * 48), Music[song_number].artist[difficulty], artist_width, 480, config, FontHandle);//�A�[�e�B�X�g�\��
			}
			DrawGraph(320, 0, H_BANNER_ARTIST, TRUE);//ARTIST�����\��
		}


		/*
		DrawStringToHandle(int(640 - ((double)genre_width / 2)) + 2, int(11 + 336 + (0 + 3 - 20 / 2) * 48 + 2), Music[song_number].genre[difficulty], GetColor(0, 0, 0), FontHandle);//�W������(�e)
		DrawStringToHandle(int(640 - ((double)genre_width / 2)), int(11 + 336 + (0 + 3 - 20 / 2) * 48), Music[song_number].genre[difficulty], GetColor(255, 255, 255), FontHandle);//�W������

		DrawStringToHandle(int(640 - ((double)artist_width / 2)) + 2, int(6 + 336 + (0 + 4 - 20 / 2) * 48 + 2), Music[song_number].artist[difficulty], GetColor(0, 0, 0), FontHandle);//�A�[�e�B�X�g(�e)
		DrawStringToHandle(int(640 - ((double)artist_width / 2)), int(6 + 336 + (0 + 4 - 20 / 2) * 48), Music[song_number].artist[difficulty], GetColor(255, 255, 255), FontHandle);//�A�[�e�B�X�g
		*/
	}
	else if (*SelectingTarget == SELECTING_COURSE) {//�i�ʔF��p����
		ShowExtendedStrFitToHandle(640, int(8 + 336 + (0 + 3 - 20 / 2) * 48), Des1, Des1_width, 640, config, FontHandle);//�W�������\��
		ShowExtendedStrFitToHandle(640, int(1 + 336 + (0 + 4 - 20 / 2) * 48), Des2, Des2_width, 640, config, FontHandle);//�A�[�e�B�X�g�\��
	}
	SetDrawMode(DX_DRAWMODE_NEAREST);

	//printfDx(L"LOOP6:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

	//printfDx(L"%s\n", Music[song_number].artist[difficulty]);





	//�A�i�E���X�\��
	if (*SelectingTarget == SELECTING_FOLDER) {
		SetDrawArea(328, 8, 953, 88);//�`��\�G���A���w��
		if (secret->song_appear_number != -1) {//�B���ȏo�����o��
			show_str(GetNowCount(), Announse_show_time_base, H_ANNOUNSE, 960, 29, secret->Announce_width);

			//show_str(GetNowCount(), Announse_show_time_base, secret->Announce, 330 * 3, 32, secret->Announce_width, config, FontHandle, secret->Color, secret->ShadowColor);
		}
		else {//�ʏ��Ԃ͒ʏ�A�i�E���X
			show_str(GetNowCount(), Announse_show_time_base, H_ANNOUNSE, 960, 29, AnnounceWidth);

			//show_str(GetNowCount(), Announse_show_time_base, StrAnnounce,      330 * 3, 32, AnnounceWidth, config, FontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));
		}
		SetDrawArea(0, 0, 1280, 720);//�`��\�G���A�����ɖ߂�
	}

	//���ۂ̃n�C�X�s�����߂�
	if (Music[song_number].speed_list[difficulty] != 0) {//�u�ԕ�����0�ł͂Ȃ��Ƃ��Ƀn�C�X�s�����킹��
		option->op.speedVal = option->op.speed.getVal() / (double)Music[song_number].speed_list[difficulty][option->op.speedAdapter.getIndex()];
	}
	else {
		option->op.speedVal = 1;
	}

	oi_counter = int(0.0004 * GetNowCount_d(config)) % OPERATION_INSTRUCTION_NUMBER;//��������\���p�̃J�E���^
	if (OptionOpen == 1) {//�I�v�V���������\��
		if (option_select == (int)OptionItem::Name::SPEED ||
			option_select == (int)OptionItem::Name::SPEED_ADAPTER) {
			if (*SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {//�ȑI���̎��ŕ��ʂ����݂���Ƃ��͏�ɕ\�����n�C�X�s�����������������\��
				wchar_t SpeedStr[128] = L"(0�`0)";
				int SpeedStrWidth = 0;

				DrawGraph(320, 2 + 48 * 13, option->H_SENT, TRUE);
				//�����\��
				if (*SelectingTarget == SELECTING_SONG) {
					sprintfDx(SpeedStr, L"(MID:%d FAST:%d MAX:%d)",
						int(Music[song_number].speed_list[difficulty][Speed::MID] * option->op.speedVal + 0.5),
						int(Music[song_number].speed_list[difficulty][Speed::FAST] * option->op.speedVal + 0.5),
						int(Music[song_number].speed_list[difficulty][Speed::MAX] * option->op.speedVal + 0.5)
					);
				}
				else if (*SelectingTarget == SELECTING_COURSE) {
					sprintfDx(SpeedStr, L"(%d�`%d)",
						int(STList->bpmmin[list_number] * option->op.speedVal + 0.5),
						int(STList->bpmmax[list_number] * option->op.speedVal + 0.5));
				}
				SpeedStrWidth = GetDrawStringWidthToHandle(SpeedStr, wcslen(SpeedStr), FontHandle);
				ShowExtendedStrFitToHandle(640, 2 + 48 * 14, SpeedStr, SpeedStrWidth, 620, config, FontHandle);
			}
			else {//�t�H���_�Z���N�g�A�i�ʔF��̎��͐^�񒆂ɕ\��
				DrawGraph(320, 24 + 48 * 13, option->H_SENT, TRUE);
			}

			//show_str(GetNowCount_d(config), time_base_str, Option->sent_speed[Option->op.speed], 330 * 3, 655, width_sent_speed[Option->op.speed],FontHandle);
		}
		else {
			DrawGraph(320, 24 + 48 * 13, option->H_SENT, TRUE);

			//show_str(GetNowCount_d(config), time_base_str, Option->sent_gauge[Option->op.gauge], 330 * 3, 655, width_sent_gauge[Option->op.gauge],FontHandle);
		}


	}
	else {//��������\��
		if (oi_counter <= 4) {
			DrawGraph(329, 632, H_BANNER_D_BUTTON[oi_counter], TRUE);//�R���g���[���摜
		}
		else {
			DrawGraph(329, 632, H_BANNER_D_BUTTON[5], TRUE);//�R���g���[���摜
		}

		DrawGraph(320, 655, H_OI_STR[oi_counter], TRUE);

		//if (config.ShowStrShadow == TRUE)DrawStringToHandle(int(696 - ((double)width_ope_ins[oi_counter] / 2)) + 2, int(655 + 2), ope_ins[oi_counter], GetColor(0, 0, 0),FontHandle);//����(�e)
		//DrawStringToHandle(int(696 - ((double)width_ope_ins[oi_counter] / 2)), int(655), ope_ins[oi_counter], GetColor(255, 255, 255), FontHandle);//����
	}



	DrawGraph(320, 0, H_BANNER_UD_SKIN, TRUE);//�㉺�o�i�[(�g)

	SetDrawBright(brightness, brightness, brightness);//�I��g�_��
	if (OptionOpen == 0) {//�I�Ȃ̃��[�h�̂Ƃ�
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(255 * jacket_alpha));
		DrawGraph(320, 336, H_BANNER_SELECT, TRUE);//�I��g
	}
	SetDrawBright(255, 255, 255);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	float GraphColorV = 255;
	unsigned int GraphColorArray[9][3] = {//�e�F�̉������o�[�\���p�̐F�z��
		{255,0,0},
		{0,255,0},
		{0,0,255},
		{0,255,255},
		{255,0,255},
		{255,255,0},
		{255,255,255},
		{0,0,0},
		{0,0,0}
	};


	if (*SelectingTarget != SELECTING_COURSE) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, coverAlpha.value);
		DrawGraph(0, 0, H_COVER[difficulty], TRUE);//�J�o�[�\��
		DrawGraph(960, 0, H_COVER[difficulty], TRUE);//�E��

		//�e�F�̉������x�O���t�̕`��
		if (*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_COURSE) {
			int BarWidth = 33;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 110);
			for (i = 0; i < 8; i++) {
				DrawBox(11 + 960 + i * BarWidth, 720, 11 + 960 + BarWidth + i * BarWidth, int(720 - ShowColorNotesGraph[i]), GetColor(GraphColorArray[i][0], GraphColorArray[i][1], GraphColorArray[i][2]), TRUE);
				/*
				for (j = 0; j < 32; j++) {
					DrawLine(11 + 960 + j + i * 33, 720, 11 + 960 + j + i * 33, 720 - Music[song_number].ColorNotesAmount[difficulty][i], GetCylinderColor(j, 32, GraphColorArray[i][0], GraphColorArray[i][1], GraphColorArray[i][2]), TRUE);
				}
				*/
			}

			//���̃O���t
			for (j = 0; j < BarWidth; j++) {
				DrawLine(11 + 960 + j + 8 * BarWidth, 720, 11 + 960 + j + 8 * BarWidth, int(720 - ShowColorNotesGraph[i]), GetRainbowColor((double)BarWidth - 1 - j, BarWidth), 1);
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);

			//���x�܂���O���t�̕`��
			for (i = 0; i < 8; i++) {
				DrawLineAA(float(11 + 960 + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i]), float(11 + 960 + BarWidth + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i + 1]), GetColor(255, 255, 255), 3);
			}

			for (i = 0; i < 9; i++) {
				DrawCircleAA(float(11 + 960 + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i]), float(4), 32, GetColor(255, 255, 255), TRUE);
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			DrawGraph(960, 0, H_COVER_POP, TRUE);//�E���̕���
		}


	}
	else {//�i�ʔF��J�o�[
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, coverAlpha.value);
		DrawGraph(0, 0, H_COVER_SKILL_TEST, TRUE);//�J�o�[�\��
		DrawGraph(960, 0, H_COVER_SKILL_TEST, TRUE);//�E��
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawGraph(960, 0, H_COVER_SKILL_TEST_POP, TRUE);//�E���̕���
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * 255);
	DrawGraph(0, 0, H_COVER_RADAR, TRUE);

	if (*SelectingTarget == SELECTING_SONG) {
		DrawGraph(0, 0, H_COVER_RADAR_NAME_TOTAL, TRUE);
	}
	else if (*SelectingTarget == SELECTING_FOLDER) {
		DrawGraph(0, 0, H_COVER_RADAR_NAME_ROBUSTNESS, TRUE);
	}



	DrawGraph(0, 0, H_COVER_HIGH_SCORE, TRUE);//�����̕���

	//���x���`��
	if (*SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {
		DrawGraph(1180, 100, H_PERCENT, TRUE);
		for (i = 0; i <= int(log10(Music[song_number].level[difficulty])); i++) {
			DrawGraph(10 + 1120 - i * 40, 100, H_SCORE_NUMBER[level_digit[i]], TRUE);
		}
	}
	else if (*SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2) {//�i�ʓ�Փx
		DrawGraph(1145, 100, H_PERCENT, TRUE);
		for (i = 0; i <= int(log10(STList->pop[list_number])); i++) {
			DrawGraph(10 + 1060 - i * 40, 100, H_SCORE_NUMBER[level_digit[i]], TRUE);
		}
	}

	//��Փx�摜�`��
	//printfDx(L"%d\n", H_DIFFICULTY[difficulty]);
	if (*SelectingTarget == SELECTING_SONG) {
		if (Music[song_number].exist[difficulty] == 1) {//���݂���Ƃ�
			if (Music[song_number].season[difficulty] != 4) {//�~����Ȃ��Ƃ�
				DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty], TRUE);//���ʓ�Փx


				//DrawGraph(1020, 260, H_DIFFICULTY[difficulty], TRUE);
			}
			else {//�~�̂Ƃ�
				if (difficulty <= 2) {//����܂�͂��̂܂�
					DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty], TRUE);//���ʓ�Փx

				}
				if (difficulty >= 3 && difficulty <= 4) {//�� ����
					DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty + 2], TRUE);//���ʓ�Փx

				}
			}
		}
	}

	//�X�R�A�`��

	int R_ShowFlag = 0;//R�\���t���O
	if ((*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_FOLDER) && option->op.color.isRainbow()) {
		R_ShowFlag = 1;
	}
	else if (*SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] == 0) {
		R_ShowFlag = 1;
	}


	if (R_ShowFlag) {
		for (i = 0; i <= 4; i++) {
			DrawExtendGraph(245 - i * 40, int(100), 245 + 64 - i * 40, int(100 + 100), H_SCORE_NUMBER[score_digit[i]], TRUE);
		}
	}
	else {
		for (i = 0; i <= 4; i++) {
			DrawExtendGraph(210 - i * 40, int(100), 210 + 64 - i * 40, int(100 + 100), H_SCORE_NUMBER[score_digit[i]], TRUE);
		}
	}

	//R�\��
	if (R_ShowFlag) {//�ȑI��
		DrawGraph(0, 97, H_R, TRUE);
	}

	int boxAlpha = viewAlphaRatio * 80;
	int boxLineAlpha = viewAlphaRatio * 255;
	//BPM, VERSION�`��
	//�g�̕\��
	if (*SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {

		//BPM�p�\���g
		DrawBoxWithLine(976, 200, 1110, 240, GetColor(50, 50, 255), boxAlpha, boxLineAlpha);
		DrawBoxWithLine(1130, 200, 1264, 240, GetColor(255, 50, 50), boxAlpha, boxLineAlpha);

		//�o�[�W�����p�\���g
		DrawBoxWithLine(976, 250, 1110, 290, GetColor(50, 255, 50), boxAlpha, boxLineAlpha);

		//�ő哯���������p�\���g
		DrawBoxWithLine(1130, 250, 1264, 290, GetColor(255, 255, 255), boxAlpha, boxLineAlpha);

		DrawGraph(960, 200, H_BPM_MINMAX_STR, TRUE);
		DrawGraph(960, 250, H_VER_MAX_CHORDS_STR, TRUE);
	}
	else if (*SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2) {//�i��
		DrawBoxWithLine(976, 200, 1110, 240, GetColor(50, 50, 255), boxAlpha, boxLineAlpha);
		DrawBoxWithLine(1130, 200, 1264, 240, GetColor(255, 50, 50), boxAlpha, boxLineAlpha);

		DrawGraph(960, 200, H_BPM_MINMAX_STR, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * 255);
	//BPM, VERSION���l�`��
	if (*SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {

		DrawNumber(1062, 196, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_BPM_NUMBER_MIN);
		DrawNumber(1216, 196, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_BPM_NUMBER_MAX);
		DrawFloatNumber(1090, 246, Music[song_number].version[difficulty], 20, 3, 0.5, H_VERSION_NUMBER, H_VERSION_DECIMAL);

		DrawNumber(1235, 246, Music[song_number].maxChords[option->op.color.isRainbow()][difficulty], 25, 0, 0, H_MAX_CHORDS_NUMBER);
	}
	else if (*SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2) {//�i��
		DrawNumber(1062, 196, STList->bpmmin[list_number], 25, 0, 0, H_BPM_NUMBER_MIN);
		DrawNumber(1216, 196, STList->bpmmax[list_number], 25, 0, 0, H_BPM_NUMBER_MAX);
	}


	SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��

	if (*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_FOLDER || *SelectingTarget == SELECTING_COURSE) {
		int RankBuf = 0;
		int ClearStateBuf = 0;
		int PlayCountBuf = 0;
		int ExistBuf = Music[song_number].exist[difficulty];

		if (*SelectingTarget == SELECTING_SONG) {
			if (ScoreShowMode == 0) {
				RankBuf = Highscore[song_number].rank[difficulty + select_rainbow];
				ClearStateBuf = Highscore[song_number].clear_state[difficulty + select_rainbow];
				PlayCountBuf = Highscore[song_number].play_count[difficulty + select_rainbow];
			}
			else if (ScoreShowMode == 1) {
				RankBuf = HighscoreRival[song_number].rank[difficulty + select_rainbow];
				ClearStateBuf = HighscoreRival[song_number].clear_state[difficulty + select_rainbow];
				PlayCountBuf = HighscoreRival[song_number].play_count[difficulty + select_rainbow];
			}
		}
		else if (*SelectingTarget == SELECTING_COURSE) {
			RankBuf = STrank[list_number];
			ClearStateBuf = STclear_state[list_number];
			PlayCountBuf = STplay_count[list_number];
			ExistBuf = 1;
		}
		else if (*SelectingTarget == SELECTING_FOLDER) {
			RankBuf = FolderScore[option->op.color.isRainbow()][folder->selected_folder][difficulty].folderRank;
			ClearStateBuf = FolderScore[option->op.color.isRainbow()][folder->selected_folder][difficulty].ClearType;
			PlayCountBuf = 1;
			ExistBuf = 1;
		}

		//�����N�`��
		if (RankBuf != 0) {
			int RankSize = 210;
			DrawExtendGraph(7 + 50, 460 + 50, 7 + RankSize + 50, 460 + RankSize + 50, H_RANK[RankBuf], TRUE);

			//DrawExtendGraph(0 + 50, 200 + 50, 220 + 50, 420 + 50, H_RANK[RankBuf], TRUE);
		}
		int PlayStateY = 650;
		float PlayStateRatioG = 0.75;
		float PlayStateRatio = (float)0.8 * PlayStateRatioG;
		float PlayStateRatioF = (float)0.66 * PlayStateRatioG;
		int PlayStatex = -30;

		//�v���C��ԕ`��
		if (RankBuf != 0 && ClearStateBuf == CLEARTYPE_FAILED && ExistBuf == 1) {//FAILED,�s���i���
			if (*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_FOLDER) {
				DrawExtendGraph(PlayStatex - 5, PlayStateY, int(PlayStatex - 5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_FAILED, TRUE);
			}
			else if (*SelectingTarget == SELECTING_COURSE) {
				DrawExtendGraph(PlayStatex + 5, PlayStateY, int(PlayStatex + 5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_SKILL_TEST_FAILED, TRUE);
			}
		}
		if (ClearStateBuf == CLEARTYPE_PLAY && ExistBuf == 1) {//PLAY���(�����\�����Ȃ�)
			//DrawExtendGraph(-95, 580, int(-95 + ((double)640 * 0.8)), int(580 + ((double)100 * 0.8)), H_CLEARED_EASY, TRUE);
		}
		if (ClearStateBuf == CLEARTYPE_EASY_CLEARED && ExistBuf == 1) {//EASY_CLEARED,���i���
			if (*SelectingTarget == SELECTING_SONG) {
				DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_EASY, TRUE);
			}
			else if (*SelectingTarget == SELECTING_COURSE) {
				DrawExtendGraph(PlayStatex + 5, PlayStateY, int(PlayStatex + 5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_SKILL_TEST_PASSED, TRUE);
			}
		}
		if (ClearStateBuf == CLEARTYPE_CLEARED && ExistBuf == 1) {//NORMAL_CLEARED���
			DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_NORMAL, TRUE);
		}
		if (ClearStateBuf == CLEARTYPE_HARD_CLEARED && ExistBuf == 1) {//HARD_CLEARED���
			DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_HARD, TRUE);
		}
		if (ClearStateBuf == CLEARTYPE_SUPER_HARD_CLEARED && ExistBuf == 1) {//SUPER_HARD_CLEARED���
			DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_SUPER_HARD, TRUE);
		}
		if (ClearStateBuf == CLEARTYPE_FULL_COMBO && ExistBuf == 1) {//FULL_COMBO���
			DrawExtendGraph(PlayStatex + 30, PlayStateY + 10, int(PlayStatex + 30 + ((double)640 * PlayStateRatioF)), int(PlayStateY + 10 + ((double)100 * PlayStateRatioF)), H_FULL_COMBO[(int(GAME_passed_time) / 70) % 6], TRUE);
		}

		if (ClearStateBuf == CLEARTYPE_PERFECT && ExistBuf == 1) {//PERFECT_FULLCOMBO���
			DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_PFC, TRUE);
		}

		if (ClearStateBuf == CLEARTYPE_NO_PLAY && ExistBuf == 1) {//no play���
			DrawExtendGraph(PlayStatex - 5, PlayStateY, int(PlayStatex - 5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_NO_PLAY, TRUE);
		}

	}
	SetDrawMode(DX_DRAWMODE_NEAREST);//�o�C���j�A����߂�



	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	if (activityState == FLAG_OPENING_STATE) {//�J����
		DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
		if (c_m_draw_counter > 1) {
			if (*SelectingTarget == SELECTING_SONG) {
				//�J������ȍĐ�
				SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
				if ((Music[song_number].secret == UnlockState::Secret && secret->song_appear_number != song_number) || BGM_continue == 1) {//�B����,��pBGM�Đ����Ȃ�炳�Ȃ�
					//�炳�Ȃ�
				}
				else {
					PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
					song_play_counter = -2;
				}
			}
			c_m_draw_counter = 1;
			activityState = FLAG_SELECT_STATE;
		}
		for (i = 0; i < CRTBuf; i++) {
			if (c_m_draw_counter <= 1) {
				c_m_draw_counter += 0.001;
			}
		}
		if (openflag == 0) {
			PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
			openflag = 1;
		}
	}



	if (activityState == FLAG_CLOSING_STATE) {//�߂�
		DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
		if (c_m_draw_counter < 0) {
			PlaySoundMem(SH_CLOSED, DX_PLAYTYPE_BACK, TRUE);
			activityState = FLAG_END_FUNCTION_STATE;
			CoverClosedTime = (int)GAME_passed_time;
		}
		for (i = 0; i < CRTBuf; i++) {
			if (c_m_draw_counter >= 0) {
				c_m_draw_counter -= 0.001;
			}
		}

	}

	if (activityState == FLAG_END_FUNCTION_STATE) {//�܂�����
		DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * 255);
	//���ʃ��[�_�[�̐��l�\��
	if (*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_FOLDER) {
		int RaderNumberInterval = 15;
		DrawNumber(160, 200, int(DRShowGlobal + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(288, 257, int(DRShowLocal + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(288, 445, int(DRShowChain + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(160, 502, int(DRShowUnstability + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(34, 445, int(DRShowStreak + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(34, 257, int(DRShowColor + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		DrawNumber(218, 348, int(DRShowTotal + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);

	}


	//���ʃ��[�_�[�̉�]������\��
	DrawRadarLine(GAME_passed_time / 1000);

	//���ʃ��[�_�[�`��
	if (*SelectingTarget == SELECTING_SONG || *SelectingTarget == SELECTING_FOLDER) {
		//if (Music[song_number].exist[difficulty] == 1) {//���݂���Ƃ�
		DrawHexagon(
			(short)(DRShowGlobal + 0.5),
			(short)(DRShowLocal + 0.5),
			(short)(DRShowChain + 0.5),
			(short)(DRShowUnstability + 0.5),
			(short)(DRShowStreak + 0.5),
			(short)(DRShowColor + 0.5));
		//}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	//NOTE��I�����Ă���Ƃ��̓v���r���[�摜��\��
	if (OptionOpen == 1 && option_select == (int)OptionItem::Name::NOTE) {
		SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
		DrawExtendGraph(332, 632, 412, 712, H_OPTION_NOTE_PREVIEW[0], TRUE);
		DrawExtendGraph(868, 632, 948, 712, H_OPTION_NOTE_PREVIEW[1], TRUE);
		SetDrawMode(DX_DRAWMODE_NEAREST);//�o�C���j�A����߂�
	}

	//NOTE SYMBOL��I�����Ă���Ƃ��̓v���r���[�摜��\��
	if (OptionOpen == 1 && option_select == (int)OptionItem::Name::NOTE_SYMBOL) {
		SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
		DrawExtendGraph(332, 632, 412, 712, H_OPTION_NOTE_SYMBOL_PREVIEW[0], TRUE);
		DrawExtendGraph(868, 632, 948, 712, H_OPTION_NOTE_SYMBOL_PREVIEW[1], TRUE);
		SetDrawMode(DX_DRAWMODE_NEAREST);//�o�C���j�A����߂�
	}



	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	optionListView.draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	cache = int(cos((3.14159265) / 2 * ((double)1 - result_draw_counter)) * (-320) + 1280 + 0.5);//���U���g�J�o�[

	DrawGraph(cache, 0, H_COVER_OPTION, TRUE);
	if ((*SelectingTarget == SELECTING_SONG) || (*SelectingTarget == SELECTING_COURSE)) {
		DrawGraph(cache, 0, H_RESULT, TRUE);//���U���g�^�C�g��
		DrawGraph(cache, 0, H_RESULT_OBJ, TRUE);//���U���g�{�[�h
	}

	if ((Music[song_number].exist[difficulty] == 1 && *SelectingTarget == SELECTING_SONG) || *SelectingTarget == SELECTING_COURSE) {//�ȑI����Ԃŕ��ʂ����݂���Ƃ����A�i�ʑI���̂Ƃ�
		int SkyPerfectBuf = 0;
		int PerfectBuf = 0;
		int GoodBuf = 0;
		int MissBuf = 0;
		int MinMissBuf = 0;
		int MaxComboBuf = 0;
		int PlayCountBuf = 0;

		if (*SelectingTarget == SELECTING_SONG) {
			if (ScoreShowMode == 0) {
				SkyPerfectBuf = Highscore[song_number].sky_perfect[difficulty + select_rainbow];
				PerfectBuf = Highscore[song_number].perfect[difficulty + select_rainbow];
				GoodBuf = Highscore[song_number].good[difficulty + select_rainbow];
				MissBuf = Highscore[song_number].miss[difficulty + select_rainbow];
				MinMissBuf = Highscore[song_number].min_miss[difficulty + select_rainbow];
				MaxComboBuf = Highscore[song_number].max_combo[difficulty + select_rainbow];
				PlayCountBuf = Highscore[song_number].play_count[difficulty + select_rainbow];
			}
			else if (ScoreShowMode == 1) {
				SkyPerfectBuf = HighscoreRival[song_number].sky_perfect[difficulty + select_rainbow];
				PerfectBuf = HighscoreRival[song_number].perfect[difficulty + select_rainbow];
				GoodBuf = HighscoreRival[song_number].good[difficulty + select_rainbow];
				MissBuf = HighscoreRival[song_number].miss[difficulty + select_rainbow];
				MinMissBuf = HighscoreRival[song_number].min_miss[difficulty + select_rainbow];
				MaxComboBuf = HighscoreRival[song_number].max_combo[difficulty + select_rainbow];
				PlayCountBuf = HighscoreRival[song_number].play_count[difficulty + select_rainbow];
			}
		}
		else if (*SelectingTarget == SELECTING_COURSE) {
			SkyPerfectBuf = STsky_perfect[list_number];
			PerfectBuf = STperfect[list_number];
			GoodBuf = STgood[list_number];
			MissBuf = STmiss[list_number];
			MinMissBuf = STmin_miss[list_number];
			MaxComboBuf = STmax_combo[list_number];
			PlayCountBuf = STplay_count[list_number];
		}

		//���l�`��
		DrawNumber((cache - 960) + 1212, 210, SkyPerfectBuf, 25, 1, 0, H_JUDGE_NUMBER);
		DrawNumber((cache - 960) + 1212, 262, PerfectBuf, 25, 1, 0, H_JUDGE_NUMBER);
		DrawNumber((cache - 960) + 1212, 314, GoodBuf, 25, 1, 0, H_JUDGE_NUMBER);
		DrawNumber((cache - 960) + 1212, 367, MissBuf, 25, 1, 0, H_JUDGE_NUMBER);

		if (MinMissBuf != -1) {//MinMiss���ۑ�����Ă���Ƃ�
			DrawNumber((cache - 960) + 1221, 499, MinMissBuf, 25, 1, 4, H_MIN_MISS_NUMBER);
		}

		DrawNumber((cache - 960) + 1221, 557, MaxComboBuf, 25, 1, 4, H_MAX_COMBO_NUMBER);
		DrawNumber((cache - 960) + 1221, 615, PlayCountBuf, 25, 1, 4, H_PLAY_COUNT_NUMBER);
	}
	else if (*SelectingTarget == SELECTING_FOLDER) {
		int boxHeight = 40;
		//�g�̕\��

		//�t�H���_�L�^
		//�N���A���
		DrawBoxWithLine((cache - 960) + 976, 130, (cache - 960) + 1110, 130 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 170, (cache - 960) + 1110, 170 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 210, (cache - 960) + 1110, 210 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 250, (cache - 960) + 1110, 250 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 290, (cache - 960) + 1110, 290 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 330, (cache - 960) + 1110, 330 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 370, (cache - 960) + 1110, 370 + boxHeight, GetColor(50, 50, 50));

		//�����N
		DrawBoxWithLine((cache - 960) + 1130, 130, (cache - 960) + 1264, 130 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 170, (cache - 960) + 1264, 170 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 210, (cache - 960) + 1264, 210 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 250, (cache - 960) + 1264, 250 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 290, (cache - 960) + 1264, 290 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 330, (cache - 960) + 1264, 330 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 1130, 370, (cache - 960) + 1264, 370 + boxHeight, GetColor(50, 50, 50));


		DrawBoxWithLine((cache - 960) + 976, 410, (cache - 960) + 1110, 410 + boxHeight, GetColor(50, 50, 50));;//PLAY
		DrawBoxWithLine((cache - 960) + 976, 450, (cache - 960) + 1264, 450 + boxHeight, GetColor(50, 50, 50));//NO PLAY

		//�g�[�^���L�^
		DrawBoxWithLine((cache - 960) + 976, 540, (cache - 960) + 1264, 540 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 580, (cache - 960) + 1264, 580 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 620, (cache - 960) + 1264, 620 + boxHeight, GetColor(50, 50, 50));
		DrawBoxWithLine((cache - 960) + 976, 660, (cache - 960) + 1264, 660 + boxHeight, GetColor(50, 50, 50));

		//����
		DrawGraph((cache), 0, H_COVER_STATUS_STR, TRUE);
		DrawGraph((cache), 0, H_COVER_STATUS, TRUE);

		//DrawGraph((cache - 960) + 960, 250, H_VERSION_STR, TRUE);


		//���l�`��

		//�t�H���_�L�^
		//�N���A���
		int R = 0;
		if (option->op.color.isRainbow())R = 1;

		FolderScore[R][folder->selected_folder][difficulty].clearState[CLEARTYPE_PERFECT];

		DrawNumber((cache - 960) + 1092, 130 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_PERFECT)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 170 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_FULL_COMBO)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 210 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_SUPER_HARD_CLEARED)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 250 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_HARD_CLEARED)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 290 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_CLEARED)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 330 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_EASY_CLEARED)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 370 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_FAILED)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1092, 410 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_PLAY)], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 450 - 4, FolderScore[R][folder->selected_folder][difficulty].clearState[clearStateConverter(CLEARTYPE_NO_PLAY)], 25, 1, 0, H_PLAY_COUNT_NUMBER);


		//�����N
		DrawNumber((cache - 960) + 1246, 130 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_S], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 170 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_A], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 210 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_B], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 250 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_C], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 290 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_D], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 330 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_E], 25, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1246, 370 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_F], 25, 1, 0, H_PLAY_COUNT_NUMBER);



		//�g�[�^���L�^
		DrawNumber((cache - 960) + 1248, 536, saveData.totalBootCount, 22, 1, 0, H_PLAY_COUNT_NUMBER);
		DrawNumber((cache - 960) + 1248, 576, saveData.totalPlayCount, 22, 1, 0, H_PLAY_COUNT_NUMBER);
		if (option->op.color.isRainbow()) {
			DrawNumber((cache - 960) + 1248, 616, saveData.totalHighScoreRainbow, 22, 1, 0, H_PLAY_COUNT_NUMBER);
		}
		else {
			DrawNumber((cache - 960) + 1248, 616, saveData.totalHighScore, 22, 1, 0, H_PLAY_COUNT_NUMBER);

		}
		DrawNumber((cache - 960) + 1248, 656, saveData.totalHitNotes, 22, 1, 0, H_PLAY_COUNT_NUMBER);


	}



	//�L�[����w���\��
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
	DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 7 + 1, H_BUTTON_B, TRUE);//��B�{�^��
	if (Key[Button[0][0]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 7 + 1, H_BUTTON_PRESS, TRUE);//��B�{�^��
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
	DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 7, H_BUTTON_B, TRUE);//�EB�{�^��
	if (Key[Button[0][3]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 7, H_BUTTON_PRESS, TRUE);//�EB�{�^��
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
	DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 681, H_BUTTON_R, TRUE);//��R�{�^��
	if (Key[Button[2][0]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 681, H_BUTTON_PRESS, TRUE);//��R�{�^��
	}


	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
	DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 681, H_BUTTON_R, TRUE);//�ER�{�^��
	if (Key[Button[2][3]] >= 1) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 681, H_BUTTON_PRESS, TRUE);//�ER�{�^��
	}



	//DrawGraph(int(960 + ((double)i - 1.5) * 128) - 16, int(0 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_B, TRUE);//�EB�{�^��

	if (OptionOpen == 0) {
		if (difficulty >= 2) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
			DrawGraph(320 - 16 + int(((double)1 - button_draw_counter) * 32) - 32, 360 - 16, H_BUTTON_G, TRUE);//��G�{�^��
			if (Key[Button[1][0]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
				DrawGraph(320 - 16 + int(((double)1 - button_draw_counter) * 32) - 32, 360 - 16, H_BUTTON_PRESS, TRUE);//��G�{�^��
			}
		}
		if (difficulty <= 3) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
			DrawGraph(960 - 16 + int(((double)button_draw_counter - 1) * 32) + 32, 360 - 16, H_BUTTON_G, TRUE);//�EG�{�^��
			if (Key[Button[1][3]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
				DrawGraph(960 - 16 + int(((double)button_draw_counter - 1) * 32) + 32, 360 - 16, H_BUTTON_PRESS, TRUE);//�EG�{�^��
			}
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	if (activityState == FLAG_END_FUNCTION_STATE) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * (GAME_passed_time - CoverClosedTime) / 1200));
		DrawGraph(0, 0, H_DARKNESS, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	}
	/*
	if (debug == 1) {
	SetFontSize(28);
	DrawString(int(120 - ((double)150 / 2)) + 2, 690 + 2, "DEBUG MODE", GetColor(0, 0, 0));//�f�o�b�O�\��(�e)
	DrawString(int(120 - ((double)150 / 2)), 690, "DEBUG MODE", GetColor(255, 255, 255));//�f�o�b�O�\��
	}
	*/

	//printfDx(L"LOOP7:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);



	if (*debug == 1 && config.ShowDebug == 1) {
		printfDx(L"AUTO PLAY\n");
	}
	if (ScoreShowMode == 1) {
		printfDx(L"RIVAL SCORE�\��\n");
	}

	/*
	printfDx(L"Global:%d\n", Music[song_number].global[difficulty]);
	printfDx(L"Local:%d\n", Music[song_number].local[difficulty]);
	printfDx(L"Color:%d\n", Music[song_number].color[difficulty]);
	printfDx(L"Streak:%d\n", Music[song_number].longNote[difficulty]);
	printfDx(L"Unstability:%d\n", Music[song_number].unstability[difficulty]);
	printfDx(L"Chain:%d\n", Music[song_number].chain[difficulty]);
	*/
	/*
	printfDx(L"R:%d\n", Music[song_number].ColorNotesAmount[difficulty][0]);
	printfDx(L"G:%d\n", Music[song_number].ColorNotesAmount[difficulty][1]);
	printfDx(L"B:%d\n", Music[song_number].ColorNotesAmount[difficulty][2]);
	printfDx(L"C:%d\n", Music[song_number].ColorNotesAmount[difficulty][3]);
	printfDx(L"M:%d\n", Music[song_number].ColorNotesAmount[difficulty][4]);
	printfDx(L"Y:%d\n", Music[song_number].ColorNotesAmount[difficulty][5]);
	printfDx(L"W:%d\n", Music[song_number].ColorNotesAmount[difficulty][6]);
	printfDx(L"D:%d\n", Music[song_number].ColorNotesAmount[difficulty][7]);
	printfDx(L"F:%d\n", Music[song_number].ColorNotesAmount[difficulty][8]);

	for (i = 0; i < 9; i++) {
		printfDx(L"i:%d\n", Music[song_number].LocalNotesAmount[difficulty][i]);
	}
	*/

	//printfDx(L"�u��BPM:%d\n", Music[song_number].speed_list[difficulty]);

	if (config.Vsync == 0) {
		i = 0;
		while (LOOP_passed_time + i < (double)1000 / config.Fps) {//FPS�����肳���邽�ߑ҂�
			WaitTimer(1);
			i++;
		}
	}

	//Sleep(7);
	ScreenFlip();
	clsDx();
}

wchar_t* SongSelect::SongSelectScreen::announce_str(int StageCount, int PlayCount)
{
	//PlayCount��0,1,2�̂Ƃ���
	static wchar_t str[300] = L" ";

	if (PlayCount == 0) {
		return L"�y�͂��߂Ă��z���̕��ցz�@�u�㉺�L�[�v�ƁuEnter�L�[�v�Ńt�H���_��I�сA�Ȃ�I��ŉ������B�u���E�L�[�v�œ�Փx�ύX���s���܂��B�ŏ��͍~���m���̏��������ꂽ�Ȃ������߂ł��B";
	}

	if (PlayCount == 1) {
		return L"���߂Ẵv���C�����l�ł����B�ԗΐ̃L�[�z�u�ɂ͊���܂������H�@���͂����̃L�[�����ŉ��̐}�̂悤�ɁA���̃V�X�e���̑�������邱�Ƃ��ł��܂��B�I�v�V�������J���Ɨl�X�Ȑݒ肪�s���܂��B����ł͎��̋Ȃ�I��ł݂܂��傤�B";
	}

	if (PlayCount == 2) {
		return L"�v���C�����l�ł����B��Փx�͓K�؂ł������H�@�F�̔F��������ꍇ�̓I�v�V�������J���ACOLOR��RAINBOW�ɂ��邱�ƂŐF���C�ɂ����y���߂܂��B�@����ňē��͈ȏ�ƂȂ�܂��B�@�ǂ�����������肨�߂������������B";
	}

	//�`���[�g���A���Ȍ�

	time_t now;//���ݎ���(�O���j�b�W�W����)
	struct tm tm_now;//���ԍ\����
	errno_t error = 0;

	now = time(NULL);//�����擾
	error = _localtime64_s(&tm_now, &now);

	if (StageCount == 0) {//�N����
		wchar_t* Greeting[3] = { L"���͂悤�������܂��B",L"����ɂ��́B",L"����΂�́B" };
		wchar_t DayOfWeek[7][3] = { L"��", L"��",L"��",L"��",L"��",L"��",L"�y" };

		int GreetingUse = 0;//�ǂ̂��������g����

		if (tm_now.tm_hour >= 5 && tm_now.tm_hour < 9) {//5���`9���܂ł́u���͂悤�������܂��v
			GreetingUse = 0;
		}
		else if (tm_now.tm_hour >= 9 && tm_now.tm_hour < 17) {//9���`17���܂ł́u����ɂ��́v
			GreetingUse = 1;
		}
		else {//����ȊO(17�`5��)�́u����΂�́v
			GreetingUse = 2;
		}

		sprintfDx(str, L"%d��%d��%s�j���@%s�������nature prhysm���R�Ǘ��Z���^�[�ł��B�{���������ق����������肪�Ƃ��������܂��B��������肨�߂������������B",
			tm_now.tm_mon + 1, tm_now.tm_mday, DayOfWeek[tm_now.tm_wday], Greeting[GreetingUse]);

		return str;
	}

	const int GeneralStrNum = 25;
	wchar_t* GeneralStr[GeneralStrNum] = {
	L"�y�q���g:�X�s�[�h���߁z�������l�܂肷���Č��Â炢�Ƃ��́A�I�v�V������SPEED��ύX�����₷�������ɂ��Ă݂܂��傤�B",
	L"�y�q���g:�����̎�ށz�����̎�ނ́u��,��,��,���v�̑��Ɂu���F,��,���F�v��u���v������܂��B�@���ꂼ��܂�A�J��Փx�ȏォ��o�����܂��B�@���̐F�������đΏ����܂��傤�B�@�܂��A�@���Ă͂����Ȃ���������������܂��B",
	L"�y�q���g:����̎�ށz����̎�ނ�4����AMISS,GOOD,PERFECT,SKY PERFECT������܂��B�@�^�C�~���O�悭�@���ėǂ�����𑽂��Ƃ�n�C�X�R�A��ڎw���܂��傤�B",
	L"�y�q���g:�_���ƃ����N�z�X�R�A�ɉ����ċȂɂ̓����N���t���܂��B�@F:5000�_�����@E:5000�_�ȏ�@D:6000�_�ȏ�@C:7000�_�ȏ�@B:8000�_�ȏ�@A:9000�_�ȏ�@S:9500�_�ȏ�ł��B",
	L"�y�q���g:�Q�[�W�̎�ށz�I�v�V�����ŃQ�[�W�̎�ނ�I�Ԃ��Ƃ��ł��܂��B�@��茵�����Q�[�W�ł̃N���A��ڎw���܂��傤�B�@���K�ɂ̓Q�[�W�̌��邱�Ƃ�����NO FAIL�Q�[�W���������߂ł��B",
	L"�y�q���g:�Q�[�W�̑����z�~�X�������ăQ�[�W�������Ȃ�Ǝ��s�ɂȂ�܂����A�ǂ���������ƃQ�[�W�͑����Ă����܂��B�@�����Ă��鉹���͑����ʂ��Q�{�Ȃ̂Œ@�������Ȃ��悤�ɂ��܂��傤�B",
	L"�y�q���g:�F�̊o�����z�����̐F�ƒ@���ꏊ�͎��R���i�ƑΉ������Ċo����Ɨǂ��ł��傤�B�@�Ⴆ�΁A�͏�ɍL�����̐F�c�c�̂悤�ɁB",
	L"�y�q���g:���x�ω��z�Ȃ̒��ɂ͓r���ő��x���ω�������̂�����܂��B�@�E��ɗ\���l���\������Ă��܂����H�ɗ\�񂪊O���ꍇ������܂��B�@���̂��ߏ�ɒ��ӂ𕥂��Ă��������B",
	L"�y�q���g:�������G���A�z�u???�v�ƕ\������Ă��鍀�ڂ͖������G���A�ł��B�@�댯�ȃG���A�̂��ߗ�������͋ւ����Ă��܂��B�@�����꒲������ꍇ�͌��d�Ɍx�����Ă��������B�@���S���m�F���ꂽ�ꍇ�A�G���A�͉������܂��B",
	L"�y�q���g:��B�̃R�c�z�����̎��͂�菭����̓�Փx���v���C�������邱�Ƃ���B�ւ̋ߓ��ł��B",
	L"�y�q���g:RAINBOW�I�v�V�����z�F��F������̂�����ꍇ�́A�I�v�V������COLOR��RAINBOW�ɂ��Ă݂܂��傤�B�@�F���C�ɂ����v���C���邱�Ƃ��ł��܂��B",
	L"�y�q���g:�N�C�b�N���g���C�z�N���A�Ɏ��s���Ă��܂����Ƃ��͐ԗΐ����������Ă݂܂��傤�B�@���U���g��ʂɈړ����������Ƀ��g���C���邱�Ƃ��ł��܂��B",
	L"�y�q���g:���R�Ǘ��Z�p�Ҍ���z���R�Ǘ��Z�p�Ҍ�����ł͎����̎��͂𑪂邱�Ƃ��ł��܂��B�@��荂�����x����ڎw���܂��傤�B",
	L"�y�q���g:�C�ۃ��[�_�[�z�Ȃ�I�ԂƂ��A���̋C�ۃ��[�_�[�ɂ͕��ʂ̌X����������܂��B�@Global:���ϖ��x Local:�ő�Ǐ����x Chain:�c�A�œx Unstability:���x�ω��x Streak:�����O�m�[�c�x Color:�F�̕��G�� Total:���v�l ��\���Ă��܂��B",
	L"�y�q���g:�X�L�����[�_�[�z�ȃt�H���_��I�ԂƂ��A���̃��[�_�[�ɂ͕��ʌX�����̃X�L����������܂��B�@�C�ۃ��[�_�[���傫���Ȃō����X�R�A�����A�X�L����L�΂��܂��傤�B",
	L"�y�q���g:�u���b�N�����O�m�[�g�z�I�[�����������O�m�[�g�͏I�[�ł������藣���Ȃ���MISS�ɂȂ�܂��B�@�������ςȂ��ɂ��Ȃ��悤�ɒ��ӂ��܂��傤�B",
	L"�y�q���g:�O���f�[�V���������O�m�[�g�z�����O�m�[�g�͓r���ŐF���ς�邱�Ƃ�����܂��B�@�F�̕ω��ɍ��킹�ĉ����܂��傤�B",
	L"�y�q���g:�J���}�z�E�̐F�̕t�����_�O���t�͊e�F�̉������x�������Ă��܂��B�@�����܂���O���t�͉������x�̎��Ԑ��ڂ�\���Ă��܂��B",
	L"�y�q���g:NIGHT�I�v�V�����z�����̐F�����Â炢�Ƃ��̓I�v�V������NIGHT��ύX���Ĕw�i���Â����Ă݂܂��傤�B",
	L"�y�q���g:���U���g�z���U���g�ŕ\�������V�C�\������Ă݂܂��傤�B�@�V�C�����x���ڂ�\���A�~���m�����Q�[�W���ڂ�\���Ă��܂��B",
	L"�y�q���g:�N�C�b�N�v���C�z�Ȃ��n�܂�O��Enter�L�[�������Ƃ����ɋȂ��J�n�ł��܂��B",
	L"�y�q���g:�`�`Music�n���z�`�`Music�n���́A�������牓�����ꂽ�ꏊ�������Ă��܂��B�@���͋C���C�ɓ������ꍇ�͂�����ɕ����̂��ǂ��ł��傤�B",
	L"�y�q���g:WIND BREAK�V�X�e���z���t����Ctrl��Shift�L�[�ŃJ�o�[�̈ʒu�𒲐߂ł��܂��B �������l�܂��Ă���ӏ��ŉ�����ƌ��₷���Ȃ�܂��B",
	L"�y�q���g:NOTE SYMBOL�I�v�V�����z�I�v�V������NOTE SYMBOL��ύX����Ɖ����̐F���ɕ\������L����ύX�ł��܂��B�@�F�̔F���Ɋ���Ă��Ȃ�����\"arrow\"�������߂ł��B",
	L"�y�q���g:SPEED ADAPTER�I�v�V�����z�I�v�V������SPEED ADAPTER�ŉ����̃X�s�[�h���ǂ��ɍ��킹�邩�����ł��܂��B�@���߂ăv���C���镈�ʂł�\"FAST\"�ɒ�������̂������߂ł��B"
	};
	int GeneralStrUse = 0;//�ǂ̒ʏ�A�i�E���X���g����
	GeneralStrUse = GetRand(GeneralStrNum - 1);//
	//GeneralStrUse = 14;
	return GeneralStr[GeneralStrUse];
}

void SongSelect::SongSelectScreen::DrawRadarLine(double angle)
{
	//short r[6] = { v1,v2,v3,v4,v5,v6 };//���a
	short r = 99;
	short o[2] = { 160,360 };//���_���W

	float X[2] = { 0,0 }, Y[2] = { 0,0 };

	//short sum = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
	//short th = 75;//�F��ω�������sum��臒l
	//double ind = 0;//0~255
	int color = GetColor(255, 255, 255);

	float d = (float)1 / 60;//

	int range = 10;//���̕�

	//100��120pix�̒���
	for (int i = 0; i <= range; i++) {
		float id = (float)i / 60;

		X[0] = (float)(o[0] + sin((3.14159265 / 3) * (angle + id - d)) * r * 1.2);
		Y[0] = (float)(o[1] - cos((3.14159265 / 3) * (angle + id - d)) * r * 1.2);

		X[1] = (float)(o[0] + sin((3.14159265 / 3) * (angle + id)) * r * 1.2);
		Y[1] = (float)(o[1] - cos((3.14159265 / 3) * (angle + id)) * r * 1.2);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(80 * (double)i / range));
		//����

		DrawTriangle((int)X[0], (int)Y[0],
			(int)X[1], (int)Y[1],
			o[0], o[1], color, TRUE);
	}

	return;
}

int SongSelect::SongSelectScreen::GetCylinderColor(double val, double range, unsigned int R, unsigned int G, unsigned int B)
{
	int color = 0;
	double indR = 0;
	double indG = 0;
	double indB = 0;

	double th = range / 6;
	if (val <= th) {//
		indR = (double)R * val / th;
		indG = (double)G * val / th;
		indB = (double)B * val / th;
		color = GetColor((int)indR, (int)indG, (int)indB);
	}
	else if (val <= th * 5) {//
		color = GetColor(R, G, B);
	}
	else if (val <= th * 6) {//
		indR = (double)R * (1 - ((val - 5 * th) / th));
		indG = (double)G * (1 - ((val - 5 * th) / th));
		indB = (double)B * (1 - ((val - 5 * th) / th));
		color = GetColor((int)indR, (int)indG, (int)indB);
	}
	else {
		color = GetColor(0, 0, 0);
	}
	return color;
}

int SongSelect::SongSelectScreen::GetRainbowColorForRadar(int val, int th)
{
	int color = 0;
	double ind = 0;
	if (val <= th) {
		ind = (double)255 * val / th;
		color = GetColor(0, (int)ind, 255);
	}
	else if (val <= th * 2) {
		ind = (double)255 * ((double)val - th) / th;
		color = GetColor(0, 255, 255 - (int)ind);
	}
	else if (val <= th * 3) {
		ind = (double)255 * ((double)val - (double)2 * th) / th;
		color = GetColor((int)ind, 255, 0);
	}
	else if (val <= th * 4) {
		ind = (double)255 * ((double)val - (double)3 * th) / th;
		color = GetColor(255, 255 - (int)ind, 0);
	}
	else if (val <= th * 5) {
		ind = (double)255 * ((double)val - (double)4 * th) / th;
		color = GetColor(255, 0, (int)ind);
	}
	else if (val <= th * 6) {
		ind = (double)255 * ((double)val - (double)5 * th) / th;
		color = GetColor(255 - (int)ind, 0, 255 - (int)ind);
	}
	else {
		color = GetColor(0, 0, 0);
	}
	return color;
}

void SongSelect::SongSelectScreen::DrawHexagon(short v1, short v2, short v3, short v4, short v5, short v6)
{
	short r[6] = { v1,v2,v3,v4,v5,v6 };//���a
	short o[2] = { 160,360 };//���_���W
	float posX[6] = { 0,0,0,0,0,0 };
	float posY[6] = { 0,0,0,0,0,0 };

	float posX_i[6] = { 0,0,0,0,0,0 };//�����p
	float posY_i[6] = { 0,0,0,0,0,0 };

	short sum = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
	short th = 75;//�F��ω�������sum��臒l
	double ind = 0;//0~255
	int color = GetColor(255, 255, 255);

	color = GetRainbowColorForRadar(sum, th);//�Ή�������F�𓾂�

	//100��120pix�̒���
	for (int i = 0; i <= 5; i++) {
		posX[i] = (float)(o[0] + sin((3.14159265 / 3) * i) * r[i] * 1.2);
		posY[i] = (float)(o[1] - cos((3.14159265 / 3) * i) * r[i] * 1.2);

		posX_i[i] = (float)(o[0] + sin((3.14159265 / 3) * i) * (r[i] + 0.5) * 1.2);
		posY_i[i] = (float)(o[1] - cos((3.14159265 / 3) * i) * (r[i] + 0.5) * 1.2);
	}




	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	//����
	for (int i = 0; i <= 5; i++) {
		DrawTriangleAA(posX_i[i], posY_i[i], posX_i[(i + 1) % 6], posY_i[(i + 1) % 6], o[0], o[1], color, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 256);
	//�O�g
	for (int i = 0; i <= 5; i++) {
		DrawLineAA(posX[i], posY[i], posX[(i + 1) % 6], posY[(i + 1) % 6], GetColor(30, 30, 30), 2);
	}

	return;
}

void SongSelect::SongSelectScreen::OptionValueChange(Option* Option, int option_select, int add)
{
	if (add >= 0) {
		for (int i = 0; i < add; i++) {
			Option->op.list[option_select]->incrementIndex();
		}
	}
	else {
		for (int i = 0; i < -add; i++) {
			Option->op.list[option_select]->decrementIndex();
		}
	}
	return;
}

void SongSelect::SongSelectScreen::DrawOptionSentence(Option* Option, OptionItem::Name option_select, Config config, int FontHandle)
{
	int i = 0;

	//�I��ł���I�v�V�����̐������̐擪�A�h���X
	const auto desc = Option->op.list[(int)option_select]->getDescription();

	const auto StrAddress = desc.c_str();

	int width = 0;
	width = GetDrawStringWidthToHandle(StrAddress, wcslen(StrAddress), FontHandle);
	SetDrawScreen(Option->H_SENT);//�I�v�V���������摜��`��Ώۂ�
	ClearDrawScreen();//�O�̉摜������
	//������`�悵�ĉ摜�쐬
	SetDrawMode(DX_DRAWMODE_BILINEAR);//�o�C���j�A�ŕ`��
	ShowExtendedStrFitToHandle(320, 7, StrAddress, width, 620, config, FontHandle);
	SetDrawMode(DX_DRAWMODE_NEAREST);//�o�C���j�A����߂�

	SetDrawScreen(DX_SCREEN_BACK);//�`��Ώۂ𗠉�ʂɖ߂�
	return;
}

int SongSelect::SongSelectScreen::clearStateConverter(int clearState)
{
	switch (clearState)
	{
	case CLEARTYPE_NO_PLAY:
		return 0;
		break;
	case CLEARTYPE_PLAY:
		return 1;
		break;
	case CLEARTYPE_FAILED:
		return 2;
		break;
	case CLEARTYPE_EASY_CLEARED:
		return 3;
		break;
	case CLEARTYPE_CLEARED:
		return 4;
		break;
	case CLEARTYPE_HARD_CLEARED:
		return 5;
		break;
	case CLEARTYPE_SUPER_HARD_CLEARED:
		return 6;
		break;
	case CLEARTYPE_FULL_COMBO:
		return 7;
		break;
	case CLEARTYPE_PERFECT:
		return 8;
		break;
	default:
		break;
	}
}

