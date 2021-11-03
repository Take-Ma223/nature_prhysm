#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_NOTE.h"
#include"STRUCT_CALCDIFF.h"
#include"STRUCT_SONG.h"
#include"STRUCT_CELL.h"
#include"STRUCT_SEQ.h"
#include"STRUCT_ANDROID_CONTROLLER.h"

#include"GAME.h"
#include"GAME_LOAD.h"
#include"cell_operation.h"
#include"setupSerial.h"
#include"LED_operation.h"
#include"number_digit.h"
#include"Get_Key_State.h"
#include"show_something.h"
#include"PlayHitSound.h"
#include"xtosinx.h"
#include"show_something.h"
#include"STRUCT_EDIT_SCORE.h"
#include"EDIT_SCORE.h"
#include"GetNowCount_d.h"
#include"number_ring.h"
#include"ShowFps.h"
#include "EffekseerForDXLib.h"
#include"ScreenShot.h"
#include<string>
#include "IR_process.h"
#include "Image.h"

using namespace std;

void GAME(int song_number, int difficulty,
	RESULT *res, int *escape, OPTION *option, int *retryAble,
	int *debug, Song *Music, int Button[3][4], int Button_Shutter, int *Key, char *Buf,
	int secret,//隠し曲演出中か(0:通常 1:演出中)
	ANDROID_CONTROLLER *AC,
	CONFIG config,
	IR_SETTING* ir,
	int SkillTestFlag,//段位認定モードか(0:通常 1~4:段位認定モード ステージ数を示す)
	double *GaugeVal,
	int *CourseCombo,
	int *CourseMaxCombo,
	int AllowExit
	)
{
	HANDLE hComm, hEvent;
	setupSerial(&hComm);//シリアル通信設定
	STATE LED_state;
	OVERLAPPED ovl;
	DWORD dwWritten;
	hEvent = CreateEvent(NULL, FALSE, FALSE, L"olp");
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = hEvent;

	SCORE_CELL score_cell_head;
	score_cell_head.next = NULL;
	score_cell_head.before = NULL;
	score_cell_head.step = -1;

	double GAME_start_time;
	double GAME_passed_time;//単位はms 判定と音用の経過時間
	double GAME_passed_time_for_draw;//譜面描画用の経過時間(ディスプレイ遅延補正用)
	double LOOP_passed_time = 1;//1ループにかかった時間(ms)
	double CounterRemainTime = 0;////カウンターの値を1msずつ変動するための時間　1msずつ引かれて小数以下は蓄積する
	double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数
	double TimePerFrame = 1000.0 / config.Fps;//1フレームの時間
	int H_NOTE[12];//音符画像(0は無しで1~9でRGBYCMWKF 10はLNを叩いた時に光らせるレイヤー用 光るノート用)
	//int H_NOTE_OR_FRAME;//ORノートの枠

	int H_LNOTE[12];//LNの中間用画像ハンドル(0は無しで1~9でRGBYCMWK(無し)F)
	//int H_LNOTE_OR_FRAME;//LNでORノートの枠

	int H_JUDGE_AREA, H_BG;//画像データのハンドル
	int H_JUDGE_AREA_PAINT;
	int H_CLOUD;

	int H_COMBO;
	int H_COMBO_NUMBER[10];
	int H_SCORE_NUMBER[10];
	int H_HIT[16], H_HIT_LARGE[20], H_HIT_D[16];//ノートを叩いた時のフラッシュ画像ハンドル
	int H_SKY_PERFECT,H_PERFECT, H_GOOD, H_MISS;//SKY_PERFECT,PERFECT,GOOD,MISS判定画像の画像ハンドル
	int H_COVER = 0;//カバー画像
	int H_COVER_FLASH;//カバーフラッシュ画像
	int H_COVER_MIDDLE;//中心カバー
	int H_GAUGE_BOX;//ゲージの下に表示
	int H_GAUGE_BAR = 0;//ゲージ画像
	int H_FAILED;//
	int H_CLEARED;//
	int H_FULL_COMBO[6];//
	int H_PFC;//
	int H_COVER_STR;//右側の文字画像
	int H_DIFFICULTY = 0;//難易度画像
	int H_R_IN;
	int H_R_OUT;

	int H_BUTTON_R;
	int H_BUTTON_G;
	int H_BUTTON_B;
	int H_BUTTON;//スイッチ画像
	int H_BUTTON_PRESS;//スイッチ押下画像

	int H_FALL[8];//音符を叩いた時に後ろに落ちるもの

	int H_TIME_NUMBER[10];//左上に表示する時間の画像
	int H_TIME_COLON;

	int H_POP_NUMBER[10];

	int H_DARKNESS;//背景を暗くする用の黒い画像
	int H_GAME_STR_JUDGE_BPM;
	int H_GAME_STR_SCORE_GRAPH;

	int FXH_FULLCOMBO = LoadEffekseerEffect(L"img/FullComboFX/FullComboFX.efk", 25.0f);//effekseerエフェクトリソースハンドル
	// 再生中のエフェクトのハンドルを初期化する。
	int FXPH_FULLCOMBO = -1;
	int FullComboFXPlayFlag = 1;//フルコンボエフェクト再生フラグ
	int FullComboFXBaseTime = 0;
	int FullComboFXFrame = 0;

	GAME_SH SH;
	int AllowSound[3][4] = { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } };//そのフレームでコントローラを叩いた時の音を出すか コントローラの並び順と同じ(0:B 1:G 2:R)
	double HitAreaBright[3][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0} };//コントローラを叩いた時の判定枠の各色の光具合

	int SH_SONG;//曲データのハンドル
	int	SH_CLOSE;
	int	SH_CLOSED;
	int	SH_OPEN;
	int SH_JINGLE;
	int SH_SHUTTER;
	int SH_SHUTTER_SIGNAL;

	/*
	int SH_R[4];
	int SH_G[4];
	int SH_B[4];
	*/

	int i = 0, j = 0, k = 0;
	double cbpm = 120;//現在のBPM
	double speed = 0.00000000001;//スピード調整
	double high_speed = 1;//ハイスピ
	high_speed = option->op.speed_val;//段位以外ではあらかじめ決めた値を格納
	double scale = 1;//判定の横の広がり幅
						//int lane1 = int(512 - 240 * scale);
						//int lane2 = int(512 - 80 * scale);
						//int lane3 = int(512 + 80 * scale);
						//int lane4 = int(512 + 240 * scale);
	int lane[4] = {
		int(512 - 240 * scale),//レーンのx座標
		int(512 - 80 * scale),
		int(512 + 80 * scale),
		int(512 + 240 * scale) };
	int cash = 0;//式の見た目を簡単にするため
	int judge_area = 490, note_fall = -192;//ノートの判定エリアと落ち始める高さ-192~490

	double judge_time_sky_perfect = 16.66;//判定時間/2(パーフェクト)ms
	double judge_time_perfect = 40;//判定時間/2(パーフェクト)ms
	double judge_time_good = 130;//判定時間/2(コンボがつながる判定の幅/2)単位はms
	double judge_time_bad = 200;//判定時間/2(good以上bad未満のずれで叩くとコンボが途切れる)
	double judge_time_through = 180;//判定時間(この時間以上ノーツを見逃すとnote_searchがインクリメントされコンボが切れる)
	double judge_time_dark = 130;//黒用の判定時間/2
	double judge_time_delay = 200;//LNをちょっと離しても良い猶予時間

	int con_jd = 5;//黒用の判定時間/2
	int j_n_n[4] = { 0,0,0,0 };//判定時間内の最も下にある(あと少しで見逃しになる)まだ叩かれていないノートの番号(黒以外用)(judge_note_number)
	int j_dn_n[4] = { 0,0,0,0 };//↑の黒ノーツ叩き判定用(judge_darknote_number)
	int j_dn_push_n[4] = { 0,0,0,0 };//↑の黒ノーツ押し込み判定用(judge_darknote_push_number)

	int searching = 0;//judge関数内で変化したj_n_nを入れる変数
	int LN_flag[4] = { 0,0,0,0 };//ロングノートが来てるかのフラグ(1or2ならそこのレーンのLNを押し続けなければならない。2のときに手を離すとミスになり1になる。 -1で押し切った状態)
	int LN_judge[4] = { 0,0,0,0 };//LNの判定を保存する(0:MISS 1:GOOD 2:PERFECT 3;SKY_PERFECT)
	int LN_flash[4] = { 0,0,0,0 };
	int LN_push[4] = { 0,0,0,0 };//LNの色でボタンを押しているかのフラグ 対応する色を押しているときは1,離したフレームは-1,それ以外の離しているとき0
	int dark_hit = 0;//黒を叩いてしまったか
	int note_search = 0;//下から順にノートを探していくカウンタ
	int timingDifference = -10000;//ノートを叩いた時のタイミングのずれ
#define NOTE_HIT_LARGE_FLASH_NUMBER 4//同時に重ねて表示する個数
	double note_hit_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { 0,0,0,0 };//ノートを叩いた時のフラッシュを表示するためのカウンタ
	int note_hit_flash_rounder = 0;//4個以内で順々にフラッシュを表示するためのカウンタ(0~3で回る)
	double note_hit_large_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { 0,0,0,0 };//光るノートを叩いた時のフラッシュを表示するためのカウンタ
	int note_hit_large_flash_rounder = 0;//4個以内で順々にフラッシュを表示するためのカウンタ(0~3で回る)
	double dnote_hit_flash[4] = { 0,0,0,0 };//黒ノートを叩いた時のフラッシュを表示するためのカウンタ
	const int FLASH_VALUE = 100;//長音ノートの点滅輝度
	const int FLASH_VALUE_ALWAYS = 70;//長音ノートの常時点滅輝度
	const int FLASH_COVER_VALUE = 15;//カバーの点滅輝度

	double hit_sky_perfect[4] = { 0,0,0,0 };//判定画像のSKY_PERFECTを表示するカウンタ
	double hit_perfect[4] = { 0,0,0,0 };//判定画像のPERFECTを表示するカウンタ
	double hit_good[4] = { 0,0,0,0 };//判定画像のGOODを表示するカウンタ
	double hit_miss[4] = { 0,0,0,0 };//判定画像のMISSを表示するカウンタ
	double combo_draw_counter = 0;//コンボ画像を表示するカウンタ
	double c_m_draw_counter = 0;//中心カバー画像を表示するカウンタ
	double str_draw_counter = -1;//CLEARED FAILED画像を表示するカウンタ
	int TimeFromEndOfGameToResult = 4000;//CLEARED FAILEDが表示されてからリザルトに移るまでの時間(ms)

	int combo = 0;//コンボ
	double score = 0;//スコア
	int SKY_PERFECT = 0;//SKY_PERFECT数
	int PERFECT = 0;//PERFECT数
	int GOOD = 0;//GOOD数
	int MISS = 0;//MISS数
	int MAX_COMBO = 0;//最大コンボ数
	int score_MAX = 10000;//スコア理論値
	double score_note = 0;//音符一つの得点
	double score_dec = int(score_MAX*0.001);//ミス時のスコア減少量(理論値の0.1%分(10点)スコア減少)
	int combo_digit[5] = { 0,0,0,0,0 };//コンボ数を桁ごとに格納
	int score_digit[5] = { 0,0,0,0,0 };//スコア
	int time_minutes_digit[2] = { 0,0 };//分
	int time_hours_digit[2] = { 0,0 };//時間
	int bpm_digit[5] = { 0,0,0,0,0 };//BPM
	double gauge = 100;
	if (SkillTestFlag != 0) {//段位なら最初のゲージの量変更
		gauge = *GaugeVal;
	}

	double gauge_draw_counter = gauge;//ゲージを滑らかに表示するためのカウンター(仲介)
	double gauge_draw_hosei = gauge;//ゲージを段々緩やかに減少させて表示するためのカウンター(xtosinx()を使う)
	int pop_digit[3] = { 0,0,0 };//ゲージ量表示用桁
	int pop = int(100 - gauge_draw_hosei);//降水量
	int HitingNoteCount = 0;//黒を除いて何個の音符の判定が終了したかを表す数値

	double key_draw_counter[3][4];//0になればコントローラ画像が位置につく


	for (i = 0; i <= 2; i++) {
		for (j = 0; j <= 3; j++) {
			key_draw_counter[i][j] = (double)1 + (double)(GetRand(99)) / (double)100;
		}

	}


	int TimeToEndScroll = 0;//Scroll時間での演奏終了タイミング
	int playing_time = 0;//最初のノートから最後のノートまでの時間
	double gauge_inc_times = 1;//曲の長さによるゲージの増幅倍率(GAUGE_INC_BASE_TIME以上で上がっていく)
	const int GAUGE_INC_BASE_TIME = 120000;//120秒
	const int SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT = 1500;//段位ゲージの時このノート数を基準にtotalを決める
	int PassedTime_Minutes = 0;//Scroll時間での経過時間の分
	int PassedTime_Hours = 6;//Scroll時間での経過時間の時間
    double HourOfGame = 0;//左上の時間の1時間がScroll時間でいくつか
	double MinuteOfGame = 0;//左上の時間の1分がScroll時間でいくつか
	int TimePop[18] = { 100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100 };//1時間毎の降水量popを保存
	int TimeAcc[6] = { 0,0,0,0,0,0};//3時間毎の精度を保存
	int TimePerfect = 0;//Perfect数を保存(3時間毎に0にリセット)
	int TimeGood = 0;//Good数を保存(3時間毎に0にリセット)
	int TimeMiss = 0;//Miss数を保存(3時間毎に0にリセット)
	int TimeNextHour = 7;


	double total = 0;//全てPERFECTで叩いたときに増えるゲージのトータル量
	double total_base = 0;//全てPERFECTで叩いたときに増えるゲージのトータル量(曲の長さで増減する前のベースの値)
	double miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
	double gauge_dec_MIN = 0;//ゲージ減少量が4%以下にならないようにする
	double gauge_dec_MAX = 0;//ゲージ減少量が8%以上にならないようにする
	double gauge_dec = 0;//ミス時のゲージ減少量(4以下8以上にはしない)
	int ClearFlag = 0;//クリアフラグ(0:不定 1:CLEARED 2:FAILED)
	double draw_alpha = 0;//画像の透過状態(0:透過 1:不透明)
	int genre_width = 1;//ジャンル横幅
	int title_width = 1;//曲名横幅
	int artist_width = 1;//アーティスト横幅
	int jingleflag = 0;//ジングルを鳴らすフラグ
	int openflag = 0;//中心カバーが開くときの音を鳴らすフラグ
	int cleared_time = 0;//クリアした時間を記録
	int start_c_draw_counter = 0;//スタート時の中心カバー描画フラグ
	double debug_time_passed = 0;//戻ったり進んだりする時間
	int debug_stop = 0;//譜面再生停止
	int debug_warp = 0;//再生位置移動したかどうか(0:していない 1:している)
	double debug_warp_time = 0;//ワープする時間(ms)1000ms以上にはしない
	double debug_stop_time = 0;//再生停止するための時間
	double debug_stoped_time = 0;//再生停止したときのカウント
	int debug_warp_counter = 0;//矢印キー長押しのカウンタ
	int debug_auto = *debug;//オートプレイ(デバッグモードなら最初からオートにしておく)
	int debug_sound = 1;//1ならキー音を鳴らす
	int debug_fail = 1;//0:閉店無し 1:閉店あり
	int debug_music = 1;//曲を鳴らすかどうか
	if (*debug == 1)debug_fail = 0;//デバッグモードならデフォルトで閉店無し
#define ROOT12_2 1.0594630943592952645618252949463
	char pitch_step = 0;//ピッチが半音単位でどれだけ上がっているか
	double pitch = 1;//この数値だけ再生速度を倍にする
	double frequency = 44100;//再生周波数

	double volume = 1;//曲のボリューム
	double flash = 0;//フラッシュ(カバーの明るさ)
	int beat = 0;//進んだ拍
	double beat_time = 0;//進んだ拍を算出するための変数
	double beat_time_cash = 0;
	double beat_percent = 0;//前の拍から何パーセント拍進んでいるか
	int bgmplay = 0;//BGMを流したかどうか(最初に一回だけ再生するためのフラグ)
	int FirstBgmPlay = 1;//今から流すBGMが最初の再生かどうかのフラグ
					//int level_s;//計算で出したレベル目安
	CALCDIFF Cdiff;//計算で出した難易度要素目安

	int bcc = 0;//bpmchangeのカウンタ
	int scc = 0;//scrollchangeのカウンタ
	double sc_timing = 0;//前にスクロール変化があった時間
	double cscroll = 1;//現在の実時間倍率(譜面全体のスピード倍率)
	double GAME_passed_time_scroll = 0;//譜面としての経過時間(scrollを考えた分)
	double real_timing = 0;//前にscrollが変化したときのGAME_passed_timeの時間
	int hash = 0;
	//int release_time[3][4];//キーが離された時間

	//NOTE note[5][NOTE_MAX_NUMBER];//[レーン][ノート番号]   //[0][]はBPM情報)

	BPMC *bpmchange;//BPM変更
	SC *scrollchange;//SCROLL変更
	STOP_SE *stopSequence;//譜面停止

	bpmchange = (BPMC*)calloc(NOTE_MAX_NUMBER, sizeof(BPMC));
	scrollchange = (SC*)calloc(NOTE_MAX_NUMBER, sizeof(SC));
	stopSequence = (STOP_SE*)calloc(NOTE_MAX_NUMBER, sizeof(STOP_SE));

	int stop_se_c = 0;//stopSequenceのカウンタ
	double stop_time = 0;//譜面停止するための時間
	double stop_time_sum = 0;//今までの総譜面停止時間
	int stopFlag = 0;//譜面停止中どうかのフラグ(1:停止中)

	


	NOTE **note;
	note = (NOTE**)calloc(4, sizeof(NOTE));

	for (i = 0; i <= 3; i++) {
		note[i] = (NOTE*)calloc(NOTE_MAX_NUMBER, sizeof(NOTE));
	}

	//小節線構造体メモリ確保
	BARLINE *barline;
	barline = (BARLINE*)calloc(BARLINE_MAX_NUMBER, sizeof(BARLINE));

	int blc = 0;//barline counter

	NOTE hit_n[4];//最後に叩いたノートの情報


	double fall_y = 0;
	//叩くと後ろに落ちるもの関連
	CELL head;//リストの先頭
	head.num = 0;
	head.xypos.x = 0;
	head.xypos.y = 0;
	head.y_speed = 2.406;
	head.next = NULL;

	CELL *insert;//リストに挿入する場所を示すポインタ(このポインタの挿すセルの後ろに挿入)
	insert = &head;//最初はリストの先頭を入れておく

	//EDIT_SCORE関数に渡す用
	double scale_score = 1;//拡大縮小率
	int step_array_number = 0;//ステップ増減値配列の添え字
	int measure_start_step = 0;//小節線を表示する起点
	int SwingBackBeatTime = SWING_DIVISION / 2;//


	//----Read----
	if (option->op.gauge == option->OP_GAUGE_NORMAL) {
		total_base = 375;
		miss_percent = 0.05;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 3;//ゲージ減少量が4%以下にならないようにする
		gauge_dec_MAX = 3;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == option->OP_GAUGE_HARD) {
		total_base = 225;
		miss_percent = 0.05;//(125~200コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 6;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 6;//ゲージ減少量が10%以上にならないようにする
	}
	if (option->op.gauge == option->OP_GAUGE_SUPER_HARD) {//10回ミスすると失敗
		total_base = 100;//ゲージ回復100
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 10;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 10;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == option->OP_GAUGE_FC_ATTACK) {//ミスすると失敗
		total_base = 0;//ゲージ回復なし
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 100;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 100;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == option->OP_GAUGE_PFC_ATTACK) {//MISS,GOODすると失敗
		total_base = 0;//ゲージ回復なし
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 100;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 100;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == option->OP_GAUGE_SKILL_TEST) {//段位ゲージ 24個で1MISS分回復するようにノート数補正をかけるy
		total_base = 125;
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 1.5;//ゲージ減少量
		gauge_dec_MAX = 1.5;//ゲージ減少量

		if (option->op.color == option->OP_COLOR_RAINBOW) {//基本の時は減少量多め
			gauge_dec_MIN = 3;//ゲージ減少量
			gauge_dec_MAX = 3;//ゲージ減少量
		}

	}


	GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, &hash, Music, &TimeToEndScroll, &playing_time, config, 1, NULL, SkillTestFlag);//noteに譜面情報を入れる(譜面部分のロード)
	Music[song_number].hash[difficulty] = hash;
	res->hash = hash;//ハッシュ値を格納

	if (playing_time > GAUGE_INC_BASE_TIME) {//120秒以上の譜面なら
		gauge_inc_times = (double)playing_time / (double)GAUGE_INC_BASE_TIME;
	}

	if (SkillTestFlag != 0) {//段位補正
		gauge_inc_times *= (double)Music[song_number].total_note[difficulty] / SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT;
	}


	total = total_base*gauge_inc_times;

	score_note = (double)score_MAX / Music[song_number].total_note[difficulty];//音符一つの得点を決める

	HourOfGame = TimeToEndScroll / 18;//左上時間の1時間の単位を決定
	MinuteOfGame = HourOfGame / 60;//分を決める

	gauge_dec = ((double)100 / (miss_percent*Music[song_number].total_note[difficulty]));
	if (gauge_dec >= gauge_dec_MAX)gauge_dec = gauge_dec_MAX;
	if (gauge_dec <= gauge_dec_MIN)gauge_dec = gauge_dec_MIN;
	cbpm = Music[song_number].bpm[difficulty];//最初のBPM
	cscroll = scrollchange[0].scroll;
	//printfDx("%d\n", Music[song_number].total_note[difficulty]);


	//スコアゲージ用のスコア読み込み
	FILE* fpHighScore = 0;
	FILE* fpLatest = 0;
	FILE* fpRival = 0;
	errno_t error = 0;
	RESULT highScore;//現在のハイスコア
    RESULT latestScore;//前回のスコア
	RESULT rivalScore;//ライバルハイスコア

	wchar_t filenameHighScore[256];
	wchar_t filenameLatest[256];
	wchar_t filenameRival[256];

	wchar_t* scoreDiff[4] = { L"sunny",L"cloudy",L"rainy",L"thunder" };

	if (option->op.color != option->OP_COLOR_RAINBOW) {//通常モードだったら
		sprintfDx(filenameHighScore, L"%s/result_%s.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameLatest, L"%s/latest_result_%s.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameRival, L"%s/result_%s.dat", Music[song_number].RivalSaveFolder, scoreDiff[difficulty - 1]);
	}
	if (option->op.color == option->OP_COLOR_RAINBOW) {//虹モードだったら
		sprintfDx(filenameHighScore, L"%s/result_%s_r.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameLatest, L"%s/latest_result_%s_r.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameRival, L"%s/result_%s_r.dat", Music[song_number].RivalSaveFolder, scoreDiff[difficulty - 1]);

	}

	error = _wfopen_s(&fpHighScore, filenameHighScore, L"rb");
	if (error == 0) {//ファイルがあるときに読み込み
		fread(&highScore, sizeof(highScore), 1, fpHighScore);//ハイスコア読み込み
		fclose(fpHighScore);
	}
	error = _wfopen_s(&fpLatest, filenameLatest, L"rb");
	if (error == 0) {//ファイルがあるときに読み込み
		fread(&latestScore, sizeof(latestScore), 1, fpLatest);//ハイスコア読み込み
		fclose(fpLatest);
	}
	error = _wfopen_s(&fpRival, filenameRival, L"rb");
	if (error == 0) {//ファイルがあるときに読み込み
		fread(&rivalScore, sizeof(rivalScore), 1, fpRival);//ハイスコア読み込み
		fclose(fpRival);
	}



	//画像音ハンドル値代入
	wchar_t *ReadNameRGB[11] = { L"r",L"g",L"b",L"y",L"c",L"m",L"w",L"d",L"f",
	L"bright",L"note_Long_hit_b" };

	for (i = 0; i <= 10; i++) {
		wchar_t strcash[128];
		sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[i]);

		H_NOTE[i + 1] = LoadGraph(strcash);

		wchar_t strcash_l[128];
		sprintfDx(strcash_l, L"img/notes/%s/%sl.png", option->note[option->op.note], ReadNameRGB[i]);
		H_LNOTE[i + 1] = LoadGraph(strcash_l);
	}

	DeleteGraph(H_LNOTE[8]);//黒ロング画像を削除し、透過加工黒ロング画像を作成
	wchar_t strcash[128];
	sprintfDx(strcash, L"img/notes/%s/dl.png", option->note[option->op.note]);
	int SOFTH_LNOTE_B = LoadSoftImage(strcash);
	for (int y = 0; y < 256; y++) {//透過グラデーション加工
		for (int x = 0; x < 256; x++) {
			int r = 0, g = 0, b = 0, a = 0;
			GetPixelSoftImage(SOFTH_LNOTE_B, x, y, &r, &g, &b, &a);
			if (a != 0)DrawPixelSoftImage(SOFTH_LNOTE_B, x, y, r, g, b, 255 - y);

		}
	}
	H_LNOTE[8]= CreateGraphFromSoftImage(SOFTH_LNOTE_B);
	DeleteSoftImage(SOFTH_LNOTE_B);

	sprintfDx(strcash, L"img/notes/%s/JudgeArea.png", option->note[option->op.note]);
	H_JUDGE_AREA = LoadGraph(strcash);
	sprintfDx(strcash, L"img/notes/%s/JudgeAreaPaint.png", option->note[option->op.note]);
	H_JUDGE_AREA_PAINT = LoadGraph(strcash);

	int Keylist[3][4] = { { Button[0][0],Button[0][1],Button[0][2],Button[0][3] },
	{ Button[1][0],Button[1][1],Button[1][2],Button[1][3] },
	{ Button[2][0],Button[2][1],Button[2][2],Button[2][3] } };//キーのリスト
	int Colorlist[3][7] = { { 3, 5, 2, 6, 1, 7, 4 },
	{ 2, 4, 1, 5, 3, 7, 6 },
	{ 1, 4, 2, 6, 3, 7, 5 } };


	XY pos_cloud;//雲の座標
	if (secret == 0) {//隠し曲演出中ではない
		H_CLOUD = LoadGraph(L"img/cloud.png");
	}
	else if (secret == 1) {//隠し曲演出中なら雲を黒に
		H_CLOUD = LoadGraph(L"img/cloud_black.png");
	}
	H_SKY_PERFECT = LoadGraph(L"img/SKY_PERFECT.png");
	H_PERFECT = LoadGraph(L"img/PERFECT.png");
	H_GOOD = LoadGraph(L"img/GOOD.png");
	H_MISS = LoadGraph(L"img/MISS.png");
	H_GAUGE_BOX = LoadGraph(L"img/gauge_box.png");
	if (option->op.gauge == option->OP_GAUGE_NO_FAIL)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_no_fail.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_NORMAL)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_normal.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_HARD)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_hard.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_SUPER_HARD)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_super_hard.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_FC_ATTACK)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_fc.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_PFC_ATTACK)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_pfc.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == option->OP_GAUGE_SKILL_TEST)H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_skill_test.png");//(左上95,105)大きさ130*510


	H_R_IN = LoadGraph(L"img/R_inside.png");
	H_R_OUT = LoadGraph(L"img/R_outside.png");

	LoadDivGraph(L"img/hit.png", 16, 4, 4, 256, 256, H_HIT);
	LoadDivGraph(L"img/hit_large.png", 20, 4, 5, 300, 300, H_HIT_LARGE);
	LoadDivGraph(L"img/hit_d.png", 16, 4, 4, 256, 256, H_HIT_D);
	H_COMBO = LoadGraph(L"img/combo.png");


	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg_game.png")).c_str());


	if (difficulty == 1) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());//難易度によってカバー変更
		H_DIFFICULTY = LoadGraph(L"img/sunny.png");
	}
	if (difficulty == 2) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
		H_DIFFICULTY = LoadGraph(L"img/cloudy.png");
	}
	if (difficulty == 3) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
		if (Music[song_number].season[difficulty] != 4) {//雪じゃないとき
			H_DIFFICULTY = LoadGraph(L"img/rainy.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/snowy.png");
		}
	}
	if (difficulty == 4) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
		if (Music[song_number].season[difficulty] != 4) {//雪じゃないとき		
			H_DIFFICULTY = LoadGraph(L"img/thunder.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/snowstorm.png");
		}
	}


	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
	H_FAILED = LoadGraph(L"img/failed.png");
	H_CLEARED = LoadGraph(L"img/cleared.png");

	H_CLEARED = LoadGraph(L"img/cleared.png");
	if (option->op.gauge == option->OP_GAUGE_NORMAL)H_CLEARED = LoadGraph(L"img/cleared_normal.png");
	if (option->op.gauge == option->OP_GAUGE_HARD)H_CLEARED = LoadGraph(L"img/cleared_hard.png");
	if (option->op.gauge == option->OP_GAUGE_SUPER_HARD)H_CLEARED = LoadGraph(L"img/cleared_super_hard.png");
	if (option->op.color == option->OP_COLOR_RGB_ONLY || 
		option->op.color == option->OP_COLOR_CMY_ONLY ||
		option->op.color == option->OP_COLOR_W_ONLY)H_CLEARED = LoadGraph(L"img/cleared_easy.png");

	LoadDivGraph(L"img/FULL_COMBO.png", 6, 1, 6, 640, 100, H_FULL_COMBO);
	H_PFC = LoadGraph(L"img/PERFECT_FULLCOMBO.png");

	LoadDivGraph(L"img/combo_number.png", 10, 10, 1, 64, 100, H_COMBO_NUMBER);
	LoadDivGraph(L"img/score_number.png", 10, 10, 1, 64, 100, H_SCORE_NUMBER);
	LoadDivGraph(L"img/time_number.png", 10, 10, 1, 64, 100, H_TIME_NUMBER);
	H_TIME_COLON = LoadGraph(L"img/time_colon.png");

	H_COVER_STR = LoadGraph(L"img/cover_str.png");
	H_COVER_FLASH = LoadGraph(L"img/cover_flash.png");

	H_FALL[0] = LoadGraph(L"img/fall_spring1.png");
	H_FALL[1] = LoadGraph(L"img/fall_spring2.png");
	H_FALL[2] = LoadGraph(L"img/fall_summer1.png");
	H_FALL[3] = LoadGraph(L"img/fall_summer2.png");
	H_FALL[4] = LoadGraph(L"img/fall_autumn1.png");
	H_FALL[5] = LoadGraph(L"img/fall_autumn2.png");
	H_FALL[6] = LoadGraph(L"img/fall_winter1.png");
	H_FALL[7] = LoadGraph(L"img/fall_winter2.png");

	H_BUTTON_R = LoadGraph(L"img/button_R.png");//スイッチ画像
	H_BUTTON_G = LoadGraph(L"img/button_G.png");//スイッチ画像
	H_BUTTON_B = LoadGraph(L"img/button_B.png");//スイッチ画像
	H_BUTTON = LoadGraph(L"img/switch.png");//スイッチ画像
	H_BUTTON_PRESS = LoadGraph(L"img/button_press.png");//スイッチ押下画像

	H_DARKNESS = LoadGraph(L"img/BlackWall.png");
	H_GAME_STR_JUDGE_BPM = LoadGraph(L"img/GameStrJudgeBPM.png");
	H_GAME_STR_SCORE_GRAPH = LoadGraph(L"img/GameStrScoreGraph.png");

	int H_SMALL_NUMBER_RED[10];
	int H_SMALL_NUMBER_BLUE[10];
	int H_SMALL_NUMBER_GREEN[10];
	int H_SMALL_NUMBER_YELLOW[10];
	int H_SMALL_NUMBER_CYAN[10];

	LoadDivGraph(L"img/SmallNumberRed.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_RED);
	LoadDivGraph(L"img/SmallNumberGreen.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_GREEN);
	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_BLUE);
	LoadDivGraph(L"img/SmallNumberYellow.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_YELLOW);
	LoadDivGraph(L"img/SmallNumberCyan.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_CYAN);

	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_POP_NUMBER);

	Image I_Fast[4] = {
	Image(L"img/judge/fast_c.png", lane[0],judge_area,0),
	Image(L"img/judge/fast_c.png", lane[1],judge_area,0),
	Image(L"img/judge/fast_c.png", lane[2],judge_area,0),
	Image(L"img/judge/fast_c.png", lane[3],judge_area,0)
	};
	Image I_Slow[4] = {
	Image(L"img/judge/slow_r.png", lane[0],judge_area,0),
	Image(L"img/judge/slow_r.png", lane[1],judge_area,0),
	Image(L"img/judge/slow_r.png", lane[2],judge_area,0),
	Image(L"img/judge/slow_r.png", lane[3],judge_area,0)
	};
	int fastSlowY = -130;
	int fastSlowYMove = 10;

	sprintfDx(strcash, L"sound/hit_sound/%s/f2.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f1.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/r2.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_R_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r3.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_R_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r1.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_R_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/g2.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_G_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g3.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_G_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g1.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_G_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/b2.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_B_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b3.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_B_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b1.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_B_S = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/k.wav", option->hitsound[option->op.hitsound]);
	SH.SH_HIT_K = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/release.wav", option->hitsound[option->op.hitsound]);
	SH.SH_RELEASE = LoadSoundMem(strcash, 1);


	//曲データは非同期で読み込む
	SetUseASyncLoadFlag(TRUE);
	SH_SONG = LoadSoundMem(Music[song_number].wavpath[difficulty]);

	SetUseASyncLoadFlag(FALSE);
	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");
	SH_OPEN = LoadSoundMem(L"sound/open.wav");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");
	/*
	SH_R[0] = LoadSoundMem(L"sound/hit_R.wav");
	SH_G[0] = LoadSoundMem(L"sound/hit_G.wav");
	SH_B[0] = LoadSoundMem(L"sound/hit_B.wav");
	SH_R[1] = DuplicateSoundMem(SH_R[0]);
	SH_G[1] = DuplicateSoundMem(SH_G[0]);
	SH_B[1] = DuplicateSoundMem(SH_B[0]);
	SH_R[2] = DuplicateSoundMem(SH_R[0]);
	SH_G[2] = DuplicateSoundMem(SH_G[0]);
	SH_B[2] = DuplicateSoundMem(SH_B[0]);
	SH_R[3] = DuplicateSoundMem(SH_R[0]);
	SH_G[3] = DuplicateSoundMem(SH_G[0]);
	SH_B[3] = DuplicateSoundMem(SH_B[0]);
	*/

	//printfDx("%s\n", Music[song_number].jinglepath[difficulty]);
	SH_JINGLE = LoadSoundMem(Music[song_number].jinglepath[difficulty]);


	//----曲名表示----
	gauge_draw_counter = 0;

	int FontHandle = CreateFontToHandle(L"メイリオ", 28, 9, DX_FONTTYPE_ANTIALIASING_EDGE);//フォントハンドル
	ChangeFont(L"メイリオ");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(30);
	SetFontThickness(9);

	genre_width = GetDrawStringWidth(Music[song_number].genre[difficulty], wcslen(Music[song_number].genre[difficulty]));//ジャンル名の横の長さを入れる
	title_width = GetDrawStringWidth(Music[song_number].title[difficulty], wcslen(Music[song_number].title[difficulty]));//曲名の横の長さを入れる
	artist_width = GetDrawStringWidth(Music[song_number].artist[difficulty], wcslen(Music[song_number].artist[difficulty]));//アーティスト名の横の長さを入れる

	double draw_alpha_speed = 1;//段位用Speed表示フラグ(これが0になるまでは開始しない)

	//ハイスピを計算
	if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
		option->op.speed_val = option->speed_val[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
	}
	else {
		option->op.speed_val = 1;
	}
	high_speed = option->op.speed_val;


	//ターゲットスコアを決める
	int targetScore = 0;
	int targetScore2 = 0;

	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_E)targetScore = 5000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_D)targetScore = 6000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_C)targetScore = 7000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_B)targetScore = 8000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_A)targetScore = 9000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_S)targetScore = 9500;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_10000)targetScore = 10000;
	if (option->op.targetscore1 == option->OP_TARGET_SCORE1_MAX)targetScore = 10000 + Music[song_number].total_note[difficulty];

	if (option->op.targetscore2 == option->OP_TARGET_SCORE2_LATEST) {
		targetScore2 = latestScore.score;
	}
	else {

	}
	//必要な場合 IRランキングからスコアを取得
	if (option->op.scoregraph == option->OP_SCORE_GRAPH_ON &&
		(option->op.targetscore2 != option->OP_TARGET_SCORE2_LATEST)) {
		getTargetScore(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, option->op.color == option->OP_COLOR_RAINBOW, option->op.targetscore2, highScore.score, ir->rivalID, config);
		targetScore2 = LoadTargetScore(Music[song_number].SaveFolder);
	}

	GAME_start_time = GetNowCount_d(config);
	while (1) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}

		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//中心カバーが上がった時からの経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		Get_Key_State(Buf, Key, AC);
		if (Key[KEY_INPUT_ESCAPE] == 1 && *escape == 0 && AllowExit == 1) {
			*escape = 1;
			ClearFlag = 2;
			c_m_draw_counter = 0.00001;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
			gauge_draw_counter = gauge;
			draw_alpha = 1;
			break;
			/*
			*escape = 1;
			ClearFlag = 2;
			//c_m_draw_counter = 0;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
			*/
		}

		if (Key[KEY_INPUT_RETURN] == 1) {//エンターでスキップできる
			gauge_draw_counter = gauge;
			for (j = 0; j <= 2; j++) {
				for (k = 0; k <= 3; k++) {
						key_draw_counter[j][k] = 0;
				}
			}
		}
		if (Key[Button_Shutter] == 1) {//スクリーンショット
			ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
		}

		for (i = 0; i < CRTBuf; i++) {
			//透過度
			if (draw_alpha <= 1) {
				draw_alpha += 0.0012;
			}
			if (draw_alpha > 1) {
				draw_alpha = 1;
			}

			//ゲージ量
			if (gauge_draw_counter <= gauge && draw_alpha == 1) {
				gauge_draw_counter += (gauge - gauge_draw_counter) / 500;
			}
			if (gauge_draw_counter > gauge && draw_alpha == 1) {
				gauge_draw_counter = gauge;
			}

			for (j = 0; j <= 2; j++) {
				for (k = 0; k <= 3; k++) {
					if (key_draw_counter[j][k] > 0) {
						key_draw_counter[j][k] -= 0.00036;
					}
					else {
						key_draw_counter[j][k] = 0;
					}
				}
			}

			if (draw_alpha_speed > 0) {
				draw_alpha_speed -= 0.0005;
			}
			if (draw_alpha_speed <= 0) {
				draw_alpha_speed = 0;
			}

			for (j = 0; j < 3; j++) {//判定エリアの光
				for (k = 0; k < 4; k++) {
					HitAreaBright[j][k] /= 1.02;
				}
			}

			/*
			//中心カバー
			if ((c_m_draw_counter <= 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {
			c_m_draw_counter += 0.001;
			}
			if ((c_m_draw_counter > 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {
			c_m_draw_counter = 1;
			}
			*/

		}

		if ((gauge_draw_counter >= gauge - 0.001) && (CheckHandleASyncLoad(SH_SONG) != TRUE) && draw_alpha_speed == 0) {//ゲージが上まで描写されて曲の読み込みが完了して(エラーでも続行)段位ようspeed表示が消えたら演奏開始
			break;
		}

		gauge_draw_hosei = xtosinx(gauge_draw_counter);

		number_digit(int(score + 0.5), score_digit, 5);//スコアを桁ごとに格納
		number_digit(int(cbpm + 0.5), bpm_digit, 5);//BPMを桁ごとに格納
		number_digit(int(PassedTime_Hours), time_hours_digit, 2);//時間を桁ごとに格納
		number_digit(int(PassedTime_Minutes), time_minutes_digit, 2);//分を桁ごとに格納
		
	    //Draw
		ClearDrawScreen();
		//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));

		DrawGraph(0, 0, H_BG, TRUE);

		for (i = 0; i <= 3; i++) {//レーン
			for (j = 0; j <= 2; j++) {//色
				if (Key[Keylist[j][i]] >= 1) {
					HitAreaBright[j][i] = 1;//判定エリアの光
				}
			}
		}
		SetDrawBright(int(HitAreaBright[2][0] * 160 + 75), int(HitAreaBright[1][0] * 160 + 75), int(HitAreaBright[0][0] * 160 + 75));
		DrawGraph(lane[0], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][1] * 160 + 75), int(HitAreaBright[1][1] * 160 + 75), int(HitAreaBright[0][1] * 160 + 75));
		DrawGraph(lane[1], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][2] * 160 + 75), int(HitAreaBright[1][2] * 160 + 75), int(HitAreaBright[0][2] * 160 + 75));
		DrawGraph(lane[2], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][3] * 160 + 75), int(HitAreaBright[1][3] * 160 + 75), int(HitAreaBright[0][3] * 160 + 75));
		DrawGraph(lane[3], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(255, 255, 255);

		for (i = 0; i <= 3; i++) {
			DrawGraph(lane[i], judge_area, H_JUDGE_AREA, TRUE);//判定枠の表示
		}


		DrawGraph(0, 0, H_COVER, TRUE);//カバー表示
		DrawGraph(960, 0, H_COVER, TRUE);//右側
		DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
		//DrawGraph(960, 0, H_COVER_STR, TRUE);//右側の文字
		DrawGraph(0, 0, H_GAUGE_BOX, TRUE);//ゲージボックス (左上95,105)大きさ130*510

		DrawRectGraph(95, int(105 + ((100 - gauge_draw_hosei) / 100 * 510)), 95, int(105 + ((100 - gauge_draw_hosei) / 100 * 510)), 130, int(511 - ((100 - gauge_draw_hosei) / 100 * 510)), H_GAUGE_BAR, TRUE, FALSE);//ゲージ

		//降水量表示
		pop = int(100 - gauge_draw_hosei);
		DrawNumber(160, 560, pop, 25, 0, 0, H_POP_NUMBER);



																																																							//コントローラ画像描画
		for (j = 3; j >= 0; j--) {//B
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_B, TRUE);
			if (Key[Button[0][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 3; j >= 0; j--) {//G
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_G, TRUE);
			if (Key[Button[1][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 3; j >= 0; j--) {//R
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_R, TRUE);
			if (Key[Button[2][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}


		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
		if (gauge_draw_counter >= gauge - 0.001) {//曲名の透過度
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - sin((3.14 / 2)*c_m_draw_counter)) * 255));
		}
		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
		ShowExtendedStrFit(640, 350, Music[song_number].title[difficulty], title_width, 620,
		  config, Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//曲名
		ShowExtendedStrFit(640, 450, Music[song_number].artist[difficulty], artist_width, 620, config);//アーティスト
		ShowExtendedStrFit(640, 260, Music[song_number].genre[difficulty], genre_width, 620, config);//ジャンル
		SetDrawMode(DX_DRAWMODE_NEAREST);

		//DrawExtendString(int(640 - ((double)640 / 2)) + 2, 350 + 2, (double)640/title_width, 1, Music[song_number].title[difficulty], GetColor(0, 0, 0));

		//DrawString(int(640 - ((double)title_width / 2)) + 2, 350 + 2, Music[song_number].title[difficulty], GetColor(0, 0, 0));//曲名表示(影)
		//DrawString(int(640 - ((double)artist_width / 2)) + 2, 450 + 2, Music[song_number].artist[difficulty], GetColor(0, 0, 0));//アーティスト表示(影)
		//DrawString(int(640 - ((double)genre_width / 2)) + 2, 260 + 2, Music[song_number].genre[difficulty], GetColor(0, 0, 0));//ジャンル表示(影)

		//DrawExtendString(int(640 - ((double)640 / 2)) , 350 , (double)640 / title_width, 1, Music[song_number].title[difficulty], GetColor(255, 255, 255));

		//DrawString(int(640 - ((double)title_width / 2)), 350, Music[song_number].title[difficulty], GetColor(255, 255, 255));//曲名表示
		//DrawString(int(640 - ((double)artist_width / 2)), 450, Music[song_number].artist[difficulty], GetColor(255, 255, 255));//アーティスト表示
		//DrawString(int(640 - ((double)genre_width / 2)), 260, Music[song_number].genre[difficulty], GetColor(255, 255, 255));//ジャンル表示
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));

		//スコア描画
		if (option->op.color == option->OP_COLOR_RAINBOW) {//虹オプションのときR表示
								 //SetDrawBright(190, 190, 190);
			DrawGraph(960, -3, H_R_OUT, TRUE);
			DrawGraph(960, -3, H_R_IN, TRUE);
			//SetDrawBright(255, 255, 255);
		}

		if (option->op.color != option->OP_COLOR_RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1170 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1170 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		if (option->op.color == option->OP_COLOR_RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1205 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1205 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		//時間描画
		if (PassedTime_Hours <= 9) {//6~9のとき
			DrawGraph(130 - 20, 0, H_TIME_COLON, TRUE);
		}
		else {
			DrawGraph(130, 0, H_TIME_COLON, TRUE);
		}

		if (PassedTime_Hours <= 9) {//6~9のとき
			for (i = 0; i <= 0; i++) {//時間
				DrawExtendGraph(86 - 20 - i * 40, int(0), 86 - 20 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_hours_digit[i]], TRUE);
			}
		}
		else {
			for (i = 0; i <= 1; i++) {//時間
				DrawExtendGraph(86 - i * 40, int(0), 86 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_hours_digit[i]], TRUE);
			}
		}

		if (PassedTime_Hours <= 9) {//6~9のとき
			for (i = 0; i <= 1; i++) {//分
				DrawExtendGraph(214 - 20 - i * 40, int(0), 214 - 20 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_minutes_digit[i]], TRUE);
			}
		}
		else {
			for (i = 0; i <= 1; i++) {//分
				DrawExtendGraph(214 - i * 40, int(0), 214 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_minutes_digit[i]], TRUE);
			}
		}


		//難易度画像描画
		DrawGraph(1020, 260, H_DIFFICULTY, TRUE);

		//スコアグラフ描画
		if (option->op.scoregraph != option->OP_SCORE_GRAPH_OFF) {
			//グラフ名ボックス描画
			DrawBoxWithLine(960, 482, 960 + 80, 482 + 40, GetColor(50, 50, 255), int((double)draw_alpha * 160), int((double)draw_alpha * 255));//現在のスコア
			DrawBoxWithLine(960 + 80, 482, 960 + 80 + 80, 482 + 40, GetColor(50, 255, 50), int((double)draw_alpha * 160), int((double)draw_alpha * 255));//ハイスコア
			DrawBoxWithLine(960 + 160, 482, 960 + 80 + 160, 482 + 40, GetColor(255, 50, 50), int((double)draw_alpha * 160), int((double)draw_alpha * 255));//ターゲットスコア
			DrawBoxWithLine(960 + 240, 482, 960 + 80 + 240, 482 + 40, GetColor(200, 200, 50), int((double)draw_alpha * 160), int((double)draw_alpha * 255));//前回のスコア

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 160));
			//ボーダーライン描画
			for (i = 0; i < 320; i++) {
				DrawPixel(960 + i, 102, GetRainbowColor(i, 320));//S
			}
			DrawLine(960, 122, 1280, 122, GetColor(0, 0, 255));//A
			DrawLine(960, 162, 1280, 162, GetColor(0, 255, 0));//B
			DrawLine(960, 202, 1280, 202, GetColor(255, 0, 0));//C
			DrawLine(960, 242, 1280, 242, GetColor(0, 0, 0));//D

			//スコアグラフ描画
			


			//グラフ全体
			DrawBox(960 + 80, 482, 960 + 80 + 80, 482 - 0.04 * highScore.score * (gauge_draw_counter / 100), GetColor(50, 100, 50), TRUE);
			DrawBox(960 + 160, 482, 960 + 80 + 160, 482 - 0.04 * targetScore * (gauge_draw_counter / 100), GetColor(100, 50, 50), TRUE);
			DrawBox(960 + 240, 482, 960 + 80 + 240, 482 - 0.04 * targetScore2 * (gauge_draw_counter / 100), GetColor(100, 100, 50), TRUE);


		}

		//判定数表示欄
		DrawBoxWithLine(976, 540, 1110, 580, GetColor(50, 255, 255), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//SKY_PERFECT
		DrawBoxWithLine(976, 580, 1110, 620, GetColor(255, 255, 50), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//PERFECT
		DrawBoxWithLine(976, 620, 1110, 660, GetColor(255, 50, 50), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//GOOD
		DrawBoxWithLine(976, 660, 1110, 700, GetColor(50, 50, 255), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//MISS

		//BPM描画欄
		DrawBoxWithLine(1130, 540, 1264, 580, GetColor(255, 50, 50), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//MAXBPM
		DrawBoxWithLine(1130, 580, 1264, 620, GetColor(255, 255, 50), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//BPM
		DrawBoxWithLine(1130, 620, 1264, 660, GetColor(50, 50, 255), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//MINBPM
		DrawBoxWithLine(1130, 660, 1264, 700, GetColor(25, 255, 25), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//SPEED

		//判定、BPM文字表示
		DrawGraph(0, 0, H_GAME_STR_JUDGE_BPM, TRUE);

		//SCORE GRAPHがOFF以外の時グラフ文字表示
		if (option->op.scoregraph != option->OP_SCORE_GRAPH_OFF) {
			DrawGraph(0, 0, H_GAME_STR_SCORE_GRAPH, TRUE);
		}


		//数値表意
		DrawNumber(1094, 536, SKY_PERFECT, 25, 1, 0, H_SMALL_NUMBER_CYAN);
		DrawNumber(1094, 576, PERFECT, 25, 1, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1094, 616, GOOD, 25, 1, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1094, 656, MISS, 25, 1, 0, H_SMALL_NUMBER_BLUE);

		DrawNumber(1216, 536, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1216, 576, int(cbpm + 0.5), 25, 0, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1216, 616, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_SMALL_NUMBER_BLUE);
		DrawNumber(1216, 656, int(cbpm * option->op.speed_val + 0.5), 25, 0, 0, H_SMALL_NUMBER_GREEN);



		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * sin((3.14159265/2)*draw_alpha_speed) * 255));//段位用speed表示透過度
		//段位の時はハイスピ調整用にハイスピがかかった値も表示
		if (SkillTestFlag != 0) {
			if (Key[Keylist[1][0]] == 1 || Key[KEY_INPUT_LEFT] == 1) {//←
				/*
				TempSpeed -= 0.1;
				if (TempSpeed <= 0.1)TempSpeed = 0.1;
				*/
				draw_alpha_speed = 1;
				
				option->op.speed--;
				option->op.speed = number_ring(option->op.speed, option->SPEED_NUM - 1);//0~11に収める
				//Option->op.speed_val = Option->speed_val[Option->op.speed];//実際の倍率を入れる
				if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
					option->op.speed_val = option->speed_val[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
				}
				else {
					option->op.speed_val = 1;
				}
				high_speed = option->op.speed_val;

			}
			if (Key[Keylist[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {//→
				/*
				TempSpeed += 0.1;
				if (TempSpeed >= 4.0)TempSpeed = 4;
				*/
				draw_alpha_speed = 1;
				
				option->op.speed++;
				option->op.speed = number_ring(option->op.speed, option->SPEED_NUM - 1);//0~11に収める
				//Option->op.speed_val = Option->speed_val[Option->op.speed];//実際の倍率を入れる
				if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
					option->op.speed_val = option->speed_val[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
				}
				else {
					option->op.speed_val = 1;
				}
				high_speed = option->op.speed_val;
			}



			wchar_t SpeedStr[128] = L"(0～0,0)";
			int SpeedStrWidth = 0;

			//速さ表示
			sprintfDx(SpeedStr, L"(%d～%d,%d)",
				int(Music[song_number].bpmmin[difficulty] * high_speed + 0.5),
				int(Music[song_number].bpmmax[difficulty] * high_speed + 0.5),
				int(Music[song_number].bpm_suggested[difficulty] * high_speed + 0.5));
			
			SpeedStrWidth = GetDrawStringWidth(SpeedStr, wcslen(SpeedStr));
			ShowExtendedStrFitToHandle(640, 2 + 48 * 14, SpeedStr, SpeedStrWidth, 620, config, FontHandle);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
		//SOUND
		if (jingleflag == 0) {
			PlaySoundMem(SH_JINGLE, DX_PLAYTYPE_BACK, TRUE);
			jingleflag = 1;
		}

		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[2][i]] == 1 && AllowSound[2][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[1][i]] == 1 && AllowSound[1][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[0][i]] == 1 && AllowSound[0][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}

		if ((gauge_draw_counter >= gauge - 0.001) && openflag == 0) {

			openflag = 1;
		}

		ScreenFlip();
    	clsDx();
		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000 / config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}
	}




	//演奏開始
	time_cash = 0;//最初のLOOP_passed_timeが負の値に(-5000とか)にならないように初期化
	GAME_start_time = GetNowCount_d(config);//譜面開始時のカウント
	while (1) {
		//ProcessMessage();
		//note_fall = -92 + 100 * sin(0.0087*GAME_passed_time);
		//judge_area = 390 + 100 * sin(0.006*GAME_passed_time);

		if (ProcessMessage() != 0) {
			turn_off_LED(hComm, &LED_state, &dwWritten, &ovl);//LEDを消す
			DxLib_End();
			exit(0);
			return;
		}

		//----Button----

		Get_Key_State(Buf, Key, AC);

		if (Key[KEY_INPUT_ESCAPE] == 1 && *escape == 0 && ClearFlag == 0 && AllowExit == 1) {
			*escape = 1;
			ClearFlag = 2;
			//c_m_draw_counter = 0;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
		}
		if (Key[Button_Shutter] == 1) {//スクリーンショット
			ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
		}

		//printfDx("%d %d\n",Key[KEY_INPUT_F],Key[KEY_INPUT_G]);

		//----CALC----
		GAME_passed_time = (((double)GetNowCount_d(config) - (double)GAME_start_time) + ((double)debug_time_passed - (double)debug_stop_time));//経過時間計算
		GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
		LOOP_passed_time = ((double)GetNowCount_d(config) - GAME_start_time) - time_cash;//1ループにかかった時間を算出
		time_cash = ((double)GetNowCount_d(config) - GAME_start_time);
		GAME_passed_time_scroll = (sc_timing + ((GAME_passed_time_for_draw - stop_time - stop_time_sum) - real_timing)*(cscroll));

		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		//printfDx("Loop:%f\n", LOOP_passed_time);


		int PressFrame = int(25.0*(17.0 / LOOP_passed_time));//ボタン押し続けてカーソルが動き続けるようになるまでのフレーム
		if (PressFrame <= 0)PressFrame = 1;//0以下にはしない

		if (*debug == 1) {//デバッグモードONなら
			for (i = 0; i < CRTBuf; i++) {
				debug_warp_counter += 2;
			}
			debug_warp = 0;
			if (Key[KEY_INPUT_UP] == 1 || (Key[KEY_INPUT_UP] >= PressFrame && debug_warp_counter >= 20)) {//一拍進む
				debug_warp_counter = 0;
				debug_warp = 1;
				debug_warp_time = (double)60000 / cbpm;
				if (1000 <= debug_warp_time) {//60BPMの1拍以上は進まないようにする
					debug_warp_time = 1000;
				}
				debug_time_passed += debug_warp_time;


				combo = 0;
				*CourseCombo = 0;
				*CourseMaxCombo = 0;
				score = 0;
				SKY_PERFECT = 0;
				PERFECT = 0;
				GOOD = 0;
				MISS = 0;
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
				TimeNextHour = 7;
				MAX_COMBO = 0;
				gauge = 100;

				beat = 0;
				beat_time = 0;
				beat_time_cash = 0;
				beat_percent = 0;
				bgmplay = 0;

				bcc = 0;
				scc = 0;
				cscroll = scrollchange[0].scroll;
				sc_timing = 0;
				real_timing = 0;

				stop_se_c = 0;
				stop_time_sum = 0;
				stop_time = 0;
				stopFlag = 0;

				FullComboFXPlayFlag = 1;
				FullComboFXFrame = 0;

				HitingNoteCount = 0;
				for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
					barline[j].fall = 0;
				}
				for (i = 0; i <= 3; i++) {
					j_n_n[i] = 0;
					j_dn_n[i] = 0;
					j_dn_push_n[i] = 0;
					LN_flag[i] = 0;
					LN_flash[i] = 0;
					for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
						note[i][j].hit = 0;
						note[i][j].fall = 0;
						note[i][j].color = note[i][j].color_init;
						note[i][j].timing = note[i][j].timing_init;
						note[i][j].timing_real = note[i][j].timing_init_real;
						//note[i][j].timing += 240000 / cbpm;
					}
					bcc = 1;
				}
				//printfDx("color:%d,color_init%d\n", note[3][0].color, note[3][0].color_init);
				StopSoundMem(SH_SONG);
				GAME_passed_time = (GetNowCount_d(config) - GAME_start_time) + (debug_time_passed - debug_stop_time);
				GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
			}
			if ((Key[KEY_INPUT_DOWN] == 1 || (Key[KEY_INPUT_DOWN] >= PressFrame && debug_warp_counter >= 20)) && GAME_passed_time >= 0) {//一拍戻る(GAME_passed_time >= 0で戻りすぎ防止)
				debug_warp_counter = 0;
				debug_warp = 1;
				debug_warp_time = (double)60000 / cbpm;
				if (1000 <= debug_warp_time) {//60BPMの1拍以上は進まないようにする
					debug_warp_time = 1000;
				}
				debug_time_passed -= debug_warp_time;
				
				combo = 0;
				*CourseCombo = 0;
				*CourseMaxCombo = 0;
				score = 0;
				SKY_PERFECT = 0;
				PERFECT = 0;
				GOOD = 0;
				MISS = 0;
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
				TimeNextHour = 7;
				MAX_COMBO = 0;
				gauge = 100;

				beat = 0;
				beat_time = 0;
				beat_time_cash = 0;
				beat_percent = 0;

				bgmplay = 0;
				bcc = 0;
				scc = 0;
				cscroll = scrollchange[0].scroll;
				sc_timing = 0;
				real_timing = 0;

				stop_se_c = 0;
				stop_time_sum = 0;
				stop_time = 0;
				stopFlag = 0;

				FullComboFXPlayFlag = 1;
				FullComboFXFrame = 0;
				
				HitingNoteCount = 0;
				for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
					barline[j].fall = 0;
				}
				for (i = 0; i <= 3; i++) {
					j_n_n[i] = 0;
					j_dn_n[i] = 0;
					j_dn_push_n[i] = 0;
					LN_flag[i] = 0;
					LN_flash[i] = 0;
					for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
						note[i][j].hit = 0;
						note[i][j].fall = 0;
						note[i][j].color = note[i][j].color_init;
						note[i][j].timing = note[i][j].timing_init;
						note[i][j].timing_real = note[i][j].timing_init_real;
						//if (i != 0)note[i][j].color = note[i][j].color_init;
						//note[i][j].timing -= 240000 / cbpm;
					}
				}
				bcc = 1;
				StopSoundMem(SH_SONG);
				GAME_passed_time = (GetNowCount_d(config) - GAME_start_time) + (debug_time_passed - debug_stop_time);
				GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
			}

			if (Key[KEY_INPUT_RIGHT] == 1 || (Key[KEY_INPUT_RIGHT] >= PressFrame && debug_warp_counter >= 40)) {//ピッチを上げる
				if (pitch_step <= 25) {//27以上上げると音声が追い付かないので禁止
					pitch_step++;//半音上げる
					pitch *= ROOT12_2;//12乗根2をかける
					if (pitch_step == 0)pitch = 1;//誤差を考慮してpitch_stepが0のときpitchを1にセット
					SetFrequencySoundMem(int(frequency*pitch), SH_SONG);

					debug_warp = 1;
					debug_time_passed -= GAME_passed_time - GAME_passed_time / ROOT12_2;//再生位置のずれを補正 GAME_passed_time-GAME_passed_time/ROOT12_2が差 
					GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScroll,&playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
					Music[song_number].hash[difficulty] = hash;
					res->hash = hash;//ハッシュ値を格納
					score_note = (double)score_MAX / Music[song_number].total_note[difficulty];//音符一つの得点を決める
					HourOfGame = TimeToEndScroll / 18;//左上時間の1時間の単位を決定
					MinuteOfGame = HourOfGame / 60;//分を決める

					if (playing_time > GAUGE_INC_BASE_TIME) {//120秒以上の譜面なら
						gauge_inc_times = (double)playing_time / (double)GAUGE_INC_BASE_TIME;
					}
					if (SkillTestFlag != 0) {//段位補正
						gauge_inc_times *= (double)Music[song_number].total_note[difficulty] / SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT;
					}
					total = total_base*gauge_inc_times;

					gauge_dec = ((double)100 / (miss_percent*Music[song_number].total_note[difficulty]));
					if (gauge_dec >= gauge_dec_MAX)gauge_dec = gauge_dec_MAX;
					if (gauge_dec <= gauge_dec_MIN)gauge_dec = gauge_dec_MIN;
					cbpm = Music[song_number].bpm[difficulty];//最初のBPM
					
					combo = 0;
					*CourseCombo = 0;
					*CourseMaxCombo = 0;
					score = 0;
					SKY_PERFECT = 0;
					PERFECT = 0;
					GOOD = 0;
					MISS = 0;
					TimePerfect = 0;
					TimeGood = 0;
					TimeMiss = 0;
					TimeNextHour = 7;
					MAX_COMBO = 0;
					gauge = 100;
					bgmplay = 0;
					bcc = 0;
					scc = 0;
					cscroll = scrollchange[0].scroll;
					sc_timing = 0;
					real_timing = 0;

					stop_se_c = 0;
					stop_time_sum = 0;
					stop_time = 0;
					stopFlag = 0;

					FullComboFXPlayFlag = 1;
					FullComboFXFrame = 0;

					HitingNoteCount = 0;
					for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
						barline[j].fall = 0;
					}
					for (i = 0; i <= 3; i++) {
						j_n_n[i] = 0;
						j_dn_n[i] = 0;
						j_dn_push_n[i] = 0;
						LN_flag[i] = 0;
						LN_flash[i] = 0;
						for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
							note[i][j].hit = 0;
							note[i][j].fall = 0;
							note[i][j].color = note[i][j].color_init;
							//if (i != 0)note[i][j].color = note[i][j].color_init;
							//note[i][j].timing -= 240000 / cbpm;
						}
					}
					bcc = 1;
					StopSoundMem(SH_SONG);
					GAME_passed_time = (GetNowCount_d(config) - GAME_start_time) + (debug_time_passed - debug_stop_time);
					GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
				}
			}
			if (Key[KEY_INPUT_LEFT] == 1 || (Key[KEY_INPUT_LEFT] >= PressFrame && debug_warp_counter >= 40)) {//ピッチを下げる
				if (pitch_step >= -47) {//48以上下げるのは禁止
					pitch_step--;//半音下げる
					pitch /= ROOT12_2;//12乗根2を割る
					if (pitch_step == 0)pitch = 1;//誤差を考慮してpitch_stepが0のときpitchを1にセット
					SetFrequencySoundMem(int(frequency*pitch), SH_SONG);

					debug_warp = 1;
					debug_time_passed -= GAME_passed_time - GAME_passed_time * ROOT12_2;//再生位置のずれを補正 GAME_passed_time - GAME_passed_time * ROOT12_2が差 
					GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScroll,&playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
					Music[song_number].hash[difficulty] = hash;
					res->hash = hash;//ハッシュ値を格納
					score_note = (double)score_MAX / Music[song_number].total_note[difficulty];//音符一つの得点を決める
					HourOfGame = TimeToEndScroll / 18;//左上時間の1時間の単位を決定
					MinuteOfGame = HourOfGame / 60;//分を決める

					if (playing_time > GAUGE_INC_BASE_TIME) {//120秒以上の譜面なら
						gauge_inc_times = (double)playing_time / (double)GAUGE_INC_BASE_TIME;
					}
					if (SkillTestFlag != 0) {//段位補正
						gauge_inc_times *= (double)Music[song_number].total_note[difficulty] / SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT;
					}
					total = total_base*gauge_inc_times;

					gauge_dec = ((double)100 / (miss_percent*Music[song_number].total_note[difficulty]));
					if (gauge_dec >= gauge_dec_MAX)gauge_dec = gauge_dec_MAX;
					if (gauge_dec <= gauge_dec_MIN)gauge_dec = gauge_dec_MIN;
					cbpm = Music[song_number].bpm[difficulty];//最初のBPM
					
					combo = 0;
					*CourseCombo = 0;
					*CourseMaxCombo = 0;
					score = 0;
					SKY_PERFECT = 0;
					PERFECT = 0;
					GOOD = 0;
					MISS = 0;
					TimePerfect = 0;
					TimeGood = 0;
					TimeMiss = 0;
					TimeNextHour = 7;
					MAX_COMBO = 0;
					gauge = 100;
					bgmplay = 0;
					bcc = 0;
					scc = 0;
					cscroll = scrollchange[0].scroll;
					sc_timing = 0;
					real_timing = 0;

					stop_se_c = 0;
					stop_time_sum = 0;
					stop_time = 0;
					stopFlag = 0;

					FullComboFXPlayFlag = 1;
					FullComboFXFrame = 0;

					HitingNoteCount = 0;
					for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
						barline[j].fall = 0;
					}
					for (i = 0; i <= 3; i++) {
						j_n_n[i] = 0;
						j_dn_n[i] = 0;
						j_dn_push_n[i] = 0;
						LN_flag[i] = 0;
						LN_flash[i] = 0;
						for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
							note[i][j].hit = 0;
							note[i][j].fall = 0;
							note[i][j].color = note[i][j].color_init;
							//if (i != 0)note[i][j].color = note[i][j].color_init;
							//note[i][j].timing -= 240000 / cbpm;
						}
					}
					bcc = 1;
					StopSoundMem(SH_SONG);
					GAME_passed_time = (GetNowCount_d(config) - GAME_start_time) + (debug_time_passed - debug_stop_time);
					GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
				}
			}

			if (Key[KEY_INPUT_SPACE] == 1) {//再生停止(開始)
				if (debug_stop == 0) {
					debug_stop = 1;
					debug_stoped_time = GetNowCount_d(config);
					bgmplay = 0;
				}
				else {
					if (debug_stop == 1) {
						debug_stop = 0;
						debug_time_passed -= debug_stop_time;
						debug_stop_time = 0;
					}
				}
				StopSoundMem(SH_SONG);
			}

			if (debug_stop == 1) {
				debug_stop_time = GetNowCount_d(config) - debug_stoped_time;
				//printfDx("%d\n", debug_stop_time);
			}

			if (Key[KEY_INPUT_F1] == 1) {//オートプレイ開始(停止)
				if (debug_auto == 0) {
					debug_auto = 1;
				}
				else {
					if (debug_auto == 1) {
						debug_auto = 0;
						turn_off_LED(hComm, &LED_state, &dwWritten, &ovl);//LEDを消す
					}
				}
			}


			if (Key[KEY_INPUT_F2] == 1) {//譜面再読み込み
				debug_warp = 1;
				GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScroll, &playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
				Music[song_number].hash[difficulty] = hash;
				res->hash = hash;//ハッシュ値を格納
				score_note = (double)score_MAX / Music[song_number].total_note[difficulty];//音符一つの得点を決める
				HourOfGame = TimeToEndScroll / 18;//左上時間の1時間の単位を決定
				MinuteOfGame = HourOfGame / 60;//分を決める

				if (playing_time > GAUGE_INC_BASE_TIME) {//120秒以上の譜面なら
					gauge_inc_times = (double)playing_time / (double)GAUGE_INC_BASE_TIME;
				}
				if (SkillTestFlag != 0) {//段位補正
					gauge_inc_times *= (double)Music[song_number].total_note[difficulty] / SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT;
				}
				total = total_base*gauge_inc_times;

				gauge_dec = ((double)100 / (miss_percent*Music[song_number].total_note[difficulty]));
				if (gauge_dec >= gauge_dec_MAX)gauge_dec = gauge_dec_MAX;
				if (gauge_dec <= gauge_dec_MIN)gauge_dec = gauge_dec_MIN;
				cbpm = Music[song_number].bpm[difficulty];//最初のBPM
				
				combo = 0;
				*CourseCombo = 0;
				*CourseMaxCombo = 0;
				score = 0;
				SKY_PERFECT = 0;
				PERFECT = 0;
				GOOD = 0;
				MISS = 0;
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
				TimeNextHour = 7;
				MAX_COMBO = 0;
				gauge = 100;
				bgmplay = 0;
				bcc = 0;
				scc = 0;
				cscroll = scrollchange[0].scroll;
				sc_timing = 0;
				real_timing = 0;

				stop_se_c = 0;
				stop_time_sum = 0;
				stop_time = 0;
				stopFlag = 0;

				FullComboFXPlayFlag = 1;
				FullComboFXFrame = 0;

				HitingNoteCount = 0;
				for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
					barline[j].fall = 0;
				}
				for (i = 0; i <= 3; i++) {
					j_n_n[i] = 0;
					j_dn_n[i] = 0;
					j_dn_push_n[i] = 0;
					LN_flag[i] = 0;
					LN_flash[i] = 0;
					for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
						note[i][j].hit = 0;
						note[i][j].fall = 0;
						note[i][j].color = note[i][j].color_init;
						//if (i != 0)note[i][j].color = note[i][j].color_init;
						//note[i][j].timing -= 240000 / cbpm;
					}
				}
				bcc = 1;
				StopSoundMem(SH_SONG);
				GAME_passed_time = (GetNowCount_d(config) - GAME_start_time) + (debug_time_passed - debug_stop_time);
				GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
			}

			if (Key[KEY_INPUT_F3] == 1) {//叩いた音を鳴らさない
				if (debug_sound == 0) {
					debug_sound = 1;
				}
				else {
					if (debug_sound == 1) {
						debug_sound = 0;
					}
				}
			}
			if (Key[KEY_INPUT_F4] == 1) {//FAILEDにならないようにする
				if (debug_fail == 0) {
					debug_fail = 1;
				}
				else {
					if (debug_fail == 1) {
						debug_fail = 0;
					}
				}
			}

			if (Key[KEY_INPUT_F5] == 1) {//曲の音量を0にする
				if (debug_music == 0) {
					debug_music = 1;
				}
				else {
					if (debug_music == 1) {
						debug_music = 0;
					}
				}
			}

			if (Key[KEY_INPUT_F11] == 1) {//エディタ画面に移行


				if (debug_stop == 0) {//止めてなかったら止める処理を行う
					debug_stop = 1;
					debug_stoped_time = GetNowCount_d(config);
					bgmplay = 0;
					StopSoundMem(SH_SONG);
				}
				double EDIT_passed_time = (GAME_passed_time - Music[song_number].noteoffset[difficulty])*pitch;
				if (EDIT_passed_time < 0) {
					EDIT_passed_time = 0;
				}

				GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, &hash, Music, &TimeToEndScroll, &playing_time, config, 1, &score_cell_head);//score_cell_headに譜面情報をロード
				
				double debug_time_passed_in_EDIT_SCORE = debug_time_passed * pitch;//再生速度を変えている分debug_time_passedが変化しているのでEDIT_SCOREに渡すときに補正しておく
				
				EDIT_SCORE(&score_cell_head,
					Music, 
					option,
					song_number, 
					difficulty,
					scrollchange[0].scroll, 
					Button, Button_Shutter, Key, Buf,
					EDIT_passed_time,
					&debug_time_passed_in_EDIT_SCORE,
					&scale_score,
					&step_array_number,
					&measure_start_step,
					&SwingBackBeatTime,
					AC, config);
	
				debug_time_passed = debug_time_passed_in_EDIT_SCORE / pitch;//今の再生速度での時間に戻す
				debug_stop_time = GetNowCount_d(config) - debug_stoped_time;//debug_stop_timeの更新ができていなかったのでする

				//譜面再読み込み
				debug_warp = 1;
				GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, &hash, Music, &TimeToEndScroll, &playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
				Music[song_number].hash[difficulty] = hash;
				res->hash = hash;//ハッシュ値を格納
				score_note = (double)score_MAX / Music[song_number].total_note[difficulty];//音符一つの得点を決める
				HourOfGame = TimeToEndScroll / 18;//左上時間の1時間の単位を決定
				MinuteOfGame = HourOfGame / 60;//分を決める

				if (playing_time > GAUGE_INC_BASE_TIME) {//120秒以上の譜面なら
					gauge_inc_times = (double)playing_time / (double)GAUGE_INC_BASE_TIME;
				}
				if (SkillTestFlag != 0) {//段位補正
					gauge_inc_times *= (double)Music[song_number].total_note[difficulty] / SKILL_TEST_GAUGE_INC_BASE_NOTES_COUNT;
				}
				total = total_base*gauge_inc_times;

				gauge_dec = ((double)100 / (miss_percent*Music[song_number].total_note[difficulty]));
				if (gauge_dec >= gauge_dec_MAX)gauge_dec = gauge_dec_MAX;
				if (gauge_dec <= gauge_dec_MIN)gauge_dec = gauge_dec_MIN;
				cbpm = Music[song_number].bpm[difficulty];//最初のBPM
				combo = 0;
				*CourseCombo = 0;
				*CourseMaxCombo = 0;
				score = 0;
				SKY_PERFECT = 0;
				PERFECT = 0;
				GOOD = 0;
				MISS = 0;
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
				TimeNextHour = 7;
				MAX_COMBO = 0;
				gauge = 100;
				bgmplay = 0;
				bcc = 0;
				scc = 0;
				cscroll = scrollchange[0].scroll;
				sc_timing = 0;
				real_timing = 0;

				stop_se_c = 0;
				stop_time_sum = 0;
				stop_time = 0;
				stopFlag = 0;

				FullComboFXPlayFlag = 1;
				FullComboFXFrame = 0;

				HitingNoteCount = 0;
				for (j = 0; j <= BARLINE_MAX_NUMBER - 1; j++) {
					barline[j].fall = 0;
				}
				for (i = 0; i <= 3; i++) {
					j_n_n[i] = 0;
					j_dn_n[i] = 0;
					j_dn_push_n[i] = 0;
					LN_flag[i] = 0;
					LN_flash[i] = 0;
					for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
						note[i][j].hit = 0;
						note[i][j].fall = 0;
						note[i][j].color = note[i][j].color_init;
						//if (i != 0)note[i][j].color = note[i][j].color_init;
						//note[i][j].timing -= 240000 / cbpm;
					}
				}
				bcc = 1;

				GAME_passed_time = (((double)GetNowCount_d(config) - (double)GAME_start_time) + ((double)debug_time_passed - (double)debug_stop_time));//経過時間計算
				GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
				LOOP_passed_time = ((double)GetNowCount_d(config) - GAME_start_time) - time_cash;//1ループにかかった時間を算出
				time_cash = ((double)GetNowCount_d(config) - GAME_start_time);
				GAME_passed_time_scroll = (sc_timing + ((GAME_passed_time_for_draw - stop_time - stop_time_sum) - real_timing)*(cscroll));

			}
		}
		//scrollchangeのインクリメント
		if (debug_warp == 0) {//再生位置移動してないとき
			while ((GAME_passed_time_for_draw >= scrollchange[scc].timing_real) && (scrollchange[scc].use == 1)) {//scroll変更
				real_timing += (((double)scrollchange[scc].timing - (double)sc_timing)) / cscroll;
				cscroll = scrollchange[scc].scroll;
				sc_timing = scrollchange[scc].timing;
				scc++;
				//printfDx("change\n");
			}
		}
		if (debug_warp == 1) {//再生位置移動したとき(cscrollの値を決める)
			while ((GAME_passed_time_for_draw >= scrollchange[scc].timing_real) && (scrollchange[scc].use == 1)) {//scroll変更
				real_timing += (((double)scrollchange[scc].timing - (double)sc_timing)) / cscroll;
				cscroll = scrollchange[scc].scroll;
				sc_timing = scrollchange[scc].timing;
				scc++;
				//printfDx("change\n");
			}
		}


		//stopSequenceのカウンタstop_se_cのインクリメント
		stopSequence[stop_se_c].timing_real;
		if (debug_warp == 0) {//再生位置移動してないとき
			while ((GAME_passed_time_for_draw >= stopSequence[stop_se_c].timing_real) && (stopSequence[stop_se_c].use == 1)) {//stop変更
				stop_se_c++;
				stop_time;
				stopFlag = 1;
				//printfDx("change\n");

			}
		}
		if (debug_warp == 1) {//再生位置移動したとき
			while ((GAME_passed_time_for_draw >= stopSequence[stop_se_c].timing_real) && (stopSequence[stop_se_c].use == 1)) {//stop変更
				stopFlag = 1;
				if (GAME_passed_time_for_draw >= stopSequence[stop_se_c].stop_time * 1000 + stopSequence[stop_se_c].timing_real) {//譜面停止時間を過ぎた
					stop_time_sum += stopSequence[stop_se_c].stop_time * 1000;
					stopFlag = 0;
				}
				stop_se_c++;

			}
			if (stopFlag == 1) {//譜面停止中
				if (GAME_passed_time_for_draw >= stopSequence[stop_se_c - 1].stop_time * 1000 + stopSequence[stop_se_c - 1].timing_real) {//譜面停止時間を過ぎた
					stop_time_sum += stopSequence[stop_se_c - 1].stop_time * 1000;//止まった分は足し重ねていく
					stop_time = 0;
					stopFlag = 0;//停止終わり
				}
			}
		}

		/*
		printfDx("stop[0]:%d\n", stopSequence[0].timing_real);
		printfDx("stop[1]:%d\n", stopSequence[1].timing_real);
		printfDx("stop[2]:%d\n", stopSequence[2].timing_real);
		printfDx("stop_time:%f\n", stop_time);
		printfDx("stop_time_sum:%f\n", stop_time_sum);
		*/


		GAME_passed_time = (((double)GetNowCount_d(config) - (double)GAME_start_time) + ((double)debug_time_passed - (double)debug_stop_time));//経過時間計算
		GAME_passed_time_for_draw = GAME_passed_time + config.DisplayTimingOffset;//ディスプレイ遅延補正用経過時間計算
	
		if (stopFlag == 1) {//譜面停止中
			stop_time = GAME_passed_time_for_draw - stopSequence[stop_se_c - 1].timing_real;
			if (GAME_passed_time_for_draw >= stopSequence[stop_se_c - 1].stop_time * 1000 + stopSequence[stop_se_c - 1].timing_real) {//譜面停止時間を過ぎた
				stop_time_sum += stopSequence[stop_se_c - 1].stop_time * 1000;//止まった分は足し重ねていく
				stop_time = 0;
				stopFlag = 0;//停止終わり
			}
		}

		//GAME_passed_time_scroll = (sc_timing + ((GAME_passed_time - stop_time - stop_time_sum) - real_timing)*(cscroll));

		GAME_passed_time_scroll = (sc_timing + ((GAME_passed_time_for_draw - stop_time - stop_time_sum) - real_timing)*(cscroll));

		//----ノート判定処理----


		if (ClearFlag == 0) {//クリア状態が決まってないときにノート判定を実行

			for (i = 0; i <= 3; i++) {//レーン
				for (j = 0; j <= 2; j++) {//色
					if (Key[Keylist[j][i]] == 1) {	
						int judge_dark = 0;
						GAME_judge_note(j_n_n[i], note, i, int(GAME_passed_time), int(judge_time_bad), Colorlist[j][0], Colorlist[j][1], Colorlist[j][2], Colorlist[j][3], Colorlist[j][4], Colorlist[j][5], Colorlist[j][6], 9, &timingDifference, LN_flag, &searching);//LN始点叩くとLN_flag変化
						
						if (abs(timingDifference) <= judge_time_good)AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない

						if (abs(timingDifference) <= judge_time_good && note[i][searching].hit) {//ノートを叩いた(abs(timingDifference)は判定が行われなかったとき10000になる) LN始点の場合はこのifは入らない(hitが0)
							note_hit_flash[note_hit_flash_rounder][i] = 11;//ノートを叩いた時の通常フラッシュ
							note_hit_flash_rounder = (note_hit_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す

							volume = 1;
							combo++;
							HitingNoteCount++;
							(*CourseCombo)++;
							combo_draw_counter = 1;
							AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
							if (abs(timingDifference) <= judge_time_perfect) {//PERFECT以内
								if (note[i][searching].snd != 0) {
									note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
									note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
								}

								gauge += total / Music[song_number].total_note[difficulty];
								if (note[i][searching].snd != 0) {//光るノートなら2倍
									gauge += total / Music[song_number].total_note[difficulty];
								}

								if (abs(timingDifference) <= judge_time_sky_perfect) {//SKY_PERFECT
									hit_sky_perfect[i] = 1;
									score += score_note + 1;
									SKY_PERFECT++;
									gauge += 0.1;
								}
								else {//PERFECT
									hit_perfect[i] = 1;
									score += score_note - 1;
									PERFECT++;

									if (timingDifference < 0) {
										I_Fast[i].appear(GAME_passed_time);
										I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Fast[i].disappear(GAME_passed_time, 200);
									}
									else {
										I_Slow[i].appear(GAME_passed_time);
										I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Slow[i].disappear(GAME_passed_time, 200);
									}
									

								}
								TimePerfect++;


								if (debug_sound == 1) {
									PlayHitSound(2, note[i][searching].color_init, note[i][searching].snd, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								hit_n[i] = note[i][searching];

								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (lane[i] + 128 - 640) * 2),
									rand() % 10000,
									rand() % 10000,
									rand() % 401,
									0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
								insert = insert->next;//挿入する場所を後ろにする
								if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする

							}
							else {//GOOD
								hit_good[i] = 1;
								gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
								if (note[i][searching].snd != 0) {//光るノートなら2倍
									gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
								}

								score += (double)0.5* score_note;
								GOOD++;
								TimeGood++;

								if (timingDifference < 0) {
									I_Fast[i].appear(GAME_passed_time);
									I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Fast[i].disappear(GAME_passed_time, 200);
								}
								else {
									I_Slow[i].appear(GAME_passed_time);
									I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Slow[i].disappear(GAME_passed_time, 200);
								}

								if (debug_sound == 1) {
									PlayHitSound(1, note[i][searching].color_init, note[i][searching].snd, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								hit_n[i] = note[i][searching];
								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (lane[i] + 128 - 640) * 2),
									rand() % 10000,
									rand() % 10000,
									rand() % 401,
									0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
								insert = insert->next;//挿入する場所を後ろにする
								if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
							}
						}
						else {//LN始点もこのループは入る
							if ((abs(timingDifference) <= judge_time_bad) && (abs(timingDifference) >= judge_time_good)) {//早く,遅く叩き過ぎた(LNも)前の (MISS)
								volume = 0;
								if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
								if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新
								combo = 0;
								HitingNoteCount++;
								*CourseCombo = 0;
								combo_draw_counter = 0;
								hit_miss[i] = 1;
								
								double gauge_dec_ratio = 1;
								if (note[i][searching].color_init == 1 || note[i][searching].color_init == 2 || note[i][searching].color_init == 3 || note[i][searching].color_init == 9) {//RGBFは一倍
									gauge_dec_ratio = 1;
								}
								else if (note[i][searching].color_init == 4 || note[i][searching].color_init == 5 || note[i][searching].color_init == 6) {//CMYは減少量0.5倍
									gauge_dec_ratio = 0.5;
								}
								else if (note[i][searching].color_init == 7) {//Wは減少量1/3倍
									gauge_dec_ratio = 0.3333334;
								}

								gauge -= gauge_dec * gauge_dec_ratio;
								if (note[i][searching].snd != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec * gauge_dec_ratio;
								}

								score -= score_dec;
								MISS++;
								TimeMiss++;
								AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない

								if (timingDifference < 0) {
									I_Fast[i].appear(GAME_passed_time);
									I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Fast[i].disappear(GAME_passed_time, 200);
								}
								else {
									I_Slow[i].appear(GAME_passed_time);
									I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Slow[i].disappear(GAME_passed_time, 200);
								}
							}
						}

						if (note[i][j_n_n[i]].group == 1 && LN_flag[i] == 2) {
							j_n_n[i]++;//LN始点を叩いた時ノートカウンタを1上げる
							if (abs(timingDifference) <= judge_time_perfect) {//PERFECT以内
								if (abs(timingDifference) <= judge_time_sky_perfect) {//SKY_PERFECT
									LN_judge[i] = 3;
								}
								else {
									LN_judge[i] = 2;

									if (timingDifference < 0) {
										I_Fast[i].appear(GAME_passed_time);
										I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Fast[i].disappear(GAME_passed_time, 200);
									}
									else {
										I_Slow[i].appear(GAME_passed_time);
										I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Slow[i].disappear(GAME_passed_time, 200);
									}
								}

								//PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);

								if (debug_sound == 1) {
									PlayHitSound(2, note[i][searching].color_init, note[i][searching].snd, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								//hit_n[i] = note[i][searching];


							}
							else {//GOODでLN始点を叩いた
								if (abs(timingDifference) <= judge_time_good) {
									LN_judge[i] = 1;

									if (timingDifference < 0) {
										I_Fast[i].appear(GAME_passed_time);
										I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Fast[i].disappear(GAME_passed_time, 200);
									}
									else {
										I_Slow[i].appear(GAME_passed_time);
										I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
										I_Slow[i].disappear(GAME_passed_time, 200);
									}

									if (debug_sound == 1) {
										PlayHitSound(1, note[i][searching].color_init, note[i][searching].snd, SH);
										AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
									}
									//hit_n[i] = note[i][searching];


								}
							}
							if ((abs(timingDifference) <= judge_time_bad) && (abs(timingDifference) >= judge_time_good)) {//遅く、早く叩き過ぎた(MISS)
								LN_judge[i] = 0;
								LN_flag[i] = 1;
								
								if (timingDifference < 0) {
									I_Fast[i].appear(GAME_passed_time);
									I_Fast[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Fast[i].disappear(GAME_passed_time, 200);
								}
								else {
									I_Slow[i].appear(GAME_passed_time);
									I_Slow[i].move(lane[i], judge_area + fastSlowY, lane[i], judge_area + fastSlowY - fastSlowYMove, Abs, ConvertMode::QuarterSine, GAME_passed_time, 200);
									I_Slow[i].disappear(GAME_passed_time, 200);
								}
							}


						}

						if (timingDifference == -10000) {//判定関数で黒を除き何も反応しなかったとき
							judge_dark = 1;//このフレームの入力で黒の判定を行う
						}

						if (debug_warp != 1 && judge_dark == 1) {//ワープ時には黒の判定やらず、このフレームで普通の音符が反応していない(judge_dark==1)とき黒音符の叩き判定をする
							GAME_judge_dark_note(j_dn_n[i], note, i, int(GAME_passed_time), int(judge_time_dark), 8, &dark_hit, &searching);
							if (dark_hit == 1) {//黒を叩いた
								dnote_hit_flash[i] = 11;
								volume = 0;
								if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
								combo = 0;
								*CourseCombo = 0;
								combo_draw_counter = 0;
								hit_miss[i] = 1;
								gauge -= gauge_dec;
								score -= score_dec;
								MISS++;
								TimeMiss++;
								PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
								hit_n[i] = note[i][searching];
							}
						}
					}
				}

				if (LN_flag[i] != 0 && note[i][j_n_n[i] - 1].group == 1) {//iレーンのLNが判定枠に重なっているとき(かつjnnがLNをしっかり指している)
							
					//LN_pushフラグの計算 対応する色を押しているときは1,離したフレームは-1,それ以外の離しているとき0
					if (((note[i][j_n_n[i] - 1].color_init == 1 && Key[Keylist[2][i]] >= 1)//R
						|| (note[i][j_n_n[i] - 1].color_init == 2 && Key[Keylist[1][i]] >= 1)//G
						|| (note[i][j_n_n[i] - 1].color_init == 3 && Key[Keylist[0][i]] >= 1)//B
						|| (note[i][j_n_n[i] - 1].color_init == 4 && Key[Keylist[2][i]] >= 1 && Key[Keylist[1][i]] >= 1)//Y
						|| (note[i][j_n_n[i] - 1].color_init == 5 && Key[Keylist[1][i]] >= 1 && Key[Keylist[0][i]] >= 1)//C
						|| (note[i][j_n_n[i] - 1].color_init == 6 && Key[Keylist[0][i]] >= 1 && Key[Keylist[2][i]] >= 1)//M
						|| (note[i][j_n_n[i] - 1].color_init == 7 && Key[Keylist[2][i]] >= 1 && Key[Keylist[1][i]] >= 1 && Key[Keylist[0][i]] >= 1)//W
						|| (note[i][j_n_n[i] - 1].color_init == 9 && (Key[Keylist[2][i]] >= 1 || Key[Keylist[1][i]] >= 1 || Key[Keylist[0][i]] >= 1)))//F
						) {
						LN_push[i] = 1;
					}
					else if (LN_push[i] == 1) {
						LN_push[i] = -1;
					}
					else {
						LN_push[i] = 0;
					}

					if (note[i][j_n_n[i]].timing_real - GAME_passed_time <= judge_time_good && note[i][j_n_n[i]].LN_k == 1 && LN_push[i] == -1) {//黒終端が猶予時間判定内に来ているときに離したとき
						LN_flag[i] = -1;//LN中だけどもう離していいことにする(押し切ったことにする)
						PlaySoundMem(SH.SH_RELEASE, DX_PLAYTYPE_BACK, TRUE);//黒終端リリース音
					}

					if (LN_push[i] == 1
						|| LN_flag[i] == -1//(押し切った状態に既になっているとき(LN_flag[i] == -1)は押してなくても始点が判定枠で止まる)
						|| debug_stop == 1 || debug_auto == 1) {//autoなら常に押し続けている)

						note[i][j_n_n[i] - 1].timing = int(GAME_passed_time_scroll * TIMING_SHOW_RATIO);//始点を判定枠に表示し続けるために現在の経過時間をtimingに入れる
						note[i][j_n_n[i] - 1].timing_real = int(GAME_passed_time);//ちょっと離しても大丈夫にするため現在の経過時間(本当)をtiming_realに入れる


						if (note[i][j_n_n[i]].timing_real - GAME_passed_time <= judge_time_delay && note[i][j_n_n[i]].LN_k == 0) {//通常のLN終点が猶予時間判定内に来たとき
							LN_flag[i] = -1;//LN中だけどもう離していいことにする(押し切ったことにする)
						}

						LN_flash[i] = 1;//LNを光らせる
					}
					else {//LN中なのに押してないときで押し切ってもいない(LN_flag[i]!=-1)
						if ((GAME_passed_time - note[i][j_n_n[i] - 1].timing_real > judge_time_delay)) {//猶予時間を超えた時
							if (LN_flag[i] == 2) {//初回(LNでの最初のミスはノート1個分のミスとして扱う)
								LN_flag[i] = 1;
								gauge -= gauge_dec;
								if (note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec;
								}
								score -= score_dec;
								MISS++;
								TimeMiss++;
								HitingNoteCount++;
							}
							LN_judge[i] = 0;
							LN_flash[i] = 0;//LNを光らせない
							volume = 0;
							if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
							if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新

							combo = 0;
							*CourseCombo = 0;
							combo_draw_counter = 0;
							hit_miss[i] = 1;
							for (k = 0; k < CRTBuf; k++) {
								gauge -= gauge_dec / 500;
								if (note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec / 500;
								}
								score -= score_dec / 500;
							}


						}
					}

					if (GAME_passed_time - note[i][j_n_n[i]].timing_real > 0) {//LN終点が判定枠を超えて来たとき
						if (note[i][j_n_n[i]].LN_k == 0 || (note[i][j_n_n[i]].LN_k == 1 && (LN_flag[i] == 1 ||LN_flag[i] == -1) && LN_push[i] <= 0)) {//終端が通常終端 または黒終端で既に消していい状態になっていて(LN_flag[i] == 1か-1)対応する色を押していないとき

							if ((LN_judge[i] == 2 || LN_judge[i] == 3) && debug_auto != 1) {//PERFECT以上
								LN_flag[i] = -1;//押し切った

								TimePerfect++;

								if (note[i][j_n_n[i] - 1].snd != 0) {
									note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
									note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
								}

								gauge += total / Music[song_number].total_note[difficulty];
								if (note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍
									gauge += total / Music[song_number].total_note[difficulty];
								}


								volume = 1;
								combo++;
								(*CourseCombo)++;
								combo_draw_counter = 1;

								if (LN_judge[i] == 3) {//SKY_PERFECT
									SKY_PERFECT++;
									score += score_note + 1;
									hit_sky_perfect[i] = 1;
									gauge += 0.1;
								}
								else {//PERFECT
									PERFECT++;
									score += score_note - 1;
									hit_perfect[i] = 1;
								}

								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (lane[i] + 128 - 640) * 2),
									rand() % 10000,
									rand() % 10000,
									rand() % 401,
									0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
								insert = insert->next;//挿入する場所を後ろにする
								if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
																		 //hit_n[i] = note[i][j_n_n[i]];
								HitingNoteCount++;
							}
							if (LN_judge[i] == 1 && debug_auto != 1) {//GOOD
								LN_flag[i] = -1;//押し切った
								GOOD++;
								TimeGood++;
								hit_good[i] = 1;
								gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
								if (note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍
									gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
								}
								score += (double)0.5 * score_note;
								volume = 1;
								combo++;
								(*CourseCombo)++;
								combo_draw_counter = 1;

								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (lane[i] + 128 - 640) * 2),
									rand() % 10000,
									rand() % 10000,
									rand() % 401,
									0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
								insert = insert->next;//挿入する場所を後ろにする
								if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
																		 //hit_n[i] = note[i][j_n_n[i]];
								HitingNoteCount++;
							}

							if (LN_flag[i] == -1) {//押し切っているときノートを消す(hit=1にする)
								note[i][j_n_n[i] - 1].hit = 1;
								note[i][j_n_n[i]].hit = 1;
								note_hit_flash[note_hit_flash_rounder][i] = 11;//ノートを叩いた時の通常フラッシュ
								note_hit_flash_rounder = (note_hit_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
								hit_n[i] = note[i][j_n_n[i]];

							}

							LN_flag[i] = 0;
							LN_flash[i] = 0;
							LN_judge[i] = 0;
						}
						else {//黒終点でLN終端が判定枠通り過ぎてるのに対応する色を押し続けているとき
							note[i][j_n_n[i]].timing = int(GAME_passed_time_scroll * TIMING_SHOW_RATIO);//終点を判定枠に表示し続けるために現在の経過時間をtimingに入れる
							note[i][j_n_n[i]].timing_real = int(GAME_passed_time);//ちょっと離しても大丈夫にするため現在の経過時間(本当)をtiming_realに入れる


							if ((note[i][j_n_n[i]].timing_real - note[i][j_n_n[i]].timing_init_real) >= judge_time_good) {//終端が後ろにずれて本来の場所よりGOOD判定幅分ずれたら押し過ぎたとしてミスにする
								if (debug_sound == 1)PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);//黒ヒット音
								hit_miss[i] = 1;

								gauge -= gauge_dec;
								if (note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec;
								}
								score -= score_dec;
								MISS++;
								TimeMiss++;
							
								volume = 0;
								if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
								if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新

								combo = 0;
								*CourseCombo = 0;
								combo_draw_counter = 0;

								//ノートを消す
								note[i][j_n_n[i] - 1].hit = 1;
								note[i][j_n_n[i]].hit = 1;
								dnote_hit_flash[i] = 11;
								HitingNoteCount++;

								LN_flag[i] = 0;
								LN_flash[i] = 0;
								LN_judge[i] = 0;
							}
						}
					}

				}

			}

			if (debug_warp != 1) {//ワープ時には黒の判定やらない
				for (i = 0; i <= 3; i++) {
					if (Key[Button[2][i]] > 1 || Key[Button[1][i]] >= 1 || Key[Button[0][i]] >= 1) {
						GAME_judge_dark_note(j_dn_push_n[i], note, i, int(GAME_passed_time), int(con_jd), 8, &dark_hit, &searching);
						if (dark_hit == 1) {//黒が判定された
							dnote_hit_flash[i] = 11;
							volume = 0;
							if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
							if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新

							combo = 0;
							*CourseCombo = 0;
							combo_draw_counter = 0;
							hit_miss[i] = 1;
							gauge -= gauge_dec;
							score -= score_dec;
							MISS++;
							TimeMiss++;
							if (debug_sound == 1)PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
							hit_n[i] = note[i][searching];
						}
					}
				}
			}

		}
		


		if (option->op.gauge == option->OP_GAUGE_FC_ATTACK) {//銀ゲージでMISS出ていたら即失敗
			if (MISS >= 1) {
				gauge = 0;
			}
		}
		if (option->op.gauge == option->OP_GAUGE_PFC_ATTACK) {//金ゲージでGOOD,MISS出ていたら即失敗
			if (GOOD >= 1 || MISS >= 1) {
				gauge = 0;
			}
		}

		/*
		printfDx("GAME passed time scrooll:%f\n", GAME_passed_time_scroll);
		printfDx("GAME passed time :%f\n", GAME_passed_time);
		printfDx("stop_time :%f\n", stop_time);
		*/
		//printfDx("%f\n", (judge_area - note_fall)*speed*pow((cbpm*(GAME_passed_time - note1[i].timing) + sqrt(1 / speed)), 2) + note_fall);


		double QE_x = 0;//式の見た目を簡単にする仲介 Quadratic equationのx
		double QE_y = 0;//式の見た目を簡単にする仲介 Quadratic equationのy


		
		for (i = 0; i <= BARLINE_MAX_NUMBER - 1; i++) {//小節線の座標計算
			//小節線
			barline[i].x = 640;
			barline[i].y = -228;

			QE_x = high_speed * barline[i].bpm * (GAME_passed_time_scroll - ((double)barline[i].timing / TIMING_SHOW_RATIO)) + sqrt(1 / speed); //放物線のxを算出
			if ((QE_x >= 0) && barline[i].fall != 2) {//まだ叩かれてなく落ち切ってなく放物線の半分超えている(x>=0)とき
				barline[i].fall = 1;//落ちた(表示をする)
				QE_y = int(((double)judge_area - note_fall) * speed * pow(QE_x, 2) + note_fall);//GAME_passed_time_scroollを変数とした2次関数(値域0~1にnote_fallとjudge_areaで位置調整)
				if (QE_y >= 720) {//落ち切ったとき
					barline[i].y = 720;
					barline[i].fall = 2;//単ノートが落ちて見えなくなったら表示をしない(落ち切った)
				}
				if (QE_y < 720) {
					barline[i].y = (short)QE_y;
				}
			}
			else {
				if (barline[i].fall != 2)barline[i].fall = 0;//落ちてない(表示をしない)
			}
		}
		for (i = 0; i <= NOTE_MAX_NUMBER - 1; i++) {//ノートの座標計算
			//音符
			for (j = 0; j <= 3; j++) {
				note[j][i].x = lane[j];//レーンのx座標
				note[j][i].y = -228;
				QE_x = high_speed*note[j][i].bpm*(GAME_passed_time_scroll-((double)note[j][i].timing / TIMING_SHOW_RATIO)) + sqrt(1 / speed); //放物線のx座標を算出
				if ((QE_x >= 0) && (note[j][i].hit == 0) && note[j][i].fall != 2) {//まだ叩かれてなく落ち切ってなく放物線の半分超えている(x>=0)とき
					note[j][i].fall = 1;//落ちた(表示をする)
					QE_y = int(((double)judge_area - note_fall)*speed*pow(QE_x, 2) + note_fall);//GAME_passed_time_scroollを変数とした2次関数(値域0~1にnote_fallとjudge_areaで位置調整)
					if (QE_y >= 720) {//落ち切ったとき
						note[j][i].y = 720;
					}
					if (QE_y < 720) {
						note[j][i].y = (short)QE_y;
					}
					if (note[j][i].y >= 720 && (note[j][i].group == 0 || note[j][i].group == 2)) {
						if (note[j][i].group == 2) {
							note[j][i - 1].fall = 2;//終点だったらその前にある始点も落ち切っている
						}
						note[j][i].fall = 2;//単ノートが落ちて見えなくなったら表示をしない(落ち切った)
											//printfDx("otikitta\n");
					}
				}
				else {
					if (note[j][i].fall != 2)note[j][i].fall = 0;//落ちてない(表示をしない)
				}
			}
		}





		if (debug_warp == 0) {
			if (GAME_passed_time_for_draw - Music[song_number].noteoffset[difficulty] /* / scrollchange[0].scroll*/ >= (beat_time)
				&& GAME_passed_time_for_draw >= (double)Music[song_number].noteoffset[difficulty] /* /scrollchange[0].scroll */) {//一拍進んだ
				beat_percent = 0;
				beat++;
				//printfDx("%d\n", beat);
				beat_time_cash = beat_time;
				beat_time = (((double)60000 / cbpm) + beat_time_cash);
				if (stopFlag != 1)flash = 1;//フラッシュ表示フラグ(停止中は表示しない)
			}
		}
		
		/*
		printfDx("j_n_n[0]:%d\n", j_n_n[0]);
		printfDx("j_dn_n[0]:%d\n", j_dn_n[0]);
		printfDx("j_dn_push_n[0]:%d\n", j_dn_push_n[0]);
		*/
		
		//printfDx("%d\n", sc_timing);
		//printfDx("GAME_passed_time       :%f\n", GAME_passed_time);
		//printfDx("GAME_passed_time_scroll:%f\n", GAME_passed_time_scroll);
		//printfDx("noteoffset:%f\n", Music[song_number].noteoffset[difficulty]);
		//printfDx("real_timing:%d\n",real_timing);
		//printfDx("sc_timing:%d\n", sc_timing);
		//printfDx("cscroll:%f\n", cscroll);
		//printfDx("scc:%d\n", scc);
		//printfDx("bcc:%d\n", bcc);
		//printfDx(L"j_n_n[0]:%d\n", j_n_n[0]);
		//printfDx("(scrollchange[scc].timing-sc_timing)/cscroll:%d\n", int((((double)scrollchange[scc].timing - (double)sc_timing))/cscroll));
		//printfDx("beat_percent:%f\n", beat_percent);
		//printfDx("beat_time_cash:%f\n", beat_time_cash);
		//printfDx("beat_time:%f\n", beat_time);
		//for (i = 0; i <= 10; i++) {
		//printfDx("scrollchange[%d]:%f :%d\n", i, scrollchange[i].scroll, scrollchange[i].timing_real);
		//printfDx("bpm[%d]:%d :%f\n", i, note[0][i].timing , note[0][i].bpm);
		//}
		//printfDx("note[%d]:%d :%d\n", 0, note[3][0].timing, note[3][0].timing_real);
		

		//printfDx("j_n_n[1]:%d\n", j_n_n[1]);

		if (ClearFlag == 0) {//クリア状態が決まってないときにノートカウンタの処理実行
			//BPMカウンタbccのインクリメント
			if (debug_warp == 0 && GAME_passed_time_for_draw >= (double)Music[song_number].noteoffset[difficulty] /*/ scrollchange[0].scroll*/) {
				while ((GAME_passed_time_for_draw >= bpmchange[bcc].timing_real) && (bpmchange[bcc].use != 0)) {//BPM変更
					beat_percent += (bpmchange[bcc].timing_real - Music[song_number].noteoffset[difficulty] /*/ scrollchange[0].scroll*/ - beat_time_cash) / ((double)60000 / cbpm);//1拍の何割進んだか計算(前のBPM変更点からの経過時間/今までのBPMでの1拍の時間)
					beat_time_cash = bpmchange[bcc].timing_real - Music[song_number].noteoffset[difficulty] /*/ scrollchange[0].scroll*/;//BPM変更時間
					beat_time = beat_time_cash;//BPM変更時間+((残りの拍割合)の時間)が次の拍の時間
					cbpm = bpmchange[bcc].bpm;//現在のBPMを変更
					beat_time += ((double)1 - beat_percent) * ((double)60000 / cbpm);//残りの拍割合*現在のBPMの一拍の時間
					bcc++;
					//printfDx("change\n");
				}
			}

			if (debug_warp == 1) {//ワープ時拍計算
				bcc = 1;
				beat_percent = 0;
				if (GAME_passed_time_for_draw >= (double)Music[song_number].noteoffset[difficulty]/* / scrollchange[0].scroll*/) {//1小節目に入ってるとき
					while (1) {//BPMカウンタbccのインクリメント
						if (((bpmchange[bcc + 1].timing_real >= GAME_passed_time_for_draw) || (bpmchange[bcc + 1].use == 0)))break;

						beat_percent += (double)(bpmchange[bcc + 1].timing_real - bpmchange[bcc].timing_real) / ((double)60000 / bpmchange[bcc].bpm);//BPM変更地点の間の拍数を計算
																																					//printfDx("j_n_n:%d beat_plus:%d\n", bcc,note[0][j_n_n[0] + 1].timing - note[0][j_n_n[0]].timing);
						bcc++;
					}
					//printfDx("bpm:%f\n", note[0][bcc].bpm);

					beat_percent += (double)((GAME_passed_time_for_draw)-bpmchange[bcc].timing_real) / ((double)60000 / bpmchange[bcc].bpm);//移動先~前のBPM変更地点の拍数

					beat = int(beat_percent);//何拍進んだか
					beat_percent = beat_percent - beat;//一拍の内何パーセント進んだか
													   //printfDx("beat_percent:%f\n", beat_percent);
					beat_time = (double)(GAME_passed_time_for_draw - Music[song_number].noteoffset[difficulty] / 1) + ((double)1 - beat_percent)*((double)60000 / bpmchange[bcc].bpm);//経過時間+(残りの拍時間)
					beat_time_cash = GAME_passed_time_for_draw - Music[song_number].noteoffset[difficulty] / 1;
					//printfDx("passed:%f beat_time:%f\n", GAME_passed_time - Music[song_number].noteoffset[difficulty], beat_time);

					cbpm = bpmchange[bcc].bpm;
					bcc++;
				}
				turn_off_LED(hComm, &LED_state, &dwWritten, &ovl);//LEDを消す
			}

			//printfDx("time:%f\n", GAME_passed_time - Music[song_number].noteoffset[difficulty]);
			//printfDx("change:%d\n", note[0][bcc].timing);
			//printfDx("beat:%d\n", beat);
			//printfDx("beat_time:%f\n", beat_time);

			for (i = 0; i <= 3; i++) {
				while ((GAME_passed_time_for_draw - note[i][j_n_n[i]].timing_real > judge_time_through) && debug_warp == 1 && note[i][j_n_n[i]].color != 0) {//再生位置移動したとき
					if (debug_auto == 1) {
						/*
						note_hit_flash[i] = 11;
						if (note[i][j_n_n[i]].group != 1 && note[i][j_n_n[i]].snd != 0)note_hit_large_flash[i] = 19;//光るノーツ用のフラッシュ
						*/
						volume = 1;

						if (note[i][j_n_n[i]].color != 8 && note[i][j_n_n[i]].group != 1) {
							combo++;
							(*CourseCombo)++;
						}
						combo_draw_counter = 1;
						if (note[i][j_n_n[i]].group == 0) {
							note[i][j_n_n[i]].hit = 1;//単ノートなら叩く
							if (note[i][j_n_n[i]].color_init != 8)HitingNoteCount++;
						}
						if (note[i][j_n_n[i]].group == 1)LN_flag[i] = 2;//LN始点ならLNフラグON
						if (note[i][j_n_n[i]].group == 2) {
							LN_flag[i] = 0;//LN終点ならLNフラグOFF
							note[i][j_n_n[i] - 1].hit = 1;
							note[i][j_n_n[i]].hit = 1;
							HitingNoteCount++;
						}
						//hit_perfect[i] = 1;
						gauge += total / Music[song_number].total_note[difficulty];
						if (note[i][j_n_n[i]].color != 8 && note[i][j_n_n[i]].group != 1)score += (double)score_note + 1;
						if (note[i][j_n_n[i]].color != 8 && note[i][j_n_n[i]].group != 1)SKY_PERFECT++;
						//PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);
					}
					if (note[i][j_n_n[i]].group == 1)LN_flag[i] = 2;//LN始点ならLNフラグON
					if (note[i][j_n_n[i]].group == 2) {
						LN_flag[i] = 0;//LN終点ならLNフラグOFF
					}

					j_n_n[i]++;
					if (j_n_n[i] >= NOTE_MAX_NUMBER)break;
				}

			}

			if (debug_auto == 1) {//オートプレイ中
				for (i = 0; i <= 3; i++) {
					if (debug_warp == 0) {
						if (j_n_n[i] <= NOTE_MAX_NUMBER - 1) {
							while ((GAME_passed_time - (note[i][j_n_n[i]].timing_real - config.VsyncOffsetCompensation) >= 0 - TimePerFrame) && (note[i][j_n_n[i]].color != 0)) {//ノートが判定枠に来た
								if (8 != note[i][j_n_n[i]].color) {//黒以外のとき叩く(単ノート LN始点 LN終点)

									flash_LED(hComm, note[i][j_n_n[i]], &LED_state, i, &dwWritten, &ovl);//コントローラのLEDを光らせる

									if (note[i][j_n_n[i]].group != 1)hit_n[i] = note[i][j_n_n[i]];
									if (note[i][j_n_n[i]].group != 1) {
										note_hit_flash[note_hit_flash_rounder][i] = 11;//ノートを叩いた時の通常フラッシュ
										note_hit_flash_rounder = (note_hit_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}
									if (note[i][j_n_n[i]].group == 0 && note[i][j_n_n[i]].snd != 0) {//単ノート
										note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
										note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}
									if (note[i][j_n_n[i]].group == 2 && note[i][j_n_n[i] - 1].snd != 0) {//LN終点
										note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
										note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}

									volume = 1;
									if (note[i][j_n_n[i]].group != 1) {
										combo++;//LN始点じゃないときコンボ加算
										(*CourseCombo)++;
									}
									if (note[i][j_n_n[i]].group != 1)combo_draw_counter = 1;
									if (note[i][j_n_n[i]].group == 0) {
										note[i][j_n_n[i]].hit = 1;//単ノートなら叩く
										HitingNoteCount++;
									}
									if (note[i][j_n_n[i]].group == 1)LN_flag[i] = 2;//LN始点ならLNフラグON
									if (note[i][j_n_n[i]].group == 2) {
										LN_flag[i] = 0;//LN終点ならLNフラグOFF
										note[i][j_n_n[i] - 1].hit = 1;
										note[i][j_n_n[i]].hit = 1;
										HitingNoteCount++;
										LN_flash[i] = 0;
										if (note[i][j_n_n[i]].LN_k == 1 && debug_sound == 1) {//黒終端なら
											PlaySoundMem(SH.SH_RELEASE, DX_PLAYTYPE_BACK, TRUE);//黒終端リリース音
										}
									}
									if (note[i][j_n_n[i]].group != 1)hit_sky_perfect[i] = 1;//単ノート、LN終点でSKY_PERFECT文字を出す
									if (note[i][j_n_n[i]].group != 1) {
										gauge += total / Music[song_number].total_note[difficulty];
										gauge += 0.1;

										if (note[i][j_n_n[i]].snd != 0) {//光るノートなら2倍
											gauge += total / Music[song_number].total_note[difficulty];
										}

										if (note[i][j_n_n[i]].group == 2 && note[i][j_n_n[i] - 1].snd != 0) {//光るノートなら2倍
											gauge += total / Music[song_number].total_note[difficulty];
										}

									}
									if (note[i][j_n_n[i]].group != 1)score += (double)score_note + 1;
									if (note[i][j_n_n[i]].group != 1) {
										SKY_PERFECT++;
										TimePerfect++;
									}
									//音の処理
									//if (note[i][j_n_n[i]].group == 0)PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);
									//if (note[i][j_n_n[i]].group == 1)PlaySoundMem(SH_HIT_LONG, DX_PLAYTYPE_BACK, TRUE);

									if (note[i][j_n_n[i]].group != 2) {
										if (debug_sound == 1)PlayHitSound(2, note[i][j_n_n[i]].color_init, note[i][j_n_n[i]].snd, SH);
									}

									/*
									if (note[i][j_n_n[i]].group != 2 && note[i][j_n_n[i]].color_init == 9)PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);//LN終点じゃないとき音を鳴らす
									if (note[i][j_n_n[i]].color_init == 1 || note[i][j_n_n[i]].color_init == 4 || note[i][j_n_n[i]].color_init == 6 || note[i][j_n_n[i]].color_init == 7) {
									if (note[i][j_n_n[i]].group != 2) {
									ChangeVolumeSoundMem(int(255), SH_R[i-1]);
									//PlaySoundMem(SH_R[i - 1], DX_PLAYTYPE_BACK, TRUE);
									}
									if (note[i][j_n_n[i]].group == 0 || note[i][j_n_n[i]].group == 2) {//LN始点以外は離すことにする
									release_time[2][i - 1] = GetNowCount_d(config)+70;
									}

									}
									if (note[i][j_n_n[i]].color_init == 2 || note[i][j_n_n[i]].color_init == 4 || note[i][j_n_n[i]].color_init == 5 || note[i][j_n_n[i]].color_init == 7) {
									if (note[i][j_n_n[i]].group != 2) {
									ChangeVolumeSoundMem(int(255), SH_G[i - 1]);
									//PlaySoundMem(SH_G[i - 1], DX_PLAYTYPE_BACK, TRUE);
									}
									if (note[i][j_n_n[i]].group == 0 || note[i][j_n_n[i]].group == 2) {
									release_time[1][i - 1] = GetNowCount_d(config)+70;
									}

									}
									if (note[i][j_n_n[i]].color_init == 3 || note[i][j_n_n[i]].color_init == 5 || note[i][j_n_n[i]].color_init == 6 || note[i][j_n_n[i]].color_init == 7) {
									if (note[i][j_n_n[i]].group != 2) {
									ChangeVolumeSoundMem(int(255), SH_B[i - 1]);
									//PlaySoundMem(SH_B[i - 1], DX_PLAYTYPE_BACK, TRUE);
									}
									if (note[i][j_n_n[i]].group == 0 || note[i][j_n_n[i]].group == 2) {
									release_time[0][i - 1] = GetNowCount_d(config)+70;
									}
									}
									*/
									///////////////

									//後ろに落ちるものの処理
									if (note[i][j_n_n[i]].group != 1) {
										insert_cell(insert, img_num(Music[song_number].season[difficulty]),
											float(640 + (lane[i] + 128 - 640) * 2),
											rand() % 10000,
											rand() % 10000,
											rand() % 401,
											0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
										insert = insert->next;//挿入する場所を後ろにする
										if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
									}
								}


								j_n_n[i]++;
							}
							while ((note[i][j_n_n[i]].hit == 1 || note[i][j_n_n[i]].color == 8) && (note[i][j_n_n[i]].color != 0)) {//叩かれているか黒ならそれ以外になるまでカウンタを進める()
								j_n_n[i]++;
								if (j_n_n[i] >= NOTE_MAX_NUMBER)break;
							}
						}
					}
				}
			}

			for (i = 0; i <= 3; i++) {//通常時
				if (debug_warp == 0) {
					if (j_n_n[i] <= NOTE_MAX_NUMBER - 1) {
						if ((GAME_passed_time - note[i][j_n_n[i]].timing_real > judge_time_through) && (note[i][j_n_n[i]].color != 0)//through時間を過ぎていった、または
							|| ((GAME_passed_time - note[i][j_n_n[i]].timing_real > judge_time_good) && (fabs(note[i][j_n_n[i] + 1].timing_real - GAME_passed_time) < judge_time_good))) {//(good判定を通り過ぎて次のノートがgood判定内にいるとき)ノートの見逃し(コンボが途切れる MISS)
							if (8 != note[i][j_n_n[i]].color && note[i][j_n_n[i]].group != 2) {//黒とLN終点以外のときミスにする
								if (note[i][j_n_n[i]].group == 1) {//LN始点のとき
									LN_flag[i] = 1;//LN中(すでに先頭見逃し)にする
								}
								volume = 0;
								if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
								if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新

								combo = 0;
								*CourseCombo = 0;
								combo_draw_counter = 0;
								hit_miss[i] = 1;
								HitingNoteCount++;
 								gauge -= gauge_dec;
 								if (note[i][j_n_n[i]].snd != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec;
								}
								score -= score_dec;
								MISS++;
								TimeMiss++;
							}
							j_n_n[i]++;
						}
						while ((note[i][j_n_n[i]].hit == 1 || note[i][j_n_n[i]].color == 8) && (note[i][j_n_n[i]].color != 0)) {//叩かれているか黒ならそれ以外になるまでカウンタを進める()
							j_n_n[i]++;
							if (j_n_n[i] >= NOTE_MAX_NUMBER)break;
						}
					}
				}

				if (j_dn_n[i] <= NOTE_MAX_NUMBER - 1) {//黒カウンタ(叩き)
					while ((GAME_passed_time - note[i][j_dn_n[i]].timing_real >  judge_time_dark) && (note[i][j_dn_n[i]].color != 0)) {
						j_dn_n[i]++;
					}
					while ((note[i][j_dn_n[i]].hit == 1 || note[i][j_dn_n[i]].color != 8) && (note[i][j_dn_n[i]].color != 0)) {//叩かれているか黒じゃないか
						j_dn_n[i]++;
						if (j_dn_n[i] >= NOTE_MAX_NUMBER)break;
					}
				}
				if (j_dn_push_n[i] <= NOTE_MAX_NUMBER - 1) {//黒カウンタ(押し込み)
					while ((GAME_passed_time - note[i][j_dn_push_n[i]].timing_real >  con_jd) && (note[i][j_dn_push_n[i]].color != 0)) {
						j_dn_push_n[i]++;
					}
					while ((note[i][j_dn_push_n[i]].hit == 1 || note[i][j_dn_push_n[i]].color != 8) && (note[i][j_dn_push_n[i]].color != 0)) {//叩かれているか黒じゃないか
						j_dn_push_n[i]++;
						if (j_dn_push_n[i] >= NOTE_MAX_NUMBER)break;
					}
				}
			}

		}
		//printfDx("%d\n", j_dn_n[4]);

		if (gauge > 100)gauge = 100;//ゲージの最大最小処理
		if (gauge < 0)gauge = 0;

		if (score < 0)score = 0;

		//printfDx("%d\n", Music[song_number].TimeToEnd[difficulty]);
		if (gauge <= 0 && ClearFlag == 0 && debug_fail == 1) {//ゲージがゼロでクリア状態になってないなら失敗(FAILED)
			ClearFlag = 2;
			c_m_draw_counter = 1;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
			StopSoundMem(SH_SONG);//曲再生停止
		}
		if (GAME_passed_time >= Music[song_number].TimeToEnd[difficulty] && ClearFlag == 0
			&& note[0][j_n_n[0]].color == 0
			&& note[1][j_n_n[1]].color == 0
			&& note[2][j_n_n[2]].color == 0
			&& note[3][j_n_n[3]].color == 0) {//終了タイミングで失敗してなかったらクリア(ウィンドウクリックし続けてクリアする不正防止のためノートカウンタの位置も見る)
			ClearFlag = 1;
			c_m_draw_counter = 1;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
		}

		
		//左上の時間関連処理
		if (ClearFlag != 2) {//演奏中のときだけ時間を計算(Clearedで幕が下がっても24時になるまで続ける)
		
			PassedTime_Hours = int(GAME_passed_time_scroll / HourOfGame) + 6;//時間を算出
			PassedTime_Minutes = int((int)GAME_passed_time_scroll % (int)HourOfGame / MinuteOfGame);//分を算出
			if (PassedTime_Hours >= 24) {
				PassedTime_Hours = 24;
				PassedTime_Minutes = 0;
			}
			if (PassedTime_Minutes <= -1) {
				PassedTime_Minutes = 0;
			}

		}

		if (PassedTime_Hours >= TimeNextHour && TimeNextHour<=24) {//7~24時の1時間間隔でゲージ量保存

			TimePop[PassedTime_Hours - 7] = int(100- gauge_draw_hosei);
			

			//3時間毎に精度調査
			if (PassedTime_Hours == 9) {
				TimeAcc[0] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}
			if (PassedTime_Hours == 12) {
				TimeAcc[1] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}
			if (PassedTime_Hours == 15) {
				TimeAcc[2] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}
			if (PassedTime_Hours == 18) {
				TimeAcc[3] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}
			if (PassedTime_Hours == 21) {
				TimeAcc[4] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}
			if (PassedTime_Hours == 24) {
				TimeAcc[5] = int(((double)TimePerfect * 2 * score_note + (double)TimeGood * 1 * score_note + (double)TimeMiss * (-score_dec) + 0.001) / (((double)TimePerfect + (double)TimeGood + (double)TimeMiss) * 2 * score_note + 0.001) * 100);// 今のスコア/全部Perfectで叩いたスコア
				TimePerfect = 0;
				TimeGood = 0;
				TimeMiss = 0;
			}

			TimeNextHour++;
		}

		/*
		for (i = 0; i <= 5; i++) {
			printfDx("Acc%d\n", TimeAcc[i]);
		}
		for (i = 0; i <= 17; i++) {
			printfDx("Gauge%d\n", TimePop[i]);
		}
		*/
		//printfDx("MaxCombo %d\n", MAX_COMBO);


		if (SkillTestFlag == 0) {
			number_digit(combo, combo_digit, 5);//コンボを桁ごとに格納
		}
		else {//コースモードの時
			number_digit(*CourseCombo, combo_digit, 5);//コンボを桁ごとに格納
		}

		number_digit(int(score + 0.5), score_digit, 5);//スコアを桁ごとに格納

		number_digit(int(PassedTime_Hours), time_hours_digit, 2);//時間を桁ごとに格納
		number_digit(int(PassedTime_Minutes), time_minutes_digit, 2);//分を桁ごとに格納

		number_digit(int(score + 0.5), score_digit, 5);//スコアを桁ごとに格納


		number_digit(int(cbpm + 0.5), bpm_digit, 5);//BPMを桁ごとに格納

													//printfDx("%d",ClearFlag);
		if (ClearFlag != 0 && str_draw_counter == 0 && c_m_draw_counter == 0 && (GAME_passed_time - cleared_time >= TimeFromEndOfGameToResult || *escape == 1)) {//クリア表示4秒後にゲームの終了(結果発表へ)
			if (*escape == 0) {//途中で抜け出してなかったら
							   //res->song_number = song_number;
				res->difficulty = difficulty;
				if (ClearFlag == 1) {
					if (option->op.gauge == option->OP_GAUGE_NORMAL) res->clear = CLEARTYPE_CLEARED;//NORMAL
					if (option->op.gauge == option->OP_GAUGE_HARD) res->clear = CLEARTYPE_HARD_CLEARED;//HARD
					if (option->op.gauge == option->OP_GAUGE_SUPER_HARD) res->clear = CLEARTYPE_SUPER_HARD_CLEARED;//SUPER HARD
					
					if (option->op.gauge == option->OP_GAUGE_NO_FAIL) {
						res->clear = CLEARTYPE_PLAY;//NO_FAIL
					}
					else {
						if (option->op.color == option->OP_COLOR_RGB_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED(Onlyオプション使ってたら灰クリアに塗り替える)
						if (option->op.color == option->OP_COLOR_CMY_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED
						if (option->op.color == option->OP_COLOR_W_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED
					}
					if (option->op.gauge == option->OP_GAUGE_SKILL_TEST) res->clear = CLEARTYPE_PLAY;//段位ゲージ
				}

				if (SkillTestFlag != 0) {
					*GaugeVal = gauge;//段位認定のときはゲージ量継承
				}

				if (ClearFlag == 2)res->clear = CLEARTYPE_FAILED;
				if (ClearFlag == 1 && MISS == 0 && (
					option->op.color == option->OP_COLOR_NONE ||
				    option->op.color == option->OP_COLOR_RGB_CHANGE ||
					option->op.color == option->OP_COLOR_RGB_RAND ||
					option->op.color == option->OP_COLOR_SUPER_RAND ||
					option->op.color == option->OP_COLOR_RAINBOW)
					) {//クリアしていてミス0ならフルコンボかPERFECTフルコンボ

					if (GOOD == 0) {
						//PERFECTフルコンボ
						res->clear = CLEARTYPE_PERFECT;
					}
					else {
						//フルコンボ
						res->clear = CLEARTYPE_FULL_COMBO;
					}
				}
				res->score = int(score + 0.5);
				res->sky_perfect = SKY_PERFECT;
				res->perfect = PERFECT;
				res->good = GOOD;
				res->miss = MISS;

				if (res->score < RANK_E_SCORE)res->rank = RANK_F;//ランクF
				if (res->score >= RANK_E_SCORE)res->rank = RANK_E;//ランクE
				if (res->score >= RANK_D_SCORE)res->rank = RANK_D;//ランクD
				if (res->score >= RANK_C_SCORE)res->rank = RANK_C;//ランクC
				if (res->score >= RANK_B_SCORE)res->rank = RANK_B;//ランクB
				if (res->score >= RANK_A_SCORE)res->rank = RANK_A;//ランクA
				if (res->score >= RANK_S_SCORE)res->rank = RANK_S;//ランクS

												//res->artist;
												//res->genre;
				for (i = 0; i <= 5; i++) {
					res->pop[i] = int(TimePop[i * 3]);//3時間毎のpopの最高値を計算
					if (int(TimePop[i * 3 + 1]) > res->pop[i])res->pop[i] = int(TimePop[i * 3 + 1]);
					if (int(TimePop[i * 3 + 2]) > res->pop[i])res->pop[i] = int(TimePop[i * 3 + 2]);


					if (TimeAcc[i] >= 90){//晴れ精度
						res->weather[i] = RESULT_WEATHER_SUNNY;
					}
					else if (TimeAcc[i] >= 80) {//曇り精度
						res->weather[i] = RESULT_WEATHER_CLOUDY;
					}
					else if (TimeAcc[i] >= 70) {//雨精度
						res->weather[i] = RESULT_WEATHER_RAINY;
					}
					else{//69以下は雷精度
						res->weather[i] = RESULT_WEATHER_THUNDER;
					}
				}

				if (combo >= MAX_COMBO)MAX_COMBO = combo;//最大コンボ数更新
				if (*CourseCombo >= *CourseMaxCombo)*CourseMaxCombo = *CourseCombo;//コースの最大コンボ数更新

				res->max_combo = MAX_COMBO;

				res->min_miss = -1;


			}

			if (*escape == 1) {
				*retryAble = 1;
			}
			if (ClearFlag == 2) {
				*retryAble = 1;
			}

			if (*debug == 1 && pitch_step != 0) {
				GAME_LOAD(song_number, difficulty, note, barline, lane, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, &hash, Music, &TimeToEndScroll, &playing_time, config, 1);//ピッチが変更されているときはこの関数の終わりにピッチ1で読み込みなおして
				Music[song_number].hash[difficulty] = hash; //正しいハッシュ値に戻す
				res->hash = hash;//ハッシュ値を格納
			}

			CloseHandle(hComm);//シリアル通信の終了
			StopSoundMem(SH_SONG);//曲の再生停止
			InitGraph();//メモリ開放
			InitSoundMem();//
			DeleteFontToHandle(FontHandle);

			free(bpmchange);
			free(scrollchange);
			free(stopSequence);
			for (i = 0; i <= 3; i++) {
				free(note[i]);
			}
			free(note);//note解放
			free(barline);//barline解放
			if (head.next != NULL) {//最低1つあるとき
				delete_cell_all(head.next, &head, insert);//headからのリストのメモリ開放
			}

			// エフェクトリソースを削除する。(Effekseer終了時に破棄されるので削除しなくてもいい)
			DeleteEffekseerEffect(FXH_FULLCOMBO);

			return;//関数終了

		}

		for (i = 0; i < CRTBuf; i++) {
			if (volume < 1) {//ボリュームを戻す
				volume += 0.0024*(cbpm / 120);
			}
			if (volume >= 1) {
				volume = 1;
			}

			for (j = 0; j < 3; j++) {//判定エリアの光
				for (k = 0; k < 4; k++) {
					HitAreaBright[j][k] /= 1.02;
				}
			}
		}

		//gauge = double(50 * (1 + sin((double)GAME_passed_time / 1000)));

		//----Draw-----------------------------------------------------------------------------------/////////////////
		ClearDrawScreen();//グラフィックを初期化
		DrawGraph(0, 0, H_BG, TRUE);//背景
		show_cloud(H_CLOUD, &pos_cloud, (stopFlag != 1)*cbpm / 150, LOOP_passed_time);//雲

	    //後ろに落ちるものの表示と削除
		if (head.next != NULL) {//最低1つあるとき

			search_cell_all(head.next, &head, insert, &insert, H_FALL, LOOP_passed_time, (stopFlag != 1)*cbpm / 150);
			//printfDx("head.next:%d\n", head.next);
			//printfDx("&head:%d\n", &head);
			//printfDx("insert:%d\n", insert);
			//ScreenFlip();
		}
		/*
		fall_y += 0.4;
		DrawRotaGraph2(500 + 80 * sin(0.001*GAME_passed_time+1), int(fall_y-320), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[0], TRUE, FALSE);
		DrawRotaGraph2(600 + 80 * sin(0.001*GAME_passed_time+3), int(fall_y-400), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[1], TRUE, FALSE);
		DrawRotaGraph2(750 + 80 * sin(0.001*GAME_passed_time+2), int(fall_y - 590), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[2], TRUE, FALSE);
		DrawRotaGraph2(820 + 80 * sin(0.001*GAME_passed_time+5), int(fall_y-100), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[3], TRUE, FALSE);
		DrawRotaGraph2(500 + 80 * sin(0.001*GAME_passed_time + 7.5), int(fall_y - 770), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[4], TRUE, FALSE);
		DrawRotaGraph2(630 + 80 * sin(0.001*GAME_passed_time + 8.5), int(fall_y - 890), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[5], TRUE, FALSE);
		DrawRotaGraph2(730 + 80 * sin(0.001*GAME_passed_time + 9.5), int(fall_y - 1207), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[6], TRUE, FALSE);
		DrawRotaGraph2(8000 + 80 * sin(0.001*GAME_passed_time + 10.5), int(fall_y-1100), 50, 50, 1, 0.002*GAME_passed_time, H_FALL[7], TRUE, FALSE);
		*/

		//背景の明るさを抑える黒画像表示
		int BlendMode = DX_BLENDMODE_ALPHA;
		int BlendVal = 255;
		SetDrawBlendMode(BlendMode, int(BlendVal*(double)option->op.darkness/4));
		DrawGraph(0, 0, H_DARKNESS, TRUE);

		//ボタンを押した時の光
		BlendMode = DX_BLENDMODE_ALPHA;
		BlendVal = 255;
		SetDrawBlendMode(BlendMode, BlendVal);


		for (i = 0; i <= 3; i++) {//レーン
			for (j = 0; j <= 2; j++) {//色
				if (Key[Keylist[j][i]] >= 1) {
					HitAreaBright[j][i] = 1;//判定エリアの光
				}
			}
		}
		SetDrawBright(int(HitAreaBright[2][0] * 160 + 75), int(HitAreaBright[1][0] * 160 + 75), int(HitAreaBright[0][0] * 160 + 75));
		DrawGraph(lane[0], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][1] * 160 + 75), int(HitAreaBright[1][1] * 160 + 75), int(HitAreaBright[0][1] * 160 + 75));
		DrawGraph(lane[1], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][2] * 160 + 75), int(HitAreaBright[1][2] * 160 + 75), int(HitAreaBright[0][2] * 160 + 75));
		DrawGraph(lane[2], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][3] * 160 + 75), int(HitAreaBright[1][3] * 160 + 75), int(HitAreaBright[0][3] * 160 + 75));
		DrawGraph(lane[3], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(255, 255, 255);

		for (i = 0; i <= 3; i++) {
			DrawGraph(lane[i], judge_area, H_JUDGE_AREA, TRUE);//判定枠の表示
		}


		//printfDx("jnn:%d", j_n_n[2]);
		//printfDx("jdnn:%d", j_dn_n[2]);
		for (i = 0; i <= 3; i++) {//ノートを叩いた後に一瞬表示するノート
			for (j = 0; j <= NOTE_HIT_LARGE_FLASH_NUMBER - 1; j++) {
				if (note_hit_flash[j][i] >= 7) {
					DrawGraph(lane[i], judge_area, H_NOTE[hit_n[i].color_init], TRUE);
					break;
				}
			}
			if (dnote_hit_flash[i] >= 7) {//黒
				DrawGraph(lane[i], judge_area, H_NOTE[8], TRUE);
			}
		}

		BlendMode = DX_BLENDMODE_ALPHA;
		BlendVal = 255;
		if (option->op.barline == option->OP_BARLINE_ON) {//小節線を表示するオプションなら
			for (i = BARLINE_MAX_NUMBER - 1; i >= 0; i--) {//小節線の描画
				if (barline[i].fall == 1 && barline[i].use == 1) {
					SetDrawBlendMode(BlendMode, BlendVal);
					//if (note[j][i].color_init != 0)DrawGraph(note[j][i].x - 32, note[j][i].y, H_NOTE_OR_FRAME, TRUE);//ORノートの場合
					if (option->op.darkness >= option->OP_DARKNESS_50) {//暗さ50%以上のときだけ白で描画
						DrawLineAA(float(320), float(barline[i].y + 128), float(960), float(barline[i].y + 128), GetColor(255, 255, 255), 2.5);
					}
					else {
						DrawLineAA(float(320), float(barline[i].y + 128), float(960), float(barline[i].y + 128), GetColor(0, 0, 0), 2.5);
					}
					//DrawGraph(barline[i].x, barline[i].y, H_NOTE[7], TRUE);//単ノートの場合
				}
			}
		}

		for (i = NOTE_MAX_NUMBER - 1; i >= 0; i--) {//ノーツの描画

			for (j = 0; j <= 3; j++) {
				if (note[j][i].fall == 1) {
					if (note[j][i].group == 0) {
						SetDrawBlendMode(BlendMode, BlendVal);

						//if (note[j][i].color_init != 0)DrawGraph(note[j][i].x - 32, note[j][i].y, H_NOTE_OR_FRAME, TRUE);//ORノートの場合
						DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[note[j][i].color], TRUE);//単ノートの場合



						if (note[j][i].snd != 0) {//光るノートは点滅させる(flashに合わせて)
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
							DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[10], TRUE);//白く光らせる
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
							DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[10], TRUE);//常に白くする
						}

					}

					if (note[j][i].group == 1) {//始点について

						if (note[j][i].y >= note[j][i + 1].y) {//始点が終点より下にある場合
							SetDrawBlendMode(BlendMode, BlendVal);
							DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[note[j][i].color_init], TRUE, FALSE);//LN始点の場合(始点下半分と中間を表示)
							if (LN_flash[j] == 1 && j_n_n[j] - 1 == i && note[j][i].group == 1) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//LN始点の場合(始点下半分と中間を表示)
							}

							if (note[j][i].snd != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//白く光らせる
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//常に白くする

							}

							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);
							DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[note[j][i].color_init], TRUE);

							
							if (LN_flash[j] == 1 && j_n_n[j] - 1 == i && note[j][i].group == 1) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);//白く光らせる
							}
							
							if (note[j][i].snd != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);;//黒終端ロングノートのグラデーション表示
							if (note[j][i + 1].LN_k == 1 && option->op.black_gradation == option->OP_BLACK_GRADATION_ON)DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[8], TRUE);

						}
						else {////始点が終点より上にある場合
							SetDrawBlendMode(BlendMode, BlendVal);

							if (note[j][i + 1].LN_k == 0) {//LN始点の場合(始点上半分と中間を表示)
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[note[j][i].color], TRUE, FALSE);//通常終端
							}
							else if (note[j][i + 1].LN_k == 1) {
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[8], TRUE, FALSE);//上半分黒音符表示
							}

							if (note[j][i + 1].LN_k == 0) {//黒終端は光らせない
								if (LN_flash[j] == 1 && j_n_n[j] - 1 == i && note[j][i].group == 1) {//LNを叩いているとき
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);//LN始点の場合(始点上半分と中間を表示)
								}

								if (note[j][i].snd != 0) {//光るノートは点滅させる(flashに合わせて)
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								}
							}

						}
					}
					if (note[j][i].group == 2) {//終点について
						if (note[j][i - 1].y >= note[j][i].y) {//始点が終点より下にある場合
							SetDrawBlendMode(BlendMode, BlendVal);

							if (note[j][i].LN_k == 0) {
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[note[j][i].color], TRUE, FALSE);//上半分表示
							}
							else if (note[j][i].LN_k == 1) {
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[8], TRUE, FALSE);//上半分黒音符表示
							}

							if (note[j][i].LN_k == 0) {//黒終端は光らせない
								if (LN_flash[j] == 1 && j_n_n[j] == i && note[j][i].group == 2) {//LNを叩いているとき
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								}

								if (note[j][i - 1].snd != 0) {//光るノートは点滅させる(flashに合わせて)
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								}
							}
						}
						else {//始点が終点より上にある場合
							SetDrawBlendMode(BlendMode, BlendVal);
							DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[note[j][i].color], TRUE, FALSE);//下半分表示
							
							
							if (LN_flash[j] == 1 && j_n_n[j] == i && note[j][i].group == 2) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//下半分表示
							}
							

							if (note[j][i - 1].snd != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);


							}

							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);
						    DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[note[j][i].color_init], TRUE);
							
							//DrawRectExtendGraph
							//DrawRect
							if (LN_flash[j] == 1 && j_n_n[j] == i && note[j][i].group == 2) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 120);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							if (note[j][i - 1].snd != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);//黒終端ロングノートのグラデーション表示
							if (note[j][i].LN_k == 1 && option->op.black_gradation == option->OP_BLACK_GRADATION_ON)DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[8], TRUE);

						}
					}
				}
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 - 195 * (c_m_draw_counter)+195 * ((double)1 - (gauge_draw_counter / 100))));
		DrawGraph(0, 0, H_COVER, TRUE);//カバー表示
		DrawGraph(960, 0, H_COVER, TRUE);//右側


		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash* FLASH_COVER_VALUE* (1 - (double)option->op.darkness / 5)));
		DrawGraph(0, 0, H_COVER_FLASH, TRUE);//カバーフラッシュ表示
		DrawGraph(960, 0, H_COVER_FLASH, TRUE);//右側
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);




		for (i = 0; i < CRTBuf; i++) {
			if (flash > 0) {//フラッシュを小さく
				flash -= 0.0072*(cbpm / 120);
			}
			if (flash <= 0) {
				flash = 0;
			}
		}



		//DrawGraph(960, 0, H_COVER_STR, TRUE);//右側の文字

		DrawGraph(0, 0, H_GAUGE_BOX, TRUE);//ゲージボックス (左上95,105)大きさ130*510
		gauge_draw_hosei = xtosinx(gauge_draw_counter);

		DrawRectGraph(95, int(105 + ((100 - gauge_draw_hosei) / 100 * 510)),
			          95, int(105 + ((100 - gauge_draw_hosei) / 100 * 510)),
			130, int(511 - ((100 - gauge_draw_hosei) / 100 * 510)), H_GAUGE_BAR, TRUE, FALSE);//ゲージ
		for (i = 0; i < CRTBuf; i++) {//ゲージを滑らかに増減
			if (gauge_draw_counter >= gauge) {
				gauge_draw_counter -= (gauge_draw_counter - gauge) / 100;
			}
			if (gauge_draw_counter <= gauge) {
				gauge_draw_counter += (gauge - gauge_draw_counter) / 100;
			}
		}

		//降水量表示
		pop = int(100 - gauge_draw_hosei);
		if (ClearFlag == 2)pop = 100;
		DrawNumber(160, 560, pop, 25, 0, 0, H_POP_NUMBER);

		//コントローラ画像描画
		for (j = 0; j <= 3; j++) {//B
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_B, TRUE);
			if (Key[Button[0][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 0; j <= 3; j++) {//G
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_G, TRUE);
			if (Key[Button[1][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 0; j <= 3; j++) {//R
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_R, TRUE);
			if (Key[Button[2][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 230));
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		//int((log10())で桁数を算出													
		//printfDx("%f\n", log10(combo));
		int ComboBuf=combo;
		if (SkillTestFlag != 0)ComboBuf = *CourseCombo;//コースモードの時はコース全体のコンボで表示

		if (combo_draw_counter > 0 && ComboBuf > 0) {//コンボ描画
			DrawExtendGraph(640, int(320 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 640 + 256, int(320 + 128 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_COMBO, TRUE);
			for (i = 0; i <= int(log10(ComboBuf)); i++) {
				DrawExtendGraph(540 - i * 40, int(320 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 540 + 64 - i * 40, int(320 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_COMBO_NUMBER[combo_digit[i]], TRUE);
			}
		}

        //時間描画
		if (PassedTime_Hours <= 9) {//6~9のとき
			DrawGraph(130 - 20, 0, H_TIME_COLON, TRUE);
		}
		else {
			DrawGraph(130, 0, H_TIME_COLON, TRUE);
		}

		if (PassedTime_Hours <= 9) {//6~9のとき
			for (i = 0; i <= 0; i++) {//時間
				DrawExtendGraph(86 - 20 - i * 40, int(0), 86 - 20 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_hours_digit[i]], TRUE);
			}
		}
		else {
			for (i = 0; i <= 1; i++) {//時間
				DrawExtendGraph(86 - i * 40, int(0), 86 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_hours_digit[i]], TRUE);
			}
		}

		if (PassedTime_Hours <= 9) {//6~9のとき
			for (i = 0; i <= 1; i++) {//分
				DrawExtendGraph(214 - 20 - i * 40, int(0), 214 - 20 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_minutes_digit[i]], TRUE);
			}
		}
		else {
			for (i = 0; i <= 1; i++) {//分
				DrawExtendGraph(214 - i * 40, int(0), 214 + 64 - i * 40, int(0 + 100), H_TIME_NUMBER[time_minutes_digit[i]], TRUE);
			}
		}




		//スコア描画
		if (option->op.color == option->OP_COLOR_RAINBOW) {//虹オプションのときR表示
								 //SetDrawBright(190, 190, 190);
			DrawGraph(960, -3, H_R_OUT, TRUE);
			DrawGraph(960, -3, H_R_IN, TRUE);
			//SetDrawBright(255, 255, 255);
		}

		if (option->op.color != option->OP_COLOR_RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1170 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1170 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		if (option->op.color == option->OP_COLOR_RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1205 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1205 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		for (i = 0; i < CRTBuf; i++) {
			if (combo_draw_counter >= 0) {
				combo_draw_counter -= 0.0036;
			}
		}

		//難易度画像描画
		DrawGraph(1020, 260, H_DIFFICULTY, TRUE);

		//スコアグラフ描画
		if (option->op.scoregraph != option->OP_SCORE_GRAPH_OFF) {
			//グラフ名ボックス描画
			DrawBoxWithLine(960, 482, 960 + 80, 482 + 40, GetColor(50, 50, 255), 160, 255);//現在のスコア
			DrawBoxWithLine(960 + 80, 482, 960 + 80 + 80, 482 + 40, GetColor(50, 255, 50), 160, 255);//ハイスコア
			DrawBoxWithLine(960 + 160, 482, 960 + 80 + 160, 482 + 40, GetColor(255, 50, 50), 160, 255);//ターゲットスコア
			DrawBoxWithLine(960 + 240, 482, 960 + 80 + 240, 482 + 40, GetColor(200, 200, 50), 160, 255);//前回のスコア

			//スコアグラフ描画
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);

			//ボーダーライン描画
			for (i = 0; i < 320; i++) {
				DrawPixel(960 + i, 102, GetRainbowColor(i, 320));//S
			}
			DrawLine(960, 122, 1280, 122, GetColor(0, 0, 255));//A
			DrawLine(960, 162, 1280, 162, GetColor(0, 255, 0));//B
			DrawLine(960, 202, 1280, 202, GetColor(255, 0, 0));//C
			DrawLine(960, 242, 1280, 242, GetColor(0, 0, 0));//D

			//グラフ描画
			double noteCountRatio = (double)HitingNoteCount / Music[song_number].total_note[difficulty];//ノートカウント進行率
			//グラフ全体
			DrawBox(960 + 80, 482 - 0.04 * highScore.score * noteCountRatio, 960 + 80 + 80, 482 - 0.04 * highScore.score, GetColor(50, 100, 50), TRUE);
			DrawBox(960 + 160, 482 - 0.04 * targetScore * noteCountRatio, 960 + 80 + 160, 482 - 0.04 * targetScore, GetColor(100, 50, 50), TRUE);
			DrawBox(960 + 240, 482 - 0.04 * targetScore2 * noteCountRatio, 960 + 80 + 240, 482 - 0.04 * targetScore2, GetColor(100, 100, 50), TRUE);
			if (HitingNoteCount != 0) {
				//リアルタイムグラフ
				DrawBoxWithLine(960, int(482 - 0.04 * score), 960 + 80, 482, GetColor(50, 50, 255), 160, 255);
				DrawBoxWithLine(960 + 80, 482 - 0.04 * highScore.score * noteCountRatio, 960 + 80 + 80, 482, GetColor(50, 255, 50), 160, 255);
				DrawBoxWithLine(960 + 160, 482 - 0.04 * targetScore * noteCountRatio, 960 + 80 + 160, 482, GetColor(255, 50, 50), 160, 255);
				DrawBoxWithLine(960 + 240, 482 - 0.04 * targetScore2 * noteCountRatio, 960 + 80 + 240, 482, GetColor(255, 255, 50), 160, 255);
			}
		}

		//判定数表示欄
		DrawBoxWithLine(976, 540, 1110, 580, GetColor(50, 255, 255), 80, 255);//SKY_PERFECT
		DrawBoxWithLine(976, 580, 1110, 620, GetColor(255, 255, 50), 80, 255);//PERFECT
		DrawBoxWithLine(976, 620, 1110, 660, GetColor(255, 50, 50), 80, 255);//GOOD
		DrawBoxWithLine(976, 660, 1110, 700, GetColor(50, 50, 255), 80, 255);//MISS

		//BPM描画欄
		DrawBoxWithLine(1130, 540, 1264, 580, GetColor(255, 50, 50), 80, 255);//MAXBPM
		DrawBoxWithLine(1130, 580, 1264, 620, GetColor(255, 255, 50), 80, 255);//BPM
		DrawBoxWithLine(1130, 620, 1264, 660, GetColor(50, 50, 255), 80, 255);//MINBPM
		DrawBoxWithLine(1130, 660, 1264, 700, GetColor(25, 255, 25), 80, 255);//SPEED

		//判定、BPM文字表示
		DrawGraph(0, 0, H_GAME_STR_JUDGE_BPM, TRUE);

		//SCORE GRAPHがOFF以外の時グラフ文字表示
		if (option->op.scoregraph != option->OP_SCORE_GRAPH_OFF) {
			DrawGraph(0, 0, H_GAME_STR_SCORE_GRAPH, TRUE);
		}

		//数値表意
		DrawNumber(1094, 536, SKY_PERFECT, 25, 1, 0, H_SMALL_NUMBER_CYAN);
		DrawNumber(1094, 576, PERFECT, 25, 1, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1094, 616, GOOD, 25, 1, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1094, 656, MISS, 25, 1, 0, H_SMALL_NUMBER_BLUE);

		DrawNumber(1216, 536, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1216, 576, int(cbpm + 0.5), 25, 0, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1216, 616, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_SMALL_NUMBER_BLUE);
		DrawNumber(1216, 656, int(cbpm* option->op.speed_val + 0.5), 25, 0, 0, H_SMALL_NUMBER_GREEN);





		for (i = 0; i <= 3; i++) {//ノートを叩いた時のフラッシュ、判定表示
			for (j = 0; j <= NOTE_HIT_LARGE_FLASH_NUMBER - 1; j++) {//4個までなら同時に表示
				if (note_hit_flash[j][i] >= 0)DrawGraph(lane[i], judge_area, H_HIT[11 - (int)note_hit_flash[j][i]], TRUE);//ノートを叩いた時のフラッシュ
				if (note_hit_large_flash[j][i] >= 0)DrawGraph(lane[i] - 22, judge_area - 22, H_HIT_LARGE[19 - (int)note_hit_large_flash[j][i]], TRUE);//光るノートを叩いた時のフラッシュ
			}




			if (dnote_hit_flash[i] >= 0)DrawGraph(lane[i], judge_area, H_HIT_D[11 - (int)dnote_hit_flash[i]], TRUE);//黒ノートを叩いた時のフラッシュ
			if (hit_sky_perfect[i] > 0)DrawGraph(lane[i], int(-10 * cos(3.14*hit_sky_perfect[i] / 2) + 10 + judge_area), H_SKY_PERFECT, TRUE);
			if (hit_perfect[i] > 0)DrawGraph(lane[i], int(-10 * cos(3.14*hit_perfect[i] / 2) + 10 + judge_area), H_PERFECT, TRUE);
			if (hit_good[i]    > 0)DrawGraph(lane[i], int(-10 * cos(3.14*hit_good[i] / 2) + 10 + judge_area), H_GOOD, TRUE);
			if (hit_miss[i]    > 0)DrawGraph(lane[i], int(-10 * cos(3.14*hit_miss[i] / 2) + 10 + judge_area), H_MISS, TRUE);

			for (j = 0; j < CRTBuf; j++) {
				for (k = 0; k <= NOTE_HIT_LARGE_FLASH_NUMBER - 1; k++) {
					if (note_hit_flash[k][i] >= 0)note_hit_flash[k][i] -= 0.036;
					if (note_hit_large_flash[k][i] >= 0)note_hit_large_flash[k][i] -= 0.048;
				}
				if (dnote_hit_flash[i] >= 0)dnote_hit_flash[i] -= 0.036;
				if (hit_sky_perfect[i] >= 0)hit_sky_perfect[i] -= 0.006;
				if (hit_perfect[i] >= 0)hit_perfect[i] -= 0.006;
				if (hit_good[i] >= 0)hit_good[i] -= 0.006;
				if (hit_miss[i] >= 0)hit_miss[i] -= 0.006;
			}

			I_Slow[i].draw(GAME_passed_time);
			I_Fast[i].draw(GAME_passed_time);

		}

		if (ClearFlag != 0) {//演奏終了していたら中心カバーの表示
			DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			for (i = 0; i < CRTBuf; i++) {
				if (c_m_draw_counter > 0) {
					c_m_draw_counter -= 0.0012;
				}
			}
			if (c_m_draw_counter < 0) {
				//printfDx("flag\n");
				PlaySoundMem(SH_CLOSED, DX_PLAYTYPE_BACK, TRUE);
				c_m_draw_counter = int(0);
				if (str_draw_counter == -1)str_draw_counter = int(1);//CLEARED FAILEDカウンタを1に
			}
		}
		//printfDx("str_draw_counter:%f\n", str_draw_counter);
		//printfDx("c_m_draw_counter:%f\n", c_m_draw_counter);


		if(note[0][j_n_n[0]].color == 0 &&
			note[1][j_n_n[1]].color == 0 &&
			note[2][j_n_n[2]].color == 0 &&
			note[3][j_n_n[3]].color == 0 &&
			note[0][j_dn_n[0]].color == 0 &&
			note[1][j_dn_n[1]].color == 0 &&
			note[2][j_dn_n[2]].color == 0 &&
			note[3][j_dn_n[3]].color == 0 && MISS == 0){
			
			if (FullComboFXPlayFlag == 1) {
				FullComboFXBaseTime = int(GAME_passed_time + 0.5);
			}
			int PFC = (MISS == 0) && (GOOD == 0);
			DrawFullComboRainbow(&FullComboFXPlayFlag, &FullComboFXFrame, int(GAME_passed_time), FullComboFXBaseTime, FXH_FULLCOMBO, &FXPH_FULLCOMBO, PFC);//フルコンボエフェクトの再生

		}


		if (ClearFlag != 0 && str_draw_counter != -1 && c_m_draw_counter == 0) {
			if (ClearFlag == 1 && MISS == 0) {//ミスなし

				if (GOOD == 0) {//PERFECT FULLCOMBO
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - str_draw_counter) * 255));//PERFECT_FULLCOMBO表示
					DrawGraph(320, 310, H_PFC, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - str_draw_counter) * 255));//FULL_COMBO表示
					DrawGraph(320, 310, H_FULL_COMBO[(int(GAME_passed_time) / 50) % 6], TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
			}

			if (ClearFlag == 1 && MISS != 0 && option->op.gauge != option->OP_GAUGE_NO_FAIL) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - str_draw_counter) * 255));//CLEARED表示
				DrawGraph(320, 310, H_CLEARED, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}

			if (ClearFlag == 2) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - str_draw_counter) * 255));//FAILED表示
				DrawGraph(320, 310, H_FAILED, TRUE);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}
			for (i = 0; i < CRTBuf; i++) {
				if (str_draw_counter > 0) {
					str_draw_counter -= 0.0012;
				}
				if (str_draw_counter <= 0) {
					str_draw_counter = 0;
				}
			}
		}


		if (start_c_draw_counter == 0 && ClearFlag == 0) {//スタート時の中心カバー上げ
			DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);//中心カバー
			if (gauge_draw_counter >= gauge - 0.001) {//曲名の透過度
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - sin((3.14 / 2)*c_m_draw_counter)) * 255));
			}
			SetFontSize(30);
			SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
			ShowExtendedStrFit(640, 350, Music[song_number].title[difficulty], title_width, 620, config,
				Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//曲名
			ShowExtendedStrFit(640, 450, Music[song_number].artist[difficulty], artist_width, 620, config);//アーティスト
			ShowExtendedStrFit(640, 260, Music[song_number].genre[difficulty], genre_width, 620, config);//ジャンル
			SetDrawMode(DX_DRAWMODE_NEAREST);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

			for (i = 0; i < CRTBuf; i++) {
				if ((c_m_draw_counter <= 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {
					c_m_draw_counter += 0.0012;
				}
				if ((c_m_draw_counter > 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {
					c_m_draw_counter = 1;
					start_c_draw_counter = 1;
				}
			}
		}


		/*
		SetFontSize(28);
		if (debug == 1) {
		DrawString(int(120 - ((double)150 / 2)) + 2, 690 + 2, "DEBUG MODE", GetColor(0, 0, 0));//デバッグ表示(影)
		DrawString(int(120 - ((double)150 / 2)), 690, "DEBUG MODE", GetColor(255, 255, 255));//デバッグ表示
		}


		if (debug_auto == 1) {
		DrawString(int(140 - ((double)150 / 2)) + 2, 650 + 2, "AUTO PLAY", GetColor(0, 0, 0));//デバッグ表示(影)
		DrawString(int(140 - ((double)150 / 2)), 650, "AUTO PLAY", GetColor(255, 255, 255));//デバッグ表示
		}
		*/


		if (*debug == 1 && SHOW_DEBUG_MODE == 1 && config.ShowDebug == 1) {
			printfDx(L"DEBUG MODE\n");

			if (debug_auto == 1) {
				printfDx(L"F1:AUTO PLAY:ON\n");
			}
			else {
				printfDx(L"F1:AUTO PLAY:OFF\n");
			}

			printfDx(L"F2:RELOAD\n");

			if (debug_sound == 1) {
				printfDx(L"F3:SOUND FX:ON\n");
			}
			else {
				printfDx(L"F3:SOUND FX:OFF\n");
			}
			if (debug_fail == 1) {
				printfDx(L"F4:FAIL:ON\n");
			}
			else {
				printfDx(L"F4:FAIL:OFF\n");
			}
			if (debug_music == 1) {
				printfDx(L"F5:MUSIC:ON\n");
			}
			else {
				printfDx(L"F5:MUSIC:OFF\n");
			}
			if (debug_stop == 1) {
				printfDx(L"SPACE:START\n");
			}
			else {
				printfDx(L"SPACE:STOP\n");
			}
			printfDx(L"↑↓:MOVE\n");
			printfDx(L"←→:PITCH:%d ×%.3f\n", pitch_step,pitch);

			printfDx(L"想定難易度:%d%%\n", Cdiff.level);
			printfDx(L"想定局所難易度:%d%%\n", Cdiff.level_local);
			printfDx(L"行:%d,%d,%d,%d%\n", note[0][j_n_n[0]].textLine
				, note[1][j_n_n[1]].textLine
				, note[2][j_n_n[2]].textLine
				, note[3][j_n_n[3]].textLine);

			//printfDx(L"再生位置:%fms\n", GAME_passed_time);


			printfDx(L"\n");

			printfDx(L"Global:%d\n", Cdiff.global);
			printfDx(L"Local:%d\n", Cdiff.local);
			printfDx(L"Chain:%d\n", Cdiff.chain);
			printfDx(L"Unstability:%d\n", Cdiff.unstability);
			printfDx(L"Streak:%d\n", Cdiff.longNote);
			printfDx(L"Color:%d\n", Cdiff.color);


			printfDx(L"F11:EDIT\n");

			//printfDx("insert:%x\n", insert);
			//printfDx("head.next:%x\n", head.next);
			//printfDx("&head:%x\n", &head);
		}





		//----Sound----

		/*
		for (i = 0; i <= 2;i++) {//BGR
		for (j = 0; j <= 3; j++) {
		if (Key[Button[i][j]] == 1) {
		if (i == 0) {
		ChangeVolumeSoundMem(int(255), SH_B[j]);
		PlaySoundMem(SH_B[j], DX_PLAYTYPE_BACK, TRUE);
		}
		if (i == 1) {
		ChangeVolumeSoundMem(int(255), SH_G[j]);
		PlaySoundMem(SH_G[j], DX_PLAYTYPE_BACK, TRUE);
		}
		if (i == 2) {
		ChangeVolumeSoundMem(int(255), SH_R[j]);
		PlaySoundMem(SH_R[j], DX_PLAYTYPE_BACK, TRUE);
		}
		}
		}
		}

		for (i = 0; i <= 2; i++) {//BGR
		for (j = 0; j <= 3; j++) {
		if (Key[Button[i][j]] == -1) {
		release_time[i][j] = GetNowCount_d(config);
		}
		}
		}

		for (i = 0; i <= 2; i++) {//BGR
		for (j = 0; j <= 3; j++) {
		if (Key[Button[i][j]] == 0 && !(debug_auto == 1 && LN_flag[j + 1] != 0)) {//(オートでLN中)じゃないときに離したら音ストップ

		if (i == 0)ChangeVolumeSoundMem(int((((double)150 + release_time[i][j] - GetNowCount_d(config)) / (double)150) * 255), SH_B[j]);
		if (i == 1)ChangeVolumeSoundMem(int((((double)150 + release_time[i][j] - GetNowCount_d(config)) / (double)150) * 255), SH_G[j]);
		if (i == 2)ChangeVolumeSoundMem(int((((double)150 + release_time[i][j] - GetNowCount_d(config)) / (double)150) * 255), SH_R[j]);



		if (GetNowCount_d(config) - release_time[i][j] >= 150) {
		if (i == 0)StopSoundMem(SH_B[j]);
		if (i == 1)StopSoundMem(SH_G[j]);
		if (i == 2)StopSoundMem(SH_R[j]);
		}
		}
		//ChangeVolumeSoundMem(int(((double)1 - cos(volume*(3.1415 / 2))) * 255), SH_);

		}
		}

		*/



		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[2][i]] == 1 && AllowSound[2][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[1][i]] == 1 && AllowSound[1][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}
		for (i = 0; i <= 3; i++) {//4列分
			if (Key[Button[0][i]] == 1 && AllowSound[0][i] == 1) {//スイッチが押された瞬間でそこから音を鳴らすのが許可されているとき
				PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);
			}
		}

		for (i = 0; i <= 3; i++) {
			AllowSound[0][i] = 1;
			AllowSound[1][i] = 1;
			AllowSound[2][i] = 1;
		}

		/*
		if (Key[Button[2][0]] == -1 || Key[Button[2][1]] == -1 || Key[Button[2][2]] == -1 || Key[Button[2][3]] == -1) {
		StopSoundMem(SH_R);
		}
		if (Key[Button[1][0]] == -1 || Key[Button[1][1]] == -1 || Key[Button[1][2]] == -1 || Key[Button[1][3]] == -1) {
		StopSoundMem(SH_G);
		}
		if (Key[Button[0][0]] == -1 || Key[Button[0][1]] == -1 || Key[Button[0][2]] == -1 || Key[Button[0][3]] == -1) {
		StopSoundMem(SH_B);
		}
		*/

		double BGM_VolTowardResult = 1;
		if (ClearFlag != 0) {//クリア表示4秒後にゲームの終了(結果発表へ)

			BGM_VolTowardResult = 1 - (double)(GAME_passed_time - cleared_time) / (double)TimeFromEndOfGameToResult;
		}

		ChangeVolumeSoundMem(int(((double)1 - cos(volume * (3.1415 / 2))) * BGM_VolTowardResult * 255 * debug_music), SH_SONG);//曲の音量セット



		//printfDx("%d\n", int(((double)44100 * (GAME_passed_time - Music[song_number].songoffset[difficulty])) / 1000));



		if ((GAME_passed_time + LOOP_passed_time > Music[song_number].songoffset[difficulty]) && (CheckSoundMem(SH_SONG) == 0 && debug_stop == 0) && (ClearFlag == 0) && (bgmplay == 0)) {
			// 1/60sの後ろずれを防ぐためフレーム時間分前倒しで再生
			if (FirstBgmPlay == 0) {//最初のBGM再生でないときのみ再生位置変更処理を行う(先頭が削れて再生されるのを防ぐため)
				SetCurrentPositionSoundMem(int(((double)44100 * pitch * ((GAME_passed_time + TimePerFrame) - Music[song_number].songoffset[difficulty])) / 1000), SH_SONG);
			}
			PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);//曲の再生開始
			bgmplay = 1;
			FirstBgmPlay = 0;
		}

		//printfDx("%f\n",LOOP_passed_time);
		
		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000/config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		//printfDx("frames:%d\n", (int)LOOP_passed_time+i);
		//Sleep(3);//s 負荷が大きいので長めに待つ


		ScreenFlip();//画面の反映
		clsDx();
	}

	return;
}

//黒以外のノートのヒット判定
void GAME_judge_note(int note_search_init, NOTE **note, int lane, int GAME_passed_time, int judge_time_bad, int H_TG, int H_1B, int H_1A, int H_2B, int H_2A, int H_3B, int H_3A, int H_F, int *score, int *LN_flag, int *searching) {
	int cash = 0;
	int note_search = note_search_init;

	while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == 8) {
		note_search++;
	}
	*searching = note_search;
	while (1) {

		if (note[lane][note_search].timing_real - GAME_passed_time > judge_time_bad) {//判定対象のノートがまだ判定時間に達していない
			*score = -10000;
			break;//while(1)を抜ける
		}

		cash = note[lane][note_search].color == H_TG || note[lane][note_search].color == H_1B || note[lane][note_search].color == H_2B || note[lane][note_search].color == H_3B || note[lane][note_search].color == H_F;
		//通常ノート判定
		if (cash == 1 && note[lane][note_search].group != 1 && note[lane][note_search].group != 2) {//判定対象のノートにH_TG(またはH_F)が含まれていてLNの始点、終点ではないとき

			if (note[lane][note_search].color == H_F) {//H_Fならhit=1で叩いたことにする
				note[lane][note_search].hit = 1;
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_TG) {//H_TGならhit=1で叩いたことにする
				note[lane][note_search].hit = 1;
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_1B) {//H_1Bなら
				note[lane][note_search].color = H_1A;//H_1Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_2B) {//H_2Bなら
				note[lane][note_search].color = H_2A;//H_2Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_3B) {//H_3Bなら
				note[lane][note_search].color = H_3A;//H_3Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

		}

		//LN始点判定(LNは餡蜜ができないようにする)
		if ((cash == 1 && note[lane][note_search].group == 1)//判定対象のノートにH_TG(またはH_F)が含まれていてLNの始点のとき
			&& ((note_search == note_search_init) || (note[lane][note_search_init].group == 2 && note_search == note_search_init + 1))) {
			//でまだ叩いてないノートがLNの前に無いとき、またはLN終点から探し始めてる時で次のノーツ(LN始点)があるとき

			if (note[lane][note_search].color == H_F) {//H_FならLN_flag[lane] = 2でLN開始
				LN_flag[lane] = 2;
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_TG) {//H_TGならLN_flag[lane] = 2でLN開始
				LN_flag[lane] = 2;
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_1B) {//H_1Bなら
				note[lane][note_search].color = H_1A;//H_1Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_2B) {//H_2Bなら
				note[lane][note_search].color = H_2A;//H_2Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

			if (note[lane][note_search].color == H_3B) {//H_3Bなら
				note[lane][note_search].color = H_3A;//H_3Aに変える
				*score = GAME_passed_time - note[lane][note_search].timing_real;
				break;
			}

		}


		if (note[lane][note_search].color == 0) {//最後以降のノーツに来たら
			*score = -10000;
			break;
		}
		else {//note_searchのノートがH_TG,H_Fを含んでいない
			do {
				note_search++;//一つ上のノートを探す
				*searching = note_search;
			} while (note[lane][note_search].hit == 1 || note[lane][note_search].color_init == 8);//もう叩かれてるノートの番号ならもう一回note_searchをインクリメント
		}
	}

	return;
}

//黒ノートのヒット判定
void GAME_judge_dark_note(int note_search, NOTE **note, int lane, int GAME_passed_time, int judge_time_dark, int H_K, int *dark_hit, int *searching) {
	int cash = 0;
	while (note[lane][note_search].hit == 1) {//判定枠をまだ通っていない音符までカウンタを進める
		note_search++;
	}
	*searching = note_search;
	while (1) {

		if (note[lane][note_search].timing_real - GAME_passed_time >= judge_time_dark) {//判定対象のノートがまだ判定時間に達していない
			*dark_hit = 0;
			break;//while(1)を抜ける
		}

		cash = note[lane][note_search].color == H_K;
		if (cash == 1) {//判定対象のノートにH_Kが含まれている

			if (note[lane][note_search].color == H_K && note[lane][note_search].hit == 0) {//H_Kならhit=1で叩いたことにする
				note[lane][note_search].hit = 1;
				*dark_hit = 1;
				break;
			}

		}

		if (note[lane][note_search].color == 0) {//最後以降のノーツに来たら
			*dark_hit = 0;
			break;
		}
		else {//note_searchのノートがH_Kを含んでいない
			do {
				note_search++;//一つ上のノートを探す
				*searching = note_search;
			} while (note[lane][note_search].hit == 1);//もう叩かれてるノートの番号ならもう一回note_searchをインクリメント
		}
	}

	return;
}


void DrawFullComboRainbow(int *play, int *step, int Time, int baseTime, int effectResourceHandle, int *playingEffectHandle, int PFC) {//フルコンボエフェクト
	
	int BarWidth = 640;

	//-1~1の3次関数で動きを作る
	int Rainbowy1 = int(-pow((double)(Time - baseTime - 1000) / 1000, 3) * 720);
	int Rainbowy2 = int(-pow((double)(Time - baseTime - 1000) / 1000, 3) * 720 + 720 * 2);

	
	//PFC時に虹表示
	if (PFC == 1) {
		SetDrawBlendMode(DX_BLENDMODE_ADD, 50);
		for (int j = 0; j < BarWidth; j++) {
			DrawLine(320 + j, Rainbowy1, 320 + j, Rainbowy2, GetRainbowColor(BarWidth - 1 - j, BarWidth), 1);
		}
	}


	if (*play == 1 && *step == 0) {//再生フラグが1の時
		*playingEffectHandle = PlayEffekseer2DEffect(effectResourceHandle);
		*play = 0;
	}
	// 再生中のエフェクトを移動する。

	SetPosPlayingEffekseer2DEffect(*playingEffectHandle, 640, 360, 0);


	// Effekseerにより再生中のエフェクトを更新する。
	//1/60秒ごとに更新
	while (((double)(Time - baseTime) / 16.666) >= *step) {
		UpdateEffekseer2D();
		*step = *step + 1;
	}

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	// Effekseerにより再生中のエフェクトを描画する。
	DrawEffekseer2D();


}


void DebugInitializeProcess(
	int* combo,
	int* CourseCombo,
	int* CourseMaxCombo,
	double* score,
	int* SKY_PERFECT,
	int* PERFECT,
	int* GOOD,
	int* MISS,
	int* TimePerfect,
	int* TimeGood,
	int* TimeMiss,
	int* TimeNextHour,
	int* MAX_COMBO,
	double* gauge,
	int* bgmplay,
	int* bcc,
	int* scc,
	double* cscroll,
	double scrollchange_scroll,
	double* sc_timing,
	double* real_timing,

	int* stop_se_c,
	double* stop_time_sum,
	double* stop_time,
	int* stopFlag,


	int* j_n_n,
	int* j_dn_n,
	int* j_dn_push_n,
	int* LN_flag,
	int* LN_flash,
	BARLINE* barline,
	NOTE** note



) {//デバッグ時の移動処理などの共通処理
	int i = 0;
	int j = 0;

	combo = 0;
	*CourseCombo = 0;
	*CourseMaxCombo = 0;
	*score = 0;
	*SKY_PERFECT = 0;
	*PERFECT = 0;
	*GOOD = 0;
	*MISS = 0;
	*TimePerfect = 0;
	*TimeGood = 0;
	*TimeMiss = 0;
	*TimeNextHour = 7;
	*MAX_COMBO = 0;
	*gauge = 100;
	*bgmplay = 0;
	*bcc = 0;
	*scc = 0;
	*cscroll = scrollchange_scroll;
	*sc_timing = 0;
	*real_timing = 0;

	*stop_se_c = 0;
	*stop_time_sum = 0;
	*stop_time = 0;
	*stopFlag = 0;

	for (i = 0; i <= 3; i++) {
		j_n_n[i] = 0;
		j_dn_n[i] = 0;
		j_dn_push_n[i] = 0;
		LN_flag[i] = 0;
		LN_flash[i] = 0;
		barline[i].fall = 0;
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
			note[i][j].hit = 0;
			note[i][j].fall = 0;
			note[i][j].color = note[i][j].color_init;
			//if (i != 0)note[i][j].color = note[i][j].color_init;
			//note[i][j].timing -= 240000 / cbpm;
		}
	}
	*bcc = 1;

	return;
}
