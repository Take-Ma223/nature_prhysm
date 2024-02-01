#pragma once
#include <DxLib.h>

/// <summary>
/// ���������������N���X
/// </summary>
class NPVsync
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isVsyncAvailable">�����������L�����ǂ���</param>
	/// <param name="fps">�t���[�����[�g</param>
	NPVsync(const bool isVsyncAvailable, const int fps) {
		NPVsync::isVsyncAvailable = isVsyncAvailable;
		NPVsync::fps = fps;
	}

	/// <summary>
	/// ����������҂�
	/// </summary>
	/// <param name="loopPassedTime">���C�����[�v�ɂ�����������</param>
	void wait(double loopPassedTime) {
		if (!isVsyncAvailable) {
			int waitCount = 0;
			while (loopPassedTime + waitCount < (double)1000 / fps) {//FPS�����肳���邽�ߑ҂�
				WaitTimer(1);
				waitCount++;
			}
		}
	}

private:
	bool isVsyncAvailable = false;
	int fps = 1;



};

