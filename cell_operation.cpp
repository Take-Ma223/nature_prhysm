//音符を叩いた時に後ろに落ちるものを制御する連結リスト操作関数

#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_CELL.h"
#include"cell_operation.h"

void insert_cell(CELL *p, char num, float x, double angle_offset, double x_sin_offset, int x_sin_amp, double y_speed) {//pの次にセルを挿入
	CELL *new_cell;
	new_cell = (CELL *)malloc(sizeof(CELL));
	new_cell->num = num;
	new_cell->xypos.x = x;
	new_cell->angle = 0;
	new_cell->angle_offset = angle_offset;
	new_cell->x_sin_offset = x_sin_offset;
	new_cell->xypos.y = -50;
	new_cell->x_sin_angle = 0;
	new_cell->x_sin_amp = x_sin_amp;
	new_cell->y_speed = y_speed;

	new_cell->next = p->next;
	p->next = new_cell;
}

void delete_cell(CELL *p) {//pの次のセルを削除
	CELL *q;
	q = p->next;
	p->next = p->next->next;//一個飛ばして繋ぐ
	free(q);
}

void search_cell_all(CELL *p, CELL *before, CELL *insert, CELL **insert_pp, int *H_FALL, double LOOP_passed_time, double BPMScale) {//pからセルの探索(表示と削除)
	CELL *p_before;//delete_cellで使う用ポインタ
	p_before = before;//コピー

	while (p->next != NULL) {
		//printf("%s\n", p->data);
		p->xypos.y += float(BPMScale * 0.08*p->y_speed *LOOP_passed_time);//位置を下に
		p->angle += (BPMScale * 0.001*LOOP_passed_time) * sin(p->x_sin_angle + p->x_sin_offset + p->x_sin_amp);//回転増加
		p->x_sin_angle += BPMScale * 0.0005*LOOP_passed_time;//横振動増加

															 //描画


		DrawRotaGraph2(int(p->xypos.x + sin(p->x_sin_angle + p->x_sin_offset) * p->x_sin_amp)//x揺らし
			, int(p->xypos.y)
			, 50
			, 50
			, 1
			, p->angle + p->angle_offset
			, H_FALL[p->num], TRUE, FALSE);


		if (p->xypos.y >= 800) {//pが画面の下に消えてたら
			p = p->next;
			delete_cell(p_before);//セルの削除

								  //*insert = *p_before;
								  //insert->next = p_before->next;
		}
		else {//次のセルへ
			p_before = p;
			p = p->next;
		}
	}
	if (p->next == NULL) {//最後のセルだった
		p->xypos.y += float(BPMScale * 0.08*p->y_speed*LOOP_passed_time);//位置を下に
		p->angle += (BPMScale * 0.001*LOOP_passed_time) * sin(p->x_sin_angle + p->x_sin_offset + p->x_sin_amp);//回転増加
		p->x_sin_angle += BPMScale * 0.0005*LOOP_passed_time;//横振動増加


															 //printfDx("p->next:%x\n", p->next);
															 //printfDx("before:%x\n", before);

															 //描画
		DrawRotaGraph2(int(p->xypos.x + sin(p->x_sin_angle + p->x_sin_offset) * p->x_sin_amp) //x揺らし
			, int(p->xypos.y)
			, 50
			, 50
			, 1
			, p->angle + p->angle_offset
			, H_FALL[p->num], TRUE, FALSE);


		if (p->xypos.y >= 800) {//pが画面の下に消えてたら
			delete_cell(p_before);//セルの削除
			*insert_pp = p_before;
			//*before = *p_before;

			//insert->next = p_before->next;
			//before->next = p_before->next;

			//リストが空になったらp->next=NULL


		}
		//printf("%s\n", p->data);
	}
}

void delete_cell_all(CELL *p, CELL *before, CELL *insert) {//pの後ろからセルの全削除
	CELL *p_before;//delete_cellで使う用ポインタ
	p_before = before;

	while (p->next != NULL) {
		p = p->next;
		delete_cell(p_before);//セルの削除
	}
	if (p->next == NULL) {//最後のセルだった
		insert = p_before;
		delete_cell(p_before);//セルの削除
	}
}

int img_num(int season) {//季節を入力すると後ろに落とす画像の番号を返す
	if (season == 0) {
		return GetRand(7);//無季節は0~7
	}
	else if (season == 1) {//春
		return GetRand(1);
	}
	else if (season == 2) {//夏
		return GetRand(1) + 2;
	}
	else if (season == 3) {//秋
		return GetRand(1) + 4;
	}
	else if (season == 4) {//冬
		return GetRand(1) + 6;
	}

	return 0;
}