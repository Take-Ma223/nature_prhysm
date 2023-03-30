#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_LIST.h"
#include"GAME.h"
#include"SONG_SELECT.h"
#include"SHOW_RESULT.h"
#include"number_digit.h"
#include"Get_Key_State.h"
#include"number_ring.h"
#include"show_something.h"
#include"secret_LOAD.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"CONSTANT_VALUE.h"
#include <time.h>
#include"LoadSkillTestNpsPathList.h"
#include"STRUCT_RESULT.h"
#include"GetNowCount_d.h"
#include"OptionStateSaveLoad.h"
#include"ShowFps.h"
#include<vector>
#include <algorithm>
#include"ScreenShot.h"
#include<string>
#include "SaveDataSaveLoad.h"
#include"STRUCT_IR_SETTING.h"
#include "IR_process.h"

using namespace std;

void SONG_SELECT(int *l_n, 
	int *s_n, 
	int *diff, 
	Option *option,
	LIST *folder, 
	Song *Music, 
	int Button[3][4], int Button_Shutter, int *Key, char *Buf, 
	int *debug, int *NumberOfSongs,
	int *result_count,int *result_rank_buf,SECRET *secret, ANDROID_CONTROLLER *AC,int *StageCount, SkillTestList *STList,
	Config config, IR_SETTING* ir) {



	int list_number_base = *l_n;//曲フォルダのソート配列のインデックスに対する番号 選択したときにSortを使ってlist_number変換する

	int list_number = *l_n;//実際の曲フォルダ中の番号 選択したときにsong_numberへfolderの表を使って変換する 選曲画面に入る前で段位認定から戻るとき以外に再算出
	int song_number = *s_n;//選択している曲番号
	int difficulty = *diff;//選択している難易度


	int H_BG;
	int H_COVER[10];//カバー画像
	int H_COVER_SKILL_TEST;//段位認定選択時のカバー
	int H_COVER_OPTION;
	int H_OPTION;
	int H_RESULT;
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
	int H_COVER_HIGH_SCORE;
	int H_COVER_POP;
	int H_COVER_SKILL_TEST_POP;
	int H_CPVER_RESULT_STR;
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
	int	H_R_IN;
	int H_R_OUT;
	int H_JACKET;
	int H_JACKET_GAUSS;
	int H_CLOUD;

	int H_NMC;

	int H_DARKNESS;
	int H_CURSOR;

	int H_RADER_NUMBER[10];

	int H_OPTION_NOTE_PREVIEW[2];

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
	int BGM_play = 0;//BGM再生フラグ
	int SH_SHUTTER_SIGNAL;
	int SH_SHUTTER;
	int SH_OPTION_HITSOUND_PREVIEW;

	int FontHandle = CreateFontToHandle(L"メイリオ", 28, 9, DX_FONTTYPE_ANTIALIASING_EDGE);//フォントハンドル
	double GAME_passed_time = 0;
	double GAME_start_time = 0;
	double LOOP_passed_time = 1;
	double CounterRemainTime = 0;//カウンターの値を1msずつ変動するための時間
	double time_cash = 0;
	int i = 0;
	int j = 0;

	int CoverClosedTime = 0;//カバーが閉まったタイミング

	const int FLAG_OPENING_STATE = 0;
	const int FLAG_SELECT_STATE = 1;//自由に選択できる状態
	const int FLAG_CLOSING_STATE = 2;
	const int FLAG_END_FUNCTION_STATE = 3;
	int flag = FLAG_OPENING_STATE;




	int Column = config.SongSelectRowNumber - 1;//バナーの縦の列数(RowNumber) -1(実際に並ぶのはColumn+1個) 偶数にする
	wchar_t *title_buf[20 + 1] = {NULL,NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, };//曲、難易度を変更するたびに曲タイトル先頭ポインタをこのバッファに格納する
	int season_banner_buf[20 + 1] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//季節
	int title_width[20 + 1] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };//曲名の幅
	int title_color[20 + 1];//曲名の色
	int title_shadow_color[20 + 1];//曲名の縁の色(白or黒)
	int H_TITLE_STR[20 + 1];//画像として保存された曲名の画像ハンドル
	for (i = 0; i <= 20; i++) {//初期化
		title_color[i] = GetColor(255, 255, 255);
		title_shadow_color[i] = GetColor(0, 0, 0);
		H_TITLE_STR[i] = MakeScreen(640, 48, TRUE);
	}

	int genre_width = 1;//ジャンル名の幅
	int artist_width = 1;//アーティスト名の幅
	int brightness = 0;
	int score_digit[5] = { 0,0,0,0,0 };
	int digit[5] = { 0,0,0,0,0 };//汎用桁格納配列
	int bpm_digit[5] = { 0,0,0,0,0 };
	int bpm_min_digit[5] = { 0,0,0,0,0 };
	int bpm_max_digit[5] = { 0,0,0,0,0 };


	double show_bpm = 0;//実際に表示するBPM
	int level_digit[3] = { 0,0,0 };

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

	HIGHSCORE* Highscore;
	HIGHSCORE* HighscoreRival;
	Highscore = (HIGHSCORE*)calloc(SONG_MAX_NUMBER, sizeof(HIGHSCORE));
	HighscoreRival = (HIGHSCORE*)calloc(SONG_MAX_NUMBER, sizeof(HIGHSCORE));



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
	wchar_t Des1[] = L"4曲連続クリアで合格です";
	wchar_t Des2[] = L"SPEEDは曲開始前にも←→で変更できます";
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
	double DRShowGlobal = 0;
	double DRShowLocal = 0;
	double DRShowChain = 0;
	double DRShowUnstability = 0;
	double DRShowStreak = 0;
	double DRShowColor = 0;

	double ShowLocalNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };
	double ShowColorNotesGraph[9] = { 0,0,0,0,0,0,0,0,0 };

	const int SELECTING_FOLDER = 1;
	const int SELECTING_SONG = 0;
	const int SELECTING_COURSE = 2;
	int static SelectingTarget = SELECTING_FOLDER;//(選択対象) フォルダ選択状態なら1 0なら曲選択へ 2は段位認定コース選択


	int ScoreShowMode = 0;//プレーヤースコアを表示するかライバルスコアを表示するか(0:プレイヤー 1:ライバル)

	const int OPERATION_INSTRUCTION_NUMBER = 7;
	wchar_t *ope_ins[OPERATION_INSTRUCTION_NUMBER] = {//operation instruct 操作説明 
		L"決定"
		,L"選択"
		,L"難易度変更"
		,L"リザルト表示"
		,L"オプション変更"
		,L"F1:DEBUG MODE"
		,L"F2:ランキング表示"
	};


	//それぞれの文字列長を格納
	int folder_name_width[NUMBER_OF_FOLDERS];

	//int *width_option_name = new int[Option->OPTION_NUM];//オプション名表示
	int *width_sent_speed = new int[option->SPEED_NUM];
	int *width_sent_gauge = new int[option->GAUGE_NUM];
	int *width_sent_lane = new int[option->LANE_NUM];
	int *width_sent_color = new int[option->COLOR_NUM];
	//int width_sent_note, width_sent_hitsound;

	int width_ope_ins[7];
	int oi_counter = 0;
	SetFontSize(28);

	option->H_SENT= MakeScreen(640, 48, TRUE);//オプション説明文の画像ハンドル作成
	DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);


	int H_OI_STR[OPERATION_INSTRUCTION_NUMBER];//操作説明文字画像作成
	for (i = 0; i < OPERATION_INSTRUCTION_NUMBER; i++) {
		H_OI_STR[i] = MakeScreen(640, 100, TRUE);
		SetDrawScreen(H_OI_STR[i]);
		width_ope_ins[i] = GetDrawStringWidth(ope_ins[i], wcslen(ope_ins[i]));
		ShowExtendedStrFitToHandle(370, 1, ope_ins[i], width_ope_ins[i], 532, config, FontHandle);
	}
	SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す


	Des1_width = GetDrawStringWidth(Des1, wcslen(Des1));
	Des2_width = GetDrawStringWidth(Des2, wcslen(Des2));


	//ShowExtendedStrFitToHandle(int x, int y, char *str, int str_width, int area_width, int FontHandle, int color, int shadow_color);

	for (i = 0; i <= SONG_MAX_NUMBER - 1; i++) {//初期化
		for (j = 0; j <= 9; j++) {
			Highscore[i].score[j] = 0;
			Highscore[i].rank[j] = 0;
			Highscore[i].clear_state[j] = CLEARTYPE_NO_PLAY;
			Highscore[i].play_count[j] = 0;
			Highscore[i].sky_perfect[j] = 0;//SKY_PERFECT数
			Highscore[i].perfect[j] = 0;//PERFECT数
			Highscore[i].good[j] = 0;//GOOD数
			Highscore[i].miss[j] = 0;//MISS数
			Highscore[i].max_combo[j] = 0;//最大コンボ数
			Highscore[i].min_miss[j] = -1;//最小ミス数

			HighscoreRival[i].score[j] = 0;
			HighscoreRival[i].rank[j] = 0;
			HighscoreRival[i].clear_state[j] = CLEARTYPE_NO_PLAY;
			HighscoreRival[i].play_count[j] = 0;
			HighscoreRival[i].sky_perfect[j] = 0;//SKY_PERFECT数
			HighscoreRival[i].perfect[j] = 0;//PERFECT数
			HighscoreRival[i].good[j] = 0;//GOOD数
			HighscoreRival[i].miss[j] = 0;//MISS数
			HighscoreRival[i].max_combo[j] = 0;//最大コンボ数
			HighscoreRival[i].min_miss[j] = -1;//最小ミス数
		}
	}

	for (i = 0; i <= NUMBER_OF_COURSES - 1; i++) {
		STclear_state[i] = 0;//0:未プレイ 1:不合格 2:合格
		STscore[i] = 0;//
		STsky_perfect[i] = 0;//空パーフェクト数
		STperfect[i] = 0;//
		STgood[i] = 0;//
		STmiss[i] = 0;//
		STrank[i] = 0;//ランク(0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
		STplay_count[i] = 0;//何回遊んだか
		STmax_combo[i] = 0;
		STmin_miss[i] = -1;
	}

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
	center.x = 640;
	center.y = 360;
	XYPAIR jacketArea;
	jacketArea.x = 640;
	jacketArea.y = 528;
	double jacketScale = 1;
	double jacketGaussScale = 1;
	double jacket_show_counter = 0;
	double jacket_alpha = 0;

	RESULT res;
	RESULT before;

	FILE *fp = 0;
	errno_t error = 0;
	int firstplay = 0;

	for (i = 0; i <= 255; i++) {//初期化
		filename[i] = '\0';
	}
	
	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);

	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
	H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());//難易度によってカバー変更
	H_DIFFICULTY[1] = LoadGraph(L"img/sunny.png");
	
	H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
	H_DIFFICULTY[2] = LoadGraph(L"img/cloudy.png");

	H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
	H_DIFFICULTY[3] = LoadGraph(L"img/rainy.png");

	H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
	H_DIFFICULTY[4] = LoadGraph(L"img/thunder.png");

	H_DIFFICULTY[5] = LoadGraph(L"img/snowy.png");
	H_DIFFICULTY[6] = LoadGraph(L"img/snowstorm.png");

	H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
	H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());

	H_OPTION = LoadGraph(L"img/cover_option_str.png");
	H_RESULT = LoadGraph(L"img/cover_result_obj.png");

	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
	H_BANNER_UD = LoadGraph(L"img/banner_ud.png");
	H_BANNER_AREA = LoadGraph(L"img/banner_Area.png");
	H_BANNER_ARTIST = LoadGraph(L"img/banner_Artist.png");
	H_BANNER_UD_SKIN = LoadGraph(L"img/banner_ud_skin.png");
	H_BANNER_BACK = LoadGraph(L"img/banner_back.png");
	H_BANNER_BACK_NE = LoadGraph(L"img/banner_back_ne.png");

	H_BANNER_FLAME = LoadGraph(L"img/banner_flame.png");
	H_BANNER_FLAME_SECRET = LoadGraph(L"img/banner_flame_secret.png");
	LoadDivGraph(L"img/banner_d_button.png", 6, 1, 6, 108, 80, H_BANNER_D_BUTTON);
	H_BANNER_SELECT = LoadGraph(L"img/banner_select.png");
	H_BANNER_SEASON[0] = LoadGraph(L"img/banner_spring.png");
	H_BANNER_SEASON[1] = LoadGraph(L"img/banner_summer.png");
	H_BANNER_SEASON[2] = LoadGraph(L"img/banner_autumn.png");
	H_BANNER_SEASON[3] = LoadGraph(L"img/banner_winter.png");

	H_COVER_HIGH_SCORE = LoadGraph(L"img/cover_high_score.png");
	H_COVER_POP = LoadGraph(L"img/cover_pop.png");
	H_COVER_SKILL_TEST_POP = LoadGraph(L"img/cover_skill_test_pop.png");

	H_CPVER_RESULT_STR = LoadGraph(L"img/cover_result_str.png");;

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

	H_SKILL_TEST_PASSED = LoadGraph(L"img/合格.png");
	H_SKILL_TEST_FAILED = LoadGraph(L"img/不合格.png");
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

	H_R_IN = LoadGraph(L"img/R_inside.png");
	H_R_OUT = LoadGraph(L"img/R_outside.png");


	H_BPM_MINMAX_STR = LoadGraph(L"img/BPM_minmax_str.png");
	H_VER_MAX_CHORDS_STR = LoadGraph(L"img/VER_MAX_CHORDS_str.png");

	H_BPM_SLASH = LoadGraph(L"img/slash.png");

	H_DARKNESS = LoadGraph(L"img/BlackWall.png");

	H_CURSOR = LoadGraph(L"img/cursor.png");

	//NOTEプレビュー画像読み込み
	wchar_t* ReadNameRGB[11] = { L"r",L"g",L"b",L"y",L"c",L"m",L"w",L"d",L"f",L"bright",L"note_Long_hit_b" };
	wchar_t strcash[128];
	sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[1]);
	H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
	sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[8]);
	H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);

	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");
	SH_OPEN = LoadSoundMem(L"sound/open.wav");
	SH_NO = LoadSoundMem(L"sound/no.wav");
	SH_SONG_SELECT = LoadSoundMem(L"sound/song_select.wav");
	SH_DIFFICULTY_SELECT = LoadSoundMem(L"sound/difficulty_select.wav");
	SH_SONG = LoadSoundMem(Music[song_number].wavpath[difficulty]);//曲デモ用音声
	double demo_vol = 1;//デモの音量
	SH_FOLDER_SELECT = LoadSoundMem(L"sound/folder_select.wav");
	SH_ALARM = LoadSoundMem(L"sound/alarm.wav");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");

	sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
	SH_OPTION_HITSOUND_PREVIEW = LoadSoundMem(strcash, 1);//HIT SOUNDプレビュー音声読み込み
	
	double bgm_vol = 1;//BGMの音量


	H_JACKET = LoadGraph(Music[song_number].jacketpath[difficulty]);//ジャケットの読み込み
	H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath[difficulty]);//ジャケットの読み込み

	int PlayCountSum = 0;//総プレイカウント(初回アナウンス表示に使用)



	RESULT before_STRes;
	//段位認定リザルト読み込み
	for (i = 0; i <= NUMBER_OF_COURSES - 2; i++) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}

		//読み込むファイル名を決定
		sprintfDx(filename, L"save_data/skill_test/score/%s.dat", STList->title[i]);

		error = _wfopen_s(&fp, filename, L"rb");
		if (error != 0) {//スコアファイルが見つからなかったら初プレイ
			firstplay = 1;
		}
		else {
			firstplay = 0;//スコアファイルがある
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
			STsky_perfect[i] = before_STRes.sky_perfect;//SKY_PERFECT数
			STperfect[i] = before_STRes.perfect;//PERFECT数
			STgood[i] = before_STRes.good;//GOOD数
			STmiss[i] = before_STRes.miss;//MISS数
			STmax_combo[i] = before_STRes.max_combo;//最大コンボ
			STmin_miss[i] = before_STRes.min_miss;//最小ミス

			PlayCountSum += before.play_count;//プレイヤーデータの総プレイカウント
			fclose(fp);
		}
	}


	//スコア読み込み
	for (int side = 0; side <= 1; side++) {//プレイヤースコア(side=0),おまけスコア(side=1)の読み込み
		for (i = 0; i <= *NumberOfSongs - 1; i++) {//曲番号
			for (j = 0; j <= 9; j++) {//難易度
				if (ProcessMessage() != 0) {
					DxLib_End();
					exit(0);
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
				if (error != 0) {//スコアファイルが見つからなかったら初プレイ
					firstplay = 1;
				}
				else {
					firstplay = 0;//スコアファイルがある
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
					if (ScoreData_Authentication(before, i, j, Music) == -1) {//違う曲、難易度の譜面データだったら無視する(簡単な不正防止)
						firstplay = 1;
						fclose(fp);
					}
				}
				if (j >= 5 && firstplay == 0) {//j>=5は虹オプションのとき
					if (ScoreData_Authentication(before, i, j, Music) == -1) {//違う曲、難易度の譜面データだったら無視する(簡単な不正防止)
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
						Highscore[i].sky_perfect[j] = before.sky_perfect;//SKY_PERFECT数
						Highscore[i].perfect[j] = before.perfect;//PERFECT数
						Highscore[i].good[j] = before.good;//GOOD数
						Highscore[i].miss[j] = before.miss;//MISS数
						if (before.save_data_version >= RESULT_DATA_VERSION_MAX_COMBO) {
							Highscore[i].max_combo[j] = before.max_combo;//最大コンボ数記録バージョン以降のセーブデータなら読み込む
						}
						if (before.save_data_version >= RESULT_DATA_VERSION_MIN_MISS) {
							Highscore[i].min_miss[j] = before.min_miss;//最小ミス記録バージョン以降のセーブデータなら読み込む
							if (before.clear == CLEARTYPE_PLAY && Highscore[i].min_miss[j] == 0) {//MISS 0ならPLAY状態でもフルコンボを表示するようにする
								Highscore[i].clear_state[j] = CLEARTYPE_FULL_COMBO;
							}
							if (before.clear == CLEARTYPE_PLAY && Highscore[i].min_miss[j] == 0 && Highscore[i].good[j] == 0) {//GOOD 0ならPLAY状態でもPERFECTを表示するようにする
								Highscore[i].clear_state[j] = CLEARTYPE_PERFECT;
							}
						}
						else {//それ以前のセーブデータ
							if (Highscore[i].clear_state[j] == CLEARTYPE_FULL_COMBO || Highscore[i].clear_state[j] == CLEARTYPE_PERFECT) {//フルコン済みなら
								Highscore[i].min_miss[j] = 0;
							}
							else if (Highscore[i].clear_state[j] == CLEARTYPE_EASY_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_HARD_CLEARED ||
								Highscore[i].clear_state[j] == CLEARTYPE_SUPER_HARD_CLEARED) {//クリア済み
								Highscore[i].min_miss[j] = Highscore[i].miss[j];
							}
							else {
								Highscore[i].min_miss[j] = -1;
							}
						}

						PlayCountSum += before.play_count;//プレイヤーデータの総プレイカウント
						fclose(fp);
					}
					else if (side == 1) {
						HighscoreRival[i].score[j] = before.score;
						HighscoreRival[i].rank[j] = before.rank;
						HighscoreRival[i].clear_state[j] = before.clear;
						HighscoreRival[i].play_count[j] = before.play_count;
						HighscoreRival[i].sky_perfect[j] = before.sky_perfect;//SKY_PERFECT数
						HighscoreRival[i].perfect[j] = before.perfect;//PERFECT数
						HighscoreRival[i].good[j] = before.good;//GOOD数
						HighscoreRival[i].miss[j] = before.miss;//MISS数
						if (before.save_data_version >= RESULT_DATA_VERSION_MAX_COMBO) {
							HighscoreRival[i].max_combo[j] = before.max_combo;//最大コンボ数記録バージョン以降のセーブデータなら読み込む
						}

						if (before.save_data_version >= RESULT_DATA_VERSION_MIN_MISS) {
							HighscoreRival[i].min_miss[j] = before.min_miss;//最小ミス記録バージョン以降のセーブデータなら読み込む
							if (HighscoreRival[i].min_miss[j] == 0) {//MISS 0ならPLAY状態でもフルコンボを表示するようにする
								HighscoreRival[i].clear_state[j] = CLEARTYPE_FULL_COMBO;
							}
							if (HighscoreRival[i].min_miss[j] == 0 && HighscoreRival[i].good[j] == 0) {//GOOD 0ならPLAY状態でもPERFECTを表示するようにする
								HighscoreRival[i].clear_state[j] = CLEARTYPE_PERFECT;
							}
						}
						else {//それ以前のセーブデータ
							if (HighscoreRival[i].clear_state[j] == CLEARTYPE_FULL_COMBO || HighscoreRival[i].clear_state[j] == CLEARTYPE_PERFECT) {//フルコン済みなら
								HighscoreRival[i].min_miss[j] = 0;
							}
							else if (HighscoreRival[i].clear_state[j] == CLEARTYPE_EASY_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_HARD_CLEARED ||
								HighscoreRival[i].clear_state[j] == CLEARTYPE_SUPER_HARD_CLEARED) {//クリア済み
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

	typedef struct FolderScore {//フォルダ全体のスコア構造体
		int AverageScore = 0;//平均点
		int Global = 0;
		int Local = 0;
		int Chain = 0;
		int Unstability = 0;
		int Streak = 0;
		int Color = 0;

		int clearState[9] = { 0,0,0,0,0,0,0,0,0 };
		int rank[8] = { 0,0,0,0,0,0,0,0 };


		int ClearType = CLEARTYPE_PERFECT;//フォルダ全体のクリアタイプ
		int folderRank = 0;//フォルダ全体のランク(平均)
	}FolderScore;
		
	FolderScore FolderScore[2][NUMBER_OF_FOLDERS][5];

	typedef struct FolderScoreRaderBuf {
		int Global = 0;
		int Local = 0;
		int Chain = 0;
		int Unstability = 0;
		int Streak = 0;
		int Color = 0;
	}FolderScoreRaderBuf;



	//フォルダ毎の成績を算出
	for (int rainbow = 0; rainbow < 2; rainbow++) {
		int count = 0;

		for (i = 0; i < NUMBER_OF_FOLDERS; i++) {
			if (folder->FolderKind[i] == FOLDER_KIND_NORMAL) {//難易度毎に算出
				for (int diff = 1; diff < 5; diff++) {//難易度
					FolderScoreRaderBuf FolderScoreRaderBuf;
					count = 0;
					for (j = 0; j < folder->folder_c[i]; j++) {
						if (folder->folder[i][j].kind == 0 && Music[folder->folder[i][j].song_number].exist[diff] == 1) {//「フォルダ選択に戻る」ではなく存在する譜面
							FolderScore[rainbow][i][diff].AverageScore += Highscore[folder->folder[i][j].song_number].score[diff+ rainbow*4];
							double ScoreRate = (double)Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4] / 10000;


							FolderScoreRaderBuf.Global      += Music[folder->folder[i][j].song_number].global[rainbow][diff];
							FolderScoreRaderBuf.Local       += Music[folder->folder[i][j].song_number].local[rainbow][diff];
							FolderScoreRaderBuf.Chain       += Music[folder->folder[i][j].song_number].chain[rainbow][diff];
							FolderScoreRaderBuf.Unstability += Music[folder->folder[i][j].song_number].unstability[rainbow][diff];
							FolderScoreRaderBuf.Streak      += Music[folder->folder[i][j].song_number].longNote[rainbow][diff];
							FolderScoreRaderBuf.Color       += Music[folder->folder[i][j].song_number].color[rainbow][diff];

							FolderScore[rainbow][i][diff].Global      += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].global[rainbow][diff];
							FolderScore[rainbow][i][diff].Local       += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].local[rainbow][diff];
							FolderScore[rainbow][i][diff].Chain       += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].chain[rainbow][diff];
							FolderScore[rainbow][i][diff].Unstability += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].unstability[rainbow][diff];
							FolderScore[rainbow][i][diff].Streak      += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].longNote[rainbow][diff];
							FolderScore[rainbow][i][diff].Color       += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].color[rainbow][diff];


							FolderScore[rainbow][i][diff].clearState[clearStateConverter(Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4])]++;
							FolderScore[rainbow][i][diff].rank[Highscore[folder->folder[i][j].song_number].rank[diff + rainbow * 4]]++;

							if (FolderScore[rainbow][i][diff].ClearType > Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4]) {//フォルダにある譜面の中で一番低いクリア状態をそのフォルダのクリア状態にする
								FolderScore[rainbow][i][diff].ClearType = Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4];
							}

							count++;

						}
					}
					if (count != 0)FolderScore[rainbow][i][diff].AverageScore = (double)FolderScore[rainbow][i][diff].AverageScore / count;

					if (FolderScoreRaderBuf.Global != 0)     FolderScore[rainbow][i][diff].Global = FolderScore[rainbow][i][diff].Global / FolderScoreRaderBuf.Global;
					if (FolderScoreRaderBuf.Local != 0)      FolderScore[rainbow][i][diff].Local       =  FolderScore[rainbow][i][diff].Local/FolderScoreRaderBuf.Local;
					if (FolderScoreRaderBuf.Chain != 0)      FolderScore[rainbow][i][diff].Chain       =  FolderScore[rainbow][i][diff].Chain/FolderScoreRaderBuf.Chain;
					if (FolderScoreRaderBuf.Unstability != 0)FolderScore[rainbow][i][diff].Unstability = FolderScore[rainbow][i][diff].Unstability/ FolderScoreRaderBuf.Unstability;
					if (FolderScoreRaderBuf.Streak != 0)     FolderScore[rainbow][i][diff].Streak      = FolderScore[rainbow][i][diff].Streak/ FolderScoreRaderBuf.Streak;
					if (FolderScoreRaderBuf.Color != 0)      FolderScore[rainbow][i][diff].Color       = FolderScore[rainbow][i][diff].Color / FolderScoreRaderBuf.Color;
				}
				
			}
			else if (folder->FolderKind[i] == FOLDER_KIND_DIFFICULTY) {//フォルダ単位で算出

				FolderScoreRaderBuf FolderScoreRaderBuf;
				count = 0;



				for (j = 0; j < folder->folder_c[i]; j++) {
					int diff = folder->folder[i][j].difficulty;

					if (folder->folder[i][j].kind == 0 && Music[folder->folder[i][j].song_number].exist[diff] == 1) {//「フォルダ選択に戻る」ではなく存在する譜面
						FolderScore[rainbow][i][1].AverageScore += Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4];
						double ScoreRate = (double)Highscore[folder->folder[i][j].song_number].score[diff + rainbow * 4] / 10000;


						FolderScoreRaderBuf.Global += Music[folder->folder[i][j].song_number].global[rainbow][diff];
						FolderScoreRaderBuf.Local += Music[folder->folder[i][j].song_number].local[rainbow][diff];
						FolderScoreRaderBuf.Chain += Music[folder->folder[i][j].song_number].chain[rainbow][diff];
						FolderScoreRaderBuf.Unstability += Music[folder->folder[i][j].song_number].unstability[rainbow][diff];
						FolderScoreRaderBuf.Streak += Music[folder->folder[i][j].song_number].longNote[rainbow][diff];
						FolderScoreRaderBuf.Color += Music[folder->folder[i][j].song_number].color[rainbow][diff];

						FolderScore[rainbow][i][1].Global += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].global[rainbow][diff];
						FolderScore[rainbow][i][1].Local += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].local[rainbow][diff];
						FolderScore[rainbow][i][1].Chain += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].chain[rainbow][diff];
						FolderScore[rainbow][i][1].Unstability += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].unstability[rainbow][diff];
						FolderScore[rainbow][i][1].Streak += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].longNote[rainbow][diff];
						FolderScore[rainbow][i][1].Color += 100.0 * ScoreRate * Music[folder->folder[i][j].song_number].color[rainbow][diff];

						FolderScore[rainbow][i][1].clearState[clearStateConverter(Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4])]++;
						FolderScore[rainbow][i][1].rank[Highscore[folder->folder[i][j].song_number].rank[diff + rainbow * 4]]++;

						if (FolderScore[rainbow][i][1].ClearType > Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4]) {//フォルダにある譜面の中で一番低いクリア状態をそのフォルダのクリア状態にする
							FolderScore[rainbow][i][1].ClearType = Highscore[folder->folder[i][j].song_number].clear_state[diff + rainbow * 4];
						}

						count++;

					}
				}
				if (count != 0)FolderScore[rainbow][i][1].AverageScore = (double)FolderScore[rainbow][i][1].AverageScore / count;

				if (FolderScoreRaderBuf.Global != 0)     FolderScore[rainbow][i][1].Global =  FolderScore[rainbow][i][1].Global / FolderScoreRaderBuf.Global;
				if (FolderScoreRaderBuf.Local != 0)      FolderScore[rainbow][i][1].Local = FolderScore[rainbow][i][1].Local / FolderScoreRaderBuf.Local;
				if (FolderScoreRaderBuf.Chain != 0)      FolderScore[rainbow][i][1].Chain = FolderScore[rainbow][i][1].Chain / FolderScoreRaderBuf.Chain;
				if (FolderScoreRaderBuf.Unstability != 0)FolderScore[rainbow][i][1].Unstability =  FolderScore[rainbow][i][1].Unstability / FolderScoreRaderBuf.Unstability;
				if (FolderScoreRaderBuf.Streak != 0)     FolderScore[rainbow][i][1].Streak =  FolderScore[rainbow][i][1].Streak / FolderScoreRaderBuf.Streak;
				if (FolderScoreRaderBuf.Color != 0)      FolderScore[rainbow][i][1].Color = FolderScore[rainbow][i][1].Color / FolderScoreRaderBuf.Color;

				for (int diff = 2; diff <= 4; diff++) {//難易度固定フォルダはフォルダ選択時にどの譜面難易度を選んでいても同じ値を表示するために値をコピー
					FolderScore[rainbow][i][diff].AverageScore = FolderScore[rainbow][i][1].AverageScore;

					FolderScore[rainbow][i][diff].Global = FolderScore[rainbow][i][1].Global;
					FolderScore[rainbow][i][diff].Local = FolderScore[rainbow][i][1].Local;
					FolderScore[rainbow][i][diff].Chain = FolderScore[rainbow][i][1].Chain;
					FolderScore[rainbow][i][diff].Unstability = FolderScore[rainbow][i][1].Unstability;
				    FolderScore[rainbow][i][diff].Streak = FolderScore[rainbow][i][1].Streak;
					FolderScore[rainbow][i][diff].Color = FolderScore[rainbow][i][1].Color;

					FolderScore[rainbow][i][diff].ClearType = FolderScore[rainbow][i][1].ClearType;

					for (j = 0; j < 9; j++)FolderScore[rainbow][i][diff].clearState[j] = FolderScore[rainbow][i][1].clearState[j];
					for (j = 0; j < 8; j++)FolderScore[rainbow][i][diff].rank[j] = FolderScore[rainbow][i][1].rank[j];
				}

			}
			//フォルダのランク決定 (0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
			for (int diff = 1; diff < 5; diff++) {//難易度
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


	//セーブデータ読み込み
	SAVEDATA saveData;
	if (loadSaveData(&saveData) == -1) {
		//初回作成時
		saveData.totalPlayCount = PlayCountSum;
		saveData.totalHitNotes = PlayCountSum*500;//暫定で叩いた音符数を格納
		if (saveData.totalPlayCount >= SAVE_DATA_VALUE_LIMIT)saveData.totalPlayCount = SAVE_DATA_VALUE_LIMIT;
		if (saveData.totalHitNotes >= SAVE_DATA_VALUE_LIMIT)saveData.totalHitNotes = SAVE_DATA_VALUE_LIMIT;
	}


	saveData.totalHighScore = 0;
	saveData.totalHighScoreRainbow = 0;
	for (int diff = 1; diff < 5; diff++) {//難易度
		for (j = 0; j < folder->folder_c[FolderIndex::ALL_SONGS]; j++) {
			if (folder->folder[FolderIndex::ALL_SONGS][j].kind == 0 && Music[folder->folder[FolderIndex::ALL_SONGS][j].song_number].exist[diff] == 1) {//「フォルダ選択に戻る」ではなく存在する譜面
				saveData.totalHighScore += Highscore[folder->folder[FolderIndex::ALL_SONGS][j].song_number].score[diff];
				saveData.totalHighScoreRainbow += Highscore[folder->folder[FolderIndex::ALL_SONGS][j].song_number].score[diff + 4];
			}
		}
	}
	if (saveData.totalHighScore >= SAVE_DATA_VALUE_LIMIT)saveData.totalHighScore = SAVE_DATA_VALUE_LIMIT;
	if (saveData.totalHighScoreRainbow >= SAVE_DATA_VALUE_LIMIT)saveData.totalHighScoreRainbow = SAVE_DATA_VALUE_LIMIT;
	writeSaveData(saveData);


	for (int rainbow = 0; rainbow < 2; rainbow++) {//自然管理技術者検定フォルダでランク、クリア状態を表示しないようプレイ状態扱いにする
		for (int diff = 1; diff < 5; diff++) {
			FolderScore[rainbow][FolderIndex::SKILL_TEST_NUMBER][diff].ClearType = CLEARTYPE_PLAY;
		}
	}

	//曲リストのソート
	vector<vector<vector<vector<vector<SortSongListIndex>>>>> SortList;//曲番号ソートリスト[][][][][]

	for (int SortKind = 0; SortKind < option->SORT_NUM+10; SortKind++) {//ソート種類の数だけ用意[*][][][][]
		SortList.push_back(vector<vector<vector<vector<SortSongListIndex>>>>());
		for (int FolderInd = 0; FolderInd < NUMBER_OF_FOLDERS; FolderInd++) {//ソート種類ごとにフォルダの数を追加[][*][][][]
			SortList[SortKind].push_back(vector<vector<vector<SortSongListIndex>>>());

			for (int RainbowInd = 0; RainbowInd < 2; RainbowInd++) {//フォルダ内リスト毎に正規、虹のソート対象の値を追加[][][*][][]
				SortList[SortKind][FolderInd].push_back(vector<vector<SortSongListIndex>>());
				for (int DifficultyInd = 0; DifficultyInd < 4; DifficultyInd++) {//正規、虹のソート対象毎に難易度を追加[][][][*][] 降水確率フォルダではどれを使っても同じ
					SortList[SortKind][FolderInd][RainbowInd].push_back(vector<SortSongListIndex>());

					for (int ListInd = 0; ListInd < folder->folder_c[FolderInd]; ListInd++) {//フォルダの数ごとにフォルダ内リスト数を追加[][][][][*]
						SortList[SortKind][FolderInd][RainbowInd][DifficultyInd].push_back(SortSongListIndex());

						SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].index = ListInd;
						if (SortKind == (int)OptionItem::Sort::NAME) {
							SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = ListInd;
						}
						else if (SortKind == (int)OptionItem::Sort::LEVEL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Music[folder->folder[FolderInd][ListInd].song_number].level[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Music[folder->folder[FolderInd][ListInd].song_number].level[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::SCORE) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].score[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].score[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::CLEAR_STATE) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].clear_state[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].clear_state[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MIN_MISS) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].min_miss[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].min_miss[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::PLAY_COUNT) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].play_count[folder->folder[FolderInd][ListInd].difficulty + RainbowInd * 4];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = Highscore[folder->folder[FolderInd][ListInd].song_number].play_count[1 + DifficultyInd + RainbowInd * 4];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::RADAR) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value =
									(int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][folder->folder[FolderInd][ListInd].difficulty]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][folder->folder[FolderInd][ListInd].difficulty]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][folder->folder[FolderInd][ListInd].difficulty]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][folder->folder[FolderInd][ListInd].difficulty]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][folder->folder[FolderInd][ListInd].difficulty]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
						
							
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = 
									(int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][DifficultyInd + 1]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][DifficultyInd + 1]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][DifficultyInd + 1]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][DifficultyInd + 1]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][DifficultyInd + 1]+
									(int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][DifficultyInd + 1];

							}
						}
						else if (SortKind == (int)OptionItem::Sort::GLOBAL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].global[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::LOCAL) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].local[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::CHAIN) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].chain[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::UNSTABILITY) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].unstability[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::STREAK) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].longNote[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::COLOR) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].color[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind >= (int)OptionItem::Sort::RED_DENSITY && SortKind <= (int)OptionItem::Sort::RAINBOW_DENSITY) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].ColorNotesAmount[folder->folder[FolderInd][ListInd].difficulty][SortKind - (int)OptionItem::Sort::RED_DENSITY];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].ColorNotesAmount[DifficultyInd + 1][SortKind - (int)OptionItem::Sort::RED_DENSITY];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MAX_BPM) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmax[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmax[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MIN_BPM) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmin[folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].bpmmin[DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::MAX_CHORDS) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].maxChords[RainbowInd][folder->folder[FolderInd][ListInd].difficulty];
							}
							else {
								SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][ListInd].value = (int)Music[folder->folder[FolderInd][ListInd].song_number].maxChords[RainbowInd][DifficultyInd + 1];
							}
						}
						else if (SortKind == (int)OptionItem::Sort::VERSION) {
							if (folder->FolderKind[FolderInd] == FOLDER_KIND_DIFFICULTY) {//降水確率別フォルダでは難易度を指定しておく
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
	
	//ソート処理
	for (int SortKind = 0; SortKind < option->SORT_NUM; SortKind++) {//ソートの仕方の数だけ用意[*][][][][]
		for (int FolderInd = 0; FolderInd < NUMBER_OF_FOLDERS; FolderInd++) {//ソート種類ごとにフォルダの数を追加[][*][][][]

			for (int RainbowInd = 0; RainbowInd < 2; RainbowInd++) {//フォルダ内リスト毎に正規、虹のソート対象の値を追加[][][][*][]
				for (int DifficultyInd = 0; DifficultyInd < 4; DifficultyInd++) {//正規、虹のソート対象毎に難易度を追加[][][][][*] 降水確率フォルダではどれを使っても同じ


					stable_sort(&SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][0],
						        &SortList[SortKind][FolderInd][RainbowInd][DifficultyInd][folder->folder_c[FolderInd]-1]);

				}
			}
			
		}
	}

	//2回目以降の選曲画面に戻ってきたときのためのlist_number_baseとlist_number算出(段位認定から戻ってきたときは行わない)
	if (folder->selected_folder != FolderIndex::SKILL_TEST_NUMBER) {
		//ソート結果が前と変わってた時のために、元の曲番号と同じ番号、同じ難易度を指すlist_number_baseを探す
		int now_song_number = song_number;
		int now_difficulty = difficulty;
		int notSortedIndex = 0;//ソート前のリストのインデックスを指す
		for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {//ソート後のリストのインデックスを指す
		    //ソート前のリストのインデックスに変換
			notSortedIndex = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index;

			if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY) {//難易度別フォルダを選んでいた時は曲番号と難易度も合っているか確認する
				//前回選んでいた曲番号と難易度が一致していたらこのソート前のリストのインデックスが正しい
				if (now_song_number ==
					folder->folder[folder->selected_folder][notSortedIndex].song_number

					&&

					now_difficulty ==
					folder->folder[folder->selected_folder][notSortedIndex].difficulty
					) {


					list_number_base = SearchListNumberBase;
				}
			}else if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_NORMAL) {//通常フォルダを選んでいた時は曲番号だけ合っているか確認する
				//前回選んでいた曲番号が一致していたらこのソート前のリストのインデックスが正しい
				if (now_song_number ==
					folder->folder[folder->selected_folder][notSortedIndex].song_number){


					list_number_base = SearchListNumberBase;
				}
			}
		}

		//list_number_baseが見つかったのでlist_number算出
		list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;
	}

	if (PlayCountSum <= 2) {//3回目のプレイ後からフォルダ選択に戻されなくなる
		SelectingTarget = SELECTING_FOLDER;
	}

	wchar_t *StrAnnounce = L"";//通常アナウンス文
	StrAnnounce = announce_str(*StageCount, PlayCountSum);
	int AnnounceWidth =//アナウンス文字長
		GetDrawStringWidthToHandle(//文字長を取得
			StrAnnounce, //アナウンスを出す
			wcslen(StrAnnounce),//同じようにその文字数も引数として入れてやる
			FontHandle);



	//int secret_song_appear_number = -1;
	//int song_appear_number = -1;
	wchar_t secret_str[5] = L"???";//隠し曲表示用文字列
	int rank_point = result_rank_buf[0] + result_rank_buf[1] + result_rank_buf[2];
	
	if (secret->total != 0 && secret->song_appear_number == -1) {//隠し曲があって前に発生した隠し曲演出を消化している(-1)とき

		if ((rank_point >= 5 * 3) || SECRET_ALWAYS_APPEAR) {//過去3回のランクが平均B(値は5)以上で隠し曲演出へ
			if (secret->all_get == 1) {
				secret->secret_song_appear_number = GetRand(secret->total - 1);//現れる曲の隠し曲番号をランダムに選んで入れる
				secret->song_appear_number = secret->song_number[secret->secret_song_appear_number];//曲番号に変換
			}
			else {//全曲解禁していない
				do {
					secret->secret_song_appear_number = GetRand(secret->total - 1);//現れる曲の隠し曲番号をランダムに選んで入れる
					if (secret->get_song_number[secret->secret_song_appear_number] == 0) {//未解禁の隠し曲曲番号ならそれにする
						secret->song_appear_number = secret->song_number[secret->secret_song_appear_number];//曲番号に変換
						break;
					}
				} while (1);//未解禁の曲番号を選ぶまでやり直す
			}
			secret_LOAD_contents(Music, secret->song_appear_number, secret);//アナウンス文の読み込み(secret->Announceに格納)
		
			secret->Announce_width =
				GetDrawStringWidthToHandle(//文字長を取得
					secret->Announce, //アナウンスを出す
					wcslen(secret->Announce),//同じようにその文字数も引数として入れてやる
					FontHandle);

			if (secret->all_get == 0)SelectingTarget = SELECTING_FOLDER;//全解禁していないときフォルダ選択状態にする(アナウンス文を見せるため)
		}
	}

	XY pos_cloud;//雲の座標

	int H_ANNOUNSE;//アナウンス用文字列画像作成
	if (secret->song_appear_number != -1) {//隠し曲出現演出中
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
		if (rank_point == 14) {//ランクの合計が丁度14だったら (Sを二回連続で取るとなる BBCでもなる)
			SH_BGM = LoadSoundMem(L"sound/song_select_bgm2.ogg");//選曲BGMの読み込み
		}
		else if(rank_point >= 12) {//A二回連続またはBBE,CCC等
			SH_BGM = LoadSoundMem(L"sound/song_select_bgm3.ogg");//選曲BGMの読み込み

		}
		else {
			SH_BGM = LoadSoundMem(L"sound/song_select_bgm.ogg");//選曲BGMの読み込み
		}
		H_CLOUD = LoadGraph(L"img/cloud.png");
	}else if (secret->song_appear_number != -1){//隠し曲演出時
		if (wcscmp(L"\0", secret->BGM) == 0) {//何も指定されていなかったら
			SH_BGM = LoadSoundMem(L"sound/song_select_secret_bgm.ogg");//隠し曲選曲BGMの読み込み
		}
		else {
			SH_BGM = LoadSoundMem(secret->BGM);//専用選曲BGMの読み込み
		}
		H_CLOUD = LoadGraph(L"img/cloud_black.png");//隠し曲演出なら黒い雲
	}

	int BGM_continue = 0;//BGMを選曲画面で流し続けるかどうかのフラグ(0:流さない 1:流す)
	if (secret->song_appear_number != -1) {//隠し曲演出時
		if (wcscmp(L"\0", secret->BGM) != 0 && Music[secret->song_appear_number].secret == 1) {//未解禁隠し曲の出現時で専用BGMがあったら
			BGM_continue = 1;//BGMを鳴らし続ける
		}
		else {
			BGM_continue = 0;//BGMはフォルダ選択画面のみ流す
		}
	}
	else {
		BGM_continue = 0;//BGMはフォルダ選択画面のみ流す
	}


	if (option->op.color != OptionItem::Color::RAINBOW) {//虹オプション選択フラグの設定
		select_rainbow = 0;
	}
	if (option->op.color == OptionItem::Color::RAINBOW) {
		select_rainbow = 4;
	}


	ChangeFont(L"メイリオ");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(28);
	SetFontThickness(9);
	
	//非解禁隠し曲でアラーム鳴らす設定の曲は出現時にアラームを鳴らす
	if (secret->get_song_number[secret->secret_song_appear_number] == 0 && secret->beep_alarm == 1) {
		//NMC緊急速報表示
		H_NMC = LoadGraph(L"img/NMC.png");
		DrawGraph(0, 0, H_NMC, TRUE);
		ScreenFlip();

		//0.1秒待つ
		Sleep(100);

		PlaySoundMem(SH_ALARM, DX_PLAYTYPE_BACK, TRUE);
		while (CheckSoundMem(SH_ALARM) == 1) {//なり終わるまで待機
			if (ProcessMessage() != 0 || Key[KEY_INPUT_ESCAPE] == 1 && flag != FLAG_CLOSING_STATE) {//ESCでゲーム終了
				DxLib_End();
				exit(0);
				return;
			}
		}
	}

	

	GAME_start_time = GetNowCount_d(config);
	int Announse_show_time_base = GetNowCount() + 1500;//アナウンス表示の基準時間
	while (1) {
		if (ProcessMessage() != 0 || Key[KEY_INPUT_ESCAPE] == 1 && flag != FLAG_CLOSING_STATE && flag != FLAG_END_FUNCTION_STATE) {//ESCでゲーム終了
			DxLib_End();
			exit(0);
			return;
		}

		//BGM再生
		if (BGM_play == 0 && GAME_passed_time >= 1000) {
			PlaySoundMem(SH_BGM, DX_PLAYTYPE_LOOP, TRUE);
			BGM_play = 1;
		}
		if (flag != FLAG_END_FUNCTION_STATE) {//シャッター閉まったらここではBGMの音量調整は行わない
			ChangeVolumeSoundMem(int(bgm_vol * 255), SH_BGM);
		}

		//CALC
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		Get_Key_State(Buf, Key, AC);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		if (flag == FLAG_SELECT_STATE) {
			int PressFrame = int(25.0 * (17.0 / LOOP_passed_time));//ボタン押し続けてカーソルが動き続けるようになるまでのフレーム
			if (PressFrame <= 0)PressFrame = 1;//0以下にはしない

			//printfDx(L"%d\n", PressFrame);

			if (Key[KEY_INPUT_F1] == 1) {//デバッグモードONOFF
				if (*debug == 0) {
					*debug = 1;
					PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
				}
				else {
					*debug = 0;
					PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
				}
			}

			if (Key[KEY_INPUT_F2] == 1) {//スコア再送信&ランキング表示
				if (SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {//存在する譜面を選んでいる時
					PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
					IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color == OptionItem::Color::RAINBOW, config);
					IRview(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, option->op.color == OptionItem::Color::RAINBOW, config);
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

			if (Key[KEY_INPUT_F3] == 1 && SEND_EXIST_SCORE_TO_IR) {//既存スコアを送信
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

				IRsave(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, IrScore, difficulty, Music[song_number].season[difficulty], option->op.color == OptionItem::Color::RAINBOW, 0, config);
				IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color == OptionItem::Color::RAINBOW, config);
			}

			if (Key[Button_Shutter] == 1) {//スクリーンショット
				ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
			}


			//オプション画面ONOFF処理(この処理を行ったフレームでは他の操作を受け付けない)
			if (OptionOpen == 0 && (Key[Button[0][0]] == 1 || Key[Button[2][0]] == 1)) {//オプション画面ON
				OptionOpen = 1;//オプション選択モードにする
				PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
				time_base_str = int(GetNowCount_d(config));
			}
			else if (OptionOpen == 1 && (Key[Button[0][0]] == 1 || Key[Button[2][0]] == 1 || Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1)) {//オプション画面OFF
				OptionOpen = 0;//選曲モードにする
				PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
				time_base_str = int(GetNowCount_d(config));
			}
			else {//この処理を行わなかったフレーム

				if (Key[Button[0][3]] == 1 || Key[Button[2][3]] == 1) {//リザルト画面ONOFF
					if (ShowResultFlag == 0) {
						ShowResultFlag = 1;//リザルト表示モードにする
						PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
					}
					else {
						ShowResultFlag = 0;//選曲モードにする
						PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
					}
				}


				if (OptionOpen == 0 && SelectingTarget == SELECTING_SONG) {//オプション出してなくて曲選択モードのとき
					if ((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
						&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1)) {//上下同時押しでフォルダセレクトに戻る
						SelectingTarget = SELECTING_FOLDER;
						StopSoundMem(SH_SONG);
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						song_play_counter = 1;
						jacket_show_counter = 1;//ジャケットの読み込み
						jacket_alpha = 0;
						widthCalcFlag = 1;
					}
					else {
						if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
							if (difficulty != 1) {
								PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

								if ((wcscmp(Music[song_number].wavpath[difficulty], Music[song_number].wavpath[difficulty - 1]) != 0//パスが違う
									|| (Music[song_number].demostart[difficulty] != Music[song_number].demostart[difficulty - 1]))//それかデモスタートが違う
									|| ((Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty - 1]))) {
									StopSoundMem(SH_SONG);
									//DeleteSoundMem(SH_SONG);
									song_play_counter = 1;
								}

								if (
									(wcscmp(Music[song_number].jacketpath[difficulty], Music[song_number].jacketpath[difficulty - 1]) != 0)
									|| (Music[song_number].exist[difficulty] != Music[song_number].exist[difficulty - 1])
									) {
									jacket_show_counter = 1;//ジャケットの読み込み
									jacket_alpha = 0;
								}
								widthCalcFlag = 1;

								//別難易度で元のlist_numberと同じ番号を指すlist_number_baseを探す
								int now_list_number = list_number;
								for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
									if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1 - 1][SearchListNumberBase].index) {
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
									jacket_show_counter = 1;//ジャケットの読み込み
									jacket_alpha = 0;
								}
								widthCalcFlag = 1;

								//別難易度で元のlist_numberと同じ番号を指すlist_number_baseを探す
								int now_list_number = list_number;
								for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
									if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty + 1 - 1][SearchListNumberBase].index) {
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

							list_number_base--;
							list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

							list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;

							song_number = folder->folder[folder->selected_folder][list_number].song_number;


							if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//レベル別フォルダの時で「フォルダ選択に戻る」じゃないときは難易度も変えておく
								difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
							}

							bn_draw_counter--;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							song_play_counter = 1;

							jacket_show_counter = 1;//ジャケットの読み込み
							jacket_alpha = 0;
							widthCalcFlag = 1;
						}
						if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
							StopSoundMem(SH_SONG);
							//DeleteSoundMem(SH_SONG);
							list_number_base++;
							list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

							list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;
							song_number = folder->folder[folder->selected_folder][list_number].song_number;

							if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//レベル別フォルダの時で「フォルダ選択に戻る」じゃないときは難易度も変えておく
								difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
							}

							bn_draw_counter++;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							song_play_counter = 1;

							jacket_show_counter = 1;//ジャケットの読み込み
							jacket_alpha = 0;
							widthCalcFlag = 1;
						}


						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);
								list_number_base--;
								list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

								list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;
								song_number = folder->folder[folder->selected_folder][list_number].song_number;

								if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//レベル別フォルダの時で「フォルダ選択に戻る」じゃないときは難易度も変えておく
									difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
								}

								bn_draw_counter--;
								roll_counter = 1;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								song_play_counter = 1;

								jacket_show_counter = 1;//ジャケットの読み込み
								jacket_alpha = 0;
								widthCalcFlag = 1;
							}
						}
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								StopSoundMem(SH_SONG);
								//DeleteSoundMem(SH_SONG);
								list_number_base++;
								list_number_base = number_ring(list_number_base, folder->folder_c[folder->selected_folder] - 1);

								list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;
								song_number = folder->folder[folder->selected_folder][list_number].song_number;

								if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && folder->folder[folder->selected_folder][list_number].difficulty != 0) {//レベル別フォルダの時で「フォルダ選択に戻る」じゃないときは難易度も変えておく
									difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
								}

								bn_draw_counter++;
								roll_counter = 1;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								song_play_counter = 1;

								jacket_show_counter = 1;//ジャケットの読み込み
								jacket_alpha = 0;
								widthCalcFlag = 1;
							}
						}

						if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//曲決定
							if (folder->folder[folder->selected_folder][list_number].kind == 0) {//内容が「フォルダ選択に戻る」じゃなくて「曲」のとき
								if ((Music[song_number].exist[difficulty] == 1 && Music[song_number].secret != 1)//その難易度が存在して隠し曲ではない
									|| (Music[song_number].exist[difficulty] == 1 && Music[song_number].secret == 1 && secret->song_appear_number == song_number)//それか隠し曲で出現対象になっているとき
									) {
									flag = FLAG_CLOSING_STATE;
									PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
								}
								else {
									PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);//選曲不可
								}

							}
							else {//「フォルダ選択に戻る」を決定したとき
								SelectingTarget = SELECTING_FOLDER;

								StopSoundMem(SH_SONG);

								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								song_play_counter = 1;

								jacket_show_counter = 1;//ジャケットの読み込み
								jacket_alpha = 0;
								widthCalcFlag = 1;
								Get_Key_State(Buf, Key, AC);//すぐ下の決定処理に反応しないようにもう一度キー入力把握処理
							}
						}
					}
				}

				if (OptionOpen == 0 && SelectingTarget == SELECTING_COURSE) {//段位認定選択モードのとき
					if ((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
						&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1)) {//上下同時押しでフォルダセレクトに戻る
						SelectingTarget = SELECTING_FOLDER;
						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						widthCalcFlag = 1;
					}
					else {
						if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
							list_number--;
							list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
							list_number_base = list_number;
							bn_draw_counter--;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							widthCalcFlag = 1;
						}
						if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
							list_number++;
							list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
							list_number_base = list_number;
							bn_draw_counter++;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
							widthCalcFlag = 1;
						}


						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								list_number--;
								list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
								list_number_base = list_number;
								bn_draw_counter--;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
							}
						}
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								list_number++;
								list_number = number_ring(list_number, NUMBER_OF_COURSES - 1);
								list_number_base = list_number;
								bn_draw_counter++;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
							}
						}

						if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//コース決定
							if (STList->Kind[list_number] != 2) {//内容が「フォルダ選択に戻る」じゃなくて「コース」のとき
								flag = FLAG_CLOSING_STATE;
								PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
							}
							else {//「フォルダ選択に戻る」を決定したとき
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								SelectingTarget = SELECTING_FOLDER;
								widthCalcFlag = 1;
								Get_Key_State(Buf, Key, AC);//すぐ下の決定処理に反応しないようにもう一度キー入力把握処理
							}
						}
					}
				}


				if (OptionOpen == 0 && SelectingTarget == SELECTING_FOLDER) {//フォルダ選択モードのとき

					if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
						if (difficulty != 1) {
							PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
							widthCalcFlag = 1;
						}
						difficulty--;
						if (difficulty > 4)difficulty = 4;
						if (difficulty < 1)difficulty = 1;
					}

					if (Key[Button[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {
						if (difficulty != 4) {
							PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
							widthCalcFlag = 1;
						}
						difficulty++;
						if (difficulty > 4)difficulty = 4;
						if (difficulty < 1)difficulty = 1;
					}


					if (!((Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[KEY_INPUT_UP] >= 1)
						&& (Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[KEY_INPUT_DOWN] >= 1))
						) {//上下同時押ししてたら動かさない
						if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {

							folder->selected_folder--;
							folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);

							bn_draw_counter--;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);


							widthCalcFlag = 1;
						}
						if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
							folder->selected_folder++;
							folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);

							bn_draw_counter++;
							PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);


							widthCalcFlag = 1;
						}


						if (Key[Button[0][1]] > PressFrame || Key[Button[0][2]] > PressFrame || Key[KEY_INPUT_UP] >= PressFrame) {
							if (roll_counter == 0) {
								folder->selected_folder--;
								folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);
								bn_draw_counter--;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								//time_base_str = GetNowCount_d(config);
							}
						}
						if (Key[Button[2][1]] > PressFrame || Key[Button[2][2]] > PressFrame || Key[KEY_INPUT_DOWN] >= PressFrame) {
							if (roll_counter == 0) {
								folder->selected_folder++;
								folder->selected_folder = number_ring(folder->selected_folder, folder->NumberOfFolders - 1);
								bn_draw_counter++;
								PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
								widthCalcFlag = 1;
								roll_counter = 1;
								//time_base_str = GetNowCount_d(config);
							}
						}
					}

					if (Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[KEY_INPUT_RETURN] == 1) {//フォルダ決定
						if (folder->folder_c[folder->selected_folder] != 0) {//そのフォルダに曲が存在したら(曲数が0じゃないとき)
							PlaySoundMem(SH_FOLDER_SELECT, DX_PLAYTYPE_BACK, TRUE);

							if (folder->FolderKind[folder->selected_folder] != FOLDER_KIND_SKILL_TEST) {//段位認定以外のフォルダを開いたとき
								SelectingTarget = SELECTING_SONG;
								list_number_base = 0;

								list_number = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base].index;

								song_number = folder->folder[folder->selected_folder][list_number].song_number;

								if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY) {//レベル別フォルダを開いた時は難易度も変えておく
									difficulty = folder->folder[folder->selected_folder][list_number].difficulty;
								}

							}
							else {//段位認定フォルダを開いた時
								list_number = 0;//コース選択番号を0に song_numberは初期化必要無し
								list_number_base = list_number;
								SelectingTarget = SELECTING_COURSE;

							}


						}
						else {
							PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);//選曲不可
						}

						song_play_counter = 1;
						jacket_show_counter = 1;//ジャケットの読み込み
						jacket_alpha = 0;

						widthCalcFlag = 1;
					}
				}

				if (OptionOpen == 1) {//オプション選択画面の時
					if (Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[KEY_INPUT_UP] == 1) {
						option_select--;
						option_select = number_ring((int)option_select, option->OPTION_NUM - 1);//OPTION_NUM収める
						DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);

						if (option_select > OptionShowEnd) {
							OptionShowEnd = option_select;
							OptionShowStart = option_select - (OptionShowAmount - 1);
						}
						else if(option_select < OptionShowStart){
							OptionShowEnd = option_select + (OptionShowAmount - 1);
							OptionShowStart = option_select;
						}

						PlaySoundMem(SH_SONG_SELECT, DX_PLAYTYPE_BACK, TRUE);
						time_base_str = int(GetNowCount_d(config));
					}
					if (Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[KEY_INPUT_DOWN] == 1) {
						option_select++;
						option_select = number_ring(option_select, option->OPTION_NUM - 1);//0~3に収める
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
					}

					if (Key[Button[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {
						time_base_str = int(GetNowCount_d(config));
						OptionValueChange(option, option_select, -1);
						DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);
						if(option_select != (int)OptionItem::Name::HITSOUND)PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

						if (option_select == (int)OptionItem::Name::COLOR || option_select == (int)OptionItem::Name::SORT) {
							//元のlist_numberと同じ番号を指すlist_number_baseを探す
							int now_list_number = list_number;
							for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
								if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index) {
									list_number_base = SearchListNumberBase;
								}
							}
							widthCalcFlag = 1;//ソート種類を変更したときは曲名画像再描画
						}

						if (option_select == (int)OptionItem::Name::NOTE) {
							sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[1]);
							H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
							sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[8]);
							H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);
						}
						if (option_select == (int)OptionItem::Name::HITSOUND) {
							sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
							SH_OPTION_HITSOUND_PREVIEW = LoadSoundMem(strcash, 1);//HIT SOUNDプレビュー音声読み込み
							PlaySoundMem(SH_OPTION_HITSOUND_PREVIEW, DX_PLAYTYPE_BACK, TRUE);
						}
						if (option_select == (int)OptionItem::Name::THEME) {
							wstring themeStr1(L"img/themes/");
							wstring themeStr2(option->theme[option->op.theme]);
							H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
							H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());
							H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
							H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
							H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
							H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
							H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());
							H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
						}
					}

					if (Key[Button[1][0]] > PressFrame || Key[KEY_INPUT_LEFT] > PressFrame) {//押し続けたとき
						if (roll_counter == 0) {
							time_base_str = int(GetNowCount_d(config));
							OptionValueChange(option, option_select, -1);
							DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);
							if (option_select != (int)OptionItem::Name::HITSOUND)PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
							roll_counter = 1;

							if (option_select == (int)OptionItem::Name::COLOR || option_select == (int)OptionItem::Name::SORT) {
								//元のlist_numberと同じ番号を指すlist_number_baseを探す
								int now_list_number = list_number;
								for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
									if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index) {
										list_number_base = SearchListNumberBase;
									}
								}
								widthCalcFlag = 1;//ソート種類を変更したときは曲名画像再描画
							}

							if (option_select == (int)OptionItem::Name::NOTE) {
								sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[1]);
								H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
								sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[8]);
								H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);
							}
							if (option_select == (int)OptionItem::Name::HITSOUND) {
								sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
								SH_OPTION_HITSOUND_PREVIEW = LoadSoundMem(strcash, 1);//HIT SOUNDプレビュー音声読み込み
								PlaySoundMem(SH_OPTION_HITSOUND_PREVIEW, DX_PLAYTYPE_BACK, TRUE);
							}
							if (option_select == (int)OptionItem::Name::THEME) {
								wstring themeStr1(L"img/themes/");
								wstring themeStr2(option->theme[option->op.theme]);
								H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
								H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());
								H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
								H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
								H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
								H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
								H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());
								H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
							}
						}
					}

					if (Key[Button[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {
						time_base_str = int(GetNowCount_d(config));
						OptionValueChange(option, option_select, 1);
						DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);
						if (option_select != (int)OptionItem::Name::HITSOUND)PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);

						if (option_select == (int)OptionItem::Name::COLOR || option_select == (int)OptionItem::Name::SORT) {
							//元のlist_numberと同じ番号を指すlist_number_baseを探す
							int now_list_number = list_number;
							for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
								if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index) {
									list_number_base = SearchListNumberBase;
								}
							}
							widthCalcFlag = 1;//ソート種類を変更したときは曲名画像再描画
						}

						if (option_select == (int)OptionItem::Name::NOTE) {
							sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[1]);
							H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
							sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[8]);
							H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);
						}
						if (option_select == (int)OptionItem::Name::HITSOUND) {
							sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
							SH_OPTION_HITSOUND_PREVIEW = LoadSoundMem(strcash, 1);//HIT SOUNDプレビュー音声読み込み
							PlaySoundMem(SH_OPTION_HITSOUND_PREVIEW, DX_PLAYTYPE_BACK, TRUE);
						}
						if (option_select == (int)OptionItem::Name::THEME) {
							wstring themeStr1(L"img/themes/");
							wstring themeStr2(option->theme[option->op.theme]);
							H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
							H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());
							H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
							H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
							H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
							H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
							H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());
							H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
						}
					}


					if (Key[Button[1][3]] > PressFrame || Key[KEY_INPUT_RIGHT] > PressFrame) {//押し続けたとき
						if (roll_counter == 0) {
							time_base_str = int(GetNowCount_d(config));
							OptionValueChange(option, option_select, 1);
							DrawOptionSentence(option, (OptionItem::Name)option_select, config, FontHandle);
							if (option_select != (int)OptionItem::Name::HITSOUND)PlaySoundMem(SH_DIFFICULTY_SELECT, DX_PLAYTYPE_BACK, TRUE);
							roll_counter = 1;

							if (option_select == (int)OptionItem::Name::COLOR || option_select == (int)OptionItem::Name::SORT) {
								//元のlist_numberと同じ番号を指すlist_number_baseを探す
								int now_list_number = list_number;
								for (int SearchListNumberBase = 0; SearchListNumberBase < folder->folder_c[folder->selected_folder]; SearchListNumberBase++) {
									if (now_list_number == SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][SearchListNumberBase].index) {
										list_number_base = SearchListNumberBase;
									}
								}
								widthCalcFlag = 1;//ソート種類を変更したときは曲名画像再描画
							}

							if (option_select == (int)OptionItem::Name::NOTE) {
								sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[1]);
								H_OPTION_NOTE_PREVIEW[0] = LoadGraph(strcash);
								sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[8]);
								H_OPTION_NOTE_PREVIEW[1] = LoadGraph(strcash);
							}
							if (option_select == (int)OptionItem::Name::HITSOUND) {
								sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
								SH_OPTION_HITSOUND_PREVIEW = LoadSoundMem(strcash, 1);//HIT SOUNDプレビュー音声読み込み
								PlaySoundMem(SH_OPTION_HITSOUND_PREVIEW, DX_PLAYTYPE_BACK, TRUE);
							}
							if (option_select == (int)OptionItem::Name::THEME) {
								wstring themeStr1(L"img/themes/");
								wstring themeStr2(option->theme[option->op.theme]);
								H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
								H_COVER[1] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());
								H_COVER[2] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
								H_COVER[3] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
								H_COVER[4] = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
								H_COVER_SKILL_TEST = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_skill_test.png")).c_str());
								H_COVER_OPTION = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str());
								H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
							}
						}
					}
				}

			}

			if (option->op.color != OptionItem::Color::RAINBOW) {//虹オプション選択フラグの設定
				select_rainbow = 0;
			}
			if (option->op.color == OptionItem::Color::RAINBOW) {
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

			

			if (Music[song_number].secret == 1 && secret->song_appear_number != song_number && SelectingTarget == SELECTING_SONG) {//隠し曲はレーダー,グラフを表示しない
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
			else {//通常曲、解禁曲はレーダー,グラフ表示
				int R = 0;
				if (option->op.color == OptionItem::Color::RAINBOW)R = 1;


				if (SelectingTarget == SELECTING_SONG) {
					DRShowGlobal += 0.01 * ((double)Music[song_number].global[R][difficulty] - DRShowGlobal);
					DRShowLocal += 0.01 * ((double)Music[song_number].local[R][difficulty] - DRShowLocal);
					DRShowChain += 0.01 * ((double)Music[song_number].chain[R][difficulty] - DRShowChain);
					DRShowUnstability += 0.01 * ((double)Music[song_number].unstability[R][difficulty] - DRShowUnstability);
					DRShowStreak += 0.01 * ((double)Music[song_number].longNote[R][difficulty] - DRShowStreak);
					DRShowColor += 0.01 * ((double)Music[song_number].color[R][difficulty] - DRShowColor);
				}
				else if (SelectingTarget == SELECTING_FOLDER) {
					DRShowGlobal += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Global - DRShowGlobal);
					DRShowLocal += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Local - DRShowLocal);
					DRShowChain += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Chain - DRShowChain);
					DRShowUnstability += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Unstability - DRShowUnstability);
					DRShowStreak += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Streak - DRShowStreak);
					DRShowColor += 0.01 * ((double)FolderScore[R][folder->selected_folder][difficulty].Color - DRShowColor);
				}


				for (j = 0; j < 9; j++) {
					ShowLocalNotesGraph[j] += 0.01 * (Music[song_number].LocalNotesAmount[difficulty][j] - ShowLocalNotesGraph[j]);
					ShowColorNotesGraph[j] += 0.01 * (Music[song_number].ColorNotesAmount[difficulty][j] - ShowColorNotesGraph[j]);
				}
			}
			if (DRShowGlobal < 0)DRShowGlobal = 0;
			if (DRShowLocal < 0)DRShowLocal = 0;
			if (DRShowChain < 0)DRShowChain = 0;
			if (DRShowUnstability < 0)DRShowUnstability = 0;
			if (DRShowStreak < 0)DRShowStreak = 0;
			if (DRShowColor < 0)DRShowColor = 0;
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
				jacket_alpha += 0.003;
			}

			if (SelectingTarget == SELECTING_FOLDER || SelectingTarget == SELECTING_COURSE) {//フォルダ,コースセレクト時
				if (BGM_continue == 0) {
					if (bgm_vol < 1) {
						bgm_vol += 0.001;
					}
					if (bgm_vol >= 1) {
						bgm_vol = 1;
					}
				}
			}


			if (SelectingTarget == SELECTING_SONG) {//曲選択時はBGMの音量0になめらかに移行
				if (BGM_continue == 0) {
					if (bgm_vol > 0) {
						bgm_vol -= 0.01;
					}
					if (bgm_vol <= 0) {
						bgm_vol = 0;
					}
				}
			}
		}
	

		if (SelectingTarget == SELECTING_SONG) {
			if (song_play_counter < 0 && song_play_counter != -1 && song_play_counter != -2 && song_play_counter != -3 && BGM_continue == 0) {//専用選曲BGMのある未解禁隠し曲の出現時以外で曲読み込み
				song_play_counter = -1;
				SetUseASyncLoadFlag(TRUE);

				SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMPRESS);
				DeleteSoundMem(SH_SONG);
				SH_SONG = LoadSoundMem(Music[song_number].wavpath[difficulty]);

				SetCreateSoundDataType(DX_SOUNDDATATYPE_MEMNOPRESS);

				SetUseASyncLoadFlag(FALSE);
			}
		}

		if (SelectingTarget == SELECTING_SONG) {
			if (jacket_show_counter < 0 && jacket_show_counter != -1 && jacket_show_counter != -2) {//ジャケット読み込み
				jacket_show_counter = -1;
				jacket_alpha = 0;
				SetUseASyncLoadFlag(TRUE);
				DeleteGraph(H_JACKET);
				DeleteGraph(H_JACKET_GAUSS);
				H_JACKET = LoadGraph(Music[song_number].jacketpath[difficulty]);//ジャケットの読み込み
				H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath[difficulty]);//ぼかす方のジャケットの読み込み
				SetUseASyncLoadFlag(FALSE);
			}
		}

		//ProcessMessage();

		if (SelectingTarget == SELECTING_SONG) {
			if (CheckHandleASyncLoad(SH_SONG) == FALSE && song_play_counter == -1) {//曲の読み込みが終了したので再生
				song_play_counter = -2;
				SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
				if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら鳴らさない
					//鳴らさない
				}
				else {//隠し曲ではない(解禁済み)なので鳴らす
					PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
				}
			}

			//曲の再生が終了したらループ再生
			if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら鳴らさない
					//鳴らさない
			}
			else {//隠し曲ではない(解禁済み)なので鳴らす
				if (song_play_counter == -2 && CheckSoundMem(SH_SONG) == 0) {
					SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
					PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
				}
			}

		}

		brightness = int(155 + 100 * (1 + sin((double)GAME_passed_time / 300)) / 2);

		if (flag == FLAG_END_FUNCTION_STATE) {
			//デモのフェードアウト
			if (demo_vol > 0) {
				demo_vol = 1.0 - ((GAME_passed_time - CoverClosedTime) / 1200);
			}
			if (demo_vol < 0) {
				demo_vol = 0;
			}
			ChangeVolumeSoundMem(int(demo_vol * 255), SH_SONG);//デモの音量は曲決定後にフェードアウト
			if (BGM_continue == 1) {
				ChangeVolumeSoundMem(int(demo_vol * 255), SH_BGM);//BGM有り隠し曲出現時はBGMもフェードアウト
			}
			if (SelectingTarget == SELECTING_COURSE) {
				ChangeVolumeSoundMem(int(demo_vol * 255), SH_BGM);//段位コース選んだ時はBGMもフェードアウト
			}
		}

		if (flag == FLAG_END_FUNCTION_STATE && GAME_passed_time - CoverClosedTime >= 1200) {//曲選択
			
			for (i = 0; i <= 40; i++) {
                /*
				if (ProcessMessage() != 0 || Key[KEY_INPUT_ESCAPE] == 1 && flag != FLAG_CLOSING_STATE) {//ESCでゲーム終了
					DxLib_End();
					exit(0);
					return;
				}
             	*/
            	Sleep(1);
			}
			

			DeleteFontToHandle(FontHandle);//フォントハンドル削除
			InitSoundMem();
			InitGraph();//グラフィックメモリ開放
			


			if (SelectingTarget == SELECTING_SONG) {//曲選択の時

				int gauge_buf = (int)option->op.gauge;//今選んでいるゲージ種類を保存
				int AllowExit = 1;//途中退出可能か
				if (Music[song_number].secret == 1 && secret->song_appear_number == song_number) {//隠し曲出現中にその隠し曲を選択したので
					//debug取り消し
					*debug = 0;
					//ゲージ種類決定
					if (SECRET_GAUGE_CHANGE_OFF == 0) {

						int rank_point = result_rank_buf[0] + result_rank_buf[1] + result_rank_buf[2];


						if (rank_point == 5 * 3) {//全てBランク(rank_point==15)
							option->op.gauge = OptionItem::Gauge::FC_ATTACK;//強制FC ATTACKゲージ
						}
						else if (rank_point < 6 * 3) {//平均がAランクより小さい値(rank_point==16~17)
							option->op.gauge = OptionItem::Gauge::SUPER_HARD;//強制SUPER HARDゲージ
						}
						else if (rank_point < 7 * 3) {//平均がSランクより小さい値(rank_point==18~20)
							option->op.gauge = OptionItem::Gauge::HARD;//強制HARDゲージ
						}
						else {//全てSランク(rank_point==21)
							option->op.gauge = OptionItem::Gauge::NORMAL;//強制NORMALゲージ
						}
					}

					AllowExit = 0;
				}

				int CourseCombo = 0;
				int CourseMaxCombo = 0;

				int TryCount = 0;//トライ数(クイックリトライするたびに加算)

				do {
					TryCount++;
					escape = 0;
					retryAble = 0;

					GAME(song_number, difficulty,
						&res, &escape, option, &retryAble,
						debug, Music, Button, Button_Shutter, Key, Buf, secret->song_appear_number != -1, AC,
						config, ir,
						0, NULL, &CourseCombo, &CourseMaxCombo, AllowExit);//選曲番号と難易度を渡してゲーム画面へ
					Get_Key_State(Buf, Key, AC);
				} while ((Key[Button[0][0]] >= 1 || Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[Button[0][3]] >= 1)
					&& (Key[Button[2][0]] >= 1 || Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[Button[2][3]] >= 1)
					&& (retryAble == 1)
					&& (secret->song_appear_number != song_number || Music[song_number].secret != 1)//リトライできるときの条件(隠し曲演出中の未解禁隠し曲プレイはリトライできない)
					);

				RESULT STResDummy;
				//演奏終了後処理
				if (escape == 0
					|| (*debug == 0
						&& escape == 1
						&& secret->song_appear_number == song_number
						&& secret->get_song_number[secret->secret_song_appear_number] == 0)) {
					//途中で抜け出していない,または抜け出してても非デバッグモードでそれが隠し曲演出中の未解禁隠し曲ならクリア失敗扱いにする

					if (escape == 1 && secret->song_appear_number == song_number) {//隠し曲プレイの抜け出しはランクをFに
						res.rank = 1;
					}

					SHOW_RESULT(res, option, song_number, difficulty, debug, Music, Button, Button_Shutter, Key, Buf, AC, TryCount,
						STList,
						list_number, config, ir);//結果発表
					if (*debug == 0) {
						if (secret->secret_song_appear_number == -1) {//通常プレイで隠し曲演出になっていないとき
							result_rank_buf[*result_count] = res.rank;//ランクを保存

							(*result_count)++;
							if (*result_count == SECRET_SONG_APPEAR_CYCLE) {
								(*result_count) = 0;
							}
						}
						if (secret->secret_song_appear_number != -1) {//隠し曲演出の後
							*result_count = 0;
							for (i = 0; i <= SECRET_SONG_APPEAR_CYCLE - 1; i++) {
								result_rank_buf[i] = 0;//ランクバッファを初期化
							}

							if (res.clear >= CLEARTYPE_EASY_CLEARED && song_number == secret->song_appear_number) {//対象曲をクリアしていた
								//解禁情報を更新
								secret->get_song_number[secret->secret_song_appear_number] = 1;//解禁済みにする
								Music[secret->song_appear_number].secret = 2;
								//全解禁したか確認
								secret_all_get(secret);//全解禁されているならall_getは1
							}
							//隠し曲演出終了
							secret->secret_song_appear_number = -1;
							secret->song_appear_number = -1;
						}
					}
					option->op.gauge = (OptionItem::Gauge)gauge_buf;//ゲージをもとに戻す
				}

			}
			else if (SelectingTarget == SELECTING_COURSE) {//段位認定コース選択の時
				//*debug = 0;//debug取り消し
				//対象曲パス読み込み

				//オプションを直す
				int gauge_buf = (int)option->op.gauge;//後で戻すためのバッファ

				option->op.gauge = OptionItem::Gauge::SKILL_TEST;

				if (option->op.lane != OptionItem::Lane::MIRROR) {//MIRROR以外を選んだときは正規にする
					option->op.lane = OptionItem::Lane::NONE;
				}


				option->op.color = OptionItem::Color::NONE;



				//基本なら虹オプションにする
				if (STList->Kind[list_number] == 0) {
					option->op.color = OptionItem::Color::RAINBOW;
				}

				int SongNumberList[4];//曲番号リスト
				int DifficultyList[4];//難易度リスト

				LoadSkillTestNpsPathList(SongNumberList, DifficultyList, STList->fliepath[list_number], Music, *NumberOfSongs);//npsファイルパス読み込み
				RESULT STRes;//段位認定リザルト
				STRes.clear = 1;//初期値は合格にしておく

				RESULT Result[4];//各曲のリザルト

				double GaugeVal = 100;

				int CourseCombo = 0;
				int CourseMaxCombo = 0;
				int AllowExit = 1;
				//1~4曲目
				for (i = 0; i <= 3; i++) {
					int TryCount = 1;//トライ数
					GAME(SongNumberList[i], DifficultyList[i],
						&Result[i], &escape, option, &retryAble,
						debug, Music, Button, Button_Shutter, Key, Buf, 0, AC, config, ir,
						i + 1, &GaugeVal,
						&CourseCombo, &CourseMaxCombo, AllowExit);//選曲番号と難易度を渡してゲーム画面へ	
					if (escape == 1) {//プレイの抜け出しはランクをF、FAILEDに
						Result[i].rank = 1;
						Result[i].clear = CLEARTYPE_FAILED;
					}
					if (escape == 0) {//抜けだしてない
						SHOW_RESULT(Result[i], option, SongNumberList[i], DifficultyList[i], debug, Music, Button, Button_Shutter, Key, Buf, AC, TryCount,
							STList,
							list_number,
							config,
							ir,
							i + 1);//結果発表
					}

					if (Result[i].clear >= CLEARTYPE_FULL_COMBO && Music[SongNumberList[i]].secret == 1) {//隠し曲をフルコンボ以上でクリアした
						//解禁情報を更新
						int secretSongNnumber = 0;
						//隠し曲番号の確認
						for (j = 0; j < secret->total; j++) {
							if (secret->song_number[j] == SongNumberList[i]) {
								secretSongNnumber = j;
							}
						}
						secret->get_song_number[secretSongNnumber] = 1;//解禁済みにする
						Music[SongNumberList[i]].secret = 2;
						//全解禁したか確認
						secret_all_get(secret);//全解禁されているならall_getは1
					}

					if (Result[i].clear == CLEARTYPE_FAILED) {//この曲で抜け出した時段位終了
						STRes.clear = 0;
						break;
					}
				}
				
				if (escape == 0) {//抜けだしてない
				//段位認定リザルトの保存
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

					if (STRes.rank <= 0)STRes.rank = 1;//F以下にはしない




					//段位のリザルト表示


					SHOW_RESULT(STRes, option, SongNumberList[0], DifficultyList[0], debug, Music, Button, Button_Shutter, Key, Buf, AC, 0,
						STList,
						list_number,
						config,
						ir,
						SHOW_SKILL_TEST_RESULT);//結果発表
				}
				option->op.gauge = (OptionItem::Gauge)gauge_buf;//ゲージを元に戻す
			}

			*l_n = list_number_base;//前選んだフォルダ中のリストを選んでいる状態にする
			*s_n = song_number;//前選んだ曲を選んでいる状態にする
			*diff = difficulty;//前選んだ難易度を選んでいる状態にする
			//*opt = Option->op;//前選んだオプションを選択している状態にする
			
			//プレイオプションの保存
			OP op_buf = option->op;//オプション保存用
			if (op_buf.gauge == OptionItem::Gauge::SKILL_TEST) {//段位ゲージの時はNORMALゲージで保存
				op_buf.gauge = OptionItem::Gauge::NORMAL;
			}
			SaveOptionState(op_buf);

			//メモリ開放
			delete[] width_sent_speed;
			delete[] width_sent_gauge;
			delete[] width_sent_lane;
			delete[] width_sent_color;

			free(Highscore);
			free(HighscoreRival);
			return;//関数終了
		}

		//printfDx(L"LOOP3:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

		//表示する複数のジャンル、アーティスト、曲名幅の計算
		if (SelectingTarget == SELECTING_SONG) {
			if (widthCalcFlag == 1) {//上下左右のキーを押して曲,難易度が変わっていたら
				for (i = 0; i <= Column; i++) {//Column/2が中心
	                //title_width[i] = GetDrawStringWidth(Music[number_ring(list_number + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1)].title[difficulty],wcslen(Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].title[difficulty]));
					int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

					int list_number_buf = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base_buf].index;

					int sn_buf = folder->folder[folder->selected_folder]
						[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number;//list_numberの情報から曲番号を算出


					int difficulty_buf = difficulty;

					if (folder->FolderKind[folder->selected_folder] == FOLDER_KIND_DIFFICULTY && (i != (Column / 2))) {//難易度別フォルダの時は選んでない曲は難易度固定で表示するためにループ毎にdifficultyの値をフォルダでの指定値に変更
						difficulty_buf= folder->folder[folder->selected_folder]
							[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].difficulty;//list_numberの情報から曲番号を算出
					}

					season_banner_buf[i] = Music[sn_buf].season[difficulty_buf];//季節のバナー飾りのための値をバッファに格納
					
					if (Music[sn_buf].secret != 1) {//隠し曲ではない、それか隠し曲でも解禁済みの曲のときは通常表示
						//タイトル
						title_buf[i] =
							Music[sn_buf].title[difficulty_buf];//曲名の先頭ポインタを配列に格納
						title_width[i] =
							GetDrawStringWidthToHandle(//文字長を取得
								Music[sn_buf].title[difficulty_buf], //タイトルを出す
								wcslen(Music[sn_buf].title[difficulty_buf]),//同じようにその文字数も引数として入れてやる
								FontHandle);
						title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//色を配列に格納

						title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//色を配列に格納
					}
					else {//隠し曲(secret=1)
						if (secret->song_appear_number == sn_buf) {//隠し曲出現対象になっている
							//タイトル
							title_buf[i] =
								Music[sn_buf].title[difficulty_buf];//曲名の先頭ポインタを配列に格納

							title_width[i] =
								GetDrawStringWidthToHandle(//文字長を取得
									Music[sn_buf].title[difficulty_buf], //タイトルを出す
									wcslen(Music[sn_buf].title[difficulty_buf]),//同じようにその文字数も引数として入れてやる
									FontHandle);
							title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//色を配列に格納
							title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//色を配列に格納
						}
						else {//隠し曲出現対象になっていない
							if (Music[sn_buf].exist[difficulty_buf] == 1) {//譜面が存在するなら
								
								title_buf[i] =
									secret_str;//曲名の先頭ポインタを配列に格納

								title_width[i] =
									GetDrawStringWidthToHandle(//文字長を取得
										secret_str, //タイトルを出す
										wcslen(secret_str),//同じようにその文字数も引数として入れてやる
										FontHandle);
								title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//色を配列に格納
								title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//色を配列に格納
							}
							else {
								//タイトル
								title_buf[i] =
									Music[sn_buf].title[difficulty_buf];//曲名の先頭ポインタを配列に格納

								title_width[i] =
									GetDrawStringWidthToHandle(//文字長を取得
										Music[sn_buf].title[difficulty_buf], //タイトルを出す
										wcslen(Music[sn_buf].title[difficulty_buf]),//同じようにその文字数も引数として入れてやる
										FontHandle);
								title_color[i] = Music[sn_buf].StrColor[difficulty_buf];//色を配列に格納
								title_shadow_color[i] = Music[sn_buf].StrShadowColor[difficulty_buf];//色を配列に格納
							}
						}


					}

					SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
					SetDrawScreen(H_TITLE_STR[i]);//曲名画像を描画対象に
					ClearDrawScreen();//前に描かれていた文字を消す
					//画像として一時的に描画
					ShowExtendedStrFitToHandle(320, 7,
						title_buf[i],
						title_width[i],
						620, config,
						FontHandle,
						title_color[i],
						title_shadow_color[i]);//曲名描画

					SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す
					SetDrawMode(DX_DRAWMODE_NEAREST);

				}

				//ジャンル、アーティストのwidthの計算
				if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら???にする
					genre_width = GetDrawStringWidth(secret_str, wcslen(secret_str));
					artist_width = GetDrawStringWidth(secret_str, wcslen(secret_str));
				}
				else {
					genre_width = GetDrawStringWidth(Music[song_number].genre[difficulty], wcslen(Music[song_number].genre[difficulty]));
					artist_width = GetDrawStringWidth(Music[song_number].artist[difficulty], wcslen(Music[song_number].artist[difficulty]));
				}
				widthCalcFlag = 0;



			}
		}

		//段位認定コース名幅の計算
		if (SelectingTarget == SELECTING_COURSE) {
			if (widthCalcFlag == 1) {//上下左右のキーを押して曲,難易度が変わっていたら
				for (i = 0; i <= Column; i++) {//Column/2が中心
											   //title_width[i] = GetDrawStringWidth(Music[number_ring(list_number + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1)].title[difficulty],wcslen(Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].title[difficulty]));
					int sn_buf = number_ring(list_number + (i - Column / 2), NUMBER_OF_COURSES - 1);//list_numberの情報から曲番号を算出
					
					title_buf[i] = STList->title[sn_buf];
					title_width[i] =
						GetDrawStringWidthToHandle(//文字長を取得
							title_buf[i], //タイトルを出す
							wcslen(title_buf[i]),//同じようにその文字数も引数として入れてやる
							FontHandle);
					title_color[i] = STList->Color[sn_buf];//色を配列に格納
					title_shadow_color[i] = STList->ShadowColor[sn_buf];//色を配列に格納
				

					SetDrawScreen(H_TITLE_STR[i]);//段位認定コース名画像を描画対象に
					ClearDrawScreen();//前に描かれていた文字を消す
									  //画像として一時的に描画
					ShowExtendedStrFitToHandle(320, 7,
						title_buf[i],
						title_width[i],
						620, config,
						FontHandle,
						title_color[i],
						title_shadow_color[i]);//曲名描画

					SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す


				}

				widthCalcFlag = 0;
			}
		}

		//フォルダ名幅の計算
		if (SelectingTarget == SELECTING_FOLDER) {
			if (widthCalcFlag == 1) {//上下左右のキーを押して曲,難易度が変わっていたら
				for (i = 0; i <= Column; i++) {//Column/2が中心
					folder_name_width[i] = GetDrawStringWidthToHandle(folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)],
						wcslen(folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)]),FontHandle);
					//printfDx(L"i:%d num:%d ring:%d\n",i, song_number + (i - 8), number_ring(song_number + (i - 8), NumberOfSongs-1));
				
					SetDrawScreen(H_TITLE_STR[i]);//フォルダ名画像を描画対象に
					ClearDrawScreen();//前に描かれていた文字を消す
									  //画像として一時的に描画
					
					ShowExtendedStrFitToHandle(320, 7, 
						folder->folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)],
						folder_name_width[i], 
						620, config,
						FontHandle);//フォルダ名表示

					SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す
				
				}
				widthCalcFlag = 0;
			}
		}

		show_bpm = (Music[song_number].bpmmax[difficulty] + Music[song_number].bpmmin[difficulty]) / 2 + (sin(0.002*GetNowCount_d(config)))*((double)Music[song_number].bpmmax[difficulty] - Music[song_number].bpmmin[difficulty]);//実際に表示するBPMを決定
		if (show_bpm >= Music[song_number].bpmmax[difficulty]) {
			show_bpm = Music[song_number].bpmmax[difficulty];
		}
		if (show_bpm <= Music[song_number].bpmmin[difficulty]) {
			show_bpm = Music[song_number].bpmmin[difficulty];
		}

		if (SelectingTarget == SELECTING_SONG) {
			if (ScoreShowMode == 0) {
				number_digit(int(Highscore[song_number].score[difficulty + select_rainbow] + 0.5), score_digit, 5);//ハイスコアを桁ごとに格納
			}
			else if (ScoreShowMode == 1) {
				number_digit(int(HighscoreRival[song_number].score[difficulty + select_rainbow] + 0.5), score_digit, 5);//ハイスコアを桁ごとに格納
			}
		}
		else if (SelectingTarget == SELECTING_FOLDER) {//ハイスコアを桁ごとに格納
			int R = option->op.color == OptionItem::Color::RAINBOW;
			number_digit(int(FolderScore[R][folder->selected_folder][difficulty].AverageScore + 0.5), score_digit, 5);
		}
		else if(SelectingTarget == SELECTING_COURSE) {//ハイスコアを桁ごとに格納
			number_digit(int(STscore[list_number] + 0.5), score_digit, 5);
		}

																								 //printfDx(L"%d\n",score[song_number][difficulty]);
		number_digit(int(show_bpm + 0.5), bpm_digit, 5);//BPMを桁ごとに格納
		

		if (SelectingTarget == SELECTING_COURSE) {//段位
			number_digit(int(STList->pop[list_number]), level_digit, 3);//レベルを桁ごとに格納
			number_digit(int(STList->bpmmin[list_number]), bpm_min_digit, 5);//最小BPMを桁ごとに格納
			number_digit(int(STList->bpmmax[list_number]), bpm_max_digit, 5);//最大BPMを桁ごとに格納
		}
		else {
			number_digit(int(Music[song_number].level[difficulty]), level_digit, 3);//レベルを桁ごとに格納
			number_digit(int(Music[song_number].bpmmin[difficulty]), bpm_min_digit, 5);//最小BPMを桁ごとに格納
			number_digit(int(Music[song_number].bpmmax[difficulty]), bpm_max_digit, 5);//最大BPMを桁ごとに格納
		}


		//printfDx(L"LOOP4:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

		//DRAW
		ClearDrawScreen();

		DrawGraph(0, 0, H_BG, TRUE);//背景表示


		show_cloud(H_CLOUD, &pos_cloud, 1, LOOP_passed_time);//隠し曲演出中なら白い雲表示


		if (CheckHandleASyncLoad(H_JACKET) == FALSE && CheckHandleASyncLoad(H_JACKET_GAUSS) == FALSE && jacket_show_counter == -1) {//普通とぼかし用どちらも読み込み完了してたらぼかし処理
			jacket_show_counter = -2;
			GetGraphSize(H_JACKET, &jacketSize.x, &jacketSize.y);//サイズを取得
			GetGraphSize(H_JACKET_GAUSS, &jacketGaussSize.x, &jacketGaussSize.y);//サイズを取得
																				 //GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_DOWN_SCALE, 4);
			GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_GAUSS, 16, 4000);



			if (jacketArea.x - jacketSize.x >= jacketArea.y - jacketSize.y) {//エリアに対して縦長
				jacketScale = (double)jacketArea.y / jacketSize.y;
				//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
			}
			else {//エリアに対して横長
				jacketScale = (double)jacketArea.x / jacketSize.x;
				//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
			}

			if (jacketArea.x - jacketGaussSize.x >= jacketArea.y - jacketGaussSize.y) {//ぼかしたジャケット画像がエリアに対して縦長
				jacketGaussScale = (double)jacketArea.x / jacketGaussSize.x;
				//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
			}
			else {//エリアに対して横長
				jacketGaussScale = (double)jacketArea.y / jacketGaussSize.y;
				//DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, TRUE);
			}

		}

		//printfDx(L"%d %d\n", CheckHandleASyncLoad(H_JACKET), jacket_show_counter);
		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで
		if (CheckHandleASyncLoad(H_JACKET) == FALSE && jacket_show_counter == -2) {//ぼかし完了してたら画像表示
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * jacket_alpha));

			//ぼかしたジャケットで隙間を埋める

			if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら表示しない
			  //表示しない
			}
			else {
				//ぼかしたジャケット
				DrawExtendGraph(int(center.x - jacketGaussScale*(jacketGaussSize.x / 2)-1),
					int(center.y - jacketGaussScale*(jacketGaussSize.y / 2)-1),
					int(center.x + jacketGaussScale*(jacketGaussSize.x / 2)+1),
					int(center.y + jacketGaussScale*(jacketGaussSize.y / 2)+1), H_JACKET_GAUSS, FALSE);
				//元のジャケット
				DrawExtendGraph(int(center.x - jacketScale*(jacketSize.x / 2)), int(center.y - jacketScale*(jacketSize.y / 2)), int(center.x + jacketScale*(jacketSize.x / 2)), int(center.y + jacketScale*(jacketSize.y / 2)), H_JACKET, FALSE);
			}
		}
		SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す

		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((flash) * 100));
		//jacket_show_counter == -1


		for (i = 0; i <= Column; i++) {//画面真ん中領域の「フォルダ名、曲名」の後ろに置くバナーの表示
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
			if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100 - int(50 * jacket_alpha));
			//SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(155 * jacket_alpha));
			int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

			int list_number_buf = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base_buf].index;


			if (SelectingTarget == SELECTING_SONG) {
				if (Music[folder->folder[folder->selected_folder][number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number].exist[difficulty]
					== 1//そこの譜面が存在するとき
						//Music[number_ring(song_number + (i - Column / 2), NumberOfSongs - 1)].exist[difficulty]
					) {
					DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK, TRUE);//バナー(存在する譜面)
				}
				else {
					DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK_NE, TRUE);//バナー(存在しない譜面)
				}
			}

			if (SelectingTarget == SELECTING_FOLDER) {
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_BACK, TRUE);//バナー
			}

			if (SelectingTarget == SELECTING_SONG) {
				if (season_banner_buf[i] != 0) {
					//無季節じゃないなら

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
					if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150 - int(75 * jacket_alpha));
					DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_SEASON[season_banner_buf[i] - 1], TRUE);
					//バナー(季節の模様)に飾りを付ける

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
					if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100 - int(50 * jacket_alpha));
				}
			}

			DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_FLAME, TRUE);//バナーフレーム

		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		//printfDx(L"LOOP5:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter) * c_m_draw_counter));
		DrawGraph(640 - 16 + int(((double)1 - button_draw_counter) * 288) + 32, 360 - 16, H_BUTTON_G, TRUE);//左真ん中Gボタン
		DrawGraph(640 - 16 + int(((double)button_draw_counter - 1) * 288) - 32, 360 - 16, H_BUTTON_G, TRUE);//右真ん中Gボタン
		if (Key[Button[1][2]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(640 - 16 + int(((double)1 - button_draw_counter) * 288) + 32, 360 - 16, H_BUTTON_PRESS, TRUE);//左真ん中Gボタン
		}
		if (Key[Button[1][1]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(640 - 16 + int(((double)button_draw_counter - 1) * 288) - 32, 360 - 16, H_BUTTON_PRESS, TRUE);//右真ん中Gボタン
		}

		//printfDx(L"LOOP5.1:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);



		//SetFontSize(28);

		if (SelectingTarget == SELECTING_SONG) {//曲名表示
			for (i = 0; i <= Column; i++) {
				int list_number_base_buf = number_ring(list_number_base + (i - Column / 2), folder->folder_c[folder->selected_folder] - 1);

				int list_number_buf = SortList[(int)option->op.sort][folder->selected_folder][option->op.color == OptionItem::Color::RAINBOW][difficulty - 1][list_number_base_buf].index;

				int sn_buf = folder->folder[folder->selected_folder]
					[number_ring(list_number_buf, folder->folder_c[folder->selected_folder] - 1)].song_number;//list_numberの情報から曲番号を算出


				if (i != Column / 2) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(220 * jacket_alpha));//選択してる曲名以外薄く表示
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
				//文字列を求める処理が重い
				//title_buf[i];曲名の先頭ポインタが配列に格納されている

				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[i], TRUE);
				/*
				ShowExtendedStrFitToHandle(640, int(7 + 336 + (bn_draw_counter + i - Column / 2) * 48), 
					title_buf[i] ,
					title_width[i],
					620,
					FontHandle,
					title_color[i], 
					title_shadow_color[i]);//曲名描画
				*/

				if ((secret->song_appear_number == sn_buf) && (Music[sn_buf].secret == 1)) {//隠し曲が出現対象になっている
					//Attack a Secret Song!の表示
					SetDrawBright(brightness, brightness, brightness);//選択枠点滅
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(20 * jacket_alpha));
					DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_BANNER_FLAME_SECRET, TRUE);//選択枠
					SetDrawBright(255, 255, 255);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}

			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		if (SelectingTarget == SELECTING_COURSE) {//段位認定コース名表示
			for (i = 0; i <= Column; i++) {
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[i], TRUE);		
			}
		}


		if (SelectingTarget == SELECTING_FOLDER) {//フォルダ名表示
			for (i = 0; i <= Column; i++) {
				DrawGraph(320, int(336 + (bn_draw_counter + i - Column / 2) * 48), H_TITLE_STR[i], TRUE);
			}
			/*
			for (i = 0; i <= Column; i++) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				if (i != Column / 2)SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(168 * jacket_alpha));

				ShowExtendedStrFitToHandle(640, int(7 + 336 + (bn_draw_counter + i - Column / 2) * 48), folder_name[number_ring(folder->selected_folder + (i - Column / 2), folder->NumberOfFolders - 1)], folder_name_width[i], 620, FontHandle);//フォルダ名表示
			}
			*/
		}

		//printfDx(L"LOOP5.3:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawGraph(320, 0, H_BANNER_UD, TRUE);//上下バナー
		
		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
		//ジャンル、アーティスト表示
		if (SelectingTarget == SELECTING_SONG) {
			if (Music[song_number].genre[difficulty][0] != 0) {//ジャンルが存在
				if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら???にする
					ShowExtendedStrFitToHandle(640, int(11 + 336 + (0 + 3 - 20 / 2) * 48), secret_str, genre_width, 500, config, FontHandle);//ジャンル表示「???」
				}
				else {
					ShowExtendedStrFitToHandle(640, int(11 + 336 + (0 + 3 - 20 / 2) * 48), Music[song_number].genre[difficulty], genre_width, 500, config, FontHandle);//ジャンル表示
				}
				DrawGraph(320, 0, H_BANNER_AREA, TRUE);//AREA文字表示
			}
			if (Music[song_number].artist[difficulty][0] != 0) {//アーティストが存在
				if (Music[song_number].secret == 1 && secret->song_appear_number != song_number) {//隠し曲なら???にする
					ShowExtendedStrFitToHandle(640, int(6 + 336 + (0 + 4 - 20 / 2) * 48), secret_str, artist_width, 480, config, FontHandle);//アーティスト表示「???」
				}
				else {
					ShowExtendedStrFitToHandle(640, int(6 + 336 + (0 + 4 - 20 / 2) * 48), Music[song_number].artist[difficulty], artist_width, 480, config, FontHandle);//アーティスト表示
				}
				DrawGraph(320, 0, H_BANNER_ARTIST, TRUE);//ARTIST文字表示
			}


			/*
			DrawStringToHandle(int(640 - ((double)genre_width / 2)) + 2, int(11 + 336 + (0 + 3 - 20 / 2) * 48 + 2), Music[song_number].genre[difficulty], GetColor(0, 0, 0), FontHandle);//ジャンル(影)
			DrawStringToHandle(int(640 - ((double)genre_width / 2)), int(11 + 336 + (0 + 3 - 20 / 2) * 48), Music[song_number].genre[difficulty], GetColor(255, 255, 255), FontHandle);//ジャンル

			DrawStringToHandle(int(640 - ((double)artist_width / 2)) + 2, int(6 + 336 + (0 + 4 - 20 / 2) * 48 + 2), Music[song_number].artist[difficulty], GetColor(0, 0, 0), FontHandle);//アーティスト(影)
			DrawStringToHandle(int(640 - ((double)artist_width / 2)), int(6 + 336 + (0 + 4 - 20 / 2) * 48), Music[song_number].artist[difficulty], GetColor(255, 255, 255), FontHandle);//アーティスト
		    */
		}
		else if (SelectingTarget == SELECTING_COURSE) {//段位認定用説明
			ShowExtendedStrFitToHandle(640, int(11 + 336 + (0 + 3 - 20 / 2) * 48), Des1, Des1_width, 640, config, FontHandle);//ジャンル表示
			ShowExtendedStrFitToHandle(640, int(6 + 336 + (0 + 4 - 20 / 2) * 48), Des2, Des2_width, 640, config, FontHandle);//アーティスト表示
		}
		SetDrawMode(DX_DRAWMODE_NEAREST);

		//printfDx(L"LOOP6:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);

		//printfDx(L"%s\n", Music[song_number].artist[difficulty]);

		



		//アナウンス表示
		if (SelectingTarget == SELECTING_FOLDER) {
			if (secret->song_appear_number != -1) {//隠し曲出現演出中
				show_str(GetNowCount(), Announse_show_time_base, H_ANNOUNSE, 960, 32, secret->Announce_width);

				//show_str(GetNowCount(), Announse_show_time_base, secret->Announce, 330 * 3, 32, secret->Announce_width, config, FontHandle, secret->Color, secret->ShadowColor);
			}
			else {//通常状態は通常アナウンス
				show_str(GetNowCount(), Announse_show_time_base, H_ANNOUNSE, 960, 32, AnnounceWidth);

				//show_str(GetNowCount(), Announse_show_time_base, StrAnnounce,      330 * 3, 32, AnnounceWidth, config, FontHandle, GetColor(255, 255, 255), GetColor(0, 0, 0));
			}
		}

		//実際のハイスピを決める
		if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
			option->op.speedVal = option->speedVal[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
		}
		else {
			option->op.speedVal = 1;
		}

		oi_counter = int(0.0004*GetNowCount_d(config)) % OPERATION_INSTRUCTION_NUMBER;//操作説明表示用のカウンタ
		if (OptionOpen == 1) {//オプション説明表示
			if (option_select == (int)OptionItem::Name::SPEED) {
				if ((SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) ||
					(SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2)) {//曲選択の時で譜面が存在するとき(段位認定でフォルダ選択に戻る意外)は上に表示しハイスピがかかった速さも表示
					wchar_t SpeedStr[128] = L"(0～0)";
					int SpeedStrWidth = 0;

					DrawGraph(320, 2 + 48 * 13, option->H_SENT, TRUE);
					//速さ表示
					if (SelectingTarget == SELECTING_SONG) {
						sprintfDx(SpeedStr, L"(%d～%d,瞬間風速%d)",
							int(Music[song_number].bpmmin[difficulty] * option->op.speedVal + 0.5),
							int(Music[song_number].bpmmax[difficulty] * option->op.speedVal + 0.5),
							int(Music[song_number].bpm_suggested[difficulty] * option->op.speedVal + 0.5));
					}
					else if (SelectingTarget == SELECTING_COURSE) {
						sprintfDx(SpeedStr, L"(%d～%d)",
							int(STList->bpmmin[list_number] * option->op.speedVal + 0.5),
							int(STList->bpmmax[list_number] * option->op.speedVal + 0.5));
					}
					SpeedStrWidth = GetDrawStringWidth(SpeedStr, wcslen(SpeedStr));
					ShowExtendedStrFitToHandle(640, 2 + 48 * 14, SpeedStr, SpeedStrWidth, 620, config, FontHandle);
				}
				else {//フォルダセレクトの時は真ん中に表示
					DrawGraph(320, 24 + 48 * 13, option->H_SENT, TRUE);
				}
				
				//show_str(GetNowCount_d(config), time_base_str, Option->sent_speed[Option->op.speed], 330 * 3, 655, width_sent_speed[Option->op.speed],FontHandle);
			}
			else{
				DrawGraph(320, 24 + 48 * 13, option->H_SENT, TRUE);

				//show_str(GetNowCount_d(config), time_base_str, Option->sent_gauge[Option->op.gauge], 330 * 3, 655, width_sent_gauge[Option->op.gauge],FontHandle);
			}


		}
		else {//操作説明表示
			if (oi_counter <= 4) {
				DrawGraph(329, 632, H_BANNER_D_BUTTON[oi_counter], TRUE);//コントローラ画像
			}
			else {
				DrawGraph(329, 632, H_BANNER_D_BUTTON[5], TRUE);//コントローラ画像
			}

			DrawGraph(320, 655, H_OI_STR[oi_counter], TRUE);

			//if (config.ShowStrShadow == TRUE)DrawStringToHandle(int(696 - ((double)width_ope_ins[oi_counter] / 2)) + 2, int(655 + 2), ope_ins[oi_counter], GetColor(0, 0, 0),FontHandle);//説明(影)
			//DrawStringToHandle(int(696 - ((double)width_ope_ins[oi_counter] / 2)), int(655), ope_ins[oi_counter], GetColor(255, 255, 255), FontHandle);//説明
		}



		DrawGraph(320, 0, H_BANNER_UD_SKIN, TRUE);//上下バナー(枠)

		SetDrawBright(brightness, brightness, brightness);//選択枠点滅
		if (OptionOpen == 0) {//選曲のモードのとき
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255 - int(200 * jacket_alpha));
			DrawGraph(320, 336, H_BANNER_SELECT, TRUE);//選択枠
		}
		SetDrawBright(255, 255, 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		float GraphColorV = 255;
		unsigned int GraphColorArray[9][3] = {//各色の音符個数バー表示用の色配列
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


		if (SelectingTarget != SELECTING_COURSE) {
			DrawGraph(0, 0, H_COVER[difficulty], TRUE);//カバー表示
			DrawGraph(960, 0, H_COVER[difficulty], TRUE);//右側

			//各色の音符密度グラフの描画
			if (SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_COURSE) {
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
				
				//虹のグラフ
				for (j = 0; j < BarWidth; j++) {
					DrawLine(11 + 960 + j + 8 * BarWidth, 720, 11 + 960 + j + 8 * BarWidth, int(720 - ShowColorNotesGraph[i]), GetRainbowColor((double)BarWidth-1 - j, BarWidth), 1);
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			
				//密度折れ線グラフの描画
				for (i = 0; i < 8; i++) {
					DrawLineAA(float(11 + 960 + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i]), float(11 + 960 + BarWidth + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i + 1]), GetColor(255,255,255), 3);
				}

				for (i = 0; i < 9; i++) {
					DrawCircleAA(float(11 + 960 + (i + 1) * BarWidth - 17), float(720 - ShowLocalNotesGraph[i]), float(4), 32, GetColor(255, 255, 255), TRUE);
				}
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				DrawGraph(960, 0, H_COVER_POP, TRUE);//右側の文字
			}

			
		}
		else {//段位認定カバー
			DrawGraph(0, 0, H_COVER_SKILL_TEST, TRUE);//カバー表示
			DrawGraph(960, 0, H_COVER_SKILL_TEST, TRUE);//右側
			DrawGraph(960, 0, H_COVER_SKILL_TEST_POP, TRUE);//右側の文字
		}


		DrawGraph(0, 0, H_COVER_HIGH_SCORE, TRUE);//左側の文字
		
		//レベル描画
		if (SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {
			DrawGraph(1180, 100, H_PERCENT, TRUE);
			for (i = 0; i <= int(log10(Music[song_number].level[difficulty])); i++) {
				DrawGraph(10 + 1115 - i * 40, 100, H_SCORE_NUMBER[level_digit[i]], TRUE);
			}
		}
		else if (SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2) {//段位難易度
			DrawGraph(1145, 100, H_PERCENT, TRUE);
			for (i = 0; i <= int(log10(STList->pop[list_number])); i++) {
				DrawGraph(10 + 1040 - i * 40, 100, H_SCORE_NUMBER[level_digit[i]], TRUE);
			}
		}

		//難易度画像描画
		//printfDx(L"%d\n", H_DIFFICULTY[difficulty]);
		if (SelectingTarget == SELECTING_SONG) {
			if (Music[song_number].exist[difficulty] == 1) {//存在するとき
				if (Music[song_number].season[difficulty] != 4) {//冬じゃないとき
					DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty], TRUE);//譜面難易度


					//DrawGraph(1020, 260, H_DIFFICULTY[difficulty], TRUE);
				}
				else {//冬のとき
					if (difficulty <= 2) {//晴れ曇りはそのまま
						DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty], TRUE);//譜面難易度

					}
					if (difficulty >= 3 && difficulty <= 4) {//雪 吹雪
						DrawExtendGraph(970, 112, 970 + 80, 112 + 80, H_DIFFICULTY[difficulty + 2], TRUE);//譜面難易度

					}
				}
			}
		}

		//スコア描画

		int R_ShowFlag = 0;//R表示フラグ
		if ((SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_FOLDER )&& option->op.color == OptionItem::Color::RAINBOW) {
			R_ShowFlag = 1;
		}
		else if(SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] == 0){
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
		
		//R表示
		if (R_ShowFlag) {//曲選択
			DrawGraph(0, 97, H_R_OUT, TRUE);
			DrawGraph(0, 97, H_R_IN, TRUE);
		}

		
		
		//BPM, VERSION描画
		//枠の表示
		if (SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {
			
			//BPM用表示枠
			DrawBoxWithLine(976, 200, 1110, 240, GetColor(50, 50, 255));
			DrawBoxWithLine(1130, 200, 1264, 240, GetColor(255, 50, 50));

			//バージョン用表示枠
			DrawBoxWithLine(976, 250, 1110, 290, GetColor(50, 255, 50));

			//最大同時押し数用表示枠
			DrawBoxWithLine(1130, 250, 1264, 290, GetColor(255, 255, 255));

			DrawGraph(960, 200, H_BPM_MINMAX_STR, TRUE);
			DrawGraph(960, 250, H_VER_MAX_CHORDS_STR, TRUE);
		}
		else if (SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2) {//段位
			DrawBoxWithLine(976, 200, 1110, 240, GetColor(50, 50, 255));
			DrawBoxWithLine(1130, 200, 1264, 240, GetColor(255, 50, 50));

			DrawGraph(960, 200, H_BPM_MINMAX_STR, TRUE);
		}

		//BPM, VERSION数値描画
		if (SelectingTarget == SELECTING_SONG && Music[song_number].exist[difficulty] == 1) {

			DrawNumber(1062, 196, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_BPM_NUMBER_MIN);
			DrawNumber(1216, 196, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_BPM_NUMBER_MAX);
			DrawFloatNumber(1090, 246, Music[song_number].version[difficulty], 20, 3, 0.5, H_VERSION_NUMBER, H_VERSION_DECIMAL);

			DrawNumber(1235, 246, Music[song_number].maxChords[option->op.color == OptionItem::Color::RAINBOW][difficulty], 25, 0, 0, H_MAX_CHORDS_NUMBER);


			/*
			cash = int(log10(Music[song_number].bpmmax[difficulty]));//桁を格納
			for (i = 0; i <= cash; i++) {//BPM最大値
				DrawGraph(1050 - i * 25 + 12.5 * cash, 196, H_BPM_NUMBER_MAX[bpm_max_digit[i]], TRUE);
			}
			cash = int(log10(Music[song_number].bpmmin[difficulty]));//桁を格納
			for (i = 0; i <= cash; i++) {//BPM最小値
				DrawGraph(1204 - i * 25 + 12.5 * cash, 196, H_BPM_NUMBER_MIN[bpm_min_digit[i]], TRUE);

			}
			*/

			//DrawGraph(960, 520, H_BPM_SLASH, TRUE);// /の表示
		}
		else if(SelectingTarget == SELECTING_COURSE && STList->Kind[list_number] != 2){//段位
			DrawNumber(1062, 196, STList->bpmmin[list_number], 25, 0, 0, H_BPM_NUMBER_MIN);
			DrawNumber(1216, 196, STList->bpmmax[list_number], 25, 0, 0, H_BPM_NUMBER_MAX);
			//DrawFloatNumber(1228, 246, STList->version, 25, 3, 1, H_VERSION_NUMBER, H_VERSION_DECIMAL);

			/*
			cash = int(log10(STList->bpmmax[list_number]));//桁を格納
			for (i = 0; i <= cash; i++) {//BPM最大値
				DrawGraph(1045 - i * 25 + 12.5 * cash, 196, H_BPM_NUMBER_MAX[bpm_max_digit[i]], TRUE);
			}
			cash = int(log10(STList->bpmmin[list_number]));//桁を格納
			for (i = 0; i <= cash; i++) {//BPM最小値
				DrawGraph(1209 - i * 25 + 12.5 * cash, 196, H_BPM_NUMBER_MIN[bpm_min_digit[i]], TRUE);

			}
			*/
			//DrawGraph(960, 520, H_BPM_SLASH, TRUE);// /の表示
		}

		/*
		if (SelectingTarget == SELECTING_SONG) {
			for (i = 0; i <= int(log10(show_bpm)); i++) {//BPM
				DrawGraph(1130 - i * 40, 570, H_SCORE_NUMBER[bpm_digit[i]], TRUE);

			}
		}
		*/

		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く

		if (SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_FOLDER || SelectingTarget == SELECTING_COURSE) {
			int RankBuf = 0;
			int ClearStateBuf = 0;
			int PlayCountBuf = 0;
			int ExistBuf = Music[song_number].exist[difficulty];

			if (SelectingTarget == SELECTING_SONG) {
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
			else if (SelectingTarget == SELECTING_COURSE) {
				RankBuf = STrank[list_number];
				ClearStateBuf = STclear_state[list_number];
				PlayCountBuf = STplay_count[list_number];
				ExistBuf = 1;
			}
			else if (SelectingTarget == SELECTING_FOLDER) {
				RankBuf = FolderScore[option->op.color == OptionItem::Color::RAINBOW][folder->selected_folder][difficulty].folderRank;
				ClearStateBuf = FolderScore[option->op.color == OptionItem::Color::RAINBOW][folder->selected_folder][difficulty].ClearType;
				PlayCountBuf = 1;
				ExistBuf = 1;
			}

			//ランク描画
			if (RankBuf != 0) {
				int RankSize = 210;
				DrawExtendGraph(7+50, 460 + 50, 7+RankSize + 50, 460+ RankSize + 50, H_RANK[RankBuf], TRUE);

				//DrawExtendGraph(0 + 50, 200 + 50, 220 + 50, 420 + 50, H_RANK[RankBuf], TRUE);
			}
			int PlayStateY = 650;
			float PlayStateRatioG = 0.75;
			float PlayStateRatio = (float)0.8 * PlayStateRatioG;
			float PlayStateRatioF = (float)0.66* PlayStateRatioG;
			int PlayStatex = -30;

			//プレイ状態描画
			if (RankBuf != 0 && ClearStateBuf == CLEARTYPE_FAILED && ExistBuf == 1) {//FAILED,不合格状態
				if (SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_FOLDER) {
					DrawExtendGraph(PlayStatex-5, PlayStateY, int(PlayStatex-5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_FAILED, TRUE);
				}
				else if(SelectingTarget == SELECTING_COURSE){
					DrawExtendGraph(PlayStatex+5, PlayStateY, int(PlayStatex+5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_SKILL_TEST_FAILED, TRUE);
				}
			}
			if (ClearStateBuf == CLEARTYPE_PLAY && ExistBuf == 1) {//PLAY状態(何も表示しない)
				//DrawExtendGraph(-95, 580, int(-95 + ((double)640 * 0.8)), int(580 + ((double)100 * 0.8)), H_CLEARED_EASY, TRUE);
			}
			if (ClearStateBuf == CLEARTYPE_EASY_CLEARED && ExistBuf == 1) {//EASY_CLEARED,合格状態
				if (SelectingTarget == SELECTING_SONG) {
					DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_EASY, TRUE);
				}
				else if (SelectingTarget == SELECTING_COURSE) {
					DrawExtendGraph(PlayStatex+5, PlayStateY, int(PlayStatex+5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_SKILL_TEST_PASSED, TRUE);
				}
			}
			if (ClearStateBuf == CLEARTYPE_CLEARED && ExistBuf == 1) {//NORMAL_CLEARED状態
				DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_NORMAL, TRUE);
			}
			if (ClearStateBuf == CLEARTYPE_HARD_CLEARED && ExistBuf == 1) {//HARD_CLEARED状態
				DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_HARD, TRUE);
			}
			if (ClearStateBuf == CLEARTYPE_SUPER_HARD_CLEARED && ExistBuf == 1) {//SUPER_HARD_CLEARED状態
				DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_CLEARED_SUPER_HARD, TRUE);
			}
			if (ClearStateBuf == CLEARTYPE_FULL_COMBO && ExistBuf == 1) {//FULL_COMBO状態
				DrawExtendGraph(PlayStatex+30, PlayStateY+10, int(PlayStatex+30 + ((double)640 * PlayStateRatioF)), int(PlayStateY+10 + ((double)100 * PlayStateRatioF)), H_FULL_COMBO[(int(GAME_passed_time) / 70) % 6], TRUE);
			}

			if (ClearStateBuf == CLEARTYPE_PERFECT && ExistBuf == 1) {//PERFECT_FULLCOMBO状態
				DrawExtendGraph(PlayStatex, PlayStateY, int(PlayStatex + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_PFC, TRUE);
			}

			if (ClearStateBuf == CLEARTYPE_NO_PLAY && ExistBuf == 1) {//no play状態
				DrawExtendGraph(PlayStatex-5, PlayStateY, int(PlayStatex-5 + ((double)640 * PlayStateRatio)), int(PlayStateY + ((double)100 * PlayStateRatio)), H_NO_PLAY, TRUE);
			}

		}
		SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す



		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		if (flag == FLAG_OPENING_STATE) {//開ける
			DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			if (c_m_draw_counter > 1) {
				if (SelectingTarget == SELECTING_SONG) {
					//開いたら曲再生
					SetCurrentPositionSoundMem(int(((double)Music[song_number].demostart[difficulty] / 1000) * 44100), SH_SONG);
					if ((Music[song_number].secret == 1 && secret->song_appear_number != song_number) || BGM_continue == 1) {//隠し曲,専用BGM再生中なら鳴らさない
						//鳴らさない
					}
					else {
						PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
						song_play_counter = -2;
					}
				}
				c_m_draw_counter = 1;
				flag = FLAG_SELECT_STATE;
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



		if (flag == FLAG_CLOSING_STATE) {//閉める
			DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			if (c_m_draw_counter < 0) {
				PlaySoundMem(SH_CLOSED, DX_PLAYTYPE_BACK, TRUE);
				flag = FLAG_END_FUNCTION_STATE;
				CoverClosedTime = (int)GAME_passed_time;
			}
			for (i = 0; i < CRTBuf; i++) {
				if (c_m_draw_counter >= 0) {
					c_m_draw_counter -= 0.001;
				}
			}

		}

		if (flag == FLAG_END_FUNCTION_STATE) {//閉まった後
			DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
		}

		//譜面レーダーの数値表示
		if (SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_FOLDER) {
			int RaderNumberInterval = 15;
			DrawNumber(160, 200, int(DRShowGlobal + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
			DrawNumber(288, 257, int(DRShowLocal + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
			DrawNumber(288, 445, int(DRShowChain + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
			DrawNumber(160, 502, int(DRShowUnstability + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
			DrawNumber(34, 445, int(DRShowStreak + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
			DrawNumber(34, 257, int(DRShowColor + 0.5), RaderNumberInterval, 0, 3, H_RADER_NUMBER);
		}


		//譜面レーダーの回転する線表示
		DrawRadarLine(GAME_passed_time / 1000);

		//譜面レーダー描画
		if (SelectingTarget == SELECTING_SONG || SelectingTarget == SELECTING_FOLDER) {
			//if (Music[song_number].exist[difficulty] == 1) {//存在するとき
				DrawHexagon(
					(short)(DRShowGlobal + 0.5),
					(short)(DRShowLocal + 0.5),
					(short)(DRShowChain + 0.5),
					(short)(DRShowUnstability + 0.5),
					(short)(DRShowStreak + 0.5),
					(short)(DRShowColor + 0.5));
			//}
		}
		
		int OptionY_Base = 150;
		int dist = 90;
		//オプション描画
		cache = int(cos((3.14159265) / 2 * (option_draw_counter - 1)) * 320 - 320);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawGraph(cache, 0, H_COVER_OPTION, TRUE);
		DrawGraph(cache, 0, H_OPTION, TRUE);//オプションカバー
		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
		for (i = 0; i < OptionShowAmount; i++) {
			DrawExtendGraph(cache + 35, OptionY_Base + i * dist, cache + 280, OptionY_Base + 53 + i * dist, H_BANNER_BACK, TRUE);
			DrawExtendGraph(cache + 35, OptionY_Base + i * dist, cache + 280, OptionY_Base + 53 + i * dist, H_BANNER_FLAME, TRUE);
		}
		

		SetDrawBright(brightness, brightness, brightness);
		DrawExtendGraph(cache + 35, OptionY_Base + (option_select - OptionShowStart) * dist, cache + 280, OptionY_Base + 53 + (option_select - OptionShowStart) * dist, H_BANNER_SELECT, TRUE);//選択枠
		SetDrawBright(255, 255, 255);
		SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す


		//オプション種類名称描画
		
		int index = 0;
		for (i = 0, index = OptionShowStart; i < OptionShowAmount; i++, index++) {
			cache2 = GetDrawStringWidth(option->OptionName[index], wcslen(option->OptionName[index]));
			if (config.ShowStrShadow == TRUE)DrawString(cache + 160 - (cache2 / 2) + 2, OptionY_Base - 33 + i * dist + 2, option->OptionName[index], GetColor(0, 0, 0));
			DrawString(cache + 160 - (cache2 / 2), OptionY_Base - 33 + i * dist, option->OptionName[index], GetColor(255, 128, 0));
		}


		//オプション名称描画
		for (i = 0, index = OptionShowStart; i < OptionShowAmount; i++, index++) {
			wchar_t* OptionStrAddress = option->ArrayOptionKindName[index][*(option->ArrayValue[index])];//i番目のオプションで現在選んでいる名称を表す文字列の先頭アドレス

			cache2 = GetDrawStringWidth(OptionStrAddress, wcslen(OptionStrAddress));
			if (config.ShowStrShadow == TRUE)DrawString(cache + 160 - (cache2 / 2) + 2, OptionY_Base + 9 + i * dist + 2, OptionStrAddress, GetColor(0, 0, 0));
			DrawString(cache + 160 - (cache2 / 2), OptionY_Base + 9 + i * dist, OptionStrAddress, GetColor(255, 255, 255));
		}

		if (SelectingTarget == SELECTING_COURSE) {//段位選択中は使えないオプションを暗くする
			SetDrawBright(0, 0, 0);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 150);
			int shadowLocateIndex;
			shadowLocateIndex = 1 - OptionShowStart;//GAUGE
			if (shadowLocateIndex >= 0)DrawExtendGraph(cache + 35, OptionY_Base + shadowLocateIndex * dist, cache + 280, OptionY_Base + 53 + shadowLocateIndex * dist, H_BANNER_BACK, TRUE);
			shadowLocateIndex = 2 - OptionShowStart;//LANE
			if (shadowLocateIndex >= 0 && 
				!((option->op.lane == OptionItem::Lane::NONE) || (option->op.lane == OptionItem::Lane::MIRROR)))DrawExtendGraph(cache + 35, OptionY_Base + shadowLocateIndex * dist, cache + 280, OptionY_Base + 53 + shadowLocateIndex * dist, H_BANNER_BACK, TRUE);
			shadowLocateIndex = 3 - OptionShowStart;//COLOR
			if (shadowLocateIndex >= 0)DrawExtendGraph(cache + 35, OptionY_Base + shadowLocateIndex * dist, cache + 280, OptionY_Base + 53 + shadowLocateIndex * dist, H_BANNER_BACK, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			SetDrawBright(255, 255, 255);
		}									

		//NOTEを選択しているときはプレビュー画像を表示
		if (OptionOpen == 1 && option_select == (int)OptionItem::Name::NOTE) {
			SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
			DrawExtendGraph(332, 632, 412, 712, H_OPTION_NOTE_PREVIEW[0], TRUE);
			DrawExtendGraph(868, 632, 948, 712, H_OPTION_NOTE_PREVIEW[1], TRUE);
			SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す
		}

		//オプション欄の上下にカーソルを表示
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * sin(3.1415 * button_draw_counter) * c_m_draw_counter));
		if (OptionShowEnd != option->OPTION_NUM - 1) {//表示範囲の下にまだオプションがあるとき↓のカーソル表示
			DrawGraph(cache, 655 - int(((double)1 - button_draw_counter) * 10), H_CURSOR, TRUE);
		}
		if (OptionShowStart != 0) {//表示範囲の上にまだオプションがあるとき↑のカーソル表示
			DrawReverseGraph(cache, 92 + int(((double)1 - button_draw_counter) * 10), H_CURSOR, TRUE, FALSE, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		cache = int(cos((3.14159265) / 2 * ((double)1 - result_draw_counter)) * (-320) + 1280);//リザルトカバー

		DrawGraph(cache, 0, H_COVER_OPTION, TRUE);
		if ((SelectingTarget == SELECTING_SONG) || (SelectingTarget == SELECTING_COURSE))DrawGraph(cache, 0, H_RESULT, TRUE);//リザルトカバー


		if ((Music[song_number].exist[difficulty] == 1 && SelectingTarget == SELECTING_SONG) || SelectingTarget == SELECTING_COURSE) {//曲選択状態で譜面が存在するときか、段位選択のとき
			int SkyPerfectBuf = 0;
			int PerfectBuf = 0;
			int GoodBuf = 0;
			int MissBuf = 0;
			int MinMissBuf = 0;
			int MaxComboBuf = 0;
			int PlayCountBuf = 0;

			if (SelectingTarget == SELECTING_SONG) {
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
			else if (SelectingTarget == SELECTING_COURSE) {
				SkyPerfectBuf = STsky_perfect[list_number];
				PerfectBuf = STperfect[list_number];
				GoodBuf = STgood[list_number];
				MissBuf = STmiss[list_number];
				MinMissBuf = STmin_miss[list_number];
				MaxComboBuf = STmax_combo[list_number];
				PlayCountBuf = STplay_count[list_number];
			}

			//数値描画
			DrawNumber((cache - 960) + 1212, 210, SkyPerfectBuf, 25, 1, 0, H_JUDGE_NUMBER);
			DrawNumber((cache - 960) + 1212, 262, PerfectBuf, 25, 1, 0, H_JUDGE_NUMBER);
			DrawNumber((cache - 960) + 1212, 314, GoodBuf, 25, 1, 0, H_JUDGE_NUMBER);
			DrawNumber((cache - 960) + 1212, 367, MissBuf, 25, 1, 0, H_JUDGE_NUMBER);

			if (MinMissBuf != -1) {//MinMissが保存されているとき
				DrawNumber((cache - 960) + 1221, 499, MinMissBuf, 25, 1, 4, H_MIN_MISS_NUMBER);
			}

			DrawNumber((cache - 960) + 1221, 557, MaxComboBuf, 25, 1, 4, H_MAX_COMBO_NUMBER);
			DrawNumber((cache - 960) + 1221, 615, PlayCountBuf, 25, 1, 4, H_PLAY_COUNT_NUMBER);
		}
		else if (SelectingTarget == SELECTING_FOLDER){
			int boxHeight = 40;
			//枠の表示

			//フォルダ記録
			//クリア状態
			DrawBoxWithLine((cache - 960) + 976, 130, (cache - 960) + 1110, 130 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 170, (cache - 960) + 1110, 170 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 210, (cache - 960) + 1110, 210 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 250, (cache - 960) + 1110, 250 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 290, (cache - 960) + 1110, 290 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 330, (cache - 960) + 1110, 330 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 370, (cache - 960) + 1110, 370 + boxHeight, GetColor(50, 50, 50));

			//ランク
			DrawBoxWithLine((cache - 960) + 1130, 130, (cache - 960) + 1264, 130 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 170, (cache - 960) + 1264, 170 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 210, (cache - 960) + 1264, 210 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 250, (cache - 960) + 1264, 250 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 290, (cache - 960) + 1264, 290 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 330, (cache - 960) + 1264, 330 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 1130, 370, (cache - 960) + 1264, 370 + boxHeight, GetColor(50, 50, 50));


			DrawBoxWithLine((cache - 960) + 976, 410, (cache - 960) + 1110, 410 + boxHeight, GetColor(50, 50, 50));;//PLAY
			DrawBoxWithLine((cache - 960) + 976, 450, (cache - 960) + 1264, 450 + boxHeight, GetColor(50, 50, 50));//NO PLAY

			//トータル記録
			DrawBoxWithLine((cache - 960) + 976, 540, (cache - 960) + 1264, 540 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 580, (cache - 960) + 1264, 580 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 620, (cache - 960) + 1264, 620 + boxHeight, GetColor(50, 50, 50));
			DrawBoxWithLine((cache - 960) + 976, 660, (cache - 960) + 1264, 660 + boxHeight, GetColor(50, 50, 50));

			//文字
			DrawGraph((cache - 960), 0, H_CPVER_RESULT_STR, TRUE);
			//DrawGraph((cache - 960) + 960, 250, H_VERSION_STR, TRUE);


			//数値描画

			//フォルダ記録
			//クリア状態
			int R = 0;
			if (option->op.color == OptionItem::Color::RAINBOW)R = 1;

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


			//ランク
			DrawNumber((cache - 960) + 1246, 130 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_S], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 170 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_A], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 210 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_B], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 250 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_C], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 290 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_D], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 330 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_E], 25, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1246, 370 - 4, FolderScore[R][folder->selected_folder][difficulty].rank[RANK_F], 25, 1, 0, H_PLAY_COUNT_NUMBER);



			//トータル記録
			DrawNumber((cache - 960) + 1248, 536, saveData.totalBootCount, 22, 1, 0, H_PLAY_COUNT_NUMBER);
			DrawNumber((cache - 960) + 1248, 576, saveData.totalPlayCount, 22, 1, 0, H_PLAY_COUNT_NUMBER);
			if (option->op.color == OptionItem::Color::RAINBOW) {
				DrawNumber((cache - 960) + 1248, 616, saveData.totalHighScoreRainbow, 22, 1, 0, H_PLAY_COUNT_NUMBER);
			}
			else {
				DrawNumber((cache - 960) + 1248, 616, saveData.totalHighScore, 22, 1, 0, H_PLAY_COUNT_NUMBER);

			}
			DrawNumber((cache - 960) + 1248, 656, saveData.totalHitNotes, 22, 1, 0, H_PLAY_COUNT_NUMBER);


		}



		//キー操作指示表示
		for (i = 1; i <= 2; i++) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(0 + 90 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_B, TRUE);//Bボタン
			if (Key[Button[0][i]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
				DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(0 + 90 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_PRESS, TRUE);//Bボタン
			}

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(688 - 90 - 7 + ((double)button_draw_counter - 1) * 32), H_BUTTON_R, TRUE);//Rボタン
			if (Key[Button[2][i]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
				DrawGraph(int(640 + ((double)i - 1.5) * 128 * 4) - 16, int(688 - 90 - 7 + ((double)button_draw_counter - 1) * 32), H_BUTTON_PRESS, TRUE);//Rボタン
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
		DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 7 + 1, H_BUTTON_B, TRUE);//左Bボタン
		if (Key[Button[0][0]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 7 + 1, H_BUTTON_PRESS, TRUE);//左Bボタン
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
		DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 7, H_BUTTON_B, TRUE);//右Bボタン
		if (Key[Button[0][3]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 7, H_BUTTON_PRESS, TRUE);//右Bボタン
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
		DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 681, H_BUTTON_R, TRUE);//左Rボタン
		if (Key[Button[2][0]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(320 - 16 - (button_draw_counter) * 320), 681, H_BUTTON_PRESS, TRUE);//左Rボタン
		}


		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
		DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 681, H_BUTTON_R, TRUE);//右Rボタン
		if (Key[Button[2][3]] >= 1) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(int(960 - 16 + (button_draw_counter) * 320), 681, H_BUTTON_PRESS, TRUE);//右Rボタン
		}



		//DrawGraph(int(960 + ((double)i - 1.5) * 128) - 16, int(0 + 7 + ((double)1 - button_draw_counter) * 32), H_BUTTON_B, TRUE);//右Bボタン


		if (difficulty >= 2) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(320 - 16 + int(((double)1 - button_draw_counter) * 32) - 32, 360 - 16, H_BUTTON_G, TRUE);//左Gボタン
			if (Key[Button[1][0]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
				DrawGraph(320 - 16 + int(((double)1 - button_draw_counter) * 32) - 32, 360 - 16, H_BUTTON_PRESS, TRUE);//左Gボタン
			}
		}
		if (difficulty <= 3) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(140 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
			DrawGraph(960 - 16 + int(((double)button_draw_counter - 1) * 32) + 32, 360 - 16, H_BUTTON_G, TRUE);//右Gボタン
			if (Key[Button[1][3]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(230 * sin(3.1415*button_draw_counter)  * c_m_draw_counter));
				DrawGraph(960 - 16 + int(((double)button_draw_counter - 1) * 32) + 32, 360 - 16, H_BUTTON_PRESS, TRUE);//右Gボタン
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		if (flag == FLAG_END_FUNCTION_STATE) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * (GAME_passed_time - CoverClosedTime) / 1200));
			DrawGraph(0, 0, H_DARKNESS, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}
		/*
		if (debug == 1) {
		SetFontSize(28);
		DrawString(int(120 - ((double)150 / 2)) + 2, 690 + 2, "DEBUG MODE", GetColor(0, 0, 0));//デバッグ表示(影)
		DrawString(int(120 - ((double)150 / 2)), 690, "DEBUG MODE", GetColor(255, 255, 255));//デバッグ表示
		}
		*/

		//printfDx(L"LOOP7:%d\n", GetNowCount_d(config) - GAME_start_time - time_cash);



		if (*debug == 1 && config.ShowDebug == 1) {
			printfDx(L"DEBUG MODE\n");
		}
		if (ScoreShowMode == 1) {
			printfDx(L"RIVAL SCORE表示\n");
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

		//printfDx(L"瞬間BPM:%d\n", Music[song_number].bpm_suggested[difficulty]);

		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000 / config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		//Sleep(7);
		ScreenFlip();
		clsDx();
	}


}


wchar_t * announce_str(int StageCount, int PlayCount) {//通常アナウンスを返す
	//PlayCountが0,1,2のときは
	static wchar_t str[300] = L" ";

	if (PlayCount == 0) {
		return L"【はじめてお越しの方へ】　「上下キー」と「Enterキー」でフォルダを選び、曲を選んで下さい。「左右キー」で難易度変更が行えます。最初は降水確率の小さい晴れた曲がお勧めです。";
	}

	if (PlayCount == 1) {
		return L"初めてのプレイお疲れ様でした。赤緑青のキー配置には慣れましたか？　実はこれらのキーだけで下の図のように、このシステムの操作をすることができます。オプションを開くと様々な設定が行えます。それでは次の曲を選んでみましょう。";
	}

	if (PlayCount == 2) {
		return L"プレイお疲れ様でした。難易度は適切でしたか？　色を判別するのが難しい方はオプションを開き、COLORをRAINBOWにすることをお勧めします。　これで案内は以上となります。　どうぞごゆっくりお過ごしください。";
	}

	//チュートリアル以後

	time_t now;//現在時刻(グリニッジ標準時)
	struct tm tm_now;//時間構造体
	errno_t error = 0;

	now = time(NULL);//時刻取得
	error = _localtime64_s(&tm_now, &now);

	if (StageCount == 0) {//起動時
		wchar_t *Greeting[3] = { L"おはようございます。",L"こんにちは。",L"こんばんは。" };
		wchar_t DayOfWeek[7][3] = { L"日", L"月",L"火",L"水",L"木",L"金",L"土" };

		int GreetingUse = 0;//どのあいさつを使うか

		if (tm_now.tm_hour >= 5 && tm_now.tm_hour < 9) {//5時～9時までは「おはようございます」
			GreetingUse = 0;
		}
		else if (tm_now.tm_hour >= 9 && tm_now.tm_hour < 17) {//9時～17時までは「こんにちは」
			GreetingUse = 1;
		}
		else {//それ以外(17～5時)は「こんばんは」
			GreetingUse = 2;
		}

		sprintfDx(str, L"%d月%d日%s曜日　%sこちらはnature prhysm自然管理センターです。本日もご来館いただきありがとうございます。ごゆっくりお過ごしください。",
			tm_now.tm_mon + 1, tm_now.tm_mday, DayOfWeek[tm_now.tm_wday], Greeting[GreetingUse]);

		return str;
	}

	const int GeneralStrNum = 23;
	wchar_t *GeneralStr[GeneralStrNum] = {
	L"【ヒント:スピード調節】音符が詰まりすぎて見づらいときは、オプションのSPEEDを変更し見やすい速さにしてみましょう。",
	L"【ヒント:音符の種類】音符の種類は「赤,緑,青,虹」の他に「水色,紫,黄色」や「白」があります。　それぞれ曇り、雨難易度以上から出現します。　光の色を混ぜて対処しましょう。　また、叩いてはいけない黒い音符もあります。",
	L"【ヒント:判定の種類】判定の種類は4つあり、MISS,GOOD,PERFECT,SKY PERFECTがあります。　タイミングよく叩いて良い判定を多くとりハイスコアを目指しましょう。",
	L"【ヒント:点数とランク】スコアに応じて曲にはランクが付きます。　F:5000点未満　E:5000点以上　D:6000点以上　C:7000点以上　B:8000点以上　A:9000点以上　S:9500点以上です。",
	L"【ヒント:ゲージの種類】オプションでゲージの種類を選ぶことができます。　より厳しいゲージでのクリアを目指しましょう。　練習にはゲージの減ることが無いNO FAILゲージがおすすめです。",
	L"【ヒント:ゲージの増減】ミスが続いてゲージが無くなると失敗になりますが、良い判定を取るとゲージは増えていきます。　光っている音符は増減量が２倍なので叩き逃さないようにしましょう。",
	L"【ヒント:色の覚え方】音符の色と叩く場所は自然風景と対応させて覚えると良いでしょう。　例えば、青は上に広がる空の色……のように。",
	L"【ヒント:速度変化】曲の中には途中で速度が変化するものがあります。　右上に予測値が表示されていますが稀に予報が外れる場合があります。　そのため常に注意を払ってください。",
	L"【ヒント:未調査エリア】「???」と表示されている項目は未調査エリアです。　危険なエリアのため立ち入りは禁じられています。　万が一調査する場合は厳重に警戒してください。　安全が確認された場合、エリアは解放されます。",
	L"【ヒント:上達のコツ】自分の実力より少し上の難易度をプレイし続けることが上達への近道です。",
	L"【ヒント:RAINBOWオプション】色を認識するのが難しい場合は、オプションのCOLORをRAINBOWにしてみましょう。　色を気にせずプレイすることができます。",
	L"【ヒント:クイックリトライ】クリアに失敗してしまったときは赤と青を押し続けてみましょう。　リザルト画面に移動せずすぐにリトライすることができます。",
	L"【ヒント:自然管理技術者検定】自然管理技術者検定会場では自分の実力を測ることができます。　より高いレベルを目指しましょう。",
	L"【ヒント:瞬間風速】瞬間風速は音符群の最大の速さの予測値です。　オプションのSPEEDを変更する際には瞬間風速の値も気にしてみましょう。",
	L"【ヒント:気象レーダー(1)】曲を選ぶとき、左の気象レーダーには譜面の傾向が示されます。　Global:平均密度 Local:最大局所密度 Chain:縦連打度 Unstability:速度変化度 Streak:ロングノーツ度 Color:色の複雑さ を表しています。",
	L"【ヒント:気象レーダー(2)】曲フォルダを選ぶとき、左の気象レーダーにはそのフォルダの傾向毎の達成度が示されます。　全項目100を目指してみましょう。",
	L"【ヒント:黒終端ロングノート】終端が黒いロングノートは終端でしっかり離さないとMISSになります。　押しっぱなしにしないように注意しましょう。",
	L"【ヒント:雨温図】右の色の付いた棒グラフは各色の音符密度を示しています。　白い折れ線グラフは音符密度の時間推移を表しています。",
	L"【ヒント:NIGHTオプション】音符の色が見づらいときはオプションのNIGHTを変更して背景を暗くしてみましょう。",
	L"【ヒント:リザルト】リザルトで表示される天気予報を見てみましょう。　天気が精度推移を表し、降水確率がゲージ推移を表しています。",
	L"【ヒント:クイックプレイ】曲が始まる前にEnterキーを押すとすぐに曲を開始できます。",
	L"【ヒント:～～Music地方】～～Music地方は、ここから遠く離れた場所を示しています。　雰囲気が気に入った場合はそちらに赴くのも良いでしょう。",
	L"【ヒント:WIND BREAKシステム】演奏中にCtrlとShiftキーでカバーの位置を調節できます。 音符が詰まっている箇所で下げると見やすくなります。"
	};
	int GeneralStrUse = 0;//どの通常アナウンスを使うか
	GeneralStrUse = GetRand(GeneralStrNum - 1);//
	//GeneralStrUse = 22;
	return GeneralStr[GeneralStrUse];
}

void DrawRadarLine(double angle) {
	//short r[6] = { v1,v2,v3,v4,v5,v6 };//半径
	short r = 99;
	short o[2] = { 160,360 };//原点座標

	float X[2] = { 0,0 }, Y[2] = { 0,0 };

	//short sum = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
	//short th = 75;//色を変化させるsumの閾値
	//double ind = 0;//0~255
	int color = GetColor(255, 255, 255);

	float d = (float)1 / 60;//

	int range = 10;//線の幅

	//100で120pixの長さ
	for (int i = 0; i <= range; i++) {
		float id = (float)i / 60;

		X[0] = (float)(o[0] + sin((3.14159265 / 3) * (angle + id - d)) * r * 1.2);
		Y[0] = (float)(o[1] - cos((3.14159265 / 3) * (angle + id - d)) * r * 1.2);

		X[1] = (float)(o[0] + sin((3.14159265 / 3) * (angle + id)) * r * 1.2);
		Y[1] = (float)(o[1] - cos((3.14159265 / 3) * (angle + id)) * r * 1.2);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(80 * (double)i / range));
		//内側

		DrawTriangle((int)X[0], (int)Y[0],
			(int)X[1], (int)Y[1],
			o[0], o[1], color, TRUE);
	}

	return;
}

int GetCylinderColor(double val, double range, unsigned int R, unsigned int G, unsigned int B) {//円柱っぽく色を得る関数 0~rangeの間で変化
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
		indR = (double)R * (1-((val - 5 * th) / th));
		indG = (double)G * (1 - ((val - 5 * th) / th));
		indB = (double)B * (1 - ((val - 5 * th) / th));
		color = GetColor((int)indR, (int)indG, (int)indB);
	}
	else {
		color = GetColor(0, 0, 0);
	}
	return color;
}

int GetRainbowColorForRadar(int val,int th) {//虹色のコードを得る関数(レーダー用)
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

void DrawHexagon(short v1, short v2, short v3, short v4, short v5, short v6) {
	short r[6] = { v1,v2,v3,v4,v5,v6 };//半径
	short o[2] = { 160,360 };//原点座標
	float posX[6] = { 0,0,0,0,0,0 };
	float posY[6] = { 0,0,0,0,0,0 };

	float posX_i[6] = { 0,0,0,0,0,0 };//内側用
	float posY_i[6] = { 0,0,0,0,0,0 };

	short sum = r[0] + r[1] + r[2] + r[3] + r[4] + r[5];
	short th = 75;//色を変化させるsumの閾値
	double ind = 0;//0~255
	int color = GetColor(255, 255, 255);

	color = GetRainbowColorForRadar(sum, th);//対応する虹色を得る

	//100で120pixの長さ
	for (int i = 0; i <= 5; i++) {
		posX[i] = (float)(o[0] + sin((3.14159265 / 3) * i) * r[i] * 1.2);
		posY[i] = (float)(o[1] - cos((3.14159265 / 3) * i) * r[i] * 1.2);

		posX_i[i] = (float)(o[0] + sin((3.14159265 / 3) * i) * (r[i]+0.5) * 1.2);
		posY_i[i] = (float)(o[1] - cos((3.14159265 / 3) * i) * (r[i]+0.5) * 1.2);
	}


	

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	//内側
	for (int i = 0; i <= 5; i++) {
		DrawTriangle((int)posX_i[i], (int)posY_i[i], (int)posX_i[(i + 1) % 6], (int)posY_i[(i + 1) % 6], o[0], o[1], color, TRUE);
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 256);
	//外枠
	for (int i = 0; i <= 5; i++) {
		DrawLineAA(posX[i], posY[i], posX[(i + 1) % 6], posY[(i + 1) % 6], GetColor(30, 30, 30), 2);
	}

	return;
}

void OptionValueChange(Option *Option, int option_select, int add) {//オプションの値変更関数
	*Option->ArrayValue[option_select] = *Option->ArrayValue[option_select] + add;
	*Option->ArrayValue[option_select] = number_ring(*Option->ArrayValue[option_select], *Option->ArrayOptionNum[option_select] - 1);//数値を収める

	return;
}

void DrawOptionSentence(Option* Option, OptionItem::Name option_select, Config config, int FontHandle) {//Option->H_SENTにオプションの説明を描画
	int i = 0;
	wchar_t* StrAddress;//選んでいるオプションの説明文の先頭アドレス

	if (option_select == OptionItem::Name::NOTE) {
		StrAddress = Option->ArrayOptionSent[(int)OptionItem::Name::NOTE][0];
	}
	else if (option_select == OptionItem::Name::HITSOUND) {
		StrAddress = Option->ArrayOptionSent[(int)OptionItem::Name::HITSOUND][0];
	}
	else if (option_select == OptionItem::Name::THEME) {
		StrAddress = Option->ArrayOptionSent[(int)OptionItem::Name::THEME][0];
	}
	else {
		StrAddress = Option->ArrayOptionSent[(int)option_select][*(Option->ArrayValue[(int)option_select])];
	}

	int width = 0;
	width = GetDrawStringWidth(StrAddress, wcslen(StrAddress));
	SetDrawScreen(Option->H_SENT);//オプション説明画像を描画対象に
	ClearDrawScreen();//前の画像を消去
	//説明を描画して画像作成
	SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
	ShowExtendedStrFitToHandle(320, 7, StrAddress, width, 620, config, FontHandle);
	SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す

	SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す
	return;
}


//SortSongListIndexに対する<>の演算子オーバーロード
bool operator<(const SortSongListIndex& left, const SortSongListIndex& right)
{
	return left.value < right.value;
}

bool operator>(const SortSongListIndex& left, const SortSongListIndex& right)
{
	return left.value > right.value;
}


int clearStateConverter(int clearState) {//クリア状態番号をFolderScore用の数値に変換する関数
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

