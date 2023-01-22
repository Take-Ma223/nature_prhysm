#pragma once
#include "View.h"
#include <cassert>

using namespace std;

class NumberImageHandles {
    vector<ImageHandle> numbers;
    ImageHandle point;
public:
    NumberImageHandles(vector<ImageHandle> numbers = vector<ImageHandle>(), ImageHandle point = ImageHandle()) {
        NumberImageHandles::numbers = numbers;
        NumberImageHandles::point = point;
    }

    vector<ImageHandle> getNumbers() { return numbers; }
    ImageHandle getPoint() { return point; }
};

enum class NumberViewShowType {
    Left,//������
    Center,//���S����
    Right//�E����
};

class NumberViewFormat {
private:
    NumberImageHandles handles;
    int digits;//�ő包��
    int floatingPoint;//digit�ɑ΂��ď����_��łʒu
    int interval;//interval:���l�̊Ԋu
    NumberViewShowType showType;//������
    int zeroFilingDigit;//zeroFilingDigit:0�Ŗ��߂錅�� 0���w�肷���0�Ŗ��߂Ȃ�

public:
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

/**
* ���l�摜��p�������̐��l�̕`��View
* format��floatingPoint���w�肷�邱�ƂŎ����Ƃ��ĕ\�����\
* 
*/
class NumberView : public View
{
private:
    NumberViewFormat format;
    int value = 0;//���l
    static const int maxDigit = 10;
    int digit = 1;//�\������
    int digitArray[maxDigit] = { 0,0,0,0,0,0,0,0,0,0, };//�e�����i�[ [0]��1�̈�
    int paintLength = 0;//����value��`�悷��Ƃ��̉������̒���

    //value���������ɔz��Ɋi�[
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

    //�\�����錅�������߂�
    void setShowDigit() {
        digit = int(log10(value) + 1);//�\���������i�[
        if (value == 0) {//0�̎���1���\��
            digit = 1;
        }

        if (format.getFloatingPoint() >= digit) {
            digit = format.getFloatingPoint() + 1;
        }

        if (format.getZeroFilingDigit() > digit)digit = format.getZeroFilingDigit();
    }

    //�`�掞�̒��������߂�
    void setPaintLength() {
        int digitWidth = format.getHandles().getNumbers()[0].getSize().x;
        int pointWidth = format.getHandles().getPoint().getSize().x;
        int interval = format.getInterval();

        paintLength = 
            digit * digitWidth +
            (digit - 1) * interval;

        if (format.getFloatingPoint() != 0)paintLength += pointWidth + interval;//�����_�\�������l��

        int zeroFillRemain= format.getZeroFilingDigit() - digit;//0���߂��Ȃ���΂����Ȃ�����
        if (zeroFillRemain >= 1) {
            paintLength += zeroFillRemain * (digitWidth + interval);
        }
    }
protected:
    virtual void prepareScreen(int drawScreen) override;

public:
    NumberView(ViewContext* vc, NumberViewFormat numberViewInitFormat, DrawableInitParam drawableInitParam = DrawableInitParam()) : View(vc, drawableInitParam) {
        setFormat(numberViewInitFormat);
    }

    void setFormat(NumberViewFormat f) {
        format = f;

        int digitWidth = format.getHandles().getNumbers()[0].getSize().x;

        Size s = Size(
            digitWidth * (format.getDigits())
            + format.getInterval() * (format.getDigits())
            + format.getHandles().getPoint().getSize().x,//��蓾��ő�̒���
            format.getHandles().getNumbers()[0].getSize().y
        );
        makeScreen(s);
        setValue(0);
    }

    void setValue(int value) { 
        NumberView::value = value;
        int min = 0;
        int max;
        if (format.getDigits() == 10) {//INT�̒l��𒴂��邽��INT_MAX�Œu������
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

