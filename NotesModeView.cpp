#include "NotesModeView.h"

NotesModeView::NotesModeView(ViewContext* vc, Cordinate cordinate) : View(vc, cordinate)
{
	int rgbHandle = vc->asset->img(L"img/edit/notes_mode_rgb.png");
	int kflnHandle = vc->asset->img(L"img/edit/notes_mode_kfln.png");
	int commandHandle = vc->asset->img(L"img/edit/notes_mode_command.png");

	auto c = Cordinate(X, Y);
	rgb = Image(rgbHandle, vc->time, c);
	kfln = Image(kflnHandle, vc->time, c);
	command = Image(commandHandle, vc->time, c);
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


void NotesModeView::draw()
{
	if (isCommandMode) {
		command.draw();
		return;
	}

	if (selectingMode == RGB) {
		rgb.draw();
	}
	else if (selectingMode == KFLN) {
		kfln.draw();
	}
}
