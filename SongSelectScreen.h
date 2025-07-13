#pragma once
#include "Screen.h"
#include "STRUCT_OP.h"
#include "STRUCT_LIST.h"
#include "STRUCT_Song.h"
#include "STRUCT_SECRET.h"
#include "STRUCT_ANDROID_CONTROLLER.h"
#include "STRUCT_IR_SETTING.h"
#include "Asset.h"
#include "AppContext.h"
#include "ActivityContext.h"
#include "TransValue.h"
#include "Drawable.h"
#include "OptionListView.h"
#include "STRUCT_SAVEDATA.h"
#include "STRUCT_RESULT.h"

namespace SongSelect {
	class TitleCycleIndex {
	public:
		TitleCycleIndex(int range) {
			indexRange = range;
		}

		int index = 0;
		int indexRange = 1;
		void plus() {
			index += 1;
			if (index >= indexRange)index -= indexRange;
		}
		void minus() {
			index -= 1;
			if (index < 0)index += indexRange;
		}
	};

	enum TitleStrUpdateFlag {
		TOP,
		ALL,
		BOTTOM,
	};

	typedef struct HIGHSCORE {//�n�C�X�R�A�\����(SONG SELECT�ł����g�p���Ȃ�)
		short score[10];//�X�R�A��ۑ�(1,2,3,4:�ʏ� 5,6,7,8:���I�v�V����)
		short sky_perfect[10];//��PERFECT����ۑ�
		short perfect[10];//PERFECT����ۑ�
		short good[10];//GOOD����ۑ�
		short miss[10];//MISS����ۑ�
		short rank[10];//�����N��ۑ�
		short clear_state[10];//�N���A��Ԃ�ۑ�
		short play_count[10];//�v���C�񐔂�ۑ�
		short max_combo[10];//�ő�R���{����ۑ�
		short min_miss[10];//�ŏ��~�X����ۑ� (-1�̂Ƃ��͕\�����Ȃ�)
	}HIGHSCORE;

	//���[�_�[�X�L���̎Z�o
	typedef struct RadarSkillValue {
		double score;
		int song_number;
		int difficulty;
		RadarSkillValue(const double score, const int song_number, const int difficulty) {
			RadarSkillValue::score = score;
			RadarSkillValue::song_number = song_number;
			RadarSkillValue::difficulty = difficulty;
		}
	};

	typedef struct SkillList {
		std::vector<RadarSkillValue> list = {};

		void add(double score, int song_number, int difficulty) {
			if (list.size() >= 10) {
				auto min = std::min_element(list.begin(), list.end(),
					[](const auto& a, const auto& b) {return a.score < b.score; });

				if (score > min->score) {
					(*min) = RadarSkillValue(score, song_number, difficulty);
				}
			}
			else {
				list.push_back(RadarSkillValue(score, song_number, difficulty));
			}
		}
		double average() {
			auto sum = 0;

			for (const auto& e : list) {
				sum += e.score;
			}

			auto ave = (double)sum / 10;
			return ave;
		}

	};

	typedef struct RadarSkillList {
		SkillList global = {};
		SkillList local = {};
		SkillList chain = {};
		SkillList unstability = {};
		SkillList streak = {};
		SkillList color = {};
		SkillList robustness = {};
	};

	typedef struct SkillRadarResult {
		int global = 0;
		int local = 0;
		int chain = 0;
		int unstability = 0;
		int streak = 0;
		int color = 0;
		int robustness = 0;
	};

	typedef struct FolderScore {//�t�H���_�S�̂̃X�R�A�\����
		int AverageScore = 0;//���ϓ_

		int clearState[9] = { 0,0,0,0,0,0,0,0,0 };
		int rank[8] = { 0,0,0,0,0,0,0,0 };


		int ClearType = CLEARTYPE_PERFECT;//�t�H���_�S�̂̃N���A�^�C�v
		int folderRank = 0;//�t�H���_�S�̂̃����N(����)
	}FolderScore;



	class SongSelectScreen :public Screen
	{
	public:
		typedef struct SortSongListIndex {
			int index;
			int value;

			//SortSongListIndex�ɑ΂���<>�̉��Z�q�I�[�o�[���[�h
			bool operator<(const SongSelect::SongSelectScreen::SortSongListIndex& right) const
			{
				return value < right.value;
			}




		}SortSongListIndex;



		SongSelectScreen(
			int* l_n,
			int* s_n,
			int* diff,
			Option* option, LIST* folder, Song* Music,
			int Button[3][4], int Button_Shutter, int* Key, char* Buf,
			int* debug, int* NumberOfSongs,
			int* result_count, int* result_rank_buf, SECRET* secret,
			ANDROID_CONTROLLER* AC, int* StageCount, SkillTestList* STList,
			Config config, IR_SETTING* ir, bool* isBackToTitle, int* SelectingTarget
		) :
			l_n(l_n),
			s_n(s_n),
			diff(diff),
			option(option),
			folder(folder),
			Music(Music),
			Button_Shutter(Button_Shutter),
			Key(Key),
			Buf(Buf),
			debug(debug),
			NumberOfSongs(NumberOfSongs),
			result_count(result_count),
			result_rank_buf(result_rank_buf),
			secret(secret),
			AC(AC),
			StageCount(StageCount),
			STList(STList),
			config(config),
			ir(ir),
			isBackToTitle(isBackToTitle),
			SelectingTarget(SelectingTarget)
		{
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 4; j++) {
					SongSelectScreen::Button[i][j] = Button[i][j];
				}
			}
		}

		/// <summary>
		/// ����������
		/// </summary>
		void init() override;

		void updateModel() override;

		void updateView() override;

		int* l_n;
		int* s_n;
		int* diff;
		Option* option;
		LIST* folder;
		Song* Music;
		int Button[3][4];
		int Button_Shutter;
		int* Key;
		char* Buf;
		int* debug;
		int* NumberOfSongs;
		int* result_count;
		int* result_rank_buf;
		SECRET* secret;
		ANDROID_CONTROLLER* AC;
		int* StageCount;
		SkillTestList* STList;
		Config config;
		IR_SETTING* ir;
		bool* isBackToTitle;
		int* SelectingTarget;


		Asset asset;//�g���摜�Z�b�g
		//�R���e�L�X�g
		AppContext appContext = AppContext(NULL, option, &config);
		ActivityContext context = ActivityContext(&appContext, &asset);

		int list_number_base;//�ȃt�H���_�̃\�[�g�z��̃C���f�b�N�X�ɑ΂���ԍ� �I�������Ƃ���Sort���g����list_number�ϊ�����

		int list_number;//���ۂ̋ȃt�H���_���̔ԍ� �I�������Ƃ���song_number��folder�̕\���g���ĕϊ����� �I�ȉ�ʂɓ���O�Œi�ʔF�肩��߂�Ƃ��ȊO�ɍĎZ�o
		int song_number;//�I�����Ă���Ȕԍ�
		int difficulty;//�I�����Ă����Փx

		int H_BG;
		int H_COVER[10];//�J�o�[�摜
		int H_COVER_SKILL_TEST;//�i�ʔF��I�����̃J�o�[
		int H_COVER_OPTION;
		int H_OPTION;
		int H_RESULT;
		int H_RESULT_OBJ;
		int H_COVER_MIDDLE;//���S�J�o�[
		int H_DIFFICULTY[10];//��Փx�摜
		int H_BANNER_UD;
		int H_BANNER_AREA;
		int H_BANNER_ARTIST;
		int H_BANNER_UD_SKIN;
		int H_BANNER_BACK;
		int H_BANNER_BACK_NE;
		int H_BANNER_FLAME;
		int H_BANNER_FLAME_SECRET;
		int H_BANNER_D_BUTTON[6];
		int H_BANNER_SELECT;
		int H_BANNER_SEASON[4];
		int H_OPTION_BANNER;
		int H_COVER_HIGH_SCORE;
		int H_COVER_RADAR;
		int H_COVER_RADAR_NAME_ROBUSTNESS;
		int H_COVER_RADAR_NAME_TOTAL;
		int H_COVER_POP;
		int H_COVER_SKILL_TEST_POP;
		int H_COVER_STATUS_STR;
		int H_COVER_STATUS;
		int H_SCORE_NUMBER[10];
		int H_JUDGE_NUMBER[10];

		int H_MIN_MISS_NUMBER[10];
		int H_MAX_COMBO_NUMBER[10];
		int H_PLAY_COUNT_NUMBER[10];

		int H_BPM_NUMBER_MIN[10];
		int H_BPM_NUMBER_MAX[10];
		int H_BPM_MINMAX_STR;
		int H_BPM_SLASH;

		int H_VERSION_NUMBER[10];
		int H_VERSION_DECIMAL;
		//int H_VERSION_STR;

		int H_MAX_CHORDS_NUMBER[10];
		int H_VER_MAX_CHORDS_STR;

		int H_PERCENT;
		int H_NO_PLAY;
		int H_FAILED;
		int H_CLEARED_EASY;
		int H_CLEARED_NORMAL;
		int H_CLEARED_HARD;
		int H_CLEARED_SUPER_HARD;
		int H_FULL_COMBO[6];
		int H_PFC;

		int H_SKILL_TEST_PASSED;
		int H_SKILL_TEST_FAILED;

		int H_RANK[8];//0:���� 1:F 2:E 3:D 4:C 5:B 6:A 7:S
		int H_BUTTON_R;
		int H_BUTTON_G;
		int H_BUTTON_B;
		int H_BUTTON_PRESS;
		int	H_R;
		int H_JACKET;
		int H_JACKET_GAUSS;
		int H_CLOUD;

		int H_NMC;

		int H_DARKNESS;
		int H_CURSOR;

		int H_RADER_NUMBER[10];

		int H_OPTION_NOTE_PREVIEW[2];
		int H_OPTION_NOTE_SYMBOL_PREVIEW[2];

		TransValue coverAlpha;
		std::function<void(void)> coverAlphaOpen;
		std::function<void(void)> coverAlphaClose;

		TransValue viewAlpha;
		bool isViewAlphaAnimationRun;
		std::function<void(void)> alphaAnimationOnStart;

		int SH_CLOSE;
		int SH_CLOSED;
		int SH_OPEN;
		int SH_NO;
		int SH_SONG_SELECT;
		int SH_DIFFICULTY_SELECT;
		int SH_SONG;//�Ȃ̃f���p
		int SH_FOLDER_SELECT;
		int SH_BGM;//�t�H���_�I������BGM
		int SH_ALARM;//����։B���ȏo�����̃A���[��
		int BGM_play;//BGM�Đ��t���O
		int SH_SHUTTER_SIGNAL;
		int SH_SHUTTER;
		int SH_OPTION_HITSOUND_PREVIEW;

		int FontHandle;

		double GAME_passed_time;
		double GAME_start_time;
		double LOOP_passed_time;
		double CounterRemainTime;//�J�E���^�[�̒l��1ms���ϓ����邽�߂̎���
		double time_cash;
		int i;
		int j;

		int CoverClosedTime;

		const int FLAG_OPENING_STATE = 0;
		const int FLAG_SELECT_STATE = 1;//���R�ɑI���ł�����
		const int FLAG_CLOSING_STATE = 2;
		const int FLAG_END_FUNCTION_STATE = 3;//�����J�o�[���܂�����̏��
		int activityState;


		int Column;//�o�i�[�̏c�̗�(RowNumber) -1(���ۂɕ��Ԃ̂�Column+1��) �����ɂ���
		wchar_t* title_buf[20 + 1] = { NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };//�ȁA��Փx��ύX���邽�тɋȃ^�C�g���擪�|�C���^�����̃o�b�t�@�Ɋi�[����
		int season_banner_buf[20 + 1] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//�G��
		int title_width[20 + 1] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };//�Ȗ��̕�
		int title_color[20 + 1];//�Ȗ��̐F
		int title_shadow_color[20 + 1];//�Ȗ��̉��̐F(��or��)
		int H_TITLE_STR[20 + 1];//�摜�Ƃ��ĕۑ����ꂽ�Ȗ��̉摜�n���h��

		std::function<int(int, int)> cycleIndex;

		TitleCycleIndex titleCycleIndex = TitleCycleIndex(0);
		TitleStrUpdateFlag titleStrUpdateFlag;

		int genre_width;//�W���������̕�
		int artist_width;//�A�[�e�B�X�g���̕�
		int brightness;
		int score_digit[5] = { 0,0,0,0,0 };
		int digit[5] = { 0,0,0,0,0 };//�ėp���i�[�z��
		int bpm_digit[5] = { 0,0,0,0,0 };
		int bpm_min_digit[5] = { 0,0,0,0,0 };
		int bpm_max_digit[5] = { 0,0,0,0,0 };
		double show_bpm;//���ۂɕ\������BPM
		int level_digit[3] = { 0,0,0 };

		HIGHSCORE* Highscore;
		HIGHSCORE* HighscoreRival;



		//�i�ʔF��p
		short STclear_state[NUMBER_OF_COURSES];//0:���v���C 1:�s���i 2:���i
		int STscore[NUMBER_OF_COURSES];//
		short STsky_perfect[NUMBER_OF_COURSES];//��p�[�t�F�N�g��
		short STperfect[NUMBER_OF_COURSES];//
		short STgood[NUMBER_OF_COURSES];//
		short STmiss[NUMBER_OF_COURSES];//
		short STrank[NUMBER_OF_COURSES];//�����N(0:���v���C 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
		short STplay_count[NUMBER_OF_COURSES];//����V�񂾂�
		short STmax_combo[NUMBER_OF_COURSES];
		short STmin_miss[NUMBER_OF_COURSES];
		wchar_t* Des1 = L"4�ȘA���N���A�ō��i�ł�";
		wchar_t* Des2 = L"SPEED�͋ȊJ�n�O�ɂ������ŕύX�ł��܂�";
		int Des1_width = 0;
		int Des2_width = 0;

		int escape = 0;
		int retryAble = 0;//�N�C�b�N���g���C���\���ǂ����̃t���O

		int OptionOpen = 0;//�I�v�V�����J���Ă��邩�ǂ����t���O(0:�J���Ă��Ȃ� 1:�I�v�V�������J���Ă���)
		int OptionShowStart = 0;//�I�v�V������\������X�^�[�g
		int OptionShowEnd = 5;//�I�v�V������\������Ō�
		int OptionShowAmount = 6;//�I�v�V��������x�ɕ��ׂ��

		int ShowResultFlag = 0;//�ڍ׃��U���g��\�����邩(0:���Ȃ� 1:����)
		double option_draw_counter = 0;//�I�v�V�����J�o�[�̃J�E���^(0~1)
		double result_draw_counter = 0;//���U���g�J�o�[�̃J�E���^(0~1)
		int cache = 0;//�����ڂ��ȒP�ɂ���ϐ�
		int cache2 = 0;
		int option_select = 0;//�I�v�V�����ō��I��ł���Ώ�(0:speed 1:gauge 2:lane 3:color)

		int time_base_str = 1;//���̕������X�N���[����������
		int select_rainbow = 0;//���I�v�V������I��ł�����4�A�I��ł��Ȃ�������0�ɂȂ�


		//���[�_�[�\���p�ϐ�
		double DRShowTotal = 0;
		double DRShowGlobal = 0;
		double DRShowLocal = 0;
		double DRShowChain = 0;
		double DRShowUnstability = 0;
		double DRShowStreak = 0;
		double DRShowColor = 0;

		double ShowLocalNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };
		double ShowColorNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };

		//(�I��Ώ�) �t�H���_�I����ԂȂ�1 0�Ȃ�ȑI���� 2�͒i�ʔF��R�[�X�I��
		static const int SELECTING_FOLDER = 1;
		static const int SELECTING_SONG = 0;
		static const int SELECTING_COURSE = 2;

		int ScoreShowMode = 0;//�v���[���[�X�R�A��\�����邩���C�o���X�R�A��\�����邩(0:�v���C���[ 1:���C�o��)

		static const int OPERATION_INSTRUCTION_NUMBER = 5;
		wchar_t* ope_ins[OPERATION_INSTRUCTION_NUMBER] = {//operation instruct ������� 
			L"����"
			,L"�I��"
			,L"��Փx�ύX"
			,L"���U���g�\��"
			,L"�I�v�V�����ύX"
		};

		//���ꂼ��̕����񒷂��i�[
		int folder_name_width[NUMBER_OF_FOLDERS];
		int width_ope_ins[OPERATION_INSTRUCTION_NUMBER];
		int oi_counter;

		int H_OI_STR[OPERATION_INSTRUCTION_NUMBER];//������������摜�쐬

		DrawableInitParam optionListParam = DrawableInitParam(Cordinate(-320, 0));
		OptionListView optionListView = OptionListView(option, &context, optionListParam);

		double c_m_draw_counter = 0;
		double bn_draw_counter = 0;
		double song_play_counter = -3;//�ȍĐ��p�t���O -3:�V���b�^�[���オ��܂ł̋Ȃ��Đ����Ă��Ȃ����� 1~0:�ȓǂݍ��݂܂ł̗P�\���� -1:�ȓǂݍ��ݒ� -2:�ȍĐ�
		double roll_counter = 0;
		double button_draw_counter = 0;
		int openflag = 0;
		wchar_t filename[256];
		int widthCalcFlag = 1;//�Ȃ�ύX�����Ƃ��ɋȖ��\���̕����v�Z����t���O(1�̂Ƃ��Ɍv�Z���s����)

		XYPAIR jacketSize;//�W���P�b�g�̃T�C�Y
		XYPAIR jacketGaussSize;//�W���P�b�g�̃T�C�Y
		XYPAIR center;

		XYPAIR jacketArea;

		double jacketScale = 1;
		double jacketGaussScale = 1;
		double jacket_show_counter = 0;
		double jacket_alpha = 0;

		RESULT res;
		RESULT before;

		FILE* fp = 0;
		errno_t error = 0;
		int firstplay = 0;

		wstring themeStr1;
		wstring themeStr2;

		//NOTE�v���r���[�摜�ǂݍ���
		wchar_t* ReadNameRGB[10] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"k",L"f",L"bright" };
		wchar_t strcash[128];
		wstring note_folder;
		wstring note_symbol_folder;

		//�O���f�[�V�����摜�̗p��
		int screenHandle;
		double demo_vol = 1;//�f���̉���

		wstring hit_sound_folder;
		double bgm_vol_ratio = 1;//BGM�̉���

		std::function<void(void)> updateVolume;
		int PlayCountSum = 0;//���v���C�J�E���g(����A�i�E���X�\���Ɏg�p)

		RESULT before_STRes;
		RadarSkillList radar_skill_list;
		SkillRadarResult skill_radar_result;

		FolderScore FolderScore[2][NUMBER_OF_FOLDERS][5];

		SAVEDATA saveData;

		vector<vector<vector<vector<vector<SortSongListIndex>>>>> SortList;//�Ȕԍ��\�[�g���X�g[][][][][]

		wchar_t* StrAnnounce = L"";//�ʏ�A�i�E���X��
		int AnnounceWidth;//�A�i�E���X������

		wchar_t secret_str[5] = L"???";//�B���ȕ\���p������

		XY pos_cloud;//�_�̍��W

		int H_ANNOUNSE;//�A�i�E���X�p������摜�쐬

		int BGM_continue = 0;//BGM��I�ȉ�ʂŗ��������邩�ǂ����̃t���O(0:�����Ȃ� 1:����)
		int Announse_show_time_base;//�A�i�E���X�\���̊����

		int CRTBuf;


	private:
		wchar_t* announce_str(int StageCount, int PlayCount);

		void DrawRadarLine(double angle);
		int GetCylinderColor(double val, double range, unsigned int R, unsigned  int G, unsigned int B);
		int GetRainbowColorForRadar(int val, int th);
		void DrawHexagon(short v1, short v2, short v3, short v4, short v5, short v6);

		void OptionValueChange(Option* Option, int option_select, int add);
		void DrawOptionSentence(Option* Option, OptionItem::Name option_select, Config config, int FontHandle);

		int clearStateConverter(int clearState);




	};

}