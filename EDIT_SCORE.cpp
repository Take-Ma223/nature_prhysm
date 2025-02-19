﻿#include"DxLib.h"
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
#include "DxLibUtil.h"
#include "NPLoadSoundMem.h"
#include "GradationNoteImage.h"

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

	int H_GLN;//画像データのハンドル


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
	wchar_t *ReadNameRGB[11] = { L"r",L"g",L"b",L"c",L"m",L"y",L"w",L"k",L"f",
		L"bright",L"note_Long_hit_b" };

	GradationNoteImage gradationLongNote(wstring(L"img/notes/") + wstring(option->op.note.toString()));


	auto note_folder = option->op.note.toString();
	for (i = 0; i <= 10; i++) {
		wchar_t strcash[128];
		sprintfDx(strcash, L"img/notes/%s/%s.png", note_folder.c_str(), ReadNameRGB[i]);
		H_NOTE[i + 1] = LoadGraph(strcash);

		wchar_t strcash_l[128];
		sprintfDx(strcash_l, L"img/notes/%s/%sl.png", note_folder.c_str(), ReadNameRGB[i]);
		H_LNOTE[i + 1] = LoadGraph(strcash_l);
	}
	wchar_t strcash[128];
	sprintfDx(strcash, L"img/notes/%s/JudgeArea.png", note_folder.c_str());
	H_JUDGE_AREA = LoadGraph(strcash);


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

	H_GLN = LoadGraph(L"img/edit/gln.png");

	int digit[5] = { 0,0,0,0,0 };

	wchar_t command_show_str[100];


	/*
	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_STOP_NUMBER);

	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_HS_NUMBER);

	LoadDivGraph(L"img/ResultPopNumber.png", 10, 10, 1, 25, 50, H_BPM_NUMBER);
	LoadDivGraph(L"img/ResultMaxComboNumber.png", 10, 10, 1, 25, 50, H_SCROLL_NUMBER);
	*/

	
	auto hit_sound_folder = option->op.hitSound.toString();
	sprintfDx(strcash, L"sound/hit_sound/%s/f2.wav", hit_sound_folder.c_str());
	SH.SH_HIT_N = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f3.wav", hit_sound_folder.c_str());
	SH.SH_HIT_L = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/f1.wav", hit_sound_folder.c_str());
	SH.SH_HIT_S = NPLoadHitSoundMem(strcash, option, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/r2.wav", hit_sound_folder.c_str());
	SH.SH_HIT_R_N = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r3.wav", hit_sound_folder.c_str());
	SH.SH_HIT_R_L = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/r1.wav", hit_sound_folder.c_str());
	SH.SH_HIT_R_S = NPLoadHitSoundMem(strcash, option, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/g2.wav", hit_sound_folder.c_str());
	SH.SH_HIT_G_N = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g3.wav", hit_sound_folder.c_str());
	SH.SH_HIT_G_L = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/g1.wav", hit_sound_folder.c_str());
	SH.SH_HIT_G_S = NPLoadHitSoundMem(strcash, option, 1);

	sprintfDx(strcash, L"sound/hit_sound/%s/b2.wav", hit_sound_folder.c_str());
	SH.SH_HIT_B_N = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b3.wav", hit_sound_folder.c_str());
	SH.SH_HIT_B_L = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/b1.wav", hit_sound_folder.c_str());
	SH.SH_HIT_B_S = NPLoadHitSoundMem(strcash, option, 1);
	sprintfDx(strcash, L"sound/hit_sound/%s/k.wav", hit_sound_folder.c_str());
	SH.SH_HIT_K = NPLoadHitSoundMem(strcash, option, 1);




	SH_STEP_CHANGE = NPLoadFxSoundMem(L"sound/difficulty_select.wav", option);
	SH_STEP_INC_DEC = NPLoadFxSoundMem(L"sound/song_select.wav", option);
	SH_SAVE = NPLoadFxSoundMem(L"sound/shutter_signal.wav", option);
	SH_NO = NPLoadFxSoundMem(L"sound/no.wav", option);
	SH_SHUTTER = NPLoadFxSoundMem(L"sound/shutter.wav", option);
	SH_SHUTTER_SIGNAL = NPLoadFxSoundMem(L"sound/shutter_signal.wav", option);

	int Keylist[4][4] = { { KEY_INPUT_V,KEY_INPUT_B,KEY_INPUT_N,KEY_INPUT_M },
	{ KEY_INPUT_F,KEY_INPUT_G,KEY_INPUT_H,KEY_INPUT_J },
	{ KEY_INPUT_R,KEY_INPUT_T,KEY_INPUT_Y,KEY_INPUT_U },
	{ KEY_INPUT_4,KEY_INPUT_5,KEY_INPUT_6,KEY_INPUT_7 } };//キーのリスト 下から順に

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->op.theme.toString());
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
	ChangeFont(config.BaseFont.c_str());
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(15);
	SetFontThickness(9);

	GAME_start_time = GetNowCount_d(config);//開始時のカウント
	while (1) {
		appContext.updateTime();
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();

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
			if (config.Editable == 1) {//編集可能
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
			SolveLN(head, end);

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
			SolveLN(head, end);

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
			auto deleteSolve = [](SCORE_CELL* before, SCORE_CELL* next, int lane) {
				NoteGroup beforeGroup = before == NULL ? NoteGroup::None : before->data.note.group[lane];
				NoteGroup nextGroup = next == NULL ? NoteGroup::None : next->data.note.group[lane];

				if (beforeGroup == NoteGroup::None && nextGroup == NoteGroup::LongNoteMiddle) {
					next->data.note.group[lane] = NoteGroup::LongNoteStart;
				}
				else if (beforeGroup == NoteGroup::None && nextGroup == NoteGroup::LongNoteEnd) {
					next->data.note.group[lane] = NoteGroup::Single;
				}
				else if (beforeGroup == NoteGroup::Single && nextGroup == NoteGroup::LongNoteMiddle) {
					next->data.note.group[lane] = NoteGroup::LongNoteStart;
				}
				else if (beforeGroup == NoteGroup::Single && nextGroup == NoteGroup::LongNoteEnd) {
					next->data.note.group[lane] = NoteGroup::Single;
				}
				else if (beforeGroup == NoteGroup::LongNoteStart && nextGroup == NoteGroup::None) {
					before->data.note.group[lane] = NoteGroup::Single;
				}
				else if (beforeGroup == NoteGroup::LongNoteStart && nextGroup == NoteGroup::Single) {
					before->data.note.group[lane] = NoteGroup::Single;
				}
				else if (beforeGroup == NoteGroup::LongNoteStart && nextGroup == NoteGroup::LongNoteStart) {
					before->data.note.group[lane] = NoteGroup::Single;
				}
				else if (beforeGroup == NoteGroup::LongNoteMiddle && nextGroup == NoteGroup::None) {
					before->data.note.group[lane] = NoteGroup::LongNoteEnd;
				}
				else if (beforeGroup == NoteGroup::LongNoteMiddle && nextGroup == NoteGroup::Single) {
					before->data.note.group[lane] = NoteGroup::LongNoteEnd;
				}
				else if (beforeGroup == NoteGroup::LongNoteMiddle && nextGroup == NoteGroup::LongNoteStart) {
					before->data.note.group[lane] = NoteGroup::LongNoteEnd;
				}
				else if (beforeGroup == NoteGroup::LongNoteEnd && nextGroup == NoteGroup::LongNoteMiddle) {
					next->data.note.group[lane] = NoteGroup::LongNoteStart;
				}
				else if (beforeGroup == NoteGroup::LongNoteEnd && nextGroup == NoteGroup::LongNoteEnd) {
					next->data.note.group[lane] = NoteGroup::Single;
				}
			};

			if (command_put == 0) {//音符配置モードのとき
				for (i = 0; i <= 2; i++) {//縦 RGB 012
					for (j = 0; j <= 3; j++) {//横 0123
						if (Key[Keylist[i][j]] == 1) {
							if (insert->content == CONTENTS_COMMAND || insert->step != step_count) {//insertが命令のセルだったりstepとinsert違うならそこに新たなセル挿入
								score_insert_cell(insert, step_count);

								insert = insert->next;

								score_cell_write_note(insert, 0, NoteColor::NONE, NoteGroup::Single, 0);//color0で初期化
								score_cell_write_note(insert, 1, NoteColor::NONE, NoteGroup::Single, 0);
								score_cell_write_note(insert, 2, NoteColor::NONE, NoteGroup::Single, 0);
								score_cell_write_note(insert, 3, NoteColor::NONE, NoteGroup::Single, 0);
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
									if (insert->data.note.color[j] == NoteColor::NONE) {//何もない
										score_cell_write_note(insert, j, NoteColor::R, NoteGroup::Single, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NoteColor::R) {
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NoteColor::G) {
										score_cell_write_note(insert, j, NoteColor::Y, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::B) {
										score_cell_write_note(insert, j, NoteColor::M, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::Y) {
										score_cell_write_note(insert, j, NoteColor::G, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::C) {
										score_cell_write_note(insert, j, NoteColor::W, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::M) {
										score_cell_write_note(insert, j, NoteColor::B, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::W) {
										score_cell_write_note(insert, j, NoteColor::C, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::F) {
										score_cell_write_note(insert, j, NoteColor::R, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::K) {
										score_cell_write_note(insert, j, NoteColor::R, insert->data.note.group[j], 0);
									}

									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_R_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_R_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}

								if (i == 1) {//Green
									if (insert->data.note.color[j] == NoteColor::NONE) {//何もない
										score_cell_write_note(insert, j, NoteColor::G, NoteGroup::Single, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NoteColor::R) {
										score_cell_write_note(insert, j, NoteColor::Y, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::G) {
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NoteColor::B) {
										score_cell_write_note(insert, j, NoteColor::C, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::Y) {
										score_cell_write_note(insert, j, NoteColor::R, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::C) {
										score_cell_write_note(insert, j, NoteColor::B, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::M) {
										score_cell_write_note(insert, j, NoteColor::W, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::W) {
										score_cell_write_note(insert, j, NoteColor::M, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::F) {
										score_cell_write_note(insert, j, NoteColor::G, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::K) {
										score_cell_write_note(insert, j, NoteColor::G, insert->data.note.group[j], 0);
									}

									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_G_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_G_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}

								if (i == 2) {//Blue
									if (insert->data.note.color[j] == NoteColor::NONE) {//何もない
										score_cell_write_note(insert, j, NoteColor::B, NoteGroup::Single, 0);
										putflag = 1;
									}
									else if (insert->data.note.color[j] == NoteColor::R) {
										score_cell_write_note(insert, j, NoteColor::M, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::G) {
										score_cell_write_note(insert, j, NoteColor::C, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::B) {
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else if (insert->data.note.color[j] == NoteColor::Y) {
										score_cell_write_note(insert, j, NoteColor::W, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::C) {
										score_cell_write_note(insert, j, NoteColor::G, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::M) {
										score_cell_write_note(insert, j, NoteColor::R, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::W) {
										score_cell_write_note(insert, j, NoteColor::Y, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::F) {
										score_cell_write_note(insert, j, NoteColor::B, insert->data.note.group[j], insert->data.note.isBright[j]);
									}
									else if (insert->data.note.color[j] == NoteColor::K) {
										score_cell_write_note(insert, j, NoteColor::B, NoteGroup::Single, 0);
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
									if (insert->data.note.color[j] == NoteColor::K) {
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
									}
									else {//黒音符を書き込む
										if (insert->data.note.group[j] == NoteGroup::LongNoteEnd) {//終点を黒で上書きしたとき
											insert->data.note.LN_k[j] = !insert->data.note.LN_k[j];//黒終端を切り替える
											PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
										}
										else {
											if (insert->data.note.group[j] == NoteGroup::LongNoteStart) {//始点を黒で上書きしたとき
												if (find_next->data.note.group[j] == NoteGroup::LongNoteMiddle) {
													find_next->data.note.group[j] = NoteGroup::LongNoteStart;
												}
												else if (find_next->data.note.group[j] == NoteGroup::LongNoteEnd) {
													score_cell_write_note(find_next, j, NoteColor::NONE, NoteGroup::Single, 0);//終点を消す
													score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
												}
												score_cell_write_note(insert, j, NoteColor::K, NoteGroup::Single, 0);//黒音符書き込み
												PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
											}
											else if (find_next != NULL && find_before != NULL) {
												//LN中に黒は置けない
												bool isInLn = 
													(find_before->data.note.group[j] == NoteGroup::LongNoteStart || find_before->data.note.group[j] == NoteGroup::LongNoteMiddle)
													|| (find_next->data.note.group[j] == NoteGroup::LongNoteMiddle || find_next->data.note.group[j] == NoteGroup::LongNoteEnd);

												if (!isInLn) {
													score_cell_write_note(insert, j, NoteColor::K, NoteGroup::Single, 0);//黒音符書き込み
													PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
												}
											}
											else {
												score_cell_write_note(insert, j, NoteColor::K, NoteGroup::Single, 0);//黒音符書き込み
												PlaySoundMem(SH.SH_HIT_K, DX_PLAYTYPE_BACK, TRUE);
											}
											
										}
									}
									
								}
								if (i == 1) {//Rainbow
									if (insert->data.note.color[j] == NoteColor::F) {//既に虹がある
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す
										putflag = 0;
									}
									else {//虹ではない
										if (insert->data.note.color[j] == NoteColor::NONE)putflag = 1;//何もないときは置いたフラグ立てる

										score_cell_write_note(insert, j, NoteColor::F, insert->data.note.group[j], insert->data.note.isBright[j]);

									}

									if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//音を鳴らす
										PlaySoundMem(SH.SH_HIT_L, DX_PLAYTYPE_BACK, TRUE);
									}
									else {
										PlaySoundMem(SH.SH_HIT_N, DX_PLAYTYPE_BACK, TRUE);
									}
								}
								if (i == 2) {//LN
									if (insert->data.note.group[j] == NoteGroup::LongNoteEnd) {//既にLN終点があったら
										score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
										score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す

										if (find_before->data.note.group[j] == NoteGroup::LongNoteStart) {
											score_cell_write_note(find_before, j, find_before->data.note.color[j], NoteGroup::Single, find_before->data.note.isBright[j]);
										}
										else if (find_before->data.note.group[j] == NoteGroup::LongNoteMiddle) {
											score_cell_write_note(find_before, j, find_before->data.note.color[j], NoteGroup::LongNoteEnd, find_before->data.note.isBright[j]);

										}
									}
									else {//今のstep_countの行にLN終点が配置されてない
										if (find_before != NULL) {//この行の下に音符があって
											if (find_before->data.note.color[j] != NoteColor::K && find_before->data.note.group[j] != NoteGroup::LongNoteEnd) {//それが黒じゃなくLN終点でもないならLN終端を置ける

												if (find_next != NULL) {//上に音符がある
													if (find_next->data.note.group[j] == NoteGroup::LongNoteEnd) {//それがLN終点なら消す
														score_cell_write_note(find_next, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
														score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
													}else if (find_next->data.note.group[j] == NoteGroup::LongNoteMiddle) {//それがLN中間なら始点にする
														find_next->data.note.group[j] = NoteGroup::LongNoteStart;
													}
												}
												if (find_before->data.note.group[j] == NoteGroup::Single) {
													score_cell_write_note(find_before, j, find_before->data.note.color[j], NoteGroup::LongNoteStart, find_before->data.note.isBright[j]);//下にある音符をLN始点にする
												}

												//終点の色について
												NoteColor end_color; 
												if (insert->data.note.color[j] == NoteColor::NONE || insert->data.note.color[j] == NoteColor::K) {
													end_color = score_cell_find_before_note(insert, j)->data.note.color[j];
												}
												else {end_color = insert->data.note.color[j];}

												if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//SHIFT押しながらだと黒終点にする
													score_cell_write_note(insert, j, end_color, NoteGroup::LongNoteEnd, score_cell_find_before_note(insert, j)->data.note.isBright[j], 1);//LN情報を書き込む(黒終点)
												}
												else {
													score_cell_write_note(insert, j, end_color, NoteGroup::LongNoteEnd, score_cell_find_before_note(insert, j)->data.note.isBright[j], 0);//LN情報を書き込む(通常終点)
												}
											}
											else if (find_before->data.note.group[j] == NoteGroup::LongNoteEnd) {//下がLN終点のときは今の位置にずらす
												//終点の色について
												NoteColor end_color;
												if (insert->data.note.color[j] == NoteColor::NONE || insert->data.note.color[j] == NoteColor::K) {
													end_color = score_cell_find_before_note(insert, j)->data.note.color[j];
												}
												else {end_color = insert->data.note.color[j];}

												if (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1) {//SHIFT押しながらだと黒終点にする
													score_cell_write_note(insert, j, end_color, NoteGroup::LongNoteEnd, score_cell_find_before_note(insert, j)->data.note.isBright[j], 1);//LN情報を書き込む(黒終点)
												}
												else {
													score_cell_write_note(insert, j, end_color, NoteGroup::LongNoteEnd, score_cell_find_before_note(insert, j)->data.note.isBright[j], 0);//LN情報を書き込む(通常終点)
												}

												//下にあるLNについて
												//GLNか確認
												bool isGLN = score_cell_find_before_note(find_before, j)->data.note.color[j] != find_before->data.note.color[j];
												if (isGLN){
													score_cell_write_note(find_before, j, find_before->data.note.color[j], NoteGroup::LongNoteMiddle, find_before->data.note.isBright[j]);//下にある終点をLN中間にする
												}
												else {
													score_cell_write_note(find_before, j, NoteColor::NONE, NoteGroup::Single, 0);//下にある音符を消す
													score_cell_delete_if_no_note(&find_before);//その結果何もない行になったら行を消す
												}

												

												if (find_next != NULL) {//上に音符がある
													if (find_next->data.note.group[j] == NoteGroup::LongNoteEnd) {//それがLN終点なら消す
														score_cell_write_note(find_next, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
														score_cell_delete_if_no_note(&find_next);//その結果何もない行になったら行を消す
													}
													else if (find_next->data.note.group[j] == NoteGroup::LongNoteMiddle) {//それがLN中間なら始点にする
														find_next->data.note.group[j] = NoteGroup::LongNoteStart;
													}
												}
											}


										}
									}
								}
							}

							if (putflag == 1 && (Key[KEY_INPUT_LSHIFT] >= 1 || Key[KEY_INPUT_RSHIFT] >= 1)) {//音符を置いたときSHIFT押してたら光らせる
								if (insert->data.note.color[j] != NoteColor::K && insert->data.note.color[j] != NoteColor::NONE) {
									insert->data.note.isBright[j] = 1;//光らせる
									if (insert->data.note.group[j] == NoteGroup::LongNoteEnd) {//LN終点なら始点も光らせる
										find_before->data.note.isBright[j] = true;

									}
									if (insert->data.note.group[j] == NoteGroup::LongNoteStart) {//LN始点なら終点も光らせる
										find_next->data.note.isBright[j] = true;
									}
								}
							}

							if (putflag != -1) {
								if (putflag == 0) {//RGB虹音符を消していたとき 黒を置いたとき
									deleteSolve(find_before, find_next, j);
								}
								if (putflag == 1) {//音符を置いていたとき
									if (find_next != NULL && find_before != NULL) {
										if (find_next->data.note.group[j] == NoteGroup::LongNoteMiddle || find_next->data.note.group[j] == NoteGroup::LongNoteEnd) {//LNの中に音符を置いていたとき
											score_cell_write_note(insert, j, insert->data.note.color[j], NoteGroup::LongNoteMiddle, insert->before->data.note.isBright[j]);//この音符をLN中間点にする (光るかどうかは直前の音符と合わせる)
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
						SolveLN(head, end);
					}
					else {//範囲選択していない
						for (j = 0; j <= 3; j++) {
							if (insert->content == CONTENTS_NOTE && insert->step == step_count) {//insertが音符のセルでstepとinsert同じなら削除

								//insertから下、上にある音符を保存 使うときはその都度NULLじゃないか確かめる必要あり
								SCORE_CELL* find_before = score_cell_find_before_note(insert, j);
								SCORE_CELL* find_next = score_cell_find_next_note(insert, j);

								score_cell_write_note(insert, j, NoteColor::NONE, NoteGroup::Single, 0);//消す
								score_cell_delete_if_no_note(&insert);//その結果何もない行になったら行を消す

								deleteSolve(find_before, find_next, j);

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
							score_cell_write_note(insert_command, 0, NoteColor::NONE, NoteGroup::Single, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 1, NoteColor::NONE, NoteGroup::Single, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 2, NoteColor::NONE, NoteGroup::Single, 0);//音符セルということにして消す
							score_cell_write_note(insert_command, 3, NoteColor::NONE, NoteGroup::Single, 0);//音符セルということにして消す

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
		SetDrawScreen(appContext.baseHandle.getHandle());
		ClearDrawScreen();//グラフィックを初期化
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
					if (show->data.note.color[i] != NoteColor::NONE) {//音符があったら
						if (show->data.note.group[i] == NoteGroup::Single) {//単ノート
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

							if (show->data.note.isBright[i] == false) {
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[(int)show->data.note.color[i]], TRUE);
							}
							if (show->data.note.isBright[i] == true) {
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[(int)show->data.note.color[i]], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
								DrawExtendGraph(lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5), H_NOTE[10], TRUE);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
							}
						}
						else if (show->data.note.group[i] == NoteGroup::LongNoteStart) {//LN始点
							//下半分と中間描画
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

							DrawRectExtendGraph(
								lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 64 + 0.5),
								0, 128, 256, 128,
								H_NOTE[(int)show->data.note.color[i]], TRUE
							);

							//LN中間表示

							DrawRectExtendGraph(
								lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
								lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
								0, 0, 256, 256,
								H_LNOTE[(int)show->data.note.color[i]], TRUE
							);
						



							if (score_cell_find_next_note(show, i)->data.note.LN_k[i] == true && option->op.blackGradation.getIndex() == (int)OptionItem::BlackGradation::ON) {
								//黒終端ロングの中間
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
									lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256,
									gradationLongNote.H_LNOTE_GRAD_FADE_IN[8], TRUE
								);
							}


						
							//光るノーツ
							if (show->data.note.isBright[i] == true) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw - 0 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw)*scale*scale_score_draw + 64 + 0.5),
									0, 128, 256, 128,
									H_NOTE[10], TRUE
								);

								DrawRectExtendGraph(//LN中間表示
									lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
									lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256,
									H_LNOTE[10], TRUE
								);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
							}

						}
						else if (show->data.note.group[i] == NoteGroup::LongNoteMiddle) {//LN中間
							//中間描画
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

							//LN中間表示

							DrawRectExtendGraph(
								lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
								lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
								0, 0, 256, 256,
								H_LNOTE[(int)show->data.note.color[i]], TRUE
							);
						



							if (score_cell_find_next_note(show, i)->data.note.LN_k[i] == true && option->op.blackGradation.getIndex() == (int)OptionItem::BlackGradation::ON) {
								//黒終端ロングの中間
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
									lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256,
									gradationLongNote.H_LNOTE_GRAD_FADE_IN[8], TRUE
								);
							}



							//光るノーツ
							if (show->data.note.isBright[i] == true) {
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

								DrawRectExtendGraph(//LN中間表示
									lane[i] - 64, int(judge_area - (score_cell_find_next_note(show, i)->step - step_count_draw) * scale * scale_score_draw - 0 + 0.5),
									lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 256,
									H_LNOTE[10], TRUE
								);
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
							}
						}
						else if (show->data.note.group[i] == NoteGroup::LongNoteEnd) {//LN終点
							//上半分描画
							SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

							//DrawExtendGraph(lane[i] - 64 + 32, judge_area - (show->step - step_count)*scale - 64, lane[i] + 64, judge_area - (show->step - step_count)*scale + 64, H_NOTE[show->data.note.color[i]], TRUE);

							DrawRectExtendGraph(
								lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
								0, 0, 256, 128,
								show->data.note.LN_k[i] == false ? H_NOTE[(int)show->data.note.color[i]] : H_NOTE[8],//黒終点なら上半分は黒音符表示
								TRUE
							);


							if (show->data.note.isBright[i] == true) {//光る音符の場合
								SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
								DrawRectExtendGraph(
									lane[i] - 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 64 + 0.5), lane[i] + 64, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw + 0 + 0.5),
									0, 0, 256, 128,
									H_NOTE[10], TRUE
								);
							}
						}
					}
				}
			}
			
			show = show->before;
		}

		//GLN文字描画
		show = end;
		while (show->step != -1) {//endからheadまで描画
			if (show->content == CONTENTS_NOTE) {
				for (i = 0; i <= 3; i++) {
					if (show->data.note.color[i] != NoteColor::NONE) {//音符があったら
						if (show->data.note.group[i] == NoteGroup::LongNoteMiddle) {//LN中間
							DrawGraph(lane[i] - 14, int(judge_area - (show->step - step_count_draw) * scale * scale_score_draw - 7 + 0.5), H_GLN, TRUE);
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

		sprintfDx(command_show_str, L"SCROLL");
		DrawString(1072, 700, command_show_str, GetColor(255, 255, 255));

		sprintfDx(command_show_str, L"SCBPM");
		DrawString(1188, 700, command_show_str, GetColor(255, 255, 255));

		if (command_put == COMMAND_KIND_MEASURE) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(0, 0, 229, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_STOPSTEP) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(229, 0, 342, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_STOP) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(342, 0, 455, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_HS) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(455, 0, 824, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_BPM) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(824, 0, 937, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_LETBPM) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(937, 0, 1050, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_SCROLL) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(1050, 0, 1163, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		}
		else if (command_put == COMMAND_KIND_SCROLL_BPM) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(105));
			DrawBox(1163, 0, 1280, 720, GetColor(255, 255, 255), true);
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));
		};

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

		if (config.Editable != 1 && isInstructionAppear) {
			setPrintColorDx(GetColor(255, 0, 0), GetColor(0, 0, 0)); 
			printfDx(L"譜面を編集できません\n");
			printfDx(L"「譜面を編集可能にする」にチェックを付けて\n");
			printfDx(L"起動すると編集できます\n");
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
					fwprintf_s(fp, L"%s", generateRow(measure_p->data.note));
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

wchar_t* generateRow(note_data& note) {
	wchar_t noteText[5] = L"";
	wchar_t colon = L'\0';
	wchar_t glnText[5] = L"";
	sprintfDx(noteText, L"%c%c%c%c",
		note_to_char(note.color[0], note.group[0], note.isBright[0], note.LN_k[0]),
		note_to_char(note.color[1], note.group[1], note.isBright[1], note.LN_k[1]),
		note_to_char(note.color[2], note.group[2], note.isBright[2], note.LN_k[2]),
		note_to_char(note.color[3], note.group[3], note.isBright[3], note.LN_k[3])
	);

	bool isLnNode[4] = { false, false, false, false };
	for (int i = 0; i < 4; i++) {
		isLnNode[i] = note.group[i] == NoteGroup::LongNoteMiddle || note.group[i] == NoteGroup::LongNoteEnd;
	}

	bool isExistLnNode = isLnNode[0] || isLnNode[1] || isLnNode[2] || isLnNode[3];

	if (isExistLnNode) {
		colon = L':';

		sprintfDx(glnText, L"%c%c%c%c",
			isLnNode[0] ? note_to_char(note.color[0], NoteGroup::Single, false, false): L'0',
			isLnNode[1] ? note_to_char(note.color[1], NoteGroup::Single, false, false): L'0',
			isLnNode[2] ? note_to_char(note.color[2], NoteGroup::Single, false, false): L'0',
			isLnNode[3] ? note_to_char(note.color[3], NoteGroup::Single, false, false): L'0'
		);
	}
	static wchar_t resultText[12] = L"";//Releaseビルドで参照できなくなるのでstaticで宣言
	sprintfDx(resultText, L"%s%c%s",noteText,colon,glnText);
	return resultText;
}

wchar_t note_to_char(NoteColor color,NoteGroup group,BOOL bright,BOOL LN_k) {//音符の数値情報を譜面データ文字に変換
	wchar_t color_list[11] = { L'0',L'r',L'g',L'b',L'c',L'm',L'y',L'w',L'k',L'f',L'e' };
	wchar_t color_list_CAP[11] = { NULL,L'R',L'G',L'B',L'C',L'M',L'Y',L'W',NULL,L'F',L'E' };
	if (group == NoteGroup::LongNoteMiddle) {
		return color_list[10];
	}
	else if (group == NoteGroup::LongNoteEnd) {
		return LN_k==0? color_list[10] : color_list_CAP[10];
	}

	if (bright == false) {
		return color_list[(int)color];
	}
	else if (bright == true) {
		return color_list_CAP[(int)color];
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
				buf2.isBright[i] = buf.isBright[3 - i];
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



void SolveLN(SCORE_CELL* start, SCORE_CELL* end) {//LNの不整合を単ノートに置き換える
	SCORE_CELL* search = end;
	int column = 0;

	auto solveEnd = [](SCORE_CELL* search, int column) {
		if (search->data.note.group[column] == NoteGroup::LongNoteEnd) {//LN終点
			if (score_cell_find_before_note(search, column) != NULL) {
				if ((score_cell_find_before_note(search, column)->data.note.group[column] != NoteGroup::LongNoteStart)
					&& (score_cell_find_before_note(search, column)->data.note.group[column] != NoteGroup::LongNoteMiddle)) {
					//LN終点の下が始点でも中間でもないので不整合
					search->data.note.group[column] = NoteGroup::Single;
				}
			}
			else {
				search->data.note.group[column] = NoteGroup::Single;
			}
		}
	};

	auto solveMiddle = [](SCORE_CELL* search, int column) {
		if (search->data.note.group[column] == NoteGroup::LongNoteMiddle) {//LN中間
			if (score_cell_find_before_note(search, column) != NULL) {
				if ((score_cell_find_before_note(search, column)->data.note.group[column] != NoteGroup::LongNoteStart)
					&& (score_cell_find_before_note(search, column)->data.note.group[column] != NoteGroup::LongNoteMiddle)) {
					//LN中間の下が始点でも中間でもないので不整合
					search->data.note.group[column] = NoteGroup::Single;
				}
			}
			if (score_cell_find_next_note(search, column) != NULL) {
				if ((score_cell_find_next_note(search, column)->data.note.group[column] != NoteGroup::LongNoteEnd)
					&& (score_cell_find_next_note(search, column)->data.note.group[column] != NoteGroup::LongNoteMiddle)) {
					//LN中間の上が終点でも中間でもないので不整合
					search->data.note.group[column] = NoteGroup::Single;
				}
			}
			if (score_cell_find_before_note(search, column) == NULL || score_cell_find_next_note(search, column) == NULL) {
				search->data.note.group[column] = NoteGroup::Single;
			}
		}
	};

	auto solveStart = [](SCORE_CELL* search, int column) {
		if (search->data.note.group[column] == NoteGroup::LongNoteStart) {//LN始点
			if (score_cell_find_next_note(search, column) != NULL) {
				if ((score_cell_find_next_note(search, column)->data.note.group[column] != NoteGroup::LongNoteEnd)
					&& (score_cell_find_next_note(search, column)->data.note.group[column] != NoteGroup::LongNoteMiddle)) {
					//LN始点の上が終点でも中間でもないので不整合
					search->data.note.group[column] = NoteGroup::Single;
				}
			}
			else {
				search->data.note.group[column] = NoteGroup::Single;
			}
		}
	};


	while (1) {
		if (search->content == CONTENTS_NOTE) {
			for (column = 0; column < 4; column++) {
				solveEnd(search, column);
				solveMiddle(search, column);
				solveStart(search, column);
			}
		}

		search = search->before;
		if (search->step == -1)break;
	}

	search = start;

	while (1) {
		if (search->content == CONTENTS_NOTE) {
			for (column = 0; column < 4; column++) {
				solveEnd(search, column);
				solveMiddle(search, column);
				solveStart(search, column);
			}
		}

		search = search->next;
		if (search->content == CONTENTS_END)break;
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
					search->data.note.isBright[column],
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
					score_cell_write_note(search, column, NoteColor::NONE, NoteGroup::Single, 0);//消す
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
					score_cell_write_note(search, column, NoteColor::NONE, NoteGroup::Single, 0);//消す
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
					copy->data.note.isBright[column],
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