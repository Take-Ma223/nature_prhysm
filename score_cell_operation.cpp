#include"STRUCT_EDIT_SCORE.h"

void score_insert_cell(SCORE_CELL *p,int step){//pポインタの後にタイミング情報だけのセルを挿入(p)(a)→(p)(new)(a)になる
	SCORE_CELL *new_cell;//挿入するセル
	new_cell = (SCORE_CELL *)malloc(sizeof(SCORE_CELL));
		
	new_cell->next = p->next;
	new_cell->before = p;

	if(p->next!=NULL)p->next->before = new_cell;//2つのセルの間に挿入するとき
	p->next = new_cell;

	new_cell->step = step;

	//score_cell = new_cell;//挿入位置を進める
}

void score_cell_write_note(SCORE_CELL* p, int column, NoteColor color, NoteGroup group, BOOL bright, BOOL LN_k = 0) {//pポインタにcolumn列目の音符情報を格納 LN黒終点は省略可
	p->content = 1;
	p->data.note.color[column] = color;
	p->data.note.group[column] = group;
	p->data.note.isBright[column] = bright;
	p->data.note.LN_k[column] = LN_k;
	return;
}

void score_cell_write_command(SCORE_CELL *p, char kind, double val1 = 0, double val2 = 0, double val3 = 0, double val4 = 0) {//pポインタに命令情報を格納
	p->content = 2;
	p->data.command.kind = kind;
	p->data.command.value[0] = val1;
	p->data.command.value[1] = val2;
	p->data.command.value[2] = val3;
	p->data.command.value[3] = val4;
	
	return;
}

void score_delete_cell(SCORE_CELL *p) {//pの次のセルを削除(p)(q)(a)→(p)(a)
	SCORE_CELL *q;
	q = p->next;
    
	
	if (p->next->next!=NULL) {//pの次が2つ以上あるとき
		p->next->next->before = p;
	}
	p->next = p->next->next;//一個飛ばして繋ぐ
	free(q);
	
	return;
}

void score_delete_cell_all(SCORE_CELL *p) {//pの次のセルから全削除
	while (p->next != NULL) {
		score_delete_cell(p);//セルの削除
	}
}

SCORE_CELL* score_cell_find_before_note(SCORE_CELL *p,int column) {//pポインタからbeforeをたどって最初にあるcolumn列のnoteのポインタを返す
	while(1){
		p = p->before;
	
		if (p == NULL)break;
		if (p->content == 1 && p->data.note.color[column] != NoteColor::NONE)break;//なんかの音符がある行が見つかった

	};
	return p;//見つからなければNULL
}

SCORE_CELL* score_cell_find_next_note(SCORE_CELL *p, int column) {//pポインタからnextをたどって最初にあるcolumn列のnoteのポインタを返す
	while (1) {
		p = p->next;

		if (p == NULL)break;
		if (p->content == 1 && p->data.note.color[column] != NoteColor::NONE)break;//なんかの音符がある行が見つかった

	};
	return p;//見つからなければNULL
}

SCORE_CELL* score_cell_find_before_command(SCORE_CELL *p, int kind) {//pポインタからbeforeをたどって最初にあるkind種類のコマンドのポインタを返す
	while (1) {
		p = p->before;

		if (p == NULL)break;
		if (p->content == 2 && p->data.command.kind == kind)break;//なんかのkindの命令がある行が見つかった

	};
	return p;//見つからなければNULL
}

SCORE_CELL* score_cell_find_command_before(SCORE_CELL *p, int kind) {//pポインタから同じstepの範囲でbeforeをたどって最初にあるcommand(種類はkind)のポインタを返す 見つからなければNULL
	int range_step = p->step;//探すステップ範囲
	if (p->content == 2 && p->data.command.kind == kind)return p;//最初からなんかのkindの命令がある行が見つかった

	while (1) {
		p = p->before;
		if (p->step != range_step) {//stepの範囲には無かった
			return NULL;//見つからなければNULL
		}
		if (p == NULL)break;
		if (p->content == 2 && p->data.command.kind == kind)break;//なんかのkindの命令がある行が見つかった

	};
	return p;//見つからなければNULL
}

SCORE_CELL* score_cell_find_command_next(SCORE_CELL* p, int kind) {//pポインタから同じstepの範囲でbeforeをたどって最初にあるcommand(種類はkind)のポインタを返す 見つからなければNULL
	int range_step = p->step;//探すステップ範囲
	if (p->content == 2 && p->data.command.kind == kind)return p;//最初からなんかのkindの命令がある行が見つかった

	while (1) {
		p = p->next;
		if (p->step != range_step) {//stepの範囲には無かった
			return NULL;//見つからなければNULL
		}
		if (p->content == 3)break;
		if (p->content == 2 && p->data.command.kind == kind)break;//なんかのkindの命令がある行が見つかった

	};
	return p;//見つからなければNULL
}

void score_cell_delete_if_no_note(SCORE_CELL **p) {//何もない行なら消す ポインタの値を変更するのでポインタのポインタを引数にする
	SCORE_CELL *delete_cell = (*p)->before;//消す用
	
	if ((*p)->data.note.color[0] == NoteColor::NONE
		&& (*p)->data.note.color[1] == NoteColor::NONE
		&& (*p)->data.note.color[2] == NoteColor::NONE
		&& (*p)->data.note.color[3] == NoteColor::NONE) {//何もない行なら

		*p = (*p)->before;//beforeに移動
		score_delete_cell(delete_cell);//pの行を消す
		
	}
}