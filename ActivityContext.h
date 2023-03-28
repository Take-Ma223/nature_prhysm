#pragma once

#include "Asset.h"
#include "STRUCT_OP.h"
#include "STRUCT_CONFIG.h"
#include "AppContext.h"

/**
* �e��ʂ̃R���e�L�X�g�N���X
* ��ʖ��ɍ���܂�
*/
class ActivityContext
{
	AppContext* appContext;
	Asset* asset;
public:
	ActivityContext(AppContext* appContext = NULL, Asset* asset = NULL) {
		ActivityContext::appContext = appContext;
		ActivityContext::asset = asset;
	};

	Option* getOption() { return appContext->option; };
	Config* getConfig() { return appContext->config; };

	Asset* getAsset() { return asset; };
	double getTime() { return appContext->time; };
	ImageHandle getBaseHandle() { return appContext->baseHandle; };

	void startActivity(Activity* activity) {
		appContext->startActivity(activity);
	}

};
