#pragma once
#include<cassert>

class Activity;//�v���g�^�C�v�錾

/// <summary>
/// �C���e���g�N���X
/// </summary>
class Intent {
	Activity* activity = NULL;

public:
	/// <summary>
	/// �A�N�e�B�r�e�B�̃Z�b�g
	/// </summary>
	/// <param name="activity">�A�N�e�B�r�e�B</param>
	void setActivity(Activity* activity) {
		assert(Intent::activity == NULL);
		Intent::activity = activity;
	}

	/// <summary>
	/// �A�N�e�B�r�e�B�̗L����Ԃ�
	/// </summary>
	/// <returns>�A�N�e�B�r�e�B�̗L��</returns>
	bool isExistActivity() {
		return Intent::activity != NULL;
	}

	/// <summary>
	/// �A�N�e�B�r�e�B���擾
	/// </summary>
	/// <returns>�A�N�e�B�r�e�B</returns>
	Activity* getActivity() {
		assert(Intent::activity != NULL);

		auto activityIntent = Intent::activity;
		Intent::activity = NULL;
		return activityIntent;
	}


};

