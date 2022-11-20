#pragma once
#include "Image.h"
#include "View.h"

class NotesModeView : View
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
	NotesModeView(ViewContext* vc, Cordinate cordinate);
	Image rgb;
	Image kfln;
	Image command;


	void switchToRgb();
	void switchToKfln();

	void switchNoteMode();
	void switchCommandMode();

	void draw();


};

