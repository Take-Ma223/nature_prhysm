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
	DeleteGraph(screenHandle);
}

void View::makeScreen(Size s)
{
	deleteGraph();
	screenHandle = MakeScreen(s.x, s.y, TRUE);
	size = s;
	vc->asset->registImageHandler(screenHandle);
}

View::View(ViewContext* vc, Cordinate cordinate){
	View::vc = vc;
	View::cordinate = cordinate;
}

View::~View()
{
	deleteGraph();
}

void View::draw(int drawScreen)
{
	//Viewの描画
	setScreen(screenHandle);
	ClearDrawScreen();
	putContents(screenHandle);

	//View全体を指定されたスクリーンへ描画
	setScreen(drawScreen);
	DrawGraph(cordinate.x, cordinate.y, screenHandle, TRUE);
}

void View::setScreen(int drawScreen)
{
	SetDrawScreen(drawScreen);
}
