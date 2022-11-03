#include "View.h"


Cordinate::Cordinate(int x, int y) {
	Cordinate::x = x;
	Cordinate::y = y;
}

ViewContext::ViewContext(Asset* asset, Option* option, double* time)
{
	ViewContext::asset = asset;
	ViewContext::option = option;
	ViewContext::time = time;
}

View::View(ViewContext vc, Cordinate cordinate){
	View::vc = vc;
	View::cordinate = cordinate;
}