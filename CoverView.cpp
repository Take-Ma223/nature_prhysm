#include "CoverView.h"

CoverView::CoverView(int leftCoverHandle, int middleCoverHandle, int rightCoverHandle, double* time)
{
	leftCover = Image(leftCoverHandle, time, XLeftCoverClose, YLeftCoverClose);
	leftCover = Image(middleCoverHandle, time, XMiddleCoverClose, YMiddleCoverClose);
	rightCover = Image(rightCoverHandle, time, XRightCoverClose, YRightCoverClose);

}

void CoverView::drawLeftCover()
{
	leftCover.draw();
}

void CoverView::drawMiddleCover()
{
	middleCover.draw();
}

void CoverView::drawRightCover()
{
	rightCover.draw();
}

void CoverView::closeAll()
{

}

void CoverView::openAll()
{
	//leftCover.moveTo(XLeftCoverOpen, YLeftCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDuration);
	//middleCover.moveTo(XMiddleCoverOpen, YMiddleCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDuration);
	//rightCover.moveTo(XRightCoverOpen, YRightCoverOpen, Abs, ConvertMode::QuarterCosine, transitionDuration);


}
