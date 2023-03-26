#pragma once
#include <string>
#include "FontInfo.h"

/// <summary>
/// TextViewのパラメータ
/// </summary>
class TextViewParam {
private:
    wstring string;
    int color;
    FontInfo fontInfo;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="string">テキスト</param>
    /// <param name="fontInfo">フォント</param>
    /// <param name="color">色 GetColorで指定</param>
    TextViewParam(wstring string = wstring(L""), FontInfo fontInfo = FontInfo(), int color = GetColor(128, 128, 128)) {
        TextViewParam::string = string;
        TextViewParam::color = color;
        TextViewParam::fontInfo = fontInfo;
    }

    wstring getString() { return string; };
    int getColor() { return color; };
    FontInfo getFontInfo() { return fontInfo; };

};
