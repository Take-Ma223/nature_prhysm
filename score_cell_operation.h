#include"STRUCT_EDIT_SCORE.h"
#ifndef _SCORE_CELL_OPERATION
#define _SCORE_CELL_OPERATION

void score_insert_cell(SCORE_CELL *score_cell,int);//score_cellポインタの後に音符を挿入

void score_cell_write_note(SCORE_CELL* p, int column, NoteColor color, NoteGroup group, BOOL bright, BOOL LN_k = 0);//pポインタにcolumn列目の音符情報を格納 LN黒終点は省略可

void score_cell_write_command(SCORE_CELL *p, char kind, double val1 = 0, double val2 = 0, double val3 = 0, double val4 = 0);//pポインタにcolumn列目の音符情報を格納

void score_delete_cell(SCORE_CELL *score_cell);

void score_delete_cell_all(SCORE_CELL *p);//pの次のセルから全削除

SCORE_CELL* score_cell_find_before_note(SCORE_CELL *p, int column);//pポインタからbeforeをたどって最初にあるcolumn列のnoteのポインタを返す

SCORE_CELL* score_cell_find_next_note(SCORE_CELL *p, int column);//pポインタからnextをたどって最初にあるcolumn列のnoteのポインタを返す

SCORE_CELL* score_cell_find_before_command(SCORE_CELL *p, int column);//pポインタからbeforeをたどって最初にあるkind種類のコマンドのポインタを返す

SCORE_CELL* score_cell_find_command_before(SCORE_CELL *p, int kind);//pポインタから同じstepの範囲でbeforeをたどって最初にあるcommand(種類はkind)のポインタを返す 見つからなければNULL

SCORE_CELL* score_cell_find_command_next(SCORE_CELL* p, int kind);//pポインタから同じstepの範囲でnextをたどって最初にあるcommand(種類はkind)のポインタを返す 見つからなければNULL


void score_cell_delete_if_no_note(SCORE_CELL **p);//何もない行なら消す
#endif