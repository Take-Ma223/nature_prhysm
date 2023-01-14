#include "View.h"
#include <DxLib.h>

ViewContext::ViewContext(Asset* asset, Option* option, double* time)
{
	ViewContext::asset = asset;
	ViewContext::option = option;
	ViewContext::time = time;
}

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
	//View�̕`��
	setScreen(handle.getHandle());
	ClearDrawScreen();
	prepareScreen(handle.getHandle());

	//View�S�̂��w�肳�ꂽ�X�N���[���֕`��
	setScreen(drawScreen);
	drawWithProcessing();
}
