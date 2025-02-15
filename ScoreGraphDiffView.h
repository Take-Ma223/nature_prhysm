#pragma once
#include "View.h"
#include <memory>
#include "ScoreGraphNumberView.h"

class ScoreGraphDiffView :
    public View
{

public:
	ScoreGraphDiffView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(size_x, size_y));

		int number_width = size_x / 4;

		DrawableInitParam init_param_high_score(Cordinate(number_width * 1 + number_width / 2, size_y / 2), CenterRatio(0.5, 0.5));
		DrawableInitParam init_param_target_score_1(Cordinate(number_width * 2 + number_width / 2, size_y / 2), CenterRatio(0.5, 0.5));
		DrawableInitParam init_param_target_score_2(Cordinate(number_width * 3 + number_width / 2, size_y / 2), CenterRatio(0.5, 0.5));

		high_score = unique_ptr<ScoreGraphNumberView>(new ScoreGraphNumberView(c, init_param_high_score));
		target_score_1 = unique_ptr<ScoreGraphNumberView>(new ScoreGraphNumberView(c, init_param_target_score_1));
		target_score_2 = unique_ptr<ScoreGraphNumberView>(new ScoreGraphNumberView(c, init_param_target_score_2));

		addDrawable(high_score.get());
		addDrawable(target_score_1.get());
		addDrawable(target_score_2.get());
		setValue(0, 0, 0, 0);
	}

	void beforeDrawProcess(int drawScreen) override {

	}

	void setValue(double score, double high_score, double target_score_1, double target_score_2) {
		ScoreGraphDiffView::high_score.get()->setValue(int(score - high_score + 0.5));
		ScoreGraphDiffView::target_score_1.get()->setValue(int(score - target_score_1 + 0.5));
		ScoreGraphDiffView::target_score_2.get()->setValue(int(score - target_score_2 + 0.5));
	}

private:
	std::unique_ptr<ScoreGraphNumberView> high_score;
	std::unique_ptr<ScoreGraphNumberView> target_score_1;
	std::unique_ptr<ScoreGraphNumberView> target_score_2;

	const int size_x = 320;
	const int size_y = 18;

};

