#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"

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
private:
	int screenHandle = 0;//このViewの画像ハンドル
	Size size = Size(1, 1);//このViewのサイズ makeScreen実行時に変更されます
	void deleteGraph();

protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);
	
	void makeScreen(Size s);//このViewを描画するスクリーンの用意 継承先のコンストラクタで呼び出してください s:用意するサイズ 

public:
	/**
	* 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	*/
	View(ViewContext* vc, Cordinate cordinate);
	~View();

	void draw(int drawScreen) override;//Viewの描画処理の開始 毎フレーム呼びます 
	void setScreen(int drawScreen) override;//描画先スクリーンの指定
	virtual void putContents(int drawScreen) override = 0;//描画処理 継承先で必ず実装してください
};

