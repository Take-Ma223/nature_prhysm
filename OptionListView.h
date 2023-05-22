#pragma once

#include "Image.h"
#include "TextView.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>

using namespace std;

/// <summary>
/// オプションリストView
/// </summary>
class OptionListView : public View
{
	int sizeX = 320;
	int sizeY = 720;

	int selectingRow = 0;//ビューで選択しているオプションの番号

	int rowDuration = 96;

	vector<unique_ptr<TextView>> detailText;

	wstring themeStr1;
	wstring themeStr2;

	unique_ptr<Image> backGround;
	vector<unique_ptr<Image>> listFrame = vector<unique_ptr<Image>>(7);
	vector<unique_ptr<TextView>> listOptionName = vector<unique_ptr<TextView>>(7);
	vector<unique_ptr<TextView>> listItemName = vector<unique_ptr<TextView>>(7);

	FontInfo fontOptionName = FontInfo(wstring(L"メイリオ"), 17, 1, FontType::ANTIALIASING_EDGE_16X16);
	FontInfo fontItemName = FontInfo(wstring(L"メイリオ"), 25, 4, FontType::ANTIALIASING_EDGE_16X16);

	Option* option;
public:

	OptionListView::OptionListView(Option* op, ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		option = op;

		makeScreen(Size(sizeX, sizeY));
		alpha.value = 255;

		themeStr1 = wstring(L"img/themes/");
		themeStr2 = wstring(option->theme[option->op.theme]);

		backGround = unique_ptr<Image>(new Image(c, c->getAsset()->img((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str())));
		addDrawable(backGround.get());


		for (int i = 0; i < 7; i++) {
			int optionIndex = getIndex(i - 3, option->OPTION_NUM);

			DrawableInitParam drawableParam = DrawableInitParam(Cordinate(160, getYPos(i)), CenterRatio(0.5, 0.5));
			listFrame[i] = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/option_banner.png"), drawableParam));
			addDrawable(listFrame[i].get());
		


			TextViewParam textViewParam = TextViewParam(option->OptionName[optionIndex], fontOptionName, GetColor(255, 255, 255));
			drawableParam = DrawableInitParam(Cordinate(50, getYPos(i) - 30), CenterRatio(0, 0.5));
			listOptionName[i] = unique_ptr<TextView>(new TextView(this, context, textViewParam, drawableParam));
			addDrawable(listOptionName[i].get());


			textViewParam = TextViewParam(
				option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
				fontItemName, GetColor(255, 255, 255));
			drawableParam = DrawableInitParam(Cordinate(162, getYPos(i)), CenterRatio(0.5, 0.5));
			listItemName[i] = unique_ptr<TextView>(new TextView(this, context, textViewParam, drawableParam));
			addDrawable(listItemName[i].get());


		}

	}

	int getYPos(int ind) {
		return (ind - 3) * rowDuration + 360;
	}

	
	int getIndex(int ind, int indexLength) {
		int result = ind;
		while (result < 0) {
			result += indexLength;
		}
		while (result >= indexLength) {
			result -= indexLength;
		}
		return result;
	}

	/// <summary>
	/// 今のオプション選択値で項目の更新
	/// </summary>
	void update(int selectingOption) {
		for (int i = 0; i < 7; i++) {
			int optionIndex = getIndex(i - 3 + selectingOption, option->OPTION_NUM);
		
			listOptionName[i].get()->setText(
				TextViewParam(
					option->OptionName[optionIndex],
					fontOptionName, GetColor(255, 255, 255))
			);

			listItemName[i].get()->setText(
				TextViewParam(
					option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
					fontItemName, GetColor(255, 255, 255))
			);
		
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