#include "CoverView.h"
#include <string>
#include "CONSTANT_VALUE.h"
using namespace std;

int CoverView::easing(int x)//���Ԃ���ʒu
{
	//0~1000��0~618�ɓ񎟊֐��ŕϊ�

	//���͐��K��(0~1)
	double input = functionXStart + ((double)x / 1000) * functionXRange;

	//���K���o��(0~1)
	double normalizedOutput = (pow(input, 2) - functionYstart) / functionYRange;

	return normalizedOutput * CoverMoveRange;
}

int CoverView::easingInvert(int x)//�ʒu���玞��
{
	//0~618��0~1000�ɓ񎟊֐��ŕϊ�
	//���͐��K��(0~1)
	double normalizedOutput = (double)x / CoverMoveRange;

	double input = sqrt(normalizedOutput * functionYRange + functionYstart);

	return ((input - functionXStart) / functionXRange) * 1000;
}

void CoverView::setMoveSpeedAnimation()
{
	MoveSpeed.clearEvent();
	MoveSpeed.eChange(0, 1000, Converter(Linear), 0, 150);
}

void CoverView::beforeDrawLeftCoverProcess(int drawScreen)
{

}

void CoverView::befpreDrawMiddleCoverProcess(int drawScreen)
{
	if (phase == Play) {
		MoveSpeed.process();
		double playSpeed = (double)MoveSpeed.value / 1000;

		CloseRatio.process();
		CloseRatio.setPlaySpeed(playSpeed);
		middleCover.Y.value = (YMiddleCoverOpen + CloseRatio.value);//���`�Ɉړ�
	}

}

void CoverView::beforeDrawRightCoverProcess(int drawScreen)
{

}

void CoverView::beforeDrawProcess(int drawScreen)
{
	beforeDrawLeftCoverProcess(drawScreen);
	befpreDrawMiddleCoverProcess(drawScreen);
	beforeDrawRightCoverProcess(drawScreen);
}

void CoverView::closeAll()
{

}

void CoverView::openAll()
{
	//leftCover.moveTo(XLeftCoverOpen, YLeftCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDurationTime);
	//middleCover.moveTo(XMiddleCoverOpen, YMiddleCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDurationTime);
	//rightCover.moveTo(XRightCoverOpen, YRightCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDurationTime);


}

void CoverView::startMiddleCover()
{
	//�J�o�[�A�j���[�V�����ݒ�
	middleCover.clearAllEvent();
	middleCover.Y.eSet(YMiddleCoverClose);
	middleCover.Y.eChange(YMiddleCoverClose, YMiddleCoverOpen + easing(CoverFirstRatio), Converter(QuarterSine), OFFSET_MARGIN, 833);
	middleCover.Y.eSet(YMiddleCoverOpen + easing(CoverFirstRatio), 833);
	middleCover.setReverseAll(FALSE);
	middleCover.playAll();

	phase = Start;
}

void CoverView::finishMiddleCover()
{
	//�J�o�[�A�j���[�V�����ݒ�
	middleCover.clearAllEvent();
	//middleCover.Y.eSet(YMiddleCoverOpen + easing(CloseRatio.getValue()));
	middleCover.Y.eChange(Point(0,Rel), YMiddleCoverClose, Converter(QuarterSine), 0, 833);
	middleCover.Y.eSet(YMiddleCoverClose, 833);
	middleCover.setReverseAll(FALSE);
	middleCover.playAll();

	phase = Finish;
}

void CoverView::startWindbreak()
{
	middleCover.clearAllEvent();//�摜�N���X�{�̂̃C�x���g���g��Ȃ��̂ō폜

	//�J�o�[�A�j���[�V�����ݒ�
	CloseRatio.clearEvent();
	CloseRatio.eSet(0);
	CloseRatio.eChange(0, CoverMoveRange, Converter(Linear), 0, transitionDurationTimeWindbreak);
	CloseRatio.eSet(CoverMoveRange, transitionDurationTimeWindbreak);
	CloseRatio.stop();

	setOpenRange(CoverFirstRatio);

	phase = Play;
}

void CoverView::setOpenRange(int openRatio)
{
	CloseRatio.value = (easing(openRatio));
	CloseRatio.setStartTimeFromRange((double)easing(openRatio) / CoverMoveRange);
}

void CoverView::setFirstOpenRange(int openRatio)
{
	CoverFirstRatio = openRatio;

}

void CoverView::openMiddleCover()
{
	CloseRatio.setReverse(TRUE);
	CloseRatio.resume();

	MoveSpeed.play();
}

void CoverView::closeMiddleCover()
{
	CloseRatio.setReverse(FALSE);
	CloseRatio.resume();

	MoveSpeed.play();
}

void CoverView::stopMiddleCover()
{
	CloseRatio.stop();
}

void CoverView::setMiddleCover()
{
	setOpenRange(easing(CoverFirstRatio));
	CloseRatio.value = (easing(CoverFirstRatio));
}

int CoverView::getMiddleCoverBottomY()
{
	return CloseRatio.value;
}

double CoverView::getSpeedRatio()
{
	int hideRatio = easingInvert(getMiddleCoverBottomY());//�B���Ă�̈�̕\�����Ԃ̊���
	double speed = 1;
	if (hideRatio == 1000) {
		speed = 99999;
	}
	else {
		speed = (double)1000 / (1000 - hideRatio);
	}

	return speed;
}
