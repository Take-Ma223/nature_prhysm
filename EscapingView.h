#pragma once

#include "View.h"
#include "NPTextViewParam.h"
#include "NPTextView.h"
#include "show_something.h"

/// <summary>
/// ESCキー長押し表示のView
/// </summary>
class EscapingView : public View
{
private:
	
	unique_ptr<NPTextView> text_view;
	const int size_x = 360;
	const int size_y = 180;
	FontInfo font;

	double gauge_ratio = 0;
	
	const int text_center_x = 0.5 * size_x;
	const int text_center_y = 0.5 * size_y;

	const int box_center_x = 0.5 * size_x;
	const int box_center_y = 0.5 * size_y;
	const int box_width = 180;
	const int box_height = 60;

public:
	EscapingView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(size_x, size_y));
		font = FontInfo(wstring(c->getConfig()->BaseFont), 17, 1, FontType::ANTIALIASING_EDGE_16X16);
		this->centerRatioX = 0.5;
		this->centerRatioY = 0.5;

		auto text = std::wstring(L"STOP PLAYING");
		NPTextViewParam text_view_param = NPTextViewParam(text, font, GetColor(255, 255, 255));
		DrawableInitParam drawableParam = DrawableInitParam(Cordinate(text_center_x, text_center_y), CenterRatio(0.5, 0.5));

		text_view = unique_ptr<NPTextView>(new NPTextView(this, c, text_view_param, drawableParam));

	}

	void setRatio(double ratio) {
		gauge_ratio = ratio;
		if (gauge_ratio > 1)gauge_ratio = 1;
		if (gauge_ratio < 0)gauge_ratio = 0;
	}

	virtual void beforeDrawProcess(int drawScreen) override {
		text_view.get()->draw();



		//枠
		DrawBoxWithLine(
			box_center_x - box_width/2, 
			box_center_y - box_height/2, 
			box_center_x + box_width / 2,
			box_center_y + box_height / 2,
			GetColor(255, 255, 255),
			0
		);

		//ゲージ
		DrawBoxWithLine(
			box_center_x - box_width / 2,
			box_center_y - box_height / 2,
			box_center_x - box_width / 2 + gauge_ratio*box_width,
			box_center_y + box_height / 2,
			GetColor(255, 255, 255)
		);


	};

};

