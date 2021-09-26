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


def authorization_password(s):
    s.send(bytes(password, 'utf-8'))
    # 応答を受け取る
    recv = s.recv(1024)
    print(recv.decode("utf-8"))
    if recv == b"ok":
        return


def recv_data():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if args.local:  # ローカル
        s.connect(('192.168.3.6', 50001))
    else:  # グローバル
        s.connect(('nature-prhysm.f5.si', 50001))

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # パスワード送信
    authorization_password(s)

    # サーバに送信
    s.send(bytes("getid", 'utf-8'))

    # サーバから受信
    full_msg = b''
    msg = s.recv(1024)
    print(len(msg))
    full_msg += msg
    print(full_msg)
    # data = pickle.loads(full_msg)
    return full_msg


def write(data):
    with open(os.path.join("save_data", "IR_id.dat"), "wb") as f:
        f.write(data)
        print(data)


def main():
    data = recv_data()
    write(data)


if __name__ == "__main__":
    if args.run == "223210":
        main()
