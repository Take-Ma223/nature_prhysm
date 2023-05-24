#pragma once

#include "Image.h"
#include "TextView.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>
#include "NPTextView.h"

using namespace std;

/// <summary>
/// オプションリストView
/// </summary>
class OptionListView : public View
{
public:
	OptionListView::OptionListView(Option* op, ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		initXAnimation();
		moveRatio = TransValue(c);

		option = op;

		makeScreen(Size(sizeX, sizeY));
		alpha.value = 255;

		themeStr1 = wstring(L"img/themes/");
		themeStr2 = wstring(option->theme[option->op.theme]);

		backGround = unique_ptr<Image>(new Image(c, c->getAsset()->img((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str())));
		addDrawable(backGround.get());


		for (int i = 0; i < rowCount; i++) {
			int optionIndex = getIndex(i - rowCenter, option->OPTION_NUM);
			DrawableInitParam drawableParam = DrawableInitParam(Cordinate(160, getYPos(i)), CenterRatio(0.5, 0.5));
			listFrame[i] = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/option_banner.png"), drawableParam));
			if (i != rowCenter)listFrame[i].get()->alpha.value = notSelectedFrameAlpha;
			addDrawable(listFrame[i].get());
		


			TextViewParam textViewParam = TextViewParam(option->OptionName[optionIndex], fontOptionName, GetColor(255, 255, 255));
			drawableParam = DrawableInitParam(Cordinate(40, getYPos(i) + optionNameY), CenterRatio(0, 0.5));
			listOptionName[i] = unique_ptr<TextView>(new TextView(this, context, textViewParam, drawableParam));
			if (i != rowCenter)listOptionName[i].get()->alpha.value = notSelectedFrameAlpha;
			addDrawable(listOptionName[i].get());


			NPTextViewParam itemNameTextViewParam = NPTextViewParam(
				option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
				fontItemName, GetColor(255, 255, 255), GetColor(0, 0, 0));
			drawableParam = DrawableInitParam(Cordinate(162, getYPos(i)), CenterRatio(0.5, 0.5));
			listItemName[i] = unique_ptr<NPTextView>(new NPTextView(this, context, itemNameTextViewParam, drawableParam));
			if (i != rowCenter)listItemName[i].get()->alpha.value = notSelectedFrameAlpha;
			addDrawable(listItemName[i].get());


		}

	}

	

	/// <summary>
	/// 今のオプション選択値で項目の更新
	/// </summary>
	void updateListView(int select) {
		selectingOption = select;

		for (int i = 0; i < rowCount; i++) {
			int optionIndex = getIndex(i - rowCenter + selectingOption, option->OPTION_NUM);
		
			listOptionName[i].get()->setText(
				TextViewParam(
					option->OptionName[optionIndex],
					fontOptionName, GetColor(255, 255, 255))
			);

			listItemName[i].get()->setText(
				NPTextViewParam(
					option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
					fontItemName, GetColor(255, 255, 255), GetColor(0, 0, 0))
			);
		
		}

		updateSkillTestModeView();

	}

	void setSkillTestMode(bool isTrue) {
		isSkillTestMode = isTrue;
		updateSkillTestModeView();
	}


	/// <summary>
	/// 表示
	/// </summary>
	void show() {
		X.setReverse(false);
	}

	/// <summary>
	/// 非表示
	/// </summary>
	void hide() {
		X.setReverse(true);
	}


	void moveToSelectUp() {
		move(true);
	}

	void moveToSelectDown() {
		move(false);
	}

	

private:
	int sizeX = 320;
	int sizeY = 720;

	int selectingRow = 0;//ビューで選択しているオプションの番号

	const int rowDuration = 90;
	const int rowCount = 19;//必ず奇数にする
	const int rowCenter = rowCount / 2;

	const int notSelectedFrameAlpha = 128;


	const int optionNameY = -35;

	bool isSkillTestMode = false;
	const int defaultBrightness = 255;
	const int skillTestModeNotAvailableFrameBrightness = 128;

	wstring themeStr1;
	wstring themeStr2;

	unique_ptr<Image> backGround;
	vector<unique_ptr<Image>> listFrame = vector<unique_ptr<Image>>(rowCount);
	vector<unique_ptr<TextView>> listOptionName = vector<unique_ptr<TextView>>(rowCount);
	vector<unique_ptr<NPTextView>> listItemName = vector<unique_ptr<NPTextView>>(rowCount);

	FontInfo fontOptionName = FontInfo(wstring(L"メイリオ"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);
	FontInfo fontItemName = FontInfo(wstring(L"メイリオ"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);

	Option* option;

	TransValue moveRatio;

	int selectingOption = 0;


	int getYPos(int ind) {
		return (ind - rowCenter) * rowDuration + 360;
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


	void updateSkillTestModeView() {
		for (int i = 0; i < rowCount; i++) {
			int optionIndex = getIndex(i - rowCenter + selectingOption, option->OPTION_NUM);

			if (!isSkillTestMode) {
				setBrightnessList(i, defaultBrightness);
				continue;
			}

			bool isGauge = optionIndex == (int)OptionItem::Name::GAUGE;
			bool isLaneRandom = (optionIndex == (int)OptionItem::Name::LANE) && (*option->ArrayValue[optionIndex] == (int)OptionItem::Lane::RANDOM);
			bool isLaneSuperRandom = (optionIndex == (int)OptionItem::Name::LANE) && (*option->ArrayValue[optionIndex] == (int)OptionItem::Lane::SUPER_RAND);
			bool isColor = optionIndex == (int)OptionItem::Name::COLOR;
			bool isAvailable = !isGauge && !isLaneRandom && !isLaneSuperRandom && !isColor;

			if (isAvailable) {
				setBrightnessList(i, defaultBrightness);
				continue;
			}
			setBrightnessList(i, skillTestModeNotAvailableFrameBrightness);
		}
	}

	void setBrightnessList(int row, int brightness) {
		listFrame[row].get()->brightnessR.value = brightness;
		listFrame[row].get()->brightnessG.value = brightness;
		listFrame[row].get()->brightnessB.value = brightness;

		listItemName[row].get()->brightnessR.value = brightness;
		listItemName[row].get()->brightnessG.value = brightness;
		listItemName[row].get()->brightnessB.value = brightness;
	}


	virtual void beforeDrawProcess(int drawScreen) override {
		moveRatio.process();

		for (int i = 0; i < rowCount; i++) {
			listFrame[i].get()->Y.value = getYPos(i) + moveRatio.value;
			listOptionName[i].get()->Y.value = getYPos(i) + moveRatio.value + optionNameY;
			listItemName[i].get()->Y.value = getYPos(i) + moveRatio.value;
		}
	};


	void initXAnimation() {
		X.clearEvent();
		X.eChange(Point(-320), Point(0), Converter(QuarterSine), 0, 350);
		X.setReverse(true);
		X.play();

	}


	void move(bool isUp) {
		double expBase = 2;
		int time = 300;
		moveRatio.clearEvent();
		if (isUp) moveRatio.value -= rowDuration;
		else moveRatio.value += rowDuration;

		moveRatio.eChangeTo(Point(0), Converter(ExponentialSlider, expBase), 0, time);
		moveRatio.play();
	}



};