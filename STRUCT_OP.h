#ifndef _STRUCT_OP
#define _STRUCT_OP

typedef struct OP {//オプション構造体(ロード時に渡す値)
	int speed = 11;//ハイスピード番号
	double speed_val = 1;//ハイスピ倍率
	int gauge = 0;//ゲージタイプ(0:ノーマルゲージ(青) 1:ハードゲージ(緑) 2:スーパーハードゲージ(赤) 3:FCゲージ(銀) 4:PFCゲージ(金))
	int lane = 0;//レーンオプション
	int color = 0;//色オプション
	int note = 0;//ノートスキン
	int hitsound = 0;//ヒット音スキン
	int barline = 0;//小節線表示
	int darkness = 0;//演奏画面の背景の暗さ
	int scoregraph = 0;//スコアグラフ表示するかどうか
	int sort = 0;//選曲画面のソート種類
	int black_gradation = 0;//黒終端ノーツのグラデーション
	int theme = 0;//テーマスキン
	int targetscore1 = 0;//TARGET SCORE1
	int targetscore2 = 0;//TARGET SCORE2
	int fastSlow = 1;//FAST/SLOW
}OP;

typedef struct OPTION {//オプション構造体(全体)
	OP op;

	static const int OPTION_NUM = 15;//オプションの数

	wchar_t* OptionName[OPTION_NUM] = { L"SPEED",L"GAUGE",L"LANE",L"COLOR",L"NOTE",L"HIT SOUND",L"FAST/SLOW",L"BARLINE",L"NIGHT",L"GRADATION",L"SCORE GRAPH",L"TARGET SCORE1",L"TARGET SCORE2",L"THEME",L"SORT"};

	static const int SPEED_NUM = 78;//ハイスピの数
	static const int GAUGE_NUM = 6;//ゲージ種類の数
	static const int LANE_NUM = 4;//レーンオプションの数
	static const int COLOR_NUM = 8;//色オプションの数
	int NOTE_NUM = 0;//ノートスキンオプションの数
	int HITSOUND_NUM = 0;//ヒット音オプションの数
	static const int FAST_SLOW_NUM = 3;//FAST/SLOWオプションの数
	static const int BARLINE_NUM = 2;//小節線オプションの数
	static const int DARKNESS_NUM = 5;//黒背景オプションの数
	static const int SCORE_GRAPH_NUM = 2;//スコアグラフオプションの数
	static const int TARGET_SCORE1_NUM = 8;//スコアグラフターゲットオプションの数
	static const int TARGET_SCORE2_NUM = 6;//スコアグラフターゲットオプションの数
	static const int SORT_NUM = 24;//曲ソート種類の数
	static const int BLACK_GRADATION_NUM = 2;//黒終端ロング表示オプションの数
	int THEME_NUM = 0;//テーマオプションの数

	const int OP_SPEED = 0;
	const int OP_GAUGE = 1;
	const int OP_LANE = 2;
	const int OP_COLOR = 3;
	const int OP_NOTE = 4;
	const int OP_HITSOUND = 5;
	const int OP_FAST_SLOW = 6;
	const int OP_BARLINE = 7;
	const int OP_DARKNESS = 8;
	const int OP_GRADATION = 9;
	const int OP_SCORETARGET = 10;
	const int OP_TARGETSCORE1 = 11;
	const int OP_TARGETSCORE2 = 12;
	const int OP_THEME = 13;
	const int OP_SORT = 14;

	const int OP_GAUGE_NORMAL = 0;
	const int OP_GAUGE_HARD = 1;
	const int OP_GAUGE_SUPER_HARD = 2;
	const int OP_GAUGE_FC_ATTACK = 3;
	const int OP_GAUGE_PFC_ATTACK = 4;
	const int OP_GAUGE_NO_FAIL = 5;
	const int OP_GAUGE_SKILL_TEST = 6;


	const int OP_LANE_NONE = 0;
	const int OP_LANE_MIRROR = 1;
	const int OP_LANE_RANDOM = 2;
	const int OP_LANE_SUPER_RAND = 3;

	const int OP_COLOR_NONE = 0;
	const int OP_COLOR_RGB_CHANGE = 1;
	const int OP_COLOR_RGB_RAND = 2;
	const int OP_COLOR_SUPER_RAND = 3;
	const int OP_COLOR_RGB_ONLY = 4;
	const int OP_COLOR_CMY_ONLY = 5;
	const int OP_COLOR_W_ONLY = 6;
	const int OP_COLOR_RAINBOW = 7;

	const int OP_FAST_SLOW_OFF = 0;
	const int OP_FAST_SLOW_ON_FAST_CYAN = 1;
	const int OP_FAST_SLOW_ON_FAST_RED = 2;

	const int OP_BARLINE_ON = 0;
	const int OP_BARLINE_OFF = 1;

	const int OP_DARKNESS_50 = 2;
	const int OP_DARKNESS_75 = 3;
	const int OP_DARKNESS_100 = 4;

	const int OP_BLACK_GRADATION_ON = 0;
	const int OP_BLACK_GRADATION_OFF = 1;

	const int OP_SCORE_GRAPH_OFF = 0;
	const int OP_SCORE_GRAPH_ON = 1;

	const int OP_TARGET_SCORE1_E = 0;
	const int OP_TARGET_SCORE1_D = 1;
	const int OP_TARGET_SCORE1_C = 2;
	const int OP_TARGET_SCORE1_B = 3;
	const int OP_TARGET_SCORE1_A = 4;
	const int OP_TARGET_SCORE1_S = 5;
	const int OP_TARGET_SCORE1_10000 = 6;
	const int OP_TARGET_SCORE1_MAX = 7;

	const int OP_TARGET_SCORE2_RIVAL = 0;
	const int OP_TARGET_SCORE2_AVERAGE = 1;
	const int OP_TARGET_SCORE2_MEDIAN = 2;
	const int OP_TARGET_SCORE2_NEXTRANK = 3;
	const int OP_TARGET_SCORE2_TOP = 4;
	const int OP_TARGET_SCORE2_LATEST = 5;

	const int OP_SORT_NAME = 0;
	const int OP_SORT_SCORE = 1;
	const int OP_SORT_CLEAR_STATE = 2;
	const int OP_SORT_MIN_MISS = 3;
	const int OP_SORT_PLAY_COUNT = 4;
	const int OP_SORT_RADAR = 5;
	const int OP_SORT_GLOBAL = 6;
	const int OP_SORT_LOCAL = 7;
	const int OP_SORT_CHAIN = 8;
	const int OP_SORT_UNSTABILITY = 9;
	const int OP_SORT_STREAK = 10;
	const int OP_SORT_COLOR = 11;
	const int OP_SORT_RED_DENSITY = 12;
	const int OP_SORT_GREEN_DENSITY = 13;
	const int OP_SORT_BLUE_DENSITY = 14;
	const int OP_SORT_CYAN_DENSITY = 15;
	const int OP_SORT_MAGENTA_DENSITY = 16;
	const int OP_SORT_YELLOW_DENSITY = 17;
	const int OP_SORT_WHITE_DENSITY = 18;
	const int OP_SORT_BLACK_DENSITY = 19;
	const int OP_SORT_RAINBOW_DENSITY = 20;
	const int OP_SORT_MAX_BPM = 21;
	const int OP_SORT_MIN_BPM = 22;
	const int OP_SORT_VERSION = 23;

	double speed_val[SPEED_NUM] = {
		10,
		20,
		30,
		40,
		50,
		60,
		70,
		80,
		90,
		100,
		110,
		120,
		130,
		140,
		150,
		160,
		170,
		180,
		190,
		200,
		210,
		220,
		230,
		240,
		250,
		260,
		270,
		280,
		290,
		300,
		310,
		320,
		330,
		340,
		350,
		360,
		370,
		380,
		390,
		400,
		410,
		420,
		430,
		440,
		450,
		460,
		470,
		480,
		490,
		500,
		510,
		520,
		530,
		540,
		550,
		560,
		570,
		580,
		590,
		600,
		625,
		650,
		675,
		700,
		725,
		750,
		775,
		800,
		850,
		900,
		950,
		1000,
		1100,
		1200,
		1300,
		1400,
		1500,
		1600};


	wchar_t *speed[SPEED_NUM] = {
		L"10",
		L"20",
		L"30",
		L"40",
		L"50",
		L"60",
		L"70",
		L"80",
		L"90",
		L"100",
		L"110",
		L"120",
		L"130",
		L"140",
		L"150",
		L"160",
		L"170",
		L"180",
		L"190",
		L"200",
		L"210",
		L"220",
		L"230",
		L"240",
		L"250",
		L"260",
		L"270",
		L"280",
		L"290",
		L"300",
		L"310",
		L"320",
		L"330",
		L"340",
		L"350",
		L"360",
		L"370",
		L"380",
		L"390",
		L"400",
		L"410",
		L"420",
		L"430",
		L"440",
		L"450",
		L"460",
		L"470",
		L"480",
		L"490",
		L"500",
		L"510",
		L"520",
		L"530",
		L"540",
		L"550",
		L"560",
		L"570",
		L"580",
		L"590",
		L"600",
		L"625",
		L"650",
		L"675",
		L"700",
		L"725",
		L"750",
		L"775",
		L"800",
		L"850",
		L"900",
		L"950",
		L"1000",
		L"1100",
		L"1200",
		L"1300",
		L"1400",
		L"1500",
		L"1600" };//オプションの名前
	
	
	wchar_t* gauge[GAUGE_NUM + 1] = { L"NORMAL",L"HARD",L"SUPER HARD",L"FC ATTACK",L"PFC ATTACK",L"NO FAIL", L"SKILL TEST"};
	wchar_t* lane[LANE_NUM] = { L"NONE",L"MIRROR",L"RANDOM",L"SUPER RAND" };
	wchar_t* color[COLOR_NUM] = { L"NONE",L"RGB CHANGE",L"RGB RAND",L"SUPER RAND",L"RGB ONLY",L"CMY ONLY",L"W ONLY",L"RAINBOW" };
	wchar_t** note = 0;//名前はフォルダ名から取る パスの役割も果たす
	wchar_t** hitsound = 0;//名前はフォルダ名から取る パスの役割も果たす
	wchar_t* fastSlow[FAST_SLOW_NUM] = { L"OFF",L"ON:C/R",L"ON:R/C" };
	wchar_t* barline[BARLINE_NUM] = { L"ON",L"OFF"};
	wchar_t* darkness[DARKNESS_NUM] = { L"0%",L"25%",L"50%",L"75%",L"100%" };
	wchar_t* scoretarget[SCORE_GRAPH_NUM] = { L"OFF", L"ON"};
	wchar_t* targetscore1[TARGET_SCORE1_NUM] = { L"E", L"D", L"C", L"B", L"A", L"S", L"10000", L"MAX"};
	wchar_t* targetscore2[TARGET_SCORE2_NUM] = { L"RIVAL", L"AVERAGE", L"MEDIAN", L"NEXT RANK", L"TOP", L"LATEST"
};

	wchar_t* sort[SORT_NUM] = { L"NAME", L"SCORE", L"CLEAR STATE",L"MIN MISS", L"PLAY COUNT",L"RADAR",L"GLOBAL",L"LOCAL",L"CHAIN",L"UNSTABILITY",L"STREAK",L"COLOR",
		L"RED",L"GREEN",L"BLUE",L"CYAN",L"MAGENTA",L"YELLOW",L"WHITE",L"BLACK",L"RAINBOW",
		L"MAX BPM",L"MIN BPM", L"VERSION" };
	wchar_t* black_gradation[BLACK_GRADATION_NUM] = { L"ON", L"OFF" };
	wchar_t** theme = 0;//名前はフォルダ名から取る パスの役割も果たす

	//説明文
	wchar_t *sent_speed[SPEED_NUM] = {
		L"音符の速さを10に合わせます",
		L"音符の速さを20に合わせます",
		L"音符の速さを30に合わせます",
		L"音符の速さを40に合わせます",
		L"音符の速さを50に合わせます",
		L"音符の速さを60に合わせます",
		L"音符の速さを70に合わせます",
		L"音符の速さを80に合わせます",
		L"音符の速さを90に合わせます",
		L"音符の速さを100に合わせます",
		L"音符の速さを110に合わせます",
		L"音符の速さを120に合わせます",
		L"音符の速さを130に合わせます",
		L"音符の速さを140に合わせます",
		L"音符の速さを150に合わせます",
		L"音符の速さを160に合わせます",
		L"音符の速さを170に合わせます",
		L"音符の速さを180に合わせます",
		L"音符の速さを190に合わせます",
		L"音符の速さを200に合わせます",
		L"音符の速さを210に合わせます",
		L"音符の速さを220に合わせます",
		L"音符の速さを230に合わせます",
		L"音符の速さを240に合わせます",
		L"音符の速さを250に合わせます",
		L"音符の速さを260に合わせます",
		L"音符の速さを270に合わせます",
		L"音符の速さを280に合わせます",
		L"音符の速さを290に合わせます",
		L"音符の速さを300に合わせます",
		L"音符の速さを310に合わせます",
		L"音符の速さを320に合わせます",
		L"音符の速さを330に合わせます",
		L"音符の速さを340に合わせます",
		L"音符の速さを350に合わせます",
		L"音符の速さを360に合わせます",
		L"音符の速さを370に合わせます",
		L"音符の速さを380に合わせます",
		L"音符の速さを390に合わせます",
		L"音符の速さを400に合わせます",
		L"音符の速さを410に合わせます",
		L"音符の速さを420に合わせます",
		L"音符の速さを430に合わせます",
		L"音符の速さを440に合わせます",
		L"音符の速さを450に合わせます",
		L"音符の速さを460に合わせます",
		L"音符の速さを470に合わせます",
		L"音符の速さを480に合わせます",
		L"音符の速さを490に合わせます",
		L"音符の速さを500に合わせます",
		L"音符の速さを510に合わせます",
		L"音符の速さを520に合わせます",
		L"音符の速さを530に合わせます",
		L"音符の速さを540に合わせます",
		L"音符の速さを550に合わせます",
		L"音符の速さを560に合わせます",
		L"音符の速さを570に合わせます",
		L"音符の速さを580に合わせます",
		L"音符の速さを590に合わせます",
		L"音符の速さを600に合わせます",
		L"音符の速さを625に合わせます",
		L"音符の速さを650に合わせます",
		L"音符の速さを675に合わせます",
		L"音符の速さを700に合わせます",
		L"音符の速さを725に合わせます",
		L"音符の速さを750に合わせます",
		L"音符の速さを775に合わせます",
		L"音符の速さを800に合わせます",
		L"音符の速さを850に合わせます",
		L"音符の速さを900に合わせます",
		L"音符の速さを950に合わせます",
		L"音符の速さを1000に合わせます",
		L"音符の速さを1100に合わせます",
		L"音符の速さを1200に合わせます",
		L"音符の速さを1300に合わせます",
		L"音符の速さを1400に合わせます",
		L"音符の速さを1500に合わせます",
		L"音符の速さを1600に合わせます" };//説明文

	wchar_t *sent_gauge[GAUGE_NUM] = { L"通常のゲージです"
		,L"減りが早いゲージです"
		,L"減りが非常に早く回復も少ないゲージです"
		,L"MISSを出すと失敗になるゲージです"
		,L"MISS,GOODを出すと失敗になるゲージです"
		,L"最後まで遊べる減らないゲージです(クリアマークは付きません)" };//説明文

	wchar_t *sent_lane[LANE_NUM] = { L"オプションを付けません"
		,L"音符の出現するレーンが左右反転します"
		,L"音符の出現するレーンがランダムに入れ替わります"
		,L"音符の出現する場所がランダムに変わります" };//説明文

	wchar_t *sent_color[COLOR_NUM] = { L"オプションを付けません"
		,L"音符全体のRGB値がランダムに入れ替わります"
		,L"音符それぞれのRGB値がランダムに入れ替わります"
		,L"音符の色がランダムに変わります"
		,L"音符の色が赤,緑,青のみになります(スコアは保存されません)"
		,L"音符の色が水色,紫,黄色のみになります(スコアは保存されません)"
		,L"音符の色が白のみになります(スコアは保存されません)"
		,L"音符の色が虹色のみになります(スコアは別に保存されます)" };//説明文

	wchar_t *sent_note = L"音符を変更します";

	wchar_t *sent_hitsound = L"音符を叩いた時の音を変更します";

	wchar_t* sent_fastSlow[FAST_SLOW_NUM] = { L"タイミングのズレを表示しません"
	,L"タイミングのズレを表示します(FAST:水色,SLOW:赤色)"
	,L"タイミングのズレを表示します(FAST:赤色,SLOW:水色)" };//説明文

	wchar_t* sent_barline[BARLINE_NUM] = { L"小節線を表示します"
		,L"小節線を表示しません"};//説明文

	wchar_t* sent_darkness[DARKNESS_NUM] = { L"演奏画面の背景を通常の明るさにします"
	,L"演奏画面の背景を25%暗くします"
	,L"演奏画面の背景を50%暗くします" 
	,L"演奏画面の背景を75%暗くします" 
	,L"演奏画面の背景を100%暗くします" };//説明文

	wchar_t* sent_black_gradation[BLACK_GRADATION_NUM] =
	{ L"黒終端ロングノートをグラデーション表示にします",
	  L"黒終端ロングノートをグラデーション表示にしません"
	};

	wchar_t* sent_scoretarget[SCORE_GRAPH_NUM] = 
	{ L"スコアグラフを表示しません"
	 ,L"スコアグラフを表示します(表示するグラフをTARGET SCOREで設定)"
	};//説明文

	wchar_t* sent_targetscore1[TARGET_SCORE1_NUM] =
	{ L"ランクEに設定します"
     ,L"ランクDに設定します"
	 ,L"ランクCに設定します"
	 ,L"ランクBに設定します"
	 ,L"ランクAに設定します"
	 ,L"ランクSに設定します"
	 ,L"10000点に設定します"
	 ,L"MAXに設定します"
	};//説明文


	wchar_t* sent_targetscore2[TARGET_SCORE2_NUM] =
	{ L"RIVALスコアに設定します(指定するRIVAL IDはタイトルで設定)"
	 ,L"ランキングの平均スコアに設定します"
	 ,L"ランキングの中央値スコアに設定します"
	 ,L"ランキングで自分の1つ上の順位のスコアに設定します"
	 ,L"ランキング1位のスコアに設定します"
	 ,L"前回プレーしたときのスコアに設定します"
	};//説明文

	wchar_t* sent_sort[SORT_NUM] =
	{ L"曲名で並び替えます"
	 ,L"スコアで並び替えます"
	 ,L"クリア状態で並び替えます"
	 ,L"最小ミス数で並び替えます"
	 ,L"プレイ回数で並び替えます"
     ,L"レーダーの合計値で並び替えます"
	 ,L"レーダーのGLOBAL値で並び替えます"
	 ,L"レーダーのLOCAL値で並び替えます"
	 ,L"レーダーのCHAIN値で並び替えます"
	 ,L"レーダーのUNSTABILITY値で並び替えます"
	 ,L"レーダーのSTREAK値で並び替えます"
     ,L"レーダーのCOLOR値で並び替えます"

	 ,L"赤色音符の密度で並び替えます"
	 ,L"緑色音符の密度で並び替えます"
	 ,L"青色音符の密度で並び替えます"
	 ,L"水色音符の密度で並び替えます"
	 ,L"紫色音符の密度で並び替えます"
	 ,L"黄色音符の密度で並び替えます"
	 ,L"白色音符の密度で並び替えます"
	 ,L"黒色音符の密度で並び替えます"
     ,L"虹色音符の密度で並び替えます"
	 ,L"最大BPMで並び替えます"
	 ,L"最小BPMで並び替えます"
     ,L"譜面が収録されたバージョンで並び替えます"
	};//説明文

	wchar_t* sent_theme = L"テーマスキンを変更します";

	int H_SENT;//画像ハンドル

	//選曲画面で使うオプション名称配列
	const int* ArrayOptionNum[OPTION_NUM] = { &SPEED_NUM,&GAUGE_NUM,&LANE_NUM,&COLOR_NUM,&NOTE_NUM,&HITSOUND_NUM,&FAST_SLOW_NUM,&BARLINE_NUM,&DARKNESS_NUM,&BLACK_GRADATION_NUM,&SCORE_GRAPH_NUM,&TARGET_SCORE1_NUM,&TARGET_SCORE2_NUM,&THEME_NUM,&SORT_NUM };//各オプションの数
	wchar_t** ArrayOptionSent[OPTION_NUM] = { sent_speed,sent_gauge,sent_lane,sent_color,&sent_note,&sent_hitsound,sent_fastSlow,sent_barline,sent_darkness,sent_black_gradation,sent_scoretarget,sent_targetscore1,sent_targetscore2,&sent_theme,sent_sort };//説明文
	wchar_t** ArrayOptionKindName[OPTION_NUM] = { speed,gauge,lane,color,note,hitsound,fastSlow,barline,darkness,black_gradation,scoretarget,targetscore1,targetscore2,theme,sort };//オプション名称
	int* ArrayValue[OPTION_NUM] = { &(op.speed),&(op.gauge),&(op.lane),&(op.color),&(op.note),&(op.hitsound),&(op.fastSlow),&(op.barline),&(op.darkness),&(op.black_gradation),&(op.scoregraph),&(op.targetscore1),&(op.targetscore2),&(op.theme),&(op.sort) };//選んでいるオプションの値


}OPTION;

#endif