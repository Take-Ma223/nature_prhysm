#pragma once
#include "Asset.h"
#include "STRUCT_OP.h"
#include "Drawable.h"
#include "Geometry.h"
#include <stack>
#include "ActivityContext.h"

/// <summary>
/// ビュークラス 全てのビューの基底クラス
/// </summary>
class View : public Drawable
{
private:
	void deleteGraph();
	std::vector<Drawable*> drawables;

protected:

	/// <summary>
	/// 子Drawableの追加
	/// </summary>
	/// <param name="d"></param>
	void addDrawable(Drawable* d) {
		drawables.push_back(d);
	}

	/// <summary>
	/// 子Drawableの削除
	/// </summary>
	/// <param name="d"></param>
	void removeDrawable() {
		drawables.clear();
	}


	/// <summary>
	/// このViewを描画するスクリーンの用意 継承先のコンストラクタで呼び出してください
	/// </summary>
	/// <param name="s">用意するサイズ </param>
	void makeScreen(Size s);

	/// <summary>
	/// 子Drawableのdrawが呼ばれる直前に行う処理を書きます 継承先で実装してください
	/// </summary>
	/// <param name="drawScreen">描画先</param>
	virtual void beforeDrawProcess(int drawScreen) = 0;

public:
	View() : Drawable() {}

	/// <summary>
	/// 継承先でコンストラクタをオーバーライドする際に、使用するImage,Viewのインスタンスを用意する処理を記述してください
	/// </summary>
	/// <param name="c"></param>
	/// <param name="param"></param>
	View(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : Drawable(c, param) {
	}
	View(Drawable* parent, ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : Drawable(parent, c, param) {
	}
	~View() {
		deleteGraph();
		drawables.clear();
	}

	/// <summary>
	/// Viewの描画処理の開始 毎フレーム呼びます
	/// </summary>
	void draw() override;

	/// <summary>
	/// Viewの描画処理の開始 毎フレーム呼びます 
	/// </summary>
	/// <param name="drawScreen">描画先</param>
	void draw(int drawScreen) override;
};

