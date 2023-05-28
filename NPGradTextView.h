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
    ImageHandle gradationHandle;//�����T�C�Y�ɍ��킹���O���f�[�V�����摜�n���h��
    Image gradationImage;

    virtual void initText(ActivityContext* c, NPTextViewParam npTextViewInitParam = NPTextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) override {
        //NPTextView::initText(
        //    c, npTextViewInitParam, drawableInitParam
        //);

        //�O���f�[�V�����摜�̗p��
        DeleteGraph(gradationHandle.getHandle());
        int screenHandle = MakeScreen(getSize().x, getSize().y, TRUE);
        Size size = Size(getSize().x, getSize().y);
        gradationHandle = ImageHandle(screenHandle, size);


        int err = setScreen(gradationHandle.getHandle());
        //Log(wstring(L"�Z�b�g�G���[    :") + to_wstring(err));
        //Log(wstring(L"�`��Ώ�        :") + to_wstring(GetDrawScreen()));

        //SetDrawMode(DX_DRAWMODE_ANISOTROPIC);

        // ��Z�ς݃A���t�@�p�A���t�@�u�����h�̃u�����h���[�h�ɐݒ肷��
        SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);
        err = DrawExtendGraph(0, 0, getSize().x, getSize().y,
            c->getAsset()->img(L"img/gradation.png").getHandle(),
            true);
        //Log(wstring(L"�`��G���[      :") + to_wstring(err));
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

        gradationImage = Image(this, c, gradationHandle);
        gradationImage.blendModeParam.mode = BlendMode::GRAPH_BLEND_DODGE;//�����Ă��ɐݒ�
        addDrawable(&gradationImage);
    }

};

