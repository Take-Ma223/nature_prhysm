#pragma once

#include "Image.h"
#include "View.h"
#include <vector>
#include <string>
#include <memory>
#include "STRUCT_NOTE.h"

using namespace std;

/// <summary>
/// Aurora表示1レーン分のView
/// </summary>
class AuroraView : public View
{
public:

	/// <summary>
	/// 色ガイドの色を変更する
	/// 100msで色が遷移します
	/// NONE指定で色が消えます
	/// </summary>
	/// <param name="color">変更先の色</param>
	void setColor(NoteColor color) {

		for (auto item = aurora.begin(); item < aurora.end(); item++) {
   			item->get()->alpha.clearEvent();
			item->get()->alpha.eChangeTo(Point(0), Converter(Linear), 0, 400);
		}

		aurora[static_cast<int>(color)].get()->alpha.clearEvent();
		aurora[static_cast<int>(color)].get()->alpha.eChangeTo(Point(255), Converter(Linear), 0, 400);

		for (auto item = aurora.begin(); item < aurora.end(); item++) {
			item->get()->alpha.play();
		}


	}


	AuroraView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(size_x, size_y));
		//this->centerRatioX = 0.5;
		//this->centerRatioY = 0;

		vector<wstring> path = {
			wstring(L"img/Aurora/none.png"),
			wstring(L"img/Aurora/r.png"),
			wstring(L"img/Aurora/g.png"),
			wstring(L"img/Aurora/b.png"),
			wstring(L"img/Aurora/c.png"),
			wstring(L"img/Aurora/m.png"),
			wstring(L"img/Aurora/y.png"),
			wstring(L"img/Aurora/w.png"),
			wstring(L"img/Aurora/k.png"),
			wstring(L"img/Aurora/f.png"),
		};

		for (auto item = path.begin(); item < path.end(); item++) {
			aurora.push_back(
				unique_ptr<Image>(
					new Image(c, c->getAsset()->img(item->c_str()))
				)
			);
		}

		for (auto item = aurora.begin(); item < aurora.end(); item++) {
			addDrawable(item->get());
		}



	}

private:
	const int size_x = 256;
	const int size_y = 256;

	vector<unique_ptr<Image>> aurora;



	virtual void beforeDrawProcess(int drawScreen) override {

	};
};

