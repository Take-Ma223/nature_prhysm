#pragma once

/// <summary>
/// 座標クラス
/// </summary>
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x = 0, int y = 0);
};

/// <summary>
/// 中心位置クラス 割合で指定します
/// </summary>
class CenterRatio {
public:
	double x = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)
	double y = 0;//左上を(0,0)としたときの中心座標(このDrawable内の割合)

	CenterRatio(double x = 0, double y = 0);
};

/// <summary>
/// 大きさクラス
/// </summary>
class Size {
public:
	int x = 0;
	int y = 0;

	Size(int x = 1, int y = 1);
};
