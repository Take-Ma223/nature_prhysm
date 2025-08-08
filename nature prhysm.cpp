#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_LIST.h"
#include"STRUCT_SONG.h"
#include"STRUCT_OP.h"
#include"STRUCT_RESULT.h"

#include"LOAD.h"
#include"TITLE.h"
#include"KeyConfigSaveLoad.h"
#include"STRUCT_CONFIG.h"
#include"ConfigLoad.h"

#include"STRUCT_SECRET.h"
#include"STRUCT_ANDROID_CONTROLLER.h"
#include"CONSTANT_VALUE.h"

#include "EffekseerForDXLib.h"
#include "STRUCT_IR_SETTING.h"
#include "IR_process.h"
#include "ActivityController.h"
#include "DxLibUtil.h"
#include "AutoDifficultyPrediction.h"
#include "WindowTitleSetter.h"
#include "SongSelectScreen.h"

void MakeScoreDate(wchar_t *title, int difficulty, int score, int Perfect, int Good, int Miss, int MaxCombo, int SkyPerfect, int MinMiss, int Clear, int Rainbow);
void EffekseerInitProcess();

#ifdef __ANDROID__
// プログラムは android_main から始まります
int android_main(void) {

}
#else
// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
#endif // !__ANDROID__

	_wchdir(L"../../");//作業ディレクトリをapplicationから親の親ディレクトリに移す

	wchar_t DX_PASSWORD[] = L"ntps";
	SetDXArchiveKeyString(DX_PASSWORD);

	//ログ出力
	SetOutApplicationLogValidFlag(FALSE);
#ifdef _DEBUG
	SetOutApplicationLogValidFlag(TRUE);
#endif


	SetGraphMode(1280, 720, 32);//画面サイズは1280*720

	Config config;
	ConfigLoad(&config);//Config.dat読み込み

	SetWaitVSyncFlag(config.Vsync);//垂直同期
	SetAlwaysRunFlag(TRUE);//非アクティブでも動作

	SetCreateSoundIgnoreLoopAreaInfo(TRUE);//LoadSoundMem などで読み込むサウンドデータにループ範囲情報があっても無視する

#ifdef __ANDROID__

#else

	SetWindowStyleMode(7);//最大化ボタン追加
	
	ChangeWindowMode(config.FullScreen == 0);//config.FullScreen が1ならフルスクリーン
	
	SetWindowSizeChangeEnableFlag(TRUE);//サイズ変更許可

	auto flag = ShowFlag(); flag.version = true;
	WindowTitleSetter::setText(flag);

	SetWindowIconID(333);//ウィンドウアイコンの設定
#endif
	//SetEnableXAudioFlag(TRUE);
	
	//サウンドデバイスを何使うか
	if (config.SoundOutputType == (int)SoundOutputType::DirectSound) {

	}
	else if (config.SoundOutputType == (int)SoundOutputType::WASAPI) {
		SetEnableWASAPIFlag(TRUE, config.WasapiExclusive);
	}
	else if (config.SoundOutputType == (int)SoundOutputType::ASIO) {
		SetUseASIODriverIndex(config.AsioDriver);
		SetEnableASIOFlag(TRUE, config.Buffer);
	}
	SetUseTransColor(false);//透過色を使用しない

	LPCWSTR font_path = L"font/NotoSansCJKjp-Black.otf"; // 読み込むフォントファイルのパス
	AddFontResourceEx(font_path, FR_PRIVATE, NULL);
	ChangeFont(L"Noto Sans CJK JP Black", DX_CHARSET_DEFAULT);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}
	SetSysCommandOffFlag(TRUE);
	SetDrawScreen(DX_SCREEN_BACK);// 描画先を裏画面にする
	EffekseerInitProcess();//Effekseer用初期化処理

	SetCreateGraphColorBitDepth(32);//作成するグラフィックのビット数設定

	AutoDifficultyPrediction adp;
	try {
		adp.bootServer();//難易度算出用サーバーを立てる
	}
	catch (exception e) {
		printfDx(L"AI難易度予測サーバーの起動に失敗したため、AI難易度予測が使えません。\n");
		printfDx(L"ゲームが動かない時.txtを参照してください。\n");

		ScreenFlip();
		Sleep(3000);
	}

	///////////////////////////////////難易度
	int C = 2, H = 3, SH = 4, F = 5, PF = 6;
	int rainbow = 1, normal = 0;
	//                                                         MaxCombo  MinMiss

	//MakeScoreDate(L"Just Falling Down", 3, 10494,                      104,        1,       0,          710,               605,        0,          PF,         1);
	//void MakeScoreDate(wchar_t *title, int difficulty, int score, int Perfect, int Good, int Miss, int MaxCombo, int SkyPerfect, int MinMiss, int Clear, int Normal) {

	//MakeScoreDate(L"Marine Perturbation", 3, 9917, 275, 68, 0, 909, 566, 0, F, 0);




	int i = 0;
	int list_number = 0;
	int song_number = 0;
	int difficulty = 1;
	Option option;//全体オプション構造体
	LIST folder;

	for (i = 0; i < NUMBER_OF_FOLDERS; i++) {
		folder.folder[i] = new song_number_diff[SONG_MAX_NUMBER * 4];
	}


	int Button[3][4] = { { KEY_INPUT_R,KEY_INPUT_T,KEY_INPUT_Y,KEY_INPUT_U }
		,{ KEY_INPUT_F,KEY_INPUT_G,KEY_INPUT_H,KEY_INPUT_J }
	,{ KEY_INPUT_V,KEY_INPUT_B,KEY_INPUT_N,KEY_INPUT_M } };//3*4のキー配置をどこにするか
														   /*
														   BBBB
														   GGGG
														   RRRR
														   この並びそのまま
														   */
	int Button_Shutter = KEY_INPUT_SPACE;//スクリーンショットボタン(デフォルトはSPACE)

	char Buf[256];
	int Key[256];//-1:離されたとき 0:押されていない 1~:押されている時間(フレーム)
	int NumberOfSongs = 0;//曲数
	int debug = 0;//デバッグモード(0:OFF 1:ON)
	Song *Music;//(添字が一つの曲を表す)曲単位の曲情報


	Music = (Song*)calloc(SONG_MAX_NUMBER, sizeof(Song));//メモリ確保

	SECRET secret;
	SkillTestList STList;
	secret.song_number= (int*)calloc(SECRET_SONG_NUMBER, sizeof(int));//メモリ確保
	secret.get_song_number = (BOOL*)calloc(SECRET_SONG_NUMBER, sizeof(BOOL));//メモリ確保
	secret.total = 0;
	secret.all_get = 0;

	
	int result_count = 0;//
	int result_rank_buf[SECRET_SONG_APPEAR_CYCLE];//ランク格納バッファ これを元に隠し曲出現演出の有無を決める
	
	for (i = 0; i <= SECRET_SONG_APPEAR_CYCLE - 1; i++) {
		result_rank_buf[i] = 0;
	}

	KeyConfigLoad(Button, &Button_Shutter);//キー配置変更
	/*
	char **note;
	note = (char**)calloc(4, sizeof(char));
	int i = 0;
	for (i = 0; i <= 3; i++) {
		note[i] = (char*)calloc(130000, sizeof(char));
	}
	*/
	//SCORE_CELL head;
	//head.next = NULL;
	//head.before = NULL;
	//EDIT_SCORE(&head);
	DATEDATA DateBuf;

	GetDateTime(&DateBuf);

	SRand(DateBuf.Hour * 3600 + DateBuf.Min * 60 + DateBuf.Sec);//乱数シード値決定


	printfDx(L"loading\n");
	ScreenFlip();



	ANDROID_CONTROLLER AC;
	for (i = 0; i <= 255; i++) {
		AC.Android_Key[i] = 0;//キー押下状態初期化
	}
	for (int i = 0; i <= 2; i++) {
		for (int j = 0; j <= 3; j++) {
			AC.Button[i][j] = Button[i][j];
		}
	}

	int SH_S = LoadSoundMem(L"sound/nature_prhysm_jingle.wav");

	AC.port = 50000;
	
	if ((AC.NetUDPHandle = MakeUDPSocket(AC.port)) == -1) {
		printfDx(L"ポート番号50000が使用できないためAndroidコントローラは使用できません\n");
	}
	else {
		//printfDx(L"無線LANに接続するとAndroidコントローラを使用できます\n");
	}


	//サーバー接続確認
	connectionConfirm(config);


	//初回起動時にIR idの取得
	IRgetID(config);

	ScreenFlip();

	//NetWorkSendUDP(NetUDPHandle, Ip, 4000, "12345", 6);
	
	/*
	while(CheckNetWorkRecvUDP(AC.NetUDPHandle) == FALSE)
	{
		// ウインドウズメッセージ処理
		if (ProcessMessage() < 0) break;
		//printfDx("Check:%d\n", check);

		ScreenFlip();//画面の反映
					 //clsDx();
	}
	
	while (1) {
		if (ProcessMessage() != 0) {
			dxLibFinishProcess();
		}
		
		int res = 0;
		sprintfDx(AC.StrBuf, "0");
		if (CheckNetWorkRecvUDP(AC.NetUDPHandle) == TRUE) {
			res = NetWorkRecvUDP(AC.NetUDPHandle, NULL, &AC.port, AC.StrBuf, 256, FALSE);
			PlaySoundMem(SH_S, DX_PLAYTYPE_BACK, TRUE);
		}


		

		//NetWorkRecvUDP(NetUDPHandle, &Ip,&port, StrBuf, 256, FALSE);
		printfDx("res:%d data:%s\n", res, AC.StrBuf);
		
		
		ScreenFlip();//画面の反映
		//clsDx();
		Sleep(1);
	}
	*/ 

	IR_SETTING ir;

	//auto ac = ActivityController(&option, &config);
	//UIテスト画面の開始
	//ac.start();


	LOAD(&folder, Music, &NumberOfSongs, &secret, &STList, &option, config, &ir);
	if (ProcessMessage() != 0) {dxLibFinishProcess();return -1;}

	int SelectingTarget = SongSelect::SongSelectScreen::SELECTING_FOLDER;
	while (1) {
		TITLE(Music, &NumberOfSongs, Button, Button_Shutter, Key, Buf, &AC, config, &option, &ir);
		if (ProcessMessage() != 0) { dxLibFinishProcess(); return -1; }
		int StageCount = 0;
		bool isBackToTitle = false;
		
		while (1) {
			if (ProcessMessage() != 0) { dxLibFinishProcess(); return -1; }
			auto song_select_screen = SongSelect::SongSelectScreen(
				&list_number,
				&song_number, &difficulty,
				&option,
				&folder,
				Music,
				Button, Button_Shutter, Key, Buf,
				&debug,
				&NumberOfSongs,
				&result_count,
				result_rank_buf,
				&secret,
				&AC,
				&StageCount,
				&STList,
				config,
				&ir,
				&isBackToTitle,
				&SelectingTarget
			);
			song_select_screen.run();

			//SONG_SELECT(&list_number,
			//	&song_number, &difficulty,
			//	&option,
			//	&folder,
			//	Music,
			//	Button, Button_Shutter, Key, Buf,
			//	&debug,
			//	&NumberOfSongs,
			//	&result_count,
			//	result_rank_buf,
			//	&secret,
			//	&AC,
			//	&StageCount,
			//	&STList,
			//	config,
			//	&ir,
			//	&isBackToTitle);
			StageCount++;

			if (isBackToTitle)break;
		}
	}

	//GAME();
	free(Music);

	dxLibFinishProcess();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

void MakeScoreDate(wchar_t *title,int difficulty,int score,int Perfect,int Good, int Miss, int MaxCombo, int SkyPerfect,int MinMiss, int Clear,int Normal) {
	RESULT res;

	res.hash_result = 0;
	res.hash = 0;

	res.score = score;
	res.difficulty = difficulty;
	res.clear = Clear;
	res.sky_perfect = SkyPerfect;
	res.perfect = Perfect;
	res.good = Good;
	res.miss = Miss;
	res.max_combo = MaxCombo;
	res.min_miss = MinMiss;
	res.play_count = 32;
	res.save_data_version = RESULT_DATA_VERSION_MIN_MISS;

	if (score >= 5000)res.rank = 2;
	if (score >= 6000)res.rank = 3;
	if (score >= 7000)res.rank = 4;
	if (score >= 8000)res.rank = 5;
	if (score >= 9000)res.rank = 6;
	if (score >= 9500)res.rank = 7;

	FILE *fp = 0;
	errno_t error = 0;
	wchar_t filename[256];
	int i = 0;
	
	for (i = 0; i <= 255; i++) {//初期化
		filename[i] = L'\0';
	}
	sprintfDx(filename, L"score/official2/%s", title);

	CreateDirectory(filename, NULL);
	wcscat_s(filename, L"/result_");

	if (Normal == 1) {//通常モードだったら
		if (difficulty == 1)wcscat_s(filename, L"sunny.dat");
		if (difficulty == 2)wcscat_s(filename, L"cloudy.dat");
		if (difficulty == 3)wcscat_s(filename, L"rainy.dat");
		if (difficulty == 4)wcscat_s(filename, L"thunder.dat");

	}
	if (Normal == 0) {//虹モードだったら
		if (difficulty == 1)wcscat_s(filename, L"sunny_r.dat");
		if (difficulty == 2)wcscat_s(filename, L"cloudy_r.dat");
		if (difficulty == 3)wcscat_s(filename, L"rainy_r.dat");
		if (difficulty == 4)wcscat_s(filename, L"thunder_r.dat");
	}
	error = _wfopen_s(&fp, filename, L"wb");
	if (error != 0) {
		printfDx(L"%sのオープンに失敗しました.\n", filename);
		printfDx(L"エラー番号:%d", error);
		ScreenFlip();

		Sleep(100000);
		exit(EXIT_FAILURE);
	}
	fwrite(&res, sizeof(res), 1, fp);
	fclose(fp);

	/*
	MakeScoreDate(L"OVER THE RAINBOW!!!", 3, 10249, 272, 41, 1, 1034, 736, 0, F, 0);
	MakeScoreDate(L"OVER THE RAINBOW!!!", 4, 8890, 486, 229, 39, 261, 764, 39, H, 0);

	MakeScoreDate(L"Acid Rain", 4, 10445, 292, 22, 0, 1147, 833, 0, F, 0);
	MakeScoreDate(L"snow town story", 4, 10341, 148, 18, 0, 772, 606, 0, F, 0);
	MakeScoreDate(L"高気圧スカイトリップ", 4, 10402, 156, 11, 0, 794, 627, 0, F, 0);

	MakeScoreDate(L"lost sky", 4, 9885, 297, 79, 7, 584, 687, 7, SH, 0);
	MakeScoreDate(L"トリノユメ", 3, 10438, 163, 11, 0, 840, 666, 0, F, 0);
	MakeScoreDate(L"トリノユメ", 4, 10180, 293, 67, 11, 469, 921, 11, SH, 0);

	MakeScoreDate(L"grow away", 4, 10457, 152, 19, 1, 901, 735, 0, F, 0);
	MakeScoreDate(L"Asphalt Memory", 4, 10606, 60, 0, 0, 726, 666, 0, PF, 0);
	MakeScoreDate(L"Luna", 4, 10000, 133, 10, 0, 856, 713, 0, F, 0);
	MakeScoreDate(L"Nyctophobia", 3, 10000, 161, 21, 4, 419, 857, 0, F, 0);
	MakeScoreDate(L"Sprout!!", 4, 10551, 210, 9, 0, 1024, 805, 0, F, 0);
	MakeScoreDate(L"アルキジャ遺跡と災いの笛", 3, 10319, 145, 14, 2, 747, 602, 0, F, 0);
	MakeScoreDate(L"Angelic Sky", 4, 10000, 295, 39, 2, 749, 738, 2, SH, 0);
	MakeScoreDate(L"A Piece of Winter", 4, 10428, 193, 31, 0, 1000, 776, 0, F, 0);
	MakeScoreDate(L"Electric Blue", 4, 10584, 132, 12, 0, 925, 781, 0, F, 0);
	MakeScoreDate(L"雷鳴の後継者", 4, 10471, 206, 35, 3, 1122, 888, 3, SH, 0);
	MakeScoreDate(L"The Top of 223", 3, 10252, 214, 28, 1, 498, 645, 0, F, 0);
	MakeScoreDate(L"Destination Landscape", 4, 10697, 107, 5, 0, 943, 831, 0, F, 0);
	MakeScoreDate(L"Marine Perturbation", 3, 10045, 211, 51, 5, 795, 642, 5, SH, 0);
	MakeScoreDate(L"MAD ACCELERATOR DESTRUCTION", 3, 10000, 203, 17, 0, 903, 683, 0, F, 0);
	MakeScoreDate(L"グリード", 3, 10297, 211, 13, 0, 812, 588, 0, F, 0);
	MakeScoreDate(L"Breaking Clouds", 3, 10503, 173, 5, 0, 882, 704, 0, F, 0);
	MakeScoreDate(L"Future Prism", 3, 10477, 124, 9, 0, 791, 658, 0, F, 0);
	MakeScoreDate(L"四季即是空", 4, 10489, 75, 0, 0, 639, 564, 0, PF, 0);
	MakeScoreDate(L"Pray to the Stars ~流れ星~", 4, 10450, 125, 5, 1, 695, 631, 0, F, 0);
	MakeScoreDate(L"Piano Train", 4, 10723, 105, 0, 0, 933, 828, 0, PF, 0);
	MakeScoreDate(L"hallucination", 3, 9906, 677, 13, 0, 690, 0, 0, F, 0);
	MakeScoreDate(L"hallucination", 4, 10223, 156, 20, 5, 776, 597, 0, F, 0);
	MakeScoreDate(L"Constructive Future", 4, 9919, 851, 14, 0, 865, 0, 0, F, 0);
	MakeScoreDate(L"Bright Stars", 4, 9923, 765, 12, 0, 777, 0, 0, F, 0);
	MakeScoreDate(L"What Color Is The Sky？" , 3, 9911, 826, 15,0, 841, 0, 0, F, 0);
	MakeScoreDate(L"maple road", 3, 10000, 612, 0, 0, 612, 0, 0, PF, 0);

	MakeScoreDate(L"Pray to the Stars ~流れ星~", 3, 9735, 585, 16, 5, 543, 0, 2, SH, 1);
	MakeScoreDate(L"Pray to the Stars ~流れ星~", 4, 8314, 646, 62, 56, 124, 0, 56, C, 1);
	MakeScoreDate(L"夢幻空間", 3, 9507, 514, 23, 10, 291, 0, 10, SH, 1);
	MakeScoreDate(L"Nyctophobia", 3, 7616, 854, 96, 101, 138, 0, 101, C, 1);
	MakeScoreDate(L"maple road", 3, 9647, 591, 11, 10, 247, 0, 10, SH, 1);
	MakeScoreDate(L"Life in Green", 1, 10000, 120, 0, 0, 120, 0, 0, PF, 1);
	MakeScoreDate(L"Life in Green", 2, 10000, 239, 0, 0, 239, 0, 0, PF, 1);
	MakeScoreDate(L"Life in Green", 3, 10000, 388, 0, 0, 388, 0, 0, PF, 1);
	MakeScoreDate(L"A.O", 1, 10000, 94, 0, 0,  94, 0, 0, PF, 1);
	MakeScoreDate(L"A.O", 2, 10000, 238, 0, 0, 238, 0, 0, PF, 1);
	MakeScoreDate(L"A.O", 3, 10000, 423, 0, 0, 423, 0, 0, PF, 1);
	MakeScoreDate(L"MAD ACCELERATOR DESTRUCTION", 2, 9561, 644, 21, 12, 264, 0, 12, SH, 1);
	MakeScoreDate(L"MAD ACCELERATOR DESTRUCTION", 3, 8354, 269, 100, 63, 132, 474, 63, C, 1);
	MakeScoreDate(L"What Color Is The Sky？" , 2, 9200, 485, 15, 22, 290, 0, 29, C, 1);
	MakeScoreDate(L"Ruin Industry", 2, 9374, 424, 13, 15, 142, 0, 15, H, 1);
	MakeScoreDate(L"Ruin Industry", 3, 9151, 730, 27, 32, 353, 0, 32, H, 1);
	MakeScoreDate(L"The Top of 223", 2, 8269, 367, 35, 41, 86, 0, 41, C, 1);
	MakeScoreDate(L"sweet sky", 3, 8643, 534, 43, 37, 105, 0, 37, H, 1);
	MakeScoreDate(L"アルキジャ遺跡と災いの笛", 2, 8366, 381, 80, 29, 108, 0, 29, H, 1);
	MakeScoreDate(L"アルキジャ遺跡と災いの笛", 3, 9690, 184, 41, 17, 322, 522, 17, SH, 1);
	MakeScoreDate(L"トリノユメ", 2, 8000, 406, 85, 57, 85, 0, 57, C, 1);
	MakeScoreDate(L"トリノユメ", 3, 7761, 216, 93, 88, 156, 446, 88, C, 1);
	MakeScoreDate(L"Constructive Future", 3, 9511, 706, 35, 11, 256, 0, 11, SH, 1);
	MakeScoreDate(L"雪解けの街にさよなら", 3, 7488, 906, 78, 113, 131, 0, 113, C, 1);
	MakeScoreDate(L"Angelic Sky", 3, 9564, 700, 34, 9, 300, 0, 9, H, 1);
	MakeScoreDate(L"Angelic Sky", 4, 8098, 887, 118, 71, 179, 0, 71, C, 1);
	MakeScoreDate(L"Chip Adventures", 3, 9349, 493, 17, 17, 130, 0, 17, H, 1);
	MakeScoreDate(L"そよ風の町と夏の忘れもの", 3, 7343, 717, 71, 103, 88, 0, 103, C, 1);
	MakeScoreDate(L"hallucination", 3, 10125, 145, 5, 11, 497, 532, 5, SH, 1);
	MakeScoreDate(L"hallucination", 4, 8475, 637, 104, 40, 146, 0, 40, H, 1);
	MakeScoreDate(L"Just Falling Down", 3, 9593, 674, 27, 9, 236, 0, 9, SH, 1);
	MakeScoreDate(L"Sprout!!", 3, 9641, 713, 22, 9, 255, 0, 9, SH, 1);
	MakeScoreDate(L"Electric Blue", 3, 9557, 606, 27, 9, 527, 0, 9, SH, 1);
	MakeScoreDate(L"Electric Blue", 4, 7385, 654, 197, 74, 189, 0, 74, C, 1);
	MakeScoreDate(L"KONOHA STREAM", 3, 9554, 750, 20, 14, 325, 0, 14, SH, 1);
	MakeScoreDate(L"Asphalt Memory", 4, 8139, 599, 72, 57, 115, 0, 57, C, 1);
	MakeScoreDate(L"A Piece of Winter", 4, 7095, 742, 151, 108, 178, 0, 108, C, 1);
	MakeScoreDate(L"高気圧スカイトリップ", 4, 7219, 575, 144, 91, 79, 0, 91, C, 1);
	MakeScoreDate(L"AO-ZORA Express", 4, 9193, 785, 29, 29, 167, 0, 29, H, 1);
	MakeScoreDate(L"Bright Stars", 4, 8108, 625, 97, 56, 160, 0, 56, C, 1);
	MakeScoreDate(L"グリード", 3, 8059, 670, 76, 66, 93, 0, 65, C, 1);
	MakeScoreDate(L"snow town story", 4, 7577, 631, 53, 92, 59, 0, 92, C, 1);
	MakeScoreDate(L"Marine Perturbation", 2, 7606, 161, 96, 65, 111, 274, 65, C, 1);
	MakeScoreDate(L"Marine Perturbation", 3, 6762, 294, 199, 104, 0, 316, 104, C, 1);
	MakeScoreDate(L"OVER THE RAINBOW!!!", 2, 6614, 180, 166, 87, 84, 207, 87, C, 1);
	MakeScoreDate(L"最後の帰り道", 3, 9165, 119, 22, 23, 81, 276, 23, H, 1);
	MakeScoreDate(L"四季即是空", 1, 10053, 7, 0, 0, 67, 60, 0, PF, 1);
	MakeScoreDate(L"四季即是空", 4, 7808, 165, 70, 70, 177, 334, 70, C, 1);
	MakeScoreDate(L"Piano Train", 4, 9784, 242, 45, 18, 243, 629, 18, H, 1);
	MakeScoreDate(L"移ろう景色の変わらない色", 3, 9105, 180, 42, 46, 157, 556, 46, H, 1);
	MakeScoreDate(L"grow away", 4, 8032, 243, 80, 88, 213, 502, 88, C, 1);
	MakeScoreDate(L"雷鳴の後継者", 4, 6706, 457, 218, 118, 109, 340, 118, C, 1);
	MakeScoreDate(L"Breaking Clouds", 3, 9376, 209, 41, 37, 286, 596, 37, H, 1);
	MakeScoreDate(L"Luna", 4, 7118, 265, 141, 99, 83, 352, 99, C, 1);
	MakeScoreDate(L"first wind", 3, 9127, 619, 22, 29, 170, 0, 29, H, 1);
	*/

}

void EffekseerInitProcess() {

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
    // Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// Effekseerを初期化する。
	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effekseer_Init(8000) == -1)
	{
		dxLibFinishProcess();
		return;
	}

	// フルスクリーンウインドウの切り替えでリソースが消えるのを防ぐ。
	// Effekseerを使用する場合は必ず設定する。
	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	// ただし、DirectX11を使用する場合は実行する必要はない。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

	// Effekseerに2D描画の設定をする。
	Effekseer_Set2DSetting(1280, 720);

	// Zバッファを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetUseZBuffer3D(TRUE);

	// Zバッファへの書き込みを有効にする。
	// Effekseerを使用する場合、2DゲームでもZバッファを使用する。
	SetWriteZBuffer3D(TRUE);

	return;
}
















