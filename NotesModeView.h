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
	NotesModeView::NotesModeView(ActivityContext* c, DrawableInitParam param = DrawableInitParam()) : View(c, param)
	{
		makeScreen(Size(sizeX, sizeY));

		ImageHandle rgbHandle = c->getAsset()->img(L"img/edit/notes_mode_rgb.png");
		ImageHandle kflnHandle = c->getAsset()->img(L"img/edit/notes_mode_kfln.png");
		ImageHandle commandHandle = c->getAsset()->img(L"img/edit/notes_mode_command.png");

		rgb = Image(c, rgbHandle);
		kfln = Image(c, kflnHandle);
		command = Image(c, commandHandle);

		addDrawable(&rgb);
		addDrawable(&kfln);
		addDrawable(&command);
	}
	Image rgb;
	Image kfln;
	Image command;


	void switchToRgb();
	void switchToKfln();

	void switchNoteMode();
	void switchCommandMode();

	virtual void beforeDrawProcess(int drawScreen) override;



};

