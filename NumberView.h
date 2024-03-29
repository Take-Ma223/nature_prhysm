#pragma once
#include "View.h"
#include "NumberViewFormat.h"

/// <summary>
/// 数値画像を用いた正の数値の描画View 
/// formatでfloatingPointを指定することで実数として表示が可能
/// </summary>
/// <param name="vc">ActivityContext</param>
/// <param name="numberViewInitFormat">フォーマット</param>
/// <param name="drawableInitParam">drawableパラメータ</param>
class NumberView : public View
{
private:
    NumberViewFormat format;
    int value = 0;//数値
    static const int maxDigit = 10;
    int digit = 1;//表示桁数
    int digitArray[maxDigit] = { 0,0,0,0,0,0,0,0,0,0, };//各桁を格納 [0]が1の位
    int paintLength = 0;//今のvalueを描画するときの横方向の長さ

    //valueを桁数毎に配列に格納
    void setDigitArray() {
        int i = 0;
        for (i = 0; i <= maxDigit - 1; i++) {
            digitArray[i] = int(
                (value % int(pow(10, i + 1)))
                /
                pow(10, i)
                );
        }

    }

    //表示する桁数を求める
    void setShowDigit() {
        digit = int(log10(value) + 1);//表示桁数を格納
        if (value == 0) {//0の時は1桁表示
            digit = 1;
        }

        if (format.getFloatingPoint() >= digit) {
            digit = format.getFloatingPoint() + 1;
        }

        if (format.getZeroFilingDigit() > digit)digit = format.getZeroFilingDigit();
    }

    //描画時の長さを求める
    void setPaintLength() {
        int digitWidth = format.getHandles().getNumbers()[0].getSize().x;
        int pointWidth = format.getHandles().getPoint().getSize().x;
        int interval = format.getInterval();

        paintLength = 
            digit * digitWidth +
            (digit - 1) * interval;

        if (format.getFloatingPoint() != 0)paintLength += pointWidth + interval;//小数点表示分を考慮

        int zeroFillRemain= format.getZeroFilingDigit() - digit;//0埋めしなければいけない桁数
        if (zeroFillRemain >= 1) {
            paintLength += zeroFillRemain * (digitWidth + interval);
        }
    }
protected:
    virtual void beforeDrawProcess(int drawScreen) override;

public:
    /// <summary>
    /// コンストラクタ
    /// </summary>
    /// <param name="c">ActivityContext</param>
    /// <param name="numberViewInitFormat">フォーマット</param>
    /// <param name="drawableInitParam">Drawableパラメータ</param>
    NumberView(ActivityContext* c, NumberViewFormat numberViewInitFormat, DrawableInitParam drawableInitParam = DrawableInitParam()) : View(c, drawableInitParam) {
        setFormat(numberViewInitFormat);
    }

    void setFormat(NumberViewFormat f) {
        format = f;

        int digitWidth = format.getHandles().getNumbers()[0].getSize().x;

        Size s = Size(
            digitWidth * (format.getDigits())
            + format.getInterval() * (format.getDigits())
            + format.getHandles().getPoint().getSize().x,//取り得る最大の長さ
            format.getHandles().getNumbers()[0].getSize().y
        );
        makeScreen(s);
        setValue(0);
    }

    void setValue(int value) { 
        NumberView::value = value;
        int min = 0;
        int max;
        if (format.getDigits() == 10) {//INTの値域を超えるためINT_MAXで置き換え
            max = INT_MAX;
        }
        else {
            max = pow(10, format.getDigits()) - 1;
        }


        if (NumberView::value < min) {
            NumberView::value = min;
        }
        if (NumberView::value > max) {
            NumberView::value = max;
        }
   
        setDigitArray();
        setShowDigit();
        setPaintLength();
    }

    


};

