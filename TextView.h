#pragma once
#include "View.h"
#include "FontSet.h"
#include "TextViewParam.h"

/// <summary>
/// �e�L�X�g�\���r���[
/// </summary>
class TextView : public View
{
private:
    TextViewParam param;
    virtual void beforeDrawProcess(int drawScreen) override;

protected:
    ImageHandle textImageHandle;//�\������摜�n���h�����

public:
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    /// <param name="c">ActivityContext</param>
    /// <param name="textViewInitParam">TextView�p�����[�^</param>
    /// <param name="drawableInitParam">Drawable�p�����[�^</param>
    TextView(ActivityContext* c, TextViewParam textViewInitParam = TextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(c, drawableInitParam) {
        setText(textViewInitParam);
    }
    ~TextView() {
        DeleteGraph(textImageHandle.getHandle());
    }

    void setText(TextViewParam p);

};

