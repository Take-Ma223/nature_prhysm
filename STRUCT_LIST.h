#include"CONSTANT_VALUE.h"
#include"DxLib.h"
#include"show_something.h"

#ifndef _STRUCT_LIST
#define _STRUCT_LIST

typedef struct song_number_diff {//曲フォルダ構造体用の曲番号と難易度を返す構造体
	int song_number = 0;
	char difficulty = 0;//1,2,3,4で晴れ～雷 0は「フォルダ選択に戻る」用
	char kind = 0;//その項目の種類 0:曲 1:フォルダ選択に戻る
}song_number_diff;


const int NUMBER_OF_FOLDERS = 41;//フォルダの数


enum FolderIndex {
ALL_SONGS,
ALL_DIFFICULTY,
SPRING,
SUMMER,
AUTUMN,
WINTER,
NOT_SEASONAL,

GLOBAL,
LOCAL,
CHAIN,
UNSTABILITY,
STREAK,
COLOR,

SKILL_RADAR_GLOBAL,
SKILL_RADAR_LOCAL,
SKILL_RADAR_CHAIN,
SKILL_RADAR_UNSTABILITY,
SKILL_RADAR_STREAK,
SKILL_RADAR_COLOR,
SKILL_RADAR_ROBUSTNESS,

DIFFICULTY_5,
DIFFICULTY_10,
DIFFICULTY_15,
DIFFICULTY_20,
DIFFICULTY_25,
DIFFICULTY_30,
DIFFICULTY_35,
DIFFICULTY_40,
DIFFICULTY_45,
DIFFICULTY_50,
DIFFICULTY_55,
DIFFICULTY_60,
DIFFICULTY_65,
DIFFICULTY_70,
DIFFICULTY_75,
DIFFICULTY_80,
DIFFICULTY_85,
DIFFICULTY_90,
DIFFICULTY_95,
DIFFICULTY_100,
SKILL_TEST_NUMBER
};

/*
const int FOLDERS_ALL_SONGS = 0;
const int FOLDERS_ALL_DIFFICULTY = 1;
const int FOLDERS_SPRING = 2;
const int FOLDERS_SUMMER = 3;
const int FOLDERS_AUTUMN = 4;
const int FOLDERS_WINTER = 5;
const int FOLDERS_NOT_SEASONAL = 6;

const int FOLDERS_DIFFICULTY_5 = 7;
const int FOLDERS_DIFFICULTY_10 = 8;
const int FOLDERS_DIFFICULTY_15 = 9;
const int FOLDERS_DIFFICULTY_20 = 10;
const int FOLDERS_DIFFICULTY_25 = 11;
const int FOLDERS_DIFFICULTY_30 = 12;
const int FOLDERS_DIFFICULTY_35 = 13;
const int FOLDERS_DIFFICULTY_40 = 14;
const int FOLDERS_DIFFICULTY_45 = 15;
const int FOLDERS_DIFFICULTY_50 = 16;
const int FOLDERS_DIFFICULTY_55 = 17;
const int FOLDERS_DIFFICULTY_60 = 18;
const int FOLDERS_DIFFICULTY_65 = 19;
const int FOLDERS_DIFFICULTY_70 = 20;
const int FOLDERS_DIFFICULTY_75 = 21;
const int FOLDERS_DIFFICULTY_80 = 22;
const int FOLDERS_DIFFICULTY_85 = 23;
const int FOLDERS_DIFFICULTY_90 = 24;
const int FOLDERS_DIFFICULTY_95 = 25;
const int FOLDERS_DIFFICULTY_100 = 26;

const int FOLDER_SKILL_TEST_NUMBER = 27;//自然管理技術者検定フォルダの番号
*/

const int NUMBER_OF_COURSES = 39;//段位認定フォルダの要素数


const int SHOW_SKILL_TEST_RESULT = 5;//段位リザルト表示

const int FOLDER_KIND_NORMAL = 0;//通常フォルダ(ALL~NOT SEASONAL)
const int FOLDER_KIND_DIFFICULTY = 1;//難易度別フォルダ
const int FOLDER_KIND_SKILL_TEST = 2;//段位認定フォルダ

const int FOLDER_SKILL_TEST_KIND_BASIC = 0;//基本段位
const int FOLDER_SKILL_TEST_KIND_APPLIED = 1;//応用段位
const int FOLDER_SKILL_TEST_KIND_BACK = 2;//段位認定フォルダから戻る


typedef struct LIST {//曲フォルダ
					 //ALL_SONGS
					 //SPRING
					 //SUMMER
					 //AUTUMN
					 //WINTER
					 //NOT_SEASONAL
					 //5~100%
	song_number_diff* folder[NUMBER_OF_FOLDERS];//曲番号を入れるリスト 6~25は 5,10,15...100までのレベルについて 

	wchar_t* folder_name[NUMBER_OF_FOLDERS] = {
		L"ALL SONGS",
		L"ALL DIFFICULTY",
		L"SPRING",
		L"SUMMER",
		L"AUTUMN",
		L"WINTER",
		L"NOT SEASONAL",

		L"GLOBAL(全体密度)傾向",
		L"LOCAL(局所密度)傾向",
		L"CHAIN(縦連打)傾向",
		L"UNSTABILITY(速度変化)傾向",
		L"STREAK(ロングノート)傾向",
		L"COLOR(色)傾向",

		L"スキルレーダー対象 GLOBAL",
		L"スキルレーダー対象 LOCAL",
		L"スキルレーダー対象 CHAIN",
		L"スキルレーダー対象 UNSTABILITY",
		L"スキルレーダー対象 STREAK",
		L"スキルレーダー対象 COLOR",
		L"スキルレーダー対象 ROBUSTNESS",

		L"降水確率5%",
		L"降水確率10%",
		L"降水確率15%",
		L"降水確率20%",
		L"降水確率25%",

		L"降水確率30%",
		L"降水確率35%",
		L"降水確率40%",
		L"降水確率45%",
		L"降水確率50%",

		L"降水確率55%",
		L"降水確率60%",
		L"降水確率65%",
		L"降水確率70%",
		L"降水確率75%",

		L"降水確率80%",
		L"降水確率85%",
		L"降水確率90%",
		L"降水確率95%",
		L"降水確率100%",

		L"自然管理技術者検定会場"
	};

	int folder_c[NUMBER_OF_FOLDERS] = { 0,0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0,0,0,0,0,
		0 };//それぞれのフォルダの要素数

	int selected_folder = 0;//現在選択しているフォルダ 0:ALL SONGS 6から25はLEVEL5~LEVEL100 26は段位認定
	int NumberOfFolders = NUMBER_OF_FOLDERS;

	char FolderKind[NUMBER_OF_FOLDERS] = {
		FOLDER_KIND_NORMAL,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_NORMAL,
		FOLDER_KIND_NORMAL,
		FOLDER_KIND_NORMAL,
		FOLDER_KIND_NORMAL,
		FOLDER_KIND_NORMAL,//NOT SEASONAL

		FOLDER_KIND_DIFFICULTY,//GLOBAL
		FOLDER_KIND_DIFFICULTY,//LOCAL
		FOLDER_KIND_DIFFICULTY,//CHAIN
		FOLDER_KIND_DIFFICULTY,//UNSTABILITY
		FOLDER_KIND_DIFFICULTY,//STREAK
		FOLDER_KIND_DIFFICULTY,//COLOR

		FOLDER_KIND_DIFFICULTY,//SKILL RADAR GLOBAL
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR LOCAL
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR CHAIN
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR UNSTABILITY
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR STREAK
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR COLOR
		FOLDER_KIND_DIFFICULTY,//SKILL RADAR ROBUSTNESS

		FOLDER_KIND_DIFFICULTY,//5%
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,//50%
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,
		FOLDER_KIND_DIFFICULTY,//100%
		FOLDER_KIND_SKILL_TEST,
	};

}LIST;


typedef struct SkillTestList {//段位認定構造体
	wchar_t *fliepath[NUMBER_OF_COURSES] = {//コース内容ファイルへのパス
		L"basic18.npst",
		L"basic17.npst",
		L"basic16.npst",
		L"basic15.npst",
		L"basic14.npst",
		L"basic13.npst",
		L"basic12.npst",
		L"basic11.npst",
		L"basic10.npst",
		L"basic9.npst",
		L"basic8.npst",
		L"basic7.npst",
		L"basic6.npst",
		L"basic5.npst",
		L"basic4.npst",
		L"basic3.npst",
		L"basic2.npst",
		L"basic1pre.npst",
		L"basic1.npst",
		L"applied18.npst",
		L"applied17.npst",
		L"applied16.npst",
		L"applied15.npst",
		L"applied14.npst",
		L"applied13.npst",
		L"applied12.npst",
		L"applied11.npst",
		L"applied10.npst",
		L"applied9.npst",
		L"applied8.npst",
		L"applied7.npst",
		L"applied6.npst",
		L"applied5.npst",
		L"applied4.npst",
		L"applied3.npst",
		L"applied2.npst",
		L"applied1pre.npst",
		L"applied1.npst",
		L""
	};
	wchar_t *title[NUMBER_OF_COURSES] = {
		L"基本自然管理技術者検定18級",
		L"基本自然管理技術者検定17級",
		L"基本自然管理技術者検定16級",
		L"基本自然管理技術者検定15級",
		L"基本自然管理技術者検定14級",
		L"基本自然管理技術者検定13級",
		L"基本自然管理技術者検定12級",
		L"基本自然管理技術者検定11級",
		L"基本自然管理技術者検定10級",
		L"基本自然管理技術者検定9級",
		L"基本自然管理技術者検定8級",
		L"基本自然管理技術者検定7級",
		L"基本自然管理技術者検定6級",
		L"基本自然管理技術者検定5級",
		L"基本自然管理技術者検定4級",
		L"基本自然管理技術者検定3級",
		L"基本自然管理技術者検定2級",
		L"基本自然管理技術者検定準1級",
		L"基本自然管理技術者検定1級",
		L"応用自然管理技術者検定18級",
		L"応用自然管理技術者検定17級",
		L"応用自然管理技術者検定16級",
		L"応用自然管理技術者検定15級",
		L"応用自然管理技術者検定14級",
		L"応用自然管理技術者検定13級",
		L"応用自然管理技術者検定12級",
		L"応用自然管理技術者検定11級",
		L"応用自然管理技術者検定10級",
		L"応用自然管理技術者検定9級",
		L"応用自然管理技術者検定8級",
		L"応用自然管理技術者検定7級",
		L"応用自然管理技術者検定6級",
		L"応用自然管理技術者検定5級",
		L"応用自然管理技術者検定4級",
		L"応用自然管理技術者検定3級",
		L"応用自然管理技術者検定2級",
		L"応用自然管理技術者検定準1級",
		L"応用自然管理技術者検定1級",
		L"フォルダ選択へ戻る"
	};

	unsigned int Color[NUMBER_OF_COURSES]{//それぞれの表示色		
		colorRatio(150, 255, 255),
		colorRatio(150, 255, 255),
		colorRatio(150, 255, 255),
		colorRatio(150, 255, 150),
		colorRatio(150, 255, 150),
		colorRatio(150, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 255),
		colorRatio(255, 150, 255),
		colorRatio(255, 150, 255),
		colorRatio(150, 150, 255),
		colorRatio(150, 150, 255),
		colorRatio(255, 255, 255),
		colorRatio(255, 255, 255),

		colorRatio(150, 255, 255),
		colorRatio(150, 255, 255),
		colorRatio(150, 255, 255),
		colorRatio(150, 255, 150),
		colorRatio(150, 255, 150),
		colorRatio(150, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 255, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 150),
		colorRatio(255, 150, 255),
		colorRatio(255, 150, 255),
		colorRatio(255, 150, 255),
		colorRatio(150, 150, 255),
		colorRatio(150, 150, 255),
		colorRatio(255, 255, 255),
		colorRatio(255, 255, 255),
		colorRatio(180, 180, 180)
	};

	unsigned int ShadowColor[NUMBER_OF_COURSES]{//それぞれの表示色(影)
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),

		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0),
		colorRatio(0,0,0)
	};



	char Kind[NUMBER_OF_COURSES] = { 
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,
		FOLDER_SKILL_TEST_KIND_BASIC,

		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_APPLIED,
		FOLDER_SKILL_TEST_KIND_BACK};//応用か基本かフォルダに戻るかのフラグ(0:基本 1:応用 2:フォルダに戻る)

	short bpmmin[NUMBER_OF_COURSES];
	short bpmmax[NUMBER_OF_COURSES];
	char pop[NUMBER_OF_COURSES] = {
		15,
		20,
		25,
		30,
		35,
		40,
		45,
		50,
		55,
		60,
		65,
		70,
		75,
		80,
		85,
		90,
		95,
		100,
		100,
		15,
		20,
		25,
		30,
		35,
		40,
		45,
		50,
		55,
		60,
		65,
		70,
		75,
		80,
		85,
		90,
		95,
		100,
		100,
		0
	};

	unsigned short version = 120;

}SkillTestList;


#endif