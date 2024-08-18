#pragma once
#include <string>
#include "FontInfo.h"

/// <summary>
/// NPTextView�̃p�����[�^
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
    /// �R���X�g���N�^
    /// </summary>
    /// <param name="string">�e�L�X�g</param>
    /// <param name="fontInfo">�t�H���g</param>
    /// <param name="frontColor">��O�̐F GetColor�Ŏw��</param>
    /// <param name="shadowColor">�e�̐F GetColor�Ŏw��</param>
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

