#pragma once
#include "Image.h"
class NotesMode
{
	int X = 439;
	int Y = 532;
	
	enum Selecting
	{
		RGB,KFLN
	};

	Selecting selectingMode = Selecting::RGB;
	BOOL isCommandMode = FALSE;

public:
	NotesMode(int rgbHandle, int kflnHandle, int commandHandle, double* time);
	Image rgb;
	Image kfln;
	Image command;


	void switchToRgb();
	void switchToKfln();

	void switchNoteMode();
	void switchCommandMode();

	void draw();


};

