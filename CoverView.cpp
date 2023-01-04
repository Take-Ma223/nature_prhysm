#include "CoverView.h"
#include <string>
using namespace std;

CoverView::CoverView(ViewContext* vc, Cordinate cordinate) :View(vc, cordinate)
{
	makeScreen(viewSize());

	wstring themeStr1(L"img/themes/");
	wstring themeStr2(vc->option->theme[vc->option->op.theme]);

	ImageHandle leftCoverHandle = ImageHandle();
	ImageHandle  middleCoverHandle = vc->asset->img((themeStr1 + themeStr2 + wstring(L"/cover_middle.png")).c_str());
	ImageHandle  rightCoverHandle = ImageHandle();

	leftCover = Image(leftCoverHandle, vc->time, Cordinate(XLeftCoverClose, YLeftCoverClose));
	middleCover = Image(middleCoverHandle, vc->time, Cordinate(XMiddleCoverClose, YMiddleCoverClose));
	rightCover = Image(rightCoverHandle, vc->time, Cordinate(XRightCoverClose, YRightCoverClose));

	CloseRatio = TransValue(vc->time);
	MoveSpeed = TransValue(vc->time);
	setMoveSpeedAnimation();
}

int CoverView::easing(int x)//時間から位置
{
	//0~1000を0~618に二次関数で変換

	//入力正規化(0~1)
	double input = functionXStart + ((double)x / 1000) * functionXRange;

	//正規化出力(0~1)
	double normalizedOutput = (pow(input, 2) - functionYstart) / functionYRange;

	return normalizedOutput * CoverMoveRange;
}

int CoverView::easingInvert(int x)//位置から時間
{
	//0~618を0~1000に二次関数で変換
	//入力正規化(0~1)
	double normalizedOutput = (double)x / CoverMoveRange;

	double input = sqrt(normalizedOutput * functionYRange + functionYstart);

	return ((input - functionXStart) / functionXRange) * 1000;
}

void CoverView::setMoveSpeedAnimation()
{
	MoveSpeed.clearEvent();
	MoveSpeed.eChange(0, 1000, Converter(Linear), 0, 150);
}

void CoverView::drawLeftCover(int drawScreen)
{
	leftCover.draw(drawScreen);
}

void CoverView::drawMiddleCover(int drawScreen)
{
	if (phase == Play) {
		MoveSpeed.update();
		double playSpeed = (double)MoveSpeed.getValue() / 1000;

		CloseRatio.update();
		CloseRatio.setPlaySpeed(playSpeed);
		middleCover.Y.setValue(YMiddleCoverOpen + CloseRatio.getValue());//線形に移動
	}

	middleCover.draw(drawScreen);
}

void CoverView::drawRightCover(int drawScreen)
{
	rightCover.draw(drawScreen);
}

void CoverView::putContents(int drawScreen)
{
	drawLeftCover(drawScreen);
	drawMiddleCover(drawScreen);
	drawRightCover(drawScreen);
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
	//カバーアニメーション設定
	middleCover.clearAllEvent();
	middleCover.Y.eSet(YMiddleCoverClose);
	middleCover.Y.eChange(YMiddleCoverClose, YMiddleCoverOpen + easing(CoverFirstRatio), Converter(QuarterSine), 0, 833);
	middleCover.Y.eSet(YMiddleCoverOpen + easing(CoverFirstRatio), 833);
	middleCover.setReverseAll(FALSE);
	middleCover.playAll();

	phase = Start;
}

void CoverView::finishMiddleCover()
{
	//カバーアニメーション設定
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
	middleCover.clearAllEvent();//画像クラス本体のイベントを使わないので削除

	//カバーアニメーション設定
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
	CloseRatio.setValue(easing(openRatio));
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
	CloseRatio.setValue(easing(CoverFirstRatio));
}

int CoverView::getMiddleCoverBottomY()
{
	return CloseRatio.getValue();
}

double CoverView::getSpeedRatio()
{
	int hideRatio = easingInvert(getMiddleCoverBottomY());//隠してる表示時間の割合
	double speed = 1;
	if (hideRatio == 1000) {
		speed = 99999;
	}
	else {
		speed = (double)1000 / (1000 - hideRatio);
	}

	return speed;
}
