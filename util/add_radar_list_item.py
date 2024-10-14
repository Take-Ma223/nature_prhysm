import argparse
import pickle
import os
from np_hash import NPHash

# 楽曲DBマスターデータ設定用CSVの生成
# 公式譜面のフラグとレーダー値の設定を行う用途
# CSVの一項目を書き込みます

parser = argparse.ArgumentParser()
parser.add_argument('nps_file_path', type=str)
parser.add_argument('radar_global', type=int)
parser.add_argument('radar_local', type=int)
parser.add_argument('radar_chain', type=int)
parser.add_argument('radar_unstability', type=int)
parser.add_argument('radar_streak', type=int)
parser.add_argument('radar_color', type=int)
parser.add_argument('--run')
args = parser.parse_args()

def main():
    np_hash = NPHash()
    hash_sha3_256 = np_hash.get_nps_hash(args.nps_file_path)

    f = open("radar.csv","a")
    f.write(hash_sha3_256+","+str(args.radar_global)+","+str(args.radar_local)+","+str(args.radar_chain)+","+str(args.radar_unstability)+","+str(args.radar_streak)+","+str(args.radar_color)+"\n")
    

if __name__ == "__main__":
    if args.run == "223210":
        main()
