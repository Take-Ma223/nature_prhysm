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

public:
	Image::Image(ImageHandle handle = ImageHandle(), double* time = NULL, DrawableInitParam param = DrawableInitParam()) : Drawable(time, param) {
		Image::handle = handle;
	}

	Image::~Image() {
		//DeleteGraph(handle.getHandle());
	}

	void draw(int drawScreen) override;//毎フレーム呼ぶ
	int drawHandle() override {
		return handle.getHandle();
	};

};

