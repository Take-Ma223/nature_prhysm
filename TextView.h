#pragma once
#include "View.h"
#include "FontSet.h"
#include "TextViewParam.h"

/// <summary>
/// テキスト表示ビュー
/// </summary>
class TextView : public View
{
private:
    TextViewParam param;
    virtual void beforeDrawProcess(int drawScreen) override;

protected:
    ImageHandle textImageHandle;//表示する画像ハンドル情報

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="c">ActivityContext</param>
    /// <param name="textViewInitParam">TextViewパラメータ</param>
    /// <param name="drawableInitParam">Drawableパラメータ</param>
    TextView(ActivityContext* c, TextViewParam textViewInitParam = TextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(c, drawableInitParam) {
        setText(textViewInitParam);
    }
    ~TextView() {
        DeleteGraph(textImageHandle.getHandle());
    }

    void setText(TextViewParam p);

};

