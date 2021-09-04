#include"CONSTANT_VALUE.h"
#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_NOTE.h"
#include"GAME_LOAD.h"
#include"number_ring.h"
#include"STRUCT_CALCDIFF.h"
#include"STRUCT_OP.h"
#include"STRUCT_SEQ.h"
#include"STRUCT_EDIT_SCORE.h"
#include"STRUCT_CONFIG.h"
#include"score_cell_operation.h"
//#include <initializer_list>
//#include <functional>

#include <algorithm>
#include "EDIT_SCORE.h"


void GAME_LOAD(int song_number,
	int difficulty,
	NOTE **note,
	BARLINE *barline,
	int *lane,
	int readflag,
	CALCDIFF *Cdiff,
	OPTION  *Option,
	BPMC *bpmchange,
	SC *scrollchange,
	STOP_SE *stopSequence,
	int *hash,
	Song *Music,
	int *TimeToEndScroll,
	int *playing_time,
	CONFIG config,
	double pitch,
	SCORE_CELL *score_cell_head,//EDIT_SCORE関数用の譜面情報格納リスト
	int SkillTestFlag
	) {//譜面の読み込み

	double step_counter = 0;//score_cellのstep値を保存
	SCORE_CELL *score_cell_insert = score_cell_head;//このポインタの次にscore_cellセルを挿入

	*hash = 0;//譜面ハッシュ値初期化
	int hash_M = 99999999;//譜面ハッシュ値の最大値

	int i = 0, j = 0, k = 0, n = 0, dataArrayLength = 0;
	int fp = 0;
	errno_t error = 0;
	//NOTE copy[4][NOTE_MAX_NUMBER];
	NOTE **copy;
	copy = (NOTE**)calloc(4, sizeof(NOTE));

	for (i = 0; i <= 3; i++) {
		copy[i] = (NOTE*)calloc(NOTE_MAX_NUMBER, sizeof(NOTE));
		if (copy[i] == NULL) {
			printfDx(L"calloc Error\n");
			Sleep(5000);
			ScreenFlip();
		}
	}
	wchar_t filename[256];
	wchar_t str[256];
	wchar_t sharp1[256];
	wchar_t sharp2[256];

	wchar_t nume_c[16], deno_c[16];//分母分子
	double measure = 1;//小節の長さ(1で4/4拍子)デフォルトは4/4
	double bpm = 120;

	short *bpmList;//最大瞬間BPM算出用のBPMバッファリスト
	bpmList = (short*)calloc(NOTE_MAX_NUMBER*4, sizeof(short));

	wchar_t high_speed_s[5][16];//ハイスピード命令の文字読み取りバッファ
	double high_speed[5] = { 1,1,1,1,1 };//ハイスピード値 (5番目は小節線の速さ)

	double time_counter = 0;//単位はミリ秒
	double time_counter_real = 0;//実際の(GAME_time_passed)経過時間



	int DN = 0;//Division Number一小節の分割数

	int ln = 0;//レーンナンバー(lane number)
	wchar_t color_list[11] =     { L'0',L'r',L'g',L'b',L'y',L'c',L'm',L'w',L'k',L'f',L'e' };
	wchar_t color_list_CAP[11] = { NULL,L'R',L'G',L'B',L'Y',L'C',L'M',L'W',NULL,L'F',L'E' };

	int nc[4] = { 0,0,0,0 };//note_counter
	int blc = 0;//barline counter
	int start_time_flag = 0;//最初のノートを見つけると1になる
	int start_time = 0;//最初のノートのタイミング
	int end_time = 0;//最後のノートのタイミング
	double LN_point = 545.454545454545;//LNの1pointあたりの秒数(ms)
	double color_point[11] = { 0,1,1,1,2,2,2,3,0,0.9,0 };//音符の色毎の難易度ポイント
	double cp_CAP = 1.333;//光る音符の難易度ポイント倍率
	double diff_point = 0;//色の得点を加算していく(rgb:1,ycp:2,w:3,f:1/3,LNは0.545秒ごとに1);

	Cdiff->level_local = 0;//ローカル難易度ポイント初期化
	typedef struct diff_buffer {
		double point = 0;
		int timing = 0;
	}diff_buffer;
	const int LOCAL_DIFF_AMOUNT = 300;
	diff_buffer diff_buf[LOCAL_DIFF_AMOUNT];////300コンボ分の難易度ポイント、タイミングを格納

	double diff_point_local = 0;//300コンボ範囲での一番大きい難易度

	int total_diff_point_notes = 0;//難易度ポイントを足した点の合計

	int ln_use_s[4] = { 0,0,0,0 };//S乱で使う
	int ln_use_c[4] = { 0,0,0,0 };//S乱で使う
	int nc_s[4] = { 0,0,0,0 };//S乱作成用
	int nc_c[4] = { 0,0,0,0 };
	int op_lane[4] = { 0,0,0,0 };//LANE_RANDOMオプションでつかう
	int *timing_same;//譜面を縦に見たとき音符のあるタイミングを配列に順番に入れる
	timing_same = (int*)calloc(NOTE_MAX_NUMBER * 4, sizeof(int));
	
	for (i = 0; i < NOTE_MAX_NUMBER * 4; i++) {
		timing_same[i] = -1;
	}

	int tc = 0;
	int lanestolanec[4] = { 0,0,0,0 };
	int rgb_change[6][11] = { { 0,1,3,2,6,5,4,7,8,9,10 },
	{ 0,2,1,3,4,6,5,7,8,9,10 },
	{ 0,2,3,1,5,6,4,7,8,9,10 },
	{ 0,3,1,2,6,4,5,7,8,9,10 },
	{ 0,3,2,1,5,4,6,7,8,9,10 },
	{ 0,1,2,3,4,5,6,7,8,9,10 } };
	double scroll = 1;//スクロールスピード
	double note_offset_scroll = 0;//スクロール考慮した表示用の方のタイミングオフセット
	int bcc = 0;//bpmchangeのカウンタ
	int scc = 0;//scrollchangeのカウンタ
	int stop_se_c = 0;//stopSequenceのカウンタ
    double sc_timing = 0;//前のスクロール変更があったGAME_time_passed_scrollの時間
	double sc_timing_real = 0;//前のスクロール変更があったGAME_time_passed(実際)の時間
	double bc_timing = 0;//前のBPM変更があったGAME_time_passed_scrollの時間
	double bc_timing_real = 0;//前のBPM変更があったGAME_time_passed(実際)の時間

	double stopTimeSum = 0;//停止時間の合計値
	int ignoreFlag = 0;//#STOPの後の最初の停止時間ずらしを無視するフラグ(#STOPの次のノートを叩けるようにするため)
					   //char Cdiff_buf[10000][30];//難易度算出用のバッファ
	int IgnoreArea = 0;//STOPが負でワープ区間になったときに今読み込んでいる区間が無視するべき音符であることを示すフラグ
	double IgnoreTimeCounter_real = 0;
	double IgnoreEndTime_real = 0;

	int Cdiff_count = 0;//バッファに入れた個数

	int textLine = 0;//テキストの現在の行の場所

	int NearColorBuf = 0;//その行でそのレーンに一番近い左の方にあるノートの色を保存するための変数
	int BeforeColorBuf[4] = { 0,0,0,0 };//レーンごとの前のノートの色を保存するための配列

	for (i = 0; i <= 255; i++) {//初期化
		filename[i] = L'\0';
		str[i] = L'\0';
		sharp1[i] = L'\0';
		sharp2[i] = L'\0';
	}
	for (i = 0; i <= NOTE_MAX_NUMBER - 1; i++) {
		bpmchange[i].use = 0;
		bpmchange[i].timing = 0;//GAME_time_passed_scrollのタイミング
		bpmchange[i].timing_real = 0;//ゲームが始まってからの実際のタイミング
		bpmchange[i].bpm = 1;

		scrollchange[i].use = 0;
		scrollchange[i].timing = 0;//GAME_time_passed_scrollのタイミング
		scrollchange[i].timing_real = 0;//ゲームが始まってからの実際のタイミング
		scrollchange[i].scroll = 1;

		stopSequence[i].use = 0;
		stopSequence[i].timing = 0;
		stopSequence[i].timing_real = 0;
		stopSequence[i].stop_time = 0;
	}
	for(i = 0; i <= BARLINE_MAX_NUMBER - 1; i++)barline[i].use = 0;

	Music[song_number].total_note[difficulty] = 0;
	//wcscpy_s(Music[song_number].jinglepath[difficulty], L"sound/jingle.wav");//デフォルトジングル
	Music[song_number].season[difficulty] = 0;//デフォルトは無季節
	Music[song_number].StrColor[difficulty] = GetColor(255, 255, 255);//デフォルト色は白
	Music[song_number].StrShadowColor[difficulty] = GetColor(0, 0, 0);//デフォルト枠色は黒


	wcscpy_s(filename, Music[song_number].SongPath[difficulty]);
	




	fp = FileRead_open(filename);
	if (fp == 0) {//無かったら戻る
				  //printfDx(L"%sのオープンに失敗しました.\n", filename);
		Music[song_number].exist[difficulty] = 0;
		//printfDx(L"%s\n", filename);
		//ScreenFlip();
		//Sleep(10000);
		free(bpmList);
		free(timing_same);
		return;
	}
	else {
		Music[song_number].exist[difficulty] = 1;//存在確認
	}
	FileRead_seek(fp, 2, SEEK_SET);//BOM飛ばし



	while (wcscmp(L"#NOTES", sharp1) != 0) {//ヘッダ読み込み部(#NOTESのとこまで)

		do {//空行は無視
			FileRead_gets(str, 256, fp);//一行取得(\nは入らない)
			textLine++;
		} while (wcscmp(str, L"") == 0);

		sharp2[0] = L'\0';
		swscanf_s(str, L"%[^:\n]:%[^:\n]", sharp1, _countof(sharp1), sharp2, _countof(sharp2));//:で前後に区切って読み込む
																							 //printfDx(L"%s jjj %s\n", sharp1, sharp2);
																							 //printfDx(L"%s", str);



		if (wcscmp(L"#TITLE", sharp1) == 0) {
			wchar_t DefaultTitle[] = L"\0";
			free(Music[song_number].title[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].title[difficulty] = (wchar_t*)calloc(strlenDx(DefaultTitle) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].title[difficulty], L"%s", DefaultTitle);
			}
			else {
				Music[song_number].title[difficulty] = (wchar_t*)calloc(strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].title[difficulty], L"%s", sharp2);
			}
		}
		if (wcscmp(L"#ARTIST", sharp1) == 0) {
			wchar_t DefaultArtist[] = L"\0";
			free(Music[song_number].artist[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].artist[difficulty] = (wchar_t*)calloc(strlenDx(DefaultArtist) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].artist[difficulty], L"%s", DefaultArtist);
			}
			else {
				Music[song_number].artist[difficulty] = (wchar_t*)calloc(strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].artist[difficulty], L"%s", sharp2);
			}
		}
		if (wcscmp(L"#GENRE", sharp1) == 0) {
			wchar_t DefaultGenre[] = L"\0";

			free(Music[song_number].genre[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].genre[difficulty] = (wchar_t*)calloc(strlenDx(DefaultGenre) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].genre[difficulty], L"%s", DefaultGenre);
			}
			else {
				Music[song_number].genre[difficulty] = (wchar_t*)calloc(strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].genre[difficulty], L"%s", sharp2);
			}
		}
		if (wcscmp(L"#WAV", sharp1) == 0) {
			wchar_t DefaultWav[] = L"\0";
			free(Music[song_number].wavpath[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].wavpath[difficulty] = (wchar_t*)calloc(strlenDx(DefaultWav) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].wavpath[difficulty], L"%s", DefaultWav);
			}
			else {

				Music[song_number].wavpath[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].wavpath[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
		}
		if (wcscmp(L"#JINGLE", sharp1) == 0) {
			wchar_t DefaultJingle[] = L"sound/jingle.wav";

			free(Music[song_number].jinglepath[difficulty]);

			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].jinglepath[difficulty] = (wchar_t*)calloc(strlenDx(DefaultJingle) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jinglepath[difficulty], L"%s", DefaultJingle);
			}
			else {

				Music[song_number].jinglepath[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jinglepath[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
			//printfDx(L"%s\n", Music[song_number].jinglepath[difficulty]);

		}
		if (wcscmp(L"#JACKET", sharp1) == 0) {
			wchar_t DefaultJacket[] = L"\0";

			free(Music[song_number].jacketpath[difficulty]);

			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].jacketpath[difficulty] = (wchar_t*)calloc(strlenDx(DefaultJacket) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath[difficulty], L"%s", DefaultJacket);
			}
			else {

				Music[song_number].jacketpath[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
			//printfDx(L"%s\n", Music[song_number].jacketpath[difficulty]);
		}

		if (wcscmp(L"#JACKET2", sharp1) == 0) {
			wchar_t DefaultJacket2[] = L"\0";

			free(Music[song_number].jacketpath2[difficulty]);

			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったらデフォルトジングル
				Music[song_number].jacketpath2[difficulty] = (wchar_t*)calloc(strlenDx(DefaultJacket2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath2[difficulty], L"%s", DefaultJacket2);
			}
			else {

				Music[song_number].jacketpath2[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath2[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
			//printfDx(L"%s\n", Music[song_number].jacketpath[difficulty]);

		}


		if (wcscmp(L"#BPM", sharp1) == 0) {
			Music[song_number].bpm[difficulty] = _wtof(sharp2);
			bpm = Music[song_number].bpm[difficulty];
			bpm *=pitch;
			//printfDx(L"%f\n", bpm);
			*hash = int(*hash + 65535 * int(bpm)) % hash_M;

		}
		if (wcscmp(L"#BPMMIN", sharp1) == 0) {
			Music[song_number].bpmmin[difficulty] = _wtoi(sharp2);
			//printfDx(L"%f\n", bpm);

		}
		if (wcscmp(L"#BPMMAX", sharp1) == 0) {
			Music[song_number].bpmmax[difficulty] = _wtoi(sharp2);
			//printfDx(L"%f\n", bpm);

		}
		if (wcscmp(L"#SCROLL", sharp1) == 0) {//最初の小節より前のスクロールスピード

			scrollchange[scc].timing = 0;
			scrollchange[scc].use = 1;//このノートを使っているとするためのフラグ
			sc_timing_real = 0;//実際の変更タイミングを格納
			scrollchange[scc].timing_real = 0;
			sc_timing = scrollchange[scc].timing;
			scroll = _wtof(sharp2);
			scrollchange[scc].scroll = scroll;
			Music[song_number].bpm[difficulty] *= scroll;


			bpmchange[bcc].timing = 0;
			bpmchange[bcc].bpm = (float)(bpm * scroll);
			bpmchange[bcc].use = 1;//このノートを使っているとするためのフラグ
			bcc++;

			scc++;

			*hash = (*hash + 650235 * int(scroll)) % hash_M;
			//printfDx(L"%f\n", bpm);

		}

		if (wcscmp(L"#LEVEL", sharp1) == 0) {
			Music[song_number].level[difficulty] = _wtoi(sharp2);
			//printfDx(L"%d\n", Music[song_number].level[difficulty]);

		}
		if (wcscmp(L"#SONGOFFSET", sharp1) == 0) {
			Music[song_number].songoffset[difficulty] = int((double)_wtoi(sharp2)/pitch);
			//printfDx(L"%f\n", Music[song_number].songoffset[difficulty]);

		}
		if (wcscmp(L"#NOTEOFFSET", sharp1) == 0) {
			Music[song_number].noteoffset[difficulty] = int((double)_wtoi(sharp2)/pitch);
			note_offset_scroll = Music[song_number].noteoffset[difficulty] * scroll;//表示用オフセット
			//printfDx(L"%f\n", Music[song_number].noteoffset[difficulty]);

		}
		if (wcscmp(L"#DEMOSTART", sharp1) == 0) {
			Music[song_number].demostart[difficulty] = int(_wtof(sharp2)*1000);
			//printfDx(L"%f\n", Music[song_number].demostart[difficulty]);

		}
		if (wcscmp(L"#SEASON", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら無季節にする
				Music[song_number].season[difficulty] = 0;
			}
			else {
				Music[song_number].season[difficulty] = _wtoi(sharp2);
			}
			//printfDx(L"%d\n", Music[song_number].season[difficulty]);
		}
		if (wcscmp(L"#COLOR", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら白
				Music[song_number].StrColor[difficulty] = GetColor(255,255,255);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				Music[song_number].StrColor[difficulty] = GetColor(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx(L"%d\n", Music[song_number].StrColor[difficulty]);
		}
		if (wcscmp(L"#SHADOWCOLOR", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら白
				Music[song_number].StrShadowColor[difficulty] = GetColor(0, 0, 0);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				Music[song_number].StrShadowColor[difficulty] = GetColor(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx(L"%d\n", Music[song_number].StrColor[difficulty]);
		}
		if (wcscmp(L"#VERSION", sharp1) == 0) {
			Music[song_number].version[difficulty] = int(_wtof(sharp2)*100 + 0.005);
		}

		//ScreenFlip();
	}
	if (readflag == 1) {
		free(bpmList);
		free(timing_same);

		for (i = 0; i <= 3; i++) {
			free(copy[i]);
		}
		free(copy);
		FileRead_close(fp);
		return;//ヘッダーだけ読み込んで終了
	}

	for (i = 0; i <= 3; i++) {
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {//noteの初期化(bpm以外)
			note[i][j].x = 0;//ノートのx座標
			note[i][j].y = -128;//ノートのy座標
			note[i][j].timing = 0;//叩くタイミング(ミリ秒)
			note[i][j].fall = 0;//0なら落ちてない,1なら落下中
			if (i != 0)note[i][j].color_init = 0;//ノートの元の色
			if (i != 0) note[i][j].color = 0;//ノートの色(画像ハンドルが入る)(1:R 2:G 3:B 4:Y 5:C 6:M 7:W 8:K)0ならこのノートは使ってない
			note[i][j].hit = 0;//1:叩いた 0:叩いてない
			note[i][j].group = 0;//
			note[i][j].textLine = 0;
			note[i][j].snd = 0;
		}
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {//copyの初期化(bpm以外)
			copy[i][j].x = 0;//ノートのx座標
			copy[i][j].y = -128;//ノートのy座標
			copy[i][j].timing = 0;//叩くタイミング(ミリ秒)
			copy[i][j].fall = 0;//0なら落ちてない,1なら落下中
			if (i != 0)copy[i][j].color_init = 0;//ノートの元の色
			if (i != 0)copy[i][j].color = 0;//ノートの色(画像ハンドルが入る)(1:R 2:G 3:B 4:Y 5:C 6:M 7:W 8:K)0ならこのノートは使ってない
			copy[i][j].hit = 0;//1:叩いた 0:叩いてない
			copy[i][j].group = 0;//
			note[i][j].textLine = 0;
			note[i][j].snd = 0;
		}

	}

	//time_counter_real = config.VsyncOffsetCompensation;
	int file_p_first = 0;//小節先頭ファイルポインタ(読み込んでいる小節の先頭を指す)
	file_p_first = FileRead_tell(fp);//最初に読み込む小節の先頭を指す
	i = 0;
	while (wcscmp(L"#END", sharp1) != 0) {//譜面読み込み部
		if (ProcessMessage() != 0) {
			DxLib_End();
			exit(0);
			return;
		}
		ClearDrawScreen();

		do {//空行は無視
			FileRead_gets(str, 256, fp);//一行取得(\nは入らない)
			if (wcscmp(str, L"") == 0)textLine++;
		} while (wcscmp(str, L"") == 0);

		swscanf_s(str, L"%[^:\n]:%[^:\n]", sharp1, _countof(sharp1), sharp2, _countof(sharp2));
		dataArrayLength++;//データの行数を数える

		if (wcscmp(L"#MEASURE", sharp1) == 0) {
			swscanf_s(sharp2, L"%[^/\n]/%[^/\n]", nume_c, _countof(nume_c), deno_c, _countof(deno_c));
			measure = (double)_wtof(nume_c) / (double)_wtof(deno_c);

			//エディタ用処理
			if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
				score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
				score_cell_insert = score_cell_insert->next;
				
				score_cell_write_command(score_cell_insert, COMMAND_KIND_MEASURE, _wtof(nume_c), _wtof(deno_c));//コマンド挿入
			}

			*hash = (*hash + int(measure * 579087)) % hash_M;
			//printfDx(L"%f\n", measure);
			//textLine++;
		}

		if ((wcslen(sharp1) >= 1) && sharp1[wcslen(sharp1) - 1] != L',') {//最後がカンマではない1文字以上の行(#命令も含めて複数行になる所)を見つけた
			if (sharp1[0] != L'#') {//先頭が#じゃないなら分割数を数える
				i++;//一小節の分割数を数える
			}
		}


		if (wcsrchr(sharp1, L',') != NULL) {//「,」がある行を見つけた → 1小節分読み込んだので分割数を決めnoteにデータを入れる
			i++;//「,」行分の分割数を足す
			DN = i;//DN分音符単位になる
			i = 0;

			FileRead_seek(fp, file_p_first, SEEK_SET);//読み込む小節の先頭にファイルポインタを合わせる

			BOOL PutBarline = 1;//このキャッシュ内で小節線をまだ置いていないというフラグ
			while (1) {
				do {//空行は無視
					FileRead_gets(str, 256, fp);//一行取得(\nは入らない)
				} while (wcscmp(str, L"") == 0);
				swscanf_s(str, L"%[^:\n]:%[^:\n]", sharp1, _countof(sharp1), sharp2, _countof(sharp2));

				if (wcscmp(L"#BPM", sharp1) == 0) {//BPM変更点を見つけた
					bpm = _wtof(sharp2) * pitch;
					bpm_change_add(bpm, bpmchange, &bcc, time_counter, note_offset_scroll, &bc_timing_real, &bc_timing, scroll, stopTimeSum);


					*hash = (*hash + int(bpm * 73087)) % hash_M;
					//printfDx(L"%f\n", bpm);

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_BPM, bpm);//コマンド挿入
					}
				}

				if (wcscmp(L"#LETBPM", sharp1) == 0) {//SCROLLを考慮したBPM変更点を見つけた
					bpm = _wtof(sharp2) * pitch / scroll;
					bpm_change_add(bpm, bpmchange, &bcc, time_counter, note_offset_scroll, &bc_timing_real, &bc_timing, scroll, stopTimeSum);


					*hash = (*hash + int(bpm * 73087)) % hash_M;
					//printfDx(L"%f\n", bpm);

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_LETBPM, _wtof(sharp2) * pitch);//コマンド挿入
					}
				}

				if (wcscmp(L"#SCROLL", sharp1) == 0 || wcscmp(L"#SC", sharp1) == 0) {//スクロールスピード変更点を見つけた(倍率指定)
					double time_buf = time_counter + note_offset_scroll;

					scrollchange[scc].timing = time_buf;
					scrollchange[scc].use = 1;//このノートを使っているとするためのフラグ
					sc_timing_real += (time_buf - sc_timing) / scroll;//実際の変更タイミングを格納
					scrollchange[scc].timing_real = sc_timing_real
						+ stopTimeSum * 1000;
					sc_timing = time_buf;
					scroll = _wtof(sharp2);
					scrollchange[scc].scroll = scroll;

					//BPM
					bc_timing_real = sc_timing_real;
					bc_timing = sc_timing;
					bpmchange[bcc].timing = scrollchange[scc].timing;//実際の変更タイミングを格納
					bpmchange[bcc].timing_real = scrollchange[scc].timing_real;//実際の変更タイミングを格納

					bpmchange[bcc].bpm = (bpm * scroll);
					bpmchange[bcc].use = 1;//このノートを使っているとするためのフラグ
					bcc++;

					if (scc == 0) {
						//Music[song_number].songoffset[difficulty] /= scroll;
						//Music[song_number].noteoffset[difficulty] /= scroll;
					}

					scc++;

					*hash = (*hash + int(scroll * 73087)) % hash_M;
					//printfDx(L"%f\n", bpm);

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_SCROLL, scroll);//コマンド挿入
					}

				}

				if (wcscmp(L"#SCROLLBPM", sharp1) == 0 || wcscmp(L"#SCBPM", sharp1) == 0) {//スクロールスピード変更点を見つけた(BPM値指定)
					double time_buf = time_counter + note_offset_scroll;

					scrollchange[scc].timing = time_buf + 0.5;
					scrollchange[scc].use = 1;//このノートを使っているとするためのフラグ
					sc_timing_real += (time_buf - sc_timing) / scroll;//実際の変更タイミングを格納
					scrollchange[scc].timing_real = sc_timing_real
						+ stopTimeSum * 1000;
					sc_timing = time_buf;
					scroll = pitch * _wtof(sharp2) / bpm;//指定されたBPM/現在のBPMの割合で速度を変える
					scrollchange[scc].scroll = scroll;

					//BPM
					bc_timing_real = sc_timing_real;
					bc_timing = sc_timing;
					bpmchange[bcc].timing = scrollchange[scc].timing;//実際の変更タイミングを格納
					bpmchange[bcc].timing_real = scrollchange[scc].timing_real;//実際の変更タイミングを格納

					bpmchange[bcc].bpm = (bpm * scroll);
					bpmchange[bcc].use = 1;//このノートを使っているとするためのフラグ
					bcc++;
					if (scc == 0) {
						//Music[song_number].songoffset[difficulty] /= scroll;
						//Music[song_number].noteoffset[difficulty] /= scroll;
					}

					scc++;

					*hash = (*hash + int(scroll * 73087)) % hash_M;
					//printfDx(L"%f\n", bpm);

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_SCROLL_BPM, _wtof(sharp2));//コマンド挿入
					}

				}

				if (wcscmp(L"#STOP", sharp1) == 0) {//停止地点を見つけた(数値指定)
					stopSequence[stop_se_c].timing = int(time_counter + note_offset_scroll + 0.5);
					stopSequence[stop_se_c].timing_real = time_counter_real + Music[song_number].noteoffset[difficulty]
						+ stopTimeSum * 1000;//停止地点のタイミング
					stopSequence[stop_se_c].stop_time = _wtof(sharp2) / pitch;//停止時間(s)
					stopSequence[stop_se_c].use = 1;


					*hash = (*hash + int(stopSequence[stop_se_c].stop_time * 73087)) % hash_M;
					*hash = (*hash + int(stopSequence[stop_se_c].timing_real * 653)) % hash_M;


					//note[k][nc[k]].timing = int((time_counter + note_offset_scroll));
					//note[k][nc[k]].timing_init = int((time_counter + note_offset_scroll));

					ignoreFlag = 1;//次のノートにはタイミングずらしを適用しない

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_STOP, _wtof(sharp2));//コマンド挿入
					}
				}

				if (wcscmp(L"#STOPSTEP", sharp1) == 0) {//停止地点を見つけた(音価指定)
					swscanf_s(sharp2, L"%[^/\n]/%[^/\n]", nume_c, _countof(nume_c), deno_c, _countof(deno_c));
					//nume_c, _countof(nume_c), deno_c, _countof(deno_c));


					stopSequence[stop_se_c].timing = int(time_counter + note_offset_scroll + 0.5);
					stopSequence[stop_se_c].timing_real = time_counter_real + Music[song_number].noteoffset[difficulty]
						+ stopTimeSum * 1000;//停止地点のタイミング
											  //stopSequence[stop_se_c].stop_time = _wtof(sharp2);//停止時間(s)

					stopSequence[stop_se_c].stop_time = (((double)60 * 4) / (bpm * scroll)) * ((double)_wtof(nume_c) / (double)_wtof(deno_c));//停止時間(s)

					stopSequence[stop_se_c].use = 1;


					*hash = (*hash + int(stopSequence[stop_se_c].stop_time * 73087)) % hash_M;
					*hash = (*hash + int(stopSequence[stop_se_c].timing_real * 653)) % hash_M;


					//note[k][nc[k]].timing = int((time_counter + note_offset_scroll));
					//note[k][nc[k]].timing_init = int((time_counter + note_offset_scroll));

					ignoreFlag = 1;//次のノートにはタイミングずらしを適用しない

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_STOPSTEP, _wtof(nume_c), _wtof(deno_c));//コマンド挿入
					}
				}

				if ((wcscmp(L"#HIGHSPEED", sharp1) == 0) || (wcscmp(L"#HS", sharp1) == 0)) {//ハイスピード変更点を見つけた
																							//high_speed[0] = _wtof(sharp2);
																							//printf();
																							//clsDx();
																							//printfDx(L"%s\n", sharp1);
					swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n],%[^,\n]", high_speed_s[0], _countof(high_speed_s[0]),
						high_speed_s[1], _countof(high_speed_s[1]),
						high_speed_s[2], _countof(high_speed_s[2]),
						high_speed_s[3], _countof(high_speed_s[3]));

					/*
					for (j = 0; j <= 3; j++) {
					printfDx(L"%d:%s\n", j, high_speed_s[j]);
					}
					*/

					//_wtofを使う
					//measure = (double)_wtof(nume_c) / (double)_wtof(deno_c);
					//printfDx(L"%s\n", sharp2);
					if (wcschr(sharp2, L',') != NULL) {//,があるので1行ごとにハイスピ設定
						for (j = 0; j <= 3; j++) {
							//printfDx(L"%d:%s\n", j, high_speed_s[j]);
							//ScreenFlip();

							high_speed[j] = _wtof(high_speed_s[j]);
						}

						high_speed[4] = (std::min)({ high_speed[0],high_speed[1],high_speed[2],high_speed[3], });
					}



					if (wcschr(sharp2, L',') == NULL) {//,がないときは全てのレーンに適用する(小節線も)

						for (j = 0; j <= 4; j++) {
							//ScreenFlip();

							high_speed[j] = _wtof(high_speed_s[0]);
						}
						//printfDx(L"%s\n", high_speed_s[0]);
						//printfDx(L"%f\n", high_speed[0]);
					}
					//ScreenFlip();
					//Sleep(5000);

					//エディタ用処理
					if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
						score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
						score_cell_insert = score_cell_insert->next;

						score_cell_write_command(score_cell_insert, COMMAND_KIND_HS, high_speed[0], high_speed[1], high_speed[2], high_speed[3]);//コマンド挿入
					}

					*hash = (*hash + int(high_speed[0] * 73087)) % hash_M;
					*hash = (*hash + int(high_speed[1] * 742087)) % hash_M;
					*hash = (*hash + int(high_speed[2] * 8742)) % hash_M;
					*hash = (*hash + int(high_speed[3] * 12963)) % hash_M;

				}



				int insert_flag = 0;//エディタ用のセルを挿入したフラグ
				if (sharp1[0] != L'#') {//命令ではない行、つまり譜面が書かれている行

					if (PutBarline == 1) {//最初の譜面が描かれている行に来たら小節線のタイミング情報格納
						barline[blc].timing = int((time_counter + note_offset_scroll) * TIMING_SHOW_RATIO + 0.5);//表示用タイミング
						barline[blc].timing_real = int(time_counter_real + Music[song_number].noteoffset[difficulty]
							+ stopTimeSum * 1000 + 0.5);//実際に叩くタイミング
						barline[blc].bpm = float(high_speed[4] * (bpm));
						barline[blc].use = 1;
						blc++;
						PutBarline = 0;
					}

					int length = wcslen(sharp1);//左端から何列探索するか
					if (length > 4)length = 4;

					for (k = 0; k < length; k++) {//レーン
						for (j = 1; j <= 10; j++) {//rからdまでの色を探す(ロングノート終端も探す)
							if (color_list[j] == sharp1[k] || color_list_CAP[j] == sharp1[k]) {//キャッシュの[i]行[k-1]列目の色が何色か(大文字だと音は長く鳴る)
								//printfDx(L"レーン番号:%d\n", k);
								//printfDx(L"time:%f\n", time_counter);
								//printfDx(L"DN:%d\n", DN);

								if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
									if (insert_flag == 0) {//1行につきセル挿入は一回
										score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
										score_cell_insert = score_cell_insert->next;

										score_cell_write_note(score_cell_insert, 0, 0, 0, 0);//color0で初期化
										score_cell_write_note(score_cell_insert, 1, 0, 0, 0);
										score_cell_write_note(score_cell_insert, 2, 0, 0, 0);
										score_cell_write_note(score_cell_insert, 3, 0, 0, 0);
										insert_flag = 1;
									}
								}

								note[k][nc[k]].timing = int((time_counter + note_offset_scroll) * TIMING_SHOW_RATIO + 0.5);//表示用タイミング
								note[k][nc[k]].timing_init = note[k][nc[k]].timing;//表示用タイミング(保存用)
								note[k][nc[k]].timing_real = int(time_counter_real + Music[song_number].noteoffset[difficulty]
									+ stopTimeSum * 1000 + 0.5 + config.VsyncOffsetCompensation);//実際に叩くタイミング
								note[k][nc[k]].timing_init_real = note[k][nc[k]].timing_real;//実際に叩くタイミング(保存用)

								note[k][nc[k]].color = j;
								note[k][nc[k]].color_init = j;

								note[k][nc[k]].x = lane[k];
								note[k][nc[k]].bpm = float(high_speed[k] * bpm );
								note[k][nc[k]].bpm_real = float(high_speed[k] * bpm * scroll);
								if (color_list_CAP[j] == sharp1[k] && color_list_CAP[10] != sharp1[k]) {//LN終端以外で大文字で書かれていたら

									note[k][nc[k]].snd = 1;//長く音を鳴らすようにする
									cp_CAP = 1.333;//難易度ポイントを1.333倍にする
								}
								else {
									cp_CAP = 1;//難易度ポイントは等倍
								}

								if (j != 8 && j != 10) {//黒とLN終点以外なら
									bpmList[Music[song_number].total_note[difficulty]] = (short)(note[k][nc[k]].bpm * scroll);//BPM情報格納
									Music[song_number].total_note[difficulty]++;//総ノート数+1する
								}

								if (IgnoreArea == 1) {//無視するべき音符ならignoreを1にする
									note[k][nc[k]].ignore = 1;
								}

								//難易度自動算出処理
								double BeforeSamePoint = 1;
								int la = 0;
								for (la = 0; la <= 3; la++) {
									if (j == BeforeColorBuf[la]) {//この前の音符の色と同じだった
										if (la == k) {//レーンも同じ
											BeforeSamePoint *= 0.800;
										}
										else {//レーンは違うけど前の色と同じ
											BeforeSamePoint *= 0.850;
										}

									}
									else {
										BeforeSamePoint = 1;
									}
								}
								if (k >= 1) {//2列目以降について
									if (note[k][nc[k]].color == NearColorBuf) {//左にある音符と同じ色だったらポイント下げる
										diff_point += BeforeSamePoint * 0.900 * color_point[j] * cp_CAP;//難易度ポイント算出
																						   //1/3として加算

																						   //難易度ポイントバッファに格納していく
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].point = BeforeSamePoint * color_point[j] * cp_CAP;
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[k][nc[k]].timing_real;
									}
									else {//違う色だった
										diff_point += BeforeSamePoint * color_point[j] * cp_CAP;//難易度ポイント普通に加算

																					 //難易度ポイントバッファに格納していく
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].point = BeforeSamePoint * color_point[j] * cp_CAP;
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[k][nc[k]].timing_real;
									}
								}
								else {//1列目について
									diff_point += BeforeSamePoint * color_point[j] * cp_CAP;//難易度ポイント加算

																				 //難易度ポイントバッファに格納していく
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].point = BeforeSamePoint * color_point[j] * cp_CAP;
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[k][nc[k]].timing_real;
								}
								NearColorBuf = j;//この音符の色を保存(後に左の音符扱い)
								if (j != 10)BeforeColorBuf[k] = j;//このレーンの今回の音符の色を保存(eは除く)



								note[k][nc[k]].textLine = textLine + 1;
								*hash = (*hash + int(note[k][nc[k]].snd * 13573)) % hash_M;

								*hash = (*hash + int(note[k][nc[k]].color * 61463)) % hash_M;
								*hash = (*hash + int(k * 897649)) % hash_M;
								if (color_list[0] != sharp1[k]) {//0以外の時
									if (timing_same[tc] == -1) {//音符のタイミング格納
										timing_same[tc] = note[k][nc[k]].timing;
										*hash = (*hash + int(timing_same[tc] * 1963)) % hash_M;

										//sprintf_s(Cdiff_buf[Cdiff_count],"%s",sharp1);//難易度算出用バッファに格納
										//Cdiff_count++;
									}

									if (start_time_flag == 0) {
										start_time = note[k][nc[k]].timing_real;
										start_time_flag = 1;
									}
									end_time = note[k][nc[k]].timing_real;
								}
								//printfDx(L"レーン:%d 色:%d\n", k, j);
								if (color_list[10] == sharp1[k] || color_list_CAP[10] == sharp1[k]) {//'e'または'E'だったら前のノートをLN始点にしてここでLN終点にする
									note[k][nc[k] - 1].group = 1;//始点
									note[k][nc[k]].group = 2;//終点

									if (color_list[10] == sharp1[k]) {//eなら
										note[k][nc[k]].LN_k = 0;//通常終点
									}
									else if (color_list_CAP[10] == sharp1[k]) {//Eなら
										note[k][nc[k]].LN_k = 1;//黒終点
									}

									note[k][nc[k]].color = note[k][nc[k] - 1].color;
									note[k][nc[k]].color_init = note[k][nc[k] - 1].color;

									diff_point += (double)cp_CAP * ((double)note[k][nc[k]].timing_real - note[k][nc[k] - 1].timing_real) / (LN_point);

									//難易度ポイントバッファに格納していく
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].point = (double)cp_CAP * ((double)note[k][nc[k]].timing_real - note[k][nc[k] - 1].timing_real) / (LN_point);
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[k][nc[k]].timing_real;


									//エディタ用処理
									if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
										score_cell_find_before_note(score_cell_insert, k)->data.note.group[k] = 1;//k列目のこのLN終端のbeforeにあるノートのグループをLN始点にする
									}
								}

								//エディタ用処理
								if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
									score_cell_write_note(score_cell_insert, k, note[k][nc[k]].color, note[k][nc[k]].group, note[k][nc[k]].snd, note[k][nc[k]].LN_k);//k列目の音符情報を格納
									if (score_cell_find_before_note(score_cell_insert, k) != NULL) {
										if (score_cell_find_before_note(score_cell_insert, k)->data.note.group[k] == 1 && score_cell_find_before_note(score_cell_insert, k)->data.note.bright[k] == 1) {//beforeがLN始点で光っていたら
											score_cell_insert->data.note.bright[k] = 1;//このLN終点も光らせる(エディタ用)
										}
									}
								}

								nc[k]++;

								if (j != 8) {//黒以外なら
									total_diff_point_notes++;//難易度加算数インクリメント
								}


								if (j != 8 && total_diff_point_notes >= LOCAL_DIFF_AMOUNT) {//黒じゃなくて難易度加算が一定以上
																							//一定分の難易度ポイントの合計を算出
									double diff_point_local_sum = 0;
									for (int ii = total_diff_point_notes; ii <= total_diff_point_notes + LOCAL_DIFF_AMOUNT - 1; ii++) {


										diff_point_local_sum += diff_buf[number_ring(ii, LOCAL_DIFF_AMOUNT - 1)].point;

									}

									diff_point_local = int(((0.9230283911671924 * diff_point_local_sum * ((double)120000 / ((double)diff_buf[number_ring(total_diff_point_notes - 1, LOCAL_DIFF_AMOUNT - 1)].timing - diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing))) / 10) + 0.5);
									if (diff_point_local >= Cdiff->level_local) {
										Cdiff->level_local = int(diff_point_local);
									}
								}

								break;//音符が見つかったので色のforを抜けて次のレーンへ
							}
						}
					}
					//停止ギミック処理
					if (ignoreFlag == 1) {//#STOPの後の最初の譜面行だったら
						ignoreFlag = 0;
						stopTimeSum += stopSequence[stop_se_c].stop_time;//総停止時間にさっきの#STOP時間を足す(ここから全体のタイミングがずれる)

						//マイナスでワープする場合はここから無視区間スタート
						if (stopSequence[stop_se_c].stop_time < 0) {
							IgnoreArea = 1;

							IgnoreTimeCounter_real = 0;
							IgnoreEndTime_real = -stopSequence[stop_se_c].stop_time;
						}

						stop_se_c++;//カウンタインクリメント


					}
				}

				if (timing_same[tc] != -1) {//音符がその行にあったらtcインクリメント
					tc++;

				}
				if (sharp1[0] != L'#') {
					time_counter += double((double)1000 * ((60 / bpm) * 4 * measure) / DN);//分割単位分の時間プラスする
					time_counter_real += double((double)1000 * ((60 / (bpm * scroll)) * 4 * measure) / DN);//分割単位分の実際の時間プラスする

					if (IgnoreArea == 1) {//今無視区間のどこにいるか逐次計算
						IgnoreTimeCounter_real += double((double)1000 * ((60 / (bpm * scroll)) * 4 * measure) / DN);
						if (IgnoreTimeCounter_real > IgnoreEndTime_real) {//無視区間終了
							IgnoreArea = 0;
						}
					}

					step_counter += (double)EDITSTEP * measure / (double)DN;

				}
				textLine++;//テキスト行数を数える

				if (wcsrchr(sharp1, L',') != NULL) {//「,」がある行を見つけた → 1小節分読み込んだので次の小節の読み込みに移る
					file_p_first = FileRead_tell(fp);//次に読み込む小節の先頭を指す
					break;
				}
			}

			i = 0;//また使うのでiを0に戻しておく
			dataArrayLength = 0;
		}
		//Music[song_number].total_note[difficulty] = nc[1] + nc[2] + nc[3] + nc[4];//黒も含めた総ノーツ数



		//Sleep(200);
	}

	//最後の小節線付加
	{
		barline[blc].timing = int((time_counter + note_offset_scroll) * TIMING_SHOW_RATIO + 0.5);//表示用タイミング
		barline[blc].timing_real = int(time_counter_real + Music[song_number].noteoffset[difficulty]
			+ stopTimeSum * 1000 + 0.5);//実際に叩くタイミング
		barline[blc].bpm = float(high_speed[4] * (bpm));
		barline[blc].use = 1;
		blc++;
	}

	//瞬間BPM算出
	double BPM_SuggestCalcRatio = 0.1;//瞬間BPMを算出するのに使うノーツ数の割合
	double BPM_suggest = 0;
	int SearchAmount = int(BPM_SuggestCalcRatio * Music[song_number].total_note[difficulty]) + 1;//探索個数 最低1個
	double weight = 0;
	int SearchCount = 0;
	std::sort(bpmList, bpmList + Music[song_number].total_note[difficulty]);  //bpmListを小さい順にソート
	for (i = Music[song_number].total_note[difficulty] - 1; i > Music[song_number].total_note[difficulty] - 1 - SearchAmount; i--, SearchCount++) {
		weight = (double)(SearchCount+0.5) * 2 / SearchAmount;//大きいBPMから0~2で重み付していく
		BPM_suggest += weight*(double)bpmList[i] / SearchAmount;
	}
	Music[song_number].bpm_suggested[difficulty] = (short)(BPM_suggest+0.5);


	//エディタ用処理 #END 
	if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
		score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
		score_cell_insert = score_cell_insert->next;
		score_cell_insert->content = CONTENTS_END;//#ENDを格納
	
		score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
		score_cell_insert = score_cell_insert->next;
		score_cell_insert->content = CONTENTS_END_EDGE;//終端を格納
	}

	Music[song_number].TimeToEnd[difficulty] = int(time_counter_real + Music[song_number].noteoffset[difficulty] + stopTimeSum * 1000);//演奏終了タイミング
	*TimeToEndScroll = int(time_counter + note_offset_scroll);//Scroll時間での演奏終了タイミング
	//降水確率算出
	Cdiff->level = 1;
	//*level_s = 1;
	if (end_time - start_time != 0) {//0除算じゃないとき
		Cdiff->level = int((diff_point * ((double)120000 / ((double)end_time - start_time))) / 10);
		*playing_time = int((end_time - start_time)*pitch);
    		//printf(L"%d\n", song_time);
		Cdiff->level = (Cdiff->level);
	}

	//DIFFICULTY_RADAR,NotesAmount算出
	DifficultyRadar DR(note, nc, bpmchange, stopSequence, end_time - start_time, start_time, end_time, timing_same, Music[song_number].ColorNotesAmount[difficulty], BPM_suggest);
	DR.GetLocalNotesGraph(Music[song_number].LocalNotesAmount[difficulty]);//音符密度グラフを得る

	Cdiff->global = DR.CalcGlobal(CALC_MODE_NORMAL);
	Cdiff->local = DR.CalcLocal(CALC_MODE_NORMAL);
	Cdiff->color = DR.CalcColor(start_time, end_time, CALC_MODE_NORMAL);
	Cdiff->longNote = DR.CalcLongNote(CALC_MODE_NORMAL);
	Cdiff->unstability = DR.CalcUnstability();
	Cdiff->chain = DR.CalcChain();

	Music[song_number].global[CALC_MODE_NORMAL][difficulty] = Cdiff->global;
	Music[song_number].local[CALC_MODE_NORMAL][difficulty] = Cdiff->local;
	Music[song_number].color[CALC_MODE_NORMAL][difficulty] = Cdiff->color;
	Music[song_number].longNote[CALC_MODE_NORMAL][difficulty] = Cdiff->longNote;
	Music[song_number].unstability[CALC_MODE_NORMAL][difficulty] = Cdiff->unstability;
	Music[song_number].chain[CALC_MODE_NORMAL][difficulty] = Cdiff->chain;

	//Cdiff->global = DR.CalcGlobal(CALC_MODE_RAINBOW);
	//Cdiff->local = DR.CalcLocal(CALC_MODE_RAINBOW);
	//Cdiff->color = DR.CalcColor(start_time, end_time, CALC_MODE_RAINBOW);
	//Cdiff->longNote = DR.CalcLongNote(CALC_MODE_RAINBOW);

	Music[song_number].global[CALC_MODE_RAINBOW][difficulty] = Cdiff->global;
	Music[song_number].local[CALC_MODE_RAINBOW][difficulty] = Cdiff->local;
	Music[song_number].color[CALC_MODE_RAINBOW][difficulty] = Cdiff->color;
	Music[song_number].longNote[CALC_MODE_RAINBOW][difficulty] = Cdiff->longNote;
	Music[song_number].unstability[CALC_MODE_RAINBOW][difficulty] = Cdiff->unstability;
	Music[song_number].chain[CALC_MODE_RAINBOW][difficulty] = Cdiff->chain;


	/*
	Cdiff->global = DR.CalcGlobal(CALC_MODE_RAINBOW);
	Cdiff->local = DR.CalcLocal(CALC_MODE_RAINBOW);
	Cdiff->color = DR.CalcColor(start_time, end_time, CALC_MODE_RAINBOW);
	Cdiff->longNote = DR.CalcLongNote(CALC_MODE_RAINBOW);
	Cdiff->unstability = DR.CalcUnstability();
	Cdiff->chain = DR.CalcChain();

	Music[song_number].global[CALC_MODE_RAINBOW][difficulty] = Cdiff->global;
	Music[song_number].local[CALC_MODE_RAINBOW][difficulty] = Cdiff->local;
	Music[song_number].color[CALC_MODE_RAINBOW][difficulty] = Cdiff->color;
	Music[song_number].longNote[CALC_MODE_RAINBOW][difficulty] = Cdiff->longNote;
	Music[song_number].unstability[CALC_MODE_RAINBOW][difficulty] = Cdiff->unstability;
	Music[song_number].chain[CALC_MODE_RAINBOW][difficulty] = Cdiff->chain;
	*/


	//オプションで加工された譜面をcopyに格納、その後noteに戻す
	if (Option->op.lane == Option->OP_LANE_NONE) {//NONE
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= nc[i] - 1; j++) {
				copy[i][j] = note[i][j];
			}
		}
	}
	if (Option->op.lane == Option->OP_LANE_MIRROR) {//MIRROR
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= nc[i] - 1; j++) {
				copy[3 - i][j] = note[i][j];
			}
		}
	}
	if (Option->op.lane == Option->OP_LANE_RANDOM) {//RANDOM
		random_kaburanai(op_lane, 4, 0, 3, ln_use_s, ln_use_c);

		while ((op_lane[0] == 0 && op_lane[1] == 1 && op_lane[2] == 2 && op_lane[3] == 3) ||//正規または鏡譜面になってしまったときはやり直し
			(op_lane[0] == 3 && op_lane[1] == 2 && op_lane[2] == 1 && op_lane[3] == 0)) {
			random_kaburanai(op_lane, 4, 0, 3, ln_use_s, ln_use_c);
		}

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= nc[i] - 1; j++) {
				copy[op_lane[i]][j] = note[i][j];
			}
		}
	}

	tc = 0;
	//printfDx(L"tc:%d\n",tc);
	//ScreenFlip();
	//nc[x] x列の音符数
	//nc_s[x] x列の今処理しているコピー元の音符番号
	//nc_c[c] x列の今処理しているコピー先の音符番号
	//op_lane[x] 入力x(0~3)を0~3に変化させるための配列

	if (Option->op.lane == Option->OP_LANE_SUPER_RAND) {//SUPER_RANDOM
		while (nc[0] != nc_s[0] || nc[1] != nc_s[1] || nc[2] != nc_s[2] || nc[3] != nc_s[3]) {//各列全ての音符の配置を変えるまで
			random_kaburanai(op_lane, 4, 0, 3, ln_use_s, ln_use_c);//op_lane[]にコピー先レーン番号を入れる
			//printfDx(L"tc:%d\n", tc);
			//ScreenFlip();
			for (i = 0; i <= 3; i++) {//元の譜面のiレーン
				if (timing_same[tc] == note[i][nc_s[i]].timing) {//縦に見てtc行目で音符を見つけた
					if (op_lane[i] != -1) {//-1になるのは元の譜面のLN終点のとき
						if (note[i][nc_s[i]].group != 2) {//LN終点以外
							copy[op_lane[i]][nc_c[op_lane[i]]] = note[i][nc_s[i]];
							//コピー元のi行nc_s[i]列目のノートをop_lane[i]行nc_c[op_lane[i]]列目のコピー先に格納
						}
						if (note[i][nc_s[i]].group == 1) {//LN始点 を見つけると終点もすぐにコピーする
							ln_use_s[i] = 1;//元の譜面のiレーンはLN中
							ln_use_c[op_lane[i]] = 1;//コピー先のop_lane[i]レーンはLN中なので音符を置けない
							lanestolanec[i] = op_lane[i];//元の譜面のiレーンのLNはコピー先のop_lane[i]レーンと対応している
							copy[op_lane[i]][nc_c[op_lane[i]] + 1] = note[i][nc_s[i] + 1];//終点を同じ場所に移す
							nc_c[op_lane[i]]++;//終点の分インクリメント
						}

						nc_c[op_lane[i]]++;//コピー先の音符番号++
					}
					
					if (note[i][nc_s[i]].group == 2) {//LN終点
						ln_use_s[i] = 0;//iレーンはLN終了
						ln_use_c[lanestolanec[i]] = 0;
						lanestolanec[i] = 0;
					}
					
					
					nc_s[i]++;
				}
			}
			tc++;
		}
	}

	for (i = 0; i <= 3; i++) {//noteに戻す
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
			note[i][j] = copy[i][j];
		}
	}


	/*
	for (i = 0; i <= 3; i++) {//ブレ
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
			double r = 1 + (double)(rand() % 50) / 100;
			note[i][j].bpm *= r;
		}
	}
	*/



	if (Option->op.color == Option->OP_COLOR_RGB_CHANGE) {//RGB CHANGE
		k = rand() % 5;//0~4
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				note[i][j].color = rgb_change[k][note[i][j].color];
				note[i][j].color_init = note[i][j].color;
			}
		}
	}

	if (Option->op.color == Option->OP_COLOR_RGB_RAND) {//RGB RAND

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				k = rand() % 6;//0~5
				note[i][j].color = rgb_change[k][note[i][j].color];
				note[i][j].color_init = note[i][j].color;
				if (note[i][j].group == 1) {
					j++;
					note[i][j].color = rgb_change[k][note[i][j].color];
					note[i][j].color_init = note[i][j].color;
				}

			}
		}
	}

	/*
	if (Option->op.color == 2) {//RGB RAND

	for (i = 1; i <= 4; i++) {
	for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
	k = rand() % 3 + 1;//1~3
	if (note[i][j].color >= 1 && note[i][j].color <= 3) {
	note[i][j].color = k;
	note[i][j].color_init = note[i][j].color;
	if (note[i][j].group == 1) {
	j++;
	note[i][j].color = k;
	note[i][j].color_init = note[i][j].color;
	}
	}
	}
	}
	}
	*/

	/*
	if (Option->op.color == 3) {//RGBCMY RAND

	for (i = 1; i <= 4; i++) {
	for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
	k = rand() % 6 + 1;//1~6
	if (note[i][j].color >= 1 && note[i][j].color <= 6) {
	note[i][j].color = k;
	note[i][j].color_init = note[i][j].color;
	if (note[i][j].group == 1) {
	j++;
	note[i][j].color = k;
	note[i][j].color_init = note[i][j].color;
	}
	}
	}
	}
	}
	*/

	if (Option->op.color == Option->OP_COLOR_SUPER_RAND) {//SUPER RAND

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				k = rand() % 7 + 1;//1~7
				if (note[i][j].color >= 1 && note[i][j].color <= 7) {
					note[i][j].color = k;
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
			}
		}
	}





	if (Option->op.color == Option->OP_COLOR_RGB_ONLY) {//RGB OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color == 4 && note[i][j].group != 2) {
					note[i][j].color = (k = rand() % 2 + 1);//Yは12どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 5 && note[i][j].group != 2) {
					note[i][j].color = (k = rand() % 2 + 2);//Cは23どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 6 && note[i][j].group != 2) {
					note[i][j].color = (k = (rand() % 2) * 2 + 1);//Mは13どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 7 && note[i][j].group != 2) {
					note[i][j].color = (k = rand() % 3 + 1);//Wは123どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}


			}
		}
	}

	if (Option->op.color == Option->OP_COLOR_CMY_ONLY) {//CMY OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color == 1 && note[i][j].group != 2) {
					note[i][j].color = (k = (rand() % 2) * 2 + 4);//Rは46どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 2 && note[i][j].group != 2) {
					note[i][j].color = (k = rand() % 2 + 4);//Gは45どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 3 && note[i][j].group != 2) {
					note[i][j].color = (k = (rand() % 2) + 5);//Bは56どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
				if (note[i][j].color == 7 && note[i][j].group != 2) {
					note[i][j].color = (k = rand() % 3 + 4);//Wは456どっちか
					note[i][j].color_init = note[i][j].color;
					if (note[i][j].group == 1) {
						j++;
						note[i][j].color = k;
						note[i][j].color_init = note[i][j].color;
					}
				}
			}
		}
	}

	if (Option->op.color == Option->OP_COLOR_W_ONLY) {//W OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color >= 1 && note[i][j].color <= 7)note[i][j].color = 7;//KF以外Wにする
				note[i][j].color_init = note[i][j].color;
			}
		}
	}

	if (Option->op.color == Option->OP_COLOR_RAINBOW) {//RAINBOW

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color >= 1 && note[i][j].color <= 7)note[i][j].color = 9;//KF以外Fにする
				note[i][j].color_init = note[i][j].color;
			}
		}
	}
	printfDx(L"diff_point:%f\n", diff_point);
	printfDx(L"level_s:%d\n", Cdiff->level);
	//ScreenFlip();
	//Sleep(1000);

	clsDx();
	//ScreenFlip();


	/*
	while (1) {
	if (ProcessMessage() != 0) {
	DxLib_End();
	exit(0);
	return;
	}
	}
	*/

	/*
	for (i = 1; i <= 4; i++) {
	note[i][0].timing = 0;
	note[i][0].color = 1;
	note[i][0].x = lane[i];
	note[i][0].bpm = 120;
	}
	*/


	//printfDx(L"ハッシュ値:%d\n", *hash);
	//ScreenFlip();
	//Sleep(1000);

	free(bpmList);
	free(timing_same);

	for (i = 0; i <= 3; i++) {
		free(copy[i]);
	}
	free(copy);
	FileRead_close(fp);

	return;
}

void random_kaburanai(int *box, int nae, int min, int max, int *ln_use_s, int *ln_use_c) {//要素数naeのbox配列にランダムに被らないようにminからmaxまでの数値を格納する
	int i = 0;
	int j = 0;
	int kaburi = 0;//被ってるなら1

	if (nae > (max - min + 1)) {//要素数が大きすぎるなら強制終了
		return;
	}

	//clsDx();
	//printfDx(L"%d,%d,%d,%d\n", ln_use_s[0], ln_use_s[1], ln_use_s[2], ln_use_s[3]);
	//ScreenFlip();

	do {//コピー元の1レーン目のコピー先レーンを決める
		if (ln_use_s[0] == 0) {
			box[0] = rand() % (max - min + 1) + min;//min~maxでランダムに

		}
		else {
			box[0] = -1;//コピー元がLNの途中なのでレーン移動考える必要無し(数字は使わない-1にしておく)
			break;//1レーン目での移動は無いのでwhile抜ける
		}
	} while (ln_use_c[box[0]] == 1);//決めたコピー先がLN中の時はやり直し


	for (i = 1; i <= nae - 1; i++) {
		do {
			kaburi = 0;
			if (ln_use_s[i] == 0) {
				box[i] = rand() % (max - min + 1) + min;//min~maxでランダムに
				for (j = 0; j <= i - 1; j++) {
					kaburi = (kaburi || (box[j] == box[i]));
				}
				kaburi = (kaburi || ln_use_c[box[i]]);
			}
			else {
				box[i] = -1;//コピー元がLNの途中なのでレーン移動考える必要無し(数字は使わない-1にしておく)
			}

		} while (kaburi == 1);
	}

	return;
}

static void bpm_change_add(double bpm,BPMC *bpmchange,int *bpm_seq_counter,double time_counter,double note_offset_scroll,double *bc_timing_real,double *bc_timing,double scroll,double stopTimeSum) {
	bpmchange[*bpm_seq_counter].timing = (time_counter + note_offset_scroll);

	//note[0][*bpm_seq_counter].timing = int((time_counter + note_offset_scroll));

	*bc_timing_real += ((double)bpmchange[*bpm_seq_counter].timing - (double)*bc_timing) / scroll;

	bpmchange[*bpm_seq_counter].timing_real = *bc_timing_real
		+ stopTimeSum * 1000;//実際の変更タイミングを格納

	//note[0][*bpm_seq_counter].timing = int(*bc_timing_real
		//+ stopTimeSum * 1000);//実際の変更タイミングを格納
	*bc_timing = (time_counter + note_offset_scroll);

	bpmchange[*bpm_seq_counter].bpm = (bpm * scroll);//scroll倍する
	bpmchange[*bpm_seq_counter].use = 1;//このノートを使っているとするためのフラグ


	//note[0][*bpm_seq_counter].bpm = (float)(bpm * scroll);//scroll倍する
	//note[0][*bpm_seq_counter].color = 1;//このノートを使っているとするためのフラグ
	(*bpm_seq_counter)++;
}

DifficultyRadar::DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE *stopSequence,int time, int StartTime, int EndTime, int *TimingSame, short* NotesAmount, double BPM_suggest) {
	DifficultyRadar::note = note;
	DifficultyRadar::nc = nc;
	DifficultyRadar::bpmchange = bpmchange;
	DifficultyRadar::stopSequence = stopSequence;
	DifficultyRadar::time = time;
	DifficultyRadar::StartTime = StartTime;
	DifficultyRadar::EndTime = EndTime;
	DifficultyRadar::TimingSame = TimingSame;
	DifficultyRadar::BPM_suggest = BPM_suggest;

	int i = 0;
	for (i = 0; i < 9; i++) {//各色音符数の初期化
		NotesAmount[i] = 0;
	}

	int lane = 0, NoteCounter = 0;
	//音符の合計を取得
	for (lane = 0; lane <= 3; lane++) {
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1) {
				if (note[lane][NoteCounter].color != 8) {

					if (note[lane][NoteCounter].color == 1 ||
						note[lane][NoteCounter].color == 2 ||
						note[lane][NoteCounter].color == 3) {//RGB
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 4 ||
						note[lane][NoteCounter].color == 5 ||
						note[lane][NoteCounter].color == 6) {//CMY
						TotalNotes += 2;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 7) {//W
						TotalNotes += 3;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == 9) {//F
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
				}
				TotalNotesK++;
				NotesAmount[NumberTranslation(note[lane][NoteCounter].color)]++;//各色がいくつあるか加算
			}
			if (note[lane][NoteCounter].group == 2 && note[lane][NoteCounter].LN_k == 1) {//LN終端の黒
				NotesAmount[NumberTranslation(8)]++;//各色がいくつあるか加算
			}
		}
	}

	for (i = 0; i < 9; i++) {//各色音符数を平均密度にする
		NotesAmount[i] = int(200 * log10((double)NotesAmount[i] * 30000 / (DifficultyRadar::time) + 1));
	}

}

void DifficultyRadar::GetLocalNotesGraph(short *LocalNotesGraph) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//局所色難易度
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing sameのカウンター
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//その区間の最初の音符のタイミング
	int et = 0;//その区間の最後の音符のタイミング


	//BoxTime秒の枠をずらしながら枠に入る音符の密度の最高値をLocal値とする
	int BoxTime = int((double)time/9 + 0.5);//枠の時間(ms)
	int SlideTime = BoxTime;//枠をスライドする時間
	int Count[4] = { 0,0,0,0 };//枠の中に入っている音符の数
	int SlideCount = 0;//スライド回数カウント

	int CountToSlide = int((double)time / SlideTime + 1);//スライドする回数

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };

	do {
		//4レーンそれぞれの枠内音符数を計算
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//新しく枠内に入ってきた音符数を足す
				if (nc_last[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != 8) {//枠終わりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_last[ind]].color == 1 ||
						note[ind][nc_last[ind]].color == 2 ||
						note[ind][nc_last[ind]].color == 3) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == 4 ||
						note[ind][nc_last[ind]].color == 5 ||
						note[ind][nc_last[ind]].color == 6) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == 7) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == 9) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//枠内から出ていった音符数を引く
				if (nc_start[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//枠内から出ていった音符数を引く
					&& note[ind][nc_start[ind]].color != 8) {//枠始まりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_start[ind]].color == 1 ||
						note[ind][nc_start[ind]].color == 2 ||
						note[ind][nc_start[ind]].color == 3) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == 4 ||
						note[ind][nc_start[ind]].color == 5 ||
						note[ind][nc_start[ind]].color == 6) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == 7) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == 9) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		if (SlideCount <= 8) {
			LocalNotesGraph[SlideCount] = int(25000*(double)NoteCount / BoxTime);//音符密度グラフを作成
		}

		SlideCount++;
	} while (SlideCount < CountToSlide);

	return;
}

int DifficultyRadar::NumberTranslation(int color) {
	int val = color;

	switch (val)
	{
	default:
		break;
	case 4:
		val = 6;//y
		break;
	case 5:
		val = 4;//y
		break;
	case 6:
		val = 5;//y
		break;
	}

	val--;//0~8にする

	return val;
}


int DifficultyRadar::CalcGlobal(int Rainbow) {
	double GlobalScore = 0;
		
	if (Rainbow == 0) {
		GlobalScore = TotalNotes;
	}
	else if(Rainbow == 1) {
		GlobalScore = TotalNotesRainbow;
	}

	double LN_point = 545.454545454545;//LNの1pointあたりの秒数(ms)
	double color_point[11] = { 0,1,1,1,2,2,2,3,0,0.9,0 };//音符の色毎の難易度ポイント
	double cp_CAP = 1.333;//光る音符の難易度ポイント倍率
	int time_use = time;
	int time_limit = 12000;//これ以下の時間で密度を出さない


	if (time_use <= time_limit)time_use = time_limit;

	//1分あたりの総ノート数を算出
	GlobalScore = (GlobalScore / ((double)time_use / 1000)) * 60;

	return (int)(GlobalScore * 100 / globalMax);
}

int DifficultyRadar::CalcLocal(int Rainbow) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//局所色難易度
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing sameのカウンター
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//その区間の最初の音符のタイミング
	int et = 0;//その区間の最後の音符のタイミング


	//BoxTime秒の枠をずらしながら枠に入る音符の密度の最高値をLocal値とする
	int BoxTime = 8000;//枠の時間(ms)
	int SlideTime = 250;//枠をスライドする時間
	int Count[4] = { 0,0,0,0 };//枠の中に入っている音符の数
	int SlideCount = 0;//スライド回数カウント

	int CountToSlide = int((double)time / SlideTime + 1);//スライドする回数

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };
	if (Rainbow == 1) {
		NoteCountVal[RGB] = 1;
		NoteCountVal[CMY] = 1;
		NoteCountVal[W] = 1;
		NoteCountVal[F] = 1;
	}

	do {
		//4レーンそれぞれの枠内音符数を計算
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//新しく枠内に入ってきた音符数を足す
				if (nc_last[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != 8) {//枠終わりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_last[ind]].color == 1 ||
						note[ind][nc_last[ind]].color == 2 ||
						note[ind][nc_last[ind]].color == 3) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == 4 ||
						note[ind][nc_last[ind]].color == 5 ||
						note[ind][nc_last[ind]].color == 6) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == 7) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == 9) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//枠内から出ていった音符数を引く
				if (nc_start[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//枠内から出ていった音符数を引く
					&& note[ind][nc_start[ind]].color != 8) {//枠始まりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_start[ind]].color == 1 ||
						note[ind][nc_start[ind]].color == 2 ||
						note[ind][nc_start[ind]].color == 3) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == 4 ||
						note[ind][nc_start[ind]].color == 5 ||
						note[ind][nc_start[ind]].color == 6) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == 7) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == 9) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		//LocalColor = CalcColor(SlideTime * SlideCount, BoxTime + SlideTime * SlideCount);//局所Colorの算出

		//Localを算出
		LocalTime = BoxTime;
		if (LocalTime != 0) {
			LocalScoreBuf = ((NoteCount + LocalColor*0) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}
		SlideCount++;
	} while (SlideCount < CountToSlide);


	/*
	while (NoteCount <= (double)TotalNotes / 10) {//まず局所相当の音符数をK以外で何個か探索(1000ノーツなら100個)
		ind = index(note, nc_last, nc);
		if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
			&& note[ind][nc_last[ind]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない
			
			if (NoteCount == 0)st = note[ind][nc_last[ind]].timing_real;//最初の音符のタイミングを格納
			et = note[ind][nc_last[ind]].timing_real;//最後の音符のタイミングを格納
			NoteCount++;
		}
		nc_last[ind]++;//下から順番に探索
	}

	//暫定局所密度計算
	LocalTime = et - st;
	if (LocalTime != 0) {
		LocalScore = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
	}

	int find = 0;

	while (NoteCount <= TotalNotes) {//全ノーツ探索したら終了

		find = 0;
		while (find == 0) {//探索の上側　k以外の単,LN始点を見つけるまで探索
			ind = index(note, nc_last, nc);
			if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
				&& note[ind][nc_last[ind]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない
				et = note[ind][nc_last[ind]].timing_real;//最後の音符のタイミングを格納
				NoteCount++;
				find = 1;
			}
			if (nc_last[ind] <= nc[ind]) {//最大値を超えないようにする
				nc_last[ind]++;//下から順番に探索
			}
		}

		find = 0;
		while (find == 0) {//探索の下側　k以外の単,LN始点を見つけるまで探索
			ind_s = index(note, nc_start, nc);
			if ((note[ind_s][nc_start[ind_s]].group == 0 || note[ind_s][nc_start[ind_s]].group == 1)
				&& note[ind_s][nc_start[ind_s]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない
				st = note[ind_s][nc_start[ind_s]].timing_real;//最後の音符のタイミングを格納
				find = 1;
			}
			if (nc_start[ind_s] <= nc[ind_s]) {//最大値を超えないようにする
				nc_start[ind_s]++;//下から順番に探索
			}
		}

		LocalTime = et - st;
		if (LocalTime != 0) {
			LocalScoreBuf = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}

	}
	*/

	return (int)(LocalScore * 100 / localMax);
}

int DifficultyRadar::index(NOTE** note, int* nc, int *ncMax) {//探索を終えているレーン(nc>ncMAX)については無視する
	int index = 0;
	int timing = 0;

	if (nc[0] <= ncMax[0]) {
		index = 0;
		timing = note[0][nc[0]].timing;
	}
	else if (nc[1] <= ncMax[1]) {
		index = 1;
		timing = note[1][nc[1]].timing;
	}
	else if (nc[2] <= ncMax[2]) {
		index = 2;
		timing = note[2][nc[2]].timing;
	}
	else if (nc[3] <= ncMax[3]) {
		index = 3;
		timing = note[3][nc[3]].timing;
	}


	for (int i = 1; i <= 3; i++) {
		if (timing > note[i][nc[i]].timing && nc[i] <= ncMax[i]) {
			index = i;
			timing = note[i][nc[i]].timing;
		}
	}
	return index;
}

int DifficultyRadar::CalcColor(int StartTime, int EndTime, int Rainbow) {//色難易度
	double ColorChangeCount = 0;
	int lane = 0, NoteCounter = 0;
	int ColorBuf[4] = { 0,0,0,0 };
	int LocalTime = EndTime - StartTime;
	

	ColorBuf[0] = note[0][0].color;
	ColorBuf[1] = note[1][0].color;
	ColorBuf[2] = note[2][0].color;
	ColorBuf[3] = note[3][0].color;
	if (Rainbow == 1) {
		for (int i = 0; i <= 3; i++) {
			if (ColorBuf[i] >= 1 && ColorBuf[i] <= 7) {//黒虹以外の音符は虹として扱う
				ColorBuf[i] = 9;
			}
		}
	}

	int NoteColor = 0;//今見ている音符の色
	int k_flag = 0;//前の音符が黒だった

	int IndexStart[4] = { 0,0,0,0 };//Colorを算出する区間内の最初のノートの番号
	int IndexEnd[4] = { 0,0,0,0 };//Colorを算出する区間内の最後のノートの番号

	//Colorを算出する区間内の最初,最後のノートの番号を決定
	for (lane = 0; lane <= 3; lane++) {
		while (note[lane][IndexStart[lane]].timing_real < StartTime) {
			IndexStart[lane]++;
			if (IndexStart[lane] >= nc[lane]) {
				IndexStart[lane]--;
				break;
			}
		}
		while (note[lane][IndexEnd[lane]].timing_real < EndTime) {
			IndexEnd[lane]++;
			if (IndexEnd[lane] > nc[lane])break;
		}
		IndexEnd[lane]--;
	}


	for (lane = 0; lane <= 3; lane++) {
		for (NoteCounter = IndexStart[lane]; NoteCounter <= IndexEnd[lane]; NoteCounter++) {
			if ((note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1)) {//単ノートかLN始点
				NoteColor = note[lane][NoteCounter].color;
				if (Rainbow == 1) {
					if (NoteColor >= 1 && NoteColor <= 7) {//虹モードは黒虹以外の音符は虹として扱う
						NoteColor = 9;
					}
				}

				if (ColorBuf[lane] != NoteColor) {//前の音符と色が違う
					if (NoteColor == 4 ||
						NoteColor == 5 ||
						NoteColor == 6) {//cmyなら4倍の重み
						ColorChangeCount += 4;
						ColorBuf[lane] = NoteColor;
						k_flag = 0;
					}
					else if (NoteColor == 7) {//wなら2倍の重み
						ColorChangeCount += 2;
						ColorBuf[lane] = NoteColor;
						k_flag = 0;
					}
					else if (NoteColor == 8 && k_flag == 0) {//前がk以外で今回kなら0.5倍の重み
						ColorChangeCount += 0.5;
						k_flag = 1;
					}
					else if (NoteColor == 9) {//fなら0.75倍の重み
						ColorChangeCount += 0.75;
						ColorBuf[lane] = NoteColor;
						k_flag = 0;
					}
					else {//RGBの場合は1倍の重み
						ColorBuf[lane] = NoteColor;
						ColorChangeCount += 1;
						k_flag = 0;
					}
				}
			}
			else if (note[lane][NoteCounter].group == 2 && note[lane][NoteCounter].LN_k == 1) {//黒終点の時
				ColorChangeCount += 0.5;
				k_flag = 1;
			}
		}
	}


	ColorChangeCount = (ColorChangeCount / ((double)LocalTime / 1000)) * 60;
	//ColorChangeCount = ColorChangeCount / TotalNotesK;//譜面単位でどれだけ色が複雑か

	return (int)(ColorChangeCount * 100 / colorMax);
}
int DifficultyRadar::CalcLongNote(int Rainbow) {
	int lane = 0;
	double LN[4] = { 0,0,0,0 };//各レーンのLN度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double LNBuf = 0;

	int weight = 1;//音符の色で重み付け (RGB:1 CMY:2 W:3)
	int WeightOfColor[3] = { 1,2,3 };//それぞれの音符グループの重み
	const int RGBF = 0;
	const int CMY = 1;
	const int W = 2;
	if (Rainbow == 1) {
		WeightOfColor[RGBF] = 1;
		WeightOfColor[CMY] = 1;
		WeightOfColor[W] = 1;
	}


	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].group == 1) {
				TimingBuf = note[lane][NoteCounter].timing_real;
				if (note[lane][NoteCounter].color == 1 || note[lane][NoteCounter].color == 2 || note[lane][NoteCounter].color == 3 || note[lane][NoteCounter].color == 9)weight = WeightOfColor[RGBF];
				if (note[lane][NoteCounter].color == 4 || note[lane][NoteCounter].color == 5 || note[lane][NoteCounter].color == 6)weight = WeightOfColor[CMY];
				if (note[lane][NoteCounter].color == 7)weight = WeightOfColor[W];
			}
			if (note[lane][NoteCounter].group == 2) {
				LN[lane] += (double)weight*((double)note[lane][NoteCounter].timing_real - TimingBuf)/1000;//LNの秒数を加算
			}
		}
	}

	double LNSum = LN[0] + LN[1] + LN[2] + LN[3];//総LN秒数を計算
	LNSum *= 0.015;
	LNSum = (log(LNSum + 1) / log(2));
	LNSum *= 70;
	LNSum = (LNSum / ((double)time / 1000)) * 60;//1分あたりのLN密度にする
	return (int)(LNSum * 100 / LNMax);
}
int DifficultyRadar::CalcUnstability() {
	//BPMについて算出
	int BpmCount = 0;
	double Unstability = 0;

	BpmCount = 0;
	double BpmBuf = bpmchange[BpmCount].bpm;
	double BPMChangeSum = 0;//BPM変化量

	/*
	while (bpmchange[BpmCount].use == 1) {
		if (BpmBuf != bpmchange[BpmCount].bpm) {//BPM変化があった
			BPMChangeSum += fabs((log(bpmchange[BpmCount].bpm + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2))) * 2;//BPM変化の重みは2倍
			//2の対数をとりBPMを比較 倍の関係になっていたら1加算
			BpmBuf = bpmchange[BpmCount].bpm;
		}
		BpmCount++;
	}
	BPMChangeSum = 0;//不使用
	*/

	//Stopについて算出
	int StopCount = 0;
	double stopSum = 0;

	while (stopSequence[StopCount].use == 1) {
		stopSum += fabs(stopSequence[StopCount].stop_time);//停止時間を加算
		StopCount++;
	}

	Unstability = BPMChangeSum + stopSum*0.5 + (double)StopCount*4;//BPM,HSとの重みバランス調節


	//HS,瞬間風速から離れた速さの音符について算出
	int lane = 0;
	double HS[4] = { 0,0,0,0 };//各レーンのHS度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
    BpmBuf = 0;

	double OutlierAmount = 0;//瞬間風速から外れた音符の度合い

	//レーン毎の音符速度変化を見る
	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].color != 8) {//黒は除外
				//HS,BPM変化について
				if (firstFlag == 0) {
					BpmBuf = note[lane][NoteCounter].bpm_real;
					firstFlag = 1;
				}
				else if (BpmBuf != note[lane][NoteCounter].bpm_real) {//BPM変化(HS変化)があった
					HS[lane] += fabs((log(note[lane][NoteCounter].bpm_real + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2)));
					//2の対数をとりBPMを比較 倍の関係になっていたら1加算
					BpmBuf = note[lane][NoteCounter].bpm_real;
				}

				//瞬間風速から外れた音符の度合いについて
				OutlierAmount += abs(log((note[lane][NoteCounter].bpm_real + 0.1) / (BPM_suggest + 0.1)) / log(2));//瞬間風速から外れた速さの音符に重みを付けて加算
			}
		}
	}

	double HS_sum = (HS[0] + HS[1] + HS[2] + HS[3]);//BPM変化度

	//値の大きさを調整
	OutlierAmount = OutlierAmount * 16 / ((double)time / 1000);
	//一秒あたりにどれだけ重み付け瞬間風速外れ音符があるかを計算
	//OutlierAmount = OutlierAmount / ((double)time / 1000);

	Unstability += OutlierAmount;//重み付け瞬間風速外れ音符密度を足す

	Unstability += HS_sum;//BPM変化と釣り合いをとるため4で割る

	Unstability *= (7.0 / unstabilityMax);//0~7ぐらいに収める

	Unstability = (log(Unstability + 1) / log(2));//0~7が0~3に対応する対数 変化数が少ないうちは値が多く上がるようにする

	Unstability *= 51.666 * 0.7;

	//Unstability = (Unstability / ((double)time / 1000)) * 60;//1分あたりの対数BPM変化密度にする
	return (int)(Unstability);
}
int DifficultyRadar::CalcChain() {
	int lane = 0;
	double chain[4] = { 0,0,0,0 };//各レーンの縦連度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double chainBuf = 0;

	int BaseBPM = 180;//16分を縦連とみなす基準BPM

	double normalizer = 1.0 / (1000.0 * ((60.0 / BaseBPM) / 4.0));
	

	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if ((note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1) && note[lane][NoteCounter].color != 8) {
				if (firstFlag == 0) {
					TimingBuf = note[lane][NoteCounter].timing_real;
					firstFlag = 1;
				}
				else {
					chainBuf = (double)1 / pow((((double)note[lane][NoteCounter].timing_real - TimingBuf) * normalizer), 2);//BaseBPMの16分より短いと1からどんどん上がっていく
					if (chainBuf >= 1)chainBuf = 1;//1以上にはしない
					chain[lane] += chainBuf;//
					TimingBuf = note[lane][NoteCounter].timing_real;
				}
			}
		}
	}

	double chainSum = chain[0] + chain[1] + chain[2] + chain[3];

	chainSum = (chainSum / ((double)time / 1000)) * 60;//1分あたりの縦連密度にする

	return (int)(chainSum * 100 / chainMax);
}