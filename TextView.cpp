#include "TextView.h"

void TextView::prepareScreen(int drawScreen)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255,255,255);
	FontHandle fh = vc->asset->font(param.getFontInfo());
	DrawStringToHandle(
		0,0, 
		param.getString().c_str(), 
		param.getColor(),
		fh.getHandle()
		);
}

void TextView::setText(TextViewParam p) {
	TextView::param = p;
	FontHandle fh = vc->asset->font(param.getFontInfo());

	int width = GetDrawStringWidthToHandle(
		param.getString().c_str(),
		param.getString().length(),
		fh.getHandle());

	makeScreen(Size(width, fh.getSize()));
}