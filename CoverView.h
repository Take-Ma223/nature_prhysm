#pragma once
#include "Image.h"
#include "TransValue.h"
#include "View.h"

enum CoverViewPhase{
	Start,
	Play,
	Finish
};

class CoverView : public View
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

	int transitionDurationTime = 833;

	int transitionDurationTimeWindbreak = 750;

	const double functionXStart = 0.09384164222873900293255131964809;
	const double functionXRange = 0.90615835777126099706744868035191;
	const double functionYstart = 0.00880625381618665130158839363266;
	const double functionYRange = 0.99119374618381334869841160636734;


	const int CoverMoveRange = 618;

	int CoverFirstRatio = 0;//最初に閉じる割合

	int easing(int x);//0~1000を0~618に二次関数で変換
	int easingInvert(int x);//easingの逆変換

	CoverViewPhase phase = Start;

	void setMoveSpeedAnimation();

	void drawLeftCover();
	void drawMiddleCover();
	void drawRightCover();
public:
	TransValue CloseRatio;//カバー閉じてる率(1000分率)
	TransValue MoveSpeed;//カバー開閉1000分率スピード(キーボード操作時)押した瞬間だけ遅く動くようにする


	Image leftCover;
	Image middleCover;
	Image rightCover;

	CoverView(ViewContext* vc, Cordinate cordinate);

	void draw();//毎フレーム呼ぶ

	void closeAll();
	void openAll();

	void startMiddleCover();//開始時にカバーを上げる処理
	void finishMiddleCover();//終了時に現在値からカバーを下げる処理

	void startWindbreak();//演奏中にカバー上げ下げする処理の許可
	void setOpenRange(int openRatio);//演奏中の現在のカバー位置の指定(1000分率指定)
	void setFirstOpenRange(int openRatio);//演奏開始時のカバー位置の指定(1000分率指定)


	void openMiddleCover();//演奏中にカバーを上げる処理
	void closeMiddleCover();//演奏中にカバーを閉じる処理
	void stopMiddleCover();//演奏中にカバーの動きを止める処理
	void setMiddleCover();//(コントローラ用)//演奏中にカバーの位置を指定

	int getMiddleCoverBottomY();

	double getSpeedRatio();//体感ハイスピ倍率を求める


};

