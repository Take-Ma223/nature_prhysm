#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"

/**
* ビュークラス 全てのビューの基底クラス
*/
class View : public Drawable
{
private:
	void deleteGraph();

protected:
	void makeScreen(Size s);//このViewを描画するスクリーンの用意 継承先のコンストラクタで呼び出してください s:用意するサイズ 
	virtual void prepareScreen(int drawScreen) = 0;//drawが呼ばれるたびにこのViewのスクリーンを用意する処理 継承先で必ず実装してください

public:
	View() : Drawable() {}
	/**
	* 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	*/
	View(DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) : Drawable(dc, param) {
	}
	View(Drawable* parent, DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) : Drawable(parent, dc, param) {
	}
	~View() {
		deleteGraph();
	}

	void draw(int drawScreen) override;//Viewの描画処理の開始 毎フレーム呼びます 
};

