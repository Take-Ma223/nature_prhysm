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
		int* noteIndex;//各レーン毎のインデックス

		float average = 0;

		static const int SIZE = 1;//先を見る個数

	public:
		SpeedBuffer(NOTE** note, int* j_n_n) {
			SpeedBuffer::note = note;
			SpeedBuffer::noteIndex = j_n_n;
		}

		void updateAverage() {
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
			int secret,//隠し曲演出中か(0:通常 1:演出中)
			ANDROID_CONTROLLER* AC,
			Config config,
			IR_SETTING* ir,
			int SkillTestFlag,//段位認定モードか(0:通常 1~4:段位認定モード ステージ数を示す)
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
		/// 初期化処理
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
		/// 音符画像などの正方形画像を指定座標を中心に[scale]の倍率で描画
		/// </summary>
		/// <param name="x">x座標</param>
		/// <param name="y">y座標</param>
		/// <param name="handle">イメージハンドル</param>
		/// <param name="TransFlag">透過フラグ</param>
		void drawSquareImage(double x, double y, int handle, int TransFlag);

		void drawNotes(SongSub& MusicSub, int difficulty, NOTE** note, int BlendMode, int BlendVal, int  H_NOTE[12], double flash, const int FLASH_VALUE, const int FLASH_VALUE_ALWAYS, NoteSearcher& noteSearcher, Option* option, GradationNoteImage& gradationLongNote, int  H_LNOTE[12], int  LN_flash[4], int  j_n_n[4], const int FLASH_VALUE_LN_PUSH, NoteSymbolImage& noteSymbol);

		//フルコンボエフェクト
		void drawFullComboRainbow(int* play, int* step, int Time, int baseTime, int effectResourceHandle, int* playingEffectHandle, int PFC);

		//透過度の取得 Movieを再生するかによって変わります
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
		int secret;//隠し曲演出中か(0:通常 1:演出中)
		ANDROID_CONTROLLER* AC;
		Config config;
		IR_SETTING* ir;
		int SkillTestFlag;//段位認定モードか(0:通常 1~4:段位認定モード ステージ数を示す)
		double* GaugeVal;
		int* CourseCombo;
		int* CourseMaxCombo;
		int AllowExit;

		enum GameScreenPhase {
			Ready,//演奏開始前
			Playing//演奏中
		};

		GameScreenPhase game_screen_phase = Ready;

		int stop_play_key = KEY_INPUT_SPACE;//再生停止に使用するキー


		Asset asset;//使う画像セット
		//コンテキスト
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
		double GAME_passed_time;//単位はms 判定と音用の経過時間
		double GAME_passed_time_for_draw;//譜面描画用の経過時間(ディスプレイ遅延補正用)

		double LOOP_passed_time = 1;//1ループにかかった時間(ms)
		double CounterRemainTime = 0;////カウンターの値を1msずつ変動するための時間　1msずつ引かれて小数以下は蓄積する
		double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数
		double TimePerFrame = 1000.0 / config.Fps;//1フレームの時間


		int H_NOTE[12];//音符画像(0は無しで1~9でRGBYCMWKF 10:点滅用 11はLNを叩いた時に光らせるレイヤー用 )
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
		int H_SKY_PERFECT, H_PERFECT, H_GOOD, H_MISS;//SKY_PERFECT,PERFECT,GOOD,MISS判定画像の画像ハンドル
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

		DrawableInitParam retryViewParam = DrawableInitParam(Cordinate(320, 600));
		RetryView retryView = RetryView(&context, retryViewParam);
		
		TransValue viewAlpha = TransValue(&context);

		std::function<void(void)> playAnimationOnEscAtStart;

		std::function<void(void)> playAnimationOnEscAtPlay;

		std::function<void(void)> playAnimationOnFinish;


		int FXH_FULLCOMBO = LoadEffekseerEffect(L"img/FullComboFX/FullComboFX.efk", 25.0f);//effekseerエフェクトリソースハンドル
		// 再生中のエフェクトのハンドルを初期化する。
		int FXPH_FULLCOMBO = -1;
		int FullComboFXPlayFlag = 1;//フルコンボエフェクト再生フラグ
		int FullComboFXBaseTime = 0;
		int FullComboFXFrame = 0;

		//画像として保存された曲名の画像ハンドル
		int H_TITLE_STR = MakeScreen(640, 48, TRUE);

		//画像として保存されたジャンルの画像ハンドル
		int H_GENRE_STR = MakeScreen(640, 48, TRUE);

		//画像として保存されたアーティストの画像ハンドル
		int H_ARTIST_STR = MakeScreen(640, 48, TRUE);

		int screenHandle;

		DrawableInitParam high_score_diff_init_param = DrawableInitParam(Cordinate(960, 450), CenterRatio(0, 0));
		ScoreGraphDiffView high_score_diff = ScoreGraphDiffView(&context, high_score_diff_init_param);

		GAME_SH SH;
		int AllowSound[3][4] = { { 1,1,1,1 },{ 1,1,1,1 },{ 1,1,1,1 } };//そのフレームでコントローラを叩いた時の音を出すか コントローラの並び順と同じ(0:B 1:G 2:R)
		double HitAreaBright[3][4] = { {0,0,0,0},{0,0,0,0},{0,0,0,0} };//コントローラを叩いた時の判定枠の各色の光具合

		int SH_SONG;//曲データのハンドル
		TransValue songVolume = TransValue(&context);


		int	SH_CLOSE;
		int	SH_CLOSED;
		int	SH_OPEN;
		int SH_JINGLE;

		TransValue jingleVolume = TransValue(&context);
		int SH_SHUTTER;
		int SH_SHUTTER_SIGNAL;


		int i = 0, j = 0, k = 0;
		double cbpm = 120;//現在のBPM
		double speed = 0.00000000001;//スピード調整
		double high_speed = 1;//ハイスピ
		double width_scale = 1;//レーンの横の広がり幅の倍率
		int laneCordinateX[4] = {0,0,0,0};

		int cash = 0;//式の見た目を簡単にするため
		int judge_area = 618, note_fall = -64;//ノートの判定エリアと落ち始める高さ-192~490

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
		double note_hit_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { { -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 } };//ノートを叩いた時のフラッシュを表示するためのカウンタ
		int note_hit_flash_rounder = 0;//4個以内で順々にフラッシュを表示するためのカウンタ(0~3で回る)
		double note_hit_large_flash[NOTE_HIT_LARGE_FLASH_NUMBER][4] = { { -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 },{ -1,-1,-1,-1 } };//光るノートを叩いた時のフラッシュを表示するためのカウンタ
		int note_hit_large_flash_rounder = 0;//4個以内で順々にフラッシュを表示するためのカウンタ(0~3で回る)
		double dnote_hit_flash[4] = { -1,-1,-1,-1 };//黒ノートを叩いた時のフラッシュを表示するためのカウンタ
		const int FLASH_VALUE = 85;//長音ノートの点滅輝度
		const int FLASH_VALUE_ALWAYS = 170;//長音ノートの常時点滅輝度
		const int FLASH_COVER_VALUE = 15;//カバーの点滅輝度
		const int FLASH_VALUE_LN_PUSH = 50;

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
		double score_dec = int(score_MAX * 0.001);//ミス時のスコア減少量(理論値の0.1%分(10点)スコア減少)
		int combo_digit[5] = { 0,0,0,0,0 };//コンボ数を桁ごとに格納
		int score_digit[5] = { 0,0,0,0,0 };//スコア
		int time_minutes_digit[2] = { 0,0 };//分
		int time_hours_digit[2] = { 0,0 };//時間
		int bpm_digit[5] = { 0,0,0,0,0 };//BPM
		double gauge = 100;


		double gauge_draw_counter = gauge;//ゲージを滑らかに表示するためのカウンター(仲介)
		double gauge_draw_hosei = gauge;//ゲージを段々緩やかに減少させて表示するためのカウンター(xtosinx()を使う)
		int pop_digit[3] = { 0,0,0 };//ゲージ量表示用桁
		int pop = int(100 - gauge_draw_hosei);//降水量
		int HitingNoteCount = 0;//黒を除いて何個の音符の判定が終了したかを表す数値

		double key_draw_counter[3][4];//0になればコントローラ画像が位置につく

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
		int TimeAcc[6] = { 0,0,0,0,0,0 };//3時間毎の精度を保存
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

		const int ESC_PRESS_TIME = 750;//ESCキー長押し時間
		int esc_press_counter = 0;

		EscapingView escaping_view = EscapingView(&context);

		const double ROOT12_2 = 1.0594630943592952645618252949463;
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

		int bcc = 0;//bpmchangeのカウンタ
		int scc = 0;//scrollchangeのカウンタ
		double sc_timing = 0;//前にスクロール変化があった時間
		double cscroll = 1;//現在の実時間倍率(譜面全体のスピード倍率)
		double GAME_passed_time_scroll = 0;//譜面としての経過時間(scrollを考えた分)
		double real_timing = 0;//前にscrollが変化したときのGAME_passed_timeの時間

		BPMC* bpmchange;//BPM変更
		SC* scrollchange;//SCROLL変更
		STOP_SE* stopSequence;//譜面停止

		int stop_se_c = 0;//stopSequenceのカウンタ
		double stop_time = 0;//譜面停止するための時間
		double stop_time_sum = 0;//今までの総譜面停止時間
		int stopFlag = 0;//譜面停止中どうかのフラグ(1:停止中)

		NOTE** note;

		NoteSearcher noteSearcher = NoteSearcher(note);

		//小節線構造体
		BARLINE* barline;

		int blc = 0;//barline counter

		NOTE hit_n[4];//最後に叩いたノートの情報

		NoteJudgeProcessor noteJudgeProcessor;

		double fall_y = 0;

		//叩くと後ろに落ちるもの関連
		CELL head;//リストの先頭
		CELL* insert;//リストに挿入する場所を示すポインタ(このポインタの挿すセルの後ろに挿入)

		//EDIT_SCORE関数に渡す用
		double scale_score = 1;//拡大縮小率
		int step_array_number = 0;//ステップ増減値配列の添え字
		int measure_start_step = 0;//小節線を表示する起点
		int SwingBackBeatTime = SWING_DIVISION / 2;//

		SongSub MusicSub;
		SpeedBuffer speedBuffer = SpeedBuffer(note, j_n_n);

		DATEDATA DateBuf;
		int rand_seed;

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

		//画像音ハンドル値代入
		wchar_t* ReadNameRGB[11] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"k",L"f",L"bright",L"longPush" };
		wstring note_folder = option->op.note.toString();

		GradationNoteImage gradationLongNote = GradationNoteImage(wstring(L"img/notes/") + wstring(note_folder.c_str()));
		NoteSymbolImage noteSymbol = NoteSymbolImage(config, option->op.noteSymbol.toString());

		int Keylist[3][4];//キーのリスト

		XY pos_cloud;//雲の座標

		wstring themeStr1 = wstring(L"img/themes/");
		wstring themeStr2 = wstring(option->op.theme.toString());

		//中央カバー
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
		LONGLONG movieFrameTime = 0;//動画ファイルの1フレームの時間(us)
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

		double draw_alpha_speed = 1;//段位用Speed表示フラグ(これが0になるまでは開始しない)

		int targetScore = 0;
		int targetScore2 = 0;
		bool isSkip = false;


		vector<unique_ptr<AuroraView>> aurora;
		bool update_aurora_color[4] = { true, true, true, true };


	};

}