#include "NotesModeView.h"

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
