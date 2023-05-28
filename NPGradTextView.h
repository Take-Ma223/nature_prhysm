#pragma once
#include "NPTextView.h"
#include "Image.h"
class NPGradTextView : public NPTextView
{
public:
    NPGradTextView(ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : NPTextView(c, npTextViewInitParam, drawableInitParam) {
        initText(c, npTextViewInitParam, drawableInitParam);
    }
    NPGradTextView(Drawable* parent, ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : NPTextView(parent, c, npTextViewInitParam, drawableInitParam) {
        initText(c, npTextViewInitParam, drawableInitParam);
    }
    ~NPGradTextView() {
        //DeleteGraph(textImageHandle.getHandle());
        DeleteGraph(gradationHandle.getHandle());
    }

private:
    ImageHandle gradationHandle;//文字サイズに合わせたグラデーション画像ハンドル
    Image gradationImage;

    virtual void initText(ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) override {
        //NPTextView::initText(
        //    c, npTextViewInitParam, drawableInitParam
        //);

        //グラデーション画像の用意
        DeleteGraph(gradationHandle.getHandle());
        int screenHandle = MakeScreen(getSize().x, getSize().y, TRUE);
        Size size = Size(getSize().x, getSize().y);
        gradationHandle = ImageHandle(screenHandle, size);


        int err = setScreen(gradationHandle.getHandle());
        //Log(wstring(L"セットエラー    :") + to_wstring(err));
        //Log(wstring(L"描画対象        :") + to_wstring(GetDrawScreen()));

        //SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

        // 乗算済みアルファ用アルファブレンドのブレンドモードに設定する
        SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);
        err = DrawExtendGraph(0, 0, getSize().x, getSize().y,
            c->getAsset()->img(L"img/gradation.png").getHandle(),
            true);
        //Log(wstring(L"描画エラー      :") + to_wstring(err));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

        gradationImage = Image(this, c, gradationHandle);
        gradationImage.blendModeParam.mode = BlendMode::GRAPH_BLEND_DODGE;//覆い焼きに設定
        addDrawable(&gradationImage);
    }

};

