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
	NotesModeView::NotesModeView(ViewContext* vc, DrawableInitParam param = DrawableInitParam()) : View(vc, param)
	{
		makeScreen(Size(sizeX, sizeY));

		ImageHandle rgbHandle = vc->asset->img(L"img/edit/notes_mode_rgb.png");
		ImageHandle kflnHandle = vc->asset->img(L"img/edit/notes_mode_kfln.png");
		ImageHandle commandHandle = vc->asset->img(L"img/edit/notes_mode_command.png");

		rgb = Image(rgbHandle, vc->time);
		kfln = Image(kflnHandle, vc->time);
		command = Image(commandHandle, vc->time);
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

