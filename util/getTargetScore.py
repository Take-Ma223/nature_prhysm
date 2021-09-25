import socket
import argparse
import pickle
from ctypes import *
import os
import hashlib

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('nps_folder_path', type=str)  # IRdataまでのパス
parser.add_argument('--rainbow', action="store_true")
parser.add_argument('mode', type=int)  # 0:RIVAL 1:平均 2:中央値スコア 3:次の順位のスコア 4:全一スコア
parser.add_argument('score', type=int)  # プレイヤーのハイスコア 次の順位のスコアを表示するときに使用
parser.add_argument('rivalID', type=int)  # Riavlに指定したプレイヤーのスコアを表示するときに使用
parser.add_argument('--run')
args = parser.parse_args()


class DataStructure(Structure):
    _fields_ = (
        ('score', c_int32),
    )


def get_hash():
    # ファイル を バイナリーモード で開く
    with open(args.nps_file_path, 'rb') as file:
        # ファイルを読み取る
        fileData = file.read()
        # sha3_256
        hash_sha3_256 = hashlib.sha3_256(fileData).hexdigest()
        return hash_sha3_256


def recv_data():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if args.local:  # ローカル
        s.connect(('192.168.3.6', 50001))
    else:  # グローバル
        s.connect(('126.89.71.177', 50001))

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # サーバに送信
    s.send(bytes("getTargetScore", 'utf-8'))

    # 応答を受け取る
    recv = s.recv(1024)
    print(recv.decode("utf-8"))
    if recv == b"ok":
        data = {"hash": get_hash(),
                "rainbow": args.rainbow,
                "mode": args.mode,
                "score": args.score,
                "rivalID": args.rivalID,
                }
        msg = pickle.dumps(data)
        s.send(msg)
        # サーバからスコアの値を受信
        full_msg = b''
        msg = s.recv(1024)
        print(len(msg))
        full_msg += msg
        print(full_msg)
        score = pickle.loads(full_msg)
        return score["score"]
    else:
        return 0


def write(data):
    # フォルダ作成
    os.makedirs(os.path.join(args.nps_folder_path, "IRdata"), exist_ok=True)
    with open(os.path.join(args.nps_folder_path, "IRdata", "TargetScore"), "wb") as f:
        f.write(DataStructure(data))
        print(DataStructure(data))


def main():
    socket.setdefaulttimeout(0.1)
    data = recv_data()
    print("data:", data)
    write(int(data))


if __name__ == "__main__":
    if args.run == "223210":
        main()
