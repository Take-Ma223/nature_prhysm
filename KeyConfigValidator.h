#pragma once
#include <DxLib.h>
class KeyConfigValidator
{
public:
	static int isValidKey(int key) {
		if (key == KEY_INPUT_ESCAPE)return FALSE;
		if (key == KEY_INPUT_LSHIFT)return FALSE;
		if (key == KEY_INPUT_LCONTROL)return FALSE;
		if (key == KEY_INPUT_RSHIFT)return FALSE;
		if (key == KEY_INPUT_RCONTROL)return FALSE;
		if (key == KEY_INPUT_F1)return FALSE;
		if (key == KEY_INPUT_F2)return FALSE;
		if (key == KEY_INPUT_F3)return FALSE;
		if (key == KEY_INPUT_F4)return FALSE;
		if (key == KEY_INPUT_F5)return FALSE;
		if (key == KEY_INPUT_F6)return FALSE;
		if (key == KEY_INPUT_F7)return FALSE;
		if (key == KEY_INPUT_F8)return FALSE;
		if (key == KEY_INPUT_F9)return FALSE;
		if (key == KEY_INPUT_F10)return FALSE;
		if (key == KEY_INPUT_F11)return FALSE;
		if (key == KEY_INPUT_F12)return FALSE;
		if (key == KEY_INPUT_RETURN)return FALSE;
		if (key == KEY_INPUT_NUMPADENTER)return FALSE;
		if (key == KEY_INPUT_UP)return FALSE;
		if (key == KEY_INPUT_DOWN)return FALSE;
		if (key == KEY_INPUT_LEFT)return FALSE;
		if (key == KEY_INPUT_RIGHT)return FALSE;

		return TRUE;
	}

};

