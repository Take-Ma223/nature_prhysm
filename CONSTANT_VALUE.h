#ifndef _CONSTANT_VALUE
#define _CONSTANT_VALUE

#define NOTE_MAX_NUMBER 10000//1レーンの最大音符数 6500は無理 6000ぐらいまでいけた
#define BARLINE_MAX_NUMBER 50000//小節線の最大数
#define SONG_MAX_NUMBER 1024//最大曲数 2300までいける
#define VERSION 1.30        //バージョン

#define SECRET_SONG_APPEAR_CYCLE 3//3曲Aを連続で取れば隠し曲が一回出現

#define SECRET_ALWAYS_APPEAR 0//隠し曲を毎回出現させる
#define SECRET_NOT_USE 0//隠し曲を最初から解禁させる
#define SECRET_GAUGE_CHANGE_OFF 1//隠し曲プレー時のゲージ強制変更をオフ

#define SHOW_DEBUG_MODE 1//DEBUG MODEのときにDEBUG MODEの表示をする

const wchar_t RUN_PASS[] = L"223210";

#endif