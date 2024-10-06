#ifndef _CONSTANT_VALUE
#define _CONSTANT_VALUE

#define NOTE_MAX_NUMBER 10000//1レーンの最大音符数 6500は無理 6000ぐらいまでいけた
#define BARLINE_MAX_NUMBER 50000//小節線の最大数
constexpr auto SONG_MAX_NUMBER = 9999;//最大曲数
const wchar_t APPLICATION_VERSION[] = L"1.39";//バージョン

constexpr int OFFSET_MARGIN = 100;//音声、動画の先頭を合わせるためのオフセットマージン

#define SECRET_SONG_APPEAR_CYCLE 3//3曲Aを連続で取れば隠し曲が一回出現

#define SECRET_ALWAYS_APPEAR 0//隠し曲を毎回出現させる
#define SECRET_NOT_USE 0//隠し曲を最初から解禁させる
#define SECRET_GAUGE_CHANGE_OFF 1//隠し曲プレー時のゲージ強制変更をオフ

#define SHOW_DEBUG_MODE 1//DEBUG MODEのときにDEBUG MODEの表示をする

#define SEND_EXIST_SCORE_TO_IR 0//F9で既存スコアをIRに送信

#define ENABLE_GENERATING_LERNING_DATA 0//学習用データ生成

#define ENABLE_GENERATING_RADAR_LIST 0 //公式譜面・レーダーリスト作成

const wchar_t RUN_PASS[] = L"223210";

#define NATURE_PRHYSM_FONT L"メイリオ"

#define FIRST_ACTIVITY TestActivity(&context);

#endif