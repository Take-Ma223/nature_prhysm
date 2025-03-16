#pragma once

#include "Image.h"
#include "TextView.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>
#include "CONSTANT_VALUE.h"
#include "NPTextView.h"

using namespace std;

/// <summary>
/// リトライ方法を示すView
/// </summary>
class RetryView : public View
{
	int sizeX = 640;
	int sizeY = 100;

	unique_ptr<TextView> text;
	vector<unique_ptr<Image>> button;
	vector<unique_ptr<Image>> flash;


public:
	RetryView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(sizeX, sizeY));
		alpha.value = 0;


		FontInfo font = FontInfo(c->getConfig()->BaseFont, 26, 10, FontType::ANTIALIASING_EDGE_16X16, 2);
		TextViewParam textParam = TextViewParam(L"Press to Retry → ", font, GetColor(255, 255, 255));
		DrawableInitParam drawableParam = DrawableInitParam(Cordinate(165, 50), CenterRatio(0, 0.333));
		text = unique_ptr<TextView>(new TextView(this, context, textParam, drawableParam));

		button.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_R.png").c_str()))
			)
		);
		button.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_G.png").c_str()))
			)
		);
		button.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_B.png").c_str()))
			)
		);

		flash.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_press.png").c_str()))
			)
		);
		flash.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_press.png").c_str()))
			)
		);
		flash.push_back(
			unique_ptr<Image>(
				new Image(c, c->getAsset()->img(wstring(L"img/button_press.png").c_str()))
			)
		);

		addDrawable(text.get());
		for (int i = 0; i < 3; i++) {
			button[i]->X.value = 450;
			button[i]->Y.value = 64 - i * 32;

			flash[i]->X.value = 450;
			flash[i]->Y.value = 64 - i * 32;

			addDrawable(button[i].get());
			addDrawable(flash[i].get());
		}

	}

	/// <summary>
	/// 各ボタンの押下状態を反映する
	/// </summary>
	void updateFlashState(int* key, int button[3][4]) {
		const int flashVal = 255;

		bool state[3] = { false,false,false };

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				int push = key[button[2-i][j]];
				state[i] = state[i] || (push >= 1);
			}
		}

		for (int i = 0; i < 3; i++) {
			flash[i]->alpha.value = state[i] ? flashVal : 0;
		}
	}

	virtual void beforeDrawProcess(int drawScreen) override {

	};

	/// <summary>
	/// 表示と非表示
	/// </summary>
	void show() {
		alpha.clearEvent();
		alpha.eChange(Point(0), Point(255), Converter(Linear), 0, 1500);
		alpha.eChange(Point(255), Point(0), Converter(Linear), 3400, 3900);
		alpha.play();
	}

	/// <summary>
	/// 即座に表示
	/// </summary>
	void showImmediately() {
		alpha.clearEvent();
		alpha.eChange(Point(0), Point(255), Converter(Linear), 0, 0);
		alpha.play();
	}

	///// <summary>
	///// 非表示
	///// </summary>
	//void hide() {
	//	alpha.clearEvent();
	//	alpha.eChange(Point(255), Point(0), Converter(Linear), 0, 500);
	//	alpha.play();
	//}







};
