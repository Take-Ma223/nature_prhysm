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
/// �I�v�V�������X�gView
/// </summary>
class OptionListView : public View
{
	int sizeX = 320;
	int sizeY = 720;

	int selectingRow = 0;//�r���[�őI�����Ă���I�v�V�����̔ԍ�

	const int rowDuration = 90;
	const int rowCount = 19;//�K����ɂ���
	const int rowCenter = rowCount / 2;

	const int notSelectedFrameAlpha = 128;

	const int optionNameY = -35;

	wstring themeStr1;
	wstring themeStr2;

	unique_ptr<Image> backGround;
	vector<unique_ptr<Image>> listFrame = vector<unique_ptr<Image>>(rowCount);
	vector<unique_ptr<TextView>> listOptionName = vector<unique_ptr<TextView>>(rowCount);
	vector<unique_ptr<NPTextView>> listItemName = vector<unique_ptr<NPTextView>>(rowCount);

	FontInfo fontOptionName = FontInfo(wstring(L"���C���I"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);
	FontInfo fontItemName = FontInfo(wstring(L"���C���I"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);

	Option* option;

	TransValue moveRatio;
public:

	OptionListView::OptionListView(Option* op, ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
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

	/// <summary>
	/// ���̃I�v�V�����I��l�ō��ڂ̍X�V
	/// </summary>
	void update(int selectingOption) {
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
	}

	virtual void beforeDrawProcess(int drawScreen) override {
		moveRatio.process();

		for (int i = 0; i < rowCount; i++) {
			listFrame[i].get()->Y.value = getYPos(i) + moveRatio.value;
			listOptionName[i].get()->Y.value = getYPos(i) + moveRatio.value + optionNameY;
			listItemName[i].get()->Y.value = getYPos(i) + moveRatio.value;
		}
	};

	/// <summary>
	/// �\��
	/// </summary>
	void show() {
		alpha.clearEvent();
		alpha.eChange(Point(0), Point(255), Converter(Linear), 0, 900);
		alpha.play();
	}

	/// <summary>
	/// ��\��
	/// </summary>
	void hide() {
		alpha.clearEvent();
		alpha.eChange(Point(255), Point(0), Converter(Linear), 0, 900);
		alpha.play();
	}


	void moveToSelectUp() {
		move(true);
	}

	void moveToSelectDown() {
		move(false);
	}

	void move(bool isUp) {
		double expBase = 1.5;
		int time = 600;
		moveRatio.clearEvent();
		if (isUp) moveRatio.value -= rowDuration;
		else moveRatio.value += rowDuration;

		//�I�𒆂̃��X�g�̒��S����̗������傫�������������悤�Ɉړ����Ԃ����߂�
		//double distance = (abs(moveRatio.value) / rowDuration);//1~3���炢 4�ŉ�ʊO�ɂ���
		//time = time - distance*75;
		//if (time <= 0)time = 1;

		moveRatio.eChangeTo(Point(0), Converter(ExponentialSlider, expBase), 0, time);
		moveRatio.play();
	}



};