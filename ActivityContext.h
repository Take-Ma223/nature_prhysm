#pragma once

#include "Asset.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include "AppContext.h"

/**
* 各画面のコンテキストクラス
* 画面毎に作られます
*/
class ActivityContext
{
	AppContext* appContext;
	Asset* asset;
	ImageHandle baseHandle = ImageHandle(DX_SCREEN_BACK, Size(1280, 720));
public:
	ActivityContext(AppContext* appContext = NULL, Asset* asset = NULL) {
		ActivityContext::appContext = appContext;
		ActivityContext::asset = asset;
	};

	Option* getOption() { return appContext->option; };
	Config* getConfig() { return appContext->config; };

	Asset* getAsset() { return asset; };
	double getTime() { return *(appContext->time); };
	ImageHandle getBaseHandle() { return baseHandle; };
};

