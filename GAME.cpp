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
#include "GetValueFromController.h"
#include "CoverView.h"
#include "Asset.h"
#include "AppContext.h"
#include "DetailView.h"
#include "AutoDifficultyPrediction.h"
#include "DxLibUtil.h"
#include "NPLoadSoundMem.h"
#include "NoteJudgeProcessor.h"
#include "GradationNoteImage.h"

using namespace std;

void GAME(int song_number, int difficulty,
	RESULT *res, int *escape, Option *option, int *retryAble,
	int *debug, Song *Music, int Button[3][4], int Button_Shutter, int *Key, char *Buf,
	int secret,//隠し曲演出中か(0:通常 1:演出中)
	ANDROID_CONTROLLER *AC,
	Config config,
	IR_SETTING* ir,
	int SkillTestFlag,//段位認定モードか(0:通常 1~4:段位認定モード ステージ数を示す)
	double *GaugeVal,
	int *CourseCombo,
	int *CourseMaxCombo,
	int AllowExit
	)
{
	Asset asset;//使う画像セット
	//コンテキスト
	AppContext appContext = AppContext(NULL, option, &config);
	ActivityContext context = ActivityContext(&appContext, &asset);


	HANDLE hComm, hEvent;
	setupSerial(&hComm, config);//シリアル通信設定
	STATE LED_state;
	OVERLAPPED ovl;
	DWORD dwWritten;
	hEvent = CreateEvent(NULL, FALSE, FALSE, L"olp");
	memset(&ovl, 0, sizeof(OVERLAPPED));
	ovl.hEvent = hEvent;
	//コントローラのボリューム値取得
	int controllerVolume = 0;
	GetValueFromController ControllerVolume(hComm);
	ControllerVolume.sendVolumeRequest();


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
	int H_R;

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

	TransValue viewAlpha = TransValue(&context);
	viewAlpha.value = 255;

	auto playAnimationOnEscAtStart = [&]() {
		//viewAlpha.clearEvent();
		//viewAlpha.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 0, 500);
		//viewAlpha.play();
	};
	auto playAnimationOnEscAtPlay = [&]() {
		//viewAlpha.clearEvent();
		//viewAlpha.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 1000, 1500);
		//viewAlpha.play();
	};
	auto playAnimationOnFinish = [&]() {
		viewAlpha.clearEvent();
		viewAlpha.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 3400, 3900);
		viewAlpha.play();
	};

	int FXH_FULLCOMBO = LoadEffekseerEffect(L"img/FullComboFX/FullComboFX.efk", 25.0f);//effekseerエフェクトリソースハンドル
	// 再生中のエフェクトのハンドルを初期化する。
	int FXPH_FULLCOMBO = -1;
	int FullComboFXPlayFlag = 1;//フルコンボエフェクト再生フラグ
	int FullComboFXBaseTime = 0;
	int FullComboFXFrame = 0;


	int H_TITLE_STR;//画像として保存された曲名の画像ハンドル
	H_TITLE_STR = MakeScreen(640, 48, TRUE);
	
	int H_GENRE_STR;//画像として保存されたジャンルの画像ハンドル
	H_GENRE_STR = MakeScreen(640, 48, TRUE);

	int H_ARTIST_STR;//画像として保存されたアーティストの画像ハンドル
	H_ARTIST_STR = MakeScreen(640, 48, TRUE);

	//グラデーション画像の用意
	int screenHandle;
	screenHandle = MakeScreen(640, 48, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	int err = SetDrawScreen(screenHandle);
	// 乗算済みアルファ用アルファブレンドのブレンドモードに設定する
	SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
	SetDrawBright(255, 255, 255);

	err = DrawExtendGraph(0, 0, 640, 35,
		context.getAsset()->img(L"img/gradation.png").getHandle(),
		true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	GAME_SH SH;
	int AllowSound[3][4] = { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } };//そのフレームでコントローラを叩いた時の音を出すか コントローラの並び順と同じ(0:B 1:G 2:R)
	double HitAreaBright[3][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0} };//コントローラを叩いた時の判定枠の各色の光具合

	int SH_SONG;//曲データのハンドル
	TransValue songVolume = TransValue(&context);
	songVolume.value = 255;
	songVolume.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 0, 800);

	int	SH_CLOSE;
	int	SH_CLOSED;
	int	SH_OPEN;
	int SH_JINGLE;

	TransValue jingleVolume = TransValue(&context);
	jingleVolume.value = 255;
	jingleVolume.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 0, 500);
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
	high_speed = option->op.speedVal;//段位以外ではあらかじめ決めた値を格納
	double scale = 1;//判定の横の広がり幅
						//int lane1 = int(512 - 240 * scale);
						//int lane2 = int(512 - 80 * scale);
						//int lane3 = int(512 + 80 * scale);
						//int lane4 = int(512 + 240 * scale);
	int laneCordinateX[4] = {
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
	enum LnFlag {
		Nothing = 0,//LNは来ていない
		Missed = 1,//そこのレーンのLNを押し続けなければならない ミス済み
		MustPush = 2,//そこのレーンのLNを押し続けなければならない 手を離すとミスになりMissedになる
		Completed = -1,//押し切った状態
	};
	
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
	double c_m_draw_counter = 0;//中心カバー画像を表示するカウンタ(0~1 0:スタート前、終了後 1:プレイ中)
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
	int detail_width = 1;//詳細横幅
	int jingleflag = 0;//ジングルを鳴らすフラグ
	int openflag = 0;//中心カバーが開くときの音を鳴らすフラグ
	int cleared_time = 0;//クリアした時間を記録
	int isStartCoverMoveUpComplete = 0;//スタート時の中心カバー描画フラグ
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

	int isMoviePlaying = 0;//BGAを流したかどうか(最初に一回だけ再生するためのフラグ)
	int FirstMoviePlay = 1;//今から流すBGAが最初の再生かどうかのフラグ

	CALCDIFF Cdiff;//計算で出した難易度要素目安
	AutoDifficultyPrediction adp;
	double autoDifficultyPredictionResult;
	if(*debug == 1)	autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);


	int bcc = 0;//bpmchangeのカウンタ
	int scc = 0;//scrollchangeのカウンタ
	double sc_timing = 0;//前にスクロール変化があった時間
	double cscroll = 1;//現在の実時間倍率(譜面全体のスピード倍率)
	double GAME_passed_time_scroll = 0;//譜面としての経過時間(scrollを考えた分)
	double real_timing = 0;//前にscrollが変化したときのGAME_passed_timeの時間
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

	NoteSearcher noteSearcher(note);
	
	//小節線構造体メモリ確保
	BARLINE *barline;
	barline = (BARLINE*)calloc(BARLINE_MAX_NUMBER, sizeof(BARLINE));

	int blc = 0;//barline counter

	NOTE hit_n[4];//最後に叩いたノートの情報

	NoteJudgeProcessor noteJudgeProcessor;


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
	if (option->op.gauge == (OptionItem::Gauge::NORMAL)) {
		total_base = 375;
		miss_percent = 0.05;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 3;//ゲージ減少量が4%以下にならないようにする
		gauge_dec_MAX = 3;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == (OptionItem::Gauge::HARD)) {
		total_base = 225;
		miss_percent = 0.05;//(125~200コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 6;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 6;//ゲージ減少量が10%以上にならないようにする
	}
	if (option->op.gauge == (OptionItem::Gauge::SUPER_HARD)) {//10回ミスすると失敗
		total_base = 100;//ゲージ回復100
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 10;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 10;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == (OptionItem::Gauge::FC_ATTACK)) {//ミスすると失敗
		total_base = 0;//ゲージ回復なし
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 100;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 100;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == (OptionItem::Gauge::PFC_ATTACK)) {//MISS,GOODすると失敗
		total_base = 0;//ゲージ回復なし
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 100;//ゲージ減少量が8%以下にならないようにする
		gauge_dec_MAX = 100;//ゲージ減少量が8%以上にならないようにする
	}
	if (option->op.gauge == (OptionItem::Gauge::SKILL_TEST)) {//段位ゲージ 24個で1MISS分回復するようにノート数補正をかけるy
		total_base = 125;
		miss_percent = 0;//(250~500コンボの時)一気にミスったらFAILEDになる総ノーツに対する割合(1:100%)
		gauge_dec_MIN = 1.5;//ゲージ減少量
		gauge_dec_MAX = 1.5;//ゲージ減少量

		if (option->op.color == OptionItem::Color::RAINBOW) {//基本の時は減少量多め
			gauge_dec_MIN = 3;//ゲージ減少量
			gauge_dec_MAX = 3;//ゲージ減少量
		}

	}

	SongSub MusicSub;
	SpeedBuffer speedBuffer = SpeedBuffer(note, j_n_n);

	GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll, &playing_time, config, 1, NULL, SkillTestFlag);//noteに譜面情報を入れる(譜面部分のロード)
	
	speedBuffer.updateAverage();


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

	if (option->op.color != OptionItem::Color::RAINBOW) {//通常モードだったら
		sprintfDx(filenameHighScore, L"%s/result_%s.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameLatest, L"%s/latest_result_%s.dat", Music[song_number].SaveFolder, scoreDiff[difficulty - 1]);
		sprintfDx(filenameRival, L"%s/result_%s.dat", Music[song_number].RivalSaveFolder, scoreDiff[difficulty - 1]);
	}
	if (option->op.color == OptionItem::Color::RAINBOW) {//虹モードだったら
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
	wchar_t *ReadNameRGB[11] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"d",L"f",
	L"bright",L"note_Long_hit_b" };

	for (i = 0; i <= 10; i++) {
		wchar_t strcash[128];
		sprintfDx(strcash, L"img/notes/%s/%s.png", option->note[option->op.note], ReadNameRGB[i]);

		H_NOTE[i + 1] = LoadGraph(strcash);

		wchar_t strcash_l[128];
		sprintfDx(strcash_l, L"img/notes/%s/%sl.png", option->note[option->op.note], ReadNameRGB[i]);
		H_LNOTE[i + 1] = LoadGraph(strcash_l);
	}

	wchar_t tmpPath[128];

	sprintfDx(tmpPath, L"img/notes/%s/JudgeArea.png", option->note[option->op.note]);
	H_JUDGE_AREA = LoadGraph(tmpPath);
	sprintfDx(tmpPath, L"img/notes/%s/JudgeAreaPaint.png", option->note[option->op.note]);
	H_JUDGE_AREA_PAINT = LoadGraph(tmpPath);

	GradationNoteImage gradationLongNote(wstring(L"img/notes/")+wstring(option->note[option->op.note]));

	int Keylist[3][4] = { { Button[0][0],Button[0][1],Button[0][2],Button[0][3] },
	{ Button[1][0],Button[1][1],Button[1][2],Button[1][3] },
	{ Button[2][0],Button[2][1],Button[2][2],Button[2][3] } };//キーのリスト

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
	if (option->op.gauge == (OptionItem::Gauge::NO_FAIL))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_no_fail.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::NORMAL))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_normal.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::HARD))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_hard.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::SUPER_HARD))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_super_hard.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::FC_ATTACK))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_fc.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::PFC_ATTACK))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_pfc.png");//(左上95,105)大きさ130*510
	if (option->op.gauge == (OptionItem::Gauge::SKILL_TEST))H_GAUGE_BAR = LoadGraph(L"img/gauge_bar_skill_test.png");//(左上95,105)大きさ130*510

	H_R = LoadGraph(L"img/R.png");

	LoadDivGraph(L"img/hit.png", 16, 4, 4, 256, 256, H_HIT);
	LoadDivGraph(L"img/hit_large.png", 20, 4, 5, 300, 300, H_HIT_LARGE);
	LoadDivGraph(L"img/hit_d.png", 16, 4, 4, 256, 256, H_HIT_D);
	H_COMBO = LoadGraph(L"img/combo.png");


	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg_game.png")).c_str());


	if (difficulty == 1) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_sunny.png")).c_str());//難易度によってカバー変更
		H_DIFFICULTY = LoadGraph(L"img/weather/sunny.png");
	}
	if (difficulty == 2) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_cloudy.png")).c_str());
		H_DIFFICULTY = LoadGraph(L"img/weather/cloudy.png");
	}
	if (difficulty == 3) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_rainy.png")).c_str());
		if (Music[song_number].season[difficulty] != 4) {//雪じゃないとき
			H_DIFFICULTY = LoadGraph(L"img/weather/rainy.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/weather/snowy.png");
		}
	}
	if (difficulty == 4) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
		if (Music[song_number].season[difficulty] != 4) {//雪じゃないとき		
			H_DIFFICULTY = LoadGraph(L"img/weather/thunder.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/weather/snowstorm.png");
		}
	}


	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());

	//中央カバー
	DrawableInitParam coverViewParam;
	coverViewParam.cordinate = Cordinate(0, 0);
	CoverView coverView = CoverView(&context, coverViewParam);
	
	
	
	H_FAILED = LoadGraph(L"img/failed.png");
	H_CLEARED = LoadGraph(L"img/cleared.png");

	H_CLEARED = LoadGraph(L"img/cleared.png");
	if (option->op.gauge == OptionItem::Gauge::NORMAL)H_CLEARED = LoadGraph(L"img/cleared_normal.png");
	if (option->op.gauge == OptionItem::Gauge::HARD)H_CLEARED = LoadGraph(L"img/cleared_hard.png");
	if (option->op.gauge == OptionItem::Gauge::SUPER_HARD)H_CLEARED = LoadGraph(L"img/cleared_super_hard.png");
	if (option->op.color == OptionItem::Color::RGB_ONLY || 
		option->op.color == OptionItem::Color::CMY_ONLY ||
		option->op.color == OptionItem::Color::W_ONLY)H_CLEARED = LoadGraph(L"img/cleared_easy.png");

	LoadDivGraph(L"img/FULL_COMBO.png", 6, 1, 6, 640, 100, H_FULL_COMBO);
	H_PFC = LoadGraph(L"img/PERFECT_FULLCOMBO.png");

	LoadDivGraph(L"img/score_number.png", 10, 10, 1, 64, 100, H_COMBO_NUMBER);
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
	int H_SMALL_NUMBER_MAGENTA[10];

	LoadDivGraph(L"img/SmallNumberRed.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_RED);
	LoadDivGraph(L"img/SmallNumberGreen.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_GREEN);
	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_BLUE);
	LoadDivGraph(L"img/SmallNumberYellow.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_YELLOW);
	LoadDivGraph(L"img/SmallNumberCyan.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_CYAN);
	LoadDivGraph(L"img/SmallNumberMagenta.png", 10, 10, 1, 25, 50, H_SMALL_NUMBER_MAGENTA);
	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_POP_NUMBER);

	int fastSlowY = -130;
	int fastSlowYMove = 10;

	std::wstring fast = L"img/judge/fast_c.png";
	std::wstring slow = L"img/judge/slow_r.png";

	if (option->op.fastSlow == OptionItem::FastSlow::ON_FAST_RED) {
		fast = L"img/judge/fast_r.png";
		slow = L"img/judge/slow_c.png";
	}

	Cordinate fastSlowCordinate[4];
	DrawableInitParam fastSlowParams[4];
	Image I_Fast[4];
	Image I_Slow[4];

	for (i = 0; i < 4; i++) {
		fastSlowCordinate[i] = Cordinate(laneCordinateX[i], judge_area + fastSlowY);
		fastSlowParams[i].visible = 0;
		fastSlowParams[i].cordinate = fastSlowCordinate[i];
		I_Fast[i] = Image(&context, asset.img(fast.c_str()), fastSlowParams[i]);
		I_Slow[i] = Image(&context, asset.img(slow.c_str()),  fastSlowParams[i]);
	}

	//出現アニメーション
	auto fastSlowAppear = [&] {
		if (option->op.fastSlow != OptionItem::FastSlow::OFF) {
			if (timingDifference < 0) {
				I_Fast[i].clearAllEvent();
				I_Fast[i].visible.eON();
				I_Fast[i].alpha.eSet(255);

				I_Fast[i].alpha.eChange(255, 0, Converter(Linear), 100, 300);
				I_Fast[i].visible.eOFF(300);

				I_Slow[i].clearAllEvent();
				I_Slow[i].visible.eOFF();

				//I_Fast[i].setLoopAll(TRUE);
				I_Fast[i].playAll();
				I_Slow[i].playAll();
			}
			else {
				I_Slow[i].clearAllEvent();
				I_Slow[i].visible.eON();
				I_Slow[i].alpha.eSet(255);

				I_Slow[i].alpha.eChange(255, 0, Converter(Linear), 100, 300);
				I_Slow[i].visible.eOFF(300);

				I_Fast[i].clearAllEvent();
				I_Fast[i].visible.eOFF();

				I_Fast[i].playAll();
				I_Slow[i].playAll();



			}
		}
	};

	sprintfDx(tmpPath, L"sound/hit_sound/%s/f2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_N = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_L = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/f1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_S = NPLoadHitSoundMem(tmpPath, option, 1);

	sprintfDx(tmpPath, L"sound/hit_sound/%s/r2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_N = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/r3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_L = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/r1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_S = NPLoadHitSoundMem(tmpPath, option, 1);

	sprintfDx(tmpPath, L"sound/hit_sound/%s/g2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_N = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/g3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_L = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/g1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_S = NPLoadHitSoundMem(tmpPath, option, 1);

	sprintfDx(tmpPath, L"sound/hit_sound/%s/b2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_N = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/b3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_L = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/b1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_S = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/k.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_K = NPLoadHitSoundMem(tmpPath, option, 1);
	sprintfDx(tmpPath, L"sound/hit_sound/%s/release.wav", option->hitSound[option->op.hitSound]);
	SH.SH_RELEASE = NPLoadHitSoundMem(tmpPath, option, 1);

	//曲データは非同期で読み込む
	SetUseASyncLoadFlag(TRUE);
	SH_SONG = NPLoadBgmSoundMem(Music[song_number].wavpath[difficulty], option);
	BOOL isOverSongPlayTiming = false;

	bool isPlayMovie;
	isPlayMovie = strcmpDx(Music[song_number].moviePath[difficulty], L"") != 0;//動画パスが存在すれば動画を再生すると見なす
	if (option->op.movie == OptionItem::Movie::OFF)isPlayMovie = false;

	int MovieGraphHandle = 0;
	LONGLONG movieFrameTime = 0;//動画ファイルの1フレームの時間(us)

	auto updateSeekTime = [&](int seekTime) {
		SeekMovieToGraph(MovieGraphHandle, seekTime);
	};

	if (isPlayMovie) {
		MovieGraphHandle = LoadGraph(Music[song_number].moviePath[difficulty]);
		movieFrameTime = GetOneFrameTimeMovieToGraph(MovieGraphHandle);
		updateSeekTime(0);
	}

	BOOL isOverMoviePlayTiming = false;

	int movieHeight;
	int movieWidth;
	GetGraphSize(MovieGraphHandle, &movieWidth, &movieHeight);

	const int drawAreaX = 1280;
	const int drawAreaY = 720;
	double movieScale = 1;
	const double drawAreaAspecRatio = (double)drawAreaX / drawAreaY;
	const double movieAspectRatio = (double)movieWidth / movieHeight;
	if (movieAspectRatio >= drawAreaAspecRatio) {//横長の動画
		movieScale = (double)drawAreaX / movieWidth;
	}
	else {//縦長の動画
		movieScale = (double)drawAreaY / movieHeight;
	}

	const int movieX1 = (int)(640 - 0.5 * movieWidth * movieScale);
	const int movieY1 = (int)(360 - 0.5 * movieHeight * movieScale);
	const int movieX2 = (int)(640 + 0.5 * movieWidth * movieScale);
	const int movieY2 = (int)(360 + 0.5 * movieHeight * movieScale);


	SetUseASyncLoadFlag(FALSE);
	SH_CLOSE = NPLoadFxSoundMem(L"sound/close.wav", option);
	SH_CLOSED = NPLoadFxSoundMem(L"sound/closed.wav", option);
	SH_OPEN = NPLoadFxSoundMem(L"sound/open.wav", option);
	SH_SHUTTER = NPLoadFxSoundMem(L"sound/shutter.wav", option);
	SH_SHUTTER_SIGNAL = NPLoadFxSoundMem(L"sound/shutter_signal.wav", option);
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
	SH_JINGLE = NPLoadBgmSoundMem(Music[song_number].jinglepath[difficulty], option);

	//重複処理ラムダ式

	//変数初期化
	auto initVariableProcess = [&] {
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
		for (j = 0; j <= MusicSub.totalMeasures[difficulty] - 1; j++) {
			barline[j].fall = 0;
		}
		for (i = 0; i <= 3; i++) {
			j_n_n[i] = 0;
			j_dn_n[i] = 0;
			j_dn_push_n[i] = 0;
			LN_flag[i] = LnFlag::Nothing;
			LN_flash[i] = 0;
			for (j = 0; j <= MusicSub.objOfLane[difficulty][i] - 1; j++) {
				note[i][j].hit = 0;
				note[i][j].fall = NoteFall::NotFaling;
				note[i][j].group = note[i][j].group_init;
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

		speedBuffer.updateAverage();
	};

	//ボタン音再生
	auto soundHitSound = [&] {
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
	};

	//透過度の取得 Movieを再生するかによって変わります
	auto getAlpha = [&](int alphaMovieOff, int alphaForMovieNormal, int alphaForMovieClear, int alphaInit = 255) {
		int alpha = 255;
		if (isPlayMovie) { 
			if (option->op.movie == OptionItem::Movie::ON_NORMAL)alpha = alphaForMovieNormal;
			if (option->op.movie == OptionItem::Movie::ON_CLEAR)alpha = alphaForMovieClear;		}
		else { 
			alpha = alphaMovieOff;
		}

		return alphaInit - (alphaInit - alpha) * c_m_draw_counter;



	};

	//----曲名表示前処理----


	gauge_draw_counter = 0;

	int FontHandleBpm = CreateFontToHandle(L"メイリオ", 28, 9, DX_FONTTYPE_ANTIALIASING_EDGE);//フォントハンドル
	int FontHandleCommon = CreateFontToHandle(L"メイリオ", 30, 9, DX_FONTTYPE_ANTIALIASING_EDGE);//フォントハンドル

	genre_width = GetDrawStringWidthToHandle(Music[song_number].genre[difficulty], wcslen(Music[song_number].genre[difficulty]), FontHandleCommon);//ジャンル名の横の長さを入れる
	title_width = GetDrawStringWidthToHandle(Music[song_number].title[difficulty], wcslen(Music[song_number].title[difficulty]), FontHandleCommon);//曲名の横の長さを入れる
	artist_width = GetDrawStringWidthToHandle(Music[song_number].artist[difficulty], wcslen(Music[song_number].artist[difficulty]), FontHandleCommon);//アーティスト名の横の長さを入れる
	
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
	SetDrawScreen(H_TITLE_STR);
	ShowExtendedStrFitToHandle(320, 1, Music[song_number].title[difficulty], title_width, 620, config, FontHandleCommon, Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//曲名

	GraphBlend(H_TITLE_STR, screenHandle, 255, DX_GRAPH_BLEND_DODGE);//覆い焼き

	SetDrawScreen(H_ARTIST_STR);
	ShowExtendedStrFitToHandle(320, 1, Music[song_number].artist[difficulty], artist_width, 620, config, FontHandleCommon);//アーティスト
	
	SetDrawScreen(H_GENRE_STR);
	ShowExtendedStrFitToHandle(320, 1, Music[song_number].genre[difficulty], genre_width, 620, config, FontHandleCommon);//ジャンル
	
	SetDrawScreen(DX_SCREEN_BACK);

	DrawableInitParam detailViewParam = DrawableInitParam(Cordinate(320,500));
	DetailView detailView = DetailView(&context, detailViewParam);
	detailView.setText(Music[song_number].detail[difficulty]);


	double draw_alpha_speed = 1;//段位用Speed表示フラグ(これが0になるまでは開始しない)

	//ハイスピを計算
	if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
		option->op.speedVal = option->speedVal[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
	}
	else {
		option->op.speedVal = 1;
	}
	high_speed = option->op.speedVal;


	//ターゲットスコアを決める
	int targetScore = 0;
	int targetScore2 = 0;

	if (option->op.targetScore1 == OptionItem::TargetScore1::E)targetScore = 5000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::D)targetScore = 6000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::C)targetScore = 7000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::B)targetScore = 8000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::A)targetScore = 9000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::S)targetScore = 9500;
	if (option->op.targetScore1 == OptionItem::TargetScore1::POINT_10000)targetScore = 10000;
	if (option->op.targetScore1 == OptionItem::TargetScore1::MAX)targetScore = 10000 + Music[song_number].total_note[difficulty];

	if (option->op.targetScore2 == OptionItem::TargetScore2::LATEST) {
		targetScore2 = latestScore.score;
	}
	else {

	}
	//必要な場合 IRランキングからスコアを取得
	if (option->op.scoreGraph == OptionItem::ScoreGraph::ON &&
		(option->op.targetScore2 != OptionItem::TargetScore2::LATEST)) {
		getTargetScore(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, option->op.color == OptionItem::Color::RAINBOW, static_cast<int>(option->op.targetScore2), highScore.score, ir->rivalID, config);
		targetScore2 = LoadTargetScore(Music[song_number].SaveFolder);
	}

	bool isSkip = false;
	GAME_start_time = GetNowCount_d(config);
	while (1) {
		appContext.updateTime();
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
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

		//コントローラから値を取得
		ControllerVolume.start();
		if (ControllerVolume.updateVal(&controllerVolume)) {
			//曲開始前は何にも使わない
		}

		Get_Key_State(Buf, Key, AC);
		if (Key[KEY_INPUT_ESCAPE] == 1 && *escape == 0 && AllowExit == 1) {
			*escape = 1;
			ClearFlag = 2;
			c_m_draw_counter = 0.00001;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
			gauge_draw_counter = gauge;
			draw_alpha = 1;
			jingleVolume.play();
			playAnimationOnEscAtStart();
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
			isSkip = true;
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

		}

		bool isGaugeDrawCompleted = (gauge_draw_counter >= gauge - 0.001);
		bool isSongLoadCompleted = (CheckHandleASyncLoad(SH_SONG) != TRUE);
		bool isSpeedAlphaZero = draw_alpha_speed == 0;
		bool isFinishJingle = CheckSoundMem(SH_JINGLE) == 0 || isSkip;
		if (isGaugeDrawCompleted && isSongLoadCompleted && isSpeedAlphaZero && isFinishJingle) {//ゲージが上まで描写されて曲の読み込みが完了して(エラーでも続行)段位用speed表示が消えてジングル音が再生終了したら演奏開始
			detailView.hide();
			jingleVolume.play();
			break;
		}

		gauge_draw_hosei = xtosinx(gauge_draw_counter);

		number_digit(int(score + 0.5), score_digit, 5);//スコアを桁ごとに格納
		number_digit(int(cbpm + 0.5), bpm_digit, 5);//BPMを桁ごとに格納
		number_digit(int(PassedTime_Hours), time_hours_digit, 2);//時間を桁ごとに格納
		number_digit(int(PassedTime_Minutes), time_minutes_digit, 2);//分を桁ごとに格納
		
	    //Draw
		SetDrawScreen(appContext.baseHandle.getHandle());
		ClearDrawScreen();//グラフィックを初期化
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
		DrawGraph(laneCordinateX[0], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][1] * 160 + 75), int(HitAreaBright[1][1] * 160 + 75), int(HitAreaBright[0][1] * 160 + 75));
		DrawGraph(laneCordinateX[1], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][2] * 160 + 75), int(HitAreaBright[1][2] * 160 + 75), int(HitAreaBright[0][2] * 160 + 75));
		DrawGraph(laneCordinateX[2], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][3] * 160 + 75), int(HitAreaBright[1][3] * 160 + 75), int(HitAreaBright[0][3] * 160 + 75));
		DrawGraph(laneCordinateX[3], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(255, 255, 255);

		for (i = 0; i <= 3; i++) {
			DrawGraph(laneCordinateX[i], judge_area, H_JUDGE_AREA, TRUE);//判定枠の表示
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

		DrawGraph(320, 350, H_TITLE_STR, TRUE);
		DrawGraph(320, 450, H_ARTIST_STR, TRUE);
		DrawGraph(320, 260, H_GENRE_STR, TRUE);
		//DrawGraph(320, 100, context.getAsset()->img(L"img/gradation.png").getHandle(), TRUE);

		//ShowExtendedStrFitToHandle(640, 350, Music[song_number].title[difficulty], title_width, 620, config, FontHandleCommon, Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//曲名
		//ShowExtendedStrFitToHandle(640, 450, Music[song_number].artist[difficulty], artist_width, 620, config, FontHandleCommon);//アーティスト
		//ShowExtendedStrFitToHandle(640, 260, Music[song_number].genre[difficulty], genre_width, 620, config, FontHandleCommon);//ジャンル
		//ShowExtendedStrFitToHandleNoShadow(640, 500, Music[song_number].detail[difficulty], detail_width, 630, config, FontHandleDetail);//詳細
		detailView.draw();
		SetDrawMode(DX_DRAWMODE_NEAREST);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));

		//スコア描画
		if (option->op.color == OptionItem::Color::RAINBOW) {//虹オプションのときR表示
								 //SetDrawBright(190, 190, 190);
			DrawGraph(960, -3, H_R, TRUE);
			//SetDrawBright(255, 255, 255);
		}

		if (option->op.color != OptionItem::Color::RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1170 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1170 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		if (option->op.color == OptionItem::Color::RAINBOW) {
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
		if (option->op.scoreGraph != OptionItem::ScoreGraph::OFF) {
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
		DrawBoxWithLine(1130, 580, 1264, 620, GetColor(255, 50, 255), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//BPM
		DrawBoxWithLine(1130, 620, 1264, 660, GetColor(50, 50, 255), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//MINBPM
		DrawBoxWithLine(1130, 660, 1264, 700, GetColor(25, 255, 25), int((double)draw_alpha * 80), int((double)draw_alpha * 255));//SPEED

		//判定、BPM文字表示
		DrawGraph(0, 0, H_GAME_STR_JUDGE_BPM, TRUE);

		//SCORE GRAPHがOFF以外の時グラフ文字表示
		if (option->op.scoreGraph != OptionItem::ScoreGraph::OFF) {
			DrawGraph(0, 0, H_GAME_STR_SCORE_GRAPH, TRUE);
		}


		//数値表示
		DrawNumber(1094, 536, SKY_PERFECT, 25, 1, 0, H_SMALL_NUMBER_CYAN);
		DrawNumber(1094, 576, PERFECT, 25, 1, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1094, 616, GOOD, 25, 1, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1094, 656, MISS, 25, 1, 0, H_SMALL_NUMBER_BLUE);

		DrawNumber(1216, 536, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1216, 576, int(cbpm + 0.5), 25, 0, 0, H_SMALL_NUMBER_MAGENTA);
		DrawNumber(1216, 616, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_SMALL_NUMBER_BLUE);
		DrawNumber(1216, 656, int(speedBuffer.getAverage() * option->op.speedVal + 0.5), 25, 0, 0, H_SMALL_NUMBER_GREEN);



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
				//Option->op.speedVal = Option->speedVal[Option->op.speed];//実際の倍率を入れる
				if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
					option->op.speedVal = option->speedVal[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
				}
				else {
					option->op.speedVal = 1;
				}
				high_speed = option->op.speedVal;

			}
			if (Key[Keylist[1][3]] == 1 || Key[KEY_INPUT_RIGHT] == 1) {//→
				/*
				TempSpeed += 0.1;
				if (TempSpeed >= 4.0)TempSpeed = 4;
				*/
				draw_alpha_speed = 1;
				
				option->op.speed++;
				option->op.speed = number_ring(option->op.speed, option->SPEED_NUM - 1);//0~11に収める
				//Option->op.speedVal = Option->speedVal[Option->op.speed];//実際の倍率を入れる
				if (Music[song_number].bpm_suggested[difficulty] != 0) {//瞬間風速が0ではないときにハイスピを合わせる
					option->op.speedVal = option->speedVal[option->op.speed] / (double)Music[song_number].bpm_suggested[difficulty];
				}
				else {
					option->op.speedVal = 1;
				}
				high_speed = option->op.speedVal;
			}



			wchar_t SpeedStr[128] = L"(0～0,0)";
			int SpeedStrWidth = 0;

			//速さ表示
			sprintfDx(SpeedStr, L"(%d～%d,%d)",
				int(Music[song_number].bpmmin[difficulty] * high_speed + 0.5),
				int(Music[song_number].bpmmax[difficulty] * high_speed + 0.5),
				int(Music[song_number].bpm_suggested[difficulty] * high_speed + 0.5));
			
			SpeedStrWidth = GetDrawStringWidth(SpeedStr, wcslen(SpeedStr));
			ShowExtendedStrFitToHandle(640, 2 + 48 * 14, SpeedStr, SpeedStrWidth, 620, config, FontHandleBpm);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 255));
		//SOUND
		if (jingleflag == 0) {
			PlaySoundMem(SH_JINGLE, DX_PLAYTYPE_BACK, TRUE);
			jingleflag = 1;

			detailView.show();
		}

		soundHitSound();

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

	if (ClearFlag == 0) {
		int volume = option->windbreakVal[static_cast<int>(option->op.windbreak)];
		ControllerVolume.getValIfExist(&volume);

		coverView.setFirstOpenRange(volume);

		coverView.startMiddleCover();//カバーを事前設定した高さまで上げる
	}
	else if(ClearFlag==2){
		coverView.setFirstOpenRange(1000);
		coverView.finishMiddleCover();//カバーを下げる

	}
	while (1) {
		appContext.updateTime();
		if (ProcessMessage() != 0) {
			turn_off_LED(hComm, &LED_state, &dwWritten, &ovl);//LEDを消す
			dxLibFinishProcess();
			return;
		}

		//----Button----

		//printfDx(L"%f", (double)controllerVolume / 255);

		Get_Key_State(Buf, Key, AC);

		if (Key[KEY_INPUT_ESCAPE] == 1 && *escape == 0 && ClearFlag == 0 && AllowExit == 1) {
			*escape = 1;
			ClearFlag = 2;
			//c_m_draw_counter = 0;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);
			if (isStartCoverMoveUpComplete) {
				coverView.finishMiddleCover();//開き切っている時は今の位置から閉じる
			}
			else {
				coverView.middleCover.setReverseAll(TRUE);//今開いている動作を逆転して閉じる
			}
			songVolume.play();
			playAnimationOnEscAtPlay();
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

		//コントローラから値を取得
		if (isStartCoverMoveUpComplete == 1 && ClearFlag == 0) {//スタート時の中心カバー上げが完了している
			ControllerVolume.start();
			if (ControllerVolume.updateVal(&controllerVolume)) {
				coverView.setOpenRange(controllerVolume);
			}
			else {//コントローラから入力が無い時キーボードでのカバー操作許可

				if (Key[KEY_INPUT_LSHIFT] == 1 || Key[KEY_INPUT_RSHIFT] == 1) {
					coverView.openMiddleCover();
				}
				if (Key[KEY_INPUT_LCONTROL] == 1 || Key[KEY_INPUT_RCONTROL] == 1) {
					coverView.closeMiddleCover();
				}

				if ((Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) && (Key[KEY_INPUT_LCONTROL] == -1 || Key[KEY_INPUT_RCONTROL] == -1)) {
					coverView.openMiddleCover();
				}
				if ((Key[KEY_INPUT_LSHIFT] == -1 || Key[KEY_INPUT_RSHIFT] == -1) && (Key[KEY_INPUT_LCONTROL] >= 1 || Key[KEY_INPUT_RCONTROL] >= 1)) {
					coverView.closeMiddleCover();
				}
				if (Key[KEY_INPUT_LSHIFT] == 0 && Key[KEY_INPUT_RSHIFT] == 0 && Key[KEY_INPUT_LCONTROL] == 0 && Key[KEY_INPUT_RCONTROL] == 0) {
					coverView.stopMiddleCover();
				}
			}

		}

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

				initVariableProcess();
			}
			if ((Key[KEY_INPUT_DOWN] == 1 || (Key[KEY_INPUT_DOWN] >= PressFrame && debug_warp_counter >= 20)) && GAME_passed_time >= 0) {//一拍戻る(GAME_passed_time >= 0で戻りすぎ防止)
				debug_warp_counter = 0;
				debug_warp = 1;
				debug_warp_time = (double)60000 / cbpm;
				if (1000 <= debug_warp_time) {//60BPMの1拍以上は進まないようにする
					debug_warp_time = 1000;
				}
				debug_time_passed -= debug_warp_time;
				
				initVariableProcess();
			}

			if (Key[KEY_INPUT_RIGHT] == 1 || (Key[KEY_INPUT_RIGHT] >= PressFrame && debug_warp_counter >= 40)) {//ピッチを上げる
				if (pitch_step <= 25) {//27以上上げると音声が追い付かないので禁止
					pitch_step++;//半音上げる
					pitch *= ROOT12_2;//12乗根2をかける
					if (pitch_step == 0)pitch = 1;//誤差を考慮してpitch_stepが0のときpitchを1にセット
					SetFrequencySoundMem(int(frequency*pitch), SH_SONG);

					debug_warp = 1;
					debug_time_passed -= GAME_passed_time - GAME_passed_time / ROOT12_2;//再生位置のずれを補正 GAME_passed_time-GAME_passed_time/ROOT12_2が差 
					GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll,&playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
					autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);
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

					initVariableProcess();
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
					GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll,&playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
					autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);
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

					initVariableProcess();
				}
			}
			if (Key[KEY_INPUT_LEFT] == -1 || Key[KEY_INPUT_RIGHT] == -1 || Key[KEY_INPUT_UP] == -1 || Key[KEY_INPUT_DOWN] == -1) {
				isMoviePlaying = 0;
				PauseMovieToGraph(MovieGraphHandle);
			}

			if (Key[KEY_INPUT_SPACE] == 1) {//再生停止(開始)
				if (debug_stop == 0) {
					debug_stop = 1;
					debug_stoped_time = GetNowCount_d(config);
					bgmplay = 0;
					isMoviePlaying = 0;
				}
				else {
					if (debug_stop == 1) {
						debug_stop = 0;
						debug_time_passed -= debug_stop_time;
						debug_stop_time = 0;
					}
				}
				StopSoundMem(SH_SONG);
				PauseMovieToGraph(MovieGraphHandle);
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
				double startTime = GetNowCount_d(config);

				debug_warp = 1;
				GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll, &playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
				autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);
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

				isMoviePlaying = 0;
				PauseMovieToGraph(MovieGraphHandle);
				if (debug_stop == 1)debug_stop_time += GetNowCount_d(config) - startTime;
				initVariableProcess();
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
					isMoviePlaying = 0;
					StopSoundMem(SH_SONG);
					PauseMovieToGraph(MovieGraphHandle);
				}
				double EDIT_passed_time = (GAME_passed_time - Music[song_number].noteoffset[difficulty])*pitch;
				if (EDIT_passed_time < 0) {
					EDIT_passed_time = 0;
				}

				GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll, &playing_time, config, 1, &score_cell_head);//score_cell_headに譜面情報をロード
				
				double debug_time_passed_in_EDIT_SCORE = debug_time_passed * pitch;//再生速度を変えている分debug_time_passedが変化しているのでEDIT_SCOREに渡すときに補正しておく
				
				EDIT_SCORE(&score_cell_head,
					Music, 
					&MusicSub,
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
					AC, config, SH_SONG);

				debug_time_passed = debug_time_passed_in_EDIT_SCORE / pitch;//今の再生速度での時間に戻す
				debug_stop_time = GetNowCount_d(config) - debug_stoped_time;//debug_stop_timeの更新ができていなかったのでする
				double startTime = GetNowCount_d(config);

				//譜面再読み込み
				debug_warp = 1;
				GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange, scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll, &playing_time, config, pitch);//noteに譜面情報を入れる(譜面部分のロード)
				autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);

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

				debug_stop_time += GetNowCount_d(config) - startTime;
				initVariableProcess();
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
					bool isLnNothing = (LN_flag[i] == LnFlag::Nothing) || (LN_flag[i] == LnFlag::Completed);

					if (Key[Keylist[j][i]] == 1 && isLnNothing) {
						int judge_dark = 0;
						NoteJudgeButtonColor buttonColor = (NoteJudgeButtonColor)(2 - j);

						noteJudgeProcessor.GAME_judge_note(j_n_n[i], note, i, int(GAME_passed_time), int(judge_time_bad), buttonColor, &timingDifference, LN_flag, &searching);//LN始点叩くとLN_flag変化
						
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
								if (note[i][searching].isBright != 0) {
									note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
									note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
								}

								gauge += total / Music[song_number].total_note[difficulty];
								if (note[i][searching].isBright != 0) {//光るノートなら2倍
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

									fastSlowAppear();
									
								}
								TimePerfect++;


								if (debug_sound == 1) {
									PlayHitSound(2, note[i][searching].color_init, note[i][searching].isBright, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								hit_n[i] = note[i][searching];

								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (laneCordinateX[i] + 128 - 640) * 2),
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
								if (note[i][searching].isBright != 0) {//光るノートなら2倍
									gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
								}

								score += (double)0.5* score_note;
								GOOD++;
								TimeGood++;

								fastSlowAppear();

								if (debug_sound == 1) {
									PlayHitSound(1, note[i][searching].color_init, note[i][searching].isBright, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								hit_n[i] = note[i][searching];
								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (laneCordinateX[i] + 128 - 640) * 2),
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
								if (note[i][searching].color_init == NoteColor::R || note[i][searching].color_init == NoteColor::G || note[i][searching].color_init == NoteColor::B || note[i][searching].color_init == NoteColor::F) {//RGBFは一倍
									gauge_dec_ratio = 1;
								}
								else if (note[i][searching].color_init == NoteColor::Y || note[i][searching].color_init == NoteColor::C || note[i][searching].color_init == NoteColor::M) {//CMYは減少量0.5倍
									gauge_dec_ratio = 0.5;
								}
								else if (note[i][searching].color_init == NoteColor::W) {//Wは減少量1/3倍
									gauge_dec_ratio = 0.3333334;
								}

								gauge -= gauge_dec * gauge_dec_ratio;
								if (note[i][searching].isBright != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec * gauge_dec_ratio;
								}

								score -= score_dec;
								MISS++;
								TimeMiss++;
								AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない

								fastSlowAppear();
							}
						}

						if (note[i][j_n_n[i]].group == NoteGroup::LongNoteStart && LN_flag[i] == LnFlag::MustPush) {
							j_n_n[i]++;//LN始点を叩いた時ノートカウンタを1上げる
							speedBuffer.updateAverage();//NOWSPEED算出

							if (abs(timingDifference) <= judge_time_perfect) {//PERFECT以内
								if (abs(timingDifference) <= judge_time_sky_perfect) {//SKY_PERFECT
									LN_judge[i] = 3;
								}
								else {
									LN_judge[i] = 2;

									fastSlowAppear();
								}

								//PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);

								if (debug_sound == 1) {
									PlayHitSound(2, note[i][searching].color_init, note[i][searching].isBright, SH);
									AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
								}
								//hit_n[i] = note[i][searching];


							}
							else {//GOODでLN始点を叩いた
								if (abs(timingDifference) <= judge_time_good) {
									LN_judge[i] = 1;

									fastSlowAppear();

									if (debug_sound == 1) {
										PlayHitSound(1, note[i][searching].color_init, note[i][searching].isBright, SH);
										AllowSound[j][i] = 0;//j行i列のスイッチからの音を出さない
									}
									//hit_n[i] = note[i][searching];


								}
							}
							if ((abs(timingDifference) <= judge_time_bad) && (abs(timingDifference) >= judge_time_good)) {//遅く、早く叩き過ぎた(MISS)
								LN_judge[i] = 0;
								LN_flag[i] = LnFlag::Missed;
								
								fastSlowAppear();
							}


						}

						if (timingDifference == -10000) {//判定関数で黒を除き何も反応しなかったとき
							judge_dark = 1;//このフレームの入力で黒の判定を行う
						}

						if (debug_warp != 1 && judge_dark == 1) {//ワープ時には黒の判定やらず、このフレームで普通の音符が反応していない(judge_dark==1)とき黒音符の叩き判定をする
							noteJudgeProcessor.GAME_judge_dark_note(j_dn_n[i], note, i, int(GAME_passed_time), int(judge_time_dark), &dark_hit, &searching);
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

				if (LN_flag[i] != LnFlag::Nothing && (note[i][j_n_n[i] - 1].group == NoteGroup::LongNoteStart || note[i][j_n_n[i] - 1].group == NoteGroup::LongNoteMiddle)) {//iレーンのLNが判定枠に重なっているとき(かつjnnがLNをしっかり指している)
					bool isNextLnNodeOver = GAME_passed_time - note[i][j_n_n[i]].timing_real > 0 - TimePerFrame;//LN終点または中間点が判定枠を超えているかどうか
					
					auto isPushColor = [&](NoteColor inputColor, int lane){
						return(
							 (inputColor == NoteColor::R && Key[Keylist[2][lane]] >= 1)//R
							|| (inputColor == NoteColor::G && Key[Keylist[1][lane]] >= 1)//G
							|| (inputColor == NoteColor::B && Key[Keylist[0][lane]] >= 1)//B
							|| (inputColor == NoteColor::Y && Key[Keylist[2][lane]] >= 1 && Key[Keylist[1][lane]] >= 1)//Y
							|| (inputColor == NoteColor::C && Key[Keylist[1][lane]] >= 1 && Key[Keylist[0][lane]] >= 1)//C
							|| (inputColor == NoteColor::M && Key[Keylist[0][lane]] >= 1 && Key[Keylist[2][lane]] >= 1)//M
							|| (inputColor == NoteColor::W && Key[Keylist[2][lane]] >= 1 && Key[Keylist[1][lane]] >= 1 && Key[Keylist[0][lane]] >= 1)//W
							|| (inputColor == NoteColor::F && (Key[Keylist[2][lane]] >= 1 || Key[Keylist[1][lane]] >= 1 || Key[Keylist[0][lane]] >= 1)));//F

					};


					//LN_pushフラグの計算 対応する色を押しているときは1,離したフレームは-1,それ以外の離しているとき0
					if (isPushColor(note[i][j_n_n[i] - 1].color_init, i) || isPushColor(note[i][j_n_n[i]].color_init, i)) {
						LN_push[i] = 1;
					}
					else if (LN_push[i] == 1) {
						LN_push[i] = -1;
					}
					else {
						LN_push[i] = 0;
					}


					int endIndex = noteSearcher.searchLnEnd(i, j_n_n[i]);
					if (note[i][endIndex].timing_real - GAME_passed_time <= judge_time_good && note[i][endIndex].LN_k == 1 && LN_push[i] == -1) {//黒終端が猶予時間判定内に来ているときに離したとき
						LN_flag[i] = LnFlag::Completed;//LN中だけどもう離していいことにする(押し切ったことにする)
						PlaySoundMem(SH.SH_RELEASE, DX_PLAYTYPE_BACK, TRUE);//黒終端リリース音
					}


					if (note[i][endIndex].timing_real - GAME_passed_time <= judge_time_delay && note[i][endIndex].LN_k == 0 && note[i][endIndex].group == NoteGroup::LongNoteEnd) {//通常のLN終点が猶予時間判定内に来たとき
						if (isPushColor(note[i][endIndex].color, i)) {//終端の色を押していたら
							LN_flag[i] = LnFlag::Completed;//LN中だけどもう離していいことにする(押し切ったことにする)
						}
					}

					if (LN_push[i] == 1
						|| LN_flag[i] == LnFlag::Completed//(押し切った状態に既になっているとき(LN_flag[i] == LnFlag::Completed)は押してなくても始点が判定枠で止まる)
						|| debug_stop == 1 || debug_auto == 1) {//autoなら常に押し続けている)

						if (note[i][j_n_n[i] - 1].group == NoteGroup::LongNoteMiddle) {
							//中間ノート途中で叩いた時は中間ノートを始点に変え、本来の始点までを叩いた扱いにする
							int ind = j_n_n[i] - 1;
							note[i][ind].group = NoteGroup::LongNoteStart;

							do {
								ind--;
								note[i][ind].hit = true;
							} while (note[i][ind].group != NoteGroup::LongNoteStart);
						}

						if (note[i][j_n_n[i]].LN_k == true || !isNextLnNodeOver) {
							note[i][j_n_n[i] - 1].timing = int(GAME_passed_time_scroll * TIMING_SHOW_RATIO);//始点を判定枠に表示し続けるために現在の経過時間をtimingに入れる
							note[i][j_n_n[i] - 1].timing_real = int(GAME_passed_time);//ちょっと離しても大丈夫にするため現在の経過時間(本当)をtiming_realに入れる
						}



						LN_flash[i] = 1;//LNを光らせる
					}
					else {//LN中なのに押してないときで押し切ってもいない(LN_flag[i]!=-1)
						int lnStartInd = noteSearcher.searchLnStart(i, j_n_n[i]);
						if ((GAME_passed_time - note[i][lnStartInd].timing_real > judge_time_delay)) {//LN始点のズレが猶予時間を超えた時
							if (LN_flag[i] == LnFlag::MustPush) {//初回(LNでの最初のミスはノート1個分のミスとして扱う)
								LN_flag[i] = LnFlag::Missed;
								gauge -= gauge_dec;
								if (note[i][j_n_n[i] - 1].isBright != 0) {//光るノートなら2倍減少
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
								if (note[i][j_n_n[i] - 1].isBright != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec / 500;
								}
								score -= score_dec / 500;
							}


						}
					}

					if (isNextLnNodeOver) {//LN終点または中間点が判定枠を超えて来たとき
						if (note[i][j_n_n[i]].LN_k == 0 || 
							(note[i][j_n_n[i]].LN_k == 1 && (LN_flag[i] == LnFlag::Missed ||LN_flag[i] == LnFlag::Completed) && LN_push[i] <= 0)) {
							//終端が通常終端 または黒終端で既に消していい状態になっていて(LN_flag[i] == 1か-1)対応する色を押していないとき

							auto commonProcess = [&] {
								LN_flag[i] = LnFlag::Completed;//押し切った

								volume = 1;
								combo++;
								(*CourseCombo)++;
								combo_draw_counter = 1;

								//後ろに落ちるものの処理
								insert_cell(insert, img_num(Music[song_number].season[difficulty]),
									float(640 + (laneCordinateX[i] + 128 - 640) * 2),
									rand() % 10000,
									rand() % 10000,
									rand() % 401,
									0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
								insert = insert->next;//挿入する場所を後ろにする
								if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
								//hit_n[i] = note[i][j_n_n[i]];
								HitingNoteCount++; 
							};

							if (note[i][j_n_n[i]].group == NoteGroup::LongNoteMiddle) {
								if (LN_push[i] == 1) {
									note[i][j_n_n[i]].group = NoteGroup::LongNoteStart;
									note[i][j_n_n[i] - 1].hit = true;
								}
								if (debug_auto) {
									flash_LED(hComm, note[i][j_n_n[i]], &LED_state, i, &dwWritten, &ovl);//コントローラのLEDを光らせる
									//if (note[i][j_n_n[i]].color_init != note[i][j_n_n[i] - 1].color_init)PlayHitSound(2, note[i][j_n_n[i]].color_init, false, SH);
								}
							}
							else if (LN_flag[i] == LnFlag::Completed) {
								if ((LN_judge[i] == 2 || LN_judge[i] == 3) && debug_auto != 1) {//PERFECT以上
									TimePerfect++;

									if (note[i][j_n_n[i] - 1].isBright != 0) {
										note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
										note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}

									gauge += total / Music[song_number].total_note[difficulty];
									if (note[i][noteSearcher.searchLnStart(i, j_n_n[i])].isBright != 0) {//光るノートなら2倍
										gauge += total / Music[song_number].total_note[difficulty];
									}

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

									commonProcess();
								}
								else if (LN_judge[i] == 1 && debug_auto != 1) {//GOOD
									GOOD++;
									TimeGood++;
									hit_good[i] = 1;
									gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
									if (note[i][j_n_n[i] - 1].isBright != 0) {//光るノートなら2倍
										gauge += (double)total / Music[song_number].total_note[difficulty] / 2;
									}
									score += (double)0.5 * score_note;

									commonProcess();
								}
							}

							if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd && LN_flag[i] == LnFlag::Completed) {//押し切っているとき始点までのノートを消す(hit=1にする)
								noteSearcher.searchLnStart(i, j_n_n[i], 
									[](NOTE& note) {
										note.hit = true;
									}
								);

								note_hit_flash[note_hit_flash_rounder][i] = 11;//ノートを叩いた時の通常フラッシュ
								note_hit_flash_rounder = (note_hit_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
								hit_n[i] = note[i][j_n_n[i]];

							}

							if (LN_flag[i] == LnFlag::Completed && note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
								LN_flag[i] = LnFlag::Nothing;
								LN_flash[i] = 0;
								LN_judge[i] = 0;
							}

							if (note[i][j_n_n[i]].group_init == NoteGroup::LongNoteMiddle) {
								j_n_n[i]++;
							}
						}
						else {//黒終点でLN終端が判定枠通り過ぎてるのに対応する色を押し続けているとき
							note[i][j_n_n[i]].timing = int(GAME_passed_time_scroll * TIMING_SHOW_RATIO);//終点を判定枠に表示し続けるために現在の経過時間をtimingに入れる
							note[i][j_n_n[i]].timing_real = int(GAME_passed_time);//ちょっと離しても大丈夫にするため現在の経過時間(本当)をtiming_realに入れる


							if ((note[i][j_n_n[i]].timing_real - note[i][j_n_n[i]].timing_init_real) >= judge_time_good) {//終端が後ろにずれて本来の場所よりGOOD判定幅分ずれたら押し過ぎたとしてミスにする
								if (debug_sound == 1)PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);//黒ヒット音
								hit_miss[i] = 1;

								gauge -= gauge_dec;
								if (note[i][j_n_n[i] - 1].isBright != 0) {//光るノートなら2倍減少
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

								LN_flag[i] = LnFlag::Nothing;
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
						noteJudgeProcessor.GAME_judge_dark_note(j_dn_push_n[i], note, i, int(GAME_passed_time), int(con_jd), &dark_hit, &searching);
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
		


		if (option->op.gauge == OptionItem::Gauge::FC_ATTACK) {//銀ゲージでMISS出ていたら即失敗
			if (MISS >= 1) {
				gauge = 0;
			}
		}
		if (option->op.gauge == OptionItem::Gauge::PFC_ATTACK) {//金ゲージでGOOD,MISS出ていたら即失敗
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


		
		for (i = 0; i <= MusicSub.totalMeasures[difficulty] - 1; i++) {//小節線の座標計算
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
		for (j = 0; j <= 3; j++) {
			for (i = 0; i <= MusicSub.objOfLane[difficulty][j] - 1; i++) {//ノートの座標計算
				//音符

				note[j][i].x = laneCordinateX[j];//レーンのx座標
				note[j][i].y = -228;
				QE_x = high_speed * note[j][i].bpm * (GAME_passed_time_scroll - ((double)note[j][i].timing / TIMING_SHOW_RATIO)) + sqrt(1 / speed); //放物線のx座標を算出
				if ((QE_x >= 0) && (note[j][i].hit == 0) && note[j][i].fall != NoteFall::Failed) {//まだ叩かれてなく落ち切ってなく放物線の半分超えている(x>=0)とき
					note[j][i].fall = NoteFall::Faling;//落ちた(表示をする)
					QE_y = int(((double)judge_area - note_fall) * speed * pow(QE_x, 2) + note_fall);//GAME_passed_time_scroollを変数とした2次関数(値域0~1にnote_fallとjudge_areaで位置調整)
					if (QE_y >= 720) {//落ち切ったとき
						note[j][i].y = 720;
					}
					if (QE_y < 720) {
						note[j][i].y = (short)QE_y;
					}
					if (note[j][i].y >= 720 && (note[j][i].group == NoteGroup::Single || note[j][i].group == NoteGroup::LongNoteEnd)) {
						if (note[j][i].group == NoteGroup::LongNoteEnd) {
							//始点までの音符を全て落ち切ったことにする
							int ind = i;
							do{
								ind--;
								note[j][ind].fall = NoteFall::Failed;
							} while (note[j][ind].group != NoteGroup::LongNoteStart);
						}
						note[j][i].fall = NoteFall::Failed;//単ノートが落ちて見えなくなったら表示をしない(落ち切った)
											//printfDx("otikitta\n");
					}
				}
				else {
					if (note[j][i].fall != NoteFall::Failed)note[j][i].fall = NoteFall::NotFaling;//落ちてない(表示をしない)
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
				if (stopFlag != 1) {
					flash = 1;//フラッシュ表示フラグ(停止中は表示しない)
					//flash_LED_all(hComm, &LED_state, &dwWritten, &ovl);//コントローラのLEDを光らせる
				}
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

			//再生位置移動したとき
			for (i = 0; i <= 3; i++) {
				while ((GAME_passed_time_for_draw - note[i][j_n_n[i]].timing_real > judge_time_through) && debug_warp == 1 && note[i][j_n_n[i]].color != NoteColor::NONE) {
					if (debug_auto == 1) {
						/*
						note_hit_flash[i] = 11;
						if (note[i][j_n_n[i]].group != 1 && note[i][j_n_n[i]].isBright != 0)note_hit_large_flash[i] = 19;//光るノーツ用のフラッシュ
						*/
						volume = 1;

						if (note[i][j_n_n[i]].color != NoteColor::K &&
							(note[i][j_n_n[i]].group == NoteGroup::Single || note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd)) {
							combo++;
							(*CourseCombo)++;
							score += (double)score_note + 1;
							SKY_PERFECT++;
						}

						combo_draw_counter = 1;
						if (note[i][j_n_n[i]].group == NoteGroup::Single) {
							note[i][j_n_n[i]].hit = 1;//単ノートなら叩く
							if (note[i][j_n_n[i]].color_init != NoteColor::K)HitingNoteCount++;
						}
						if (note[i][j_n_n[i]].group == NoteGroup::LongNoteStart) {
							LN_flag[i] = LnFlag::MustPush;//LN始点ならLNフラグON
							if (note[i][j_n_n[i] + 1].group == NoteGroup::LongNoteMiddle) {
								if(GAME_passed_time_for_draw - note[i][j_n_n[i] + 1].timing_real > 0)note[i][j_n_n[i]].hit = 1;//次の音符が既に判定枠超えてたら消しておく
							}
						}
						if (note[i][j_n_n[i]].group == NoteGroup::LongNoteMiddle) {//中間ノートの始点化
							note[i][j_n_n[i] - 1].hit = 1;
							if (GAME_passed_time_for_draw - note[i][j_n_n[i] + 1].timing_real > 0)note[i][j_n_n[i]].hit = 1;//次の音符が既に判定枠超えてたら消しておく
							note[i][j_n_n[i]].group = NoteGroup::LongNoteStart;
						}
						if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
							LN_flag[i] = LnFlag::Nothing;//LN終点ならLNフラグOFF
							note[i][j_n_n[i] - 1].hit = 1;
							note[i][j_n_n[i]].hit = 1;
							HitingNoteCount++;
						}
						//hit_perfect[i] = 1;
						gauge += total / Music[song_number].total_note[difficulty];

						//PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);
					}
					if (note[i][j_n_n[i]].group == NoteGroup::LongNoteStart)LN_flag[i] = LnFlag::MustPush;//LN始点ならLNフラグON
					if (note[i][j_n_n[i]].group == NoteGroup::LongNoteMiddle) {//中間ノートの始点化
						note[i][j_n_n[i] - 1].hit = 1;
						note[i][j_n_n[i]].group = NoteGroup::LongNoteStart;
					}

					if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
						LN_flag[i] = LnFlag::Nothing;//LN終点ならLNフラグOFF
					}

					j_n_n[i]++;
					speedBuffer.updateAverage();//NOWSPEED算出
					if (j_n_n[i] >= MusicSub.objOfLane[difficulty][i])break;
				}

			}
			note[3][1];
			printfDx(L"j_n_n[3]:%d\n", j_n_n[3]);

			printfDx(L"LN_flag[3]:%d\n", LN_flag[3]);
			printfDx(L"LN_push[3]:%d\n", LN_push[3]);

			if (debug_auto == 1) {//オートプレイ中
				for (i = 0; i <= 3; i++) {
					if (debug_warp == 0) {
						if (j_n_n[i] <= MusicSub.objOfLane[difficulty][i] - 1) {
							while ((GAME_passed_time - (note[i][j_n_n[i]].timing_real - config.VsyncOffsetCompensation) >= 0 - TimePerFrame) && (note[i][j_n_n[i]].color != NoteColor::NONE)) {//ノートが判定枠に来た
								if (NoteColor::K != note[i][j_n_n[i]].color) {//黒以外のとき叩く(単ノート LN始点 LN終点)

									flash_LED(hComm, note[i][j_n_n[i]], &LED_state, i, &dwWritten, &ovl);//コントローラのLEDを光らせる

									if (note[i][j_n_n[i]].group == NoteGroup::Single || note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
										hit_n[i] = note[i][j_n_n[i]];

										note_hit_flash[note_hit_flash_rounder][i] = 11;//ノートを叩いた時の通常フラッシュ
										note_hit_flash_rounder = (note_hit_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}
									if (note[i][j_n_n[i]].group == NoteGroup::Single && note[i][j_n_n[i]].isBright != 0) {//単ノート
										note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
										note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}
									if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd && note[i][j_n_n[i] - 1].isBright != 0) {//LN終点
										note_hit_large_flash[note_hit_large_flash_rounder][i] = 19;//光るノーツ用のフラッシュ
										note_hit_large_flash_rounder = (note_hit_large_flash_rounder + 1) % NOTE_HIT_LARGE_FLASH_NUMBER;//0~3で回す
									}

									volume = 1;
									if (note[i][j_n_n[i]].group == NoteGroup::Single) {
										note[i][j_n_n[i]].hit = 1;//単ノートなら叩く
										HitingNoteCount++;
									}
									if (note[i][j_n_n[i]].group == NoteGroup::LongNoteStart)LN_flag[i] = LnFlag::MustPush;//LN始点ならLNフラグON

									if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
										LN_flag[i] = LnFlag::Nothing;//LN終点ならLNフラグOFF
										note[i][j_n_n[i] - 1].hit = 1;
										note[i][j_n_n[i]].hit = 1;
										HitingNoteCount++;
										LN_flash[i] = 0;
										if (note[i][j_n_n[i]].LN_k == 1 && debug_sound == 1) {//黒終端なら
											PlaySoundMem(SH.SH_RELEASE, DX_PLAYTYPE_BACK, TRUE);//黒終端リリース音
										}
									}

									if (note[i][j_n_n[i]].group == NoteGroup::Single || note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
										combo++;//LN始点じゃないときコンボ加算
										(*CourseCombo)++;
										combo_draw_counter = 1;

										hit_sky_perfect[i] = 1;//単ノート、LN終点でSKY_PERFECT文字を出す
										gauge += total / Music[song_number].total_note[difficulty];
										gauge += 0.1;

										if (note[i][j_n_n[i]].isBright != 0) {//光るノートなら2倍
											gauge += total / Music[song_number].total_note[difficulty];
										}

										if (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd && note[i][j_n_n[i] - 1].isBright != 0) {//光るノートなら2倍
											gauge += total / Music[song_number].total_note[difficulty];
										}
										score += (double)score_note + 1;
										SKY_PERFECT++;
										TimePerfect++;
									}

									//音の処理
									//if (note[i][j_n_n[i]].group == 0)PlaySoundMem(SH_HIT, DX_PLAYTYPE_BACK, TRUE);
									//if (note[i][j_n_n[i]].group == 1)PlaySoundMem(SH_HIT_LONG, DX_PLAYTYPE_BACK, TRUE);
									if (debug_sound == 1) {
										if (note[i][j_n_n[i]].group == NoteGroup::LongNoteMiddle || note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
											bool isColorDifferent = note[i][j_n_n[i]].color_init != note[i][j_n_n[i] - 1].color_init;
											if(isColorDifferent)PlayHitSound(2, note[i][j_n_n[i]].color_init, note[i][j_n_n[i]].isBright, SH);
										}
										else {
											PlayHitSound(2, note[i][j_n_n[i]].color_init, note[i][j_n_n[i]].isBright, SH);
										}
									}

									//後ろに落ちるものの処理
									if (note[i][j_n_n[i]].group == NoteGroup::Single || note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd) {
										insert_cell(insert, img_num(Music[song_number].season[difficulty]),
											float(640 + (laneCordinateX[i] + 128 - 640) * 2),
											rand() % 10000,
											rand() % 10000,
											rand() % 401,
											0.012 * (rand() % 100) + 2.4);//リストにひとつ挿入
										insert = insert->next;//挿入する場所を後ろにする
										if (head.next == NULL)head.next = insert;//1個めならそのポインタを先頭にする
									}

									//中間ノートの始点化
									if (note[i][j_n_n[i]].group == NoteGroup::LongNoteMiddle) {
										note[i][j_n_n[i] - 1].hit = 1;
										note[i][j_n_n[i]].group = NoteGroup::LongNoteStart;
									}
								}


								j_n_n[i]++;
								speedBuffer.updateAverage();//NOWSPEED算出
							}
							while ((note[i][j_n_n[i]].hit == 1 || note[i][j_n_n[i]].color == NoteColor::K) && (note[i][j_n_n[i]].color != NoteColor::NONE)) {//叩かれているか黒ならそれ以外になるまでカウンタを進める()
								j_n_n[i]++;
								if (note[i][j_n_n[i]].color != NoteColor::K)speedBuffer.updateAverage();//NOWSPEED算出
								if (j_n_n[i] >= MusicSub.objOfLane[difficulty][i])break;
							}
						}
					}
				}
			}

			for (i = 0; i <= 3; i++) {//音符見逃し処理
				if (debug_warp == 0) {
					if (j_n_n[i] <= MusicSub.objOfLane[difficulty][i] - 1) {
						if ((GAME_passed_time - note[i][j_n_n[i]].timing_real > judge_time_through) && (note[i][j_n_n[i]].color != NoteColor::NONE)//through時間を過ぎていった、または
							|| ((GAME_passed_time - note[i][j_n_n[i]].timing_real > judge_time_good) && (fabs(note[i][j_n_n[i] + 1].timing_real - GAME_passed_time) < judge_time_good))) {//(good判定を通り過ぎて次のノートがgood判定内にいるとき)ノートの見逃し(コンボが途切れる MISS)
							
							bool isSingleOrStart = (NoteColor::K != note[i][j_n_n[i]].color) && (note[i][j_n_n[i]].group != NoteGroup::LongNoteEnd);

							bool isLnEnd = (note[i][j_n_n[i]].group == NoteGroup::LongNoteEnd);
							bool isMustPushEnd = isLnEnd && (LN_flag[i] == LnFlag::MustPush);
							bool isMiss = isSingleOrStart || isMustPushEnd;//ミスにするべき音符かどうか
							
							if (isMiss) {
								if (note[i][j_n_n[i]].group == NoteGroup::LongNoteStart) {//LN始点のとき
									LN_flag[i] = LnFlag::Missed;//LN中(すでに先頭見逃し)にする
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
 								if (note[i][j_n_n[i]].isBright != 0) {//光るノートなら2倍減少
									gauge -= gauge_dec;
								}
								score -= score_dec;
								MISS++;
								TimeMiss++;
							}

							if (isLnEnd)LN_flag[i] = LnFlag::Nothing;

							j_n_n[i]++;
							speedBuffer.updateAverage();//NOWSPEED算出
						}
						while ((note[i][j_n_n[i]].hit == 1 || note[i][j_n_n[i]].color == NoteColor::K) && (note[i][j_n_n[i]].color != NoteColor::NONE)) {//叩かれているか黒ならそれ以外になるまでカウンタを進める()
							j_n_n[i]++;
							if(note[i][j_n_n[i]].color != NoteColor::K)speedBuffer.updateAverage();//NOWSPEED算出
							if (j_n_n[i] >= MusicSub.objOfLane[difficulty][i])break;
						}
					}
				}

				if (j_dn_n[i] <= MusicSub.objOfLane[difficulty][i] - 1) {//黒カウンタ(叩き)
					while ((GAME_passed_time - note[i][j_dn_n[i]].timing_real >  judge_time_dark) && (note[i][j_dn_n[i]].color != NoteColor::NONE)) {
						j_dn_n[i]++;
					}
					while ((note[i][j_dn_n[i]].hit == 1 || note[i][j_dn_n[i]].color != NoteColor::K) && (note[i][j_dn_n[i]].color != NoteColor::NONE)) {//叩かれているか黒じゃないか
						j_dn_n[i]++;
						if (j_dn_n[i] >= MusicSub.objOfLane[difficulty][i])break;
					}
				}
				if (j_dn_push_n[i] <= MusicSub.objOfLane[difficulty][i] - 1) {//黒カウンタ(押し込み)
					while ((GAME_passed_time - note[i][j_dn_push_n[i]].timing_real >  con_jd) && (note[i][j_dn_push_n[i]].color != NoteColor::NONE)) {
						j_dn_push_n[i]++;
					}
					while ((note[i][j_dn_push_n[i]].hit == 1 || note[i][j_dn_push_n[i]].color != NoteColor::K) && (note[i][j_dn_push_n[i]].color != NoteColor::NONE)) {//叩かれているか黒じゃないか
						j_dn_push_n[i]++;
						if (j_dn_push_n[i] >= MusicSub.objOfLane[difficulty][i])break;
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

			coverView.finishMiddleCover();//カバーを下げる
			playAnimationOnFinish();
		}
		if (GAME_passed_time >= Music[song_number].TimeToEnd[difficulty] && ClearFlag == 0
			&& note[0][j_n_n[0]].color == NoteColor::NONE
			&& note[1][j_n_n[1]].color == NoteColor::NONE
			&& note[2][j_n_n[2]].color == NoteColor::NONE
			&& note[3][j_n_n[3]].color == NoteColor::NONE) {//終了タイミングで失敗してなかったらクリア(ウィンドウクリックし続けてクリアする不正防止のためノートカウンタの位置も見る)
			ClearFlag = 1;
			c_m_draw_counter = 1;
			PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			cleared_time = int(GAME_passed_time);

			coverView.finishMiddleCover();//カバーを下げる
			playAnimationOnFinish();
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
		bool isGameFinishStrShowComplete = ClearFlag != 0 &&
			str_draw_counter == 0 &&
			c_m_draw_counter == 0;

		bool isShowingFinishStrTimeOver = (GAME_passed_time - cleared_time) >= TimeFromEndOfGameToResult;
		bool retryRequest = false;
		if (isGameFinishStrShowComplete && ClearFlag == 2 && !retryRequest && secret == 0) {
			retryRequest = (Key[Button[0][0]] >= 1 || Key[Button[0][1]] >= 1 || Key[Button[0][2]] >= 1 || Key[Button[0][3]] >= 1)
				&& (Key[Button[2][0]] >= 1 || Key[Button[2][1]] >= 1 || Key[Button[2][2]] >= 1 || Key[Button[2][3]] >= 1);
		}

		bool exitGame = isGameFinishStrShowComplete &&
			(isShowingFinishStrTimeOver || *escape == 1 || retryRequest);

		if (exitGame) {//クリア表示4秒後にゲームの終了(結果発表へ)
			if (*escape == 0) {//途中で抜け出してなかったら
							   //res->song_number = song_number;
				res->difficulty = difficulty;
				if (ClearFlag == 1) {
					if (option->op.gauge == OptionItem::Gauge::NORMAL) res->clear = CLEARTYPE_CLEARED;//NORMAL
					if (option->op.gauge == OptionItem::Gauge::HARD) res->clear = CLEARTYPE_HARD_CLEARED;//HARD
					if (option->op.gauge == OptionItem::Gauge::SUPER_HARD) res->clear = CLEARTYPE_SUPER_HARD_CLEARED;//SUPER HARD
					
					if (option->op.gauge == OptionItem::Gauge::NO_FAIL) {
						res->clear = CLEARTYPE_PLAY;//NO_FAIL
					}
					else {
						if (option->op.color == OptionItem::Color::RGB_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED(Onlyオプション使ってたら灰クリアに塗り替える)
						if (option->op.color == OptionItem::Color::CMY_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED
						if (option->op.color == OptionItem::Color::W_ONLY) res->clear = CLEARTYPE_EASY_CLEARED;//EASYCLEARED
					}
					if (option->op.gauge == OptionItem::Gauge::SKILL_TEST) res->clear = CLEARTYPE_PLAY;//段位ゲージ
				}

				if (SkillTestFlag != 0) {
					*GaugeVal = gauge;//段位認定のときはゲージ量継承
				}

				if (ClearFlag == 2)res->clear = CLEARTYPE_FAILED;
				if (ClearFlag == 1 && MISS == 0 && (
					option->op.color == OptionItem::Color::NONE ||
				    option->op.color == OptionItem::Color::RGB_CHANGE ||
					option->op.color == OptionItem::Color::RGB_RAND ||
					option->op.color == OptionItem::Color::SUPER_RAND ||
					option->op.color == OptionItem::Color::RAINBOW)
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
				GAME_LOAD(song_number, difficulty, note, barline, laneCordinateX, 0, &Cdiff, option, bpmchange,scrollchange, stopSequence, Music, &MusicSub, &TimeToEndScroll, &playing_time, config, 1);//ピッチが変更されているときはこの関数の終わりにピッチ1で読み込みなおす
			}

			CloseHandle(hComm);//シリアル通信の終了
			StopSoundMem(SH_SONG);//曲の再生停止
			PauseMovieToGraph(MovieGraphHandle);
			InitGraph();//メモリ開放
			InitSoundMem();//
			DeleteFontToHandle(FontHandleBpm);
			DeleteFontToHandle(FontHandleCommon);

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
		SetDrawScreen(appContext.baseHandle.getHandle());
		ClearDrawScreen();//グラフィックを初期化
		DrawGraph(0, 0, H_BG, TRUE);//背景
		show_cloud(H_CLOUD, &pos_cloud, (stopFlag != 1)*cbpm / 150, LOOP_passed_time);//雲

		viewAlpha.process();
		double viewAlphaRatio = (double)viewAlpha.value / 255;

		int movieLayer = H_DARKNESS;
		if (isOverMoviePlayTiming)movieLayer = MovieGraphHandle;
		if (isPlayMovie) {
			DrawGraph(0, 0, H_DARKNESS, TRUE);//動画再生時の黒背景
			DrawExtendGraph(movieX1, movieY1, movieX2, movieY2, movieLayer, FALSE);
		}

	    //後ろに落ちるものの表示と削除
		if (head.next != NULL) {//最低1つあるとき

			search_cell_all(head.next, &head, insert, &insert, H_FALL, LOOP_passed_time, (stopFlag != 1)*cbpm / 150);
			//printfDx("head.next:%d\n", head.next);
			//printfDx("&head:%d\n", &head);
			//printfDx("insert:%d\n", insert);
			//ScreenFlip();
		}

		//背景の明るさを抑える黒画像表示
		int BlendMode = DX_BLENDMODE_ALPHA;
		int BlendVal = 255;
		SetDrawBlendMode(BlendMode, int(BlendVal*(double)option->op.darkness/4));
		DrawGraph(0, 0, H_DARKNESS, TRUE);

		//ボタンを押した時の光
		BlendMode = DX_BLENDMODE_ALPHA;
		BlendVal = 255 * 0.5;
		SetDrawBlendMode(BlendMode, BlendVal);


		for (i = 0; i <= 3; i++) {//レーン
			for (j = 0; j <= 2; j++) {//色
				if (Key[Keylist[j][i]] >= 1) {
					HitAreaBright[j][i] = 1;//判定エリアの光
				}
			}
		}
		SetDrawBright(int(HitAreaBright[2][0] * 160 + 75), int(HitAreaBright[1][0] * 160 + 75), int(HitAreaBright[0][0] * 160 + 75));
		DrawGraph(laneCordinateX[0], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][1] * 160 + 75), int(HitAreaBright[1][1] * 160 + 75), int(HitAreaBright[0][1] * 160 + 75));
		DrawGraph(laneCordinateX[1], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][2] * 160 + 75), int(HitAreaBright[1][2] * 160 + 75), int(HitAreaBright[0][2] * 160 + 75));
		DrawGraph(laneCordinateX[2], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(int(HitAreaBright[2][3] * 160 + 75), int(HitAreaBright[1][3] * 160 + 75), int(HitAreaBright[0][3] * 160 + 75));
		DrawGraph(laneCordinateX[3], judge_area, H_JUDGE_AREA_PAINT, TRUE);
		SetDrawBright(255, 255, 255);

		BlendVal = 255 * 0.75;
		SetDrawBlendMode(BlendMode, BlendVal);
		for (i = 0; i <= 3; i++) {
			DrawGraph(laneCordinateX[i], judge_area, H_JUDGE_AREA, TRUE);//判定枠の表示
		}


		//printfDx("jnn:%d", j_n_n[2]);
		//printfDx("jdnn:%d", j_dn_n[2]);
		for (i = 0; i <= 3; i++) {//ノートを叩いた後に一瞬表示するノート
			for (j = 0; j <= NOTE_HIT_LARGE_FLASH_NUMBER - 1; j++) {
				if (note_hit_flash[j][i] >= 7) {
					DrawGraph(laneCordinateX[i], judge_area, H_NOTE[(int)hit_n[i].color_init], TRUE);
					break;
				}
			}
			if (dnote_hit_flash[i] >= 7) {//黒
				DrawGraph(laneCordinateX[i], judge_area, H_NOTE[8], TRUE);
			}
		}

		BlendMode = DX_BLENDMODE_ALPHA;
		BlendVal = 255;
		if (option->op.barline == OptionItem::Barline::ON) {//小節線を表示するオプションなら
			for (i = MusicSub.totalMeasures[difficulty] - 1; i >= 0; i--) {//小節線の描画
				if (barline[i].fall == 1 && barline[i].use == 1) {
					SetDrawBlendMode(BlendMode, BlendVal);
					//if (note[j][i].color_init != 0)DrawGraph(note[j][i].x - 32, note[j][i].y, H_NOTE_OR_FRAME, TRUE);//ORノートの場合
					if (option->op.darkness >= OptionItem::Darkness::DARKNESS_50) {//暗さ50%以上のときだけ白で描画
						DrawLineAA(float(320), float(barline[i].y + 128), float(960), float(barline[i].y + 128), GetColor(255, 255, 255), 2.5);
					}
					else {
						DrawLineAA(float(320), float(barline[i].y + 128), float(960), float(barline[i].y + 128), GetColor(0, 0, 0), 2.5);
					}
					//DrawGraph(barline[i].x, barline[i].y, H_NOTE[7], TRUE);//単ノートの場合
				}
			}
		}

		for (j = 0; j <= 3; j++) {
			for (i = MusicSub.objOfLane[difficulty][j] - 1; i >= 0; i--) {//ノーツの描画


				if (note[j][i].fall == NoteFall::Faling) {
					if (note[j][i].group == NoteGroup::Single) {
						SetDrawBlendMode(BlendMode, BlendVal);

						DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[(int)note[j][i].color], TRUE);//単ノートの場合



						if (note[j][i].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
							DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[10], TRUE);//白く光らせる
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
							DrawGraph(note[j][i].x, note[j][i].y, H_NOTE[10], TRUE);//常に白くする
						}

					}

					if (note[j][i].group == NoteGroup::LongNoteStart) {//始点について
						int endInd = i;
						try {
							endInd = noteSearcher.searchLnEnd(j, i);
						}
						catch(exception){

						}

						//各LNノードは自身と上のノーツまでの中間部分を描画
						if (note[j][i].y >= note[j][endInd].y) {//始点が終点より下にある場合
							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);
							if (note[j][i + 1].LN_k == 1 && option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)NoteColor::K], TRUE);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
							}
							else if (option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)note[j][i + 1].color_init], TRUE);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
							}
							else {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[(int)note[j][i].color_init], TRUE);
							}

							if (LN_flash[j] == 1 && j_n_n[j] - 1 == i) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);

								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);//白く光らせる
							}

							if (note[j][i].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							SetDrawBlendMode(BlendMode, BlendVal);
							DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[(int)note[j][i].color_init], TRUE, FALSE);//LN始点の場合(始点下半分と中間を表示)
							if (LN_flash[j] == 1 && j_n_n[j] - 1 == i && note[j][i].group == NoteGroup::LongNoteStart) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//LN始点の場合(始点下半分を表示)
							}

							if (note[j][i].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//白く光らせる
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);//常に白くする

							}
						}
						else {////始点が終点より上にある場合
							SetDrawBlendMode(BlendMode, BlendVal);

							DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[(int)note[j][i].color_init], TRUE, FALSE);//通常終端

							if (note[j][i].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
							}
							

						}
					}
					if (note[j][i].group == NoteGroup::LongNoteMiddle) {//中間点ノーツについて

						if (note[j][i].y >= note[j][i + 1].y) {//次が自身より上にある場合
							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);

							if (note[j][i + 1].LN_k == 1 && option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)NoteColor::K], TRUE);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
							}
							else if (option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)note[j][i + 1].color_init], TRUE);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
							}
							else {
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[(int)note[j][i].color_init], TRUE);
							}


							if (LN_flash[j] == 1 && noteSearcher.searchLnStart(j, i) == j_n_n[j] - 1) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);//白く光らせる
							}

							if (note[j][i].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i + 1].x, note[j][i + 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}
						}
						else {//次が自身より下にある場合
							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);
							if (option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)note[j][i - 1].color_init], TRUE);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
							}
							else {
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[(int)note[j][i].color_init], TRUE);
							}


							if (LN_flash[j] == 1 && noteSearcher.searchLnStart(j, i) == j_n_n[j] - 1) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							if (note[j][i - 1].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

						}
					}
					if (note[j][i].group == NoteGroup::LongNoteEnd) {//終点について
						int startInd = noteSearcher.searchLnStart(j, i);

						if (note[j][startInd].y >= note[j][i].y) {//始点が終点より下にある場合
							SetDrawBlendMode(BlendMode, BlendVal);

							if (note[j][i].LN_k == 0) {
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[(int)note[j][i].color_init], TRUE, FALSE);//上半分表示
							}
							else if (note[j][i].LN_k == 1) {
								DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[8], TRUE, FALSE);//上半分黒音符表示
							}

							if (note[j][i].LN_k == 0) {//黒終端は光らせない
								if (LN_flash[j] == 1 && noteSearcher.searchLnStart(j, i) == j_n_n[j] - 1) {//LNを叩いているとき
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								}

								if (note[j][i - 1].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
									SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
									DrawRectGraph(note[j][i].x, note[j][i].y, 0, 0, 256, 128, H_NOTE[10], TRUE, FALSE);
								}
							}
						}
						else {//始点が終点より上にある場合
							SetDrawBlendMode(BlendMode, BlendVal);

							if (note[j][i].LN_k == 0) {
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[(int)note[j][i].color_init], TRUE, FALSE);//下半分表示

							}
							else if (note[j][i].LN_k == 1) {
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[8], TRUE, FALSE);//下半分表示
							}


							if (note[j][i].LN_k == 0 && note[j][i - 1].isBright == 1) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawRectGraph(note[j][i].x, note[j][i].y + 128, 0, 128, 256, 128, H_NOTE[10], TRUE, FALSE);
							}

							//LN中間表示
							SetDrawBlendMode(BlendMode, BlendVal);
							if (option->op.blackGradation == OptionItem::BlackGradation::ON && note[j][i].LN_k == 1) {
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)NoteColor::K], TRUE);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)note[j][i - 1].color_init], TRUE);
							}
							else if (option->op.blackGradation == OptionItem::BlackGradation::ON) {
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_OUT[(int)note[j][i].color_init], TRUE);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, gradationLongNote.H_LNOTE_GRAD_FADE_IN[(int)note[j][i - 1].color_init], TRUE);
							}
							else {
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[(int)note[j][i - 1].color_init], TRUE);
							}

							//DrawRectExtendGraph
							//DrawRect
							if (LN_flash[j] == 1 && j_n_n[j] == i && note[j][i].group == NoteGroup::LongNoteEnd) {//LNを叩いているとき
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							if (note[j][i - 1].isBright != 0) {//光るノートは点滅させる(flashに合わせて)
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(flash * FLASH_VALUE));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(FLASH_VALUE_ALWAYS));
								DrawExtendGraph(note[j][i - 1].x, note[j][i - 1].y + 128, note[j][i].x + 256, note[j][i].y + 128, H_LNOTE[10], TRUE);
							}

							
						}
					}

				}
			}
		}



		int sideCoverAlphaRatioGeneral = 195;
		if (isPlayMovie)sideCoverAlphaRatioGeneral = 255;
		double dangerRatio = ((double)1 - (gauge_draw_hosei / 100));//降水確率

		int sideCoverAlpha = int(getAlpha(60 + sideCoverAlphaRatioGeneral * dangerRatio, 0, 0));
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, sideCoverAlpha);
		DrawGraph(0, 0, H_COVER, TRUE);//カバー表示
		DrawGraph(960, 0, H_COVER, TRUE);//右側


		if (!isPlayMovie) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(viewAlphaRatio * flash * FLASH_COVER_VALUE * (1 - (double)option->op.darkness / 5)));
			DrawGraph(0, 0, H_COVER_FLASH, TRUE);//カバーフラッシュ表示
			DrawGraph(960, 0, H_COVER_FLASH, TRUE);//右側
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);




		for (i = 0; i < CRTBuf; i++) {
			if (flash > 0) {//フラッシュを小さく
				flash -= 0.0072*(cbpm / 120);
			}
			if (flash <= 0) {
				flash = 0;
			}
		}

		//ゲージ描画
		int gaugeAlphaGeneral = getAlpha(255, 255 * dangerRatio, 255 * dangerRatio);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * gaugeAlphaGeneral);
		
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
		int controllerAlphaGeneral = getAlpha(255, 128, 0);
		int controllerBrightAlphaGeneral = getAlpha(230, 115, 0);

		int buttonAlpha = int((double)viewAlphaRatio * draw_alpha * controllerAlphaGeneral);
		int buttonBrightAlpha = int((double)viewAlphaRatio * draw_alpha * controllerBrightAlphaGeneral);
		for (j = 0; j <= 3; j++) {//B
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonAlpha);
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_B, TRUE);
			if (Key[Button[0][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonBrightAlpha);
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[0][j], 2)), 620 + 0 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 0; j <= 3; j++) {//G
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonAlpha);
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_G, TRUE);
			if (Key[Button[1][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonBrightAlpha);
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[1][j], 2)), 620 + 1 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}
		for (j = 0; j <= 3; j++) {//R
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonAlpha);
			DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_R, TRUE);
			if (Key[Button[2][j]] >= 1) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, buttonBrightAlpha);
				DrawGraph(int(96 + j * 32 + 1500 * pow(key_draw_counter[2][j], 2)), 620 + 2 * 32, H_BUTTON_PRESS, TRUE);//押してたら光らせる
			}
		}

        //時間描画
		int timeAlphaGeneral = getAlpha(255, 128, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * timeAlphaGeneral);

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

		//R描画
		int rAlphaGeneral = getAlpha(255, 128, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * rAlphaGeneral);
		if (option->op.color == OptionItem::Color::RAINBOW) {//虹オプションのときR表示
			DrawGraph(960, -3, H_R, TRUE);
		}

		//スコア描画
		int scoreAlphaGeneral = getAlpha(255, 128, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * scoreAlphaGeneral);

		if (option->op.color != OptionItem::Color::RAINBOW) {
			for (i = 0; i <= 4; i++) {
				DrawExtendGraph(1170 - i * 40, int(0 - (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), 1170 + 64 - i * 40, int(0 + 100 + (10 - cos(3.14 / 2 * combo_draw_counter) * 10)), H_SCORE_NUMBER[score_digit[i]], TRUE);
			}
		}

		if (option->op.color == OptionItem::Color::RAINBOW) {
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
		int simbolAlphaGeneral = viewAlphaRatio * getAlpha(255, 0, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, simbolAlphaGeneral);
		DrawGraph(1020, 260, H_DIFFICULTY, TRUE);

		//スコアグラフ描画
		int boxLineAlphaGeneral = viewAlphaRatio * getAlpha(255, 255, 0);
		int scoreGraphAlphaGeneral = viewAlphaRatio * getAlpha(160, 60, 0, 160);
		int judgeBoxAlphaGeneral = viewAlphaRatio * getAlpha(80, 60, 0, 80);

		if (option->op.scoreGraph != OptionItem::ScoreGraph::OFF) {
			DrawBoxWithLine(960, 482, 960 + 80, 482 + 40, GetColor(50, 50, 255), scoreGraphAlphaGeneral, boxLineAlphaGeneral);//現在のスコア
			DrawBoxWithLine(960 + 80, 482, 960 + 80 + 80, 482 + 40, GetColor(50, 255, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);//ハイスコア
			DrawBoxWithLine(960 + 160, 482, 960 + 80 + 160, 482 + 40, GetColor(255, 50, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);//ターゲットスコア
			DrawBoxWithLine(960 + 240, 482, 960 + 80 + 240, 482 + 40, GetColor(200, 200, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);//前回のスコア

			//スコアグラフ描画
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, scoreGraphAlphaGeneral);

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
				DrawBoxWithLine(960, int(482 - 0.04 * score), 960 + 80, 482, GetColor(50, 50, 255), scoreGraphAlphaGeneral, boxLineAlphaGeneral);
				DrawBoxWithLine(960 + 80, 482 - 0.04 * highScore.score * noteCountRatio, 960 + 80 + 80, 482, GetColor(50, 255, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);
				DrawBoxWithLine(960 + 160, 482 - 0.04 * targetScore * noteCountRatio, 960 + 80 + 160, 482, GetColor(255, 50, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);
				DrawBoxWithLine(960 + 240, 482 - 0.04 * targetScore2 * noteCountRatio, 960 + 80 + 240, 482, GetColor(255, 255, 50), scoreGraphAlphaGeneral, boxLineAlphaGeneral);
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, judgeBoxAlphaGeneral);


		//判定数表示欄
		DrawBoxWithLine(976, 540, 1110, 580, GetColor(50, 255, 255), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//SKY_PERFECT
		DrawBoxWithLine(976, 580, 1110, 620, GetColor(255, 255, 50), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//PERFECT
		DrawBoxWithLine(976, 620, 1110, 660, GetColor(255, 50, 50), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//GOOD
		DrawBoxWithLine(976, 660, 1110, 700, GetColor(50, 50, 255), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//MISS

		//BPM描画欄
		DrawBoxWithLine(1130, 540, 1264, 580, GetColor(255, 50, 50), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//MAXBPM
		DrawBoxWithLine(1130, 580, 1264, 620, GetColor(255, 50, 255), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//BPM
		DrawBoxWithLine(1130, 620, 1264, 660, GetColor(50, 50, 255), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//MINBPM
		DrawBoxWithLine(1130, 660, 1264, 700, GetColor(25, 255, 25), judgeBoxAlphaGeneral, boxLineAlphaGeneral);//SPEED
		
		//判定、BPM文字表示
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * boxLineAlphaGeneral);
		DrawGraph(0, 0, H_GAME_STR_JUDGE_BPM, TRUE);

		//SCORE GRAPHがOFF以外の時グラフ文字表示
		if (option->op.scoreGraph != OptionItem::ScoreGraph::OFF) {
			DrawGraph(0, 0, H_GAME_STR_SCORE_GRAPH, TRUE);
		}

		//数値表示
		int judgeNumberAlphaGeneral = getAlpha(255, 192, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, viewAlphaRatio * judgeNumberAlphaGeneral);
		DrawNumber(1094, 536, SKY_PERFECT, 25, 1, 0, H_SMALL_NUMBER_CYAN);
		DrawNumber(1094, 576, PERFECT, 25, 1, 0, H_SMALL_NUMBER_YELLOW);
		DrawNumber(1094, 616, GOOD, 25, 1, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1094, 656, MISS, 25, 1, 0, H_SMALL_NUMBER_BLUE);

		DrawNumber(1216, 536, Music[song_number].bpmmax[difficulty], 25, 0, 0, H_SMALL_NUMBER_RED);
		DrawNumber(1216, 576, int(cbpm + 0.5), 25, 0, 0, H_SMALL_NUMBER_MAGENTA);
		DrawNumber(1216, 616, Music[song_number].bpmmin[difficulty], 25, 0, 0, H_SMALL_NUMBER_BLUE);


		//HSを考慮したSPEEDを算出
		//cbpm
		int bpmBuf = int(speedBuffer.getAverage() * option->op.speedVal * coverView.getSpeedRatio() + 0.5);
		if (bpmBuf >= 10000) {
			bpmBuf = 9999;
		}

		DrawNumber(1216, 656, bpmBuf, 25, 0, 0, H_SMALL_NUMBER_GREEN);



		if (isStartCoverMoveUpComplete == 0 && ClearFlag == 0) {//スタート時の中心カバー上げ
			//DrawGraph(320, int((cos((3.14 / 2) * c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);//中心カバー
			coverView.draw(DX_SCREEN_BACK);
			if (gauge_draw_counter >= gauge - 0.001) {//曲名の透過度
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(((double)1 - sin((3.14 / 2) * c_m_draw_counter)) * 255));
			}
			SetFontSize(30);
			SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
			DrawGraph(320, 350, H_TITLE_STR, TRUE);
			DrawGraph(320, 450, H_ARTIST_STR, TRUE);
			DrawGraph(320, 260, H_GENRE_STR, TRUE);
			//ShowExtendedStrFitToHandle(640, 350, Music[song_number].title[difficulty], title_width, 620, config, FontHandleCommon, Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//曲名
			//ShowExtendedStrFitToHandle(640, 450, Music[song_number].artist[difficulty], artist_width, 620, config, FontHandleCommon);//アーティスト
			//ShowExtendedStrFitToHandle(640, 260, Music[song_number].genre[difficulty], genre_width, 620, config, FontHandleCommon);//ジャンル
			//ShowExtendedStrFitToHandleNoShadow(640, 500, Music[song_number].detail[difficulty], detail_width, 630, config, FontHandleDetail);//詳細
			detailView.draw();
			SetDrawMode(DX_DRAWMODE_NEAREST);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

			for (i = 0; i < CRTBuf; i++) {
				if ((c_m_draw_counter < 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {
					c_m_draw_counter += 0.0012;
				}
				if ((c_m_draw_counter > 1) && (gauge_draw_counter >= gauge - 0.001) && (draw_alpha == 1)) {//カバーが上がり切った
					c_m_draw_counter = 1;
					isStartCoverMoveUpComplete = 1;

					coverView.startWindbreak();
				}
			}
		}
		else if (ClearFlag != 0) {//演奏終了していたら中心カバーの表示
			//DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			coverView.draw(DX_SCREEN_BACK);
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
		else {//演奏中のカバー位置
			//DrawGraph(320, int(pow(((double)controllerVolume/255), 2) * 720 - 720), H_COVER_MIDDLE, TRUE);
			coverView.draw(DX_SCREEN_BACK);
		}


		int ComboBuf = combo;
		if (SkillTestFlag != 0)ComboBuf = *CourseCombo;//コースモードの時はコース全体のコンボで表示


		int comboYScale = 10 - cos(3.14 / 2 * combo_draw_counter) * 10;
		int comboNumberX = 485; int comboNumberY = 360; int comboNumberHeight = 100; int comboNumberWidth = 256;
		int comboNumberTop = comboNumberY - (comboNumberHeight / 2);
		int comboNumberBottom = comboNumberY + (comboNumberHeight / 2);

		int comboX = 600;
		int comboWidth = 64;
		int comboDuration = 40;
		int comboDigit = int(log10(ComboBuf));
		int comboXOffset = comboDigit * (comboDuration / 2);
		int comboAlphaGeneral = getAlpha(255, 128, 0);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, comboAlphaGeneral);
		if (combo_draw_counter > 0 && ComboBuf > 0) {//コンボ描画
			DrawExtendGraph(comboX,          int(comboNumberTop - comboYScale), comboX + comboNumberWidth,         int(comboNumberBottom + comboYScale), H_COMBO, TRUE);
			for (i = 0; i <= comboDigit; i++) {
				DrawExtendGraph(
					comboNumberX              - i * comboDuration + comboXOffset,
					int(comboNumberTop    - comboYScale), 
					comboNumberX + comboWidth - i * comboDuration + comboXOffset,
					int(comboNumberBottom + comboYScale), 
					H_COMBO_NUMBER[combo_digit[i]], TRUE);
			}
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		for (i = 0; i <= 3; i++) {//ノートを叩いた時のフラッシュ、判定表示
			for (j = 0; j <= NOTE_HIT_LARGE_FLASH_NUMBER - 1; j++) {//4個までなら同時に表示
				if (note_hit_flash[j][i] >= 0)DrawGraph(laneCordinateX[i], judge_area, H_HIT[11 - (int)note_hit_flash[j][i]], TRUE);//ノートを叩いた時のフラッシュ
				if (note_hit_large_flash[j][i] >= 0)DrawGraph(laneCordinateX[i] - 22, judge_area - 22, H_HIT_LARGE[19 - (int)note_hit_large_flash[j][i]], TRUE);//光るノートを叩いた時のフラッシュ
			}




			if (dnote_hit_flash[i] >= 0)DrawGraph(laneCordinateX[i], judge_area, H_HIT_D[11 - (int)dnote_hit_flash[i]], TRUE);//黒ノートを叩いた時のフラッシュ
			if (hit_sky_perfect[i] > 0)DrawGraph(laneCordinateX[i], int(-10 * cos(3.14 * hit_sky_perfect[i] / 2) + 10 + judge_area), H_SKY_PERFECT, TRUE);
			if (hit_perfect[i] > 0)DrawGraph(laneCordinateX[i], int(-10 * cos(3.14 * hit_perfect[i] / 2) + 10 + judge_area), H_PERFECT, TRUE);
			if (hit_good[i] > 0)DrawGraph(laneCordinateX[i], int(-10 * cos(3.14 * hit_good[i] / 2) + 10 + judge_area), H_GOOD, TRUE);
			if (hit_miss[i] > 0)DrawGraph(laneCordinateX[i], int(-10 * cos(3.14 * hit_miss[i] / 2) + 10 + judge_area), H_MISS, TRUE);

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

			I_Slow[i].draw(DX_SCREEN_BACK);
			I_Fast[i].draw(DX_SCREEN_BACK);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		}


		if(note[0][j_n_n[0]].color == NoteColor::NONE &&
			note[1][j_n_n[1]].color == NoteColor::NONE &&
			note[2][j_n_n[2]].color == NoteColor::NONE &&
			note[3][j_n_n[3]].color == NoteColor::NONE &&
			note[0][j_dn_n[0]].color == NoteColor::NONE &&
			note[1][j_dn_n[1]].color == NoteColor::NONE &&
			note[2][j_dn_n[2]].color == NoteColor::NONE &&
			note[3][j_dn_n[3]].color == NoteColor::NONE && MISS == 0){
			
			if (FullComboFXPlayFlag == 1) {
				FullComboFXBaseTime = int(GAME_passed_time + 0.5);
			}
			int PFC = (MISS == 0) && (GOOD == 0);
			DrawFullComboRainbow(&FullComboFXPlayFlag, &FullComboFXFrame, int(GAME_passed_time), FullComboFXBaseTime, FXH_FULLCOMBO, &FXPH_FULLCOMBO, PFC);//フルコンボエフェクトの再生

		}


		int clearMarkAlpha = viewAlphaRatio * int(((double)1 - str_draw_counter) * 255);
		if (ClearFlag != 0 && str_draw_counter != -1 && c_m_draw_counter == 0) {
			if (ClearFlag == 1 && MISS == 0) {//ミスなし

				if (GOOD == 0) {//PERFECT FULLCOMBO
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, clearMarkAlpha);//PERFECT_FULLCOMBO表示
					DrawGraph(320, 310, H_PFC, TRUE);
				}
				else {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, clearMarkAlpha);//FULL_COMBO表示
					DrawGraph(320, 310, H_FULL_COMBO[(int(GAME_passed_time) / 50) % 6], TRUE);
				}
			}

			if (ClearFlag == 1 && MISS != 0 && option->op.gauge != OptionItem::Gauge::NO_FAIL) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, clearMarkAlpha);//CLEARED表示
				DrawGraph(320, 310, H_CLEARED, TRUE);
			}

			if (ClearFlag == 2) {
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, clearMarkAlpha);//FAILED表示
				DrawGraph(320, 310, H_FAILED, TRUE);
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

			printfDx(L"AI 難易度予測:%.2f%%\n", autoDifficultyPredictionResult);
			printfDx(L"旧難易度予測:%d%%\n", Cdiff.level);
			printfDx(L"旧局所難易度予測:%d%%\n", Cdiff.level_local);

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
		soundHitSound();

		double BGM_VolTowardResult = 1;
		songVolume.process();
		if (ClearFlag != 0) {//クリア表示4秒後にゲームの終了(結果発表へ)

			BGM_VolTowardResult = 1 - (double)(GAME_passed_time - cleared_time) / (double)TimeFromEndOfGameToResult;
		}
		if (!CheckHandleASyncLoad(SH_SONG)) {
			int maxVol = 255 * (double)option->op.bgmSoundVol / (int)OptionItem::BgmSoundVol::Vol_100;
			double missVolEasing = ((double)1 - cos(volume * (3.1415 / 2)));//0~1
			int vol = int(maxVol * missVolEasing * BGM_VolTowardResult * debug_music * ((double)songVolume.value/255));

			ChangeVolumeSoundMem(vol, SH_SONG);//曲の音量セット
		}

		jingleVolume.process();
		ChangeVolumeSoundMem(jingleVolume.value, SH_JINGLE);//ジングル音の音量セット

		//printfDx("%d\n", int(((double)44100 * (GAME_passed_time - Music[song_number].songoffset[difficulty])) / 1000));

		isOverSongPlayTiming = (GAME_passed_time + LOOP_passed_time > Music[song_number].songoffset[difficulty]);
		if (isOverSongPlayTiming
			&& (CheckSoundMem(SH_SONG) == 0 && debug_stop == 0)
			&& (ClearFlag == 0) 
			&& (bgmplay == 0)) {
			// 1/60sの後ろずれを防ぐためフレーム時間分前倒しで再生
			if (FirstBgmPlay == 0) {//最初のBGM再生でないときのみ再生位置変更処理を行う(先頭が削れて再生されるのを防ぐため)
				SetCurrentPositionSoundMem(int(((double)44100 * pitch * ((GAME_passed_time + TimePerFrame) - Music[song_number].songoffset[difficulty])) / 1000), SH_SONG);
			}

			PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);//曲の再生開始

			bgmplay = 1;
			FirstBgmPlay = 0;
		}
		if (!isOverSongPlayTiming){
			if (FirstBgmPlay == 0 && CheckSoundMem(SH_SONG) == 0) {
				FirstBgmPlay = 1;
				SetCurrentPositionSoundMem(0, SH_SONG);
			}
		}

		isOverMoviePlayTiming = (GAME_passed_time + LOOP_passed_time + 0.001*movieFrameTime > Music[song_number].movieoffset[difficulty]);
		if (isOverMoviePlayTiming
			&& (debug_stop == 0)
			&& (ClearFlag == 0)
			&& (isMoviePlaying == 0)) {
			// 後ろずれを防ぐためフレーム時間 + 動画ファイルの1フレーム時間 分前倒しで再生
			if (FirstMoviePlay == 0) {//最初のBGM再生でないときのみ再生位置変更処理を行う(先頭が削れて再生されるのを防ぐため)
				updateSeekTime(int((double)pitch * ((GAME_passed_time + TimePerFrame) - Music[song_number].movieoffset[difficulty])));
			}
			PlayMovieToGraph(MovieGraphHandle);//動画の再生開始
			isMoviePlaying = 1;
			FirstMoviePlay = 0;
		}
		if (!isOverMoviePlayTiming) {
			if (FirstMoviePlay == 0) {
				FirstMoviePlay = 1;
				if (*debug == 1)updateSeekTime(0);
			}
		}



		//printfDx("%f\n",LOOP_passed_time);
		
		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000/config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		ScreenFlip();//画面の反映
		clsDx();
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



//HSを考慮したNOW SPD計算のためのクラス
//これから叩く音符4つの速さの平均を取得する
SpeedBuffer::SpeedBuffer(NOTE** note, int* j_n_n) {//初期値
	SpeedBuffer::note = note;
	SpeedBuffer::noteIndex = j_n_n;
}

void SpeedBuffer::updateAverage() {
	int index[4] = { noteIndex[0],noteIndex[1], noteIndex[2], noteIndex[3] };//探索用インデックス


	float averageBuf = 0;

	int averageCount = 0;
	for (averageCount = 0; averageCount < SIZE; averageCount++) {
		//4列からタイミングが一番近いものを探す
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

		if (isFoundFirst) {//残り音符が3つ以下の時は無くなった時点で抜けだす
			if (averageCount == 0)return;
			break;
		}

		//タイミングが同じものがあれば速さを平均する
		//そのレーンのnoteIndexをインクリメント
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
		//速さ算出
		speedSum /= count;

		averageBuf += speedSum;
	}

	average = averageBuf / averageCount;
	return;
}

float SpeedBuffer::getAverage() {
	return average;
}

