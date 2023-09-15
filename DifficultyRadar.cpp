#include "DifficultyRadar.h"
#include<math.h>

DifficultyRadar::DifficultyRadar(NOTE** note, int* nc, BPMC* bpmchange, STOP_SE* stopSequence, SC* scrollchange, int time, int StartTime, int EndTime, int* TimingSame, short* NotesAmount, double BPM_suggest) {
	DifficultyRadar::note = note;
	DifficultyRadar::nc = nc;
	DifficultyRadar::bpmchange = bpmchange;
	DifficultyRadar::stopSequence = stopSequence;
	DifficultyRadar::scrollchange = scrollchange;
	DifficultyRadar::time = time;
	DifficultyRadar::StartTime = StartTime;
	DifficultyRadar::EndTime = EndTime;
	DifficultyRadar::TimingSame = TimingSame;
	DifficultyRadar::BPM_suggest = BPM_suggest;

	int i = 0;
	for (i = 0; i < 9; i++) {//各色音符数の初期化
		NotesAmount[i] = 0;
	}

	int lane = 0, NoteCounter = 0;
	//音符の合計を取得
	for (lane = 0; lane <= 3; lane++) {
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].group == NoteGroup::Single || note[lane][NoteCounter].group == NoteGroup::LongNoteStart) {
				if (note[lane][NoteCounter].color != NoteColor::K) {

					if (note[lane][NoteCounter].color == NoteColor::R ||
						note[lane][NoteCounter].color == NoteColor::G ||
						note[lane][NoteCounter].color == NoteColor::B) {//RGB
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::Y ||
						note[lane][NoteCounter].color == NoteColor::C ||
						note[lane][NoteCounter].color == NoteColor::M) {//CMY
						TotalNotes += 2;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::W) {//W
						TotalNotes += 3;
						TotalNotesRainbow += 1;
					}
					else if (note[lane][NoteCounter].color == NoteColor::F) {//F
						TotalNotes += 1;
						TotalNotesRainbow += 1;
					}
				}
				TotalNotesK++;
				NotesAmount[NumberTranslation(note[lane][NoteCounter].color)]++;//各色がいくつあるか加算
			}
			if (note[lane][NoteCounter].group == 2 && note[lane][NoteCounter].LN_k == 1) {//LN終端の黒
				NotesAmount[NumberTranslation(NoteColor::K)]++;//各色がいくつあるか加算
			}
		}
	}

	for (i = 0; i < 9; i++) {//各色音符数を平均密度にする
		NotesAmount[i] = int(200 * log10((double)NotesAmount[i] * 30000 / (DifficultyRadar::time)+1));
	}

}

void DifficultyRadar::GetLocalNotesGraph(short* LocalNotesGraph) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//局所色難易度
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing sameのカウンター
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//その区間の最初の音符のタイミング
	int et = 0;//その区間の最後の音符のタイミング


	//BoxTime秒の枠をずらしながら枠に入る音符の密度の最高値をLocal値とする
	int BoxTime = int((double)time / 9 + 0.5);//枠の時間(ms)
	int SlideTime = BoxTime;//枠をスライドする時間
	int Count[4] = { 0,0,0,0 };//枠の中に入っている音符の数
	int SlideCount = 0;//スライド回数カウント

	int CountToSlide = int((double)time / SlideTime + 1);//スライドする回数

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };

	do {
		//4レーンそれぞれの枠内音符数を計算
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//新しく枠内に入ってきた音符数を足す
				if (nc_last[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != NoteColor::K) {//枠終わりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_last[ind]].color == NoteColor::R ||
						note[ind][nc_last[ind]].color == NoteColor::G ||
						note[ind][nc_last[ind]].color == NoteColor::B) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::Y ||
						note[ind][nc_last[ind]].color == NoteColor::C ||
						note[ind][nc_last[ind]].color == NoteColor::M) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::W) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::F) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//枠内から出ていった音符数を引く
				if (nc_start[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//枠内から出ていった音符数を引く
					&& note[ind][nc_start[ind]].color != NoteColor::K) {//枠始まりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_start[ind]].color == NoteColor::R ||
						note[ind][nc_start[ind]].color == NoteColor::G ||
						note[ind][nc_start[ind]].color == NoteColor::B) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::Y ||
						note[ind][nc_start[ind]].color == NoteColor::C ||
						note[ind][nc_start[ind]].color == NoteColor::M) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::W) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::F) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		if (SlideCount <= 8) {
			LocalNotesGraph[SlideCount] = int(25000 * (double)NoteCount / BoxTime);//音符密度グラフを作成
		}

		SlideCount++;
	} while (SlideCount < CountToSlide);

	return;
}

int DifficultyRadar::NumberTranslation(NoteColor color) {
	int val = (int)color;

	switch (val)
	{
	default:
		break;
	case (int)NoteColor::Y:
		val = 6;//y
		break;
	case (int)NoteColor::C:
		val = 4;//y
		break;
	case (int)NoteColor::M:
		val = 5;//y
		break;
	}

	val--;//0~8にする

	return val;
}


int DifficultyRadar::CalcGlobal(int Rainbow) {
	double GlobalScore = 0;

	if (Rainbow == 0) {
		GlobalScore = TotalNotes;
	}
	else if (Rainbow == 1) {
		GlobalScore = TotalNotesRainbow;
	}

	double LN_point = 545.454545454545;//LNの1pointあたりの秒数(ms)
	double color_point[11] = { 0,1,1,1,2,2,2,3,0,0.9,0 };//音符の色毎の難易度ポイント
	double cp_CAP = 1.333;//光る音符の難易度ポイント倍率
	int time_use = time;
	int time_limit = 12000;//これ以下の時間で密度を出さない


	if (time_use <= time_limit)time_use = time_limit;

	//1分あたりの総ノート数を算出
	GlobalScore = (GlobalScore / ((double)time_use / 1000)) * 60;

	return (int)(GlobalScore * 100 / globalMax);
}

int DifficultyRadar::CalcLocal(int Rainbow) {
	double LocalScore = 0;
	double LocalScoreBuf = 0;
	double NoteCount = 0;
	double LocalColor = 0;//局所色難易度
	int timing = 0;
	int lane = 0;
	int tc = 0;//timing sameのカウンター
	int nc_last[4] = { 0,0,0,0 };
	int nc_start[4] = { 0,0,0,0 };
	int LocalTime = 0;
	int ind = 0;
	int ind_s = 0;
	int st = 0;//その区間の最初の音符のタイミング
	int et = 0;//その区間の最後の音符のタイミング


	//BoxTime秒の枠をずらしながら枠に入る音符の密度の最高値をLocal値とする
	int BoxTime = 6000;//枠の時間(ms)
	int SlideTime = 100;//枠をスライドする時間
	int Count[4] = { 0,0,0,0 };//枠の中に入っている音符の数
	int SlideCount = 0;//スライド回数カウント

	int CountToSlide = int((double)time / SlideTime + 1);//スライドする回数

	const int RGB = 0;
	const int CMY = 1;
	const int W = 2;
	const int F = 3;

	int NoteCountVal[4] = { 1,2,3,1 };
	if (Rainbow == 1) {
		NoteCountVal[RGB] = 1;
		NoteCountVal[CMY] = 1;
		NoteCountVal[W] = 1;
		NoteCountVal[F] = 1;
	}

	do {
		//4レーンそれぞれの枠内音符数を計算
		for (ind = 0; ind <= 3; ind++) {
			while (note[ind][nc_last[ind]].timing_real <= BoxTime + SlideTime * SlideCount + StartTime) {//新しく枠内に入ってきた音符数を足す
				if (nc_last[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
					&& note[ind][nc_last[ind]].color != NoteColor::K) {//枠終わりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_last[ind]].color == NoteColor::R ||
						note[ind][nc_last[ind]].color == NoteColor::G ||
						note[ind][nc_last[ind]].color == NoteColor::B) {//RGB
						NoteCount += NoteCountVal[RGB];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::Y ||
						note[ind][nc_last[ind]].color == NoteColor::C ||
						note[ind][nc_last[ind]].color == NoteColor::M) {//CMY
						NoteCount += NoteCountVal[CMY];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::W) {//W
						NoteCount += NoteCountVal[W];
					}
					else if (note[ind][nc_last[ind]].color == NoteColor::F) {//F
						NoteCount += NoteCountVal[F];
					}

				}

				nc_last[ind]++;
			}

			while (note[ind][nc_start[ind]].timing_real <= SlideTime * SlideCount + StartTime) {//枠内から出ていった音符数を引く
				if (nc_start[ind] > nc[ind]) {//そのレーンの最大値を超えていたら終了
					break;
				}

				if ((note[ind][nc_start[ind]].group == 0 || note[ind][nc_start[ind]].group == 1)//枠内から出ていった音符数を引く
					&& note[ind][nc_start[ind]].color != NoteColor::K) {//枠始まりから見て一番下にある単,LN始点ノートでKじゃない

					if (note[ind][nc_start[ind]].color == NoteColor::R ||
						note[ind][nc_start[ind]].color == NoteColor::G ||
						note[ind][nc_start[ind]].color == NoteColor::B) {//RGB
						NoteCount -= NoteCountVal[RGB];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::Y ||
						note[ind][nc_start[ind]].color == NoteColor::C ||
						note[ind][nc_start[ind]].color == NoteColor::M) {//CMY
						NoteCount -= NoteCountVal[CMY];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::W) {//W
						NoteCount -= NoteCountVal[W];
					}
					else if (note[ind][nc_start[ind]].color == NoteColor::F) {//F
						NoteCount -= NoteCountVal[F];
					}
				}

				nc_start[ind]++;
			}
		}

		//LocalColor = CalcColor(SlideTime * SlideCount, BoxTime + SlideTime * SlideCount);//局所Colorの算出

		//Localを算出
		LocalTime = BoxTime;
		if (LocalTime != 0) {
			LocalScoreBuf = ((NoteCount + LocalColor * 0) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}
		SlideCount++;
	} while (SlideCount < CountToSlide);


	/*
	while (NoteCount <= (double)TotalNotes / 10) {//まず局所相当の音符数をK以外で何個か探索(1000ノーツなら100個)
		ind = index(note, nc_last, nc);
		if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
			&& note[ind][nc_last[ind]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない

			if (NoteCount == 0)st = note[ind][nc_last[ind]].timing_real;//最初の音符のタイミングを格納
			et = note[ind][nc_last[ind]].timing_real;//最後の音符のタイミングを格納
			NoteCount++;
		}
		nc_last[ind]++;//下から順番に探索
	}

	//暫定局所密度計算
	LocalTime = et - st;
	if (LocalTime != 0) {
		LocalScore = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
	}

	int find = 0;

	while (NoteCount <= TotalNotes) {//全ノーツ探索したら終了

		find = 0;
		while (find == 0) {//探索の上側　k以外の単,LN始点を見つけるまで探索
			ind = index(note, nc_last, nc);
			if ((note[ind][nc_last[ind]].group == 0 || note[ind][nc_last[ind]].group == 1)
				&& note[ind][nc_last[ind]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない
				et = note[ind][nc_last[ind]].timing_real;//最後の音符のタイミングを格納
				NoteCount++;
				find = 1;
			}
			if (nc_last[ind] <= nc[ind]) {//最大値を超えないようにする
				nc_last[ind]++;//下から順番に探索
			}
		}

		find = 0;
		while (find == 0) {//探索の下側　k以外の単,LN始点を見つけるまで探索
			ind_s = index(note, nc_start, nc);
			if ((note[ind_s][nc_start[ind_s]].group == 0 || note[ind_s][nc_start[ind_s]].group == 1)
				&& note[ind_s][nc_start[ind_s]].color != 8) {//ncから見て一番下にある単,LN始点ノートでKじゃない
				st = note[ind_s][nc_start[ind_s]].timing_real;//最後の音符のタイミングを格納
				find = 1;
			}
			if (nc_start[ind_s] <= nc[ind_s]) {//最大値を超えないようにする
				nc_start[ind_s]++;//下から順番に探索
			}
		}

		LocalTime = et - st;
		if (LocalTime != 0) {
			LocalScoreBuf = (((double)TotalNotes / 10) / ((double)LocalTime / 1000)) * 60;
		}

		if (LocalScore < LocalScoreBuf) {
			LocalScore = LocalScoreBuf;
		}

	}
	*/

	return (int)(LocalScore * 100 / localMax);
}

int DifficultyRadar::index(NOTE** note, int* nc, int* ncMax) {//探索を終えているレーン(nc>ncMAX)については無視する
	int index = 0;
	int timing = 0;

	if (nc[0] <= ncMax[0]) {
		index = 0;
		timing = note[0][nc[0]].timing;
	}
	else if (nc[1] <= ncMax[1]) {
		index = 1;
		timing = note[1][nc[1]].timing;
	}
	else if (nc[2] <= ncMax[2]) {
		index = 2;
		timing = note[2][nc[2]].timing;
	}
	else if (nc[3] <= ncMax[3]) {
		index = 3;
		timing = note[3][nc[3]].timing;
	}


	for (int i = 1; i <= 3; i++) {
		if (timing > note[i][nc[i]].timing && nc[i] <= ncMax[i]) {
			index = i;
			timing = note[i][nc[i]].timing;
		}
	}
	return index;
}

int DifficultyRadar::CalcColor(int StartTime, int EndTime, int Rainbow) {//色難易度
	double ColorChangeCount = 0;
	int lane = 0, NoteCounter = 0;
	NoteColor ColorBuf[4] = { NoteColor::NONE,NoteColor::NONE,NoteColor::NONE,NoteColor::NONE };
	int LocalTime = EndTime - StartTime;

	//前の音符の色保存用バッファ
	class colorRingBuf {
	private:
		const int BUF_NUMBER = 6;
		NoteColor colorBuf[4][6];
		int index[4] = { 0,0,0,0 };

		void indexInc(int* index) {
			(*index)++;
			if (*index >= BUF_NUMBER)*index = 0;
		}
		void indexDec(int* index) {
			(*index)--;
			if (*index < 0)*index = BUF_NUMBER - 1;
		}

		double forgettingWeight(int distance) {//色忘却重み
			switch (distance)
			{
			case 0:
				return 0;
				break;
			case 1:
				return 0.5;
				break;
			case 2:
				return 0.75;
				break;
			case 3:
				return 0.875;
				break;
			case 4:
				return 0.9375;
				break;
			case 5:
				return 1;
				break;
			default:
				return 1;
				break;
			}
		}

	public:
		colorRingBuf() {//バッファ初期化
			for (int lane = 0; lane < 4; lane++) {
				for (int ind = 0; ind < BUF_NUMBER; ind++) {
					colorBuf[lane][ind] = NoteColor::NONE;
				}
			}
		}

		void setThisLaneColor(int lane, NoteColor color) {
			colorBuf[lane][index[lane]] = color;
			indexInc(&index[lane]);
		}

		double getThisLaneColorForgettingWeight(int lane, NoteColor color) {//バッファに同じ色が無いか探索し色忘却重みを返す
			int searchIndex = index[lane];//探し始めるインデックス位置
			indexDec(&searchIndex);//indexは挿入する位置なので2つ前の値にする
			indexDec(&searchIndex);

			int distance = 0;
			for (distance = 0; distance < BUF_NUMBER - 1; distance++) {
				if (colorBuf[lane][searchIndex] == color) {
					return forgettingWeight(distance);
				}
				indexDec(&searchIndex);
			}
			return forgettingWeight(distance);//見つからなかった場合はデフォルト値を返す
		}

		double getThisRowColorDifferenceWeight(int lane, NoteColor color) {//指定した音符の横の色重みを返す(直近の横バッファとどれだけ色が違うか)
			int count = 0;
			double weight[4] = {0,0.5,0.75,0.875};
			
			for (int i = 0; i < 4; i++) {
				if (i == lane)continue;

				int ind = index[lane];
				indexDec(&ind);//indexは挿入する位置なので1つ前の値にする

				if (colorBuf[i][ind] == color) {
					count += 1;
				}
			}

			return 1.0 - weight[count];
		}
	}colorRingBuf;

	ColorBuf[0] = note[0][0].color;
	ColorBuf[1] = note[1][0].color;
	ColorBuf[2] = note[2][0].color;
	ColorBuf[3] = note[3][0].color;
	if (Rainbow == 1) {
		for (int i = 0; i <= 3; i++) {
			if (ColorBuf[i] >= NoteColor::R && ColorBuf[i] <= NoteColor::W) {//黒虹以外の音符は虹として扱う
				ColorBuf[i] = NoteColor::F;
			}
		}
	}
	for (int i = 0; i <= 3; i++) {colorRingBuf.setThisLaneColor(i, ColorBuf[i]);}

	NoteColor NoteColor = NoteColor::NONE;//今見ている音符の色
	int k_flag = 0;//前の音符が黒だった

	serachNotesBFS([&](int lane, int index) {
		if ((note[lane][index].group == 0 || note[lane][index].group == 1)) {//単ノートかLN始点
			NoteColor = note[lane][index].color;
			if (Rainbow == 1) {
				if (NoteColor >= NoteColor::R && NoteColor <= NoteColor::W) {//虹モードは黒虹以外の音符は虹として扱う
					NoteColor = NoteColor::F;
				}
			}

			if (ColorBuf[lane] != NoteColor) {//前の音符と色が違う
				if (NoteColor != NoteColor::K) {//k以外でバッファに色保存
					colorRingBuf.setThisLaneColor(lane, NoteColor);
				}
				if (NoteColor == NoteColor::Y ||
					NoteColor == NoteColor::C ||
					NoteColor == NoteColor::M) {//cmyなら4倍の重み
					ColorChangeCount += 4 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == NoteColor::W) {//wなら2倍の重み
					ColorChangeCount += 2 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == NoteColor::K && k_flag == 0) {//前がk以外で今回kなら0.5倍の重み
					ColorChangeCount += 0.5;
					k_flag = 1;
				}
				else if (NoteColor == NoteColor::F) {//fなら1倍の重み
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					ColorBuf[lane] = NoteColor;
					k_flag = 0;
				}
				else if (NoteColor == NoteColor::R ||
					NoteColor == NoteColor::G ||
					NoteColor == NoteColor::B) {//RGBの場合は1倍の重み
					ColorBuf[lane] = NoteColor;
					ColorChangeCount += 1 * colorRingBuf.getThisLaneColorForgettingWeight(lane, NoteColor) * colorRingBuf.getThisRowColorDifferenceWeight(lane, NoteColor);;
					k_flag = 0;
				}
			}
		}
		else if (note[lane][index].group == 2 && note[lane][index].LN_k == 1) {//黒終点の時
			ColorChangeCount += 0.5;
			k_flag = 1;
		}

	});

	ColorChangeCount = (ColorChangeCount / ((double)LocalTime / 1000)) * 60;
	ColorChangeCount *= 1.725;//大きさ調整
	//ColorChangeCount = ColorChangeCount / TotalNotesK;//譜面単位でどれだけ色が複雑か

	return (int)(ColorChangeCount * 100 / colorMax);
}
int DifficultyRadar::CalcLongNote(int Rainbow) {
	int lane = 0;
	double LN[4] = { 0,0,0,0 };//各レーンのLN度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double LNBuf = 0;

	int weight = 1;//音符の色で重み付け (RGB:1 CMY:2 W:3)
	int WeightOfColor[3] = { 1,2,3 };//それぞれの音符グループの重み
	const int RGBF = 0;
	const int CMY = 1;
	const int W = 2;
	if (Rainbow == 1) {
		WeightOfColor[RGBF] = 1;
		WeightOfColor[CMY] = 1;
		WeightOfColor[W] = 1;
	}

	int noteIndex[4] = { 0,0,0,0 };
	int minTiming = 0;
	double lnDegrees = 0;
	class lnCount {
	private:
		int lnCountEachLane[4] = { 0,0,0,0 };
	public:
		int getLnCountWithoutThisLane(int lane) {
			int lnCount = 0;
			for (int i = 0; i < 4; i++) {
				if (lane != i)lnCount += lnCountEachLane[i];
			}
			return lnCount;
		}
		void setLnCount(int lane, int count) {
			lnCountEachLane[lane] = count;
		}
		void deleteLnCount(int lane) {
			lnCountEachLane[lane] = 0;
		}
		int getLnCountSum() {
			int sum = 0;
			for (int i = 0; i < 4; i++) {
				sum += lnCountEachLane[i];
			}
			return sum;
		}
	}lnCount;

	auto color2weight = [](NoteColor colorNumber) {
		switch (colorNumber)
		{
		case NoteColor::NONE:
			return 0;
		case NoteColor::R:
			return 1;
			break;
		case NoteColor::G:
			return 1;
			break;
		case NoteColor::B:
			return 1;
			break;
		case NoteColor::Y:
			return 2;
			break;
		case NoteColor::C:
			return 2;
			break;
		case NoteColor::M:
			return 2;
			break;
		case NoteColor::W:
			return 3;
			break;
		case NoteColor::K:
			return 0;
			break;
		case NoteColor::F:
			return 1;
			break;
		}
	};

	while (1) {
		//全ての音符を見たら終了
		if ((noteIndex[0] == nc[0]) &&
			(noteIndex[1] == nc[1]) &&
			(noteIndex[2] == nc[2]) &&
			(noteIndex[3] == nc[3])) {
			break;
		}

		//探索終わったレーン以外で音符最小タイミング探索
		int first = 1;
		for (lane = 0; lane < 4; lane++) {
			if (noteIndex[lane] < nc[lane]) {
				if (first) {
					minTiming = note[lane][noteIndex[lane]].timing;
					first = 0;
				}
				else if (minTiming > note[lane][noteIndex[lane]].timing)minTiming = note[lane][noteIndex[lane]].timing;
			}
		}

		//音符最小タイミングと同じノートを探索
		BOOL existNote[4] = { 0,0,0,0 };
		for (lane = 0; lane < 4; lane++) {
			if (note[lane][noteIndex[lane]].timing == minTiming)existNote[lane] = 1;
		}

		//レーン毎のLN数(色重み付き)を加算
		int lnFindCount = 0;//今回加算した分
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (note[lane][noteIndex[lane]].group == 1) {
					NoteColor color = note[lane][noteIndex[lane]].color;
					lnFindCount += color2weight(color);
					lnCount.setLnCount(lane, color2weight(color));
				}
			}
		}

		//LN数(色重み付き)をLN度に加算
		lnDegrees += lnFindCount;

		//各レーンの音符の指拘束押し度合いをLN度に加算
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				NoteColor color;

				if (note[lane][noteIndex[lane]].LN_k) {//黒終端なら1 (赤扱い)
					color = NoteColor::R;
				}
				else if (note[lane][noteIndex[lane]].group != 2) {
					color = note[lane][noteIndex[lane]].color;
				}
				else {//通常の終点は除外
					continue;
				}

				lnDegrees += sqrt(color2weight(color) * lnCount.getLnCountWithoutThisLane(lane));
			}
		}

		//レーン毎のLN数(色重み付き)を引く
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (note[lane][noteIndex[lane]].group == 2) {
					lnCount.deleteLnCount(lane);
				}
			}
		}

		//音符の存在するレーンのindexをインクリメント
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (noteIndex[lane] < nc[lane]) {
					noteIndex[lane]++;
				}
			}
		}
	}


	lnDegrees = (lnDegrees / ((double)time / 1000)) * 60;//1分あたりのLN密度にする
	lnDegrees *= 0.005;
	lnDegrees = (log(lnDegrees + 1) / log(2));//
	lnDegrees *= 2;
	lnDegrees *= 130;

	lnDegrees = lnDegrees * 0.315;//大きさ調整
	return lnDegrees;
}
int DifficultyRadar::CalcUnstability() {
	//BPMについて算出
	int BpmCount = 0;
	double Unstability = 0;

	BpmCount = 0;
	double BpmBuf = bpmchange[BpmCount].bpm;
	double BPMChangeSum = 0;//BPM変化量

	int scrollCount = 0;
	double scrollBuf = scrollchange[scrollCount].scroll;
	double scrollChangeSum = 0;//Scroll変化量

	/*
	while (bpmchange[BpmCount].use == 1) {
		if (BpmBuf != bpmchange[BpmCount].bpm) {//BPM変化があった
			BPMChangeSum += fabs((log(bpmchange[BpmCount].bpm + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2))) * 2;//BPM変化の重みは2倍
			//2の対数をとりBPMを比較 倍の関係になっていたら1加算
			BpmBuf = bpmchange[BpmCount].bpm;
		}
		BpmCount++;
	}
	BPMChangeSum = 0;//不使用
	*/


	//ScrollChangeについて算出
	while (scrollchange[scrollCount].use == 1) {
		if (scrollBuf != scrollchange[scrollCount].scroll) {//Scroll変化があった
			scrollChangeSum += fabs((log(abs(scrollchange[scrollCount].scroll) + 0.1) / log(2)) - (log(abs(scrollBuf) + 0.1) / log(2))) * 2;//Scroll変化の重みは2倍
			//2の対数をとりScrollを比較 倍の関係になっていたら1加算
			scrollBuf = scrollchange[scrollCount].scroll;
		}
		scrollCount++;
	}
	Unstability += scrollChangeSum;


	//Stopについて算出
	int StopCount = 0;
	double stopSum = 0;

	while (stopSequence[StopCount].use == 1) {
		stopSum += fabs(stopSequence[StopCount].stop_time);//停止時間を加算
		StopCount++;
	}

	Unstability += stopSum * 0.5 + (double)StopCount * 4;//BPM,HSとの重みバランス調節


	//HS,瞬間風速から離れた速さの音符について算出
	int lane = 0;
	double HS[4] = { 0,0,0,0 };//各レーンのHS度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	BpmBuf = 0;

	double OutlierAmount = 0;//瞬間風速から外れた音符の度合い

	//レーン毎の音符速度変化を見る
	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if (note[lane][NoteCounter].color != NoteColor::K) {//黒は除外
				//HS,BPM変化について
				if (firstFlag == 0) {
					BpmBuf = note[lane][NoteCounter].bpm_real;
					firstFlag = 1;
				}
				else if (BpmBuf != note[lane][NoteCounter].bpm_real) {//BPM変化(HS変化)があった
					HS[lane] += fabs((log(note[lane][NoteCounter].bpm_real + 0.1) / log(2)) - (log(BpmBuf + 0.1) / log(2)));
					//2の対数をとりBPMを比較 倍の関係になっていたら1加算
					BpmBuf = note[lane][NoteCounter].bpm_real;
				}

				//瞬間風速から外れた音符の度合いについて
				OutlierAmount += abs(log((note[lane][NoteCounter].bpm_real + 0.1) / (BPM_suggest + 0.1)) / log(2));//瞬間風速から外れた速さの音符に重みを付けて加算
			}
		}
	}

	double HS_sum = (HS[0] + HS[1] + HS[2] + HS[3]);//BPM変化度

	//値の大きさを調整
	OutlierAmount = OutlierAmount * 16 / ((double)time / 1000);
	//一秒あたりにどれだけ重み付け瞬間風速外れ音符があるかを計算
	//OutlierAmount = OutlierAmount / ((double)time / 1000);

	Unstability += OutlierAmount;//重み付け瞬間風速外れ音符密度を足す

	Unstability += HS_sum;//BPM変化と釣り合いをとるため4で割る

	Unstability *= (7.0 / unstabilityMax);//0~7ぐらいに収める

	Unstability = (log(Unstability + 1) / log(2));//0~7が0~3に対応する対数 変化数が少ないうちは値が多く上がるようにする

	Unstability *= 51.666 * 0.7;

	//Unstability = (Unstability / ((double)time / 1000)) * 60;//1分あたりの対数BPM変化密度にする
	return (int)(Unstability);
}
int DifficultyRadar::CalcChain() {
	int time_use = time;
	int time_limit = 12000;//これ以下の時間で密度を出さない
	if (time_use <= time_limit)time_use = time_limit;

	int lane = 0;
	double chain[4] = { 0,0,0,0 };//各レーンの縦連度
	int NoteCounter = 0;
	int TimingBuf = 0;

	int firstFlag = 0;
	double chainBuf = 0;

	int BaseBPM = 150;//16分を縦連とみなす基準BPM

	double normalizer = 1.0 / (1000.0 * ((60.0 / BaseBPM) / 4.0));//基準BPMでの16分音符の時間間隔


	for (lane = 0; lane <= 3; lane++) {
		firstFlag = 0;
		for (NoteCounter = 0; NoteCounter <= nc[lane] - 1; NoteCounter++) {
			if ((note[lane][NoteCounter].group == 0 || note[lane][NoteCounter].group == 1) && note[lane][NoteCounter].color != NoteColor::K) {
				if (firstFlag == 0) {
					TimingBuf = note[lane][NoteCounter].timing_real;
					firstFlag = 1;
				}
				else {
					chainBuf = (double)1 / pow((((double)note[lane][NoteCounter].timing_real - TimingBuf) * normalizer), 8);//BaseBPMの16分より短いと1からどんどん上がっていく
					if (chainBuf >= 1)chainBuf = 1;//1以上にはしない
					chain[lane] += chainBuf;//
					TimingBuf = note[lane][NoteCounter].timing_real;
				}
			}
		}
	}

	double chainSum = chain[0] + chain[1] + chain[2] + chain[3];
	chainSum = (chainSum / ((double)time_use / 1000)) * 60;//1分あたりの縦連密度にする

	chainSum *= 0.07992;//316を12に収める
	chainSum = (log(chainSum + 1) / log(2));//12が3.5に圧縮される

	chainSum *= 49.723;//大きさ調整

	return (int)(chainSum * 100 / chainMax);
}

void DifficultyRadar::serachNotesBFS(function<void(int lane, int index)> handler)
{
	int noteIndex[4] = { 0,0,0,0 };
	int lane = 0;
	int minTiming = 0;

	while (1) {
		//全ての音符を見たら終了
		if ((noteIndex[0] == nc[0]) &&
			(noteIndex[1] == nc[1]) &&
			(noteIndex[2] == nc[2]) &&
			(noteIndex[3] == nc[3])) {
			break;
		}

		//探索終わったレーン以外で音符最小タイミング探索
		int first = 1;
		for (lane = 0; lane < 4; lane++) {
			if (noteIndex[lane] < nc[lane]) {
				if (first) {
					minTiming = note[lane][noteIndex[lane]].timing;
					first = 0;
				}
				else if (minTiming > note[lane][noteIndex[lane]].timing)minTiming = note[lane][noteIndex[lane]].timing;
			}
		}

		//音符最小タイミングと同じノートを探索
		BOOL existNote[4] = { 0,0,0,0 };
		for (lane = 0; lane < 4; lane++) {
			if (note[lane][noteIndex[lane]].timing == minTiming) {
				existNote[lane] = 1;
				handler(lane, noteIndex[lane]);
			}
		}

		

		//音符の存在するレーンのindexをインクリメント
		for (lane = 0; lane < 4; lane++) {
			if (existNote[lane]) {
				if (noteIndex[lane] < nc[lane]) {
					noteIndex[lane]++;
				}
			}
		}
	}
}
