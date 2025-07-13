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

	typedef struct HIGHSCORE {//ハイスコア構造体(SONG SELECTでしか使用しない)
		short score[10];//スコアを保存(1,2,3,4:通常 5,6,7,8:虹オプション)
		short sky_perfect[10];//空PERFECT数を保存
		short perfect[10];//PERFECT数を保存
		short good[10];//GOOD数を保存
		short miss[10];//MISS数を保存
		short rank[10];//ランクを保存
		short clear_state[10];//クリア状態を保存
		short play_count[10];//プレイ回数を保存
		short max_combo[10];//最大コンボ数を保存
		short min_miss[10];//最小ミス数を保存 (-1のときは表示しない)
	}HIGHSCORE;

	//レーダースキルの算出
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

	typedef struct FolderScore {//フォルダ全体のスコア構造体
		int AverageScore = 0;//平均点

		int clearState[9] = { 0,0,0,0,0,0,0,0,0 };
		int rank[8] = { 0,0,0,0,0,0,0,0 };


		int ClearType = CLEARTYPE_PERFECT;//フォルダ全体のクリアタイプ
		int folderRank = 0;//フォルダ全体のランク(平均)
	}FolderScore;



	class SongSelectScreen :public Screen
	{
	public:
		typedef struct SortSongListIndex {
			int index;
			int value;

			//SortSongListIndexに対する<>の演算子オーバーロード
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
		/// 初期化処理
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


		Asset asset;//使う画像セット
		//コンテキスト
		AppContext appContext = AppContext(NULL, option, &config);
		ActivityContext context = ActivityContext(&appContext, &asset);

		int list_number_base;//曲フォルダのソート配列のインデックスに対する番号 選択したときにSortを使ってlist_number変換する

		int list_number;//実際の曲フォルダ中の番号 選択したときにsong_numberへfolderの表を使って変換する 選曲画面に入る前で段位認定から戻るとき以外に再算出
		int song_number;//選択している曲番号
		int difficulty;//選択している難易度

		int H_BG;
		int H_COVER[10];//カバー画像
		int H_COVER_SKILL_TEST;//段位認定選択時のカバー
		int H_COVER_OPTION;
		int H_OPTION;
		int H_RESULT;
		int H_RESULT_OBJ;
		int H_COVER_MIDDLE;//中心カバー
		int H_DIFFICULTY[10];//難易度画像
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

		int H_RANK[8];//0:無し 1:F 2:E 3:D 4:C 5:B 6:A 7:S
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
		int SH_SONG;//曲のデモ用
		int SH_FOLDER_SELECT;
		int SH_BGM;//フォルダ選択時のBGM
		int SH_ALARM;//非解禁隠し曲出現時のアラーム
		int BGM_play;//BGM再生フラグ
		int SH_SHUTTER_SIGNAL;
		int SH_SHUTTER;
		int SH_OPTION_HITSOUND_PREVIEW;

		int FontHandle;

		double GAME_passed_time;
		double GAME_start_time;
		double LOOP_passed_time;
		double CounterRemainTime;//カウンターの値を1msずつ変動するための時間
		double time_cash;
		int i;
		int j;

		int CoverClosedTime;

		const int FLAG_OPENING_STATE = 0;
		const int FLAG_SELECT_STATE = 1;//自由に選択できる状態
		const int FLAG_CLOSING_STATE = 2;
		const int FLAG_END_FUNCTION_STATE = 3;//中央カバーが閉まった後の状態
		int activityState;


		int Column;//バナーの縦の列数(RowNumber) -1(実際に並ぶのはColumn+1個) 偶数にする
		wchar_t* title_buf[20 + 1] = { NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };//曲、難易度を変更するたびに曲タイトル先頭ポインタをこのバッファに格納する
		int season_banner_buf[20 + 1] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//季節
		int title_width[20 + 1] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };//曲名の幅
		int title_color[20 + 1];//曲名の色
		int title_shadow_color[20 + 1];//曲名の縁の色(白or黒)
		int H_TITLE_STR[20 + 1];//画像として保存された曲名の画像ハンドル

		std::function<int(int, int)> cycleIndex;

		TitleCycleIndex titleCycleIndex = TitleCycleIndex(0);
		TitleStrUpdateFlag titleStrUpdateFlag;

		int genre_width;//ジャンル名の幅
		int artist_width;//アーティスト名の幅
		int brightness;
		int score_digit[5] = { 0,0,0,0,0 };
		int digit[5] = { 0,0,0,0,0 };//汎用桁格納配列
		int bpm_digit[5] = { 0,0,0,0,0 };
		int bpm_min_digit[5] = { 0,0,0,0,0 };
		int bpm_max_digit[5] = { 0,0,0,0,0 };
		double show_bpm;//実際に表示するBPM
		int level_digit[3] = { 0,0,0 };

		HIGHSCORE* Highscore;
		HIGHSCORE* HighscoreRival;



		//段位認定用
		short STclear_state[NUMBER_OF_COURSES];//0:未プレイ 1:不合格 2:合格
		int STscore[NUMBER_OF_COURSES];//
		short STsky_perfect[NUMBER_OF_COURSES];//空パーフェクト数
		short STperfect[NUMBER_OF_COURSES];//
		short STgood[NUMBER_OF_COURSES];//
		short STmiss[NUMBER_OF_COURSES];//
		short STrank[NUMBER_OF_COURSES];//ランク(0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
		short STplay_count[NUMBER_OF_COURSES];//何回遊んだか
		short STmax_combo[NUMBER_OF_COURSES];
		short STmin_miss[NUMBER_OF_COURSES];
		wchar_t* Des1 = L"4曲連続クリアで合格です";
		wchar_t* Des2 = L"SPEEDは曲開始前にも←→で変更できます";
		int Des1_width = 0;
		int Des2_width = 0;

		int escape = 0;
		int retryAble = 0;//クイックリトライが可能かどうかのフラグ

		int OptionOpen = 0;//オプション開いているかどうかフラグ(0:開いていない 1:オプションを開いている)
		int OptionShowStart = 0;//オプションを表示するスタート
		int OptionShowEnd = 5;//オプションを表示する最後
		int OptionShowAmount = 6;//オプションを一度に並べる個数

		int ShowResultFlag = 0;//詳細リザルトを表示するか(0:しない 1:する)
		double option_draw_counter = 0;//オプションカバーのカウンタ(0~1)
		double result_draw_counter = 0;//リザルトカバーのカウンタ(0~1)
		int cache = 0;//見た目を簡単にする変数
		int cache2 = 0;
		int option_select = 0;//オプションで今選んでいる対象(0:speed 1:gauge 2:lane 3:color)

		int time_base_str = 1;//下の文字をスクロールする基準時間
		int select_rainbow = 0;//虹オプションを選んでいたら4、選んでいなかったら0になる


		//レーダー表示用変数
		double DRShowTotal = 0;
		double DRShowGlobal = 0;
		double DRShowLocal = 0;
		double DRShowChain = 0;
		double DRShowUnstability = 0;
		double DRShowStreak = 0;
		double DRShowColor = 0;

		double ShowLocalNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };
		double ShowColorNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };

		//(選択対象) フォルダ選択状態なら1 0なら曲選択へ 2は段位認定コース選択
		static const int SELECTING_FOLDER = 1;
		static const int SELECTING_SONG = 0;
		static const int SELECTING_COURSE = 2;

		int ScoreShowMode = 0;//プレーヤースコアを表示するかライバルスコアを表示するか(0:プレイヤー 1:ライバル)

		static const int OPERATION_INSTRUCTION_NUMBER = 5;
		wchar_t* ope_ins[OPERATION_INSTRUCTION_NUMBER] = {//operation instruct 操作説明 
			L"決定"
			,L"選択"
			,L"難易度変更"
			,L"リザルト表示"
			,L"オプション変更"
		};

		//それぞれの文字列長を格納
		int folder_name_width[NUMBER_OF_FOLDERS];
		int width_ope_ins[OPERATION_INSTRUCTION_NUMBER];
		int oi_counter;

		int H_OI_STR[OPERATION_INSTRUCTION_NUMBER];//操作説明文字画像作成

		DrawableInitParam optionListParam = DrawableInitParam(Cordinate(-320, 0));
		OptionListView optionListView = OptionListView(option, &context, optionListParam);

		double c_m_draw_counter = 0;
		double bn_draw_counter = 0;
		double song_play_counter = -3;//曲再生用フラグ -3:シャッターが上がるまでの曲を再生していない期間 1~0:曲読み込みまでの猶予時間 -1:曲読み込み中 -2:曲再生
		double roll_counter = 0;
		double button_draw_counter = 0;
		int openflag = 0;
		wchar_t filename[256];
		int widthCalcFlag = 1;//曲を変更したときに曲名表示の幅を計算するフラグ(1のときに計算が行われる)

		XYPAIR jacketSize;//ジャケットのサイズ
		XYPAIR jacketGaussSize;//ジャケットのサイズ
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

		//NOTEプレビュー画像読み込み
		wchar_t* ReadNameRGB[10] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"k",L"f",L"bright" };
		wchar_t strcash[128];
		wstring note_folder;
		wstring note_symbol_folder;

		//グラデーション画像の用意
		int screenHandle;
		double demo_vol = 1;//デモの音量

		wstring hit_sound_folder;
		double bgm_vol_ratio = 1;//BGMの音量

		std::function<void(void)> updateVolume;
		int PlayCountSum = 0;//総プレイカウント(初回アナウンス表示に使用)

		RESULT before_STRes;
		RadarSkillList radar_skill_list;
		SkillRadarResult skill_radar_result;

		FolderScore FolderScore[2][NUMBER_OF_FOLDERS][5];

		SAVEDATA saveData;

		vector<vector<vector<vector<vector<SortSongListIndex>>>>> SortList;//曲番号ソートリスト[][][][][]

		wchar_t* StrAnnounce = L"";//通常アナウンス文
		int AnnounceWidth;//アナウンス文字長

		wchar_t secret_str[5] = L"???";//隠し曲表示用文字列

		XY pos_cloud;//雲の座標

		int H_ANNOUNSE;//アナウンス用文字列画像作成

		int BGM_continue = 0;//BGMを選曲画面で流し続けるかどうかのフラグ(0:流さない 1:流す)
		int Announse_show_time_base;//アナウンス表示の基準時間

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