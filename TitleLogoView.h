#pragma once
#include "Image.h"
#include "View.h"
#include <memory>
#include "DxLib.h"

using namespace std;

class TitleLogoView : public View
{
public:
	TitleLogoView::TitleLogoView(ActivityContext* context, DrawableInitParam param = DrawableInitParam()) :View(context, param) {
		c = context;
		makeScreen(Size(sizeX, sizeY));
		texture_y = TransValue(c);
		texture_alpha = TransValue(c);
		light_x = TransValue(c);

		DrawableInitParam p = DrawableInitParam(Cordinate(0, 0));
		icon = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/icon.png"), p));
		line = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/line.png"), p));
		title = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/title.png"), p));
		texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/texture.jpg"), p));
		light = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/light.png"), p));

		//texture = LoadGraph(L"img/title_logo/texture.jpg");
		//texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/texture.jpg"), p));


		//addDrawable(texture.get());
		//addDrawable(title.get());
		//addDrawable(line.get());
		addDrawable(icon.get());

	}

	void startAnimation() {
		is_playing = true;
		texture_y.eSet(0);
		texture_y.eChange(Point(0), Point(580), Converter(ConvertMode::Linear), 0, 10000);
		texture_y.eChange(Point(580), Point(0), Converter(ConvertMode::Linear), 10000, 20000);
		texture_y.setLoop(true);
		texture_y.play();

		texture_alpha.eSet(0);
		texture_alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 500);
		texture_alpha.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 5000, 5500);
		texture_alpha.eChange(Point(0), Point(0), Converter(ConvertMode::Linear), 5500, 8000);
		texture_alpha.setLoop(true);
		texture_alpha.play();

		light_x.eSet(0);
		light_x.eChange(Point(0), Point(1152), Converter(ConvertMode::Exponential, 8), 5700, 6200);
		light_x.eChange(Point(0), Point(1152), Converter(ConvertMode::Exponential, 8), 39700, 40200);
		light_x.play();

		this->alpha.eSet(0);
		this->alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);
		this->playAll();
	}

	bool isPlaying() {
		return is_playing;
	}

	void hide() {
		this->clearAllEvent();
		this->alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), 0, 400);
		this->playAll();
	}

private:
	ActivityContext* c;
	static const int sizeX = 1280;
	static const int sizeY = 720;

	unique_ptr<Image> icon;
	unique_ptr<Image> line;
	unique_ptr<Image> title;
	unique_ptr<Image> texture;
	unique_ptr<Image> light;

	TransValue texture_y;
	TransValue texture_alpha;

	TransValue light_x;

	bool is_playing = false;

	virtual void beforeDrawProcess(int drawScreen) override {
		texture_y.process();
		texture_alpha.process();
		light_x.process();

		//SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 255);

		//setScreen(title.get()->getHandle());
		//title.get()->draw(handle.getHandle());
		GraphBlendRectBlt(
			title.get()->getHandle(),
			texture.get()->getHandle(),
			handle.getHandle(),
			120, 180,
			1160, 320,
			120, texture_y.value,
			120, 180,
			texture_alpha.value,
			DX_GRAPH_BLEND_PMA_SCREEN
		);

		line.get()->draw(handle.getHandle());
		
		GraphBlendRectBlt(
			handle.getHandle(),
			light.get()->getHandle(),
			handle.getHandle(),
			light_x.value,     128,
			light_x.value+128, 128+256,
			0, 0,
			light_x.value, 128,
			255,
			DX_GRAPH_BLEND_PMA_SCREEN
		);


	}

};

