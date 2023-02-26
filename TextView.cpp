#include "TextView.h"

void TextView::beforeDrawProcess(int drawScreen)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255,255,255);
	FontHandle fh = context->getAsset()->font(param.getFontInfo());
	DrawStringToHandle(
		0,0, 
		param.getString().c_str(), 
		param.getColor(),
		fh.getHandle()
		);
}



void TextView::setText(TextViewParam p) {
	TextView::param = p;
	FontHandle fh = context->getAsset()->font(param.getFontInfo());

	int width = GetDrawStringWidthToHandle(
		param.getString().c_str(),
		param.getString().length(),
		fh.getHandle());

	//テキスト画像の用意
	int screenHandle = MakeScreen(width, fh.getSize(), TRUE);
	Size size = Size(width, fh.getSize());
	textImageHandle = ImageHandle(screenHandle, size);
	setScreen(textImageHandle.getHandle());
	DrawStringToHandle(
		0, 0,
		param.getString().c_str(),
		param.getColor(),
		fh.getHandle()
	);


	makeScreen(size);

	
}