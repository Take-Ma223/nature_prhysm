#include"STRUCT_IR_SETTING.h"
#include"STRUCT_RESULT.h"
#include"STRUCT_Song.h"
#include"STRUCT_CONFIG.h"

#ifndef _IRSETTINGSAVELOAD
#define _IRSETTINGSAVELOAD


void SaveIRSetting(IR_SETTING ir);
void LoadIRSetting(IR_SETTING* ir);
int LoadTargetScore(wchar_t* saveFolderPass);
int LoadConnectionState();
void IRgetID(CONFIG config);
void IRsave(wchar_t* npsPath, wchar_t* folderPath, RESULT res, int difficulty, int season, BOOL rainbow, BOOL onlyOption, CONFIG config);
void IRsend(IR_SETTING* ir, wchar_t* npsPath, wchar_t* folderPath, int difficulty, int rainbow, CONFIG config);
void IRview(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, CONFIG config);
void getTargetScore(wchar_t* npsPath, wchar_t* folderPath, BOOL rainbow, int mode, int score, int rivalID, CONFIG config);

const int CONNECTION_SUCCESS = 0;  // 接続成功
const int CONNECTION_SERVER_NOT_FOUND = 1;  // サーバーが見つからない
const int CONNECTION_VERSION_ERROR = 2;  // サーバーには繋がったがバージョンが違う
void connectionConfirm(CONFIG config);


#endif