import argparse
import np_request_operator as operator
from request_operator import RequestMethod
from ctypes import *
import os
import pickle

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('--run')
args = parser.parse_args()

def write(data):
    with open(os.path.join("save_data", "IR_id.dat"), "wb") as f:
        f.write(data)

def main():
    variant = operator.Variant.Pr
    if args.local:
        variant = operator.Variant.Dev

    op = operator.NPServerRequestOperator(variant=variant)
    result = op.request(cgi='get_id.php',method=RequestMethod.POST)

    if not result.isOK():
        # 通信失敗時は何もしない
        # print("result is not OK reson:",result.get_NG_reason())
        # print("status code:",result.get_status_code())
        # print("data:",result.get_data())
        return
    
    response = result.get_data()
    
    ir_data = {"id":response['id'], "authorization_key":response['authorization_key']}
    msg = pickle.dumps(ir_data)
    write(msg)

if __name__ == "__main__":
    if args.run == "223210":
        main()
