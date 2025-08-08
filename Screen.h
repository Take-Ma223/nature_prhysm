#pragma once

/// <summary>
/// ��ʃN���X
/// </summary>
class Screen
{
public:
	Screen() {};

	/// <summary>
	/// ��ʂ̊J�n
	/// </summary>
	void run() {
		init();
		while (isContinueScreen) {
			loop();
		}
	}

protected:
	/// <summary>
	/// ��ʂ̏I��
	/// </summary>
	void exitScreen() {
		isContinueScreen = false;
	}

private:
	bool isContinueScreen = true;

	/// <summary>
	/// ��ʊJ�n���̏���������
	/// </summary>
	virtual void init() = 0;

	/// <summary>
	/// Model�X�V����
	/// </summary>
	virtual void updateModel() = 0;

	/// <summary>
	/// UI�\������
	/// </summary>
	virtual void updateView() = 0;

	/// <summary>
	/// �Q�[�����[�v
	/// </summary>
	void loop() {
		updateModel();
		if (!isContinueScreen)return;
		updateView();
		if (!isContinueScreen)return;
	}
};



