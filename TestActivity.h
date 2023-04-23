#pragma once
#include "Activity.h"
#include "Log.h"
#include "TestController.h"
#include "CoverView.h"
#include "TextView.h"
#include "NumberView.h"

/// <summary>
/// フレームワークのテスト用アクティビティ
/// </summary>
class TestActivity : public Activity
{
public:
	TestController controller;

	DrawableInitParam param = DrawableInitParam(Cordinate(100,100));
	Image img = Image(&activityContext, activityContext.getAsset()->img(L"img/A.png"), param);

	DrawableInitParam backGroundParam = DrawableInitParam(Cordinate(0, 0));
	Image backGround = Image(&activityContext, activityContext.getAsset()->img(L"img/bg.png"), backGroundParam);

	DrawableInitParam tvp1 = DrawableInitParam(Cordinate(0, 0));
	TextView testText1 = TextView(
		&activityContext,
		TextViewParam(wstring(L"曲名表示テストTEST"), FontInfo(wstring(L"メイリオ"), 28, 9, FontType::ANTIALIASING_EDGE), GetColor(255, 255, 255)),
		tvp1
	);


	NumberImageHandles numberHandles = NumberImageHandles(
		activityContext.getAsset()->imgs(L"img/SmallNumberGreen.png", 10, 10, 1, 25, 50),
		activityContext.getAsset()->img(L"img/decimal.png")
	);
	NumberViewFormat nvp1 = NumberViewFormat(numberHandles,6,2,-3,NumberViewShowType::Left,4);
	DrawableInitParam tvp2 = DrawableInitParam(Cordinate(0, 200));
	NumberView testNumber1 = NumberView(&activityContext,nvp1,tvp2);

	int testNumberValue = 0;

	int a = 0;

	DrawableInitParam param2 = DrawableInitParam(Cordinate(600, 300));

	Image buttonBB = Image(&activityContext, activityContext.getAsset()->img(L"img/button_BB.png"), param2);
	int SH_SHUTTER = LoadSoundMem(L"sound/shutter.wav");
	int SH_SHUTTER_SIGNAL = LoadSoundMem(L"sound/shutter_signal.wav");

	TestActivity(AppContext* ac) : Activity(ac) {

	}
	
	TestActivity(AppContext* ac, int input) : Activity(ac) {
		a = input;
		testNumber1.setCenterRatio(0, 0);
	}

	virtual void onCreate() override {
		Activity::onCreate();
		controller.setTestKeyTapHandler([this] {onTapSpace(); });

		controller.setKey1TapHandler([this] {buttonPlay(); });
		controller.setKey2TapHandler([this] {buttonStop(); });
		controller.setKey3TapHandler([this] {buttonResume(); });
		controller.setKey4TapHandler([this] {buttonReverse(); });
		controller.setKey5TapHandler([this] {buttonSpeed2(); });
		controller.setKey6TapHandler([this] {buttonSpeed05(); });
		controller.setKey7TapHandler([this] {buttonLoop(); });

		buttonAnimation();
	}

	virtual void onStart() override {
		Activity::onStart();
		//
		//Log(to_wstring(a));

		//if (a <= 99999) {
		//	auto act = new TestActivity(appContext, a + 1);
		//	activityContext.startActivity((Activity*)act);
		//}
		//finish();

	};
	
	virtual void buttonProcess() override {
		controller.process();
	}

	virtual void logic() override {
	
	}

	virtual void draw() override {
		backGround.draw();
		img.draw();
		testText1.draw();
		testNumber1.draw();
		buttonBB.draw();
	}

private:
	void onTapSpace() {
		img.alpha.value -= 10;
		testNumberValue++;
		testNumber1.setValue(testNumberValue);
		Log(L"キーをタップしました");
		printfDx(L"キーをタップしました");
		return;
	}

	void buttonAnimation() {
		buttonBB.clearAllEvent();
		buttonBB.visible.eON();
		buttonBB.X.eSet(600);
		buttonBB.X.eChange(Point(600, Abs), Point(1200, Abs), Converter(Linear), 0, 5000);
		buttonBB.X.eSet(1200, 5000);


		buttonBB.action.eSet(0, [&] {
			PlaySoundMem(SH_SHUTTER, DX_PLAYTYPE_BACK, TRUE);
			});
		buttonBB.action.eSet(1000, [&] {
			PlaySoundMem(SH_SHUTTER_SIGNAL, DX_PLAYTYPE_BACK, TRUE);
			});
	};

	void buttonPlay() {
		buttonBB.playAll();
		buttonBB.X.setStartTimeFromRange(0.5);
	};

	void buttonStop() {
		buttonBB.stopAll();
	};

	void buttonResume() {
		buttonBB.resumeAll();
	};

	void buttonReverse() {
		buttonBB.reverseAll();
	};

	void buttonSpeed2() {
		buttonBB.setPlaySpeedAll(2);
	};

	void buttonSpeed05() {
		buttonBB.setPlaySpeedAll(0.5);
	};

	void buttonLoop() {
		buttonBB.setLoopAll(TRUE);
	};


};

