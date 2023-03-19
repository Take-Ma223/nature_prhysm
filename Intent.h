#pragma once
#include<cassert>

class Activity;//プロトタイプ宣言

/// <summary>
/// インテントクラス
/// </summary>
class Intent {
	Activity* activity = NULL;

public:
	/// <summary>
	/// アクティビティのセット
	/// </summary>
	/// <param name="activity">アクティビティ</param>
	void setActivity(Activity* activity) {
		assert(Intent::activity == NULL);
		Intent::activity = activity;
	}

	/// <summary>
	/// アクティビティの有無を返す
	/// </summary>
	/// <returns>アクティビティの有無</returns>
	bool isExistActivity() {
		return Intent::activity != NULL;
	}

	/// <summary>
	/// アクティビティを取得
	/// </summary>
	/// <returns>アクティビティ</returns>
	Activity* getActivity() {
		assert(Intent::activity != NULL);

		auto activityIntent = Intent::activity;
		Intent::activity = NULL;
		return activityIntent;
	}


};

