#include "NotesMode.h"

NotesMode::NotesMode(int rgbHandle, int kflnHandle, int commandHandle, double* time)
{
	rgb = Image(rgbHandle, time, X, Y);
	kfln = Image(kflnHandle, time, X, Y);
	command = Image(commandHandle, time, X, Y);
}

void NotesMode::switchToRgb()
{
	selectingMode = RGB;
	return;
}

void NotesMode::switchToKfln()
{
	selectingMode = KFLN;
	return;
}

void NotesMode::switchNoteMode()
{
	isCommandMode = FALSE;
}

void NotesMode::switchCommandMode()
{
	isCommandMode = TRUE;
}


void NotesMode::draw()
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
