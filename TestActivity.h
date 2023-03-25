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
	
	DrawableInitParam tvp1 = DrawableInitParam(Cordinate(0, 0));
	TextView testText1 = TextView(
		&activityContext,
		TextViewParam(wstring(L"TEST"), FontInfo(wstring(L"メイリオ"), 32, 0, FontType::ANTIALIASING_EDGE), GetColor(255, 255, 255)),
		tvp1
	);


	NumberImageHandles numberHandles = NumberImageHandles(
		activityContext.getAsset()->imgs(L"img/SmallNumberGreen.png", 10, 10, 1, 25, 50),
		activityContext.getAsset()->img(L"img/decimal.png")
	);
	NumberViewFormat nvp1 = NumberViewFormat(
		numberHandles,
		6,
		2,
		-3,
		NumberViewShowType::Left,
		4
	);
	DrawableInitParam tvp2 = DrawableInitParam(Cordinate(0, 200));
	NumberView testNumber1 = NumberView(
		&activityContext,
		nvp1,
		tvp2
	);

	int testNumberValue = 0;

	int a = 0;
	TestActivity(AppContext* ac) : Activity(ac) {

	}
	
	TestActivity(AppContext* ac, int input) : Activity(ac) {
		a = input;


		
		testNumber1 = NumberView(
			&activityContext,
			nvp1,
			tvp2
		);
		testNumber1.setCenterRatio(0, 0);
	}

	virtual void onCreate() override {
		Activity::onCreate();

		controller.setTestKeyTapHandler([&] {
			img.alpha.value -= 10;
			testNumberValue++;
			testNumber1.setValue(testNumberValue);
			Log(L"キーをタップしました");
			printfDx(L"キーをタップしました");
			return;
		});
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
		img.Drawable::draw();
	}
};

