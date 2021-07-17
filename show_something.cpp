#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"show_something.h"
#include"number_digit.h"

void show_str(int time, int time_base, int H_STR, int x, int y, int width) {//文字画像をスクロールして表示する
																							//width 画像の長さ
																							//660の長さの枠の中
	int length = 690;//表示領域

	//width = GetDrawStringWidth(str, strlen(str));
	DrawGraph(x - int((time - time_base)* 0.18) % (length + width), y, H_STR, TRUE);

	//if (config.ShowStrShadow == TRUE)DrawStringToHandle(x - int((time - time_base)* 0.18) % (length + width) + 2, y + 2, str, ShadowColor, FontHandle);//(影)
	//DrawStringToHandle(x - int((time - time_base)* 0.18) % (length + width), y, str, Color, FontHandle);//

}

void ShowExtendedStrFit(int x, int y, wchar_t *str, int str_width,int area_width, CONFIG config, int color ,int shadow_color) {//領域(area)より大きい文字は縮小して表示する関数
	if (str_width <= area_width) {//エリア内に収まっているときは普通に描画
		if (config.ShowStrShadow == TRUE)DrawString(int(x - ((double)str_width / 2)) + 2, y + 2, str, shadow_color);//(影)
		DrawString(int(x - ((double)str_width / 2)), y, str, color);//表示
	}
	else {//収まっていないときは縮小して描画
		if (config.ShowStrShadow == TRUE)DrawExtendString(int(x - ((double)area_width / 2)) + 2, y + 2, (double)area_width / (double)str_width, 1, str, shadow_color);
		DrawExtendString(int(x - ((double)area_width / 2)), y, (double)area_width / (double)str_width, 1, str, color);
	}
	return;
}

void ShowExtendedStrFitToHandle(int x, int y, wchar_t *str, int str_width, int area_width, CONFIG config, int FontHandle, int color, int shadow_color) {//領域(area)より大きい文字は縮小して表示する関数(ToHandle版)
	if (str_width <= area_width) {//エリア内に収まっているときは普通に描画
		if (config.ShowStrShadow == TRUE)DrawStringToHandle(int(x - ((double)str_width / 2)) + 2, y + 2, str, shadow_color,FontHandle);//(影)
		DrawStringToHandle(int(x - ((double)str_width / 2)), y, str, color, FontHandle);//表示(影)
	}
	else {//収まっていないときは縮小して描画
		if (config.ShowStrShadow == TRUE)DrawExtendStringToHandle(int(x - ((double)area_width / 2)) + 2, y + 2, (double)area_width / (double)str_width, 1, str, shadow_color, FontHandle);
		DrawExtendStringToHandle(int(x - ((double)area_width / 2)), y, (double)area_width / (double)str_width, 1, str, color, FontHandle);
	}
	return;
}

/*
int GetStrFrameColor(int color) {//文字の色から文字の縁を白か黒に決定

	return 0;
}
*/

void show_cloud(int HANDLE, XY *pos, double speed, double loop) {//雲をスクロールして表示する
	int i = 0;
	//pos->x = int(pos->x - (1 * speed)) % (1280);
	//for (i = 0; i < loop; i++) {
	pos->x = float(pos->x - (0.13 * loop * speed));
	//}
	//pos->x = int(pos->x) % 1280;

	while (pos->x < -1280) {
		pos->x += 1280;
	}

	DrawGraph(int(pos->x), int(pos->y), HANDLE, TRUE);//雲
	DrawGraph(int(pos->x + 1280), int(pos->y), HANDLE, TRUE);//雲2
}

void DrawBoxWithLine(int x1,int y1,int x2, int y2,int color, int alphaBox, int alphaLine) {//白い枠線付きの透過DrawBox
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaBox);
	DrawBox(x1, y1, x2, y2, color, TRUE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alphaLine);
	DrawBox(x1, y1, x2, y2, GetColor(255, 255, 255), FALSE);
}

void DrawNumber(int x, int y, int value, int interval, int showType, int zeroFilingDigit, int *NUMBER_IMAGE_HANDLE) {
	//正の整数数値を画像で表示(5桁まで)
	//interval:数値の間隔
	//showType 0:中心揃え 1:右揃え
	//zeroFilingDigit:0で埋める桁数 0を指定すると0で埋めない

	int xSize, ySize;
	GetGraphSize(NUMBER_IMAGE_HANDLE[0],&xSize, &ySize);
	int shiftX = xSize / 2;
	int shiftY = ySize / 2;

	int digit = int(log10(value)+1);//表示桁数を格納
	if (value == 0) {//0の時は1桁表示
		digit = 1;
	}

	if (zeroFilingDigit != 0) {
		digit = zeroFilingDigit;
	}

	int digitArray[8] = { 0,0,0,0,0,0,0,0 };
	number_digit(value, digitArray, 8);//数値を桁ごとに格納

	int i = 0;
	for (i = 0; i < digit; i++) {//数値を表示
		if (showType == 0) {//中心揃え
			DrawGraph(-shiftX + x - i * interval + int(((double)interval / 2) * (digit-1)), y, NUMBER_IMAGE_HANDLE[digitArray[i]], TRUE);
		}
		else if (showType == 1) {//右揃え
			DrawGraph(-shiftX + x - i * interval,                                           y, NUMBER_IMAGE_HANDLE[digitArray[i]], TRUE);
		}
	}
}

void DrawFloatNumber(int x, int y, int value, int interval, int zeroFilingDigit, int decimalShift, int* NUMBER_IMAGE_HANDLE, int NUMBER_DECIMAL_HANDLE) {//下2桁 小数表示 右揃え
	//正の整数数値を画像で表示(5桁まで)
	//interval:数値の間隔
	//zeroFilingDigit:0で埋める桁数 0を指定すると0で埋めない
	//decimalShift:実数部と小数部の間のスペースを何文字分開けるか

	int xSize, ySize;
	GetGraphSize(NUMBER_IMAGE_HANDLE[0], &xSize, &ySize);
	int shiftX = xSize / 2;
	int shiftY = ySize / 2;

	int digit = int(log10(value) + 1);//表示桁数を格納
	if (value == 0) {//0の時は3桁表示
		digit = 3;
	}

	if (zeroFilingDigit != 0) {
		digit = zeroFilingDigit;
	}

	int digitArray[5] = { 0,0,0,0,0 };
	number_digit(value, digitArray, 5);//数値を桁ごとに格納


	int i = 0;
	for (i = 0; i < digit; i++) {//数値を表示
	    //右揃え
		if (i <  2)DrawGraph(-shiftX + x - i * interval,                          y, NUMBER_IMAGE_HANDLE[digitArray[i]], TRUE);//小数点以下
		if (i >= 2)DrawGraph(-shiftX + x - i * interval - (decimalShift * xSize), y, NUMBER_IMAGE_HANDLE[digitArray[i]], TRUE);//整数部

		if (i == 1) {//小数点表示
			DrawGraph(-shiftX + x - i * interval - ((decimalShift + 1) * xSize) / 2, y, NUMBER_DECIMAL_HANDLE, TRUE);
		}
	}
}

int GetRainbowColor(double val, double range) {//虹色のコードを得る関数 0~rangeの間で虹色に変化
	int color = 0;
	double ind = 0;
	double th = range / 6;
	if (val <= th) {//R
		ind = (double)255 * val / th;
		color = GetColor(255, (int)ind, 0);
	}
	else if (val <= th * 2) {//Y
		ind = (double)255 * (val - th) / th;
		color = GetColor(255 - (int)ind, 255, 0);
	}
	else if (val <= th * 3) {//G
		ind = (double)255 * (val - 2 * th) / th;
		color = GetColor(0, 255, (int)ind);
	}
	else if (val <= th * 4) {//C
		ind = (double)255 * (val - 3 * th) / th;
		color = GetColor(0, 255 - (int)ind, 255);
	}
	else if (val <= th * 5) {//B
		ind = (double)255 * (val - 4 * th) / th;
		color = GetColor((int)ind, 0, 255);
	}
	else if (val <= th * 6) {//M
		ind = (double)255 * (val - 5 * th) / th;
		color = GetColor(255, 0, 255 - (int)ind);
	}
	else {
		color = GetColor(255, 0, 0);
	}
	return color;
}

