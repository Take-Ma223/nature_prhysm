#pragma once

#include"DxLib.h"
#include"ImageSet.h"
#include "TransValue.h"
#include "TransAction.h"
#include "View.h"
#include "Drawable.h"
#include "ImageHandle.h"
#include "Geometry.h"


class Image: public Drawable {
	ImageHandle handle;//表示する画像ハンドル情報
	double* nowTime;//現在のゲーム時間へのポインタ

public:
	Image(ImageHandle handle = ImageHandle(), double* time = NULL, Cordinate cordinate = Cordinate(0, 0), BOOL visible = 1, int alpha = 255);
	~Image();

	void draw(int drawScreen) override;//毎フレーム呼ぶ
	int drawHandle() override {
		return handle.getHandle();
	};

};

