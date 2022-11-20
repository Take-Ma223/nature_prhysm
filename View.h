#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"

/** 
* 座標クラス
*/
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x = 0, int y = 0);
};

/**
* ビュークラスの前提となるコンテキストクラス
* 画面内で共有されているもの
*/
class ViewContext {
public:
	Asset* asset;
	Option* option;
	double* time;

	ViewContext(Asset* asset = NULL, Option* option = NULL, double* time = NULL);
};



/**
* ビュークラス 全てのビューの基底クラス
*/
class View
{
protected:
	ViewContext* vc;
	Cordinate cordinate;

public:
	View(ViewContext* vc, Cordinate cordinate);
	
	virtual void draw() = 0;//Viewの描画処理 毎フレーム呼びます 継承先で必ず実装してください

};

