#include"DxLib.h"
#include<math.h>
#include<sys/stat.h>
#include<string.h>
#include"STRUCT_RESULT.h"

int make_result_hash(RESULT res) {//リザルトデータのハッシュ作成
	int hash = 0;
	int hash_MAX = 999999999;
	hash += (9933926 * res.clear) % hash_MAX;
	hash += (1357794 * res.difficulty) % hash_MAX;
	hash += (6432253 * res.good) % hash_MAX;
	hash += (6739052 * res.hash) % hash_MAX;
	hash += (5075147 * res.miss) % hash_MAX;
	hash += (1325445 * res.perfect) % hash_MAX;
	hash += (8883535 * res.play_counter) % hash_MAX;
	hash += (1357338 * res.rank) % hash_MAX;
	hash += (1335529 * res.score) % hash_MAX;
	return hash;
	INT_MAX;
}