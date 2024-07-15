#include"DxLib.h"
#include"STRUCT_XY.h"
#include"STRUCT_CONFIG.h"

#ifndef _SHOW_SOMETHING
#define _SHOW_SOMETHING

extern void show_str(int time, int time_base, int H_STR, int x, int y, int width);
void ShowExtendedStrFit(int x, int y, const wchar_t *str, int str_width, int area_width, Config, int color = GetColor(255, 255, 255), int shadow_color = GetColor(0, 0, 0));
void ShowExtendedStrFitToHandle(int x, int y, const wchar_t *str, int str_width, int area_width, Config, int FontHandle, int color = GetColor(255, 255, 255), int shadow_color = GetColor(0, 0, 0));
void ShowExtendedStrFitToHandleNoShadow(int x, int y, const wchar_t* str, int str_width, int area_width, Config config, int FontHandle, int color = GetColor(255, 255, 255));//領域(area)より大きい文字は縮小して表示する関数(ToHandle版)(影無し)

extern void show_cloud(int, XY *, double, double);
void DrawBoxWithLine(int x1, int y1, int x2, int y2, int color, int alphaBox = 80, int alphaLine=255);
void DrawNumber(int x, int y, int value, int interval, int showType, int zeroFilingDigit, int* NUMBER_IMAGE_HANDLE);
void DrawFloatNumber(int x, int y, int value, int interval, int zeroFilingDigit, double decimalShift, int* NUMBER_IMAGE_HANDLE, int NUMBER_DECIMAL_HANDLE);
extern int GetRainbowColor(double val, double range);

unsigned int colorRatio(int r, int g, int b);

#endif