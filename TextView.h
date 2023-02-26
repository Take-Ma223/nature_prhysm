#pragma once
#include "View.h"
#include "FontSet.h"
#include <string>

using namespace std;


class TextViewParam {
private:
    wstring string;
    int color;
    FontInfo fontInfo;

public:
    TextViewParam(wstring string = wstring(L""), FontInfo fontInfo = FontInfo(), int color = GetColor(128,128,128)) {
        TextViewParam::string = string;
        TextViewParam::color = color;
        TextViewParam::fontInfo = fontInfo;
    }

    wstring getString() { return string; };
    int getColor() { return color; };
    FontInfo getFontInfo() { return fontInfo; };

};

class TextView : public View
{
private:
    TextViewParam param;
    virtual void beforeDrawProcess(int drawScreen) override;

protected:
    ImageHandle textImageHandle;//表示する画像ハンドル情報

public:
    TextView(ActivityContext* c, TextViewParam textViewInitParam = TextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(c, drawableInitParam) {
        setText(textViewInitParam);
    }
    ~TextView() {
        DeleteGraph(textImageHandle.getHandle());
    }

    void setText(TextViewParam p);

};

