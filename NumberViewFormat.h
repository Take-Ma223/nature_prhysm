#pragma once
#include "NumberImageHandles.h"
#include "NumberViewShowType.h"
#include <cassert>

/// <summary>
/// NumberViewのフォーマット
/// </summary>
class NumberViewFormat {
private:
    NumberImageHandles handles;
    int digits;//最大桁数
    int floatingPoint;//digitに対して小数点を打つ位置
    int interval;//interval:数値の間隔
    NumberViewShowType showType;//揃え方
    int zeroFilingDigit;//zeroFilingDigit:0で埋める桁数 0を指定すると0で埋めない

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="handles">数値画像ハンドル</param>
    /// <param name="digits">桁数</param>
    /// <param name="floatingPoint">小数点の桁位置 0で整数表示</param>
    /// <param name="interval">数値の間隔</param>
    /// <param name="showType">表示形式</param>
    /// <param name="zeroFilingDigit">0埋めする桁数</param>
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

