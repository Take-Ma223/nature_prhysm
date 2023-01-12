#pragma once
#include "Image.h"
#include "View.h"

class NotesModeView : public View
{
	enum Selecting
	{
		RGB,KFLN
	};

	Selecting selectingMode = Selecting::RGB;
	BOOL isCommandMode = FALSE;

	int sizeX = 32;
	int sizeY = 96;

public:
	NotesModeView(ViewContext* vc, Cordinate cordinate);
	Image rgb;
	Image kfln;
	Image command;


	void switchToRgb();
	void switchToKfln();

	void switchNoteMode();
	void switchCommandMode();

	virtual void putContents(int drawScreen) override;



};

