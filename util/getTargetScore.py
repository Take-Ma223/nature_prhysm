import argparse
from ctypes import *
import os

from np_hash import NPHash
import np_request_operator as operator
from request_operator import RequestMethod

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

def write(data):
    # フォルダ作成
    os.makedirs(os.path.join(args.nps_folder_path, "IRdata"), exist_ok=True)
    with open(os.path.join(args.nps_folder_path, "IRdata", "TargetScore"), "wb") as f:
        f.write(DataStructure(data))
        print(DataStructure(data))


def main():
    np_hash = NPHash()
    hash_sha3_256 = np_hash.get_nps_hash(args.nps_file_path)
    print('sha3_256 : ' + hash_sha3_256)
    if args.rainbow:
        rainbow = "1"
    else:
        rainbow = "0"

    mode = args.mode
    if mode == 0:
        target_mode='Rival'
    elif mode == 1:
        target_mode='Average'
    elif mode == 2:
        target_mode='Median'
    elif mode == 3:
        target_mode='NextRank'
    elif mode == 4:
        target_mode='Top'

    data = {'chart_id':hash_sha3_256, 'rainbow':rainbow, 'target_mode':target_mode, 'rival_id':args.rivalID, 'best_score':args.score}

    variant = operator.Variant.Pr
    if args.local:
        variant = operator.Variant.Dev

    op = operator.NPServerRequestOperator(variant=variant)
    result = op.request(cgi='get_target_score.php',method=RequestMethod.POST, post_data=data)


    if result.isOK():
        print(result.get_data())
        print(type(result.get_data()['score']))
        target_score = int(float(result.get_data()['score']))
        print(target_score)
        write(target_score)


if __name__ == "__main__":
    if args.run == "223210":
        main()
