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
		command.visible.value = 1;
		rgb.visible.value = 0;
		kfln.visible.value = 0;
		return;
	}

	if (selectingMode == RGB) {
		command.visible.value = 0;
		rgb.visible.value = 1;
		kfln.visible.value = 0;
	}
	else if (selectingMode == KFLN) {
		command.visible.value = 0;
		rgb.visible.value = 0;
		kfln.visible.value = 1;
	}
}
