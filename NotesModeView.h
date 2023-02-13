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
	NotesModeView::NotesModeView(DrawableContext* dc, DrawableInitParam param = DrawableInitParam()) : View(dc, param)
	{
		makeScreen(Size(sizeX, sizeY));

		ImageHandle rgbHandle = dc->asset->img(L"img/edit/notes_mode_rgb.png");
		ImageHandle kflnHandle = dc->asset->img(L"img/edit/notes_mode_kfln.png");
		ImageHandle commandHandle = dc->asset->img(L"img/edit/notes_mode_command.png");

		rgb = Image(dc, rgbHandle);
		kfln = Image(dc, kflnHandle);
		command = Image(dc, commandHandle);
	}
	Image rgb;
	Image kfln;
	Image command;


	void switchToRgb();
	void switchToKfln();

	void switchNoteMode();
	void switchCommandMode();

	virtual void prepareScreen(int drawScreen) override;



};

