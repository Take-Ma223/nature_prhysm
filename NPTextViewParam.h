#pragma once
#include <string>
#include "FontInfo.h"

/// <summary>
/// NPTextViewのパラメータ
/// </summary>
class NPTextViewParam
{
private:
    wstring string;
    int frontColor;
    int shadowColor;
    FontInfo fontInfo;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="string">テキスト</param>
    /// <param name="fontInfo">フォント</param>
    /// <param name="frontColor">手前の色 GetColorで指定</param>
    /// <param name="shadowColor">影の色 GetColorで指定</param>
    NPTextViewParam(
        wstring string = wstring(L""), 
        FontInfo fontInfo = FontInfo(), 
        int frontColor = GetColor(255, 255, 255), 
        int shadowColor = GetColor(0, 0, 0)
    ) {
        NPTextViewParam::string = string;
        NPTextViewParam::frontColor = frontColor;
        NPTextViewParam::shadowColor = shadowColor;
        NPTextViewParam::fontInfo = fontInfo;
    }

    wstring getString() { return string; };
    int getFrontColor() { return frontColor; };
    int getShadowColor() { return shadowColor; };
    FontInfo getFontInfo() { return fontInfo; };

};

