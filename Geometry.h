#pragma once

/// <summary>
/// ���W�N���X
/// </summary>
class Cordinate {
public:
	int x = 0;
	int y = 0;

	Cordinate(int x = 0, int y = 0);
};

/// <summary>
/// ���S�ʒu�N���X �����Ŏw�肵�܂�
/// </summary>
class CenterRatio {
public:
	double x = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)
	double y = 0;//�����(0,0)�Ƃ����Ƃ��̒��S���W(����Drawable���̊���)

	CenterRatio(double x = 0, double y = 0);
};

/// <summary>
/// �傫���N���X
/// </summary>
class Size {
public:
	int x = 0;
	int y = 0;

	Size(int x = 1, int y = 1);
};
