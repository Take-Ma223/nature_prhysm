#include "TextView.h"
#include "Log.h"

void TextView::beforeDrawProcess(int drawScreen)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255,255,255);
	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);
	int test = setScreen(handle.getHandle());
	Log(wstring(L"�`��Ώ�        :") + to_wstring(GetDrawScreen()));

	Log(wstring(L"setScreentext   :") + to_wstring(handle.getHandle()));
	Log(wstring(L"setScreen ����  :") + to_wstring(test));
	DrawGraph(0, 0, textImageHandle.getHandle(), TRUE);
}



void TextView::setText(TextViewParam p) {
	TextView::param = p;
	SetFontCacheUsePremulAlphaFlag(TRUE);
	FontHandle fh = context->getAsset()->font(param.getFontInfo());
	SetFontCacheUsePremulAlphaFlag(FALSE);

	int width = GetDrawStringWidthToHandle(
		param.getString().c_str(),
		param.getString().length(),
		fh.getHandle());

	int height = fh.getSize() + 5;//�������t�H���g�T�C�Y���Ɖ��������邽�ߏc�T�C�Y��1���߂ɂƂ�
	//�e�L�X�g�摜�̗p��
	int screenHandle = MakeScreen(width, height, TRUE);
	Size size = Size(width, height);
	textImageHandle = ImageHandle(screenHandle, size);
	int err = setScreen(textImageHandle.getHandle());
	Log(wstring(L"�Z�b�g�G���[    :") + to_wstring(err));
	Log(wstring(L"�`��Ώ�        :") + to_wstring(GetDrawScreen()));

	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

	// ��Z�ς݃A���t�@�p�A���t�@�u�����h�̃u�����h���[�h�ɐݒ肷��
	SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);
	err = DrawStringToHandle(
		0, 2,
		param.getString().c_str(),
		param.getColor(),
		fh.getHandle()
	);
	Log(wstring(L"�`��G���[      :") + to_wstring(err));


	makeScreen(size);

}