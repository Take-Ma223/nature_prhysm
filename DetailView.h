#pragma once

#include "Image.h"
#include "TextView.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>
#include "CONSTANT_VALUE.h"

using namespace std;

/// <summary>
/// 曲開始時の詳細のView
/// </summary>
class DetailView : public View
{
	int sizeX = 640;
	int sizeY = 240;
	int rowDuration = 17;

	vector<unique_ptr<TextView>> detailText;

public:
	DetailView::DetailView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(sizeX, sizeY));
		alpha.value = 0;
		
		font = FontInfo(wstring(c->getConfig()->BaseFont), 17, 1, FontType::ANTIALIASING_EDGE_16X16);
	}

	FontInfo font;
	//vector<TextView> detailText;

	/// <summary>
	/// 詳細テキストのセット
	/// </summary>
	/// <param name="text"></param>
	void setText(vector<wstring>& textArray) {
		removeDrawable();
		detailText.clear();

		int positionX = sizeX/2;
		int positionY = 15;

		for (auto text = textArray.begin(); text != textArray.end(); ++text, positionY += rowDuration) {
			TextViewParam param = TextViewParam(*text, font, GetColor(255, 255, 255));
			DrawableInitParam drawableParam = DrawableInitParam(Cordinate(positionX, positionY), CenterRatio(0.5, 0));


			detailText.push_back(unique_ptr<TextView>(new TextView(this, context, param, drawableParam)));

			
		}

		for (auto dText = detailText.begin(); dText != detailText.end(); ++dText) {
			addDrawable(dText->get());
		}

	}

	virtual void beforeDrawProcess(int drawScreen) override {

	};

	/// <summary>
	/// 表示
	/// </summary>
	void show() {
		alpha.clearEvent();
		alpha.eChange(Point(0), Point(255), Converter(Linear), 0, 900);
		alpha.play();
	}

	/// <summary>
	/// 非表示
	/// </summary>
	void hide() {
		alpha.clearEvent();
		alpha.eChange(Point(255), Point(0), Converter(Linear), 0, 900);
		alpha.play();
	}







};
