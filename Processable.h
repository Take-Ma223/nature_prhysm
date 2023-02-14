#pragma once

/**
* メインループで処理を呼ぶことができるクラスのインターフェース
* 
**/
struct Processable
{
public:
	virtual ~Processable() {};
	virtual void process() = 0;
};

