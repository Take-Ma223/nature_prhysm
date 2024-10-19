import socket
import argparse
import pickle
import hashlib
import os
from ctypes import *

from np_hash import NPHash
import np_request_operator as operator
from request_operator import RequestMethod

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('IRdata_path', type=str)
parser.add_argument('--run')
args = parser.parse_args()

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

def main():
    np_hash = NPHash()
    hash_sha3_256 = np_hash.get_nps_hash(args.nps_file_path)
    print('sha3_256 : ' + hash_sha3_256)
    player_name = get_player_name()
    try:
        id, authorization_key = getid()
    except FileNotFoundError as e:
        print(e)
    else:
        try:
            data = load(args.IRdata_path)
        except FileNotFoundError as e:
            print(args.IRdata_path+" not found")
        else:
            if data["hash"] == hash_sha3_256 and data["id"] == id:  # 現在のidと違うidのスコアデータは送信しない
                data["player_name"] = player_name
                variant = operator.Variant.Pr
                if args.local:
                    variant = operator.Variant.Dev

                op = operator.NPServerRequestOperator(variant=variant)
                result = op.request(cgi='send.php',method=RequestMethod.POST, post_data=data)

if __name__ == "__main__":
    if args.run == "223210":
        main()

