#pragma once
#include <DxLib.h>

/// <summary>
/// 垂直同期を扱うクラス
/// </summary>
class NPVsync
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isVsyncAvailable">垂直同期が有効かどうか</param>
	/// <param name="fps">フレームレート</param>
	NPVsync(const bool isVsyncAvailable, const int fps) {
		NPVsync::isVsyncAvailable = isVsyncAvailable;
		NPVsync::fps = fps;
	}

	/// <summary>
	/// 垂直同期を待つ
	/// </summary>
	/// <param name="loopPassedTime">メインループにかかった時間</param>
	void wait(double loopPassedTime) {
		if (!isVsyncAvailable) {
			int waitCount = 0;
			while (loopPassedTime + waitCount < (double)1000 / fps) {//FPSを安定させるため待つ
				WaitTimer(1);
				waitCount++;
			}
		}
	}

private:
	bool isVsyncAvailable = false;
	int fps = 1;



};

