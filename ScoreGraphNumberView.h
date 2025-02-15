#pragma once
#include "View.h"
#include "NumberView.h"
#include <memory>
#include "Image.h"

class ScoreGraphNumberView : public View
{
public:
	ScoreGraphNumberView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		alpha.value = 255;
		makeScreen(Size(size_x, size_y));

		//ActivityContext* c, 
		NumberImageHandles number_plus_handle = NumberImageHandles(
			c->getAsset()->imgs(L"img/score_graph/score_graph_number_plus.png", 10, 10, 1, digit_width, digit_width)
		);
		NumberImageHandles number_minus_handle = NumberImageHandles(
			c->getAsset()->imgs(L"img/score_graph/score_graph_number_minus.png", 10, 10, 1, digit_width, digit_width)
		);

		NumberViewFormat number_plus_format = NumberViewFormat(number_plus_handle, 5, 0, digit_interval, NumberViewShowType::Center, 0);
		NumberViewFormat number_minus_format = NumberViewFormat(number_minus_handle, 5, 0, digit_interval, NumberViewShowType::Center, 0);

		DrawableInitParam number_init_param = DrawableInitParam(Cordinate(size_x/2, size_y/2), CenterRatio(0.5,0.5));

		//符合表示の領域のため右に半分ずらす
		number_init_param.cordinate.x += (digit_interval_sign)/2;

		number_plus = unique_ptr<NumberView>(new NumberView(c, number_plus_format, number_init_param));
		number_minus = unique_ptr<NumberView>(new NumberView(c, number_minus_format, number_init_param));

		sign_plus = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/score_graph/score_graph_sign_plus.png"), number_init_param));
		sign_minus = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/score_graph/score_graph_sign_minus.png"), number_init_param));


		addDrawable(sign_plus.get());
		addDrawable(sign_minus.get());
		addDrawable(number_plus.get());
		addDrawable(number_minus.get());


		setValue(0);
	}

	/// <summary>
	/// スコアのセット
	/// </summary>
	/// <param name="v">スコア数値</param>
	void setValue(const int v) {
		int value = v;
		if (value > 99999)value = 99999;
		if (value < -99999)value = -99999;

		if (value >= 0) {
			setStatePlus();
		}
		else {
			setStateMinus();
		}

		int abs_value = std::abs(value);

		number_plus.get()->setValue(abs_value);
		number_minus.get()->setValue(abs_value);

		int digits = countDigits(value);
		sign_plus.get()->X.value = (double)size_x / 2 - (double)digits * digit_interval_sign / 2;
		sign_minus.get()->X.value = (double)size_x / 2 - (double)digits * digit_interval_sign / 2;


	}
	void beforeDrawProcess(int drawScreen) override {

	}


private:
	//最大5桁と符合表示用で6桁分の幅になる
	const int digit_width = 18;

	const int digit_interval = -4;

	const int digit_interval_sign = digit_width + digit_interval;

	const int size_x = digit_width * 6;
	const int size_y = digit_width;

	std::unique_ptr<NumberView> number_plus;
	std::unique_ptr<NumberView> number_minus;
	std::unique_ptr<Image> sign_plus;
	std::unique_ptr<Image> sign_minus;

	void setStatePlus() {
		number_plus.get()->alpha.value = 255;
		number_minus.get()->alpha.value = 0;
		sign_plus.get()->alpha.value = 255;
		sign_minus.get()->alpha.value = 0;
	}

	void setStateMinus() {
		number_plus.get()->alpha.value = 0;
		number_minus.get()->alpha.value = 255;
		sign_plus.get()->alpha.value = 0;
		sign_minus.get()->alpha.value = 255;
	}



	int countDigits(int n) {
		if (n == 0) return 1;
		return std::log10(std::abs(n)) + 1;
	}





};

