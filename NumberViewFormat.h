#pragma once
#include "NumberImageHandles.h"
#include "NumberViewShowType.h"
#include <cassert>

/// <summary>
/// NumberView�̃t�H�[�}�b�g
/// </summary>
class NumberViewFormat {
private:
    NumberImageHandles handles;
    int digits;//�ő包��
    int floatingPoint;//digit�ɑ΂��ď����_��łʒu
    int interval;//interval:���l�̊Ԋu
    NumberViewShowType showType;//������
    int zeroFilingDigit;//zeroFilingDigit:0�Ŗ��߂錅�� 0���w�肷���0�Ŗ��߂Ȃ�

public:
    /// <summary>
    /// �R���X�g���N�^
    /// </summary>
    /// <param name="handles">���l�摜�n���h��</param>
    /// <param name="digits">����</param>
    /// <param name="floatingPoint">�����_�̌��ʒu 0�Ő����\��</param>
    /// <param name="interval">���l�̊Ԋu</param>
    /// <param name="showType">�\���`��</param>
    /// <param name="zeroFilingDigit">0���߂��錅��</param>
    NumberViewFormat(NumberImageHandles handles = NumberImageHandles(), int digits = 1, int floatingPoint = 0, int interval = 16, NumberViewShowType showType = NumberViewShowType::Left, int zeroFilingDigit = 0) {
        NumberViewFormat::handles = handles;

        assert(1 <= digits && digits <= 10);
        NumberViewFormat::digits = digits;

        assert(0 <= floatingPoint && floatingPoint <= 9);
        assert(floatingPoint < digits);
        NumberViewFormat::floatingPoint = floatingPoint;

        NumberViewFormat::interval = interval;
        NumberViewFormat::showType = showType;

        assert(0 <= zeroFilingDigit && zeroFilingDigit <= 10);
        NumberViewFormat::zeroFilingDigit = zeroFilingDigit;

        assert(zeroFilingDigit <= digits);
    }

    NumberImageHandles getHandles() { return handles; }
    int getDigits() { return digits; }
    int getFloatingPoint() { return floatingPoint; }
    int getInterval() { return interval; }
    NumberViewShowType getShowType() { return showType; }
    int getZeroFilingDigit() { return zeroFilingDigit; }

};

