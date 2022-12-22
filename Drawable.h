#pragma once
struct Drawable
{
	virtual ~Drawable() {}
	virtual void draw() = 0;
};

