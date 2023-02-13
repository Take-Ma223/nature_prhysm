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

public:
	Image::Image() : Drawable() {}

	Image::Image(DrawableContext* dc, ImageHandle handle = ImageHandle(), DrawableInitParam param = DrawableInitParam()) : Drawable(dc, param) {
		Drawable::handle = handle;
	}

	Image::~Image() {
		//DeleteGraph(handle.getHandle());
	}

	void draw(int drawScreen) override;//–ˆƒtƒŒ[ƒ€ŒÄ‚Ô
};

