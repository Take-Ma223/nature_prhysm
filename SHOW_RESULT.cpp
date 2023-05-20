#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_XY.h"
#include"STRUCT_OP.h"
#include"STRUCT_SONG.h"
#include"SHOW_RESULT.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"number_digit.h"
#include"Get_Key_State.h"
#include"make_result_hash.h"
#include"show_something.h"
#include"CONSTANT_VALUE.h"
#include"GetNowCount_d.h"
#include"ShowFps.h"

#include <string>
#include "SaveDataSaveLoad.h"
#include "IR_process.h"
#include "AppContext.h"
#include "ActivityContext.h"
#include "DxLibUtil.h"

using namespace std;

void SHOW_RESULT(RESULT res,
	Option *option, 
	int song_number,
	int difficulty,
	int *debug, 
	Song *Music,
	int Button[3][4],
	int Button_Shutter,
	int *Key, 
	char *Buf,
	ANDROID_CONTROLLER *AC,
	int TryCount,
	SkillTestList *STList,
	int list_number,
	Config config,
	IR_SETTING* ir,
	int SkillTestFlag) {
	Asset asset;//使う画像セット
	//コンテキスト
	AppContext appContext = AppContext(NULL, option, &config);
	ActivityContext context = ActivityContext(&appContext, &asset);


	RESULT high_score;//現在のハイスコア
	RESULT save;//実際に保存するデータ
	int H_BG = 0;
	int H_RESULT;
	int H_SCORE_BOARD;
	int H_STR;
	int H_SCORE_NUMBER[10];
	
	int H_POP_NUMBER[10];
	int H_MAX_COMBO_NUMBER[10];
	int H_WEATHER[4];

	int H_COVER;
	int H_COVER_MIDDLE;
	int H_DIFFICULTY;
	int H_RANK;//ランク画像
	int H_CLEARED = 0;//CLEARED画像
	int H_FAILED;//FAILED画像
	int H_FULL_COMBO[6];//フルコンボ
	int H_PFC;//PERFECTフルコンボ
	int H_FX[25];//エフェクト
	int H_R_IN;//虹オプション(内側)
	int H_R_OUT;//虹オプション(外側)
	int H_JACKET = 0;
	int H_JACKET_GAUSS = 0;

	int H_HIGH_SCORE_NUMBER[10];
	int H_HIGH_SCORE_BANNER;
	int H_HIGH_SCORE_BANNER_ALLOW;

	int H_WHITE_WALL;

	int SH_CLOSE;
	int SH_CLOSED;
	int SH_OPEN;
	int SH_RESULT;
	int SH_RANK;//ランク表示音
	int SH_SHUTTER_SIGNAL;//シャッター音警告
	int SH_SHUTTER;//シャッター音

	double GAME_start_time = 0;
	double GAME_passed_time = 0;
	double LOOP_passed_time = 1;
	double CounterRemainTime = 0;
	double time_cash = 0;
	int brightness = 0;
	int brightness2 = 0;
	
	double draw_alpha = 0;
	int i = 0, j = 0;
	int up_digit[5] = { 0,0,0,0,0 };//ハイスコア更新分の
	int score_digit[5] = { 0,0,0,0,0 };

	int sky_perfect_digit[5] = { 0,0,0,0,0 };
	int perfect_digit[5] = { 0,0,0,0,0 };
	int good_digit[5] = { 0,0,0,0,0 };
	int miss_digit[5] = { 0,0,0,0,0 };

	int pop_digit[6][3] = { {0,0,0},{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 },{ 0,0,0 } };
	int max_combo_digit[4] = { 0,0,0,0 };

	int flag = 0;//画面状態フラグ(1:カバー開く状態 2:カバー閉じる状態)
	int openflag = 0;
	int bestscore = 0;//ハイスコア更新したか(0:してない 1:した)
	int firstplay = 0;//最初のプレイかどうか(1:最初のプレイ)
	double fx_draw_counter = 0;//エフェクト描画カウンタ
	int rank_appear_time = 3500;//ランクが出現する時間
	double c_m_draw_counter = 0;
	int rank_flag = 0;

	XYPAIR SkillTestShowSlide;//段位認定時のスコア,ランク表示位置ずれ量
	SkillTestShowSlide.x = 190;
	SkillTestShowSlide.y = 150;


	XYPAIR jacketSize;//ジャケットのサイズ
	XYPAIR jacketGaussSize;//ジャケットのサイズ
	XYPAIR center;
	center.x = 640;
	center.y = 360;
	XYPAIR jacketArea;
	jacketArea.x = 1280;
	jacketArea.y = 720;
	double jacketScale = 1;
	double jacketGaussScale = 1;
	double jacket_show_counter = -1;
	double jacket_alpha = 1;

	XY HighScorePos;//ハイスコアボードの位置
	XY HighScoreV;//ハイスコアボードの速度
	double fallG = 0.0015;//重力加速度
	HighScorePos.x = 0;
	HighScorePos.y = -400;
	int HighScore_fall_stop = 0;//ハイスコアボード落ち切ったフラグ



	/*
	char speed[12][10] = { "×0.25","×0.5","×0.75","×1","×1.25","×1.5","×1.75","×2","×2.5","×3","×3.5","×4" };//オプションの名前
	char gauge[5][13] = { "NORMAL","HARD","SUPER HARD","FC ATTACK","PFC ATTACK" };
	char lane[4][13] = { "NONE","MIRROR","RANDOM","SUPER RAND" };
	char color[8][16] = { "NONE","RGB CHANGE","RGB RAND","SUPER RAND","RGB ONLY","CMY ONLY","W ONLY","RAINBOW" };
	*/

	int StrLength = 0;//オプション表示用の文字の長さ格納

	wchar_t ScreenShotFileNameBuf[256] = L"SccreenShot.jpg";//スクリーンショットのファイル名バッファ
	wchar_t ScreenShotFileName[256] = L"SccreenShot.jpg";//スクリーンショットのファイル名
	double SccreenShot_ShowWhite_counter = 0;//スクリーンショットを取ったときに出る白い光を表示するカウンタ
	
	wchar_t *DifficultyName[6] = {L"晴れ",L"曇り",L"雨",L"雷",L"雪",L"大雪"};
	int DifficultyUse = 0;//ファイル名にどの難易度を使うか

	if (difficulty == 1) {
		DifficultyUse = 0;
	}
	else if (difficulty == 2) {
		DifficultyUse = 1;
	}
	else if (difficulty == 3) {
		if (Music[song_number].season[difficulty] == 4) {//冬
			DifficultyUse = 4;
		}
		else {
			DifficultyUse = 2;
		}
	}
	else if (difficulty == 4) {
		if (Music[song_number].season[difficulty] == 4) {//冬
			DifficultyUse = 5;
		}
		else {
			DifficultyUse = 3;
		}
	}

	wchar_t *Rainbow[2] = { L"",L"_虹" };
	int RainbowUse = 0;

	if (option->op.color == OptionItem::Color::RAINBOW) {
		RainbowUse = 1;
	}
	else {
		RainbowUse = 0;
	}

	wchar_t *Auto[2] = { L"",L"_Auto" };
	int AutoUse = 0;

	if (*debug == 1) {//debugモードのときはAutoをファイル名につける
		AutoUse = 1;
	}
	else {
		AutoUse = 0;
	}

	if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
		swprintf_s(ScreenShotFileNameBuf, L"%s_%s%s%s_%d.jpg",
			Music[song_number].title[difficulty],
			DifficultyName[DifficultyUse],
			Rainbow[RainbowUse],
			Auto[AutoUse],
			res.score);//ファイル名は「曲名 難易度 虹 Auto スコア」になる
	}
	else {
		swprintf_s(ScreenShotFileNameBuf, L"%s_%d.jpg", STList->title[list_number], res.score);
	}

	//ファイル名に使えない文字があったら消去する
	ReplaceStr(ScreenShotFileNameBuf);

	swprintf_s(ScreenShotFileName, L"screenshot/%s", ScreenShotFileNameBuf);//パスを含んだ最終的な保存名

	//記録をファイルに書き込み
	FILE* fp = 0;
	FILE* fpLatest = 0;
	errno_t error = 0;
	wchar_t filename[256];
	wchar_t filenameLatest[256];

	wchar_t* season[4] = { L"sunny",L"cloudy",L"rainy",L"thunder" };

	for (i = 0; i <= 255; i++) {//初期化
		filename[i] = L'\0';
	}

	if (*debug == 0 && SkillTestFlag != SHOW_SKILL_TEST_RESULT) {//デバッグモード,段位合格表示じゃないときは保存する
		//読み込み
		//セーブファイルの場所決定
		wchar_t str[256];
		sprintfDx(str, L"save_data/score/%s", Music[song_number].KindFolder);
		CreateDirectory(str, NULL);//スコア保存用の種類ディレクトリ(official)作成
		CreateDirectory(Music[song_number].SaveFolder, NULL);//スコア保存用の曲ディレクトリ作成

		if (option->op.color != OptionItem::Color::RAINBOW) {//通常モードだったら
			sprintfDx(filename, L"%s/result_%s.dat", Music[song_number].SaveFolder, season[difficulty - 1]);
			sprintfDx(filenameLatest, L"%s/latest_result_%s.dat", Music[song_number].SaveFolder, season[difficulty - 1]);
		}
		if (option->op.color == OptionItem::Color::RAINBOW) {//虹モードだったら
			sprintfDx(filename, L"%s/result_%s_r.dat", Music[song_number].SaveFolder, season[difficulty - 1]);
			sprintfDx(filenameLatest, L"%s/latest_result_%s_r.dat", Music[song_number].SaveFolder, season[difficulty - 1]);
		}



		error = _wfopen_s(&fp, filename, L"rb");
		if (error != 0) {//ファイル見つからなかったら
			firstplay = 1;//初プレイ
						  //printfDx(L"1\n");
		}
		high_score.save_data_version = 0;

		if (firstplay == 0) {//初プレイじゃないときに読み込み
			firstplay = 0;
			fread(&high_score, sizeof(high_score), 1, fp);//high_scoreにハイスコア読み込み
			fclose(fp);

			//printfDx(L"2\n");
		}

		/*
		if (firstplay == 0) {
			if (ScoreData_Authentication(high_score, song_number, difficulty + 4 * (option->color == 7), Music) == -1) {//違う曲、難易度の譜面データだったら無視する(簡単な不正防止)
				firstplay = 1;//データが変だったら初プレイ同様にそのままデータを書き込む
			}
		}

		*/

		//save.hash = res.hash;
		//printfDx(L"%d\n%d\n", Music[song_number].hash[difficulty], res.hash);


		//書き込み
		
		//今回の記録(res)をそのままLatestScoreとして保存
		error = _wfopen_s(&fpLatest, filenameLatest, L"wb");
		if (error != 0) {
			printfDx(L"%sのオープンに失敗しました.\n", filename);
			printfDx(L"エラー番号:%d", error);
			ScreenFlip();

			Sleep(100000);
			exit(EXIT_FAILURE);
		}
		fwrite(&res, sizeof(res), 1, fpLatest);
		fclose(fpLatest);

		error = _wfopen_s(&fp, filename, L"wb");
		if (error != 0) {
			printfDx(L"%sのオープンに失敗しました.\n", filename);
			printfDx(L"エラー番号:%d", error);
			ScreenFlip();

			Sleep(100000);
			exit(EXIT_FAILURE);
		}

		//ONLYオプションを使用しているかどうか
		BOOL isOnlyOption = option->op.color == OptionItem::Color::RGB_ONLY || 
			option->op.color == OptionItem::Color::CMY_ONLY ||
			option->op.color == OptionItem::Color::W_ONLY;

		if (firstplay == 0) {//2回目以降のプレイ

			save = high_score;

			if (high_score.save_data_version < RESULT_DATA_VERSION_MAX_COMBO) {//セーブデータバージョンが古かったら(最大コンボ記録するバージョンより前)
				//新しく保存することになった記録をとりあえず全て記録する
				for (i = 0; i <= 5; i++) {
					save.weather[i] = res.weather[i];
					save.pop[i] = res.pop[i];

					save.max_combo = res.max_combo;
					if (high_score.clear == CLEARTYPE_FULL_COMBO) {//フルコンボ済みの曲なら
						save.max_combo = Music[song_number].total_note[difficulty];//ノート数を格納
						high_score.max_combo = save.max_combo;//high_scoreのMaxComboも値を更新しておく
					}
				}
				save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み
			}


			if (high_score.save_data_version < RESULT_DATA_VERSION_SKY_PERFECT) {//セーブデータバージョンが古かったら(SKY_PERFECT、PERFECT記録するバージョンより前)
				for (i = 0; i <= 5; i++) {
					save.max_combo = res.max_combo;
					if (high_score.score == 10000 && high_score.clear == CLEARTYPE_FULL_COMBO) {//PERFECT済み(PFC)の曲なら
						save.clear = CLEARTYPE_PERFECT;//PERFECTにする
					}
				}
				save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み
			}

			if (high_score.save_data_version < RESULT_DATA_VERSION_MIN_MISS) {//セーブデータバージョンが古かったら(MinMiss記録するバージョンより前)
				for (i = 0; i <= 5; i++) {					
					if (high_score.clear == CLEARTYPE_FULL_COMBO || high_score.clear == CLEARTYPE_PERFECT) {//フルコン済み(FC PFC)の曲なら
						save.min_miss = 0;//MinMiss0にする
					}
					else if(high_score.clear == CLEARTYPE_CLEARED || high_score.clear == CLEARTYPE_HARD_CLEARED || high_score.clear == CLEARTYPE_SUPER_HARD_CLEARED){//クリア済みの曲
						save.min_miss = high_score.miss;
						high_score.min_miss = high_score.miss;//これまでの最小ミスを暫定で決めておく
					}
					else {//クリアしていないのでMinMissは復元できない
						save.min_miss = -1;
					}
				}
				save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み
			}



			if (res.clear > high_score.clear) {
				save.clear = res.clear;//クリア状態更新
			}

			if (res.score > high_score.score && !isOnlyOption) {//ONLYオプションを付けていなくてハイスコア更新
				
				
				save.score = res.score;

				save.sky_perfect = res.sky_perfect;
				save.perfect = res.perfect;
				save.good = res.good;
				save.miss = res.miss;
				for (i = 0; i <= 5; i++) {
					save.weather[i] = res.weather[i];
					save.pop[i] = res.pop[i];
				}
				
				bestscore = 1;
			}
			if (res.max_combo > high_score.max_combo) {//MaxCombo更新
				save.max_combo = res.max_combo;
			}
			if ((res.clear >= CLEARTYPE_CLEARED || res.clear == CLEARTYPE_PLAY) && (res.miss < high_score.min_miss || high_score.min_miss == -1)) {//クリア、PLAYしたときにMinMiss更新またはMinMISSが保存されていないとき
				save.min_miss = res.miss;
			}
			if (res.clear >= CLEARTYPE_CLEARED && (high_score.clear == CLEARTYPE_FAILED)) {//Failedからの 初クリアはMinMiss保存
				save.min_miss = res.miss;
			}


			if (res.rank > high_score.rank && !isOnlyOption) {//ONLYモードじゃなくてランク更新
				save.rank = res.rank;
			}

		}

		if (firstplay == 1) {
			if (!isOnlyOption) {//ONLYオプションを使っていない
				save = res;//初プレイなら今回のスコアそのまま保存

				if (res.clear >= CLEARTYPE_CLEARED || res.clear == CLEARTYPE_PLAY) {//クリア、PLAYしたとき　MinMiss保存
					save.min_miss = res.miss;
				}

			}
			else {
				save.clear = res.clear;//灰クリアだけ記録
			}
			save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み

		}

		if (option->op.color == OptionItem::Color::RAINBOW) {//虹オプションなら難易度+4しておく(今はしてない)
			save.difficulty = difficulty;
		}

		//毎回保存する内容

		save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み

		int PlayCountBuf = high_score.play_count + TryCount;
		if (PlayCountBuf >= 9999)PlayCountBuf = 9999;//カンスト処理
		res.play_count = PlayCountBuf;
		save.play_count = PlayCountBuf;//プレイ回数を足す
														//printfDx(L"保存しました\n");
		save.hash_result = make_result_hash(save);

		fwrite(&save, sizeof(save), 1, fp);
		fclose(fp);



		//全体セーブデータ読み込み
		SAVEDATA saveData;
		loadSaveData(&saveData);
		saveData.totalHitNotes += res.sky_perfect + res.perfect + res.good;
		saveData.totalPlayCount++;

		if (saveData.totalHitNotes >= SAVE_DATA_VALUE_LIMIT)saveData.totalHitNotes = SAVE_DATA_VALUE_LIMIT;
		if (saveData.totalPlayCount >= SAVE_DATA_VALUE_LIMIT)saveData.totalPlayCount = SAVE_DATA_VALUE_LIMIT;

		writeSaveData(saveData);

		//インターネットランキング送信用スコアの保存、送信
		IRsave(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, res, difficulty, Music[song_number].season[difficulty], option->op.color == OptionItem::Color::RAINBOW, isOnlyOption, config);
		IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color == OptionItem::Color::RAINBOW, config);

	}
	else if(*debug == 0 && SkillTestFlag == SHOW_SKILL_TEST_RESULT){//段位スコア保存
		//読み込みファイル名
		sprintfDx(filename, L"save_data/skill_test/score/%s.dat", STList->title[list_number]);

		firstplay = 0;
		error = _wfopen_s(&fp, filename, L"rb");
		if (error != 0) {//ファイル見つからなかったら
			firstplay = 1;//初プレイ
						  //printfDx(L"1\n");
		}
		high_score.save_data_version = 0;

		if (firstplay == 0) {//初プレイじゃないときに読み込み
			firstplay = 0;
			fread(&high_score, sizeof(high_score), 1, fp);//high_scoreにハイスコア読み込み
			fclose(fp);
		}

		//書き込み

		error = _wfopen_s(&fp, filename, L"wb");
		if (error != 0) {
			printfDx(L"%sのオープンに失敗しました.\n", filename);
			printfDx(L"エラー番号:%d", error);
			ScreenFlip();

			Sleep(100000);
			exit(EXIT_FAILURE);
		}
		if (firstplay == 0) {//2回目以降のプレイ
			save = high_score;

			if (res.clear > high_score.clear) {
				save.clear = res.clear;//クリア状態更新
			}

			if (res.score > high_score.score) {//ハイスコア更新
				save.score = res.score;
				save.sky_perfect = res.sky_perfect;
				save.perfect = res.perfect;
				save.good = res.good;
				save.miss = res.miss;

				bestscore = 1;
			}
			if (res.max_combo > high_score.max_combo) {//MaxCombo更新
				save.max_combo = res.max_combo;
			}
			if (res.clear == 1 && res.miss < high_score.min_miss) {//クリアしたときにMinMiss更新していたら
				save.min_miss = res.miss;
			}
			if (res.clear == 1 && (high_score.clear == 0)) {//Failedからの 初クリアはMinMiss保存
				save.min_miss = res.miss;
			}

			if (res.rank > high_score.rank) {//ランク更新
				save.rank = res.rank;
			}

		}

		if (firstplay == 1) {
			save = res;//初プレイなら今回のスコアそのまま保存

			if (res.clear == 1) {//MinMissはクリアしたときだけ保存
				save.min_miss = res.miss;
			}
			save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み
		}

		//毎回保存する内容
		save.save_data_version = RESULT_DATA_VERSION;//セーブデータバージョン書き込み
		int PlayCountBuf = high_score.play_count + 1;
		if (PlayCountBuf >= 9999)PlayCountBuf = 9999;//カンスト処理
		save.play_count = PlayCountBuf;//プレイ回数を足す
		fwrite(&save, sizeof(save), 1, fp);
		fclose(fp);
	}

	/////////////////////////////////////////////////////////////


	wstring themeStr1(L"img/themes/");
	wstring themeStr2(option->theme[option->op.theme]);


	if (SkillTestFlag == SHOW_SKILL_TEST_RESULT) {
		if (res.clear == 1) {//合格
			H_BG = LoadGraph(L"img/Certificate.png");
		}
		else {
			H_BG = LoadGraph((themeStr1 + themeStr2 + wstring(L"/bg.png")).c_str());
		}
	}
	else {
		if (((res.rank == 7) || Music[song_number].secret == 1) && res.clear != 0) {//クリアしててSのとき または隠し曲初クリア時
			if (wcscmp(L"\0", Music[song_number].jacketpath2[difficulty]) == 0) {//リザルト専用ジャケットが無い
				H_JACKET = LoadGraph(Music[song_number].jacketpath[difficulty]);//通常ジャケットの読み込み
				H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath[difficulty]);//通常ジャケット(ぼかし用)の読み込み
			}
			else {
				H_JACKET = LoadGraph(Music[song_number].jacketpath2[difficulty]);//ジャケットの読み込み
				H_JACKET_GAUSS = LoadGraph(Music[song_number].jacketpath2[difficulty]);//ジャケット(ぼかし用)の読み込み
			}

		}
		if (!(((res.rank == 7) || Music[song_number].secret == 1) && res.clear != 0) || H_JACKET == -1) {//汎用リザルト画像読み込み
			
			wchar_t ImageFolderPass[128];
			wchar_t ImagePass[128];
			wchar_t season[7];

			if (Music[song_number].season[difficulty] == 0) {//無季節
				sprintfDx(season, L"other");
			}
			if (Music[song_number].season[difficulty] == 1) {//春
				sprintfDx(season, L"spring");
			}
			if (Music[song_number].season[difficulty] == 2) {//夏
				sprintfDx(season, L"summer");
			}
			if (Music[song_number].season[difficulty] == 3) {//秋
				sprintfDx(season, L"autumn");
			}
			if (Music[song_number].season[difficulty] == 4) {//冬
				sprintfDx(season, L"winter");
			}

			if (res.score < 7000)sprintfDx(ImageFolderPass, L"img/result/%s/D", season);
			if (res.score >= 7000)sprintfDx(ImageFolderPass, L"img/result/%s/C", season);
			if (res.score >= 8000)sprintfDx(ImageFolderPass, L"img/result/%s/B", season);
			if (res.score >= 9000)sprintfDx(ImageFolderPass, L"img/result/%s/A", season);
			if (res.clear == 0)sprintfDx(ImageFolderPass, L"img/result/%s/failed", season);
			GetBG(ImageFolderPass, ImagePass);
			H_JACKET = LoadGraph(ImagePass);
			H_JACKET_GAUSS = LoadGraph(ImagePass);
		}

		
	}

	H_RESULT = LoadGraph(L"img/RESULT.png");
	if(SkillTestFlag == SHOW_SKILL_TEST_RESULT){
		H_SCORE_BOARD = LoadGraph(L"img/SKILL_TEST_RESULT_BOARD.png");
		H_STR = LoadGraph(L"img/SKILL_TEST_RESULT_STR.png");
	}
	else {
		H_SCORE_BOARD = LoadGraph(L"img/RESULT_BOARD.png");
		H_STR = LoadGraph(L"img/RESULT_STR.png");
	}



	//LoadDivGraph(L"img/score_number.png", 10, 10, 1, 64, 100, H_SCORE_NUMBER);
	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_SCORE_NUMBER);

	LoadDivGraph(L"img/HighScoreNumber.png", 10, 10, 1, 25, 50, H_HIGH_SCORE_NUMBER);

	LoadDivGraph(L"img/SmallNumberBlue.png", 10, 10, 1, 25, 50, H_POP_NUMBER);
	LoadDivGraph(L"img/SmallNumberRed.png", 10, 10, 1, 25, 50, H_MAX_COMBO_NUMBER);

	H_WEATHER[3] = LoadGraph(L"img/weather/sunny.png");
	H_WEATHER[2] = LoadGraph(L"img/weather/cloudy.png");
	if (Music[song_number].season[difficulty] != 4) {//冬じゃないなら
		H_WEATHER[1] = LoadGraph(L"img/weather/rainy.png");
	}
	else {
		H_WEATHER[1] = LoadGraph(L"img/weather/snowy.png");
	}
	if (Music[song_number].season[difficulty] != 4) {//冬じゃないなら
		H_WEATHER[0] = LoadGraph(L"img/weather/thunder.png");
	}
	else {
		H_WEATHER[0] = LoadGraph(L"img/weather/snowstorm.png");
	}

	H_HIGH_SCORE_BANNER = LoadGraph(L"img/HighScoreBanner.png");
	H_HIGH_SCORE_BANNER_ALLOW = LoadGraph(L"img/HighScoreBannerAllow.png");

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
		if (Music[song_number].season[difficulty] != 4) {//冬じゃないなら
			H_DIFFICULTY = LoadGraph(L"img/weather/rainy.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/weather/snowy.png");
		}
	}
	if (difficulty == 4) {
		H_COVER = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_thunder.png")).c_str());
		if (Music[song_number].season[difficulty] != 4) {//冬じゃないなら
			H_DIFFICULTY = LoadGraph(L"img/weather/thunder.png");
		}
		else {
			H_DIFFICULTY = LoadGraph(L"img/weather/snowstorm.png");
		}
	}

	H_COVER_MIDDLE = LoadGraph((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());

	if (SkillTestFlag == SHOW_SKILL_TEST_RESULT) {//段位総合リザルト表示
		H_FAILED = LoadGraph(L"img/不合格.png");
		H_CLEARED = LoadGraph(L"img/合格.png");
	}
	else {
		if (res.clear == CLEARTYPE_PLAY) {
			if (option->op.gauge == OptionItem::Gauge::SKILL_TEST) {//段位認定ゲージ
				H_CLEARED = LoadGraph(L"img/cleared.png");
			}
			else {
				H_CLEARED = 0;
			}
		}
		else if (res.clear == CLEARTYPE_EASY_CLEARED) {//段位各曲のリザルト表示
			H_CLEARED = LoadGraph(L"img/cleared_easy.png");
		}
		else if (res.clear == CLEARTYPE_FAILED) {
			H_FAILED = LoadGraph(L"img/failed.png");

		}
		else if (res.clear == CLEARTYPE_CLEARED) {
			H_CLEARED = LoadGraph(L"img/cleared_normal.png");
		}
		else if (res.clear == CLEARTYPE_HARD_CLEARED) {
			H_CLEARED = LoadGraph(L"img/cleared_hard.png");
		}
		else if (res.clear == CLEARTYPE_SUPER_HARD_CLEARED) {
			H_CLEARED = LoadGraph(L"img/cleared_super_hard.png");
		}
		else if (res.clear == CLEARTYPE_FULL_COMBO) {
			LoadDivGraph(L"img/FULL_COMBO.png", 6, 1, 6, 640, 100, H_FULL_COMBO);
		}
		else if (res.clear == CLEARTYPE_PERFECT) {
			H_PFC = LoadGraph(L"img/PERFECT_FULLCOMBO.png");
		}
		else if (res.clear == 7) {
			H_CLEARED = LoadGraph(L"img/cleared.png");
		}

	}


	LoadDivGraph(L"img/result_fx.png", 25, 5, 5, 330, 330, H_FX);

	if (res.rank == 1)H_RANK = LoadGraph(L"img/F.png");
	if (res.rank == 2)H_RANK = LoadGraph(L"img/E.png");
	if (res.rank == 3)H_RANK = LoadGraph(L"img/D.png");
	if (res.rank == 4)H_RANK = LoadGraph(L"img/C.png");
	if (res.rank == 5)H_RANK = LoadGraph(L"img/B.png");
	if (res.rank == 6)H_RANK = LoadGraph(L"img/A.png");
	if (res.rank == 7)H_RANK = LoadGraph(L"img/S.png");

	H_R_IN = LoadGraph(L"img/R_inside.png");
	H_R_OUT = LoadGraph(L"img/R_outside.png");

	H_WHITE_WALL = LoadGraph(L"img/WhiteWall.png");

	SH_CLOSE = LoadSoundMem(L"sound/close.wav");
	SH_CLOSED = LoadSoundMem(L"sound/closed.wav");
	SH_OPEN = LoadSoundMem(L"sound/open.wav");
	SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");

	if (res.clear == 0 || (SkillTestFlag == SHOW_SKILL_TEST_RESULT && res.clear == 0)) {//failed時
		SH_RESULT = LoadSoundMem(L"sound/result_failed.wav");
		SH_RANK = LoadSoundMem(L"sound/result_failed_p.wav");
	}
	if (res.clear != 0 || (SkillTestFlag == SHOW_SKILL_TEST_RESULT && res.clear == 1)) {
		SH_RESULT = LoadSoundMem(L"sound/result_clear.wav");
		SH_RANK = LoadSoundMem(L"sound/result_clear_p.wav");
	}



	if (CheckHandleASyncLoad(H_JACKET) == FALSE && jacket_show_counter == -1) {//読み込み完了してたらぼかし処理
		jacket_show_counter = -2;
		GetGraphSize(H_JACKET, &jacketSize.x, &jacketSize.y);//サイズを取得
		GetGraphSize(H_JACKET_GAUSS, &jacketGaussSize.x, &jacketGaussSize.y);//サイズを取得
																			 //GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_DOWN_SCALE, 4);
		GraphFilter(H_JACKET_GAUSS, DX_GRAPH_FILTER_GAUSS, 32, 4000);

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


	wchar_t WindowTitle[128];
	if (*debug == 1) {
		swprintf_s(WindowTitle, L"nature prhysm ver %.2f  PrtSc:スクリーンショット", APPLICATION_VERSION);
	}
	else {
		swprintf_s(WindowTitle, L"nature prhysm ver %.2f  F11:Twitterに投稿  PrtSc:スクリーンショット", APPLICATION_VERSION);
	}
	SetMainWindowText(WindowTitle);//ウィンドウタイトル変更

	int show_inst_flag = 1;//F11:左上の説明(DEBUG_MODE除く)を表示するフラグ(1:する 0:しない)

	GAME_start_time = GetNowCount_d(config);
	ChangeFont(L"メイリオ");
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetFontSize(35);
	SetFontThickness(9);

	int title_width = 1;

	if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
		title_width = GetDrawStringWidth(Music[song_number].title[difficulty], wcslen(Music[song_number].title[difficulty]));//曲名の横の長さを入れる
	}
	else {
		title_width = GetDrawStringWidth(STList->title[list_number], wcslen(STList->title[list_number]));//曲名の横の長さを入れる
	}

	int H_TITLE_IMG = MakeScreen(1280, 100, TRUE);//タイトル画像ハンドル作成

	SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
	SetDrawScreen(H_TITLE_IMG);//タイトル文字画像を描画対象に
	if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
		ShowExtendedStrFit(640, 5, Music[song_number].title[difficulty], title_width, 800, config,
			Music[song_number].StrColor[difficulty], Music[song_number].StrShadowColor[difficulty]);//タイトルを描画して画像作成
	}
	else {
		ShowExtendedStrFit(640, 5, STList->title[list_number], title_width, 800, config,
			STList->Color[list_number], STList->ShadowColor[list_number]);//コース名描画
	}
	SetDrawScreen(DX_SCREEN_BACK);//描画対象を裏画面に戻す
	SetDrawMode(DX_DRAWMODE_NEAREST);

	SetFontSize(28);//オプション描画フォントサイズの設定
	while (1) {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}

		//Calc
		GAME_passed_time = GetNowCount_d(config) - GAME_start_time;//経過時間計算
		LOOP_passed_time = GAME_passed_time - time_cash;//1ループにかかった時間を算出
		time_cash = GAME_passed_time;

		CounterRemainTime -= int(CounterRemainTime);
		CounterRemainTime += LOOP_passed_time;
		int CRTBuf = int(CounterRemainTime);

		ShowFps(GAME_passed_time, LOOP_passed_time, time_cash, config);

		Get_Key_State(Buf,Key, AC);

		if (flag == 1 && GAME_passed_time >= rank_appear_time + 300) {//表示し終わった後に何かボタンを押したので選曲画面へ移動フラグ立てる
			if (Key[Button[2][0]] == 1 || Key[Button[2][1]] == 1 || Key[Button[2][2]] == 1 || Key[Button[2][3]] == 1 ||
				Key[Button[1][0]] == 1 || Key[Button[1][1]] == 1 || Key[Button[1][2]] == 1 || Key[Button[1][3]] == 1 ||
				Key[Button[0][0]] == 1 || Key[Button[0][1]] == 1 || Key[Button[0][2]] == 1 || Key[Button[0][3]] == 1 || Key[KEY_INPUT_RETURN] || Key[KEY_INPUT_ESCAPE]) {
				flag = 2;
				PlaySoundMem(SH_CLOSE, DX_PLAYTYPE_BACK, TRUE);
			}

			if (Key[KEY_INPUT_F2] == 1 && SkillTestFlag != SHOW_SKILL_TEST_RESULT) {//スコア再送信&ランキング表示
				PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
				IRsend(ir, Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, difficulty, option->op.color == OptionItem::Color::RAINBOW, config);
				IRview(Music[song_number].SongPath[difficulty], Music[song_number].SaveFolder, option->op.color == OptionItem::Color::RAINBOW, config);
			}

			if (Key[KEY_INPUT_F11] == 1 && *debug == 0) {//ツイート
				PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
			
				wchar_t tweet_sent[1024] = L"";
				wchar_t html_sent[1024] = L"";
				
				wchar_t difficulty_sent[10] = L"";
				wchar_t rainbow_sent[6] = L"";

				wchar_t *SkillTestResult_sent[2] = {
					L"不合格...",
					L"合格!"
				};
				
				if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
					if (option->op.color == OptionItem::Color::RAINBOW) {//RAINBOWモードは「虹」追加
						sprintfDx(rainbow_sent, L",虹");
					}


					if (difficulty == 1) {
						sprintfDx(difficulty_sent, L"晴れ%s", rainbow_sent);
					}
					else if (difficulty == 2) {
						sprintfDx(difficulty_sent, L"曇り%s", rainbow_sent);
					}
					else if (difficulty == 3) {
						if (Music[song_number].season[difficulty] == 4) {
							sprintfDx(difficulty_sent, L"雪%s", rainbow_sent);
						}
						else {
							sprintfDx(difficulty_sent, L"雨%s", rainbow_sent);
						}
					}
					else if (difficulty == 4) {
						if (Music[song_number].season[difficulty] == 4) {
							sprintfDx(difficulty_sent, L"大雪%s", rainbow_sent);
						}
						else {
							sprintfDx(difficulty_sent, L"雷%s", rainbow_sent);
						}
					}

					wchar_t rank_sent[4];
					if (res.rank == 1) {
						sprintfDx(rank_sent, L"F");
					}
					else if (res.rank == 2) {
						sprintfDx(rank_sent, L"E");
					}
					else if (res.rank == 3) {
						sprintfDx(rank_sent, L"D");
					}
					else if (res.rank == 4) {
						sprintfDx(rank_sent, L"C");
					}
					else if (res.rank == 5) {
						sprintfDx(rank_sent, L"B");
					}
					else if (res.rank == 6) {
						sprintfDx(rank_sent, L"A");
					}
					else if (res.rank == 7) {
						sprintfDx(rank_sent, L"S");
					}

					wchar_t clear_state_sent[25] = L"";
					if (res.clear == -1) {
						sprintfDx(clear_state_sent, L"");
					}else if (res.clear == 0) {
						sprintfDx(clear_state_sent, L"でFAILED...");
					}
					else if (res.clear == 1) {
						sprintfDx(clear_state_sent, L"でEASY_CLEARED!");
					}
					else if (res.clear == 2) {
						sprintfDx(clear_state_sent, L"でCLEARED!!");
					}
					else if (res.clear == 3) {
						sprintfDx(clear_state_sent, L"でHARD CLEARED!!!");
					}
					else if (res.clear == 4) {
						sprintfDx(clear_state_sent, L"でSUPER HARD CLEARED!!!!");
					}
					else if (res.clear == 5) {
						sprintfDx(clear_state_sent, L"でFULL COMBO!!!!!");
					}
					else if (res.clear == 6) {
						sprintfDx(clear_state_sent, L"でPERFECT!!!!!!");
					}


					sprintfDx(tweet_sent, L"%s(%s)で%d点、%sランク%s",
						Music[song_number].title[difficulty],
						difficulty_sent,
						res.score,
						rank_sent,
						clear_state_sent);
				}
				else {//段位の時
					sprintfDx(tweet_sent, L"%sに%s", STList->title[list_number], SkillTestResult_sent[res.clear]);
				}

				ReplaceTweetStr(tweet_sent);



				sprintfDx(html_sent, L"https://twitter.com/intent/tweet?text=%s%%0A%%23natureprhysm%%0Ahttps://www.freem.ne.jp/win/game/16015&source=webclient", tweet_sent);



				HINSTANCE ret = ShellExecute(NULL, L"open", html_sent, NULL, NULL, SW_SHOW);
			}
		}
		
		if (show_inst_flag == 0) {//スクリーンショット
			show_inst_flag = 1;
			PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
			SetDrawScreen(DX_SCREEN_FRONT);// 描画先を表画面にする
			SaveDrawScreenToJPEG(0, 0, 1280, 720, ScreenShotFileName, 100);
			SetDrawScreen(DX_SCREEN_BACK);// 描画先を裏画面にする
			PlaySoundMem(SH_SHUTTER, DX_PLAYTYPE_BACK, TRUE);
			SccreenShot_ShowWhite_counter = 1;
		}

		if (flag == 1 && draw_alpha == 1 && fx_draw_counter == 1 && Key[Button_Shutter] == 1) {//シャッターボタンが押された
			show_inst_flag = 0;//「F11:Twitterに投稿」の文字を消したフレームで撮影
		}

		for (i = 0; i < CRTBuf; i++) {
			if (SccreenShot_ShowWhite_counter > 0) {
				SccreenShot_ShowWhite_counter -= 0.001;
			}
			if (SccreenShot_ShowWhite_counter < 0) {
				SccreenShot_ShowWhite_counter = 0;
			}
		}



		if (flag == 1) {
			for (i = 0; i < CRTBuf; i++) {
				if (draw_alpha <= 1) {
					draw_alpha += 0.001;
				}
				if (draw_alpha > 1) {
					draw_alpha = 1;
				}
			}
		}

		brightness = int(155 * (1 + sin((double)GAME_passed_time / 600)) / 2) + 100;
		brightness2 = int(100 * (1 + sin((double)GAME_passed_time / 400)) / 2) + 155;

		number_digit(res.score, score_digit, 5);
		number_digit(res.sky_perfect, sky_perfect_digit, 5);
		number_digit(res.perfect, perfect_digit, 5);
		number_digit(res.good, good_digit, 5);
		number_digit(res.miss, miss_digit, 5);

		for (i = 0; i <= 5; i++) {
			number_digit(res.pop[i], pop_digit[i], 3);
		}
		number_digit(res.max_combo, max_combo_digit, 4);


		number_digit(res.score - high_score.score, up_digit, 5);

		//Draw
		SetDrawScreen(appContext.baseHandle.getHandle());
		ClearDrawScreen();//グラフィックを初期化

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで
		DrawGraph(0, 0, H_BG, FALSE);//背景

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255 * jacket_alpha));

		//ぼかしたジャケットで隙間を埋める

		if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			DrawExtendGraph(int(center.x - jacketGaussScale * (jacketGaussSize.x / 2)),
				int(center.y - jacketGaussScale * (jacketGaussSize.y / 2)),
				int(center.x + jacketGaussScale * (jacketGaussSize.x / 2)),
				int(center.y + jacketGaussScale * (jacketGaussSize.y / 2)), H_JACKET_GAUSS, FALSE);
			DrawExtendGraph(int(center.x - jacketScale * (jacketSize.x / 2)), int(center.y - jacketScale * (jacketSize.y / 2)), int(center.x + jacketScale * (jacketSize.x / 2)), int(center.y + jacketScale * (jacketSize.y / 2)), H_JACKET, FALSE);
		}
		SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 190));

		SetDrawBright(brightness, brightness, brightness);
		//DrawGraph(0, 0, H_RESULT, TRUE);//RESULT文字
		SetDrawBright(255, 255, 255);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 127));
		DrawGraph(0, int(0 + (1 - draw_alpha) * 50), H_SCORE_BOARD, TRUE);//ボード

		int entireAlpha = 200;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * entireAlpha));

		if (option->op.color == OptionItem::Color::RAINBOW && SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			//SetDrawBright(brightness2, brightness2, brightness2);
			DrawGraph(155, int(25 + (1 - draw_alpha) * 50), H_R_OUT, TRUE);//R_out
			DrawGraph(155, int(25 + (1 - draw_alpha) * 50), H_R_IN, TRUE);//R_in
																		   //SetDrawBright(255, 255, 255);
		}

		SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
		if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			DrawExtendGraph(1026, int(35 + (1 - draw_alpha) * 50), 1026 + 80, int(35 + 80 + (1 - draw_alpha) * 50), H_DIFFICULTY, TRUE);//譜面難易度
		}
		SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す


		if (GAME_passed_time >= rank_appear_time) {
			if (GAME_passed_time >= rank_appear_time + 300) {//エフェクトよりちょっと遅れて表示

				if (rank_flag == 0) {
					PlaySoundMem(SH_RANK, DX_PLAYTYPE_BACK, TRUE);
					rank_flag = 1;
				}

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * brightness2));
				//DrawGraph(660, int(330 + (1 - draw_alpha) * 50), H_RANK, TRUE);//ランク
				SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
				if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
					DrawExtendGraph(1029, int(205 + (1 - draw_alpha) * 50), 1029 + 220, int(205 + 220 + (1 - draw_alpha) * 50), H_RANK, TRUE);//ランク
				}
				else {
					DrawExtendGraph(1029 - SkillTestShowSlide.x, int(205 + SkillTestShowSlide.y + (1 - draw_alpha) * 50), 1029 - SkillTestShowSlide.x + 220, int(205 + SkillTestShowSlide.y + 220 + (1 - draw_alpha) * 50), H_RANK, TRUE);//ランク
				}
				SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す

				SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * entireAlpha));
				if (res.clear == 0) {
					if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
						DrawExtendGraph(939, int(375 + (1 - draw_alpha) * 50), int(939 + 96 * 4.2), int(375 + 15 * 4.2), H_FAILED, TRUE);//FAILED
					}
					else {
						DrawExtendGraph(939 - SkillTestShowSlide.x, int(375 + SkillTestShowSlide.y + (1 - draw_alpha) * 50), int(939 - SkillTestShowSlide.x + 96 * 4.2), int(375 + SkillTestShowSlide.y + 15 * 4.2), H_FAILED, TRUE);//FAILED
					}
				}
				if (res.clear == -1 || res.clear == 1 || res.clear == 2 || res.clear == 3 || res.clear == 4) {
					//DrawGraph(510, int(570 + (1 - draw_alpha) * 50), H_CLEARED, TRUE);//CLEARED
					if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
						DrawExtendGraph(939, int(375 + (1 - draw_alpha) * 50), int(939 + 96 * 4.2), int(375 + 15 * 4.2), H_CLEARED, TRUE);//CLEARED
					}
					else {
						DrawExtendGraph(939 - SkillTestShowSlide.x, int(375 + SkillTestShowSlide.y + (1 - draw_alpha) * 50), int(939 - SkillTestShowSlide.x + 96 * 4.2), int(375 + SkillTestShowSlide.y + 15 * 4.2), H_CLEARED, TRUE);//CLEARED
					}
				}
				else if (res.clear == 5) {
					SetDrawMode(DX_DRAWMODE_BILINEAR);//バイリニアで描く
					DrawExtendGraph(983, int(385 + (1 - draw_alpha) * 50), int(983 + 96 * 3.2), int(385 + 15 * 3.2), H_FULL_COMBO[(int(GAME_passed_time) / 100) % 6], TRUE);//FULLCOMBO
					SetDrawMode(DX_DRAWMODE_NEAREST);//バイリニアから戻す
				}
				else if (res.clear == 6) {
					DrawExtendGraph(939, int(375 + (1 - draw_alpha) * 50), int(939 + 96 * 4.2), int(375 + 15 * 4.2), H_PFC, TRUE);//PERFECT
				}


				SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
			}

			if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
				DrawGraph(975, int(145 + (1 - draw_alpha) * 50), H_FX[int(fx_draw_counter * 25)], TRUE);//出現エフェクト
			}
			else {
				DrawGraph(975 - SkillTestShowSlide.x, int(145 + SkillTestShowSlide.y + (1 - draw_alpha) * 50), H_FX[int(fx_draw_counter * 25)], TRUE);//出現エフェクト
			}

			for (i = 0; i < CRTBuf; i++) {
				if (fx_draw_counter < 1) {
					fx_draw_counter += 0.001;
				}
				if (fx_draw_counter >= 1) {
					fx_draw_counter = 1;
				}
			}
		}




		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * entireAlpha));


		DrawGraph(0, int(0 + (1 - draw_alpha) * 50), H_STR, TRUE);//文字

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * 200));

		if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			DrawGraph(0, int(60 + 2 + (1 - draw_alpha) * 50), H_TITLE_IMG, TRUE);//タイトル画像表示
		}
		else {
			DrawGraph(0, int(170 + 2 + (1 - draw_alpha) * 50), H_TITLE_IMG, TRUE);//タイトル画像表示
		}

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * entireAlpha));

		//score表示
		if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			DrawNumber(232,                        int(209                        + (1 - draw_alpha) * 50), res.score, 25, 1, 5, H_SCORE_NUMBER);
		}
		else {
			DrawNumber(232 + SkillTestShowSlide.x, int(SkillTestShowSlide.y + 209 + (1 - draw_alpha) * 50), res.score, 25, 1, 5, H_SCORE_NUMBER);
		}

		//sky_perfect数表示
		ShowJudgeNumber(res.sky_perfect,
			250,
			SkillTestFlag,
			draw_alpha,
			H_SCORE_NUMBER,
			sky_perfect_digit,
			SkillTestShowSlide.x,
			SkillTestShowSlide.y);

		//perfect数表示
		ShowJudgeNumber(res.perfect,
			292,
			SkillTestFlag,
			draw_alpha,
			H_SCORE_NUMBER,
			perfect_digit,
			SkillTestShowSlide.x,
			SkillTestShowSlide.y);

		//good数表示
		ShowJudgeNumber(res.good,
			334,
			SkillTestFlag,
			draw_alpha,
			H_SCORE_NUMBER,
			good_digit,
			SkillTestShowSlide.x,
			SkillTestShowSlide.y);

		//miss数表示
		ShowJudgeNumber(res.miss,
			376,
			SkillTestFlag,
			draw_alpha,
			H_SCORE_NUMBER,
			miss_digit,
			SkillTestShowSlide.x,
			SkillTestShowSlide.y);



		//ハイスコアバナー表示
		if (bestscore == 1) {
			if (GAME_passed_time >= rank_appear_time - 400) {//ランクが出る直前に落下

				for (i = 0; i < CRTBuf; i++) {
					if (HighScore_fall_stop == 0) {//落ち切っていないとき
						HighScorePos.y = HighScorePos.y + HighScoreV.y;
					}
				}
				for (i = 0; i < CRTBuf; i++) {
					HighScoreV.y = float(HighScoreV.y + fallG);
				}

				if (HighScorePos.y >= 0) {
					HighScorePos.y = 0;
					if (HighScoreV.y >= fallG * 2) {//動きがほぼ止まってるときはもう速度を変更しない
						HighScoreV.y = float(-HighScoreV.y + 0.6);
					}				
					if (HighScoreV.y >= 0) {//速さ(-HighScoreV.y + 0.6)が正
						HighScore_fall_stop = 1;//落ち切ったフラグ1に
					}
				}

			}

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, entireAlpha);
			if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
				DrawGraph(int(HighScorePos.x), int(HighScorePos.y), H_HIGH_SCORE_BANNER, TRUE);//ハイスコアバナー
			}
			else {
				DrawGraph(int(HighScorePos.x + SkillTestShowSlide.x), int(HighScorePos.y + SkillTestShowSlide.y), H_HIGH_SCORE_BANNER, TRUE);//ハイスコアバナー
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, entireAlpha);
			if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
				DrawGraph(int(HighScorePos.x), int(HighScorePos.y), H_HIGH_SCORE_BANNER_ALLOW, TRUE);//ハイスコアバナー矢印
			}
			else {
				DrawGraph(int(HighScorePos.x + SkillTestShowSlide.x), int(HighScorePos.y + SkillTestShowSlide.y), H_HIGH_SCORE_BANNER_ALLOW, TRUE);//ハイスコアバナー矢印
			}

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, entireAlpha);
			for (i = 0; i <= int(log10(res.score - high_score.score)); i++) {//ハイスコア増分表示
				if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
					DrawGraph(int(HighScorePos.x + 220 - i * 25), int(HighScorePos.y + 155), H_HIGH_SCORE_NUMBER[up_digit[i]], TRUE);
				}
				else {
					DrawGraph(int(HighScorePos.x + SkillTestShowSlide.x + 220 - i * 25), int(HighScorePos.y + SkillTestShowSlide.y + 155), H_HIGH_SCORE_NUMBER[up_digit[i]], TRUE);
				}
			}
		}

		//下に天気、降水、MAX_COMBO表示
		if (SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, int((double)draw_alpha * entireAlpha));
			for (i = 0; i <= 5; i++) {
				DrawExtendGraph(315 + i * 120, int(545 + (1 - draw_alpha) * 50), 315 + 50 + i * 120, int(545 + 50 + (1 - draw_alpha) * 50), H_WEATHER[res.weather[i]], TRUE);//天気

				DrawNumber(340 + i * 120, int(604 + (1 - draw_alpha) * 50), res.pop[i], 25, 0, 0, H_POP_NUMBER);

			}
			DrawNumber(379 +6 * 120, int(604 + (1 - draw_alpha) * 50), res.max_combo, 25, 1, 4, H_MAX_COMBO_NUMBER);
		}

		//オプション名称描画
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(255));

		StrLength = GetDrawStringWidth(option->speed[option->op.speed], wcslen(option->speed[option->op.speed]));
		DrawString(-160 + 320 * 1 - (StrLength / 2) + 2, 690 + 2, option->speed[option->op.speed], GetColor(0, 0, 0));//speed
		DrawString(-160 + 320 * 1 - (StrLength / 2), 690, option->speed[option->op.speed], GetColor(255, 255, 255));//speed

		StrLength = GetDrawStringWidth(option->gauge[(int)option->op.gauge], wcslen(option->gauge[(int)option->op.gauge]));
		DrawString(-160 + 320 * 2 - (StrLength / 2) + 2, 690 + 2, option->gauge[(int)option->op.gauge], GetColor(0, 0, 0));//gauge
		DrawString(-160 + 320 * 2 - (StrLength / 2), 690, option->gauge[(int)option->op.gauge], GetColor(255, 255, 255));//gauge

		StrLength = GetDrawStringWidth(option->lane[(int)option->op.lane], wcslen(option->lane[(int)option->op.lane]));
		DrawString(-160 + 320 * 3 - (StrLength / 2) + 2, 690 + 2, option->lane[(int)option->op.lane], GetColor(0, 0, 0));//lane
		DrawString(-160 + 320 * 3 - (StrLength / 2), 690, option->lane[(int)option->op.lane], GetColor(255, 255, 255));//lane

		StrLength = GetDrawStringWidth(option->color[(int)option->op.color], wcslen(option->color[(int)option->op.color]));
		DrawString(-160 + 320 * 4 - (StrLength / 2) + 2, 690 + 2, option->color[(int)option->op.color], GetColor(0, 0, 0));//color
		DrawString(-160 + 320 * 4 - (StrLength / 2), 690, option->color[(int)option->op.color], GetColor(255, 255, 255));//color




		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		if (flag == 0) {//開ける
			DrawGraph(int(-320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 0), 0, H_COVER, TRUE);//カバー表示
			DrawGraph(int(320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 960), 0, H_COVER, TRUE);//右側
			DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			if (c_m_draw_counter > 1) {
				c_m_draw_counter = 1;
				PlaySoundMem(SH_RESULT, DX_PLAYTYPE_BACK, TRUE);
				flag = 1;
			}
			for (i = 0; i < CRTBuf; i++) {
				if (c_m_draw_counter <= 1) {
					c_m_draw_counter += 0.001;
				}
			}
		}


		if (flag == 2) {//閉める
			DrawGraph(int(-320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 0), 0, H_COVER, TRUE);//カバー表示
			DrawGraph(int(320 * (-(cos((3.14 / 2)*c_m_draw_counter) - 1)) + 960), 0, H_COVER, TRUE);//右側
			DrawGraph(320, int((cos((3.14 / 2)*c_m_draw_counter) - 1) * 720), H_COVER_MIDDLE, TRUE);
			if (c_m_draw_counter < 0) {
				PlaySoundMem(SH_CLOSED, DX_PLAYTYPE_BACK, TRUE);
				for (i = 0; i <= 1500; i++) {//1.5s待つ
					if (ProcessMessage() != 0) {
						dxLibFinishProcess();
						return;
					}
					Sleep(1);
				}
				InitGraph();//メモリ開放
				InitSoundMem();//

				wchar_t WindowTitle[30];
				swprintf_s(WindowTitle, L"nature prhysm ver %.2f", APPLICATION_VERSION);
				SetMainWindowText(WindowTitle);//ウィンドウタイトル変更
				return;
			}
			for (i = 0; i < CRTBuf; i++) {
				if (c_m_draw_counter >= 0) {
					c_m_draw_counter -= 0.001;
				}
			}

		}

		//カメラシャッター表示
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(SccreenShot_ShowWhite_counter*255));
		//DrawGraph(0, 0, H_WHITE_WALL, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

		/*
		SetFontSize(28);
		if (debug == 1) {
		DrawString(int(120 - ((double)150 / 2)) + 2, 10 + 2, "DEBUG MODE", GetColor(0, 0, 0));//デバッグ表示(影)
		DrawString(int(120 - ((double)150 / 2)), 10, "DEBUG MODE", GetColor(255, 255, 255));//デバッグ表示
		}
		*/

		if (*debug == 1 && config.ShowDebug == 1) {
			printfDx(L"DEBUG MODE\n");
			if (show_inst_flag == 1 && flag == 1 && GAME_passed_time >= rank_appear_time + 300 && SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
				printfDx(L"F2:スコアランキング表示\n");
			}
		}
		else {
			if (show_inst_flag == 1 && flag == 1 && GAME_passed_time >= rank_appear_time + 300 && SkillTestFlag != SHOW_SKILL_TEST_RESULT) {
				printfDx(L"F2:スコアランキング表示\n");
			}
			if (show_inst_flag == 1 && flag == 1 && GAME_passed_time >= rank_appear_time + 300) {
				printfDx(L"F11:Twitterに投稿\n");
			}
		}
		if (show_inst_flag == 1 && flag == 1 && GAME_passed_time >= rank_appear_time + 300) {
			printfDx(L"PrtSc:スクリーンショット\n");
		}



		if (openflag == 0) {
			PlaySoundMem(SH_OPEN, DX_PLAYTYPE_BACK, TRUE);
			openflag = 1;
		}

		if (config.Vsync == 0) {
			i = 0;
			while (LOOP_passed_time + i < (double)1000 / config.Fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				i++;
			}
		}

		ScreenFlip();
		clsDx();



	}
}


void ShowJudgeNumber(int Number, int y, int Flag, double draw_alpha, int *H_SCORE_NUMBER, int digit[5], int SlideX, int SlideY) {
	int i = 0;
	if (Number == 0) {//Number数表示
		if (Flag != SHOW_SKILL_TEST_RESULT) {
			DrawGraph(220 - 0 * 25, int(y + (1 - draw_alpha) * 50), H_SCORE_NUMBER[digit[0]], TRUE);
		}
		else {
			DrawGraph(220 + SlideX - 0 * 25, int(y + SlideY + (1 - draw_alpha) * 50), H_SCORE_NUMBER[digit[0]], TRUE);
		}
	}
	else {
		for (i = 0; i <= int(log10(Number)); i++) {
			if (Flag != SHOW_SKILL_TEST_RESULT) {
				DrawGraph(220 - i * 25, int(y + (1 - draw_alpha) * 50), H_SCORE_NUMBER[digit[i]], TRUE);
			}
			else {
				DrawGraph(220 + SlideX - i * 25, int(y + SlideY + (1 - draw_alpha) * 50), H_SCORE_NUMBER[digit[i]], TRUE);
			}
		}
	}
}




void ReplaceStr(wchar_t *Str) {//ファイル名に使えない文字を置き換える関数
    //\/:*?"<>| を消去
	std::wstring NewStr = std::wstring(Str);//charポインタをstring形に変換し代入

	size_t c;
	while ((c = NewStr.find(L"?")) != std::wstring::npos) {//先頭に?があった
		NewStr.replace(c,1,L"？");//それを置換
	}

	while ((c = NewStr.find(L"\\")) != std::wstring::npos) {//先頭に\があった
		NewStr.replace(c, 1, L"￥");//それを置換
	}

	while ((c = NewStr.find(L"/")) != std::wstring::npos) {//先頭に/があった
		NewStr.replace(c, 1, L"／");//それを置換
	}

	while ((c = NewStr.find(L":")) != std::wstring::npos) {//先頭に:があった
		NewStr.replace(c, 1, L"：");//それを置換
	}

	while ((c = NewStr.find(L"*")) != std::wstring::npos) {//先頭に*があった
		NewStr.replace(c, 1, L"＊");//それを置換
	}

	while ((c = NewStr.find(L"\"")) != std::wstring::npos) {//先頭に"があった
		NewStr.replace(c, 1, L"”");//それを置換
	}

	while ((c = NewStr.find(L"<")) != std::wstring::npos) {//先頭に<があった
		NewStr.replace(c, 1, L"＜");//それを置換
	}

	while ((c = NewStr.find(L">")) != std::wstring::npos) {//先頭に>があった
		NewStr.replace(c, 1, L"＞");//それを置換
	}

	while ((c = NewStr.find(L"|")) != std::wstring::npos) {//先頭に|があった
		NewStr.replace(c, 1, L"｜");//それを置換
	}

	sprintfDx(Str, L"%s", NewStr.c_str());//文字列を置き換え
	
	return;
}

void ReplaceTweetStr(wchar_t *Str) {//ツイートに使えない文字を置き換える関数
    //\#&+;{}|^%を置換
	std::wstring NewStr = std::wstring(Str);//charポインタをstring形に変換し代入

	size_t c;
	while ((c = NewStr.find(L"%")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"％");//それを置換
	}

	while ((c = NewStr.find(L"\\")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%5C");//それを置換
	}

	while ((c = NewStr.find(L"\"")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%22");//それを置換
	}

	while ((c = NewStr.find(L"#")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%23");//それを置換
	}

	while ((c = NewStr.find(L"&")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%26");//それを置換
	}

	while ((c = NewStr.find(L"+")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%2B");//それを置換
	}

	while ((c = NewStr.find(L";")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%3B");//それを置換
	}

	while ((c = NewStr.find(L"{")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%7B");//それを置換
	}

	while ((c = NewStr.find(L"}")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%7D");//それを置換
	}

	while ((c = NewStr.find(L"|")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%7C");//それを置換
	}

	while ((c = NewStr.find(L"^")) != std::wstring::npos) {
		NewStr.replace(c, 1, L"%5E");//それを置換
	}



	sprintfDx(Str, L"%s", NewStr.c_str());//文字列を置き換え

	return;
}

void GetBG(wchar_t *SearchPass, wchar_t *ImagePass) {//背景画像を指定フォルダ(SearchPass)からランダムに取得する関数

	HANDLE hFind_Images = 0;//見つける用ハンドル
	WIN32_FIND_DATA lp;

	wchar_t pass[512];
	sprintfDx(pass, L"%s/*", SearchPass);

	int ImageCount = 0;
	//まず個数を調べる
	hFind_Images = FindFirstFile(pass, &lp);//hit_soundフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを除いて通常のファイルを見つけた
			ImageCount++;
		}
	} while (FindNextFile(hFind_Images, &lp));//何も見つからなかったら0になりループを抜ける
	if (FindClose(hFind_Images) == 0) {
		dxLibFinishProcess();
		return;
	}

	//読み込む画像番号決定
	int ReadIndex = 0;
	ReadIndex = GetRand(ImageCount - 1);


	//実際の名前を入れていく
	ImageCount = 0;
	hFind_Images = FindFirstFile(pass, &lp);//notesフォルダの最初の探索
	do {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
			return;
		}
		if ((wcscmp(lp.cFileName, L".") != 0) && (wcscmp(lp.cFileName, L"..") != 0) && (lp.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)) {//自身と親以外のディレクトリを見つけた

			if (ImageCount == ReadIndex) {//読み込むと決めた画像なので読み込む
				break;
			}
			ImageCount++;
		}
	} while (FindNextFile(hFind_Images, &lp));//何も見つからなかったら0になりループを抜ける
	if (FindClose(hFind_Images) == 0) {
		dxLibFinishProcess();
		return;
	}

	sprintfDx(ImagePass, L"%s/%s", SearchPass, lp.cFileName);//画像パスを返す

	return;
}