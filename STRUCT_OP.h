#ifndef _STRUCT_OP
#define _STRUCT_OP

namespace OptionItem {
	enum class Name : int {
		SPEED,
		GAUGE,
		LANE,
		COLOR,
		WINDBREAK,
		FAST_SLOW,
		BARLINE,
		DARKNESS,
		GRADATION,
		NOTE_OFFSET,
		SCORETARGET,
		TARGETSCORE1,
		TARGETSCORE2,
		THEME,
		NOTE,
		HITSOUND,
		SORT
	};

	enum class Gauge : int {
		NORMAL,
		HARD,
		SUPER_HARD,
		FC_ATTACK,
		PFC_ATTACK,
		NO_FAIL,
		SKILL_TEST
	};

	enum class Lane : int {
		NONE,
		MIRROR,
		RANDOM,
		SUPER_RAND
	};

	enum class Color : int {
		NONE,
		RGB_CHANGE,
		RGB_RAND,
		SUPER_RAND,
		RGB_ONLY,
		CMY_ONLY,
		W_ONLY,
		RAINBOW
	};

	enum class Windbreak : int {
		CLOSE_0,
		CLOSE_10,
		CLOSE_20,
		CLOSE_30,
		CLOSE_40,
		CLOSE_50,
		CLOSE_60,
		CLOSE_70,
		CLOSE_80,
		CLOSE_90,
		CLOSE_100
	};

	enum class FastSlow : int {
		OFF,
		ON_FAST_CYAN,
		ON_FAST_RED
	};

	enum class Barline : int {
		ON,
		OFF
	};

	enum class Darkness : int {
		DARKNESS_0,
		DARKNESS_25,
		DARKNESS_50,
		DARKNESS_75,
		DARKNESS_100
	};

	enum class BlackGradation : int {
		ON,
		OFF
	};

	enum class ScoreGraph : int {
		OFF,
		ON
	};

	enum class TargetScore1 : int {
		E,
		D,
		C,
		B,
		A,
		S,
		POINT_10000,
		MAX
	};

	enum class TargetScore2 : int {
		RIVAL,
		AVERAGE,
		MEDIAN,
		NEXTRANK,
		TOP,
		LATEST
	};

	enum class Sort : int {
		NAME,
		LEVEL,
		SCORE,
		CLEAR_STATE,
		MIN_MISS,
		PLAY_COUNT,
		RADAR,
		GLOBAL,
		LOCAL,
		CHAIN,
		UNSTABILITY,
		STREAK,
		COLOR,
		RED_DENSITY,
		GREEN_DENSITY,
		BLUE_DENSITY,
		CYAN_DENSITY,
		MAGENTA_DENSITY,
		YELLOW_DENSITY,
		WHITE_DENSITY,
		BLACK_DENSITY,
		RAINBOW_DENSITY,
		MAX_BPM,
		MIN_BPM,
		MAX_CHORDS,
		VERSION,
	};
}

typedef struct OP {//オプション構造体(ロード時に渡す値)
	int speed = 11;//ハイスピード番号
	double speedVal = 1;//ハイスピ倍率
	OptionItem::Gauge gauge = OptionItem::Gauge::NORMAL;//ゲージタイプ(0:ノーマルゲージ(青) 1:ハードゲージ(緑) 2:スーパーハードゲージ(赤) 3:FCゲージ(銀) 4:PFCゲージ(金))
	OptionItem::Lane lane = OptionItem::Lane::NONE;//レーンオプション
	OptionItem::Color color = OptionItem::Color::NONE;//色オプション
	int note = 0;//ノートスキン
	int hitSound = 0;//ヒット音スキン
	OptionItem::Barline barline = OptionItem::Barline::ON;//小節線表示
	OptionItem::Darkness darkness = OptionItem::Darkness::DARKNESS_0;//演奏画面の背景の暗さ
	OptionItem::ScoreGraph scoreGraph = OptionItem::ScoreGraph::OFF;//スコアグラフ表示するかどうか
	OptionItem::Sort sort = OptionItem::Sort::NAME;//選曲画面のソート種類
	OptionItem::BlackGradation blackGradation = OptionItem::BlackGradation::ON;//黒終端ノーツのグラデーション
	int theme = 0;//テーマスキン
	OptionItem::TargetScore1 targetScore1 = OptionItem::TargetScore1::E;//TARGET SCORE1
	OptionItem::TargetScore2 targetScore2 = OptionItem::TargetScore2::RIVAL;//TARGET SCORE2
	OptionItem::FastSlow fastSlow = OptionItem::FastSlow::ON_FAST_CYAN;//FAST/SLOW
	int noteOffset_DepregatedVer1_33 = 6;//音符オフセット調整(非推奨)
	OptionItem::Windbreak windbreak = OptionItem::Windbreak::CLOSE_0;//レーンカバー初期位置
	int noteOffset = 20;//音符オフセット調整
}OP;

typedef struct Option {//オプション構造体(全体)
	OP op;

	static const int OPTION_NUM = 17;//オプションの数

	wchar_t* OptionName[OPTION_NUM] = { L"SPEED",L"GAUGE",L"LANE",L"COLOR",L"WINDBREAK",L"FAST/SLOW",L"BARLINE",L"NIGHT",L"GRADATION",L"NOTE OFFSET",L"SCORE GRAPH",L"TARGET SCORE1",L"TARGET SCORE2",L"THEME",L"NOTE",L"HIT SOUND",L"SORT"};

	static const int SPEED_NUM = 78;//ハイスピの数
	static const int GAUGE_NUM = 6;//ゲージ種類の数
	static const int LANE_NUM = 4;//レーンオプションの数
	static const int COLOR_NUM = 8;//色オプションの数
	static const int WINDBREAK_NUM = 11;//カバー位置オプションの数

	static const int FAST_SLOW_NUM = 3;//FAST/SLOWオプションの数
	static const int BARLINE_NUM = 2;//小節線オプションの数
	static const int DARKNESS_NUM = 5;//黒背景オプションの数
	static const int BLACK_GRADATION_NUM = 2;//黒終端ロング表示オプションの数
	static const int NOTE_OFFSET_NUM = 41;//音符オフセットオプションの数

	static const int SCORE_GRAPH_NUM = 2;//スコアグラフオプションの数
	static const int TARGET_SCORE1_NUM = 8;//スコアグラフターゲットオプションの数
	static const int TARGET_SCORE2_NUM = 6;//スコアグラフターゲットオプションの数
	int NOTE_NUM = 0;//ノートスキンオプションの数
	int HITSOUND_NUM = 0;//ヒット音オプションの数
	static const int SORT_NUM = 26;//曲ソート種類の数
	int THEME_NUM = 0;//テーマオプションの数

	

	double speedVal[SPEED_NUM] = {
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
	wchar_t* windbreak[WINDBREAK_NUM] = { L"OFF",L"10%",L"20%",L"30%",L"40%",L"50%",L"60%",L"70%",L"80%",L"90%",L"100%" };

	int windbreakVal[WINDBREAK_NUM] = {
		0,
		100,
		200,
		300,
		400,
		500,
		600,
		700,
		800,
		900,
		1000
	};

	wchar_t** note = 0;//名前はフォルダ名から取る パスの役割も果たす
	wchar_t** hitSound = 0;//名前はフォルダ名から取る パスの役割も果たす
	wchar_t* fastSlow[FAST_SLOW_NUM] = { L"OFF",L"ON:C/R",L"ON:R/C" };
	wchar_t* barline[BARLINE_NUM] = { L"ON",L"OFF"};
	wchar_t* darkness[DARKNESS_NUM] = { L"0%",L"25%",L"50%",L"75%",L"100%" };
	wchar_t* noteOffset[NOTE_OFFSET_NUM] = {
		L"-100ms",
		L"-95ms",
		L"-90ms",
		L"-85ms",
		L"-80ms",
		L"-75ms",
		L"-70ms",
		L"-65ms",
		L"-60ms",
		L"-55ms",
		L"-50ms",
		L"-45ms",
		L"-40ms",
		L"-35ms",
		L"-30ms",
		L"-25ms",
		L"-20ms",
		L"-15ms",
		L"-10ms",
		L"-5ms",
		L"0ms",
		L"+5ms",
		L"+10ms",
		L"+15ms",
		L"+20ms",
		L"+25ms",
		L"+30ms",
		L"+35ms",
		L"+40ms",
		L"+45ms",
		L"+50ms",
		L"+55ms",
		L"+60ms",
		L"+65ms",
		L"+70ms",
		L"+75ms",
		L"+80ms",
		L"+85ms",
		L"+90ms",
		L"+95ms",
		L"+100ms"
	};

	double noteOffsetVal[NOTE_OFFSET_NUM] = {
		-100,
		-95,
		-90,
		-85,
		-80,
		-75,
		-70,
		-65,
		-60,
		-55,
		-50,
		-45,
		-40,
		-35,
		-30,
		-25,
		-20,
		-15,
		-10,
		-5,
		0,
		+5,
		+10,
		+15,
		+20,
		+25,
		+30,
		+35,
		+40,
		+45,
		+50,
		+55,
		+60,
		+65,
		+70,
		+75,
		+80,
		+85,
		+90,
		+95,
		+100
	};

	wchar_t* scoreTarget[SCORE_GRAPH_NUM] = { L"OFF", L"ON"};
	wchar_t* targetScore1[TARGET_SCORE1_NUM] = { L"E", L"D", L"C", L"B", L"A", L"S", L"10000", L"MAX"};
	wchar_t* targetScore2[TARGET_SCORE2_NUM] = { L"RIVAL", L"AVERAGE", L"MEDIAN", L"NEXT RANK", L"TOP", L"LATEST"
};

	wchar_t* sort[SORT_NUM] = { L"NAME", L"LEVEL", L"SCORE", L"CLEAR STATE",L"MIN MISS", L"PLAY COUNT",L"RADAR",L"GLOBAL",L"LOCAL",L"CHAIN",L"UNSTABILITY",L"STREAK",L"COLOR",
		L"RED",L"GREEN",L"BLUE",L"CYAN",L"MAGENTA",L"YELLOW",L"WHITE",L"BLACK",L"RAINBOW",
		L"MAX BPM",L"MIN BPM", L"MAX CHORDS", L"VERSION" };
	wchar_t* blackGradation[BLACK_GRADATION_NUM] = { L"ON", L"OFF" };
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

	wchar_t* sent_windbreak[WINDBREAK_NUM] = {
		L"レーンカバーを下げずに演奏開始します"
		,L"レーンカバーを10%下げて演奏開始します"
		,L"レーンカバーを20%下げて演奏開始します"
		,L"レーンカバーを30%下げて演奏開始します"
		,L"レーンカバーを40%下げて演奏開始します"
		,L"レーンカバーを50%下げて演奏開始します"
		,L"レーンカバーを60%下げて演奏開始します"
		,L"レーンカバーを70%下げて演奏開始します"
		,L"レーンカバーを80%下げて演奏開始します"
		,L"レーンカバーを90%下げて演奏開始します"
		,L"レーンカバーを100%下げて演奏開始します"};//説明文

	wchar_t *sent_note = L"音符を変更します";

	wchar_t *sent_hitSound = L"音符を叩いた時の音を変更します";

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

	wchar_t* sent_blackGradation[BLACK_GRADATION_NUM] =
	{ L"黒終端ロングノートをグラデーション表示にします",
	  L"黒終端ロングノートをグラデーション表示にしません"
	};

	wchar_t* sent_noteOffset[NOTE_OFFSET_NUM] =
	{
		L"音符の流れるタイミングを100ミリ秒早くします",
		L"音符の流れるタイミングを95ミリ秒早くします",
		L"音符の流れるタイミングを90ミリ秒早くします",
		L"音符の流れるタイミングを85ミリ秒早くします",
		L"音符の流れるタイミングを80ミリ秒早くします",
		L"音符の流れるタイミングを75ミリ秒早くします",
		L"音符の流れるタイミングを70ミリ秒早くします",
		L"音符の流れるタイミングを65ミリ秒早くします",
		L"音符の流れるタイミングを60ミリ秒早くします",
		L"音符の流れるタイミングを55ミリ秒早くします",
		L"音符の流れるタイミングを50ミリ秒早くします",
		L"音符の流れるタイミングを45ミリ秒早くします",
		L"音符の流れるタイミングを40ミリ秒早くします",
		L"音符の流れるタイミングを35ミリ秒早くします",
		L"音符の流れるタイミングを30ミリ秒早くします",
		L"音符の流れるタイミングを25ミリ秒早くします",
		L"音符の流れるタイミングを20ミリ秒早くします",
		L"音符の流れるタイミングを15ミリ秒早くします",
		L"音符の流れるタイミングを10ミリ秒早くします",
		L"音符の流れるタイミングを5ミリ秒早くします",
		L"音符の流れるタイミングを調節しません",
		L"音符の流れるタイミングを5ミリ秒遅くします",
		L"音符の流れるタイミングを10ミリ秒遅くします",
		L"音符の流れるタイミングを15ミリ秒遅くします",
		L"音符の流れるタイミングを20ミリ秒遅くします",
		L"音符の流れるタイミングを25ミリ秒遅くします",
		L"音符の流れるタイミングを30ミリ秒遅くします",
		L"音符の流れるタイミングを35ミリ秒遅くします",
		L"音符の流れるタイミングを40ミリ秒遅くします",
		L"音符の流れるタイミングを45ミリ秒遅くします",
		L"音符の流れるタイミングを50ミリ秒遅くします",
		L"音符の流れるタイミングを55ミリ秒遅くします",
		L"音符の流れるタイミングを60ミリ秒遅くします",
		L"音符の流れるタイミングを65ミリ秒遅くします",
		L"音符の流れるタイミングを70ミリ秒遅くします",
		L"音符の流れるタイミングを75ミリ秒遅くします",
		L"音符の流れるタイミングを80ミリ秒遅くします",
		L"音符の流れるタイミングを85ミリ秒遅くします",
		L"音符の流れるタイミングを90ミリ秒遅くします",
		L"音符の流れるタイミングを95ミリ秒遅くします",
		L"音符の流れるタイミングを100ミリ秒遅くします"
	};

	wchar_t* sent_scoreTarget[SCORE_GRAPH_NUM] = 
	{ L"スコアグラフを表示しません"
	 ,L"スコアグラフを表示します(表示するグラフをTARGET SCOREで設定)"
	};//説明文

	wchar_t* sent_targetScore1[TARGET_SCORE1_NUM] =
	{ L"ランクEに設定します"
     ,L"ランクDに設定します"
	 ,L"ランクCに設定します"
	 ,L"ランクBに設定します"
	 ,L"ランクAに設定します"
	 ,L"ランクSに設定します"
	 ,L"10000点に設定します"
	 ,L"MAXに設定します"
	};//説明文


	wchar_t* sent_targetScore2[TARGET_SCORE2_NUM] =
	{ L"RIVALスコアに設定します(指定するRIVAL IDはタイトルで設定)"
	 ,L"ランキングの平均スコアに設定します"
	 ,L"ランキングの中央値スコアに設定します"
	 ,L"ランキングで自分の1つ上の順位のスコアに設定します"
	 ,L"ランキング1位のスコアに設定します"
	 ,L"前回プレーしたときのスコアに設定します"
	};//説明文

	wchar_t* sent_sort[SORT_NUM] =
	{ L"曲名で並び替えます"
	 ,L"難易度(降水確率)で並び替えます"
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
	 ,L"最大同時押し数で並び替えます"
     ,L"譜面が収録されたバージョンで並び替えます"
	};//説明文

	wchar_t* sent_theme = L"テーマスキンを変更します";

	int H_SENT;//画像ハンドル

	//選曲画面で使うオプション名称配列
	const int* ArrayOptionNum[OPTION_NUM] = {
		&SPEED_NUM,
		&GAUGE_NUM,
		&LANE_NUM,
		&COLOR_NUM,
		&WINDBREAK_NUM,
		&FAST_SLOW_NUM,
		&BARLINE_NUM,
		&DARKNESS_NUM,
		&BLACK_GRADATION_NUM,
		&NOTE_OFFSET_NUM,
		&SCORE_GRAPH_NUM,
		&TARGET_SCORE1_NUM,
		&TARGET_SCORE2_NUM,
		&THEME_NUM,
		&NOTE_NUM,
		&HITSOUND_NUM,
		&SORT_NUM };//各オプションの数

	wchar_t** ArrayOptionSent[OPTION_NUM] = {
		sent_speed,
		sent_gauge,
		sent_lane,
		sent_color,
		sent_windbreak,
		sent_fastSlow,
		sent_barline,
		sent_darkness,
		sent_blackGradation,
		sent_noteOffset,
		sent_scoreTarget,
		sent_targetScore1,
		sent_targetScore2,
		&sent_theme,
		&sent_note,
		&sent_hitSound,
		sent_sort };//説明文

	wchar_t** ArrayOptionKindName[OPTION_NUM] = { 
		speed,
		gauge,
		lane,
		color,
		windbreak,
		fastSlow,
		barline,
		darkness,
		blackGradation,
		noteOffset,
		scoreTarget,
		targetScore1,
		targetScore2,
		theme,
		note,
		hitSound,
		sort };//オプション名称

	int* ArrayValue[OPTION_NUM] = {
		&(op.speed),
		(int*)&(op.gauge),
		(int*)&(op.lane),
		(int*)&(op.color),
		(int*)&(op.windbreak),
		(int*)&(op.fastSlow),
		(int*)&(op.barline),
		(int*)&(op.darkness),
		(int*)&(op.blackGradation),
		&(op.noteOffset),
		(int*)&(op.scoreGraph),
		(int*)&(op.targetScore1),
		(int*)&(op.targetScore2),
		&(op.theme),
		&(op.note),
		&(op.hitSound),
		(int*)&(op.sort) };//選んでいるオプションの値

	
 }Option;

#endif