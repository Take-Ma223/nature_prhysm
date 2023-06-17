#pragma once

#include "Image.h"
#include "TextView.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>
#include "NPTextView.h"
#include "NPGradTextView.h"

using namespace std;

/// <summary>
/// オプションリストView
/// </summary>
class OptionListView : public View
{
public:
	OptionListView::OptionListView(Option* op, ActivityContext* context, DrawableInitParam param = DrawableInitParam()) : View(context, param)
	{
		c = context;
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
			int optionIndex = getOptionIndex(i - rowCenterIndex);
			DrawableInitParam drawableParam = DrawableInitParam(Cordinate(160, getYPos(i)), CenterRatio(0.5, 0.5));
			listFrame[i] = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/option_list/green.png"), drawableParam));
			if (i != rowCenterIndex)listFrame[i].get()->alpha.value = notSelectedFrameAlpha;
			updateFrame(i, optionIndex);
			addDrawable(listFrame[i].get());
		


			TextViewParam textViewParam = TextViewParam(option->OptionName[optionIndex], fontOptionName, optionNameColor);
			drawableParam = DrawableInitParam(Cordinate(40, getYPos(i) + optionNameY), CenterRatio(0, 0.5));
			listOptionName[i] = unique_ptr<TextView>(new TextView(this, c, textViewParam, drawableParam));
			if (i != rowCenterIndex)listOptionName[i].get()->alpha.value = notSelectedFrameAlpha;
			addDrawable(listOptionName[i].get());


			NPTextViewParam itemNameTextViewParam = NPTextViewParam(
				option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
				fontItemName, itemNameColor, itemNameShadowColor);
			drawableParam = DrawableInitParam(Cordinate(162, getYPos(i)), CenterRatio(0.5, 0.5));
			listItemName[i] = unique_ptr<NPGradTextView>(new NPGradTextView(this, context, itemNameTextViewParam, drawableParam));
			if (i != rowCenterIndex)listItemName[i].get()->alpha.value = notSelectedFrameAlpha;
			addDrawable(listItemName[i].get());


		}

		title = unique_ptr<Image>(new Image(c, c->getAsset()->img(wstring(L"img/option_list/cover_title_option.png").c_str())));
		addDrawable(title.get());

		arrowLeft = unique_ptr<Image>(new Image(c, c->getAsset()->img(wstring(L"img/option_list/arrow_left_release.png").c_str())));
		arrowLeft.get()->Y.value = 360;
		arrowLeft.get()->centerRatioY = 0.5;
		addDrawable(arrowLeft.get());

		arrowRight = unique_ptr<Image>(new Image(c, c->getAsset()->img(wstring(L"img/option_list/arrow_right_release.png").c_str())));
		arrowRight.get()->Y.value = 360;
		arrowRight.get()->centerRatioY = 0.5;
		addDrawable(arrowRight.get());
	}


	/// <summary>
	/// 指定したオプション番号でリストの指定した行一つを更新
	/// </summary>
	/// <param name="listIndex">内部リストインデックス</param>
	/// <param name="optionIndex">オプション番号</param>
	void updateItem(int listIndex, int optionIndex)
	{
		updateFrame(listIndex, optionIndex);
		updateOptionName(listIndex, optionIndex);
		updateItemName(listIndex, optionIndex);
		updateBrightness(listIndex, optionIndex);
	}

	


	/// <summary>
	/// 今選んでいるオプション選択値の更新
	/// </summary>
	void updateSelectedOptionItem() {
		const int targetIndex = getListIndex(rowCenterIndex + rotationIndex);

		updateItemName(targetIndex, selectingOption);
		updateBrightness(targetIndex, selectingOption);
	}


	void setSkillTestMode(bool isTrue) {
		isSkillTestMode = isTrue;
		for (int i = 0; i < rowCount; i++) {
			int listIndex = getListIndex(i + rotationIndex);
			int optionIndex = getOptionIndex(i - rowCenterIndex + selectingOption);
			updateBrightness(listIndex, optionIndex);

		}
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
		rotationIndex = getListIndex(rotationIndex - 1);//上を選ぶとリストは下に動くため注意
	}

	void moveToSelectDown() {
		move(false);
		rotationIndex = getListIndex(rotationIndex + 1);
	}

	void setCoverImage() {
		themeStr1 = wstring(L"img/themes/");
		themeStr2 = wstring(option->theme[option->op.theme]);
		backGround.get()->setImage(c->getAsset()->img((themeStr1 + themeStr2 + wstring(L"/cover_option.png")).c_str()));
	}

	void pushLeftArrow() {
		arrowLeft.get()->setImage(c->getAsset()->img(L"img/option_list/arrow_left_push.png"));
	}

	void releaseLeftArrow() {
		arrowLeft.get()->setImage(c->getAsset()->img(L"img/option_list/arrow_left_release.png"));
	}

	void pushRightArrow() {
		arrowRight.get()->setImage(c->getAsset()->img(L"img/option_list/arrow_right_push.png"));
	}

	void releaseRightArrow() {
		arrowRight.get()->setImage(c->getAsset()->img(L"img/option_list/arrow_right_release.png"));
	}


private:
	ActivityContext* c;

	static const int sizeX = 320;
	static const int sizeY = 720;


	static const int rowDuration = 90;
	static const int rowCount = 19;//必ず奇数にする
	static const int rowCenterIndex = rowCount / 2;
	int selectingOption = 0;//ビューで選択しているオプションの番号

	static const int selectedFrameAlpha = 255;
	static const int notSelectedFrameAlpha = 128;

	int rotationIndex = 0;//リストがどっちの方向に何回動いたかを表す(+:上方向 -:下方向)(値域:0~+(rowCount-1))



	static const int optionNameY = -35;

	bool isSkillTestMode = false;
	static const int defaultBrightness = 255;
	static const int skillTestModeNotAvailableFrameBrightness = 128;

	const int optionNameColor = GetColor(255, 255, 255);
	const int itemNameColor = GetColor(160,160,160);
	const int itemNameShadowColor = GetColor(0,0,0);

	wstring themeStr1;
	wstring themeStr2;

	unique_ptr<Image> backGround;
	vector<unique_ptr<Image>> listFrame = vector<unique_ptr<Image>>(rowCount);
	vector<unique_ptr<TextView>> listOptionName = vector<unique_ptr<TextView>>(rowCount);
	vector<unique_ptr<NPGradTextView>> listItemName = vector<unique_ptr<NPGradTextView>>(rowCount);

	unique_ptr<Image> title;

	unique_ptr<Image> arrowLeft;
	unique_ptr<Image> arrowRight;

	FontInfo fontOptionName = FontInfo(wstring(L"メイリオ"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);
	FontInfo fontItemName = FontInfo(wstring(L"メイリオ"), 25, 9, FontType::ANTIALIASING_EDGE_16X16);

	Option* option;

	TransValue moveRatio;

	int getYPos(int ind) {
		return (ind - rowCenterIndex) * rowDuration + 360;
	}


	int getOptionIndex(int ind) {
		const int length = option->OPTION_NUM;
		return getRotationCount(ind, length);
	}

	int getListIndex(int ind) {
		const int length = rowCount;
		return getRotationCount(ind, length);
	}

	int getRotationCount(const int& ind, const int& length)
	{
		int result = ind;
		while (result < 0) {
			result += length;
		}
		while (result >= length) {
			result -= length;
		}
		return result;
	}

	void updateFrame(int listIndex, int optionIndex)
	{
		std::unordered_map<OptionItem::BannerColor, std::wstring> mapping = {
			{OptionItem::BannerColor::RED,wstring(L"img/option_list/red.png")},
			{OptionItem::BannerColor::GREEN,wstring(L"img/option_list/green.png")},
			{OptionItem::BannerColor::BLUE,wstring(L"img/option_list/blue.png")},
			{OptionItem::BannerColor::CYAN,wstring(L"img/option_list/cyan.png")},
			{OptionItem::BannerColor::MAGENTA,wstring(L"img/option_list/magenta.png")},
			{OptionItem::BannerColor::YELLOW,wstring(L"img/option_list/yellow.png")},
			{OptionItem::BannerColor::WHITE,wstring(L"img/option_list/white.png")},
			{OptionItem::BannerColor::BLACK,wstring(L"img/option_list/black.png")},
		};

		listFrame[listIndex].get()->setImage(c->getAsset()->img(mapping[option->bannerColor[optionIndex]].c_str()));

	}

	void updateItemName(int listIndex, int optionIndex)
	{
		listItemName[listIndex].get()->setText(
			NPTextViewParam(
				option->ArrayOptionKindName[optionIndex][*option->ArrayValue[optionIndex]],
				fontItemName, itemNameColor, itemNameShadowColor)
		);
	}

	void updateOptionName(int listIndex, int optionIndex)
	{
		listOptionName[listIndex].get()->setText(
			TextViewParam(
				option->OptionName[optionIndex],
				fontOptionName, optionNameColor)
		);
	}

	void updateBrightness(int listIndex, int optionIndex) {
		if (!isSkillTestMode) {
			setBrightnessList(listIndex, defaultBrightness);
			return;
		}

		bool isGauge = optionIndex == (int)OptionItem::Name::GAUGE;
		bool isLaneRandom = (optionIndex == (int)OptionItem::Name::LANE) && (*option->ArrayValue[optionIndex] == (int)OptionItem::Lane::RANDOM);
		bool isLaneSuperRandom = (optionIndex == (int)OptionItem::Name::LANE) && (*option->ArrayValue[optionIndex] == (int)OptionItem::Lane::SUPER_RAND);
		bool isColor = optionIndex == (int)OptionItem::Name::COLOR;
		bool isAvailable = !isGauge && !isLaneRandom && !isLaneSuperRandom && !isColor;

		if (isAvailable) {
			setBrightnessList(listIndex, defaultBrightness);
			return;
		}
		setBrightnessList(listIndex, skillTestModeNotAvailableFrameBrightness);
	
	}

	void setBrightnessList(int listIndex, int brightness) {
		listFrame[listIndex].get()->brightnessR.value = brightness;
		listFrame[listIndex].get()->brightnessG.value = brightness;
		listFrame[listIndex].get()->brightnessB.value = brightness;

		listItemName[listIndex].get()->brightnessR.value = brightness;
		listItemName[listIndex].get()->brightnessG.value = brightness;
		listItemName[listIndex].get()->brightnessB.value = brightness;
	}


	virtual void beforeDrawProcess(int drawScreen) override {
		moveRatio.process();

		for (int i = 0; i < rowCount; i++) {
			int listIndex = getListIndex(i + rotationIndex);

			listFrame[listIndex].get()->Y.value = getYPos(i) + moveRatio.value;
			listOptionName[listIndex].get()->Y.value = getYPos(i) + moveRatio.value + optionNameY;
			listItemName[listIndex].get()->Y.value = getYPos(i) + moveRatio.value;
		}
	};


	void initXAnimation() {
		X.clearEvent();
		X.eChange(Point(-320), Point(0), Converter(QuarterSine), 0, 350);
		X.setReverse(true);
		X.play();
	}

	void move(bool isSelectingUp) {
		setEntireMoveAnimation(isSelectingUp);
		setSelectingBrightAnimation(isSelectingUp);
		iterateSelectingOption(isSelectingUp);
	}

	void setEntireMoveAnimation(bool isSelectingUp)
	{
		setEntireOffset(isSelectingUp);

		double expBase = 2;
		int time = 300;
		moveRatio.clearEvent();
		if (isSelectingUp) moveRatio.value -= rowDuration;
		else moveRatio.value += rowDuration;

		moveRatio.eChangeTo(Point(0), Converter(ExponentialSlider, expBase), 0, time);
		moveRatio.play();
	}

	void setEntireOffset(bool isSelectingUp)
	{
		int upperListIndex = getListIndex(0 + rotationIndex);//一番上
		int downerListIndex = getListIndex(rowCount - 1 + rotationIndex);//一番下

		int bornIndex;//次に生成されるべき位置を持つインデックス
		if (isSelectingUp)bornIndex = upperListIndex;
		else bornIndex = downerListIndex;

		int moveIndex;//bornIndexの位置に移動するインデックス
		if (isSelectingUp)moveIndex = downerListIndex;
		else moveIndex = upperListIndex;

		int frameBornY = listFrame[bornIndex].get()->Y.value;
		int optionNameBornY = listOptionName[bornIndex].get()->Y.value;
		int itemNameBornY = listItemName[bornIndex].get()->Y.value;

		int duration;
		if (isSelectingUp)duration = rowDuration;
		else duration = -rowDuration;

		for (int i = 0; i < rowCount; i++) {
			listFrame[i].get()->Y.value += duration;
			listOptionName[i].get()->Y.value += duration;
			listItemName[i].get()->Y.value += duration;
		}

		listFrame[moveIndex].get()->Y.value = frameBornY;
		listOptionName[moveIndex].get()->Y.value = optionNameBornY;
		listItemName[moveIndex].get()->Y.value = itemNameBornY;

		int bornOptionIndex;
		if (isSelectingUp)bornOptionIndex = getOptionIndex(selectingOption - rowCenterIndex - 1);
		else bornOptionIndex = getOptionIndex(selectingOption + rowCenterIndex + 1);

		updateItem(moveIndex, bornOptionIndex);
	}

	void setSelectingBrightAnimation(bool isSelectingUp)
	{
		int animeDuration = 100;

		int next = isSelectingUp ? -1 : 1;
		int targetIndex = getListIndex(rowCenterIndex + rotationIndex);
		int nextIndex = getListIndex(targetIndex + next);

		int centerAlphaValue = listFrame[targetIndex].get()->alpha.value;
		int nextAlphaValue = listFrame[nextIndex].get()->alpha.value;

		listFrame[targetIndex].get()->alpha.clearEvent();
		listOptionName[targetIndex].get()->alpha.clearEvent();
		listItemName[targetIndex].get()->alpha.clearEvent();

		listFrame[targetIndex].get()->alpha.eChange(Point(centerAlphaValue), Point(notSelectedFrameAlpha), Converter(Linear), 0, animeDuration);
		listOptionName[targetIndex].get()->alpha.eChange(Point(centerAlphaValue), Point(notSelectedFrameAlpha), Converter(Linear), 0, animeDuration);
		listItemName[targetIndex].get()->alpha.eChange(Point(centerAlphaValue), Point(notSelectedFrameAlpha), Converter(Linear), 0, animeDuration);

		listFrame[targetIndex].get()->alpha.play();
		listOptionName[targetIndex].get()->alpha.play();
		listItemName[targetIndex].get()->alpha.play();

		listFrame[nextIndex].get()->alpha.clearEvent();
		listOptionName[nextIndex].get()->alpha.clearEvent();
		listItemName[nextIndex].get()->alpha.clearEvent();

		listFrame[nextIndex].get()->alpha.eChange(Point(nextAlphaValue), Point(selectedFrameAlpha), Converter(Linear), 0, animeDuration);
		listOptionName[nextIndex].get()->alpha.eChange(Point(nextAlphaValue), Point(selectedFrameAlpha), Converter(Linear), 0, animeDuration);
		listItemName[nextIndex].get()->alpha.eChange(Point(nextAlphaValue), Point(selectedFrameAlpha), Converter(Linear), 0, animeDuration);

		listFrame[nextIndex].get()->alpha.play();
		listOptionName[nextIndex].get()->alpha.play();
		listItemName[nextIndex].get()->alpha.play();
	}

	void iterateSelectingOption(bool isUp)
	{
		if (isUp)selectingOption = getOptionIndex(selectingOption - 1);
		else selectingOption = getOptionIndex(selectingOption + 1);
	}





};