#pragma once
struct Drawable
{
	virtual ~Drawable() {}
	virtual void draw(int drawScreen) = 0;
	virtual void setScreen(int drawScreen) = 0;
	virtual void putContents(int drawScreen) = 0;
};

