import socket
import argparse
import pickle
import hashlib
import os

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('nps_folder_path', type=str)  # IRdataまでのパス
parser.add_argument('difficulty', type=int)
parser.add_argument('season', type=int)
parser.add_argument('--rainbow', action="store_true")
parser.add_argument('--onlyOption', action="store_true")
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
        title = ""
        artist = ""
        genre = ""

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
    title, artist, genre = get_nps_information()
    try:
        id, authorization_key = getid()
    except FileNotFoundError as e:
        print(e)
    else:
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
                "min_miss": args.min_miss,
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

        # ONLYオプションを使っていた場合の前処理 スコアを更新しないようにする
        if args.onlyOption:
            data["rank"] = 0
            data["score"] = 0
            data["sky_perfect"] = 0
            data["perfect"] = 0
            data["good"] = 0
            data["miss"] = 0
            # data["clear_type"] = 0
            data["max_combo"] = 0
            data["min_miss"] = -1

        # データ確認
        isExistFile = os.path.exists(path)
        if isExistFile:  # 2回目以降の保存の時は更新して保存
            bestData = load(path)
            isSameID = bestData["id"] == data["id"]
            isSameHash = bestData["hash"] == data["hash"]
            if isSameID and isSameHash:
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
                if (data["clear_type"] == -1 or data["clear_type"] >= 2) and not args.onlyOption:  # ONLYオプション無しでクリア、PLAYしたとき
                    if bestData["min_miss"] == -1:  # 最初の最小ミス保存
                        bestData["min_miss"] = data["miss"]
                    if bestData["min_miss"] > data["miss"]:  # 最小ミス更新
                        bestData["min_miss"] = data["miss"]
                    if data["min_miss"] != -1:  # 既存スコア送信の場合
                        bestData["min_miss"] = data["min_miss"]
                if bestData["max_combo"] < data["max_combo"]:  # 最大コンボ更新
                    bestData["max_combo"] = data["max_combo"]
                bestData["id"] = data["id"]
                bestData["authorization_key"] = data["authorization_key"]
                bestData["play_count"] = data["play_count"]
                save(bestData, path)
            else:  # IDが違う または 譜面が変更された時はそのまま保存
                if (data["clear_type"] == -1 or data["clear_type"] >= 2) and not args.onlyOption:  # ONLYオプション無しでクリア、PLAYしたとき　MinMiss保存
                    if data["min_miss"] == -1:  # 通常は必ず-1
                        data["min_miss"] = data["miss"]
                    else:
                        # 既存スコア送信の場合
                        data["min_miss"] = data["min_miss"]
                save(data, path)
        else:  # 初回保存の時はそのまま保存
            if (data["clear_type"] == -1 or data["clear_type"] >= 2) and not args.onlyOption:  # ONLYオプション無しでクリア、PLAYしたとき　MinMiss保存
                if data["min_miss"] == -1:  # 通常は必ず-1
                    data["min_miss"] = data["miss"]
                else:
                    # 既存スコア送信の場合
                    data["min_miss"] = data["min_miss"]
            save(data, path)


if __name__ == "__main__":
    if args.run == "223210":
        main()
