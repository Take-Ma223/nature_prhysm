#pragma once
#include "Image.h"
#include "View.h"
#include <memory>
#include "DxLib.h"
#include "STRUCT_Song.h"
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

class TitleLogoView : public View
{
public:
	TitleLogoView::TitleLogoView(Song* Music, int* NumberOfSongs, ActivityContext* context, DrawableInitParam param = DrawableInitParam()) :View(context, param) {
		c = context;
		makeScreen(screen_size);
		for (int i = 0; i < TEXTURE_COUNT; i++) {
			texture_x.push_back(TransValue(c));
			texture_y.push_back(TransValue(c));
		}

		texture_alpha = TransValue(c);
		icon_flush_alpha = TransValue(c);
		light_x = TransValue(c);
		surface_light_alpha = TransValue(c);
		texture_switcher = TransAction(c);
		handler_t1 = TransAction(c);

		DrawableInitParam p = DrawableInitParam(Cordinate(0, 0));
		icon = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/icon.png"), p));
		line = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/line.png"), p));
		title = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/title.png"), p));

		paths = getTexturePaths(Music, NumberOfSongs);
		
		for (int i = 0; i < TEXTURE_COUNT; i++) {
			
			//テクスチャ用の画像を用意し、sizeXに幅を合わせたサイズを得る
			auto texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(paths[i]), p));
			auto texture_x = texture.get()->getSize().x;
			auto texture_y = texture.get()->getSize().y;
			auto texture_x_sizeX_ratio = (double)screen_size.x / texture_x;
			auto texture_resized_size = Size(texture_x * texture_x_sizeX_ratio, texture_y * texture_x_sizeX_ratio);
			textures_size.push_back(texture_resized_size);

			//テクスチャを用意
			int screenHandle = MakeScreen(textures_size[i].x, textures_size[i].y, TRUE);
			auto img = ImageHandle(screenHandle, textures_size[i]);
			textures.push_back(unique_ptr<Image>(new Image(c, img, p)));

			//テクスチャの貼り付け
			SetDrawMode(DX_DRAWMODE_BILINEAR);
			SetDrawScreen(textures[i].get()->getHandle());
			DrawExtendGraph(0,0, texture_resized_size.x, texture_resized_size.y,
				texture.get()->getHandle(), TRUE);


		}





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
		icon_flush = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/bright/icon.png"), p));

		//texture = LoadGraph(L"img/title_logo/texture.jpg");
		//texture = unique_ptr<Image>(new Image(c, c->getAsset()->img(L"img/title_logo/texture.jpg"), p));


		//addDrawable(texture.get());
		//addDrawable(title.get());
		//addDrawable(line.get());
		//addDrawable(icon.get());

	}

	void startAnimation() {
		is_playing = true;

		double texture_loop_time_length_x = 20000;
		double texture_loop_time_length_y = 16000;

		//テクスチャ参照位置
		for (int i=0; i < TEXTURE_COUNT; i++) {
			int margin_x = textures_size[i].x - logo_str_size.x;
			texture_x[i].eSet(0);
			texture_x[i].eChange(Point(margin_x / 2), Point(0), Converter(ConvertMode::Linear), 0, 5000);
			texture_x[i].eChange(Point(0), Point(margin_x / 2), Converter(ConvertMode::Linear), 5000, 10000);
			texture_x[i].eChange(Point(margin_x / 2), Point(margin_x), Converter(ConvertMode::Linear), 10000, 15000);
			texture_x[i].eChange(Point(margin_x), Point(margin_x / 2), Converter(ConvertMode::Linear), 15000, 20000);
			texture_x[i].setLoop(true);
			texture_x[i].eSet(0, texture_loop_time_length_x);
			texture_x[i].play();
			texture_x[i].setStartTimeFromRange(GetRand(100) * 0.01);//位相ずらし


			int margin_y = textures_size[i].y - logo_str_size.y;
			texture_y[i].eSet(0);
			texture_y[i].eChange(Point(0), Point(margin_y), Converter(ConvertMode::Linear), 0, 8000);
			texture_y[i].eChange(Point(margin_y), Point(0), Converter(ConvertMode::Linear), 8000, 16000);
			texture_y[i].setLoop(true);
			texture_y[i].eSet(0, texture_loop_time_length_y);
			texture_y[i].play();
			texture_y[i].setStartTimeFromRange(GetRand(100) * 0.01);//位相ずらし

		}

		//テクスチャ透明度
		double texture_appear_loop_time = 8000;
		texture_alpha.eSet(0);
		texture_alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 500);
		texture_alpha.eChange(Point(255), Point(0), Converter(ConvertMode::QuarterCosine), 5622, 5722);
		texture_alpha.eChange(Point(0), Point(0), Converter(ConvertMode::Linear), 6500, texture_appear_loop_time);
		texture_alpha.setLoop(true);
		texture_alpha.play();

		//テクスチャ切り替え
		set_texture_switch_duration(413.79310344827586206896551724138 / 2);//8分音符間隔
	

		//ライト
		light_x.eSet(0);
		light_x.eChange(Point(0), Point(1152), Converter(ConvertMode::Exponential, 8), 5200, 5700);
		light_x.eChange(Point(0), Point(1152), Converter(ConvertMode::Exponential, 8), 39200, 39500);
		light_x.play();

		//文字フラッシュ
		double str_flash_start_timing = 5721;
		double str_flash_duration = 68.965;

		for (int i = 0; i < STR_LENGTH; i++) {
			str_bright[i].get()->alpha.eSet(0);
			str_bright[i].get()->alpha.eSet(255, str_flash_start_timing + str_flash_duration * i);
			str_bright[i].get()->alpha.eChangeTo(0, Converter(ConvertMode::Linear), str_flash_start_timing + str_flash_duration * i, str_flash_start_timing + str_flash_duration * (3+i));

			str_bright[i].get()->playAll();
		}

		//アイコンフラッシュ
		icon_flush_alpha.eChange(Point(100), Point(0), Converter(ConvertMode::Linear), 0, 413.79310344827586206896551724138);
		icon_flush_alpha.setLoop(true);
		icon_flush_alpha.play();

		//ロゴ全体フラッシュ
		double surface_light_t1 = 6620;
		double surface_light_t2 = 39517;
		double surface_light_duration = 2000;

		surface_light_alpha.eSet(0);
		surface_light_alpha.eSet(255, surface_light_t1);
		surface_light_alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), surface_light_t1, surface_light_t1 + surface_light_duration);
		surface_light_alpha.eSet(255, surface_light_t2);
		surface_light_alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), surface_light_t2, surface_light_t2 + surface_light_duration);
		surface_light_alpha.play();

		//ロゴ全体透明度
		this->alpha.eSet(0);
		this->alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 6200);
		this->alpha.eSet(Point(255),6620);

		this->playAll();

		//最初のロゴ点滅前時点の処理
		handler_t1.eSet(0, [] {});

		handler_t1.eSet(4966, [this] {
			//テクスチャ切り替え間隔を変化
			set_texture_switch_duration(413.79310344827586206896551724138 / 6);//24分音符間隔
		});
		handler_t1.eSet(5726, [this] {
			//アイコン点滅停止
			icon_flush_alpha.value = 0;
			icon_flush_alpha.stop();
		});
		handler_t1.eSet(6620, [this] {
			//最初のロゴ点滅時点の処理
			set_texture_switch_duration(8000);
			texture_alpha.clearEvent();

			double texture_appear_loop_time = 8000;
			texture_alpha.eSet(0);
			texture_alpha.eChange(Point(0), Point(255), Converter(ConvertMode::Linear), 0, 500);
			texture_alpha.eChange(Point(255), Point(0), Converter(ConvertMode::Linear), 5390, 5722);
			texture_alpha.eChange(Point(0), Point(0), Converter(ConvertMode::Linear), 6500, texture_appear_loop_time);
			texture_alpha.setLoop(true);
			texture_alpha.play();

			//アイコン点滅開始
			icon_flush_alpha.play();
		});
		handler_t1.eSet(39517, [this] {
			//アイコン点滅停止
			icon_flush_alpha.value = 0;
			icon_flush_alpha.stop();
		});
		
		handler_t1.play();


	}



	bool isPlaying() {
		return is_playing;
	}

	void hide() {
		surface_light_alpha.clearEvent();
		surface_light_alpha.eSet(255);
		surface_light_alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), 0,600);
		surface_light_alpha.play();

		this->clearAllEvent();
		this->alpha.eChangeTo(Point(0), Converter(ConvertMode::Linear), 0, 400);
		this->playAll();
	}

private:
	ActivityContext* c;

	Size screen_size = Size(1280, 720);//画面サイズ

	unique_ptr<Image> icon;
	unique_ptr<Image> line;
	unique_ptr<Image> title;

	vector < wstring > paths;

	static const int TEXTURE_COUNT = 10;
	int drawing_texture_number = 0;
	TransAction texture_switcher;

	vector< unique_ptr<Image> >textures;
	vector< Size > textures_size;
	Size logo_str_size = Size(1040, 140);//ロゴの文字領域サイズ
	Cordinate logo_str_start_top_cord = Cordinate(120, 180);//ロゴ文字の左上座標

	unique_ptr<Image> bar_light;

	static const int STR_LENGTH = 12;
	vector< unique_ptr<Image> > str_bright;

	unique_ptr<Image> icon_flush;
	TransValue icon_flush_alpha;

	unique_ptr<Image> surface_light;

	vector < TransValue > texture_x;
	vector < TransValue > texture_y;
	TransValue texture_alpha;

	TransValue light_x;

	TransValue surface_light_alpha;

	bool is_playing = false;

	TransAction handler_t1;//最初のロゴ点滅前時点の処理ハンドラ

	/// <summary>
	/// スクリーン描画を行うため、addDrawableで登録は行わず手動で描画
	/// </summary>
	virtual void beforeDrawProcess(int drawScreen) override {
		process_transition();

		title.get()->draw(handle.getHandle());
		icon.get()->draw(handle.getHandle());
		
		double icon_flush_hide_ratio = (double)(255 - texture_alpha.value) / 255;
		icon_flush.get()->alpha.value = icon_flush_hide_ratio * icon_flush_alpha.value;
		icon_flush.get()->draw(handle.getHandle());

		GraphBlendRectBlt(
			handle.getHandle(),
			textures[drawing_texture_number].get()->getHandle(),
			handle.getHandle(),
			logo_str_start_top_cord.x, logo_str_start_top_cord.y,	//int SrcX1, int SrcY1
			logo_str_start_top_cord.x + logo_str_size.x,			//int SrcX2,
			logo_str_start_top_cord.y + logo_str_size.y,			//int SrcY2
			texture_x[drawing_texture_number].value, texture_y[drawing_texture_number].value,	//int BlendX,  int BlendY
			logo_str_start_top_cord.x, logo_str_start_top_cord.y,	//int DestX, int DestY
			texture_alpha.value,
			DX_GRAPH_BLEND_PMA_HARDLIGHT
		);

		for (int i = 0; i < STR_LENGTH; i++) {
			str_bright[i].get()->draw(handle.getHandle());
		}

		line.get()->draw(handle.getHandle());

		GraphBlendRectBlt(
			handle.getHandle(),
			bar_light.get()->getHandle(),
			handle.getHandle(),
			light_x.value,     128,
			light_x.value+128, 128+256,
			0, 0,
			light_x.value, 128,
			255,
			DX_GRAPH_BLEND_PMA_HARDLIGHT
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
			DX_GRAPH_BLEND_PMA_HARDLIGHT
		);
	}

	void process_transition() {
		for (int i = 0; i < TEXTURE_COUNT; i++) {
			texture_x[i].process();
			texture_y[i].process();
		}
		texture_alpha.process();
		texture_switcher.process();
		light_x.process();
		icon_flush_alpha.process();
		surface_light_alpha.process();
		handler_t1.process();
	}

	void set_texture_switch_duration(double duration) {
		//テクスチャ切り替え間隔を変化
		texture_switcher.clearEvent();
		texture_switcher.eSet(duration, [this] {
			drawing_texture_number++;
			if (drawing_texture_number >= TEXTURE_COUNT) {
				drawing_texture_number = 0;
			}
			});
		texture_switcher.setLoop(true);
		texture_switcher.play();
	}

	vector< wstring > getTexturePaths(Song* Music, int* NumberOfSongs) {
		int songs_count = *NumberOfSongs;
		int difficulty_count = 4;

		//全てのジャケットパスを取得
		vector<wstring> all_paths;
		for (int i = 0; i < songs_count; i++) {
			for (int j = 1; j <= difficulty_count; j++) {
				if (Music[i].secret == UnlockState::Secret) {
					break;
				}
				if (!Music[i].exist[j]) {
					continue;
				}
				if (wstring(Music[i].jacketpath[j]) == wstring(L"")) {
					continue;
				}
				if (!isFileExist(wstring(Music[i].jacketpath[j]))) {
					continue;
				}

				if (all_paths.size() > 0) {//被ってるジャケット画像は入れない
					if (all_paths.back() == wstring(Music[i].jacketpath[j])) {
						continue;
					}
				}
				all_paths.push_back(Music[i].jacketpath[j]);
			}
		}

		//全てのジャケットパスからTEXTURE_COUNT個選ぶ
		vector< wstring > paths;
		while (paths.size() < TEXTURE_COUNT) {
			if (all_paths.size() == 0) {
				paths.push_back(L"");
				continue;
			}

			int ind = GetRand(all_paths.size() - 1);
			paths.push_back(all_paths[ind]);

			if (paths.size() > 0 && all_paths.size() >= TEXTURE_COUNT) {
				//被らないように選んだものを消す
				all_paths.erase(all_paths.begin() + ind);
			}
		}

		return paths;
	}

	bool isFileExist(const std::wstring& path) {
		auto handle = FileRead_open(path.c_str());

		auto is_exist = handle != 0;
		FileRead_close(handle);
		return is_exist;
	}

};

