#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"

/** 
* 座標クラス
*/
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x, int y);
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
class View : Drawable
{
protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);

public:
	/**
	* 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	*/
	View(ViewContext* vc, Cordinate cordinate);
	
	virtual void draw() override = 0;//Viewの描画処理 毎フレーム呼びます 継承先で必ず実装してください

};

