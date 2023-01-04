#include "View.h"
#include <DxLib.h>

ViewContext::ViewContext(Asset* asset, Option* option, double* time)
{
	ViewContext::asset = asset;
	ViewContext::option = option;
	ViewContext::time = time;
}

void View::makeScreen(Size s)
{
	int h = MakeScreen(s.x, s.y, TRUE);
	handle = ImageHandle(h, s);
	vc->asset->registImageHandler(handle);
}

View::View(ViewContext* vc, Cordinate cordinate){
	View::vc = vc;
	View::cordinate = cordinate;
}

void View::draw(int drawScreen)
{
	//View�̕`��
	setScreen(handle.getHandle());
	ClearDrawScreen();
	putContents(handle.getHandle());

	//View�S�̂��w�肳�ꂽ�X�N���[���֕`��
	setScreen(drawScreen);
	DrawGraph(cordinate.x, cordinate.y, handle.getHandle(), TRUE);
}

void View::setScreen(int drawScreen)
{
	SetDrawScreen(drawScreen);
}
