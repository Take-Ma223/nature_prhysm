#include "Image.h"

#pragma once
class CoverView
{
	int XLeftCoverOpen = -320;
	int XLeftCoverClose = 0;
	int YLeftCoverOpen = 0;
	int YLeftCoverClose = 0;

	int XMiddleCoverOpen = 320;
	int XMiddleCoverClose = 320;
	int YMiddleCoverOpen = -720;
	int YMiddleCoverClose = 0;

	int XRightCoverOpen = 960;
	int XRightCoverClose = 1280;
	int YRightCoverOpen = 0;
	int YRightCoverClose = 0;

	int transitionDuration = 833;

public:
	Image leftCover;
	Image middleCover;
	Image rightCover;

	CoverView(int leftCoverHandle, int middleCoverHandle, int rightCoverHandle, double* time);
	void drawLeftCover();//���t���[���Ă�
	void drawMiddleCover();//���t���[���Ă�
	void drawRightCover();//���t���[���Ă�

	void closeAll();
	void openAll();




};

