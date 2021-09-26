import socket
import argparse
import pickle
import hashlib
import os
from ctypes import *

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('IRdata_path', type=str)
parser.add_argument('--run')
args = parser.parse_args()

password = "natureprhysmserver1.00"


class DataStructure(Structure):
    _fields_ = (
        ('playerName', c_wchar * 17),
        ('IR_Enable', c_int32),
        ('rivalID', c_uint32),
    )


def get_player_name():
    with open(os.path.join("save_data", "IR_Setting.dat"), "rb") as f:
        data = DataStructure()
        f.readinto(data)

    # print(f"player_name:{data.playerName}")
    # print(f"IR_Enable:{data.IR_Enable}")
    # print(f"rivalID:{data.rivalID}")
    return data.playerName

def get_hash():
    # ファイル を バイナリーモード で開く
    with open(args.nps_file_path, 'rb') as file:
        # ファイルを読み取る
        fileData = file.read()
        # sha3_256
        hash_sha3_256 = hashlib.sha3_256(fileData).hexdigest()
        return hash_sha3_256


def authorization_password(s):
    s.send(bytes(password, 'utf-8'))
    # 応答を受け取る
    recv = s.recv(1024)
    print(recv.decode("utf-8"))
    if recv == b"ok":
        return


def send_data(data):
    print(data)
    msg = pickle.dumps(data)
    print(msg)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    if args.local:  # ローカル
        s.connect(('192.168.3.6', 50001))
    else:  # グローバル
        s.connect(('nature-prhysm.f5.si', 50001))

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # パスワード送信
    authorization_password(s)

    # サーバに送信
    s.send(bytes("send", 'utf-8'))
    recv = s.recv(1024)
    print(recv.decode("utf-8"))
    if recv == b"ok":
        s.send(msg)


def getid():
    with open(os.path.join("save_data", "IR_id.dat"), "rb") as f:
        msg = f.read()
        data = pickle.loads(msg)
        print(data)
        return data["id"], data["authorization_key"]


def load(path):
    with open(path, "rb") as f:
        msg = f.read()
        data = pickle.loads(msg)
        print(data)
        return data


def number_to_clear_type(number):
    if number == -2:
        return "NO PLAY"
    if number == -1:
        return "PLAY"
    if number == 0:
        return "FAILED"
    if number == 1:
        return "EASY CLEARED"
    if number == 2:
        return "CLEARED"
    if number == 3:
        return "HARD CLEARED"
    if number == 4:
        return "SUPER HARD CLEARED"
    if number == 5:
        return "FULL COMBO"
    if number == 6:
        return "PERFECT"


def number_to_rank(number):
    if number == 0:
        return "NONE"
    if number == 1:
        return "F"
    if number == 2:
        return "E"
    if number == 3:
        return "D"
    if number == 4:
        return "C"
    if number == 5:
        return "B"
    if number == 6:
        return "A"
    if number == 7:
        return "S"


def main():
    hash_sha3_256 = get_hash()
    print('sha3_256 : ' + hash_sha3_256)
    player_name = get_player_name()
    data = load(args.IRdata_path)
    if data["hash"] == hash_sha3_256:
        data["player_name"] = player_name
        data["clear_type"] = number_to_clear_type(data["clear_type"])
        data["rank"] = number_to_rank(data["rank"])
        send_data(data)


if __name__ == "__main__":
    if args.run == "223210":
        main()
        get_player_name()
