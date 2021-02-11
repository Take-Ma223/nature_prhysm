#include"STRUCT_XY.h"
#ifndef _STRUCT_CELL
#define _STRUCT_CELL

typedef struct _cell {//音符を叩いた時に後ろに落ちるもののデータ
	char num = 0;//表示する画像の種類(0~7)
	XY xypos;//xy座標(横揺れの中心xはレーンによって決める)
	double angle = 0;//回転角度
	double angle_offset = 0;//角度のオフセット(最初にランダムに決める)
	double x_sin_angle = 0;//横揺れ位相
	double x_sin_offset = 0;//sinで横揺れするオフセット位相(最初にランダムに決める)
	int x_sin_amp = 100;//横揺れの振幅
	double y_speed = 2.406;//落ちる速度
	struct _cell *next = NULL;//次のセルへのポインタ
} CELL;

#endif