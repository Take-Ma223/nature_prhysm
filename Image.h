#pragma once

#include"DxLib.h"
#include"ImageSet.h"
#include "TransValue.h"
#include "TransAction.h"
#include "View.h"
#include "Drawable.h"
#include "ImageHandle.h"
#include "Geometry.h"

/// <summary>
/// ‰æ‘œƒNƒ‰ƒX
/// </summary>
class Image: public Drawable {

public:
	Image::Image() : Drawable() {}

	Image::Image(ActivityContext* c, ImageHandle handle = ImageHandle(), DrawableInitParam param = DrawableInitParam()) : Drawable(c, param) {
		Drawable::handle = handle;
	}

	Image::Image(Drawable* parent, ActivityContext* c, ImageHandle handle = ImageHandle(), DrawableInitParam param = DrawableInitParam()) : Drawable(parent, c, param) {
		Drawable::handle = handle;
	}

	Image::~Image() {
		//DeleteGraph(handle.getHandle());
	}

	void draw() override;
	void draw(int drawScreen) override;
};

