#include "View.h"
#include <DxLib.h>

void View::deleteGraph()
{
	DeleteGraph(handle.getHandle());
}

void View::makeScreen(Size s)
{
	deleteGraph();
	int screenHandle = MakeScreen(s.x, s.y, TRUE);
	Size size = s;
	handle = ImageHandle(screenHandle, size);
}

void View::draw(int drawScreen)
{
	//Viewの描画
	setScreen(handle.getHandle());
	ClearDrawScreen();
	beforeDrawProcess(handle.getHandle());

	//このViewの抱えるDrawableを全て描画(stackに積まれた順に描画する)
	for (auto content : drawables) {
		content->draw(handle.getHandle());
	}

	//View全体を指定されたスクリーンへ描画
	setScreen(drawScreen);
	drawWithProcessing();
}
