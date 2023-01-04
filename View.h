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
	
protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);

	ImageHandle handle;//表示する画像ハンドル情報

	virtual Size viewSize() = 0;//継承先でこのViewのサイズを指定してください
	void makeScreen(Size s);//このViewを描画するスクリーンの用意 継承先のコンストラクタで呼び出してください

public:
	/**
	* 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	*/
	View(ViewContext* vc, Cordinate cordinate);

	void draw(int drawScreen) override;//Viewの描画処理 毎フレーム呼びます 継承先で必ず実装してください
	void setScreen(int drawScreen) override;
	virtual void putContents(int drawScreen) override = 0;
};

