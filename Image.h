#include"DxLib.h"
#include <queue>
#include"ImageSet.h"

#pragma once
enum ConvertMode {
	Teleportation,
	Linear,
	HalfCosine,
	QuarterSine,
	QuarterCosine,
	Exponential
};

enum Specify {
	Abs,//���
	Rel//����
};

struct position{
	int x = 0;
	int y = 0;
};

class event {
	int startVal = 0;
	int endVal = 0;

	ConvertMode mode = Linear;
	double base = 1;

	double timeStart = 0;
	double timeEnd = 0;

public:
	event(int startVal = 0, int endVal = 0, ConvertMode mode = Linear, double base = 0, double timeStart = 0, double  timeEnd = 0);


	double calculateTimeRatio(double now);
	double convert(double input);

	//setter
	void setStartVal(int start);
	void setEndVal(int end);

	void setMode(ConvertMode mode, double base = 1);
	void setTime(int start, int end);

	//getter
	int getStartVal();
	int getEndVal();

	ConvertMode getMode();
	double getBase();

	double getTimeStart();
	double getTimeEnd();


};

class Image{
	int handle = 0;//�ǂݍ��񂾉摜�n���h��

	std::queue<event> visible;
	std::queue<event> moveXEvent;
	std::queue<event> moveYEvent;
	std::queue<event> alpha;

	int visibleVal;
	int x;
	int y;
	int alphaVal;

	int absRel(Specify specify, int now, int target);//��Α��Δ��ʂ��Ēl���i�[
	int calculateVal(double nowTime, event event);
	void pop(std::queue<event>* eventQueue, double time);
	void clearEvent(std::queue<event> &eventQueue);
public:
	Image(int handle, int x = 0, int y = 0, BOOL visible = 1, int alpha = 255);
	~Image();
	
	void reset();

	void move(int xStart, int yStart, int xEnd, int yEnd, Specify specify = Abs, ConvertMode mode = Teleportation, double nowTime = 0, double time = 0, double base = 4);
	void moveTo(int x, int y, Specify = Abs, ConvertMode mode = Teleportation, double nowTime = 0, double time = 0, double base = 4);
	void appear(double nowTime = 0, double time = 0);
	void disappear(double nowTime = 0, double time = 0);

	void changeAlpha(int startVal, int endVal, Specify specify = Abs, ConvertMode mode = Teleportation, double nowTime = 0, double time = 0, double base = 4);
	void changeAlphaTo(int val, Specify specify = Abs, ConvertMode mode = Teleportation, double nowTime = 0, double time = 0, double base = 4);

	void draw(double nowTime);//���t���[���Ă�

};

