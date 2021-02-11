//NOTE構造体等に関連する定数
#ifndef _CONST_NOTE
#define _CONST_NOTE

//ノートの色(1~9の数字)(1:R 2:G 3:B 4:Y 5:C 6:M 7:W 8:K 9:F(FullColor) 10:ロングノート終点)0ならこのノートは使ってない
#define NOTE_COLOR_RED 1
#define NOTE_COLOR_GREEN 2
#define NOTE_COLOR_BLUE 3
#define NOTE_COLOR_YELLOW 4
#define NOTE_COLOR_CYAN 5
#define NOTE_COLOR_MAGENTA 6
#define NOTE_COLOR_WHITE 7
#define NOTE_COLOR_BLACK 8
#define NOTE_COLOR_RAINBOW 9

#define NOTE_GROUP_NORMAL 0//単ノート
#define NOTE_GROUP_LN_START 1//LN始点
#define NOTE_GROUP_LN_END 2//LN終点

#define NOTE_NOT_BRIGHT 0//光らない音符
#define NOTE_BRIGHT 1//光る音符


#endif