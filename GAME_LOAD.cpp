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
#include <algorithm>
#include "EDIT_SCORE.h"
#include "NPStringUtil.h"
#include "DxLibUtil.h"
#include "show_something.h"
#include "AutoDifficultyPrediction.h"
#include "DifficultyRadar.h"

void GAME_LOAD(int song_number,
	int difficulty,
	NOTE **note,
	BARLINE *barline,
	int *laneCordinateX,
	int readflag,
	CALCDIFF *Cdiff,
	Option  *Option,
	BPMC *bpmchange,
	SC *scrollchange,
	STOP_SE *stopSequence,
	Song *Music,
	SongSub *MusicSub,
	int *TimeToEndScroll,
	int *playing_time,
	Config config,
	double pitch,
	SCORE_CELL *score_cell_head,//EDIT_SCORE関数用の譜面情報格納リスト
	int SkillTestFlag
	) {//譜面の読み込み

	AutoDifficultyPrediction adp;
	float autoDifficultyPredictionResult = 0;
	auto setAiPredictedDifficultyIfEnable = [&]() {
		//自動難易度算出
		if (config.UseAiPredictedDifficulty == 1) {
			autoDifficultyPredictionResult = adp.getDifficulty(Music[song_number], difficulty);
			Music[song_number].level[difficulty] = autoDifficultyPredictionResult;
		}
	};

	double step_counter = 0;//score_cellのstep値を保存
	SCORE_CELL *score_cell_insert = score_cell_head;//このポインタの次にscore_cellセルを挿入

	int hash_M = 99999999;//譜面ハッシュ値の最大値

	int i = 0, j = 0, lane = 0, n = 0, dataArrayLength = 0;
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

	std::unique_ptr<vector<float>> bpmList(new std::vector<float>());//最大瞬間BPM算出用のBPMバッファリスト

	wchar_t high_speed_s[5][16];//ハイスピード命令の文字読み取りバッファ
	double high_speed[5] = { 1,1,1,1,1 };//ハイスピード値 (5番目は小節線の速さ)

	double time_counter = 0;//単位はミリ秒
	double time_counter_real = 0;//実際の(GAME_time_passed)経過時間

	int offsetMargin = 50;//音声、動画の先頭を合わせるためのオフセットマージン

	int DN = 0;//Division Number一小節の分割数

	int ln = 0;//レーンナンバー(lane number)
	wchar_t color_list[11] =     { L'0',L'r',L'g',L'b',L'c',L'm',L'y',L'w',L'k',L'f',L'e' };
	wchar_t color_list_CAP[11] = { NULL,L'R',L'G',L'B',L'C',L'M',L'Y',L'W',NULL,L'F',L'E' };

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
		double Point = 0;
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
	NoteColor rgb_change[6][10] = { 
	{ NoteColor::NONE,NoteColor::R,NoteColor::B,NoteColor::G,NoteColor::C,NoteColor::Y,NoteColor::M,NoteColor::W,NoteColor::K,NoteColor::F },
	{ NoteColor::NONE,NoteColor::G,NoteColor::R,NoteColor::B,NoteColor::M,NoteColor::C,NoteColor::Y,NoteColor::W,NoteColor::K,NoteColor::F },
	{ NoteColor::NONE,NoteColor::G,NoteColor::B,NoteColor::R,NoteColor::M,NoteColor::Y,NoteColor::C,NoteColor::W,NoteColor::K,NoteColor::F },
	{ NoteColor::NONE,NoteColor::B,NoteColor::R,NoteColor::G,NoteColor::Y,NoteColor::C,NoteColor::M,NoteColor::W,NoteColor::K,NoteColor::F },
	{ NoteColor::NONE,NoteColor::B,NoteColor::G,NoteColor::R,NoteColor::Y,NoteColor::M,NoteColor::C,NoteColor::W,NoteColor::K,NoteColor::F },
	{ NoteColor::NONE,NoteColor::R,NoteColor::G,NoteColor::B,NoteColor::C,NoteColor::M,NoteColor::Y,NoteColor::W,NoteColor::K,NoteColor::F } };
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

	NoteColor NearColorBuf = NoteColor::NONE;//その行でそのレーンに一番近い左の方にあるノートの色を保存するための変数
	int BeforeColorBuf[4] = { 0,0,0,0 };//レーンごとの前のノートの色を保存するための配列

	//最大同時押し数算出クラス
	class MaxChords {
	private:
		int maxChords = 0;
		int maxChordsRainbow = 0;

		int chords[4] = { 0,0,0,0 };//レーン毎の必要同時押し数
		int isLongNote[4] = { 0,0,0,0 };//ロング中かどうか



		void reset() {
			chords[0] = 0;
			chords[1] = 0;
			chords[2] = 0;
			chords[3] = 0;
		}

		int color2chords(NoteColor color, int previousChords) {
			switch (color)
			{
			case NoteColor::R:
				return 1;
				break;
			case NoteColor::G:
				return 1;
				break;
			case NoteColor::B:
				return 1;
				break;
			case NoteColor::Y:
				return 2;
				break;
			case NoteColor::C:
				return 2;
				break;
			case NoteColor::M:
				return 2;
				break;
			case NoteColor::W:
				return 3;
				break;
			case NoteColor::F:
				return 1;
				break;
			default:
				return 0;
				break;
			}
		}
	public:
		//音符を見つけるたびに呼び出す
		//isInLong:ロングノート始点、中間の時だけTRUEを入れる
		void setChords(int lane, NoteColor color, BOOL isInLong) {
			chords[lane] = color2chords(color, chords[lane]);
			if (isInLong) {
				isLongNote[lane] = TRUE;
			}
			else {
				isLongNote[lane] = FALSE;
			}
		}

		//必ず一行分setChords()を行った後に呼び出す
		void updateMaxChord() {
			int max = chords[0] + chords[1] + chords[2] + chords[3];

			if (max > maxChords) {
				maxChords = max;
			}

			max = (chords[0] > 0) + (chords[1] > 0) + (chords[2] > 0) + (chords[3] > 0);

			if (max > maxChordsRainbow) {
				maxChordsRainbow = max;
			}

			//ロング中のレーン以外の同時押し数を消しておく
			for (int i = 0; i < 4; i++) {
				if (isLongNote[i]) {
					//そのまま
				}
				else {
					chords[i] = 0;
				}
			}
		}

		//譜面読み込み処理が終わった後に呼び出す
		int getMaxChords() {
			return maxChords;
		}
		int getMaxChordsRainbow() {
			return maxChordsRainbow;
		}

	}MaxChords;

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
	Music[song_number].detail = vector<vector<wstring>>(5);//detailメモリ確保

	wcscpy_s(filename, Music[song_number].SongPath[difficulty]);

	//デフォルト色
	Music[song_number].StrColor[difficulty] = colorRatio(255, 255, 255);
	Music[song_number].StrShadowColor[difficulty] = colorRatio(0, 0, 0);

	NoteSearcher noteSercher = NoteSearcher(note);

	fp = FileRead_open(filename);
	if (fp == 0) {//無かったら戻る
				  //printfDx(L"%sのオープンに失敗しました.\n", filename);
		Music[song_number].exist[difficulty] = 0;
		//printfDx(L"%s\n", filename);
		//ScreenFlip();
		//Sleep(10000);
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

		wstring s1, s2;
		NPStringUtil().devideByFirstColon(str, &s1, &s2);
		sprintfDx(sharp1, L"%s", s1.c_str());
		sprintfDx(sharp2, L"%s", s2.c_str());

		if (wcscmp(L"#TITLE", sharp1) == 0) {
			wchar_t DefaultTitle[] = L"\0";
			free(Music[song_number].title[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {
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
			if (wcscmp(L"\0", sharp2) == 0) {
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
			if (wcscmp(L"\0", sharp2) == 0) {
				Music[song_number].genre[difficulty] = (wchar_t*)calloc(strlenDx(DefaultGenre) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].genre[difficulty], L"%s", DefaultGenre);
			}
			else {
				Music[song_number].genre[difficulty] = (wchar_t*)calloc(strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].genre[difficulty], L"%s", sharp2);
			}
		}
		if (wcscmp(L"#DETAIL", sharp1) == 0) {
			wchar_t DefaultDetail[] = L"";

			if (wcscmp(L"\0", sharp2) == 0) {
				Music[song_number].detail[difficulty].push_back(DefaultDetail);
			}
			else {
				Music[song_number].detail[difficulty].push_back(sharp2);
			}
		}
		if (wcscmp(L"#WAV", sharp1) == 0) {
			wchar_t DefaultWav[] = L"\0";
			free(Music[song_number].wavpath[difficulty]);
			if (wcscmp(L"\0", sharp2) == 0) {
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

		}
		if (wcscmp(L"#JACKET", sharp1) == 0) {
			wchar_t DefaultJacket[] = L"\0";

			free(Music[song_number].jacketpath[difficulty]);

			if (wcscmp(L"\0", sharp2) == 0) {
				Music[song_number].jacketpath[difficulty] = (wchar_t*)calloc(strlenDx(DefaultJacket) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath[difficulty], L"%s", DefaultJacket);
			}
			else {

				Music[song_number].jacketpath[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].jacketpath[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
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

		}
		if (wcscmp(L"#MOVIE", sharp1) == 0) {
			wchar_t DefaultMovie[] = L"\0";

			free(Music[song_number].moviePath[difficulty]);

			if (wcscmp(L"\0", sharp2) == 0) {
				Music[song_number].moviePath[difficulty] = (wchar_t*)calloc(strlenDx(DefaultMovie) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].moviePath[difficulty], L"%s", DefaultMovie);
			}
			else {

				Music[song_number].moviePath[difficulty] = (wchar_t*)calloc(strlenDx(Music[song_number].FolderPath) + strlenDx(L"/") + strlenDx(sharp2) + 1, sizeof(wchar_t));
				sprintfDx(Music[song_number].moviePath[difficulty], L"%s/%s", Music[song_number].FolderPath, sharp2);
			}
		}

		if (wcscmp(L"#BPM", sharp1) == 0) {
			Music[song_number].bpm[difficulty] = _wtof(sharp2);
			bpm = Music[song_number].bpm[difficulty];
			bpm *=pitch;
			//printfDx(L"%f\n", bpm);
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

			//printfDx(L"%f\n", bpm);
		}

		if (wcscmp(L"#LEVEL", sharp1) == 0) {
			Music[song_number].level[difficulty] = _wtoi(sharp2);
			//printfDx(L"%d\n", Music[song_number].level[difficulty]);

		}
		if (wcscmp(L"#SONGOFFSET", sharp1) == 0) {
			Music[song_number].songoffset[difficulty] = int((double)_wtoi(sharp2)/pitch) + offsetMargin;
			//printfDx(L"%f\n", Music[song_number].songoffset[difficulty]);

		}
		if (wcscmp(L"#NOTEOFFSET", sharp1) == 0) {
			Music[song_number].noteoffset[difficulty] = int((double)_wtoi(sharp2) / pitch) + offsetMargin + Option->op.noteOffset.getVal();
			note_offset_scroll = Music[song_number].noteoffset[difficulty] * scroll;//表示用オフセット
			//printfDx(L"%f\n", Music[song_number].noteoffset[difficulty]);

		}
		if (wcscmp(L"#MOVIEOFFSET", sharp1) == 0) {
			Music[song_number].movieoffset[difficulty] = int((double)_wtoi(sharp2) / pitch) + offsetMargin;
			//printfDx(L"%f\n", Music[song_number].songoffset[difficulty]);

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
				Music[song_number].StrColor[difficulty] = colorRatio(255,255,255);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				Music[song_number].StrColor[difficulty] = colorRatio(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx(L"%d\n", Music[song_number].StrColor[difficulty]);
		}
		if (wcscmp(L"#SHADOWCOLOR", sharp1) == 0) {
			if (wcscmp(L"\0", sharp2) == 0) {//何も書かれていなかったら白
				Music[song_number].StrShadowColor[difficulty] = colorRatio(0, 0, 0);
			}
			else {
				wchar_t rgb[3][5];
				swscanf_s(sharp2, L"%[^,\n],%[^,\n],%[^,\n]",
					rgb[0], _countof(rgb[0]),
					rgb[1], _countof(rgb[1]),
					rgb[2], _countof(rgb[2]));

				Music[song_number].StrShadowColor[difficulty] = colorRatio(_wtoi(rgb[0]), _wtoi(rgb[1]), _wtoi(rgb[2]));
			}
			//printfDx(L"%d\n", Music[song_number].StrColor[difficulty]);
		}
		if (wcscmp(L"#VERSION", sharp1) == 0) {
			Music[song_number].version[difficulty] = int(_wtof(sharp2)*100 + 0.005);
		}

	}
	if (readflag == 1) {
		setAiPredictedDifficultyIfEnable();

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
			note[i][j] = NOTE();
		}
		for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {//copyの初期化(bpm以外)
			copy[i][j] = NOTE();
		}

	}

	int file_p_first = 0;//小節先頭ファイルポインタ(読み込んでいる小節の先頭を指す)
	file_p_first = FileRead_tell(fp);//最初に読み込む小節の先頭を指す
	i = 0;
	while (wcscmp(L"#END", sharp1) != 0) {//譜面読み込み部
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}

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

			//printfDx(L"%f\n", measure);
			//textLine++;
		}

		if ((wcslen(sharp1) >= 1) && wstring(str).back() != L',') {//最後にカンマがない1文字以上の行(#命令も含めて複数行になる所)を見つけた
			if (sharp1[0] != L'#') {//先頭が#じゃないなら分割数を数える
				i++;//一小節の分割数を数える
			}
		}


		if (wstring(str).front() != L'#' && wstring(str).back() == L',') {//命令行ではなく、「,」がある行を見つけた → 1小節分読み込んだので分割数を決めnoteにデータを入れる
			i++;//「,」行分の分割数を足す
			DN = i;//DN分音符単位になる
			i = 0;

			FileRead_seek(fp, file_p_first, SEEK_SET);//読み込む小節の先頭にファイルポインタを合わせる

			BOOL PutBarline = 1;//このキャッシュ内で小節線をまだ置いていないというフラグ
			while (1) {
				do {//空行は無視
					FileRead_gets(str, 256, fp);//一行取得(\nは入らない)
				} while (wcscmp(str, L"") == 0);

				//1行を:で分割して格納
				sharp2[0] = '\0'; sharp2[1] = '\0'; sharp2[2] = '\0'; sharp2[3] = '\0';
				swscanf_s(str, L"%[^:\n]:%[^:\n]", sharp1, _countof(sharp1), sharp2, _countof(sharp2));

				if (wcscmp(L"#BPM", sharp1) == 0) {//BPM変更点を見つけた
					bpm = _wtof(sharp2) * pitch;
					bpm_change_add(bpm, bpmchange, &bcc, time_counter, note_offset_scroll, &bc_timing_real, &bc_timing, scroll, stopTimeSum);

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

					//note[lane][nc[lane]].timing = int((time_counter + note_offset_scroll));
					//note[lane][nc[lane]].timing_init = int((time_counter + note_offset_scroll));

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

					//note[lane][nc[lane]].timing = int((time_counter + note_offset_scroll));
					//note[lane][nc[lane]].timing_init = int((time_counter + note_offset_scroll));

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

					for (lane = 0; lane < length; lane++) {//レーン
						for (j = 1; j <= 10; j++) {//rからdまでの色を探す(ロングノート終端も探す)
							if (color_list[j] == sharp1[lane] || color_list_CAP[j] == sharp1[lane]) {//キャッシュの[i]行[lane-1]列目の色が何色か(大文字だと音は長く鳴る)
								//printfDx(L"レーン番号:%d\n", lane);
								//printfDx(L"time:%f\n", time_counter);
								//printfDx(L"DN:%d\n", DN);

								if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
									if (insert_flag == 0) {//1行につきセル挿入は一回
										score_insert_cell(score_cell_insert, int(step_counter + 0.5));//エディタ用セル挿入
										score_cell_insert = score_cell_insert->next;

										score_cell_write_note(score_cell_insert, 0, NoteColor::NONE, NoteGroup::Single, 0);//color0で初期化
										score_cell_write_note(score_cell_insert, 1, NoteColor::NONE, NoteGroup::Single, 0);
										score_cell_write_note(score_cell_insert, 2, NoteColor::NONE, NoteGroup::Single, 0);
										score_cell_write_note(score_cell_insert, 3, NoteColor::NONE, NoteGroup::Single, 0);
										insert_flag = 1;
									}
								}

								note[lane][nc[lane]].timing = int((time_counter + note_offset_scroll) * TIMING_SHOW_RATIO + 0.5);//表示用タイミング
								note[lane][nc[lane]].timing_init = note[lane][nc[lane]].timing;//表示用タイミング(保存用)
								note[lane][nc[lane]].timing_real = int(time_counter_real + Music[song_number].noteoffset[difficulty]
									+ stopTimeSum * 1000 + 0.5 + config.VsyncOffsetCompensation);//実際に叩くタイミング
								note[lane][nc[lane]].timing_init_real = note[lane][nc[lane]].timing_real;//実際に叩くタイミング(保存用)

								note[lane][nc[lane]].color = (NoteColor)j;
								note[lane][nc[lane]].color_init = (NoteColor)j;

								note[lane][nc[lane]].x = laneCordinateX[lane];
								note[lane][nc[lane]].bpm = float(high_speed[lane] * bpm );
								note[lane][nc[lane]].bpm_real = float(high_speed[lane] * bpm * scroll);
								if (color_list_CAP[j] == sharp1[lane] && color_list_CAP[10] != sharp1[lane]) {//LN終端以外で大文字で書かれていたら

									note[lane][nc[lane]].isBright = 1;//長く音を鳴らすようにする
									cp_CAP = 1.333;//難易度ポイントを1.333倍にする
								}
								else {
									cp_CAP = 1;//難易度ポイントは等倍
								}

								if (j != 8 && j != 10) {//黒とLN終点以外なら
									if (note[lane][nc[lane]].group != NoteGroup::LongNoteMiddle) {//中間ノーツはコンボ・風速計算対象外
										bpmList->push_back((float)(note[lane][nc[lane]].bpm* scroll));//BPM情報格納
										Music[song_number].total_note[difficulty]++;//総ノート数+1する
									}
								}

								if (IgnoreArea == 1) {//無視するべき音符ならignoreを1にする
									note[lane][nc[lane]].ignore = 1;
								}

								//難易度自動算出処理
								double BeforeSamePoint = 1;
								int la = 0;
								for (la = 0; la <= 3; la++) {
									if (j == BeforeColorBuf[la]) {//この前の音符の色と同じだった
										if (la == lane) {//レーンも同じ
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
								if (lane >= 1) {//2列目以降について
									if (note[lane][nc[lane]].color == NearColorBuf) {//左にある音符と同じ色だったらポイント下げる
										diff_point += BeforeSamePoint * 0.900 * color_point[j] * cp_CAP;//難易度ポイント算出
																						   //1/3として加算

																						   //難易度ポイントバッファに格納していく
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].Point = BeforeSamePoint * color_point[j] * cp_CAP;
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[lane][nc[lane]].timing_real;
									}
									else {//違う色だった
										diff_point += BeforeSamePoint * color_point[j] * cp_CAP;//難易度ポイント普通に加算

																					 //難易度ポイントバッファに格納していく
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].Point = BeforeSamePoint * color_point[j] * cp_CAP;
										diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[lane][nc[lane]].timing_real;
									}
								}
								else {//1列目について
									diff_point += BeforeSamePoint * color_point[j] * cp_CAP;//難易度ポイント加算

																				 //難易度ポイントバッファに格納していく
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].Point = BeforeSamePoint * color_point[j] * cp_CAP;
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[lane][nc[lane]].timing_real;
								}
								NearColorBuf = (NoteColor)j;//この音符の色を保存(後に左の音符扱い)
								if (j != 10)BeforeColorBuf[lane] = j;//このレーンの今回の音符の色を保存(eは除く)



								note[lane][nc[lane]].textLine = textLine + 1;
								if (color_list[0] != sharp1[lane]) {//0以外の時
									if (timing_same[tc] == -1) {//音符のタイミング格納
										timing_same[tc] = note[lane][nc[lane]].timing;

										//sprintf_s(Cdiff_buf[Cdiff_count],"%s",sharp1);//難易度算出用バッファに格納
										//Cdiff_count++;
									}

									if (start_time_flag == 0) {
										start_time = note[lane][nc[lane]].timing_real;
										start_time_flag = 1;
									}
									end_time = note[lane][nc[lane]].timing_real;
								}
								//printfDx(L"レーン:%d 色:%d\n", lane, j);

								if (color_list[10] == sharp1[lane] || color_list_CAP[10] == sharp1[lane]) 
								{//'e'または'E'だったら
									//TODO:LN始点にするノートは一番近い直前のSingleノート

									if (note[lane][nc[lane] - 1].group == NoteGroup::Single) {
										//前の音符が単ノートのときはLN始点にする
										note[lane][nc[lane] - 1].group = NoteGroup::LongNoteStart;
										note[lane][nc[lane] - 1].group_init = NoteGroup::LongNoteStart;
									}else if (note[lane][nc[lane] - 1].group == NoteGroup::LongNoteEnd) {
										//前の音符がLN終点のときはLN中間ノートにする
										note[lane][nc[lane] - 1].group = NoteGroup::LongNoteMiddle;
										note[lane][nc[lane] - 1].group_init = NoteGroup::LongNoteMiddle;
									}

									note[lane][nc[lane]].group = NoteGroup::LongNoteEnd;//終点
									note[lane][nc[lane]].group_init = NoteGroup::LongNoteEnd;//終点

									//前と同じ色で初期化
									note[lane][nc[lane]].color = note[lane][nc[lane] - 1].color;
									note[lane][nc[lane]].color_init = note[lane][nc[lane] - 1].color;

									note[lane][nc[lane]].isBright = note[lane][nc[lane] - 1].isBright;

									if (color_list[10] == sharp1[lane]) {//eなら
										note[lane][nc[lane]].LN_k = 0;//通常終点
										
										if (!sharp2[0] == '\0') {
											for (int colorInd = (int)NoteColor::R; colorInd <= (int)NoteColor::F; colorInd++) {
												if (colorInd == (int)NoteColor::K) { continue; }
												
												//色変化があるか
												if (sharp2[lane] == color_list[colorInd] || sharp2[lane] == color_list_CAP[colorInd]) {
													note[lane][nc[lane]].color = (NoteColor)colorInd;
													note[lane][nc[lane]].color_init = (NoteColor)colorInd;
												}
											}
										}
									
									}
									else if (color_list_CAP[10] == sharp1[lane]) {//Eなら
										note[lane][nc[lane]].LN_k = 1;//黒終点
									}

									

									

									diff_point += (double)cp_CAP * ((double)note[lane][nc[lane]].timing_real - note[lane][nc[lane] - 1].timing_real) / (LN_point);

									//難易度ポイントバッファに格納していく
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].Point = (double)cp_CAP * ((double)note[lane][nc[lane]].timing_real - note[lane][nc[lane] - 1].timing_real) / (LN_point);
									diff_buf[number_ring(total_diff_point_notes, LOCAL_DIFF_AMOUNT - 1)].timing = note[lane][nc[lane]].timing_real;


									//エディタ用処理
									if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
										if (score_cell_find_before_note(score_cell_insert, lane)->data.note.group[lane] == NoteGroup::Single) {
											score_cell_find_before_note(score_cell_insert, lane)->data.note.group[lane] = NoteGroup::LongNoteStart;//k列目のこのLN終端のbeforeにあるノートのグループをLN始点にする
										}
										else {
											score_cell_find_before_note(score_cell_insert, lane)->data.note.group[lane] = NoteGroup::LongNoteMiddle;//k列目のこのLN終端のbeforeにあるノートのグループをLN始点にする
										}


									}
								}

								//エディタ用処理
								if (score_cell_head != NULL) {//エディタ用譜面形式での読み込みも行うとき
									score_cell_write_note(score_cell_insert, lane, note[lane][nc[lane]].color, note[lane][nc[lane]].group, note[lane][nc[lane]].isBright, note[lane][nc[lane]].LN_k);//k列目の音符情報を格納
									if (score_cell_find_before_note(score_cell_insert, lane) != NULL) {
										if (score_cell_find_before_note(score_cell_insert, lane)->data.note.group[lane] == NoteGroup::LongNoteStart 
											&& score_cell_find_before_note(score_cell_insert, lane)->data.note.isBright[lane] == true) {//beforeがLN始点で光っていたら
											score_cell_insert->data.note.isBright[lane] = true;//このLN終点も光らせる(エディタ用)
										}
									}
								}

								nc[lane]++;

								if (j != 8) {//黒以外なら
									total_diff_point_notes++;//難易度加算数インクリメント
								}


								if (j != 8 && total_diff_point_notes >= LOCAL_DIFF_AMOUNT) {//黒じゃなくて難易度加算が一定以上
																							//一定分の難易度ポイントの合計を算出
									double diff_point_local_sum = 0;
									for (int ii = total_diff_point_notes; ii <= total_diff_point_notes + LOCAL_DIFF_AMOUNT - 1; ii++) {


										diff_point_local_sum += diff_buf[number_ring(ii, LOCAL_DIFF_AMOUNT - 1)].Point;

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
					time_counter      += double((double)1000 * ((60 /  bpm          ) * 4 * measure) / DN);//分割単位分の時間プラスする
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

				
				

				if (wstring(str).front() != L'#' && wstring(str).back() == L',') {//最後に「,」がある行を見つけた → 1小節分読み込んだので次の小節の読み込みに移る
					file_p_first = FileRead_tell(fp);//次に読み込む小節の先頭を指す
					break;
				}
			}

			i = 0;//また使うのでiを0に戻しておく
			dataArrayLength = 0;
		}

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
		MusicSub->totalMeasures[difficulty] = blc;
	}



	//瞬間BPM算出
	double BPM_SuggestCalcRatio = 0.10;//瞬間BPMを算出するのに使うノーツ数の割合
	double ignore_ratio = 0.005;//瞬間BPMを算出するのに無視する上位スピードノーツの割合

	float BPM_suggest = 0;
	int SearchAmount = int(BPM_SuggestCalcRatio * Music[song_number].total_note[difficulty]);//探索個数
	int ignore_amount = int(ignore_ratio * Music[song_number].total_note[difficulty]);//無視個数 
	double medium_percentile = 0.5;

	if (SearchAmount <= 1) {
		if (Music[song_number].total_note[difficulty] == 0) {
			Music[song_number].speed_list[difficulty][Speed::MID] = Music[song_number].bpm[difficulty];
			Music[song_number].speed_list[difficulty][Speed::FAST] = Music[song_number].bpm[difficulty];
			Music[song_number].speed_list[difficulty][Speed::MAX] = Music[song_number].bpm[difficulty];
		}
		else {
			Music[song_number].speed_list[difficulty][Speed::MID] = bpmList->at(medium_percentile * Music[song_number].total_note[difficulty]);
			Music[song_number].speed_list[difficulty][Speed::FAST] = bpmList->back();
			Music[song_number].speed_list[difficulty][Speed::MAX] = bpmList->back();
		}
	}
	else {
		double weight = 0;
		int SearchCount = 0;
		std::sort(bpmList->begin(), bpmList->end());  //bpmListを小さい順にソート


		Music[song_number].speed_list[difficulty][Speed::MID] = bpmList->at(medium_percentile * Music[song_number].total_note[difficulty]);
		Music[song_number].speed_list[difficulty][Speed::MAX] = bpmList->back();

		//FAST速度の算出
		int scan_start_ind = Music[song_number].total_note[difficulty] - 1 - ignore_amount;
		for (i = scan_start_ind; i > scan_start_ind - SearchAmount; i--, SearchCount++) {
			weight = (double)(SearchCount) * 2 / (SearchAmount - 1);//大きいBPMから0~2で重み付していく
			BPM_suggest += weight * (float)(bpmList->at(i)) / (SearchAmount);
		}

		Music[song_number].speed_list[difficulty][Speed::FAST] = BPM_suggest;

	}


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


	//最大同時押し数算出
	int ncChords[4] = { 0,0,0,0 };
	int timingSameIndex = 0;

	while (ncChords[0] < nc[0] ||
		ncChords[1] < nc[1] ||
		ncChords[2] < nc[2] ||
		ncChords[3] < nc[3]) {

		for (i = 0; i < 4; i++) {
			if (note[i][ncChords[i]].timing == timing_same[timingSameIndex]) {
				MaxChords.setChords(i, note[i][ncChords[i]].color, note[i][ncChords[i]].group == NoteGroup::LongNoteStart || note[i][ncChords[i]].group == NoteGroup::LongNoteMiddle);
				ncChords[i]++;
			}
		}
		MaxChords.updateMaxChord();
		timingSameIndex++;
	}

	Music[song_number].maxChords[0][difficulty] = MaxChords.getMaxChords();
	Music[song_number].maxChords[1][difficulty] = MaxChords.getMaxChordsRainbow();
	
	


	//DIFFICULTY_RADAR,NotesAmount算出
	DifficultyRadar DR(note, nc, bpmchange, stopSequence, scrollchange, end_time - start_time, start_time, end_time, timing_same, BPM_suggest, std::make_shared<std::vector<float>>(*bpmList));
	DR.GetLocalNotesGraph(Music[song_number].LocalNotesAmount[RainbowMode::General][difficulty], false);//音符密度グラフを得る
	DR.GetLocalNotesGraph(Music[song_number].LocalNotesAmount[RainbowMode::Rainbow][difficulty], false);//音符密度グラフを得る

	DR.GetColorNotesGraph(Music[song_number].ColorNotesAmount[RainbowMode::General][difficulty], Music[song_number].ColorNotesAmount[RainbowMode::Rainbow][difficulty]);


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



	
	//Cdiff->global = DR.CalcGlobal(CALC_MODE_RAINBOW);
	//Cdiff->local = DR.CalcLocal(CALC_MODE_RAINBOW);
	//Cdiff->color = DR.CalcColor(start_time, end_time, CALC_MODE_RAINBOW);
	//Cdiff->longNote = DR.CalcLongNote(CALC_MODE_RAINBOW);
	//Cdiff->unstability = DR.CalcUnstability();
	//Cdiff->chain = DR.CalcChain();

	//Music[song_number].global[CALC_MODE_RAINBOW][difficulty] = Cdiff->global;
	//Music[song_number].local[CALC_MODE_RAINBOW][difficulty] = Cdiff->local;
	//Music[song_number].color[CALC_MODE_RAINBOW][difficulty] = Cdiff->color;
	//Music[song_number].longNote[CALC_MODE_RAINBOW][difficulty] = Cdiff->longNote;
	//Music[song_number].unstability[CALC_MODE_RAINBOW][difficulty] = Cdiff->unstability;
	//Music[song_number].chain[CALC_MODE_RAINBOW][difficulty] = Cdiff->chain;
	


	//オプションで加工された譜面をcopyに格納、その後noteに戻す
	if (Option->op.lane.getIndex() == (int)OptionItem::Lane::NONE) {//NONE
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= nc[i] - 1; j++) {
				copy[i][j] = note[i][j];
			}
		}
	}
	if (Option->op.lane.getIndex() == (int)OptionItem::Lane::MIRROR) {//MIRROR
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= nc[i] - 1; j++) {
				copy[3 - i][j] = note[i][j];
			}
		}
	}
	if (Option->op.lane.getIndex() == (int)OptionItem::Lane::RANDOM) {//RANDOM
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

	if (Option->op.lane.getIndex() == (int)OptionItem::Lane::SUPER_RAND) {//SUPER_RANDOM
		while (nc[0] != nc_s[0] || nc[1] != nc_s[1] || nc[2] != nc_s[2] || nc[3] != nc_s[3]) {//各列全ての音符の配置を変えるまで
			random_kaburanai(op_lane, 4, 0, 3, ln_use_s, ln_use_c);//op_lane[]にコピー先レーン番号を入れる
			//printfDx(L"tc:%d\n", tc);
			//ScreenFlip();
			for (i = 0; i <= 3; i++) {//元の譜面のiレーン
				if (timing_same[tc] == note[i][nc_s[i]].timing) {//縦に見てtc行目で音符を見つけた
					if (op_lane[i] != -1) {//-1になるのは元の譜面のLN終点のとき
						if (note[i][nc_s[i]].group == NoteGroup::Single) {//LN終点以外
							copy[op_lane[i]][nc_c[op_lane[i]]] = note[i][nc_s[i]];
							//コピー元のi行nc_s[i]列目のノートをop_lane[i]行nc_c[op_lane[i]]列目のコピー先に格納
							nc_c[op_lane[i]]++;//コピー先の音符番号++
						}
						if (note[i][nc_s[i]].group == NoteGroup::LongNoteStart) {//LN始点 を見つけるとLN終点までコピーする
							ln_use_s[i] = 1;//元の譜面のiレーンはLN中
							ln_use_c[op_lane[i]] = 1;//コピー先のop_lane[i]レーンはLN中なので音符を置けない
							lanestolanec[i] = op_lane[i];//元の譜面のiレーンのLNはコピー先のop_lane[i]レーンと対応している

							//終点まですべての音符を移す
							int ind = 0;
							while(1){
								copy[op_lane[i]][nc_c[op_lane[i]]] = note[i][nc_s[i] + ind];
								nc_c[op_lane[i]]++;//終点の分インクリメント

								
								if (note[i][nc_s[i] + ind].group == NoteGroup::LongNoteEnd)break;
								ind++;
							};
							
						}

						
					}
					
					if (note[i][nc_s[i]].group == NoteGroup::LongNoteEnd) {//LN終点
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

	[&] {
		for (i = 0; i <= 3; i++) {
			j = 0;
			while (note[i][j].color != NoteColor::NONE) {
				j++;
			}
			MusicSub->objOfLane[difficulty][i] = j;
		}
	}();



	if (Option->op.color.getIndex() == (int)OptionItem::Color::RGB_CHANGE) {//RGB CHANGE
		lane = GetRand(4);
		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				note[i][j].color = rgb_change[lane][(int)note[i][j].color];
				note[i][j].color_init = note[i][j].color;
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::RGB_RAND) {//RGB RAND

		for (i = 0; i <= 3; i++) {
			NoteColor latestColor = NoteColor::NONE;
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				lane = GetRand(5);
				if (note[i][j].LN_k == true) {
					note[i][j].color = note[i][j - 1].color;
					note[i][j].color_init = note[i][j - 1].color_init;
				}
				else {
					bool isLnMiddleOrEnd = (note[i][j].group_init == NoteGroup::LongNoteMiddle || note[i][j].group_init == NoteGroup::LongNoteEnd);
					bool isLnSameColor = isLnMiddleOrEnd && (note[i][j].color == latestColor);
					if (isLnMiddleOrEnd) {
						if (isLnSameColor) {
							note[i][j].color = note[i][j - 1].color;
							note[i][j].color_init = note[i][j].color;
							continue;
						}
					}
					latestColor = note[i][j].color;
					note[i][j].color = rgb_change[lane][(int)note[i][j].color];
					note[i][j].color_init = note[i][j].color;
				}
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::RGB_RAND_PLUS) {//RGB RAND+

		for (i = 0; i <= 3; i++) {
			NoteColor latestColor = NoteColor::NONE;
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				lane = GetRand(5);
				if (note[i][j].LN_k == true) {
					note[i][j].color = note[i][j - 1].color;
					note[i][j].color_init = note[i][j - 1].color_init;
				}
				else {
					note[i][j].color = rgb_change[lane][(int)note[i][j].color];
					note[i][j].color_init = note[i][j].color;
				}
			}
		}
	}
	if (Option->op.color.getIndex() == (int)OptionItem::Color::SUPER_RAND) {//SUPER RAND

		for (i = 0; i <= 3; i++) {
			NoteColor latestColor = NoteColor::NONE;
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				lane = GetRand(6) + 1;//1~7
				if (note[i][j].color >= NoteColor::R && note[i][j].color <= NoteColor::W) {
					if (note[i][j].LN_k == true) {
						note[i][j].color = note[i][j - 1].color;
						note[i][j].color_init = note[i][j - 1].color_init;
					}
					else {
						bool isLnMiddleOrEnd = (note[i][j].group_init == NoteGroup::LongNoteMiddle || note[i][j].group_init == NoteGroup::LongNoteEnd);
						bool isLnSameColor = isLnMiddleOrEnd && (note[i][j].color == latestColor);
						if (isLnMiddleOrEnd) {
							if (isLnSameColor) {
								note[i][j].color = note[i][j - 1].color;
								note[i][j].color_init = note[i][j].color;
								continue;
							}
						}
						latestColor = note[i][j].color;
						note[i][j].color = (NoteColor)lane;
						note[i][j].color_init = note[i][j].color;
					}
				}
				else {
					latestColor = note[i][j].color;
				}
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::SUPER_RAND_PLUS) {//SUPER RAND+

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				lane = GetRand(6) + 1;//1~7
				if (note[i][j].color >= NoteColor::R && note[i][j].color <= NoteColor::W) {
					if (note[i][j].LN_k == true) {
						note[i][j].color = note[i][j - 1].color;
						note[i][j].color_init = note[i][j - 1].color_init;
					}
					else {
						note[i][j].color = (NoteColor)lane;
						note[i][j].color_init = note[i][j].color;
					}
				}
			}
		}
	}





	if (Option->op.color.getIndex() == (int)OptionItem::Color::RGB_ONLY) {//RGB OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				if (note[i][j].LN_k == true) {
					note[i][j].color = note[i][j - 1].color;
					note[i][j].color_init = note[i][j - 1].color_init;
				}
				else {
					if (note[i][j].color == NoteColor::Y) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) + (int)NoteColor::R);//Yは12どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::C) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) + (int)NoteColor::G);//Cは23どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::M) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) * 2 + (int)NoteColor::R);//Mは13どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::W) {
						note[i][j].color = (NoteColor)(lane = GetRand(2) + (int)NoteColor::R);//Wは123どっちか
						note[i][j].color_init = note[i][j].color;
					}
				}
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::CMY_ONLY) {//CMY OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {
				if (note[i][j].LN_k == true) {
					note[i][j].color = note[i][j - 1].color;
					note[i][j].color_init = note[i][j - 1].color_init;
				}
				else {
					if (note[i][j].color == NoteColor::R) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) + (int)NoteColor::M);//Rは46どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::G) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) * 2 + (int)NoteColor::C);//Gは45どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::B) {
						note[i][j].color = (NoteColor)(lane = GetRand(1) + (int)NoteColor::C);//Bは56どっちか
						note[i][j].color_init = note[i][j].color;
					}
					if (note[i][j].color == NoteColor::W) {
						note[i][j].color = (NoteColor)(lane = GetRand(2) + (int)NoteColor::C);//Wは456どっちか
						note[i][j].color_init = note[i][j].color;
					}
				}
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::W_ONLY) {//W OMLY

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color >= NoteColor::R && note[i][j].color <= NoteColor::W)note[i][j].color = NoteColor::W;//KF以外Wにする
				note[i][j].color_init = note[i][j].color;
			}
		}
	}

	if (Option->op.color.getIndex() == (int)OptionItem::Color::RAINBOW) {//RAINBOW

		for (i = 0; i <= 3; i++) {
			for (j = 0; j <= NOTE_MAX_NUMBER - 1; j++) {

				if (note[i][j].color >= NoteColor::R && note[i][j].color <= NoteColor::W)note[i][j].color = NoteColor::F;//KF以外Fにする
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
	dxLibFinishProcess();
	return;
	}
	}
	*/

	/*
	for (i = 1; i <= 4; i++) {
	note[i][0].timing = 0;
	note[i][0].color = 1;
	note[i][0].x = laneCordinateX[i];
	note[i][0].bpm = 120;
	}
	*/
	setAiPredictedDifficultyIfEnable();

	//printfDx(L"ハッシュ値:%d\n", *hash);
	//ScreenFlip();
	//Sleep(1000);

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
			box[0] = GetRand(max - min) + min;//min~maxでランダムに

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
				box[i] = GetRand(max - min) + min;//min~maxでランダムに
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

