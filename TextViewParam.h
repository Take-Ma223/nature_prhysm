#pragma once
#include <string>
#include "FontInfo.h"

/// <summary>
/// TextView�̃p�����[�^
/// </summary>
class TextViewParam {
private:
    wstring string;
    int color;
    FontInfo fontInfo;

public:
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    /// <param name="string">�e�L�X�g</param>
    /// <param name="fontInfo">�t�H���g</param>
    /// <param name="color">�F GetColor�Ŏw��</param>
    TextViewParam(wstring string = wstring(L""), FontInfo fontInfo = FontInfo(), int color = GetColor(128, 128, 128)) {
        TextViewParam::string = string;
        TextViewParam::color = color;
        TextViewParam::fontInfo = fontInfo;
    }

    wstring getString() { return string; };
    int getColor() { return color; };
    FontInfo getFontInfo() { return fontInfo; };

};
