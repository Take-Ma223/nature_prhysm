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
    virtual void prepareScreen(int drawScreen) override;

protected:
    ImageHandle textImageHandle;//ï\é¶Ç∑ÇÈâÊëúÉnÉìÉhÉãèÓïÒ

public:
    TextView(DrawableContext* dc, TextViewParam textViewInitParam = TextViewParam(), DrawableInitParam drawableInitParam = DrawableInitParam()) : View(dc, drawableInitParam) {
        setText(textViewInitParam);
    }
    ~TextView() {
        DeleteGraph(textImageHandle.getHandle());
    }

    void setText(TextViewParam p);

};

