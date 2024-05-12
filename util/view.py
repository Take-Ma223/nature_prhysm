import argparse
from np_hash import NPHash
import np_request_operator as operator

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('nps_file_path', type=str)
parser.add_argument('--rainbow', action="store_true")
parser.add_argument('--run')

args = parser.parse_args()

def main():
    np_hash = NPHash()
    hash_sha3_256 = np_hash.get_nps_hash(args.nps_file_path)
    print('sha3_256 : ' + hash_sha3_256)
    if args.rainbow:
        rainbow = "1"
    else:
        rainbow = "0"
    
    query_chart_id = "chart_id="+hash_sha3_256
    query_rainbow = "rainbow="+rainbow 

    op = operator.NPServerRequestOperator(variant=operator.Variant.Dev)
    result = op.open_with_browser(cgi='view.php?'+query_chart_id+'&'+query_rainbow)

if __name__ == "__main__":
    if args.run == "223210":
        main()
