#pragma once
#include "Screen.h"
#include "STRUCT_RESULT.h"
#include "STRUCT_OP.h"
#include "STRUCT_Song.h"
#include "STRUCT_ANDROID_CONTROLLER.h"
#include "STRUCT_CONFIG.h"
#include "STRUCT_IR_SETTING.h"
#include "Asset.h"
#include "AppContext.h"
#include "ActivityContext.h"
#include "LED_operation.h"
#include "GetValueFromController.h"
#include "STRUCT_EDIT_SCORE.h"
#include "Drawable.h"
#include "RetryView.h"
#include <EffekseerForDXLib.h>
#include "ScoreGraphDiffView.h"
#include "STRUCT_GAME_SH.h"
#include "EscapingView.h"
#include "STRUCT_CALCDIFF.h"
#include "AutoDifficultyPrediction.h"
#include "STRUCT_SEQ.h"
#include "NoteJudgeProcessor.h"
#include "STRUCT_CELL.h"
#include "EDIT_SCORE.h"
#include "GradationNoteImage.h"
#include "NoteTextImage.h"
#include "CoverView.h"
#include "DetailView.h"
#include "AuroraView.h"

namespace Game {



	class SpeedBuffer {
		NOTE** note;
		int* noteIndex;//�e���[�����̃C���f�b�N�X

		float average = 0;

		static const int SIZE = 1;//��������

	public:
		SpeedBuffer(NOTE** note, int* j_n_n) {
			SpeedBuffer::note = note;
			SpeedBuffer::noteIndex = j_n_n;
		}

		void updateAverage() {
			int index[4] = { noteIndex[0],noteIndex[1], noteIndex[2], noteIndex[3] };//�T���p�C���f�b�N�X


			float averageBuf = 0;

			int averageCount = 0;
			for (averageCount = 0; averageCount < SIZE; averageCount++) {
				//4�񂩂�^�C�~���O����ԋ߂����̂�T��
				int timingBuf = 0;
				int isFoundFirst = TRUE;
				for (int i = 0; i < 4; i++) {
					if (note[i][index[i]].color != NoteColor::NONE && note[i][index[i]].color != NoteColor::K) {
						if (isFoundFirst) {
							timingBuf = note[i][index[i]].timing_real;
							isFoundFirst = FALSE;
						}

						if (timingBuf > note[i][index[i]].timing_real) {
							timingBuf = note[i][index[i]].timing_real;
						}

					}
				}

				if (isFoundFirst) {//�c�艹����3�ȉ��̎��͖����Ȃ������_�Ŕ�������
					if (averageCount == 0)return;
					break;
				}

				//�^�C�~���O���������̂�����Α����𕽋ς���
				//���̃��[����noteIndex���C���N�������g
				int count = 0;
				float speedSum = 0;
				for (int i = 0; i < 4; i++) {
					if (note[i][index[i]].color != NoteColor::NONE && note[i][index[i]].color != NoteColor::K) {
						if (timingBuf == note[i][index[i]].timing_real) {
							speedSum += note[i][index[i]].bpm_real;
							index[i]++;
							count++;
						}
					}
				}
				//�����Z�o
				speedSum /= count;

				averageBuf += speedSum;
			}

			average = averageBuf / averageCount;
			return;
		}

		float getAverage() {
			return average;
		}
	};

	class GameScreen : public Screen
	{
	public:
		GameScreen(
			int song_number, int difficulty,
			RESULT* res, int* escape, Option* option, int* retryAble,
			int* debug, Song* Music, int Button[3][4], int Button_Shutter, int* Key, char* Buf,
			int secret,//�B���ȉ��o����(0:�ʏ� 1:���o��)
			ANDROID_CONTROLLER* AC,
			Config config,
			IR_SETTING* ir,
			int SkillTestFlag,//�i�ʔF�胂�[�h��(0:�ʏ� 1~4:�i�ʔF�胂�[�h �X�e�[�W��������)
			double* GaugeVal,
			int* CourseCombo,
			int* CourseMaxCombo,
			int AllowExit
		) :
			song_number(song_number),
			difficulty(difficulty),
			res(res),
			escape(escape),
			option(option),
			retryAble(retryAble),
			debug(debug),
			Music(Music),
			Button_Shutter(Button_Shutter),
			Key(Key),
			Buf(Buf),
			secret(secret),
			AC(AC),
			config(config),
			ir(ir),
			SkillTestFlag(SkillTestFlag),
			GaugeVal(GaugeVal),
			CourseCombo(CourseCombo),
			CourseMaxCombo(CourseMaxCombo),
			AllowExit(AllowExit)
		{
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					GameScreen::Button[i][j] = Button[i][j];
					Keylist[i][j] = Button[i][j];
				}
			}
		}

		/// <summary>
		/// ����������
		/// </summary>
		void init() override;

		void updateModel() override;

		void updateView() override;

		void updateModelReady();
		void updateViewReady();

		void startPlaying();

		void updateModelPlaying();
		void updateViewPlaying();

		/// <summary>
		/// �����摜�Ȃǂ̐����`�摜���w����W�𒆐S��[scale]�̔{���ŕ`��
		/// </summary>
		/// <param name="x">x���W</param>
		/// <param name="y">y���W</param>
		/// <param name="handle">�C���[�W�n���h��</param>
		/// <param name="TransFlag">���߃t���O</param>
		void drawSquareImage(double x, double y, int handle, int TransFlag);

		void drawNotes(SongSub& MusicSub, int difficulty, NOTE** note, int BlendMode, int BlendVal, int  H_NOTE[12], double flash, const int FLASH_VALUE, const int FLASH_VALUE_ALWAYS, NoteSearcher& noteSearcher, Option* option, GradationNoteImage& gradationLongNote, int  H_LNOTE[12], int  LN_flash[4], int  j_n_n[4], const int FLASH_VALUE_LN_PUSH, NoteSymbolImage& noteSymbol);

		//�t���R���{�G�t�F�N�g
		void drawFullComboRainbow(int* play, int* step, int Time, int baseTime, int effectResourceHandle, int* playingEffectHandle, int PFC);

		//���ߓx�̎擾 Movie���Đ����邩�ɂ���ĕς��܂�
		double getAlpha(int alphaMovieOff, int alphaForMovieNormal, int alphaForMovieClear, int alphaInit = 255);

		int song_number;
		int difficulty;
		RESULT* res;
		int* escape;
		Option* option;
		int* retryAble;
		int* debug; Song* Music;
		int Button[3][4];
		int Button_Shutter;
		int* Key;
		char* Buf;
		int secret;//�B���ȉ��o����(0:�ʏ� 1:���o��)
		ANDROID_CONTROLLER* AC;
		Config config;
		IR_SETTING* ir;
		int SkillTestFlag;//�i�ʔF�胂�[�h��(0:�ʏ� 1~4:�i�ʔF�胂�[�h �X�e�[�W��������)
		double* GaugeVal;
		int* CourseCombo;
		int* CourseMaxCombo;
		int AllowExit;

		enum GameScreenPhase {
			Ready,//���t�J�n�O
			Playing//���t��
		};

		GameScreenPhase game_screen_phase = Ready;

		int stop_play_key = KEY_INPUT_SPACE;//�Đ���~�Ɏg�p����L�[


		Asset asset;//�g���摜�Z�b�g
		//�R���e�L�X�g
		AppContext appContext = AppContext(NULL, option, &config);
		ActivityContext context = ActivityContext(&appContext, &asset);

		HANDLE hComm, hEvent;
		STATE LED_state;
		OVERLAPPED ovl;
		DWORD dwWritten;


		int controllerVolume = 0;
		GetValueFromController ControllerVolume = GetValueFromController(hComm);

		SCORE_CELL score_cell_head;


		double GAME_start_time;
		double GAME_passed_time;//�P�ʂ�ms ����Ɖ��p�̌o�ߎ���
		double GAME_passed_time_for_draw;//���ʕ`��p�̌o�ߎ���(�f�B�X�v���C�x���␳�p)

		double LOOP_passed_time = 1;//1���[�v�ɂ�����������(ms)
		double CounterRemainTime = 0;////�J�E���^�[�̒l��1ms���ϓ����邽�߂̎��ԁ@1ms��������ď����ȉ��͒~�ς���
		double time_cash = 0;//LOOP_passed_time���Z�o���邽�߂̋L���ϐ�
		double TimePerFrame = 1000.0 / config.Fps;//1�t���[���̎���


		int H_NOTE[12];//�����摜(0�͖�����1~9��RGBYCMWKF 10:�_�ŗp 11��LN��@�������Ɍ��点�郌�C���[�p )
		//int H_NOTE_OR_FRAME;//OR�m�[�g�̘g

		int H_LNOTE[12];//LN�̒��ԗp�摜�n���h��(0�͖�����1~9��RGBYCMWK(����)F)
		//int H_LNOTE_OR_FRAME;//LN��OR�m�[�g�̘g

		int H_JUDGE_AREA, H_BG;//�摜�f�[�^�̃n���h��
		int H_JUDGE_AREA_PAINT;
		int H_CLOUD;

		int H_COMBO;
		int H_COMBO_NUMBER[10];
		int H_SCORE_NUMBER[10];
		int H_HIT[16], H_HIT_LARGE[20], H_HIT_D[16];//�m�[�g��@�������̃t���b�V���摜�n���h��
		int H_SKY_PERFECT, H_PERFECT, H_GOOD, H_MISS;//SKY_PERFECT,PERFECT,GOOD,MISS����摜�̉摜�n���h��
		int H_COVER = 0;//�J�o�[�摜
		int H_COVER_FLASH;//�J�o�[�t���b�V���摜
		int H_COVER_MIDDLE;//���S�J�o�[

		int H_GAUGE_BOX;//�Q�[�W�̉��ɕ\��
		int H_GAUGE_BAR = 0;//�Q�[�W�摜
		int H_FAILED;//
		int H_CLEARED;//
		int H_FULL_COMBO[6];//
		int H_PFC;//
		int H_COVER_STR;//�E���̕����摜
		int H_DIFFICULTY = 0;//��Փx�摜
		int H_R;

		int H_BUTTON_R;
		int H_BUTTON_G;
		int H_BUTTON_B;
		int H_BUTTON;//�X�C�b�`�摜
		int H_BUTTON_PRESS;//�X�C�b�`�����摜

		int H_FALL[8];//������@�������Ɍ��ɗ��������

		int H_TIME_NUMBER[10];//����ɕ\�����鎞�Ԃ̉摜
		int H_TIME_COLON;

		int H_POP_NUMBER[10];

		int H_DARKNESS;//�w�i���Â�����p�̍����摜
		int H_GAME_STR_JUDGE_BPM;
		int H_GAME_STR_SCORE_GRAPH;

		DrawableInitParam retryViewParam = DrawableInitParam(Cordinate(320, 600));
		RetryView retryView = RetryView(&context, retryViewParam);
		
		TransValue viewAlpha = TransValue(&context);

		std::function<void(void)> playAnimationOnEscAtStart;

		std::function<void(void)> playAnimationOnEscAtPlay;

		std::function<void(void)> playAnimationOnFinish;


		int FXH_FULLCOMBO = LoadEffekseerEffect(L"img/FullComboFX/FullComboFX.efk", 25.0f);//effekseer�G�t�F�N�g���\�[�X�n���h��
		// �Đ����̃G�t�F�N�g�̃n���h��������������B
		int FXPH_FULLCOMBO = -1;
		int FullComboFXPlayFlag = 1;//�t���R���{�G�t�F�N�g�Đ��t���O
		int FullComboFXBaseTime = 0;
		int FullComboFXFrame = 0;

		//�摜�Ƃ��ĕۑ����ꂽ�Ȗ��̉摜�n���h��
		int H_TITLE_STR = MakeScreen(640, 48, TRUE);

		//�摜�Ƃ��ĕۑ����ꂽ�W�������̉摜�n���h��
		int H_GENRE_STR = MakeScreen(640, 48, TRUE);

		//�摜�Ƃ��ĕۑ����ꂽ�A�[�e�B�X�g�̉摜�n���h��
		int H_ARTIST_STR = MakeScreen(640, 48, TRUE);

		int screenHandle;

		DrawableInitParam high_score_diff_init_param = DrawableInitParam(Cordinate(960, 450), CenterRatio(0, 0));
		ScoreGraphDiffView high_score_diff = ScoreGraphDiffView(&context, high_score_diff_init_param);

		GAME_SH SH;
		int AllowSound[3][4] = { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } };//���̃t���[���ŃR���g���[����@�������̉����o���� �R���g���[���̕��я��Ɠ���(0:B 1:G 2:R)
		double HitAreaBright[3][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0} };//�R���g���[����@�������̔���g�̊e�F�̌��

		int SH_SONG;//�ȃf�[�^�̃n���h��
		TransValue songVolume = TransValue(&context);


		int	SH_CLOSE;
		int	SH_CLOSED;
		int	SH_OPEN;
		int SH_JINGLE;

		TransValue jingleVolume = TransValue(&context);
		int SH_SHUTTER;
		int SH_SHUTTER_SIGNAL;


		int i = 0, j = 0, k = 0;
		double cbpm = 120;//���݂�BPM
		double speed = 0.00000000001;//�X�s�[�h����
		double high_speed = 1;//�n�C�X�s
		double width_scale = 1;//���[���̉��̍L���蕝�̔{��
		int laneCordinateX[4] = {0,0,0,0};

		int cash = 0;//���̌����ڂ��ȒP�ɂ��邽��
		int judge_area = 618, note_fall = -64;//�m�[�g�̔���G���A�Ɨ����n�߂鍂��-192~490

		double judge_time_sky_perfect = 16.66;//���莞��/2(�p�[�t�F�N�g)ms
		double judge_time_perfect = 40;//���莞��/2(�p�[�t�F�N�g)ms
		double judge_time_good = 130;//���莞��/2(�R���{���Ȃ��锻��̕�/2)�P�ʂ�ms
		double judge_time_bad = 200;//���莞��/2(good�ȏ�bad�����̂���Œ@���ƃR���{���r�؂��)
		double judge_time_through = 180;//���莞��(���̎��Ԉȏ�m�[�c����������note_search���C���N�������g����R���{���؂��)
		double judge_time_dark = 130;//���p�̔��莞��/2
		double judge_time_delay = 200;//LN��������Ɨ����Ă��ǂ��P�\����

		int con_jd = 5;//���p�̔��莞��/2
		int j_n_n[4] = { 0,0,0,0 };//���莞�ԓ��̍ł����ɂ���(���Ə����Ō������ɂȂ�)�܂��@����Ă��Ȃ��m�[�g�̔ԍ�(���ȊO�p)(judge_note_number)
		int j_dn_n[4] = { 0,0,0,0 };//���̍��m�[�c�@������p(judge_darknote_number)
		int j_dn_push_n[4] = { 0,0,0,0 };//���̍��m�[�c�������ݔ���p(judge_darknote_push_number)

		int searching = 0;//judge�֐����ŕω�����j_n_n������ϐ�
		int LN_flag[4] = { 0,0,0,0 };//�����O�m�[�g�����Ă邩�̃t���O(1or2�Ȃ炻���̃��[����LN�����������Ȃ���΂Ȃ�Ȃ��B2�̂Ƃ��Ɏ�𗣂��ƃ~�X�ɂȂ�1�ɂȂ�B -1�ŉ����؂������)
		enum LnFlag {
			Nothing = 0,//LN�͗��Ă��Ȃ�
			Missed = 1,//�����̃��[����LN�����������Ȃ���΂Ȃ�Ȃ� �~�X�ς�
			MustPush = 2,//�����̃��[����LN�����������Ȃ���΂Ȃ�Ȃ� ��𗣂��ƃ~�X�ɂȂ�Missed�ɂȂ�
			Completed = -1,//�����؂������
		};

		int LN_judge[4] = { 0,0,0,0 };//LN�̔����ۑ�����(0:MISS 1:GOOD 2:PERFECT 3;SKY_PERFECT)
		int LN_flash[4] = { 0,0,0,0 };
		int LN_push[4] = { 0,0,0,0 };//LN�̐F�Ń{�^���������Ă��邩�̃t���O �Ή�����F�������Ă���Ƃ���1,�������t���[����-1,����ȊO�̗����Ă���Ƃ�0
		int dark_hit = 0;//����@���Ă��܂�����
		int note_search = 0;//�����珇�Ƀm�[�g��T���Ă����J�E���^
		int timingDifference = -10000;//�m�[�g��@�������̃^�C�~���O�̂���
#define NOTE_HIT_LARGE_FLASH_NUMBER 4//�����ɏd�˂ĕ\�������
		double note_hit_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { { -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 } };//�m�[�g��@�������̃t���b�V����\�����邽�߂̃J�E���^
		int note_hit_flash_rounder = 0;//4�ȓ��ŏ��X�Ƀt���b�V����\�����邽�߂̃J�E���^(0~3�ŉ��)
		double note_hit_large_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { { -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 } };//����m�[�g��@�������̃t���b�V����\�����邽�߂̃J�E���^
		int note_hit_large_flash_rounder = 0;//4�ȓ��ŏ��X�Ƀt���b�V����\�����邽�߂̃J�E���^(0~3�ŉ��)
		double dnote_hit_flash[4] = { -1,-1,-1,-1 };//���m�[�g��@�������̃t���b�V����\�����邽�߂̃J�E���^
		const int FLASH_VALUE = 85;//�����m�[�g�̓_�ŋP�x
		const int FLASH_VALUE_ALWAYS = 170;//�����m�[�g�̏펞�_�ŋP�x
		const int FLASH_COVER_VALUE = 15;//�J�o�[�̓_�ŋP�x
		const int FLASH_VALUE_LN_PUSH = 50;

		double hit_sky_perfect[4] = { 0,0,0,0 };//����摜��SKY_PERFECT��\������J�E���^
		double hit_perfect[4] = { 0,0,0,0 };//����摜��PERFECT��\������J�E���^
		double hit_good[4] = { 0,0,0,0 };//����摜��GOOD��\������J�E���^
		double hit_miss[4] = { 0,0,0,0 };//����摜��MISS��\������J�E���^
		double combo_draw_counter = 0;//�R���{�摜��\������J�E���^
		double c_m_draw_counter = 0;//���S�J�o�[�摜��\������J�E���^(0~1 0:�X�^�[�g�O�A�I���� 1:�v���C��)
		double str_draw_counter = -1;//CLEARED FAILED�摜��\������J�E���^
		int TimeFromEndOfGameToResult = 4000;//CLEARED FAILED���\������Ă��烊�U���g�Ɉڂ�܂ł̎���(ms)

		int combo = 0;//�R���{
		double score = 0;//�X�R�A
		int SKY_PERFECT = 0;//SKY_PERFECT��
		int PERFECT = 0;//PERFECT��
		int GOOD = 0;//GOOD��
		int MISS = 0;//MISS��
		int MAX_COMBO = 0;//�ő�R���{��
		int score_MAX = 10000;//�X�R�A���_�l
		double score_note = 0;//������̓��_
		double score_dec = int(score_MAX * 0.001);//�~�X���̃X�R�A������(���_�l��0.1%��(10�_)�X�R�A����)
		int combo_digit[5] = { 0,0,0,0,0 };//�R���{���������ƂɊi�[
		int score_digit[5] = { 0,0,0,0,0 };//�X�R�A
		int time_minutes_digit[2] = { 0,0 };//��
		int time_hours_digit[2] = { 0,0 };//����
		int bpm_digit[5] = { 0,0,0,0,0 };//BPM
		double gauge = 100;


		double gauge_draw_counter = gauge;//�Q�[�W�����炩�ɕ\�����邽�߂̃J�E���^�[(����)
		double gauge_draw_hosei = gauge;//�Q�[�W��i�X�ɂ₩�Ɍ��������ĕ\�����邽�߂̃J�E���^�[(xtosinx()���g��)
		int pop_digit[3] = { 0,0,0 };//�Q�[�W�ʕ\���p��
		int pop = int(100 - gauge_draw_hosei);//�~����
		int HitingNoteCount = 0;//���������ĉ��̉����̔��肪�I����������\�����l

		double key_draw_counter[3][4];//0�ɂȂ�΃R���g���[���摜���ʒu�ɂ�

		int TimeToEndScroll = 0;//Scroll���Ԃł̉��t�I���^�C�~���O
		int playing_time = 0;//�ŏ��̃m�[�g����Ō�̃m�[�g�܂ł̎���
		double gauge_inc_times = 1;//�Ȃ̒����ɂ��Q�[�W�̑����{��(GAUGE_INC_BASE_TIME�ȏ�ŏオ���Ă���)
		const int GAUGE_INC_BASE_TIME = 120000;//120�b
		const int SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT = 1500;//�i�ʃQ�[�W�̎����̃m�[�g�������total�����߂�
		int PassedTime_Minutes = 0;//Scroll���Ԃł̌o�ߎ��Ԃ̕�
		int PassedTime_Hours = 6;//Scroll���Ԃł̌o�ߎ��Ԃ̎���
		double HourOfGame = 0;//����̎��Ԃ�1���Ԃ�Scroll���Ԃł�����
		double MinuteOfGame = 0;//����̎��Ԃ�1����Scroll���Ԃł�����
		int TimePop[18] = { 100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100 };//1���Ԗ��̍~����pop��ۑ�
		int TimeAcc[6] = { 0,0,0,0,0,0 };//3���Ԗ��̐��x��ۑ�
		int TimePerfect = 0;//Perfect����ۑ�(3���Ԗ���0�Ƀ��Z�b�g)
		int TimeGood = 0;//Good����ۑ�(3���Ԗ���0�Ƀ��Z�b�g)
		int TimeMiss = 0;//Miss����ۑ�(3���Ԗ���0�Ƀ��Z�b�g)
		int TimeNextHour = 7;


		double total = 0;//�S��PERFECT�Œ@�����Ƃ��ɑ�����Q�[�W�̃g�[�^����
		double total_base = 0;//�S��PERFECT�Œ@�����Ƃ��ɑ�����Q�[�W�̃g�[�^����(�Ȃ̒����ő�������O�̃x�[�X�̒l)
		double miss_percent = 0;//(250~500�R���{�̎�)��C�Ƀ~�X������FAILED�ɂȂ鑍�m�[�c�ɑ΂��銄��(1:100%)
		double gauge_dec_MIN = 0;//�Q�[�W�����ʂ�4%�ȉ��ɂȂ�Ȃ��悤�ɂ���
		double gauge_dec_MAX = 0;//�Q�[�W�����ʂ�8%�ȏ�ɂȂ�Ȃ��悤�ɂ���
		double gauge_dec = 0;//�~�X���̃Q�[�W������(4�ȉ�8�ȏ�ɂ͂��Ȃ�)
		int ClearFlag = 0;//�N���A�t���O(0:�s�� 1:CLEARED 2:FAILED)
		double draw_alpha = 0;//�摜�̓��ߏ��(0:���� 1:�s����)
		int genre_width = 1;//�W����������
		int title_width = 1;//�Ȗ�����
		int artist_width = 1;//�A�[�e�B�X�g����
		int detail_width = 1;//�ڍ׉���
		int jingleflag = 0;//�W���O����炷�t���O
		int openflag = 0;//���S�J�o�[���J���Ƃ��̉���炷�t���O
		int cleared_time = 0;//�N���A�������Ԃ��L�^
		int isStartCoverMoveUpComplete = 0;//�X�^�[�g���̒��S�J�o�[�`��t���O
		double debug_time_passed = 0;//�߂�����i�񂾂肷�鎞��
		int debug_stop = 0;//���ʍĐ���~
		int debug_warp = 0;//�Đ��ʒu�ړ��������ǂ���(0:���Ă��Ȃ� 1:���Ă���)
		double debug_warp_time = 0;//���[�v���鎞��(ms)1000ms�ȏ�ɂ͂��Ȃ�
		double debug_stop_time = 0;//�Đ���~���邽�߂̎���
		double debug_stoped_time = 0;//�Đ���~�����Ƃ��̃J�E���g
		int debug_warp_counter = 0;//���L�[�������̃J�E���^
		int debug_auto = *debug;//�I�[�g�v���C(�f�o�b�O���[�h�Ȃ�ŏ�����I�[�g�ɂ��Ă���)
		int debug_sound = 1;//1�Ȃ�L�[����炷
		int debug_fail = 1;//0:�X���� 1:�X����
		int debug_music = 1;//�Ȃ�炷���ǂ���

		const int ESC_PRESS_TIME = 750;//ESC�L�[����������
		int esc_press_counter = 0;

		EscapingView escaping_view = EscapingView(&context);

		const double ROOT12_2 = 1.0594630943592952645618252949463;
		char pitch_step = 0;//�s�b�`�������P�ʂłǂꂾ���オ���Ă��邩
		double pitch = 1;//���̐��l�����Đ����x��{�ɂ���
		double frequency = 44100;//�Đ����g��

		double volume = 1;//�Ȃ̃{�����[��
		double flash = 0;//�t���b�V��(�J�o�[�̖��邳)
		int beat = 0;//�i�񂾔�
		double beat_time = 0;//�i�񂾔����Z�o���邽�߂̕ϐ�
		double beat_time_cash = 0;
		double beat_percent = 0;//�O�̔����牽�p�[�Z���g���i��ł��邩
		int bgmplay = 0;//BGM�𗬂������ǂ���(�ŏ��Ɉ�񂾂��Đ����邽�߂̃t���O)
		int FirstBgmPlay = 1;//�����痬��BGM���ŏ��̍Đ����ǂ����̃t���O

		int isMoviePlaying = 0;//BGA�𗬂������ǂ���(�ŏ��Ɉ�񂾂��Đ����邽�߂̃t���O)
		int FirstMoviePlay = 1;//�����痬��BGA���ŏ��̍Đ����ǂ����̃t���O


		CALCDIFF Cdiff;//�v�Z�ŏo������Փx�v�f�ڈ�
		AutoDifficultyPrediction adp;
		double autoDifficultyPredictionResult;

		int bcc = 0;//bpmchange�̃J�E���^
		int scc = 0;//scrollchange�̃J�E���^
		double sc_timing = 0;//�O�ɃX�N���[���ω�������������
		double cscroll = 1;//���݂̎����Ԕ{��(���ʑS�̂̃X�s�[�h�{��)
		double GAME_passed_time_scroll = 0;//���ʂƂ��Ă̌o�ߎ���(scroll���l������)
		double real_timing = 0;//�O��scroll���ω������Ƃ���GAME_passed_time�̎���

		BPMC* bpmchange;//BPM�ύX
		SC* scrollchange;//SCROLL�ύX
		STOP_SE* stopSequence;//���ʒ�~

		int stop_se_c = 0;//stopSequence�̃J�E���^
		double stop_time = 0;//���ʒ�~���邽�߂̎���
		double stop_time_sum = 0;//���܂ł̑����ʒ�~����
		int stopFlag = 0;//���ʒ�~���ǂ����̃t���O(1:��~��)

		NOTE** note;

		NoteSearcher noteSearcher = NoteSearcher(note);

		//���ߐ��\����
		BARLINE* barline;

		int blc = 0;//barline counter

		NOTE hit_n[4];//�Ō�ɒ@�����m�[�g�̏��

		NoteJudgeProcessor noteJudgeProcessor;

		double fall_y = 0;

		//�@���ƌ��ɗ�������̊֘A
		CELL head;//���X�g�̐擪
		CELL* insert;//���X�g�ɑ}������ꏊ�������|�C���^(���̃|�C���^�̑}���Z���̌��ɑ}��)

		//EDIT_SCORE�֐��ɓn���p
		double scale_score = 1;//�g��k����
		int step_array_number = 0;//�X�e�b�v�����l�z��̓Y����
		int measure_start_step = 0;//���ߐ���\������N�_
		int SwingBackBeatTime = SWING_DIVISION / 2;//

		SongSub MusicSub;
		SpeedBuffer speedBuffer = SpeedBuffer(note, j_n_n);

		DATEDATA DateBuf;
		int rand_seed;

		//�X�R�A�Q�[�W�p�̃X�R�A�ǂݍ���
		FILE* fpHighScore = 0;
		FILE* fpLatest = 0;
		FILE* fpRival = 0;
		errno_t error = 0;
		RESULT highScore;//���݂̃n�C�X�R�A
		RESULT latestScore;//�O��̃X�R�A
		RESULT rivalScore;//���C�o���n�C�X�R�A

		wchar_t filenameHighScore[256];
		wchar_t filenameLatest[256];
		wchar_t filenameRival[256];

		wchar_t* scoreDiff[4] = { L"sunny",L"cloudy",L"rainy",L"thunder" };

		//�摜���n���h���l���
		wchar_t* ReadNameRGB[11] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"k",L"f",L"bright",L"longPush" };
		wstring note_folder = option->op.note.toString();

		GradationNoteImage gradationLongNote = GradationNoteImage(wstring(L"img/notes/") + wstring(note_folder.c_str()));
		NoteSymbolImage noteSymbol = NoteSymbolImage(config, option->op.noteSymbol.toString());

		int Keylist[3][4];//�L�[�̃��X�g

		XY pos_cloud;//�_�̍��W

		wstring themeStr1 = wstring(L"img/themes/");
		wstring themeStr2 = wstring(option->op.theme.toString());

		//�����J�o�[
		DrawableInitParam coverViewParam = DrawableInitParam(Cordinate(0, 0));
		CoverView coverView = CoverView(&context, coverViewParam);

		int H_SMALL_NUMBER_RED[10];
		int H_SMALL_NUMBER_BLUE[10];
		int H_SMALL_NUMBER_GREEN[10];
		int H_SMALL_NUMBER_YELLOW[10];
		int H_SMALL_NUMBER_CYAN[10];
		int H_SMALL_NUMBER_MAGENTA[10];

		int fastSlowY = -130;
		int fastSlowYMove = 10;

		Cordinate fastSlowCordinate[4];
		DrawableInitParam fastSlowParams[4];
		Image I_Fast[4];
		Image I_Slow[4];
		int fast_count = 0;
		int slow_count = 0;

		int fast_background_color;
		int slow_background_color;
		int* fast_count_color = nullptr;
		int* slow_count_color = nullptr;

		std::function<void(void)> fastSlowAppear;


		BOOL isOverSongPlayTiming = false;

		bool isPlayMovie;
		int MovieGraphHandle = 0;
		LONGLONG movieFrameTime = 0;//����t�@�C����1�t���[���̎���(us)
		std::function<void(int)> updateSeekTime;
		BOOL isOverMoviePlayTiming = false;
		int movieHeight;
		int movieWidth;
		const int drawAreaX = 1280;
		const int drawAreaY = 720;
		double movieScale = 1;
		const double drawAreaAspecRatio = (double)drawAreaX / drawAreaY;


		int movieX1 = 0;
		int movieY1 = 0;
		int movieX2 = 0;
		int movieY2 = 0;

		std::function<void(void)> initVariableProcess;
		std::function<void(void)> soundHitSound;


		int FontHandleBpm;
		int FontHandleCommon;

		DrawableInitParam detailViewParam = DrawableInitParam(Cordinate(320, 500));
		DetailView detailView = DetailView(&context, detailViewParam);

		double draw_alpha_speed = 1;//�i�ʗpSpeed�\���t���O(���ꂪ0�ɂȂ�܂ł͊J�n���Ȃ�)

		int targetScore = 0;
		int targetScore2 = 0;
		bool isSkip = false;


		vector<unique_ptr<AuroraView>> aurora;
		bool update_aurora_color[4] = { true, true, true, true };


	};

}