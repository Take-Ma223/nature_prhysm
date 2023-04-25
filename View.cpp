#include "View.h"
#include <DxLib.h>
#include "Log.h"

void View::deleteGraph()
{
	Log(wstring(L"�폜            :") + to_wstring(handle.getHandle()));

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
	Log(wstring(L"draw���s        :") + to_wstring(drawScreen));


	//View�̕`��
	int test = setScreen(handle.getHandle());
	Log(wstring(L"setScreen       :") + to_wstring(handle.getHandle()));
	Log(wstring(L"setScreen ����  :") + to_wstring(test));

	ClearDrawScreen();
	Log(wstring(L"ClearDrawScreen :") + to_wstring(GetDrawScreen()));

	beforeDrawProcess(handle.getHandle());

	//����View�̕�����Drawable��S�ĕ`��(stack�ɐς܂ꂽ���ɕ`�悷��)
	for (auto content = drawables.begin(); content != drawables.end(); ++content) {
		(*content)->draw(handle.getHandle());
	}

	//View�S�̂��w�肳�ꂽ�X�N���[���֕`��

	test = setScreen(drawScreen);
	Log(wstring(L"setScreen       :") + to_wstring(drawScreen));
	Log(wstring(L"setScreen ����  :") + to_wstring(test));

	drawWithProcessing();
}
