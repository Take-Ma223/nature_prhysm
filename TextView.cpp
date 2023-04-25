#include "TextView.h"
#include "Log.h"

void TextView::beforeDrawProcess(int drawScreen)
{
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
	SetDrawBright(255,255,255);
	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);
	int test = setScreen(handle.getHandle());
	Log(wstring(L"描画対象        :") + to_wstring(GetDrawScreen()));

	Log(wstring(L"setScreentext   :") + to_wstring(handle.getHandle()));
	Log(wstring(L"setScreen 成功  :") + to_wstring(test));
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

	int height = fh.getSize() + 5;//小さいフォントサイズだと下が欠けるため縦サイズは1多めにとる
	//テキスト画像の用意
	int screenHandle = MakeScreen(width, height, TRUE);
	Size size = Size(width, height);
	textImageHandle = ImageHandle(screenHandle, size);
	int err = setScreen(textImageHandle.getHandle());
	Log(wstring(L"セットエラー    :") + to_wstring(err));
	Log(wstring(L"描画対象        :") + to_wstring(GetDrawScreen()));

	SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

	// 乗算済みアルファ用アルファブレンドのブレンドモードに設定する
	SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);
	err = DrawStringToHandle(
		0, 2,
		param.getString().c_str(),
		param.getColor(),
		fh.getHandle()
	);
	Log(wstring(L"描画エラー      :") + to_wstring(err));


	makeScreen(size);

}