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


void NotesModeView::beforeDrawProcess(int drawScreen)
{
	if (isCommandMode) {
		command.visible.setValue(1);
		rgb.visible.setValue(0);
		kfln.visible.setValue(0);
		return;
	}

	if (selectingMode == RGB) {
		command.visible.setValue(0);
		rgb.visible.setValue(1);
		kfln.visible.setValue(0);
	}
	else if (selectingMode == KFLN) {
		command.visible.setValue(0);
		rgb.visible.setValue(0);
		kfln.visible.setValue(1);
	}
}
