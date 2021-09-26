import socket
import argparse
import pickle
from ctypes import *
import os


parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('--run')
args = parser.parse_args()

password = "natureprhysmserver1.00"

CONNECTION_SUCCESS = 0  # 接続成功
CONNECTION_SERVER_NOT_FOUND = 1  # サーバーが見つからない
CONNECTION_VERSION_ERROR = 2  # サーバーには繋がったがバージョンが違う


class DataStructure(Structure):
    _fields_ = (
        ('ConnectionState', c_int32),
    )


def authorization_password():

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if args.local:  # ローカル
            s.connect(('192.168.3.6', 50001))
        else:  # グローバル
            s.connect(('nature-prhysm.f5.si', 50001))
    except socket.timeout:
        write(CONNECTION_SERVER_NOT_FOUND)
        return

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    s.send(bytes(password, 'utf-8'))
    try:
        # 応答を受け取る
        recv = s.recv(1024)
        print(recv.decode("utf-8"))
        if recv == b"ok":
            write(CONNECTION_SUCCESS)
            return
    except socket.timeout:
        write(CONNECTION_VERSION_ERROR)
        return


def write(data):
    # フォルダ作成
    os.makedirs(os.path.join("save_data", "tmp"), exist_ok=True)
    with open(os.path.join("save_data", "tmp", "ConnectState"), "wb") as f:
        f.write(DataStructure(data))
        print(DataStructure(data))


def main():
    socket.setdefaulttimeout(0.1)
    authorization_password()


if __name__ == "__main__":
    if args.run == "223210":
        main()
