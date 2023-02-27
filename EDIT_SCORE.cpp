#include"DxLib.h"
#include"EDIT_SCORE.h"
#include"STRUCT_EDIT_SCORE.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"score_cell_operation.h"
#include"GAME_LOAD.h"
#include"Get_Key_State.h"
#include"STRUCT_GAME_SH.h"
#include <stdio.h>
#include <time.h>
#include"GetNowCount_d.h"
#include"ShowFps.h"
#include"ScreenShot.h"
#include<string>
#include "Asset.h"
#include "NotesModeView.h"
#include "AppContext.h"

using namespace std;

void EDIT_SCORE(SCORE_CELL* head,
	Song* Music,
	SongSub* MusicSub,
	Option *option,
	int song_number,
	int difficulty,
	double init_scroll,
	int Button[3][4],
	int Button_Shutter,
	int *Key,
	char *Buf,
	double passed_time,//GAMEからEDIT_SCOREを呼び出したときの向こう(GAME)のGAME_passed_time
	double *debug_time_passed,
	double *scale_score,//拡大縮小率
	int *step_array_number,//ステップ増減値配列の添え字
    int *measure_start_step,//小節線を表示する起点
	int *SwingBackBeatTime,//裏拍のSwingDivisionに対する位置
	ANDROID_CONTROLLER *AC,
	Config config,
	int SH_SONG
) {
	//headのstepは-1
	
	double GAME_start_time;
	double GAME_passed_time;//単位はms
	double LOOP_passed_time = 1;//1ループにかかった時間(ms)
	double CounterRemainTime = 0;
	double time_cash = 0;//LOOP_passed_timeを算出するための記憶変数

	Asset asset;//使う画像セット

	//ビューコンテキスト
	AppContext appContext = AppContext(NULL, option, &config);
	ActivityContext context = ActivityContext(&appContext, &asset);

	int H_NOTE[12];//音符画像(0は無しで1~9でRGBYCMWKF 10はLNを叩いた時に光らせるレイヤー用 光るノート用)
				   //int H_NOTE_OR_FRAME;//ORノートの枠

	int H_LNOTE[12];//LNの中間用画像ハンドル(0は無しで1~9でRGBYCMWK(無し)F)
					//int H_LNOTE_OR_FRAME;//LNでORノートの枠

	int H_JUDGE_AREA, H_BG;//画像データのハンドル

	GAME_SH SH;
	int SH_STEP_CHANGE;
	int SH_STEP_INC_DEC;
	int SH_SAVE;
	int SH_NO;
	int SH_SHUTTER;
	int SH_SHUTTER_SIGNAL;

	int i = 0, j = 0;
	
	int lane[4] = {
		int(640 - 120),//レーンのx座標
		int(640 - 40),
		int(640 + 40),
		int(640 + 120) };
	int cash = 0;//式の見た目を簡単にするため
	double cash2 = 0;//式の見た目を簡単にする仲介2
	int cash3 = 0;//式の見た目を簡単にする仲介3
	int judge_area = 580, note_fall = -192;//ノートの判定エリアと落ち始める高さ-192~490

	double scale = 0.650 / EDITSTEP_SCALE_RATIO;//stepからの補正縮小率
	
	double scale_score_draw = *scale_score;//拡大縮小率(表示用)ヌルっと変わる
	int scale_number = 3;
	const int SCALE_ARRAY_NUMBER = 23;
	double scale_array[SCALE_ARRAY_NUMBER] = {0.25,0.5,0.75,1,1.25,1.5,1.75,2,2.5,3,3.5,4,5,6,7,8,10,15,20,25,30,40,50};

	int step_count = 0;//現在のステップ値
	double step_count_draw = 0;//現在のステップ数(表示用)ヌルっと変わる

	int step = EDITSTEP;//矢印キーで移動するステップ数
	
	

	AreaSelect area;//範囲選択構造体
	SCORE_CELL CopyHead;//一時コピー用のヘッダセル
	CopyHead.next = NULL;
	int PasteRange = 0;//現在のstep_countからどの範囲までを消して貼り付けすることになっているか

	const int STEP_INDEX_ARRAY = 27;
	int step_array[STEP_INDEX_ARRAY] = { EDITSTEP,EDITSTEP / 2,EDITSTEP / 4,EDITSTEP / 6,EDITSTEP / 8,EDITSTEP / 12,EDITSTEP / 16,EDITSTEP / 24,EDITSTEP / 32,EDITSTEP / 48,EDITSTEP / 64,EDITSTEP / 96,EDITSTEP / 128,EDITSTEP / 192,EDITSTEP / 256,EDITSTEP / 384,EDITSTEP / 768 ,
	EDITSTEP / 5,EDITSTEP / 10,EDITSTEP / 20,EDITSTEP / 40,EDITSTEP / 80, EDITSTEP / 160, EDITSTEP / 320, EDITSTEP / 640, EDITSTEP / 1280, EDITSTEP / 3840, };
	int step_array_show[STEP_INDEX_ARRAY] = { 1,2,4,6,8,12,16,24,32,48,64,96,128,192,256,384,768, 5,10,20,40,80,160,320,640,1280,3840 };

	int command_put = 0;//今どの命令について数値を変えているか(0:音符を置いている 1~命令の種類)
	int cvb_STOPSTEP[2] = {1,1};//command value buf 命令の値を保存し新しく命令を置いた時この値をデフォルトの値に設定する
	double cvb_STOP = 0;
	double cvb_HS[4] = { 1,1,1,1 };
	double cvb_BPM = Music[song_number].bpm[difficulty];
	double cvb_LETBPM = Music[song_number].bpm[difficulty];
	double cvb_SCROLL = 1;
	double cvb_SCROLL_BPM = Music[song_number].bpm[difficulty];
	int cvb_MEASURE[2] = { 4,4 };

	int push_counter = 0;//ボタン長押しカウンタ
	int push_LR_counter = 0;
	int push_command_val_counter = 0;

	int note_kind = 0;//Ctrlで音符の種類を変えたフラグ
	int AutoMove = 0;//音符を置いた後自動で動くかどうかのフラグ
	int FrameButtonFlag = 0;//前のフレームで音符配置ボタンをどれか一つでも押していたかどうかのフラグ

	BOOL saveErrorTextShowFlag = 0;//保存できなかったときのエラーメッセージを表示するフラグ

	SCORE_CELL *insert;	//insert今の行から下にある一番stepが近いセル(音符or命令 どちらもあるときは音符を指す) または現在挿入中のセルを指すポインタ
	insert = head;//headのsteoは-1

	SCORE_CELL *insert_command = NULL;//今操作している命令セルを指す

	double insert_passed_time = 0;//insertが指している値が実際の時間で何msかを格納

	SCORE_CELL *end = head;//#ENDのセルのアドレスを格納
	while (end->content != CONTENTS_END) {
		end = end->next;
	}

	int line_draw_step = 0;//補助線表示用カウンタ
	SCORE_CELL *show;//表示するとき用ポインタ
	show = head->next;

	BOOL isInstructionAppear = 1;//操作説明表示フラグ

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
	wchar_t strcash[128];
	sprintfDx(strcash, L"img/notes/%s/JudgeArea.png", option->note[option->op.note]);
	H_JUDGE_AREA = LoadGraph(strcash);

	DeleteGraph(H_LNOTE[8]);//黒ロング画像を削除し、透過加工黒ロング画像を作成
	sprintfDx(strcash, L"img/notes/%s/dl.png", option->note[option->op.note]);
	int SOFTH_LNOTE_B = LoadSoftImage(strcash);
	for (int y = 0; y < 256; y++) {//透過グラデーション加工
		for (int x = 0; x < 256; x++) {
			int r = 0, g = 0, b = 0, a = 0;
			GetPixelSoftImage(SOFTH_LNOTE_B, x, y, &r, &g, &b, &a);
			if (a != 0)DrawPixelSoftImage(SOFTH_LNOTE_B, x, y, r, g, b, 255 - y);

		}
	}
	H_LNOTE[8] = CreateGraphFromSoftImage(SOFTH_LNOTE_B);
	DeleteSoftImage(SOFTH_LNOTE_B);

	DrawableInitParam notesModeViewParam;
	notesModeViewParam.cordinate = Cordinate(439, 532);
	NotesModeView notesMode = NotesModeView(&context,notesModeViewParam);
	notesMode.switchToRgb();
														 //H_LNOTE_OR_FRAME = LoadGraph(L"img/note_or_frame_L.png");
	/*
	int H_BPM_NUMBER[10];
	int H_HS_NUMBER[10];
	int H_SCROLL_NUMBER[10];
	int H_STOP_NUMBER[10];
	int H_MEASURE_NUMBER[10];
	*/
	int digit[5] = { 0,0,0,0,0 };

	wchar_t command_show_str[100];


	/*
	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_STOP_NUMBER);

	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_HS_NUMBER);

	LoadDivGraph(L"img/ResultPopNumber.png", 10, 10, 1, 25, 50, H_BPM_NUMBER);
	LoadDivGraph(L"img/ResultMaxComboNumber.png", 10, 10, 1, 25, 50, H_SCROLL_NUMBER);
	*/

	
	
	sprintfDx(strcash, L"sound/hit_sound/%s/f2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/r2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_R_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/g2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_G_S = LoadSoundMem(strcash, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/b2.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_N = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b3.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_L = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b1.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_B_S = LoadSoundMem(strcash, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/k.wav", option->hitSound[option->op.hitSound]);
	SH.SH_HIT_K = LoadSoundMem(strcash, 1);




	SH_STEP_CHANGE = LoadSoundMem(L"sound/difficulty_select.wav");
	SH_STEP_INC_DEC = LoadSoundMem(L"sound/song_select.wav");
	SH_SAVE = LoadSoundMem(L"sound/shutter_signal.wav");
	SH_NO = LoadSoundMem(L"sound/no.wav");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");

	int Keylist[4][4] = { { KEY_INPUT_V,KEY_INPUT_B,KEY_INPUT_N,KEY_INPUT_M },
	{ KEY_INPUT_F,KEY_INPUT_G,KEY_INPUT_H,KEY_INPUT_J },
	{ KEY_INPUT_R,KEY_INPUT_T,KEY_INPUT_Y,KEY_INPUT_U },
	{ KEY_INPUT_4,KEY_INPUT_5,KEY_INPUT_6,KEY_INPUT_7 } };//キーのリスト 下から順に

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);
	H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg_game.png")).c_str());

	double time_per_step = ((double)60/Music[song_number].bpm[difficulty])/((double)EDITSTEP/4)*1000;//1stepあたりに進む経過時間(ms)
	double base_bpm = Music[song_number].bpm[difficulty];//基本BPM
	double scroll = init_scroll;
	double bpm = base_bpm*scroll;//今の実質のBPM

	//insertをpassed_timeまでもっていく
	//insertまでのpassed_timeがわかる
	
	while (insert_passed_time < passed_time) {


		while (insert->next->content != CONTENTS_END && insert->next->step <= step_count) {//insertの次の音符、命令がstep_countより下に行ったのでinsertを次へ
			insert = insert->next;
			if (insert->content == CONTENTS_COMMAND) {
				if (insert->data.command.kind == COMMAND_KIND_BPM) {
					base_bpm = insert->data.command.value[0];
					bpm = base_bpm * scroll;
				}
				if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
					base_bpm = insert->data.command.value[0];
					bpm = base_bpm;
				}
				if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
					scroll = insert->data.command.value[0];
					bpm = base_bpm*scroll;
				}
				if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
					scroll = insert->data.command.value[0] / base_bpm;
					bpm = base_bpm*scroll;
				}
				if (insert->data.command.kind == COMMAND_KIND_STOP) {
					insert_passed_time += insert->data.command.value[0]*1000;
				}
				if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
					insert_passed_time += ((double)240*1000 / bpm)*(insert->data.command.value[0] / insert->data.command.value[1]);
				}
				time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
			}
			//#ENDには行かないようにする
		}

		step_count += 1;
		insert_passed_time += time_per_step;

	}

	//insertが一番最初指してるとき用

	
	while (insert->next->content != CONTENTS_END && insert->next->step <= step_count) {//insertの次の音符、命令がstep_countより下に行ったのでinsertを次へ
		insert = insert->next;
		if (insert->content == CONTENTS_COMMAND) {
			if (insert->data.command.kind == COMMAND_KIND_BPM) {
				base_bpm = insert->data.command.value[0];
				bpm = base_bpm*scroll;
			}
			if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
				base_bpm = insert->data.command.value[0];
				bpm = base_bpm;
			}
			if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
				scroll = insert->data.command.value[0];
				bpm = base_bpm*scroll;
			}
			if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
				scroll = insert->data.command.value[0] / base_bpm;
				bpm = base_bpm*scroll;
			}
			if (insert->data.command.kind == COMMAND_KIND_STOP) {
				insert_passed_time += insert->data.command.value[0] * 1000;
			}
			if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
				insert_passed_time += ((double)240 * 1000 / bpm)*(insert->data.command.value[0] / insert->data.command.value[1]);
			}
			time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
		}
		//#ENDには行かないようにする
	}
	insert_passed_time += time_per_step;

	//微妙な場所を指さないようにinsertから行き過ぎた分のstepを引く
	insert_passed_time -= time_per_step*(step_count - insert->step);
	step_count = insert->step;
	if (step_count < 0)step_count = 0;
	step_count_draw = step_count;

	//フォント設定
	ChangeFont(L"メイリオ");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(15);
	SetFontThickness(9);

	GAME_start_time = GetNowCount_d(config);//開始時のカウント
	while (1) {
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}

		//----CALC----
		GAME_passed_time = (((double)GetNowCount_d(config) - (double)GAME_start_time));//経過時間計算
		LOOP_passed_time = ((double)GetNowCount_d(config) - GAME_start_time) - time_cash;//1ループにかかった時間を算出
		time_cash = ((double)GetNowCount_d(config) - GAME_start_time);
	
		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		//----Button----
		Get_Key_State(Buf, Key, AC);

		int PressFrame = int(15.0*(17.0 / LOOP_passed_time));//ボタン押し続けてカーソルが動き続けるようになるまでのフレーム
		if (PressFrame <= 0)PressFrame = 1;//0以下にはしない

		int PushTimeTh = 35;//長押し閾値

		if (Key[KEY_INPUT_ESCAPE] == 1) {
			if (MusicSub->editable[difficulty] == 1) {//編集可能
				if (SAVE_EDIT_SCORE(head, Music, song_number, difficulty, 1) == 0) {//正常に保存出来たら戻る
					double insert_p_time = calc_insert_passed_time(Music[song_number].bpm[difficulty], init_scroll, head, insert);

					double passed_time_difference = insert_p_time - passed_time;//insert-passed_timeまでの時間の差

					*debug_time_passed += passed_time_difference;

					score_delete_cell_all(head);
					//copyのメモリ解放
					score_delete_cell_all(&CopyHead);
					break;
				}
				else {
					saveErrorTextShowFlag = 1;
					PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			else {
				PlaySoundMem(SH_NO, DX_PLAYTYPE_BACK, TRUE);
			}
			
			
			//PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[Button_Shutter] == 1) {//スクリーンショット
			ScreenShot(SH_SHUTTER_SIGNAL, SH_SHUTTER);
		}

		if (Key[KEY_INPUT_F11] == 1) {//保存せずに戻る
			double insert_p_time = calc_insert_passed_time(Music[song_number].bpm[difficulty], init_scroll, head, insert);

			double passed_time_difference = insert_p_time - passed_time;//insert-passed_timeまでの時間の差

			*debug_time_passed += passed_time_difference;

			score_delete_cell_all(head);
			//copyのメモリ解放
			score_delete_cell_all(&CopyHead);
			break;
			//PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
		}

		if (Key[KEY_INPUT_F1] == 1) {//操作説明の表示非表示
			if (isInstructionAppear == 0)isInstructionAppear = 1;
			else isInstructionAppear = 0;
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}

		/*
		if (Key[KEY_INPUT_F2] == 1) {//曲を流す
			if (CheckSoundMem(SH_SONG)) {
				StopSoundMem(SH_SONG);
			}
			else {
				SetCurrentPositionSoundMem(int(((double)44100 * 1 * (insert_passed_time)) / 1000), SH_SONG);
				PlaySoundMem(SH_SONG, DX_PLAYTYPE_BACK, FALSE);
			}
		}
		*/

		if (Key[KEY_INPUT_Z] == 1 || (Key[KEY_INPUT_Z] >= PressFrame && push_counter >= PushTimeTh)) {//step_countをinsertにフィットさせる 前の音符に移動
			push_counter = 0;
			
			if (step_count == insert->step && insert->before != NULL) {//step_countの位置に音符命令が既にあって下に音符、命令があるときはそこへ移動
				SCORE_CELL* tmp = insert;

				while (step_count == insert->step && tmp->before != NULL) {
					step_count = tmp->before->step;
					tmp = tmp->before;
				}
			}
			else {//step_countの位置に音符命令が無い時は一番近い下の音符命令に移動
				step_count = insert->step;
			}

			if (step_count <= -1)step_count = 0;//-1以下にはしない

			while (insert->before != NULL && insert->step > step_count) {//insertがstep_countより上に行ったのでinsertを下へ

				insert = insert->before;
				if (insert->content == CONTENTS_COMMAND) {
					if (insert->data.command.kind == COMMAND_KIND_BPM) {
						base_bpm = insert->data.command.value[0];
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
						base_bpm = insert->data.command.value[0];
						bpm = base_bpm;
					}
					if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
						scroll = insert->data.command.value[0];
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
						scroll = insert->data.command.value[0] / base_bpm;
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_STOP) {
						insert_passed_time -= insert->data.command.value[0] * 1000;
					}
					if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
						insert_passed_time -= ((double)240 * 1000 / bpm) * (insert->data.command.value[0] / insert->data.command.value[1]);
					}
					time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
				}
			}
			if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)area.end = step_count;//シフト押してたら範囲選択を適応
			PlaySoundMem(SH_STEP_INC_DEC, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_A] == 1 || (Key[KEY_INPUT_A] >= PressFrame && push_counter >= PushTimeTh)) {//次の音符、命令に移動
			push_counter = 0;

			if (insert->next != NULL) {
				step_count = insert->next->step;
			}

			while (insert->next->content != CONTENTS_END && insert->next->step <= step_count) {//insertの次の音符、命令がstep_countより下に行ったのでinsertを次へ

				insert = insert->next;
				if (insert->content == CONTENTS_COMMAND) {
					if (insert->data.command.kind == COMMAND_KIND_BPM) {
						base_bpm = insert->data.command.value[0];
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
						base_bpm = insert->data.command.value[0];
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
						scroll = insert->data.command.value[0];
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
						scroll = insert->data.command.value[0] / base_bpm;
						bpm = base_bpm * scroll;
					}
					if (insert->data.command.kind == COMMAND_KIND_STOP) {
						insert_passed_time += insert->data.command.value[0] * 1000;
					}
					if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
						insert_passed_time += ((double)240 * 1000 / bpm) * (insert->data.command.value[0] / insert->data.command.value[1]);
					}
					time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
				}
				//#ENDには行かないようにする
			}
			if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)area.end = step_count;//シフト押してたら範囲選択を適応
			PlaySoundMem(SH_STEP_INC_DEC, DX_PLAYTYPE_BACK, TRUE);
		}


		/*
		if (Key[KEY_INPUT_A] == 1) {
			if (AutoMove == 0) {
				AutoMove = 1;//配置音符種類変更
			}
			else if (AutoMove == 1) {
				AutoMove = 0;
			}
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		*/
		if (Key[KEY_INPUT_Q] == 1) {//小節線表示起点を現在のstep_countにする
			*measure_start_step = step_count%EDITSTEP;
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}


		if ((Key[KEY_INPUT_LCONTROL] >= 1 || Key[KEY_INPUT_RCONTROL] >= 1) &&
			Key[KEY_INPUT_M] == 1 &&
			area.StartCell != NULL && area.EndCell != NULL) {//ミラー配置にする
			MirrorSelectArea(area);
			SolveLN(end);

			//選択範囲の処理領域の再算出
			area.end += 1;//選択範囲endが事前に1減っているのであらかじめ1足しておく
			DecideProcessArea(&area, end);//areaの指すセルが消えている場合があるので処理領域を再算出
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}


		if ((Key[KEY_INPUT_LCONTROL] >= 1 || Key[KEY_INPUT_RCONTROL] >= 1) &&
			Key[KEY_INPUT_C] == 1 &&
			area.StartCell != NULL && area.EndCell != NULL) {//選択範囲をコピー
			
			PasteRange = area.end - area.start;

			//copyのメモリ解放
			score_delete_cell_all(&CopyHead);
			//CopyHeadに選択範囲をコピー
			CopySelectArea(step_count, area, &CopyHead);
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}


		if ((Key[KEY_INPUT_LCONTROL] >= 1 || Key[KEY_INPUT_RCONTROL] >= 1) &&
			Key[KEY_INPUT_V] == 1 &&
			CopyHead.next != NULL) {//step_countを基準に選択範囲を貼り付け
			
			//貼り付け範囲が#ENDを超えたときのために最後の拍子を取得
			SCORE_CELL* p_m = score_cell_find_before_command(end, COMMAND_KIND_MEASURE);
			double scale_m = 1;
			if (p_m != NULL) {//直前の拍子変更位置を探して見つかった
				scale_m = p_m->data.command.value[0] / p_m->data.command.value[1];
			}
			else {//#MEASUREが見つからなかったのでデフォルトの4/4拍子と見る
			}

			PasteSelectArea(step_count, &insert, PasteRange, &CopyHead, scale_m);
			SolveLN(end);

			area.end += 1;//選択範囲endが事前に1減っているのであらかじめ1足しておく
			DecideProcessArea(&area, end);//areaの指すセルが消えている場合があるので処理領域を再算出
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}


		//範囲選択
		if (Key[KEY_INPUT_LSHIFT] == 1 || Key[KEY_INPUT_RSHIFT] == 1) {//選択範囲を現在位置で初期化
			area.start = step_count;
			area.end = step_count;
			area.StartCell = NULL;
			area.EndCell = NULL;
		}
		if (Key[KEY_INPUT_LSHIFT] == -1 || Key[KEY_INPUT_RSHIFT] == -1) {//選択範囲の選択完了したら処理領域を決定
			DecideProcessArea(&area, end);//areaに処理領域を格納
		}

		if (Key[KEY_INPUT_LCONTROL] == 0 && Key[KEY_INPUT_RCONTROL] == 0) {//移動
			if (Key[KEY_INPUT_UP] == 1 || (Key[KEY_INPUT_UP] >= PressFrame && push_counter >= PushTimeTh)) {
				push_counter = 0;
				int MoveStep = int(((double)*SwingBackBeatTime / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);//スウィング比*基本移動ステップ
				*SwingBackBeatTime = SWING_DIVISION - *SwingBackBeatTime;//拍を入れ替えたとして扱う

				for (i = 0; i < MoveStep; i++) {

					step_count += 1;
					while (insert->next->content != CONTENTS_END && insert->next->step <= step_count) {//insertの次の音符、命令がstep_countより下に行ったのでinsertを次へ
						insert = insert->next;
						if (insert->content == CONTENTS_COMMAND) {
							if (insert->data.command.kind == COMMAND_KIND_BPM) {
								base_bpm = insert->data.command.value[0];
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
								base_bpm = insert->data.command.value[0];
								bpm = base_bpm * scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
								scroll = insert->data.command.value[0];
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
								scroll = insert->data.command.value[0] / base_bpm;
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_STOP) {
								insert_passed_time += insert->data.command.value[0] * 1000;
							}
							if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
								insert_passed_time += ((double)240 * 1000 / bpm)*(insert->data.command.value[0] / insert->data.command.value[1]);
							}
							time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
						}
						//#ENDには行かないようにする
					}
					insert_passed_time += time_per_step;

				}
				//音符があるならそれを指している
				PlaySoundMem(SH_STEP_INC_DEC, DX_PLAYTYPE_BACK, TRUE);

				if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)area.end = step_count;//シフト押してたら範囲選択を適応
			}
			if (Key[KEY_INPUT_DOWN] == 1 || (Key[KEY_INPUT_DOWN] >= PressFrame && push_counter >= PushTimeTh)) {
				push_counter = 0;
				int MoveStep = int(((SWING_DIVISION - (double)*SwingBackBeatTime) / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);//スウィング比*基本移動ステップ
				*SwingBackBeatTime = SWING_DIVISION - *SwingBackBeatTime;//拍を入れ替えたとして扱う

				for (i = 0; i < MoveStep; i++) {
					if (step_count == 0)break;//forを抜ける
					step_count -= 1;
					while (insert->before != NULL && insert->step > step_count) {//insertがstep_countより上に行ったのでinsertを下へ
						insert = insert->before;
						if (insert->content == CONTENTS_COMMAND) {
							if (insert->data.command.kind == COMMAND_KIND_BPM) {
								base_bpm = insert->data.command.value[0];
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_LETBPM) {
								base_bpm = insert->data.command.value[0];
								bpm = base_bpm;
							}
							if (insert->data.command.kind == COMMAND_KIND_SCROLL) {
								scroll = insert->data.command.value[0];
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
								scroll = insert->data.command.value[0] / base_bpm;
								bpm = base_bpm*scroll;
							}
							if (insert->data.command.kind == COMMAND_KIND_STOP) {
								insert_passed_time -= insert->data.command.value[0] * 1000;
							}
							if (insert->data.command.kind == COMMAND_KIND_STOPSTEP) {
								insert_passed_time -= ((double)240 * 1000 / bpm)*(insert->data.command.value[0] / insert->data.command.value[1]);
							}
							time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;
						}
						//#ENDには行かないようにする
					}
					insert_passed_time -= time_per_step;
				}
				//音符(n)があるならそれを指している
				//cccn-cn-n-n-cn-ccn-n-n-n

				PlaySoundMem(SH_STEP_INC_DEC, DX_PLAYTYPE_BACK, TRUE);

				if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)area.end = step_count;//シフト押してたら範囲選択を適応
			}
		}
		else {//CTRLを押しているとき拡大縮小
			if (Key[KEY_INPUT_UP] == 1 || (Key[KEY_INPUT_UP] >= PressFrame && push_counter >= PushTimeTh)) {
				push_counter = 0;
				if (scale_number < SCALE_ARRAY_NUMBER - 1) {
					scale_number++;
					*scale_score = scale_array[scale_number];
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (Key[KEY_INPUT_DOWN] == 1 || (Key[KEY_INPUT_DOWN] >= PressFrame && push_counter >= PushTimeTh)) {
				push_counter = 0;
				if (scale_number >= 1) {
					scale_number--;
					*scale_score = scale_array[scale_number];
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}


		if (Key[KEY_INPUT_LCONTROL] == 0 && Key[KEY_INPUT_RCONTROL] == 0) {//CTRLを押していない
			if (Key[KEY_INPUT_RIGHT] == 1 || (Key[KEY_INPUT_RIGHT] >= PressFrame && push_LR_counter >= PushTimeTh)) {
				push_LR_counter = 0;
				if (*step_array_number < STEP_INDEX_ARRAY - 1) {
					(*step_array_number)++;
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (Key[KEY_INPUT_LEFT] == 1 || (Key[KEY_INPUT_LEFT] >= PressFrame && push_LR_counter >= PushTimeTh)) {
				push_LR_counter = 0;
				if (*step_array_number >= 1) {
					(*step_array_number)--;
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}else{//CTRLを押している時はスウィング比変更
			if (Key[KEY_INPUT_RIGHT] == 1 || (Key[KEY_INPUT_RIGHT] >= PressFrame && push_LR_counter >= PushTimeTh)) {//スウィング比を増やす(裏拍の位置を後ろにずらす)
				push_LR_counter = 0;
				if (*SwingBackBeatTime < SWING_DIVISION - 1) {
					(*SwingBackBeatTime)++;
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
			if (Key[KEY_INPUT_LEFT] == 1 || (Key[KEY_INPUT_LEFT] >= PressFrame && push_LR_counter >= PushTimeTh)) {//スウィング比を減らす(裏拍の位置を前にずらす)
				push_LR_counter = 0;
				if (*SwingBackBeatTime > 1) {
					(*SwingBackBeatTime)--;
					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}
			}
		}

		if (Key[KEY_INPUT_SPACE] == 1) {
			if (note_kind == 0) {
				note_kind = 1;//配置音符種類変更
				notesMode.switchToKfln();
			}
			else if (note_kind == 1) {
				note_kind = 0;
				notesMode.switchToRgb();
			}
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		
		
		//音符、命令配置
		if (Key[KEY_INPUT_LCONTROL] == 0 && Key[KEY_INPUT_RCONTROL] == 0) {//CTRLを押していない

			if (command_put == 0) {//音符配置モードのとき
				for (i = 0; i <= 2; i++) {//縦 RGB 012
					for (j = 0; j <= 3; j++) {//横 0123
						if (Key[Keylist[i][j]] == 1) {
							if (insert->content == CONTENTS_COMMAND || insert->step != step_count) {//insertが命令のセルだったりstepとinsert違うならそこに新たなセル挿入
								score_insert_cell(insert, step_count);

								insert = insert->next;

								score_cell_write_note(insert, 0, 0, 0, 0);//color0で初期化
								score_cell_write_note(insert, 1, 0, 0, 0);
								score_cell_write_note(insert, 2, 0, 0, 0);
								score_cell_write_note(insert, 3, 0, 0, 0);
							}
							/*
							if (insert->next->content == 3) {//一番最後の音符、命令だったら
								insert->next->step = insert->step + EDITSTEP;//#ENDの位置を変更
								insert->next->next->step = insert->step + EDITSTEP;//終端の位置を変更
							}
							*/
							if (insert->next->content == CONTENTS_END) {//一番最後の音符、命令だったら
								SCORE_CELL* p_m = score_cell_find_before_command(insert->next, COMMAND_KIND_MEASURE);
								if (p_m != NULL) {//直前の拍子変更位置を探して見つかった
									double scale_m = p_m->data.command.value[0] / p_m->data.command.value[1];
									insert->next->step = insert->step + int(EDITSTEP * scale_m);//#ENDの位置を変更(現在の位置+現在の拍子一小節分)
									insert->next->next->step = insert->step + int(EDITSTEP * scale_m);//終端の位置を変更
								}
								else {//#MEASUREが見つからなかったのでデフォルトの4/4拍子と見る
									insert->next->step = insert->step + EDITSTEP;//#ENDの位置を変更(現在の位置+4/4一小節分)
									insert->next->next->step = insert->step + EDITSTEP;//終端の位置を変更
								}
							}

							//insertから下、上にある音符を保存 使うときはその都度NULLじゃないか確かめる必要あり
							SCORE_CELL* find_before = score_cell_find_before_note(insert, j);
							SCORE_CELL* find_next = score_cell_find_next_note(insert, j);

							int putflag = -1;//-1:黒、LN終点を置いた 0:消した 1:置いた
							if (note_kind == 0) {
								if (i == 0) {//Red
									if (insert->data.note.color[j] == 0) {//何もない
										score_cell_write_note(insert, j, NOTE_COLOR_RED, 0, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RED) {
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_GREEN) {
										score_cell_write_note(insert, j, NOTE_COLOR_YELLOW, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLUE) {
										score_cell_write_note(insert, j, NOTE_COLOR_MAGENTA, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_YELLOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_GREEN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_CYAN) {
										score_cell_write_note(insert, j, NOTE_COLOR_WHITE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_MAGENTA) {
										score_cell_write_note(insert, j, NOTE_COLOR_BLUE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_WHITE) {
										score_cell_write_note(insert, j, NOTE_COLOR_CYAN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RAINBOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_RED, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLACK) {
										score_cell_write_note(insert, j, NOTE_COLOR_RED, insert->data.note.group[j], 0);
									}
									if (find_before != NULL) {
										if (find_before->data.note.group[j] == 1) {//LN終点の色を変えたら
											find_before->data.note.color[j] = insert->data.note.color[j];//始点の色も変える
										}
									}
									if (find_next != NULL) {
										if (find_next->data.note.group[j] == 2) {//LN始点の色を変えたら
											find_next->data.note.color[j] = insert->data.note.color[j];//終点の色も変える
										}
									}
									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}

								if (i == 1) {//Green
									if (insert->data.note.color[j] == 0) {//何もない
										score_cell_write_note(insert, j, NOTE_COLOR_GREEN, 0, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RED) {
										score_cell_write_note(insert, j, NOTE_COLOR_YELLOW, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_GREEN) {
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLUE) {
										score_cell_write_note(insert, j, NOTE_COLOR_CYAN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_YELLOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_RED, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_CYAN) {
										score_cell_write_note(insert, j, NOTE_COLOR_BLUE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_MAGENTA) {
										score_cell_write_note(insert, j, NOTE_COLOR_WHITE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_WHITE) {
										score_cell_write_note(insert, j, NOTE_COLOR_MAGENTA, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RAINBOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_GREEN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLACK) {
										score_cell_write_note(insert, j, NOTE_COLOR_GREEN, insert->data.note.group[j], 0);
									}
									if (find_before != NULL) {
										if (find_before->data.note.group[j] == 1) {//LN終点の色を変えたら
											find_before->data.note.color[j] = insert->data.note.color[j];//始点の色も変える
										}
									}
									if (find_next != NULL) {
										if (find_next->data.note.group[j] == 2) {//LN始点の色を変えたら
											find_next->data.note.color[j] = insert->data.note.color[j];//終点の色も変える
										}
									}
									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}

								if (i == 2) {//Blue
									if (insert->data.note.color[j] == 0) {//何もない
										score_cell_write_note(insert, j, NOTE_COLOR_BLUE, 0, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RED) {
										score_cell_write_note(insert, j, NOTE_COLOR_MAGENTA, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_GREEN) {
										score_cell_write_note(insert, j, NOTE_COLOR_CYAN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLUE) {
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_YELLOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_WHITE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_CYAN) {
										score_cell_write_note(insert, j, NOTE_COLOR_GREEN, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_MAGENTA) {
										score_cell_write_note(insert, j, NOTE_COLOR_RED, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_WHITE) {
										score_cell_write_note(insert, j, NOTE_COLOR_YELLOW, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_RAINBOW) {
										score_cell_write_note(insert, j, NOTE_COLOR_BLUE, insert->data.note.group[j], insert->data.note.bright[j]);
									}
									else if (insert->data.note.color[j] == NOTE_COLOR_BLACK) {
										score_cell_write_note(insert, j, NOTE_COLOR_BLUE, 0, 0);
									}
									if (find_before != NULL) {
										if (find_before->data.note.group[j] == 1) {//LN終点の色を変えたら
											find_before->data.note.color[j] = insert->data.note.color[j];//始点の色も変える
										}
									}
									if (find_next != NULL) {
										if (find_next->data.note.group[j] == 2) {//LN始点の色を変えたら
											find_next->data.note.color[j] = insert->data.note.color[j];//終点の色も変える
										}
									}
									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_B_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_B_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}
							}
							else if (note_kind == 1) {//配置音符の種類変更をしている
								if (i == 0) {//Black
									if (insert->data.note.color[j] == NOTE_COLOR_BLACK) {
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
									}
									else {//黒音符を書き込む

										if (insert->data.note.group[j] == 1) {//始点を黒で上書きしたとき
											score_cell_write_note(find_next, j, 0, 0, 0);//終点を消す
											score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
										}
										else if (insert->data.note.group[j] == 2) {//終点を黒で上書きしたとき
											find_before->data.note.group[j] = 0;//始点を単ノートにする
										}
										else if (find_next != NULL && find_before != NULL) {
											if (find_before->data.note.group[j] == 1) {//LNの中に黒を置いた時
												find_next->data.note.group[j] = 0;//始点を単ノートにする
												find_before->data.note.group[j] = 0;//終点を単ノートにする
											}
										}

										score_cell_write_note(insert, j, NOTE_COLOR_BLACK, 0, 0);//黒音符書き込み

									}
									PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
								}
								if (i == 1) {//Rainbow
									if (insert->data.note.color[j] == NOTE_COLOR_RAINBOW) {//既に虹がある
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else {//虹ではない
										if (insert->data.note.color[j] == 0)putflag = 1;//何もないときは置いたフラグ立てる

										score_cell_write_note(insert, j, NOTE_COLOR_RAINBOW, insert->data.note.group[j], insert->data.note.bright[j]);

									}
									if (find_before != NULL) {
										if (find_before->data.note.group[j] == 1) {//LN終点の色を変えたら
											find_before->data.note.color[j] = insert->data.note.color[j];//始点の色も変える
										}
									}
									if (find_next != NULL) {
										if (find_next->data.note.group[j] == 2) {//LN始点の色を変えたら
											find_next->data.note.color[j] = insert->data.note.color[j];//終点の色も変える
										}
									}
									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}
								if (i == 2) {//LN
									if (insert->data.note.group[j] == 2) {//既にLN終点があったら
										score_cell_write_note(insert, j, 0, 0, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す

										score_cell_write_note(find_before, j, find_before->data.note.color[j], 0, find_before->data.note.bright[j]);//下にある音符を単ノートにする
									}
									else {//今のstep_countの行にLN終点が配置されてない
										if (find_before != NULL) {//この行の下に音符があって
											if (find_before->data.note.color[j] != NOTE_COLOR_BLACK && find_before->data.note.group[j] != 2) {//それが黒じゃなくLN終点でもないならLN終端を置ける

												if (find_next != NULL) {//上に音符がある
													if (find_next->data.note.group[j] == 2) {//それがLN終点なら消す
														score_cell_write_note(find_next, j, 0, 0, 0);//消す
														score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
													}
												}
												score_cell_write_note(find_before, j, find_before->data.note.color[j], 1, find_before->data.note.bright[j]);//下にある音符をLN始点にする

												if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//SHIFT押しながらだと黒終点にする
													score_cell_write_note(insert, j, score_cell_find_before_note(insert, j)->data.note.color[j], 2, score_cell_find_before_note(insert, j)->data.note.bright[j], 1);//LN情報を書き込む(黒終点)
												}
												else {
													score_cell_write_note(insert, j, score_cell_find_before_note(insert, j)->data.note.color[j], 2, score_cell_find_before_note(insert, j)->data.note.bright[j], 0);//LN情報を書き込む(通常終点)
												}
											}
											else if (find_before->data.note.group[j] == 2) {//下がLN終点のときは今の位置にずらす

												score_cell_write_note(find_before, j, 0, 0, 0);//下にある音符を消す
												score_cell_delete_if_no_note(&find_before);//その結果何もない行になったら行を消す

												if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//SHIFT押しながらだと黒終点にする
													score_cell_write_note(insert, j, score_cell_find_before_note(insert, j)->data.note.color[j], 2, score_cell_find_before_note(insert, j)->data.note.bright[j], 1);//LN情報を書き込む(黒終点)
												}
												else {
													score_cell_write_note(insert, j, score_cell_find_before_note(insert, j)->data.note.color[j], 2, score_cell_find_before_note(insert, j)->data.note.bright[j], 0);//LN情報を書き込む(通常終点)
												}

												if (find_next != NULL) {//上に音符がある
													if (find_next->data.note.group[j] == 2) {//それがLN終点なら消す
														score_cell_write_note(find_next, j, 0, 0, 0);//消す
														score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
													}
												}
											}


										}
									}
								}
							}

							if (putflag == 1 && (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)) {//音符を置いたときSHIFT押してたら光らせる
								if (insert->data.note.color[j] != NOTE_COLOR_BLACK && insert->data.note.color[j] != 0) {
									insert->data.note.bright[j] = 1;//光らせる
									if (insert->data.note.group[j] == 2) {//LN終点なら始点も光らせる
										find_before->data.note.bright[j] = 1;

									}
									if (insert->data.note.group[j] == 1) {//LN始点なら終点も光らせる
										find_next->data.note.bright[j] = 1;
									}
								}
							}

							if (putflag != -1) {
								if (putflag == 0) {//RGB虹音符を消していたとき 黒を置いたとき
									if (find_next != NULL) {
										if (find_next->data.note.group[j] == 2) {//LN始点を消していたとき
											score_cell_write_note(find_next, j, 0, 0, 0);//LN終点も消す
											score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
										}
									}
									if (find_before != NULL) {
										if (find_before->data.note.group[j] == 1) {//LN終点を消していたとき
											find_before->data.note.group[j] = 0;//LN始点を単ノートにする
										}
									}
								}
								if (putflag == 1) {//音符を置いていたとき
									if (find_next != NULL && find_before != NULL) {
										if (find_next->data.note.group[j] == 2) {//LNの中に音符を置いていたとき
											score_cell_write_note(insert, j, insert->data.note.color[j], 1, insert->data.note.bright[j]);//この音符をLN始点にする
											score_cell_write_note(find_before, j, find_before->data.note.color[j], 0, find_before->data.note.bright[j]);//下の音符は単ノートにする
										}
									}
								}
							}

							//選択範囲の処理領域の再算出
							area.end += 1;//選択範囲endが事前に1減っているのであらかじめ1足しておく
							DecideProcessArea(&area, end);//areaの指すセルが消えている場合があるので処理領域を再算出
						}
					}
				}

				if (Key[KEY_INPUT_BACK] == 1) {//BackSpace押されてたら音符行を削除
					if (area.StartCell != NULL && area.EndCell != NULL) {//選択範囲があればそこを削除する
						DeleteSelectArea(&insert, area);
						SolveLN(end);
					}
					else {//範囲選択していない
						for (j = 0; j <= 3; j++) {
							if (insert->content == CONTENTS_NOTE && insert->step == step_count) {//insertが音符のセルでstepとinsert同じなら削除

								//insertから下、上にある音符を保存 使うときはその都度NULLじゃないか確かめる必要あり
								SCORE_CELL* find_before = score_cell_find_before_note(insert, j);
								SCORE_CELL* find_next = score_cell_find_next_note(insert, j);

								score_cell_write_note(insert, j, 0, 0, 0);//消す
								score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す


								if (find_next != NULL) {
									if (find_next->data.note.group[j] == 2) {//LN始点を消していたとき
										score_cell_write_note(find_next, j, 0, 0, 0);//LN終点も消す
										score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
									}
								}
								if (find_before != NULL) {
									if (find_before->data.note.group[j] == 1) {//LN終点を消していたとき
										find_before->data.note.group[j] = 0;//LN始点を単ノートにする
									}
								}

							}
						}
					}
					//選択範囲の処理領域の再算出
					area.end += 1;//選択範囲endが事前に1減っているのであらかじめ1足しておく
					DecideProcessArea(&area, end);//areaの指すセルが消えている場合があるので処理領域を再算出

					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}

			}
			else {//命令配置モード(command_put!=0)の時
				for (i = 0; i <= 2; i++) {//縦 RGB 012
					for (j = 0; j <= 3; j++) {//横 0123
						if (Key[Keylist[i][j]] == 1) {
							if (i == 1) {
								//緑が押された
								insert_command = insert;
								if (insert_command->step != step_count) {//stepとinsert_command->step違うならそこに新たなセル挿入
									score_insert_cell(insert_command, step_count);

									insert_command = insert_command->next;
									insert = insert_command;

									score_cell_write_command(insert_command, command_put, 1, 1, 1, 1);//1,1,1,1で初期化
								}

								insert_command = score_cell_find_command_before(insert_command, command_put);//↑でセル挿入してたらそのセルが返る(何も変わらない)
								if (insert_command == NULL) {//今のステップにこの命令が無いなら挿入

									if (insert->content == CONTENTS_NOTE) {//insertが指しているのが音符
										score_insert_cell(insert->before, step_count);//insertの前に新しいセル挿入
										insert_command = insert->before;//現在操作している命令セルを格納
									}
									if (insert->content == CONTENTS_COMMAND) {//insertが指しているのが命令
										score_insert_cell(insert, step_count);//insertに新しいセル挿入
										insert_command = insert->next;//現在操作している命令セルを格納
										insert = insert_command;
									}
								}

								if (command_put == COMMAND_KIND_STOPSTEP) {
									score_cell_write_command(insert_command, command_put, cvb_STOPSTEP[0], cvb_STOPSTEP[1], 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_STOP) {
									score_cell_write_command(insert_command, command_put, cvb_STOP, 1, 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_HS) {
									score_cell_write_command(insert_command, command_put, cvb_HS[0], cvb_HS[1], cvb_HS[2], cvb_HS[3]);//初期化
								}
								else if (command_put == COMMAND_KIND_BPM) {
									score_cell_write_command(insert_command, command_put, cvb_BPM, 1, 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_LETBPM) {
									score_cell_write_command(insert_command, command_put, cvb_LETBPM, 1, 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_SCROLL) {
									score_cell_write_command(insert_command, command_put, cvb_SCROLL, 1, 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_SCROLL_BPM) {
									score_cell_write_command(insert_command, command_put, cvb_SCROLL_BPM, 1, 1, 1);//初期化
								}
								else if (command_put == COMMAND_KIND_MEASURE) {
									score_cell_write_command(insert_command, command_put, cvb_MEASURE[0], cvb_MEASURE[1], 1, 1);//初期化
								}

							

								if (insert_command->next->content == CONTENTS_END) {//一番最後の音符、命令だったら
									SCORE_CELL* p_m = score_cell_find_before_command(insert_command->next, COMMAND_KIND_MEASURE);
									if (p_m != NULL) {//直前の拍子変更位置を探して見つかった
										double scale_m = p_m->data.command.value[0] / p_m->data.command.value[1];
										insert_command->next->step = insert->step + int(EDITSTEP * scale_m);//#ENDの位置を変更(現在の位置+現在の拍子一小節分)
										insert_command->next->next->step = insert->step + int(EDITSTEP * scale_m);//終端の位置を変更
									}
									else {//#MEASUREが見つからなかったのでデフォルトの4/4拍子と見る
										insert_command->next->step = insert->step + EDITSTEP;//#ENDの位置を変更(現在の位置+4/4一小節分)
										insert_command->next->next->step = insert->step + EDITSTEP;//終端の位置を変更
									}
								}
							}
						}
						if ((Key[Keylist[i][j]] == 1)//押した　か
							|| ((Key[Keylist[i][j]] >= PressFrame) && push_command_val_counter >= PushTimeTh)) {//押し続けている
							if (insert->step == step_count) {//insertとstep_countが一致している(命令行にいるとき)
								if (i == 0 || i == 2) {//赤、青が押された 値を増減する
									push_command_val_counter = 0;
									insert_command = insert;
									insert_command = score_cell_find_command_before(insert_command, command_put);//目的の命令がstep_countにあるか
									if (insert_command != NULL) {//目的の命令が存在する
										double inc = 1;
										if (command_put == COMMAND_KIND_HS) {
											inc *= 0.1;//HSは0.1ずつ
										}

										if (i == 0) {//赤が押されている
											inc *= -1;//減らすようにする
										}
										if (Key[KEY_INPUT_LALT] >= 1 || Key[KEY_INPUT_RALT] >= 1) {//ALT押しながらは値を小さく増減
											if (command_put == COMMAND_KIND_HS) {
												inc *= 0.1;//HSは0.1ずつ
											}
											else {
												inc *= 0.01;//100分の1単位で増減
											}
										}

										auto limitValue = [&] {
											for (int k = 0; k < 4; k++) {//コマンドの値の下限処理
												if (command_put == COMMAND_KIND_MEASURE) {
													if (insert_command->data.command.value[k] < 1) {
														insert_command->data.command.value[k] = 1;
													}
												}
												else {
													if (insert_command->data.command.value[k] < 0) {
														insert_command->data.command.value[k] = 0;
													}
												}
											}
										};

										if (command_put == COMMAND_KIND_STOPSTEP) {
											if (j == 0 || j == 1)insert_command->data.command.value[0] += inc;
											if (j == 2 || j == 3)insert_command->data.command.value[1] += inc;
											limitValue();
											cvb_STOPSTEP[0] = int(insert_command->data.command.value[0]);
											cvb_STOPSTEP[1] = int(insert_command->data.command.value[1]);
										}
										else if (command_put == COMMAND_KIND_STOP) {
											insert_command->data.command.value[0] += inc;
											limitValue();
											cvb_STOP = insert_command->data.command.value[0];
										}
										else if (command_put == COMMAND_KIND_HS) {
											insert_command->data.command.value[j] += inc;
											limitValue();
											cvb_HS[j] = insert_command->data.command.value[j];
										}
										else if (command_put == COMMAND_KIND_BPM) {
											insert_command->data.command.value[0] += inc;
											limitValue();
											cvb_BPM = insert_command->data.command.value[0];
										}
										else if (command_put == COMMAND_KIND_LETBPM) {
											insert_command->data.command.value[0] += inc;
											limitValue();
											cvb_LETBPM = insert_command->data.command.value[0];
										}
										else if (command_put == COMMAND_KIND_SCROLL) {
											insert_command->data.command.value[0] += inc;
											limitValue();
											cvb_SCROLL = insert_command->data.command.value[0];
										}
										else if (command_put == COMMAND_KIND_SCROLL_BPM) {
											insert_command->data.command.value[0] += inc;
											limitValue();
											cvb_SCROLL_BPM = insert_command->data.command.value[0];
										}
										else if (command_put == COMMAND_KIND_MEASURE) {
											if (j == 0 || j == 1)insert_command->data.command.value[0] += inc;
											if (j == 2 || j == 3)insert_command->data.command.value[1] += inc;
											limitValue();
											cvb_MEASURE[0] = int(insert_command->data.command.value[0]);
											cvb_MEASURE[1] = int(insert_command->data.command.value[1]);
										}

										
									}
								}
							}
						}
					}
				}
				if (Key[KEY_INPUT_BACK] == 1) {//BackSpace押されてたら命令行を削除


					if (insert->step == step_count) {//stepとinsert同じなら削除に移る
						insert_command = insert;
						insert_command = score_cell_find_command_before(insert_command, command_put);//目的の命令がstep_countにあるか
						if (insert_command != NULL) {//目的の命令が存在する
							score_cell_write_note(insert_command, 0, 0, 0, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 1, 0, 0, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 2, 0, 0, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 3, 0, 0, 0);//音符セルということにして消す

							if (insert_command == insert) {//音符が付いてない所の命令を消すとき
								score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
							}
							else {
								score_cell_delete_if_no_note(&insert_command);//その結果何もない行になったら行を消す
							}
						}
					}


					PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
				}

				//選択範囲の処理領域の再算出
				area.end += 1;//選択範囲endが事前に1減っているのであらかじめ1足しておく
				DecideProcessArea(&area, end);//areaの指すセルが消えている場合があるので処理領域を再算出
			}
		}


		//配置モードの変更
		if (Key[KEY_INPUT_0] == 1) {//音符配置モードにする
			command_put = 0;
			notesMode.switchNoteMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}

		if (Key[KEY_INPUT_1] == 1) {
			command_put = COMMAND_KIND_MEASURE;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_2] == 1) {
			command_put = COMMAND_KIND_STOPSTEP;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_3] == 1) {
			command_put = COMMAND_KIND_STOP;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_4] == 1) {
			command_put = COMMAND_KIND_HS;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_5] == 1) {
			command_put = COMMAND_KIND_BPM;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_6] == 1) {
			command_put = COMMAND_KIND_LETBPM;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_7] == 1) {
			command_put = COMMAND_KIND_SCROLL;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}
		if (Key[KEY_INPUT_8] == 1) {
			command_put = COMMAND_KIND_SCROLL_BPM;
			notesMode.switchCommandMode();
			PlaySoundMem(SH_STEP_CHANGE, DX_PLAYTYPE_BACK, TRUE);
		}


		//カウンタ処理
		for (i = 0; i < CRTBuf; i++) {
			push_counter += 1;
			push_LR_counter += 1;
			push_command_val_counter += 1;

			step_count_draw += ((double)step_count - step_count_draw)*0.015;
			scale_score_draw += ((double)*scale_score - scale_score_draw)*0.03;
		}


		//AutoMove処理
		if (AutoMove == 1) {
			if (FrameButtonFlag == 0) {
				for (i = 0; i <= 2; i++) {
					for (j = 0; j <= 3; j++) {
						if (Key[Keylist[i][j]] >= 1) {
							FrameButtonFlag = 1;//このフレームで音符配置ボタンをどれか押している
						}
					}
				}
			}else if (FrameButtonFlag == 1) {//前のフレームで押している
				FrameButtonFlag = 0;
				for (i = 0; i <= 2; i++) {
					for (j = 0; j <= 3; j++) {
						if (Key[Keylist[i][j]] >= 1) {
							FrameButtonFlag = 1;//このフレームで音符配置ボタンをどれか押している
						}
					}
				}
				if (FrameButtonFlag == 0) {//全部離した状態になったときstep進める
					step_count += step_array[*step_array_number];
					while (insert->next->content != CONTENTS_END && insert->next->step <= step_count) {//insertの次の音符、命令がstep_countより後ろに行ったのでinsertを次へ
						insert = insert->next;
						//#ENDには行かないようにする
					}
				}
			}
		}

		//printfDx("%d %d\n",Key[KEY_INPUT_F],Key[KEY_INPUT_G]);



		//----DRAW----

		DrawGraph(0, 0, H_BG, TRUE);
		


		


		//小節線と4分間隔補助線
		int measure_draw_step = 0;
		int measure_draw_count = 0;
		while (1) {

			if (measure_draw_count == 0) {//4拍ごとに赤線
				DrawBox(lane[0] - 64,//小節線
					int(judge_area - (measure_draw_step - step_count_draw + *measure_start_step)*scale*scale_score_draw + 3),
					lane[3] + 64,
					int(judge_area - (measure_draw_step - step_count_draw + *measure_start_step)*scale*scale_score_draw - 2), GetColor(255, 0, 0),TRUE);
			}
			else {
				DrawBox(lane[0] - 64,//1拍ごとに白線
					int(judge_area - (measure_draw_step - step_count_draw + *measure_start_step)*scale*scale_score_draw + 3),
					lane[3] + 64,
					int(judge_area - (measure_draw_step - step_count_draw + *measure_start_step)*scale*scale_score_draw - 2), GetColor(255, 255, 255),TRUE);
			}

			measure_draw_step += step_array[2];

			if (int(judge_area - (measure_draw_step - step_count_draw + *measure_start_step)*scale*scale_score_draw) < 0)break;//上まで描画した
			measure_draw_count = (measure_draw_count + 1) % 4;//0~3でループ
		}


		//stepで移動できる場所を示す補助線描画
		line_draw_step = 0;
		int OnBackBeat = 0;//裏拍のとき1

		while (1) {//上
			DrawLine(lane[0] - 64, int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw),
				     lane[3] + 64, int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw), GetColor(0, 0, 0));

			if (OnBackBeat==0) {
				line_draw_step += int(((double)*SwingBackBeatTime / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);
			}
			else {
				line_draw_step += int(((SWING_DIVISION - (double)*SwingBackBeatTime) / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);
			}
			OnBackBeat = (OnBackBeat + 1) % 2;
			if (int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw) < 0)break;
		}
		line_draw_step = 0;
		OnBackBeat = 0;//裏拍のとき1

		while (1) {//下
			DrawLine(lane[0] - 64, int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw),
				     lane[3] + 64, int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw), GetColor(0, 0, 0));

			if (OnBackBeat == 1) {
				line_draw_step -= int(((double)*SwingBackBeatTime / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);
			}
			else {
				line_draw_step -= int(((SWING_DIVISION - (double)*SwingBackBeatTime) / ((double)SWING_DIVISION / 2)) * step_array[*step_array_number] + 0.5);
			}
			OnBackBeat = (OnBackBeat + 1) % 2;
			if (int(judge_area - (line_draw_step - step_count_draw + step_count)*scale*scale_score_draw) > 720)break;
		}



		for (i = 0; i <= 3; i++) {//判定枠
			DrawExtendGraph(lane[i] - 64, judge_area - 64, lane[i] + 64, judge_area + 64, H_JUDGE_AREA, TRUE);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 85);
		//選択範囲表示
		DrawBox(lane[0] - 64,
			int(judge_area - (area.start - step_count_draw) * scale * scale_score_draw - 2.5),
			lane[3] + 64,
			int(judge_area - (area.end - step_count_draw) * scale * scale_score_draw + 3.5),
			GetColor(255, 0, 0), TRUE);

		//処理対象領域表示
		if (area.StartCell != NULL && area.EndCell != NULL) {
			DrawBox(lane[0] - 64,
				int(judge_area - (area.StartCell->step - step_count_draw) * scale * scale_score_draw - 2.5),
				lane[3] + 64,
				int(judge_area - (area.EndCell->step - step_count_draw) * scale * scale_score_draw + 3.5),
				GetColor(0, 0, 255), TRUE);
		}


		//貼り付け範囲表示
		if (CopyHead.next != NULL) {
			DrawBox(lane[0] - 64,
				int(judge_area - (step_count - step_count_draw) * scale * scale_score_draw - 2.5),
				lane[3] + 64,
				int(judge_area - (step_count + PasteRange - step_count_draw) * scale * scale_score_draw + 3.5),
				GetColor(0, 255, 0), TRUE);
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);


		//音符描画
		show = end;
		while (show->step != -1) {//endからheadまで描画
			if (show->content == CONTENTS_NOTE) {
				for (i = 0; i <= 3; i++) {
					if (show->data.note.color[i] != 0) {//音符があったら
						if (show->data.note.group[i] == 0) {//単ノート
							if (show->data.note.bright[i] == NOTE_NOT_BRIGHT) {
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[show->data.note.color[i]], TRUE);
							}
							if (show->data.note.bright[i] == NOTE_BRIGHT) {
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[show->data.note.color[i]], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
							}
						}
						if (show->data.note.group[i] == 1) {//LN始点
							if (show->data.note.bright[i] == NOTE_NOT_BRIGHT) {
								/*
								DrawExtendGraph(lane[i] - 64 ,
									judge_area - (show->step - step_count)*scale - 64,
									lane[i] + 64,
									judge_area - (show->step - step_count)*scale + 64,
									H_NOTE[show->data.note.color[i]], TRUE);
									*/

								DrawRectExtendGraph(
									lane[i] - 64, judge_area - int((show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64),
									0, 128, 256, 128,
									H_NOTE[show->data.note.color[i]], TRUE
								);

							}
							if (show->data.note.bright[i] == NOTE_BRIGHT) {
								/*
								DrawExtendGraph(lane[i] - 64 - 32, judge_area - (show->step - step_count)*scale - 64, lane[i] + 64, judge_area - (show->step - step_count)*scale + 64, H_NOTE[show->data.note.color[i]], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
								DrawExtendGraph(lane[i] - 64 - 32, judge_area - (show->step - step_count)*scale - 64, lane[i] + 64, judge_area - (show->step - step_count)*scale + 64, H_NOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
								*/

								DrawRectExtendGraph(
									lane[i] - 64, judge_area - int((show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5),
									0, 128, 256, 128,
									H_NOTE[show->data.note.color[i]], TRUE
								);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5),
									0, 128, 256, 128,
									H_NOTE[10], TRUE
								);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
							}

						}
						if (show->data.note.group[i] == 2) {//LN終点

							//DrawExtendGraph(lane[i] - 64 + 32, judge_area - (show->step - step_count)*scale - 64, lane[i] + 64, judge_area - (show->step - step_count)*scale + 64, H_NOTE[show->data.note.color[i]], TRUE);

							DrawRectExtendGraph(
								lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
								0, 0, 256, 128,
								show->data.note.LN_k[i] == 0 ? H_NOTE[show->data.note.color[i]] : H_NOTE[8],//黒終点なら上半分は黒音符表示
								TRUE
							);

							DrawRectExtendGraph(//LN中間表示
								lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (score_cell_find_before_note(show, i)->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
								0, 0, 256, 256,
								H_LNOTE[show->data.note.color[i]], TRUE
							);

							if (show->data.note.bright[i] == NOTE_BRIGHT) {//光る音符の場合
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 128,
									H_NOTE[10], TRUE
								);
								DrawRectExtendGraph(//LN中間表示
									lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (score_cell_find_before_note(show, i)->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256,
									H_LNOTE[10], TRUE
								);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
							}

							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);//黒終端ロングノートのグラデーション表示
							if (show->data.note.LN_k[i] == 1 && option->op.blackGradation == option->OP_BLACK_GRADATION_ON) {
								DrawRectExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (score_cell_find_before_note(show, i)->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256, H_LNOTE[8], TRUE);
							}

						}
					}
				}
			}
			
			show = show->before;
		}




		//命令描画
		//下に命令種類描画

		DrawLine(229, 0, 229, 720, GetColor(0, 0, 0));
		DrawLine(342, 0, 342, 720, GetColor(0, 0, 0));
		DrawLine(455, 0, 455, 720, GetColor(0, 0, 0));
		DrawLine(824, 0, 824, 720, GetColor(0, 0, 0));
		DrawLine(937, 0, 937, 720, GetColor(0, 0, 0));
		DrawLine(1050, 0, 1050, 720, GetColor(0, 0, 0));
		DrawLine(1163, 0, 1163, 720, GetColor(0, 0, 0));

		sprintfDx(command_show_str, L"MEASURE");
		DrawString(123, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"STOPSTEP");
		DrawString(237, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"STOP");
		DrawString(375, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"BPM");
		DrawString(860, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"LETBPM");
		DrawString(958, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"SC");
		DrawString(1095, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"SCBPM");
		DrawString(1188, 700, command_show_str, GetColor(255, 255, 255));


		show = end;
		while (show->step != -1) {//endからheadまで描画
			if (show->content == CONTENTS_COMMAND) {//命令の時
				if (show->data.command.kind == COMMAND_KIND_HS) {
					sprintfDx(command_show_str, L"HS");
					DrawString(472, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));

					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(lane[0] - 21, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[1]);
					DrawString(lane[1] - 24, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[2]);
					DrawString(lane[2] - 24, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[3]);
					DrawString(lane[3] - 24, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));


				}
				if (show->data.command.kind == COMMAND_KIND_STOP) {
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(365, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_STOPSTEP) {
					sprintfDx(command_show_str, L"%d/%d", int(show->data.command.value[0]), int(show->data.command.value[1]));
					DrawString(266, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_MEASURE) {
					sprintfDx(command_show_str, L"%d/%d", int(show->data.command.value[0]), int(show->data.command.value[1]));
					DrawString(150, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_BPM) {
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(845, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_LETBPM) {
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(960, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_SCROLL) {
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(1085, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
				if (show->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
					sprintfDx(command_show_str, L"%.2f", show->data.command.value[0]);
					DrawString(1188, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), command_show_str, GetColor(255, 255, 255));
				}
			}
			else if (show->content == CONTENTS_END) {//#ENDのとき
				DrawString(600, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5 - 8), L"#END", GetColor(255, 255, 255));
			}
			show = show->before;
		}

		if(isInstructionAppear)notesMode.draw(DX_SCREEN_BACK);


		
		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000 / config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		/*
		printfDx("base_bpm:%f\n", base_bpm);
		printfDx("scroll:%f\n", scroll);
		printfDx("bpm:%f\n", bpm);
		printfDx("insert_passed_time:%f\n", insert_passed_time);
		*/

		if (saveErrorTextShowFlag) {
			printfDx(L"保存できません　別のプログラムでnpsファイルが開かれている可能性があります\n");
		}

		if (MusicSub->editable[difficulty] != 1 && isInstructionAppear) {
			setPrintColorDx(GetColor(255, 0, 0), GetColor(0, 0, 0)); 
			printfDx(L"この譜面は編集できません\n");
			printfDx(L"編集するには #EDITABLE:1 を\n");
			printfDx(L"npsファイルに追加してください\n");
			printfDx(L"\n");
			setPrintColorDx(GetColor(255, 255, 255), GetColor(0, 0, 0));
		}

		if (isInstructionAppear) {
			printfDx(L"F1:操作説明を非表示\n");

			printfDx(L"F11:保存せず戻る return\n");
			printfDx(L"ESC:保存して戻る SAVE & return\n");

			printfDx(L"\n");
			printfDx(L"↑↓:移動\n");
			printfDx(L"Shift+↑↓:範囲選択\n");
			printfDx(L"Ctrl+↑↓:拡大縮小 %f\n", *scale_score);
			printfDx(L"←→:移動幅変更\n");
			printfDx(L"note value:1/%d\n", step_array_show[*step_array_number]);
			printfDx(L"Ctrl+←→:スウィング比変更\n");
			printfDx(L"1");
			for (i = 0; i < *SwingBackBeatTime - 1; i++) {
				printfDx(L"0");
			}
			printfDx(L"1");
			for (i = 0; i < SWING_DIVISION - *SwingBackBeatTime - 1; i++) {
				printfDx(L"0");
			}
			printfDx(L"\n");


			printfDx(L"\n");


			printfDx(L"step:%d\n", step_array[*step_array_number]);
			printfDx(L"step_count:%d\n", step_count);

			printfDx(L"\n");

			if (command_put == 0) {

				if (note_kind == 0) {
					printfDx(L"RTYU:青音符\n");
					printfDx(L"FGHJ:緑音符\n");
					printfDx(L"VBNM:赤音符\n");

					if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {
						printfDx(L"SHIFT:光る音符ON\n");
					}
					else {
						printfDx(L"SHIFT:光る音符OFF\n");
					}
				}
				else if (note_kind == 1) {
					if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {
						printfDx(L"RTYU:ロングノート黒終端\n");
					}
					else {
						printfDx(L"RTYU:ロングノート終端\n");
					}

					printfDx(L"FGHJ:虹音符\n");
					printfDx(L"VBNM:黒音符\n");

					if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {
						printfDx(L"SHIFT:光る音符,LN黒終端ON\n");
					}
					else {
						printfDx(L"SHIFT:光る音符,LN黒終端OFF\n");
					}
				}



				printfDx(L"SPACE:音符種類変更\n");

			}
			else {//命令を置くモード
				printfDx(L"RTYU:値を増やす\n");
				printfDx(L"FGHJ:命令を置く\n");
				printfDx(L"VBNM:値を減らす\n");
				printfDx(L"Alt押しながらで値を細かく調整");
				printfDx(L"\n");
				printfDx(L"\n");
			}

			printfDx(L"\n");

			if (command_put == 0) {

				if (area.StartCell != NULL && area.EndCell != NULL) {
					printfDx(L"BackSpace:選択範囲の音符を消去\n");
				}
				else {
					printfDx(L"BackSpace:音符を消去\n");
				}
			}
			else {
				printfDx(L"BackSpace:選択した命令を消去\n");
			}

			printfDx(L"Q:グリッド位置変更\n");

			/*
			if (AutoMove == 0) {
				printfDx(L"A:AutoMove OFF\n");
			}
			else if (AutoMove == 1) {
				printfDx(L"A:AutoMove ON\n");
			}
			*/
			printfDx(L"A:上の音符,命令に移動\n");
			printfDx(L"Z:下の音符,命令に移動\n");

			if (area.StartCell != NULL && area.EndCell != NULL) {
				printfDx(L"Ctrl+M:選択範囲(赤)を反転\n");
			}
			else {
				printfDx(L"\n");
			}
			if (area.StartCell != NULL && area.EndCell != NULL) {
				printfDx(L"Ctrl+C:コピー対象範囲(青)をコピー\n");
			}
			else {
				printfDx(L"\n");
			}
			if (CopyHead.next != NULL) {
				printfDx(L"Ctrl+V:コピー範囲を緑領域に貼り付け\n");
			}
			else {
				printfDx(L"\n");
			}

			printfDx(L"\n");
			printfDx(L"数字キーで配置モード変更\n");
			if (command_put == 0) {
				printfDx(L"配置モード:#NOTE\n");
			}
			else if (command_put == COMMAND_KIND_STOPSTEP) {
				printfDx(L"配置モード:#STOPSTEP\n");
			}
			else if (command_put == COMMAND_KIND_STOP) {
				printfDx(L"配置モード:#STOP\n");
			}
			else if (command_put == COMMAND_KIND_HS) {
				printfDx(L"配置モード:#HS\n");
			}
			else if (command_put == COMMAND_KIND_BPM) {
				printfDx(L"配置モード:#BPM\n");
			}
			else if (command_put == COMMAND_KIND_LETBPM) {
				printfDx(L"配置モード:#LETBPM\n");
			}
			else if (command_put == COMMAND_KIND_SCROLL) {
				printfDx(L"配置モード:#SCROLL\n");
			}
			else if (command_put == COMMAND_KIND_SCROLL_BPM) {
				printfDx(L"配置モード:#SCROLL_BPM\n");
			}
			else if (command_put == COMMAND_KIND_MEASURE) {
				printfDx(L"配置モード:#MEASURE\n");
			}

			printfDx(L"0:#NOTE\n");
			printfDx(L"1:#MEASURE\n");
			printfDx(L"2:#STOPSTEP\n");
			printfDx(L"3:#STOP\n");
			printfDx(L"4:#HS\n");
			printfDx(L"5:#BPM\n");
			printfDx(L"6:#LETBPM\n");
			printfDx(L"7:#SCROLL\n");
			printfDx(L"8:#SCROLLBPM\n");
		}
		else {
			printfDx(L"F1:操作説明を表示\n");
		}

		



		//printfDx(L"start:%d\n", area.start);
		//printfDx(L"end:%d\n", area.end);
		//printfDx(L"insert:%d\n", insert->step);


		ScreenFlip();//画面の反映
		clsDx();

	}

	//メモリ解放

	//画像データ
	for (i = 0; i < 12; i++) {
		DeleteGraph(H_NOTE[i]);
		DeleteGraph(H_LNOTE[i]);
	}
	DeleteGraph(H_JUDGE_AREA);
	DeleteGraph(H_BG);

	//音声データ
	DeleteSoundMem(SH.SH_HIT_N);
	DeleteSoundMem(SH.SH_HIT_L);
	DeleteSoundMem(SH.SH_HIT_S);
	DeleteSoundMem(SH.SH_HIT_R_N);
	DeleteSoundMem(SH.SH_HIT_R_L);
	DeleteSoundMem(SH.SH_HIT_R_S);
	DeleteSoundMem(SH.SH_HIT_G_N);
	DeleteSoundMem(SH.SH_HIT_G_L);
	DeleteSoundMem(SH.SH_HIT_G_S);
	DeleteSoundMem(SH.SH_HIT_B_N);
	DeleteSoundMem(SH.SH_HIT_B_L);
	DeleteSoundMem(SH.SH_HIT_B_S);
	DeleteSoundMem(SH.SH_HIT_K);
	DeleteSoundMem(SH_STEP_CHANGE);
	DeleteSoundMem(SH_STEP_INC_DEC);
	DeleteSoundMem(SH_SAVE);
	DeleteSoundMem(SH_NO);
	DeleteSoundMem(SH_SHUTTER);
	DeleteSoundMem(SH_SHUTTER_SIGNAL);

	return;
}

int SAVE_EDIT_SCORE(SCORE_CELL *head, Song *Music, int song_number, int difficulty,double scroll) {//譜面データをテキストにして保存
	int row_buf[3072];//1小節分のタイミング差分格納
	
	double measure = 1;//拍子
	int DN = EDITSTEP;//一小節の分割数

	int measure_min_DN = 1;//現在の小節を表すのに必要な最小分割数

	double step = 0;//現在のステップ数

	int measure_step = 0;//現在の小節の先頭ステップ
	int measure_put_step = int(measure*EDITSTEP);//次の小節の先頭位置

	SCORE_CELL *score_cell_p = head;
	SCORE_CELL *measure_p = head;//現在の小節の先頭セルを指すポインタ
	int buf_number = 0;
	int i = 0;

	int base_fp = 0;//読み込む元ファイル


	FILE *fp = 0;//書き込むファイル
	errno_t error = 0;
	wchar_t filename_save[256];//一時保存ファイル名

	wchar_t filename_save_final[256];//保存ファイル名

	wchar_t filename_read[256];//保存ファイル名

	wchar_t filename_backup[256];//バックアップファイル名


	for (i = 0; i <= 255; i++) {//初期化
		filename_save[i] = L'\0';
		filename_save_final[i] = L'\0';
		filename_read[i] = L'\0';
		filename_backup[i] = L'\0';
	}

	wchar_t str[256] = L"\0";

	time_t now;//現在時刻(グリニッジ標準時)
	struct tm tm_now;//時間構造体

	now = time(NULL);//時刻取得
	error= _localtime64_s(&tm_now,&now);
	
	sprintfDx(filename_save, L"%s/%d.temp", Music[song_number].FolderPath, difficulty);//一時保存ファイル名決定
	error = _wfopen_s(&fp, filename_save, L"w,ccs=UNICODE");
	//rewind(fp);//BOMを消すためにファイルポインタを先頭に

	sprintfDx(filename_save_final, L"%s/%d.nps", Music[song_number].FolderPath, difficulty);//保存ファイル名決定

	sprintfDx(filename_read, L"%s/%d.nps", Music[song_number].FolderPath, difficulty);//読み込みファイル名決定
	base_fp = FileRead_open(filename_read);
	FileRead_seek(base_fp, 2, SEEK_SET);//BOM飛ばし

	sprintfDx(filename_backup, L"%s/%d_backup_%d_%d_%d_%d_%d_%d", Music[song_number].FolderPath, difficulty,
		tm_now.tm_year+1900, tm_now.tm_mon+1, tm_now.tm_mday, tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);//バックアップファイル名決定

	CopyFile(filename_read, filename_backup, FALSE);//バックアップファイル作成

	do {
		FileRead_gets(str, 256, base_fp);
		fwprintf_s(fp, L"%s\n", str);
	} while (wcscmp(L"#NOTES:", str) != 0);//ヘッダは元ファイルからコピー(#NOTES:のとこまで)	

	score_cell_p = score_cell_p->next;//データの一番先頭を指すようにする
	while (score_cell_p->content != CONTENTS_END) {//終端まで
		
		//1小節の最小単位を検出
		if (score_cell_p->step != measure_step) {//小節の先頭にセルが無い時はダミーを挿入
			score_insert_cell(score_cell_p->before, measure_step);
			score_cell_p = score_cell_p->before;
			score_cell_write_command(score_cell_p, CONTENTS_KIND_DUMMY);
		}
		measure_p = score_cell_p;

		buf_number= 0;//バッファの格納個数
		BOOL isNextMeasure = 0;
		while (measure_put_step > score_cell_p->step && isNextMeasure == 0 && score_cell_p->next->content != CONTENTS_END_EDGE) {//1小節分
			if (score_cell_p->content == CONTENTS_COMMAND) {//命令のMEASUREあったら
				if (score_cell_p->data.command.kind == COMMAND_KIND_MEASURE) {
					//fprintf_s(fp, "#MEASURE:%d/%d\n", (int)score_cell_p->data.command.value[0], (int)score_cell_p->data.command.value[1]);

					measure = (double)score_cell_p->data.command.value[0] / (double)score_cell_p->data.command.value[1];
					DN = int(measure*EDITSTEP+0.5);
					measure_put_step = measure_step + int(DN);
				}
			}

			//小節内に小節線変更コマンドがあるかどうか
			if (score_cell_p->next->content == CONTENTS_COMMAND && score_cell_p->next->step != measure_p->step) {
				if (score_cell_p->next->data.command.kind == COMMAND_KIND_MEASURE) {
					isNextMeasure = 1;

					//小節線を変更
					int measureLength = score_cell_p->next->step - measure_step;

					if (measureLength < DN) {//次の#MEASUREコマンドまでの長さが現在の小節長より短い場合に小節の先頭に小節線コマンド変更
						measure = (double)measureLength / EDITSTEP;
						DN = measureLength;
						measure_put_step = measure_step + int(DN);

						int gcdVal = gcd(measureLength, EDITSTEP);
						int numerator = measureLength / gcdVal;
						int denominator = EDITSTEP / gcdVal;

						SCORE_CELL* measure_command = score_cell_find_command_next(measure_p, COMMAND_KIND_MEASURE);
						if (measure_command == NULL) {//既に小節線変更コマンドが無ければ挿入
							SCORE_CELL* insert = measure_p->before;
							score_insert_cell(insert, measure_p->step);
							measure_p = insert->next;
							score_cell_write_command(measure_p, COMMAND_KIND_MEASURE, numerator, denominator, 0, 0);
							measure_command = measure_p;
						}
						else {
							measure_command->data.command.value[0] = numerator;
							measure_command->data.command.value[1] = denominator;
						}
					}
				}
			}

			if (score_cell_p->next->step >= measure_put_step || isNextMeasure) {//小節内の最後の音符(または小節内に小節変更命令があった)のとき　　音符間隔の値は"音符または命令"と次の小節の先頭との差分とする
				if (buf_number == 0) {//最後で最初の音符 小節に1つしかない
					if (score_cell_p->step - measure_step != 0) {
						row_buf[buf_number] = score_cell_p->step - measure_step;//バッファにタイミング差分格納(0なら入れない)
						buf_number++;
					}
				}

				row_buf[buf_number] = measure_put_step - score_cell_p->step;//バッファに(次の小節頭-今の行)のタイミング差分格納(0なら入れない)
				buf_number++;
			
			}
			else {//この小節の最後ではない
				if (buf_number == 0) {
					if (score_cell_p->step - measure_step != 0) {//この小節の最初の音符、命令が小節の先頭に無いとき
						row_buf[buf_number] = score_cell_p->step - measure_step;//バッファにタイミング差分格納(0なら入れない)
						buf_number++;
					}
				}
				if (score_cell_p->next->step - score_cell_p->step != 0) {
					row_buf[buf_number] = score_cell_p->next->step - score_cell_p->step;//バッファに(1つ下の行-今の行)のタイミング差分格納(0なら入れない)
					buf_number++;
				}
			}

			if (score_cell_p->next->content != CONTENTS_END_EDGE) {//次に音符、または#ENDがある
				score_cell_p = score_cell_p->next;//1つ進める
			}
			else {
				break;//最後の音符、命令まで到達
			}

		}

		/*
		if (score_cell_p->next->content == 3 && measure_put_step > score_cell_p->step) {//最後の音符は次の小節の先頭との差分をとる
			row_buf[buf_number] = measure_put_step - score_cell_p->step;//バッファに(1つ下の行-今の行)のタイミング差分格納(0なら入れない)
			buf_number++;

			score_cell_p = score_cell_p->next;//1つ進め#ENDに到達
		}
		*/

		int DN_gcd = row_buf[0];//1小節の最小単位
		for (i = 0; i <= buf_number - 1; i++) {
			DN_gcd = gcd(DN_gcd, row_buf[i]);//最大公約数を入れる
		}

		if (buf_number == 0)DN_gcd = DN;//何もない小節の時

		int end = 0;
		int lflag = 0;//改行フラグ
		int note_put_flag = 0;//小節内に音符を置いたかどうかのフラグ
		//1小節分書き込む
		while (measure_put_step > (int)step && end == 0) {//
			if (int(step) >= measure_p->step) {//今のステップに音符、命令がある
				lflag = 0;
				while (measure_p->content == CONTENTS_COMMAND && int(step) >= measure_p->step) {//命令を先に書き込む
					if (measure_p->data.command.kind == COMMAND_KIND_MEASURE) {
						fwprintf_s(fp, L"#MEASURE:%d/%d\n", (int)measure_p->data.command.value[0], (int)measure_p->data.command.value[1]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_BPM) {
						fwprintf_s(fp, L"#BPM:%.3f\n", measure_p->data.command.value[0]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_LETBPM) {
						fwprintf_s(fp, L"#LETBPM:%.3f\n", measure_p->data.command.value[0]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_SCROLL) {
						fwprintf_s(fp, L"#SCROLL:%.3f\n", measure_p->data.command.value[0]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
						fwprintf_s(fp, L"#SCROLLBPM:%.3f\n", measure_p->data.command.value[0]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_STOP) {
						fwprintf_s(fp, L"#STOP:%f\n", measure_p->data.command.value[0]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_STOPSTEP) {
						fwprintf_s(fp, L"#STOPSTEP:%d/%d\n", (int)measure_p->data.command.value[0], (int)measure_p->data.command.value[1]);
					}
					if (measure_p->data.command.kind == COMMAND_KIND_HS) {
						fwprintf_s(fp, L"#HS:%.3f,%.3f,%.3f,%.3f\n", measure_p->data.command.value[0],
							measure_p->data.command.value[1],
							measure_p->data.command.value[2],
							measure_p->data.command.value[3]);
					}
					lflag = 0;
					if (measure_p->next != NULL) {//次がNULLじゃない
						measure_p = measure_p->next;
					}
					else {
						end = 1;
						break;
					}
				}

				if (measure_p->content == CONTENTS_NOTE && int(step) >= measure_p->step) {//音符
					fwprintf_s(fp, L"%c%c%c%c",
						note_to_char(measure_p->data.note.color[0], measure_p->data.note.group[0], measure_p->data.note.bright[0], measure_p->data.note.LN_k[0]),
						note_to_char(measure_p->data.note.color[1], measure_p->data.note.group[1], measure_p->data.note.bright[1], measure_p->data.note.LN_k[1]),
						note_to_char(measure_p->data.note.color[2], measure_p->data.note.group[2], measure_p->data.note.bright[2], measure_p->data.note.LN_k[2]),
						note_to_char(measure_p->data.note.color[3], measure_p->data.note.group[3], measure_p->data.note.bright[3], measure_p->data.note.LN_k[3])
					);
					if (measure_p->next != NULL) {//次がNULLじゃない
						measure_p = measure_p->next;
					}
					else {
						end = 1;
						break;
					}
					step += DN_gcd;
					lflag = 1;
				}else if (measure_p->content == CONTENTS_END && int(step) >= measure_p->step) {//#END

					end = 1;
					break;
				}
				
			}
			else {//今のステップに音符、命令が無い
				fwprintf_s(fp, L"0");
				step += DN_gcd;
				lflag = 1;
			}

			
			
			if ((int)step >= measure_put_step) {//小節区切りカンマを付ける
				fwprintf_s(fp, L",");
				fwprintf_s(fp, L"\n");
				measure_put_step = int(step + DN);
				break;
			}

			if (lflag==1)fwprintf_s(fp, L"\n");//このステップの処理で命令しか置いてないときは改行書き込まない

		}

		measure_step = int(step);//次の小節へ

	}
	
	//score_cell_pは#ENDを指している
	//最後の音符、命令以降にまだ空白の小節があるとき(#ENDがすぐにないとき)「0,」で埋める
	while (step < score_cell_p->step) {
		fwprintf_s(fp, L"0,\n");
		step += DN;//1小節進める
	}
	fwprintf_s(fp, L"#END");

	fclose(fp);

	FileRead_close(base_fp);

	if (_wremove(filename_read) != 0) {//元ファイル削除 (開かれている場合はエラーを返す)
		_wremove(filename_save);//一時保存ファイルを削除
		return 1;
	}
	_wrename(filename_save, filename_save_final);//一時保存ファイルを.npsにリネーム

	return 0;
}

wchar_t note_to_char(char color,char group,BOOL bright,BOOL LN_k) {//音符の数値情報を譜面データ文字に変換
	wchar_t color_list[11] = { L'0',L'r',L'g',L'b',L'y',L'c',L'm',L'w',L'k',L'f',L'e' };
	wchar_t color_list_CAP[11] = { NULL,L'R',L'G',L'B',L'Y',L'C',L'M',L'W',NULL,L'F',L'E' };
	if (group == NOTE_GROUP_LN_END) {
		return LN_k==0? color_list[10] : color_list_CAP[10];
	}

	if (bright == NOTE_NOT_BRIGHT) {
		return color_list[color];
	}
	else if (bright == NOTE_BRIGHT) {
		return color_list_CAP[color];
	}

	return L'0';
}

int gcd(int n, int m) {//最大公約数
	int i, r;

	if (n<m) {//mを小さい方にする
		i = n;
		n = m;
		m = i;
	}

	while (m != 0) {
		r = n%m;
		n = m;
		m = r;
	}
	return n;
}

double calc_insert_passed_time(double init_bpm, double init_scroll, SCORE_CELL *p, SCORE_CELL *insert) {//headからinsertまでの実時間をmsで計測し返す関数
	double insert_passed_time = 0;//insertまでの実時間
	double time_per_step = ((double)60 / init_bpm) / ((double)EDITSTEP / 4) * 1000;//1stepあたりに進む経過時間(ms)
	double base_bpm = init_bpm;//基本BPM
	double scroll = init_scroll;
	double bpm = base_bpm*scroll;//今の実質のBPM

	while (p->next->content != CONTENTS_END && p->next->step <= insert->step) {//pの次の音符、命令がstep_countより下に行ったのでpを次へ
		p = p->next;

		insert_passed_time += time_per_step*(p->step - p->before->step);//次の音符、命令まで移動した分実時間足す
		if (p->content == CONTENTS_COMMAND) {
			if (p->data.command.kind == COMMAND_KIND_BPM) {
				base_bpm = p->data.command.value[0];
				bpm = base_bpm*scroll;
			}
			if (p->data.command.kind == COMMAND_KIND_LETBPM) {
				base_bpm = p->data.command.value[0];
				bpm = base_bpm;
			}
			if (p->data.command.kind == COMMAND_KIND_SCROLL) {
				scroll = p->data.command.value[0];
				bpm = base_bpm*scroll;
			}
			if (p->data.command.kind == COMMAND_KIND_SCROLL_BPM) {
				scroll = p->data.command.value[0] / base_bpm;
				bpm = base_bpm*scroll;
			}
			if (p->data.command.kind == COMMAND_KIND_STOP) {
				insert_passed_time += p->data.command.value[0] * 1000;
			}
			if (p->data.command.kind == COMMAND_KIND_STOPSTEP) {
				insert_passed_time += ((double)240 * 1000 / bpm)*(p->data.command.value[0] / p->data.command.value[1]);
			}
			time_per_step = ((double)60 / bpm) / ((double)EDITSTEP / 4) * 1000;


		}
		//#ENDには行かないようにする

	}
	return insert_passed_time;
}


void DecideProcessArea(AreaSelect* area, SCORE_CELL* end) {


	if (area->end > end->step)area->end = end->step;//endをはみ出して選択させない
	if (area->start > end->step)area->start = end->step;//endをはみ出して選択させない

	if (area->start != area->end) {//範囲に幅があることが前提
		if (area->start > area->end) {//上から下に選択した時はstartがendより小さい値になるように交換する
			int buf = area->end;
			area->end = area->start;
			area->start = buf;
		}
		area->end -= 1;
		if (area->end < 0)area->end = 0;

		//endから範囲内を探して端のセルのアドレスを格納
		SCORE_CELL* AreaSearch = end;
		//下方向
		while (1) {
			if (AreaSearch->before->step < area->start) {//範囲下端の外側に出てしまう直前
				area->StartCell = AreaSearch;
				break;
			}
			AreaSearch = AreaSearch->before;
		}

		AreaSearch = end;
		//上方向
		while (1) {
			if (AreaSearch->step <= area->end) {//範囲上端の内側に入ったとき
				area->EndCell = AreaSearch;
				break;
			}
			AreaSearch = AreaSearch->before;
		}

		//範囲内に音符が無かったらNULLにする
		if (area->EndCell->step < area->start)area->EndCell = NULL;
		if (area->StartCell->step > area->end)area->StartCell = NULL;

	}


}


void MirrorSelectArea(AreaSelect area) {//選択範囲をミラー配置にする
	SCORE_CELL *search = area.StartCell;

	note_data buf;
	note_data buf2;
	int i = 0;

	//選択範囲内の音符配置を反転
	while (1) {
		if (search->content == CONTENTS_NOTE) {
			buf = search->data.note;
			for (i = 0; i < 4; i++) {
				buf2.bright[i] = buf.bright[3 - i];
				buf2.color[i] = buf.color[3 - i];
				buf2.group[i] = buf.group[3 - i];
				buf2.LN_k[i] = buf.LN_k[3 - i];
			}
			search->data.note = buf2;
		}
		search = search->next;
		if (search->step > area.EndCell->step)break;
	}

	return;
}

void SolveLN(SCORE_CELL* end) {//LNの不整合を良い感じにする
	SCORE_CELL* search = end;
	int column = 0;

	while (1) {
		if (search->content == CONTENTS_NOTE) {
			for (column = 0; column < 4; column++) {
				if (search->data.note.group[column] == 2) {//LN終点
					if (score_cell_find_before_note(search, column) != NULL) {
						if (score_cell_find_before_note(search, column)->data.note.group[column] != 1) {
							search->data.note.group[column] = 0;
						}
					}
					else {
						search->data.note.group[column] = 0;
					}
				}
				if (search->data.note.group[column] == 1) {//LN始点
					if (score_cell_find_next_note(search, column) != NULL) {
						if (score_cell_find_next_note(search, column)->data.note.group[column] != 2) {
							search->data.note.group[column] = 0;
						}
					}
					else {
						search->data.note.group[column] = 0;
					}
				}


			}
		}

		search = search->before;
		if (search->step == -1)break;
	}

	return;
}


void CopySelectArea(int step_count, AreaSelect area, SCORE_CELL *CopyHead) {//CopyHeadに選択範囲をコピー
	SCORE_CELL* search = area.StartCell;
		
	CopyHead->step = -1;

	SCORE_CELL* copy = CopyHead;//insertに挿入することを想定したstep_countでcopyに選択範囲の音符だけをコピーする

	int column = 0;

	//areaのStartCellからEndCellまでコピー
	while (1) {

		if (search->content == CONTENTS_NOTE) {//音符の時
			score_insert_cell(copy, search->step - area.start);//area.startからの相対step値に変換してコピー
			copy = copy->next;
			//内容をコピー
			for (column = 0; column < 4; column++) {
			    score_cell_write_note(copy, column, 
					search->data.note.color[column], 
					search->data.note.group[column],
					search->data.note.bright[column],
					search->data.note.LN_k[column]);
			}
		}

		if (search == area.EndCell)break;
		search = search->next;
	};
	score_insert_cell(copy, search->step - area.start + step_count);//#ENDを入れる
	copy = copy->next;
	copy->content = CONTENTS_END;

	return;
}

void DeleteSelectArea(SCORE_CELL** insert, AreaSelect area) {//選択範囲の音符の削除
	SCORE_CELL* search = *insert;

	if ((*insert)->step >= area.StartCell->step && (*insert)->step <= area.EndCell->step) {//insertが削除範囲内にあるときinsertを避難
		//範囲内でinsertより下のコマンドを見つけてそれをinsertとする
		while (1) {
			if (search->content == CONTENTS_COMMAND) {
				*insert = search;
				break;
			}
			else {
				search = search->before;
			}

			if (search->step < area.start) {//無かったら下側範囲外の一番上にあるセルをinsertとする
				*insert = search;
				break;
			}

		}
	}

	//選択範囲の削除
	int column = 0;

	search =area.StartCell;

	if (search->content != CONTENTS_END) {//最初から#END指していたら何も消さない
		//そこから範囲内の音符削除
		while (1) {
			if (search->content == 1) {
				for (column = 0; column < 4; column++) {
					score_cell_write_note(search, column, 0, 0, 0);//消す
				}
				score_cell_delete_if_no_note(&search);//その結果何もない行になったら行を消す
			}

			search = search->next;
			if (search->step >= area.end || search->content == CONTENTS_END)break;//削除範囲を超えた または#ENDに到達した

		}
	}


}

void PasteSelectArea(int step_count, SCORE_CELL** insert, int PasteRange, SCORE_CELL* CopyHead, double scale_m) {//CopyHeadをstep_countを基準にinsertに貼り付け
	SCORE_CELL* search = *insert;


    //一時コピー先の先頭
	SCORE_CELL* copy = CopyHead;//insertに挿入することを想定したstep_countでcopyに選択範囲の音符だけをコピーする

	int column = 0;

	




	//選択した範囲と同じ範囲をstep_countから考えてそこを貼り付け領域として、その領域内の音符だけを削除
	search = *insert;
	while (search->step < step_count) {//searchをstep_countより上か同じを指すようにする(#ENDは超えないようにする)
		search = search->next;
		if (search->content == CONTENTS_END)break;
	}

	SCORE_CELL* insert_buf = search->before;//現在のinsertセルが消える場合もあるので削除する直前のセルを記憶

	if (search->content != CONTENTS_END) {//最初から#END指していたら何も消さない
		//そこから範囲内の音符削除
		while (1) {
			if (search->content == 1) {
				for (column = 0; column < 4; column++) {
					score_cell_write_note(search, column, 0, 0, 0);//消す
				}
				score_cell_delete_if_no_note(&search);//その結果何もない行になったら行を消す
			}


			search = search->next;
			if ((search->step >= step_count + PasteRange) || search->content == CONTENTS_END)break;//step_countからの貼り付け範囲を超えた または#ENDに到達した

		}
	}

	//insertは消えたセルを指している場合もある
	*insert = insert_buf;//削除する直前のセルを挿入


	//insertにcopyを挿入
	search = *insert;
	while (search->step < step_count) {//searchをstep_countより上か同じを指すようにする(#ENDは超えないようにする)
		search = search->next;
		if (search->content == CONTENTS_END)break;
	}
	search = search->before;//step_countの直前のセルを指すようにする

	copy = CopyHead->next;

	//search: 0   0    00     0
	//copy  : 0 0 0 00  0  0    0 

	//そこから挿入
	while (1) {

		if (copy->content == CONTENTS_END)break;//copyが最後までいったので終了



		if (search->next->step > copy->step + step_count) {//searchの次の命令,範囲外の音符までの間にcopyを挟められるとき

			score_insert_cell(search, copy->step + step_count);//searchにcopyのタイミング情報だけのセルを挿入
			search = search->next;

			//内容を格納
			for (column = 0; column < 4; column++) {
				score_cell_write_note(search, column,
					copy->data.note.color[column],
					copy->data.note.group[column],
					copy->data.note.bright[column],
					copy->data.note.LN_k[column]);
			}

			copy = copy->next;//コピー対象を一つ進める

		}
		else {
			if (search->next->content == CONTENTS_END) {//searchの次が#ENDに到達してたらsearchを進める代わりに#ENDのstepを後ろにずらす
				search->next->step += int(EDITSTEP * scale_m);
			}
			else {
				search = search->next;
			}
		}

	}

	if ((*insert)->next->step == step_count && (*insert)->next->content != CONTENTS_END) {//step_countの位置にセルがあったらinsertはそれを指すようにする(#ENDにはならないようにする)
		*insert = (*insert)->next;

	}

	return;
}