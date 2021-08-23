import socket
import argparse
import pickle
import hashlib
import os

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('nps_folder_path', type=str)
parser.add_argument('difficulty', type=int)
parser.add_argument('season', type=int)
parser.add_argument('--rainbow', action="store_true")
parser.add_argument('clear_type', type=int)  # 数値で渡す
parser.add_argument('rank', type=int)  # 数値で渡す
parser.add_argument('score', type=int)
parser.add_argument('sky_perfect', type=int)
parser.add_argument('perfect', type=int)
parser.add_argument('good', type=int)
parser.add_argument('miss', type=int)
parser.add_argument('min_miss', type=int)
parser.add_argument('max_combo', type=int)
parser.add_argument('play_count', type=int)
parser.add_argument('--run')
args = parser.parse_args()


def get_nps_information():
    with open(args.nps_file_path, "r", encoding="utf-16") as f:
        datalist = f.readlines()
        for data in datalist:
            split = data.split(":", 1)
            if split[0] == "#TITLE":
                title = split[1].replace('\n', '')
            if split[0] == "#ARTIST":
                artist = split[1].replace('\n', '')
            if split[0] == "#GENRE":
                genre = split[1].replace('\n', '')
            if split[0] == "#NOTES":
                break
    return title, artist, genre


def get_hash():
    # ファイル を バイナリーモード で開く
    with open(args.nps_file_path, 'rb') as file:
        # ファイルを読み取る
        fileData = file.read()
        # sha3_256
        hash_sha3_256 = hashlib.sha3_256(fileData).hexdigest()
        return hash_sha3_256


def getid():
    with open(os.path.join("save_data", "IR_id.dat"), "rb") as f:
        msg = f.read()
        data = pickle.loads(msg)
        print(data)
        return data["id"], data["authorization_key"]


def save(data, path):
    with open(path, "wb") as f:
        msg = pickle.dumps(data)
        f.write(msg)
        print(data)


def load(path):
    with open(path, "rb") as f:
        msg = f.read()
        data = pickle.loads(msg)
        print(data)
        return data


def main():
    hash_sha3_256 = get_hash()
    print('sha3_256 : ' + hash_sha3_256)
    id, authorization_key = getid()
    title, artist, genre = get_nps_information()

    # 受け取ったスコア
    data = {"hash": hash_sha3_256,
            "id": id,
            "authorization_key": authorization_key,
            "difficulty": args.difficulty,
            "season": args.season,
            "rainbow": args.rainbow,
            "title": title,
            "genre": genre,
            "artist": artist,
            "clear_type": args.clear_type,
            "rank": args.rank,
            "score": args.score,
            "sky_perfect": args.sky_perfect,
            "perfect": args.perfect,
            "good": args.good,
            "miss": args.miss,
            "min_miss": -1,
            "max_combo": args.max_combo,
            "play_count": args.play_count
            }

    # フォルダ作成
    os.makedirs(os.path.join(args.nps_folder_path, "IRdata"), exist_ok=True)

    # IRデータの保存パス決定 (IRdata/1_r)
    if args.rainbow:
        path = os.path.join(args.nps_folder_path, "IRdata", str(args.difficulty) + "_r")
    else:
        path = os.path.join(args.nps_folder_path, "IRdata", str(args.difficulty))

    # データ確認
    isExistFile = os.path.exists(path)
    if isExistFile:  # 2回目以降の保存の時は更新して保存
        bestData = load(path)
        isSameID = bestData["id"] == data["id"]
        if isSameID:
            if bestData["clear_type"] < data["clear_type"]:  # クリア状態更新していたとき
                bestData["clear_type"] = data["clear_type"]
            if bestData["rank"] < data["rank"]:  # ランク更新していたとき
                bestData["rank"] = data["rank"]
            if bestData["score"] < data["score"]:  # スコア更新していた時
                bestData["score"] = data["score"]
                bestData["sky_perfect"] = data["sky_perfect"]
                bestData["perfect"] = data["perfect"]
                bestData["good"] = data["good"]
                bestData["miss"] = data["miss"]
            if data["clear_type"] == -1 or data["clear_type"] >= 2:  # //クリア、PLAYしたとき
                if bestData["min_miss"] == -1:  # 最初の最小ミス保存
                    bestData["min_miss"] = data["miss"]
                if bestData["min_miss"] > data["miss"]:  # 最小ミス更新
                    bestData["min_miss"] = data["miss"]
            if bestData["max_combo"] < data["max_combo"]:  # 最大コンボ更新
                bestData["max_combo"] = data["max_combo"]
            bestData["id"] = data["id"]
            bestData["authorization_key"] = data["authorization_key"]
            bestData["play_count"] = data["play_count"]
            save(bestData, path)
        else:  # IDが違う時はそのまま保存
            if data["clear_type"] == -1 or data["clear_type"] >= 2:  # //クリア、PLAYしたとき　MinMiss保存
                data["min_miss"] = data["miss"]
            save(data, path)
    else:  # 初回保存の時はそのまま保存
        if data["clear_type"] == -1 or data["clear_type"] >= 2:  # //クリア、PLAYしたとき　MinMiss保存
            data["min_miss"] = data["miss"]
        save(data, path)


if __name__ == "__main__":
    if args.run == "223210":
        main()
