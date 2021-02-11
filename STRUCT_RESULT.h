#ifndef _STRUCT_RESULT
#define _STRUCT_RESULT

#define SAVE_DATA_VERSION_MAX_COMBO 1.03//MAX_COMBOを保存するようになったバージョン
#define SAVE_DATA_VERSION_SKY_PERFECT 1.10//SKY_PERFECT数、PERFECTを保存するようになったセーブデータバージョン
#define SAVE_DATA_VERSION_MIN_MISS 1.11//MIN MISSを保存するようになったセーブデータバージョン


#define SAVE_DATA_VERSION 1.11//現在のセーブデータバージョン
typedef struct RESULT {//リザルト用構造体
	int hash = 0;//譜面のハッシュ(譜面やスコアが改ざんされてないか確認する)
	int difficulty = 0;//難易度
	int clear = 0;//-1:PLAY 0:FAILED(不合格) 1:EASY_CLEARED(合格) 2:CLEARED 3:HARD_CLEARED 4:SUPER_HARD_CLEARED 5:FULL_COMBO 6:PERFECT
	int score = 0;//


	int perfect = 0;//
	int good = 0;//
	int miss = 0;//
	int rank = 0;//ランク(0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
	int play_counter = 0;//何回遊んだか
	int hash_result = 0;//リザルトデータ自体のハッシュ
	int weather[6] = {0,0,0,0,0,0};//3時間毎の精度(0,1,2,3で良くなっていく 雷、雨、曇り、晴れ)
	int pop[6] = {100,100,100,100,100,100};//3時間毎の(100-ゲージ量)
	int max_combo = 0;
	double save_data_version = 0;//セーブデータ構造のバージョン(以前のセーブファイルとの見分けのため)

	int sky_perfect = 0;//空パーフェクト数
	int min_miss = -1;//クリアしたときの最小ミス数(-1は未プレイ)
	//int S_perfect = 0;

}RESULT;

/*
typedef struct SkillTestResult {//段位認定リザルト用構造体
	short clear = 0;//0:不合格 1:合格
	int score = 0;//

	short sky_perfect = 0;//空パーフェクト数
	short perfect = 0;//
	short good = 0;//
	short miss = 0;//
	short rank = 0;//ランク(0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
	short play_counter = 0;//何回遊んだか
	short max_combo = 0;
	short min_miss = -1;
	double save_data_version = 0;//セーブデータ構造のバージョン(以前のセーブファイルとの見分けのため)

}SkillTestResult;
*/

#endif