#include"DxLib.h"
#include"STRUCT_NOTE.h"
#ifndef _LED_OPERATION
#define _LED_OPERATION

typedef struct STATE {//LEDの状態
	char R[4] = { 0,0,0,0 };
	char G[4] = { 0,0,0,0 };
	char B[4] = { 0,0,0,0 };
}STATE;

extern void flash_LED(HANDLE, NOTE, STATE *, int, DWORD *, OVERLAPPED *);
extern void flash_LED_all(HANDLE hComm, STATE* LED_state, DWORD* dwWritten, OVERLAPPED* ovl);
extern void turn_off_LED(HANDLE, STATE *, DWORD *, OVERLAPPED *);

#endif