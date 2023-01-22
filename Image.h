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
	Image::Image(ImageHandle handle = ImageHandle(), double* time = NULL, DrawableInitParam param = DrawableInitParam()) : Drawable(time, param) {
		Drawable::handle = handle;
	}

	Image::~Image() {
		//DeleteGraph(handle.getHandle());
	}

	void draw(int drawScreen) override;//–ˆƒtƒŒ[ƒ€ŒÄ‚Ô
};

