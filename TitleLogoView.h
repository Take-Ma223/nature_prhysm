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
		surface_light_alpha = TransValue(c);

		DrawableInitParam p = DrawableInitParam(Cordinate(0, 0));
		icon = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/icon.png"), p));
		line = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/line.png"), p));
		title = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/title.png"), p));
		texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/texture.jpg"), p));
		bar_light = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/light.png"), p));

		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/n.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/a.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/t.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/u.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/r1.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/e.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/p.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/r2.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/h.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/y.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/s.png"), p)));
		str_bright.push_back(unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/m.png"), p)));

		surface_light = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/surface.png"), p));

		//texture = LoadGraph(L"img/title_logo/texture.jpg");
		//texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/texture.jpg"), p));


		//addDrawable(texture.get());
		//addDrawable(title.get());
		//addDrawable(line.get());
		//addDrawable(icon.get());

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
		light_x.eChange(Point(0), Point(1152), Converter(ConvertMode::Exponential, 8), 39700, 40000);
		light_x.play();

		double str_flash_start_timing = 6221;
		double str_flash_duration = 68.965;

		for (int i = 0; i < STR_LENGTH; i++) {
			str_bright[i].get()->alpha.eSet(0);
			str_bright[i].get()->alpha.eSet(255, str_flash_start_timing + str_flash_duration * i);
			str_bright[i].get()->alpha.eChangeTo(0, Converter(ConvertMode::Linear), str_flash_start_timing + str_flash_duration * i, str_flash_start_timing + str_flash_duration * (3+i));

			str_bright[i].get()->playAll();
		}


		double surface_light_t1 = 7120;
		double surface_light_t2 = 40017;
		double surface_light_duration = 2000;

		surface_light_alpha.eSet(0);
		surface_light_alpha.eSet(255, surface_light_t1);
		surface_light_alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), surface_light_t1, surface_light_t1 + surface_light_duration);
		surface_light_alpha.eSet(255, surface_light_t2);
		surface_light_alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), surface_light_t2, surface_light_t2 + surface_light_duration);
		surface_light_alpha.play();

		this->alpha.eSet(0);
		this->alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);

		this->brightnessR.eSet(0);
		this->brightnessG.eSet(0);
		this->brightnessB.eSet(0);
		this->brightnessR.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);
		this->brightnessG.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);
		this->brightnessB.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);

		this->alpha.eSet(Point(255),7120);

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
	unique_ptr<Image> bar_light;

	static const int STR_LENGTH = 12;
	vector< unique_ptr<Image> > str_bright;

	unique_ptr<Image> surface_light;

	TransValue texture_y;
	TransValue texture_alpha;

	TransValue light_x;

	TransValue surface_light_alpha;

	bool is_playing = false;

	/// <summary>
	/// ÉXÉNÉäÅ[Éìï`âÊÇçsÇ§ÇΩÇﬂÅAaddDrawableÇ≈ìoò^ÇÕçsÇÌÇ∏éËìÆÇ≈ï`âÊ
	/// </summary>
	virtual void beforeDrawProcess(int drawScreen) override {
		texture_y.process();
		texture_alpha.process();
		light_x.process();
		surface_light_alpha.process();

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

		for (int i = 0; i < STR_LENGTH; i++) {
			str_bright[i].get()->draw(handle.getHandle());
		}

		line.get()->draw(handle.getHandle());
		icon.get()->draw(handle.getHandle());

		GraphBlendRectBlt(
			handle.getHandle(),
			bar_light.get()->getHandle(),
			handle.getHandle(),
			light_x.value,     128,
			light_x.value+128, 128+256,
			0, 0,
			light_x.value, 128,
			255,
			DX_GRAPH_BLEND_PMA_SCREEN
		);

		GraphBlendRectBlt(
			handle.getHandle(),
			surface_light.get()->getHandle(),
			handle.getHandle(),
			0, 0,
			1280, 720,
			0, 0,
			0, 0,
			surface_light_alpha.value,
			DX_GRAPH_BLEND_PMA_SCREEN
		);



	}

};

