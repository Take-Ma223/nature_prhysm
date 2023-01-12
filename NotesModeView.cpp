#include "NotesModeView.h"

NotesModeView::NotesModeView(ViewContext* vc, Cordinate cordinate, BOOL visible, int alpha) : View(vc, cordinate, visible, alpha)
{
	makeScreen(Size(sizeX, sizeY));

	ImageHandle rgbHandle = vc->asset->img(L"img/edit/notes_mode_rgb.png");
	ImageHandle kflnHandle = vc->asset->img(L"img/edit/notes_mode_kfln.png");
	ImageHandle commandHandle = vc->asset->img(L"img/edit/notes_mode_command.png");

	rgb = Image(rgbHandle, vc->time, Cordinate(0, 0));
	kfln = Image(kflnHandle, vc->time, Cordinate(0, 0));
	command = Image(commandHandle, vc->time, Cordinate(0, 0));
}

void NotesModeView::switchToRgb()
{
	selectingMode = RGB;
	return;
}

void NotesModeView::switchToKfln()
{
	selectingMode = KFLN;
	return;
}

void NotesModeView::switchNoteMode()
{
	isCommandMode = FALSE;
}

void NotesModeView::switchCommandMode()
{
	isCommandMode = TRUE;
}


void NotesModeView::prepareScreen(int drawScreen)
{
	if (isCommandMode) {
		command.draw(drawScreen);
		return;
	}

	if (selectingMode == RGB) {
		rgb.draw(drawScreen);
	}
	else if (selectingMode == KFLN) {
		kfln.draw(drawScreen);
	}
}
