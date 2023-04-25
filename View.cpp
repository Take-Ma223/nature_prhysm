#include "View.h"
#include <DxLib.h>
#include "Log.h"

void View::deleteGraph()
{
	Log(wstring(L"削除            :") + to_wstring(handle.getHandle()));

	DeleteGraph(handle.getHandle());
}

void View::makeScreen(Size s)
{
	deleteGraph();

	int screenHandle = MakeScreen(s.x, s.y, TRUE);
	Size size = s;
	handle = ImageHandle(screenHandle, size);
}

void View::draw()
{
	Drawable::draw();
}

void View::draw(int drawScreen)
{
	Log(wstring(L"draw実行        :") + to_wstring(drawScreen));


	//Viewの描画
	int test = setScreen(handle.getHandle());
	Log(wstring(L"setScreen       :") + to_wstring(handle.getHandle()));
	Log(wstring(L"setScreen 成功  :") + to_wstring(test));

	ClearDrawScreen();
	Log(wstring(L"ClearDrawScreen :") + to_wstring(GetDrawScreen()));

	beforeDrawProcess(handle.getHandle());

	//このViewの抱えるDrawableを全て描画(stackに積まれた順に描画する)
	for (auto content = drawables.begin(); content != drawables.end(); ++content) {
		(*content)->draw(handle.getHandle());
	}

	//View全体を指定されたスクリーンへ描画

	test = setScreen(drawScreen);
	Log(wstring(L"setScreen       :") + to_wstring(drawScreen));
	Log(wstring(L"setScreen 成功  :") + to_wstring(test));

	drawWithProcessing();
}
