#include "NumberView.h"

void NumberView::beforeDrawProcess(int drawScreen)
{
	int xOffset = 0;
	if (format.getShowType() == NumberViewShowType::Left) {
		xOffset = 0;
	}
	else if (format.getShowType() == NumberViewShowType::Center) {
		xOffset = (handle.getSize().x - paintLength)/2;
	}
	else if (format.getShowType() == NumberViewShowType::Right) {
		xOffset = handle.getSize().x - paintLength;
	}

	int numberWidth = format.getHandles().getNumbers()[0].getSize().x;
	int floatingOffset = format.getHandles().getPoint().getSize().x + format.getInterval();
	
	SetDrawMode(DX_DRAWMODE_BILINEAR);
	SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, alpha.value);

	//�E���獶�ɕ`��
	for (int i = 0; i < digit; i++) {
		int digitArrayIndex = digit - 1 - i;//digit~0

		bool isFloatingPoint = i == format.getFloatingPoint();
		if (isFloatingPoint) {//�����_��łX�y�[�X�����炷
			floatingOffset = 0;
		}
		int x = xOffset + floatingOffset + (numberWidth + format.getInterval()) * digitArrayIndex;

		bool isRealNumber = format.getFloatingPoint() != 0;
		if (isFloatingPoint && isRealNumber) {//�����\���̎��̂ݏ����_��\��
			int pointHandle = format.getHandles().getPoint().getHandle();
			DrawGraph(x + numberWidth, 0, pointHandle, TRUE);
		}

		int numberHandle = format.getHandles().getNumbers()[
			digitArray[i]
		].getHandle();
		DrawGraph(x, 0, numberHandle, TRUE);
	}

}
