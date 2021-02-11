#include"STRUCT_CELL.h"

#ifndef _CELL_OPERATION
#define _CELL_OPERATION

extern void insert_cell(CELL *, char, float, double, double, int, double);//pの次にセルを挿入
extern void delete_cell(CELL);//pの次のセルを削除
extern void search_cell_all(CELL *, CELL *, CELL *, CELL **, int *, double, double);//pからセルの探索
extern void delete_cell_all(CELL *, CELL *, CELL *);//pからセルの削除
extern int img_num(int);//季節を入力すると後ろに落とす画像の番号を返す

#endif