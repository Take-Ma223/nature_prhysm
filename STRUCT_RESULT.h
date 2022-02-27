#ifndef _STRUCT_RESULT
#define _STRUCT_RESULT

#define RESULT_DATA_VERSION_MAX_COMBO 1.03//MAX_COMBOを保存するようになったバージョン
#define RESULT_DATA_VERSION_SKY_PERFECT 1.10//SKY_PERFECT数、PERFECTを保存するようになったセーブデータバージョン
#define RESULT_DATA_VERSION_MIN_MISS 1.11//MIN MISSを保存するようになったセーブデータバージョン


#define RESULT_DATA_VERSION 1.11//現在のセーブデータバージョン

static const int RANK_S = 7;//Sランク
static const int RANK_A = 6;//Aランク
static const int RANK_B = 5;//Bランク
static const int RANK_C = 4;//Cランク
static const int RANK_D = 3;//Dランク
static const int RANK_E = 2;//Eランク
static const int RANK_F = 1;//Fランク
static const int RANK_NONE = 0;//ランク無し

static const int RANK_S_SCORE = 9500;//Sランク下限スコア
static const int RANK_A_SCORE = 9000;//Aランク下限スコア
static const int RANK_B_SCORE = 8000;//Bランク下限スコア
static const int RANK_C_SCORE = 7000;//Cランク下限スコア
static const int RANK_D_SCORE = 6000;//Dランク下限スコア
static const int RANK_E_SCORE = 5000;//Eランク下限スコア


static const int CLEARTYPE_PERFECT = 6;//PERFECT
static const int CLEARTYPE_FULL_COMBO = 5;//FULL_COMBO
static const int CLEARTYPE_SUPER_HARD_CLEARED = 4;//SUPER HARD CLEARED
static const int CLEARTYPE_HARD_CLEARED = 3;//HARD CLEARED
static const int CLEARTYPE_CLEARED = 2;//CLEARED
static const int CLEARTYPE_EASY_CLEARED = 1;//EASY CLEARED
static const int CLEARTYPE_FAILED = 0;//FAILED
static const int CLEARTYPE_PLAY = -1;//PLAY
static const int CLEARTYPE_NO_PLAY = -2;//NO PLAY


static const int RESULT_WEATHER_SUNNY = 3;//晴れ
static const int RESULT_WEATHER_CLOUDY = 2;//曇り
static const int RESULT_WEATHER_RAINY = 1;//雨
static const int RESULT_WEATHER_THUNDER = 0;//雷

static const int SAVE_DATA_VALUE_LIMIT = 99999999;//セーブデータ値上限

typedef struct RESULT {//リザルト用構造体
	int hash = 0;//譜面のハッシュ(譜面やスコアが改ざんされてないか確認する)
	int difficulty = 0;//難易度
	int clear = CLEARTYPE_NO_PLAY;//-1:PLAY 0:FAILED(不合格) 1:EASY_CLEARED(合格) 2:CLEARED 3:HARD_CLEARED 4:SUPER_HARD_CLEARED 5:FULL_COMBO 6:PERFECT
	int score = 0;//


	int perfect = 0;//
	int good = 0;//
	int miss = 0;//
	int rank = 0;//ランク(0:未プレイ 1:F 2:E 3:D 4:C 5:B 6:A 7:S)
	int play_count = 0;//何回遊んだか
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