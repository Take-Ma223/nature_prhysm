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
class View : public Drawable
{
private:
	void deleteGraph();

protected:
	ViewContext* vc;
	Cordinate cordinate = Cordinate(0,0);
	
	void makeScreen(Size s);//このViewを描画するスクリーンの用意 継承先のコンストラクタで呼び出してください s:用意するサイズ 
	virtual void prepareScreen(int drawScreen) = 0;//drawが呼ばれるたびにこのViewのスクリーンを用意する処理 継承先で必ず実装してください

public:
	/**
	* 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	*/
	View(ViewContext* vc, DrawableInitParam param = DrawableInitParam()) : Drawable(vc->time, param) {
		View::vc = vc;
		View::cordinate = cordinate;
	}
	~View() {
		deleteGraph();
	}

	void draw(int drawScreen) override;//Viewの描画処理の開始 毎フレーム呼びます 
};

