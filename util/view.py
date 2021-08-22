import socket
import argparse
import pickle
import hashlib
import os
import webbrowser

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('--rainbow', action="store_true")
parser.add_argument('--run')

args = parser.parse_args()

def get_hash():
    # ファイル を バイナリーモード で開く
    with open(args.nps_file_path, 'rb') as file:
        # ファイルを読み取る
        fileData = file.read()
        # sha3_256
        hash_sha3_256 = hashlib.sha3_256(fileData).hexdigest()
        return hash_sha3_256


def main():
    hash_sha3_256 = get_hash()
    print('sha3_256 : ' + hash_sha3_256)
    if args.rainbow:
        rainbow = "_rainbow"
    else:
        rainbow = ""

    if args.local:  # ローカル
        webbrowser.open("http://192.168.3.6"  +"/ranking/"+str(hash_sha3_256)+"/score_ranking"+rainbow+".html",new=0)
    else:  # グローバル
        webbrowser.open("http://126.89.71.177"+"/ranking/"+str(hash_sha3_256)+"/score_ranking"+rainbow+".html")


if __name__ == "__main__":
    if args.run == "223210":
        main()
