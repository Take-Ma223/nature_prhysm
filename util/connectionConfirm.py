import argparse
import np_request_operator as operator
from request_operator import RequestMethod
import os
from ctypes import *

parser = argparse.ArgumentParser()
parser.add_argument('--local', action="store_true")
parser.add_argument('--run')
args = parser.parse_args()

CONNECTION_SUCCESS = 0  # 接続成功
CONNECTION_SERVER_NOT_FOUND = 1  # サーバーが見つからない
CONNECTION_VERSION_ERROR = 2  # サーバーには繋がったがバージョンが違う


CONNECTION_RESULT_SAVE_PATH = os.path.join('save_data', 'tmp')
CONNECTION_RESULT_SAVE_FILE_NAME = 'ConnectState'
class DataStructure(Structure):
    _fields_ = (
        ('ConnectionState', c_int32),
    )

class ConnectionResultWriter():
    def write(self, data:int):
        # フォルダ作成
        os.makedirs(CONNECTION_RESULT_SAVE_PATH, exist_ok=True)
        with open(os.path.join(CONNECTION_RESULT_SAVE_PATH, CONNECTION_RESULT_SAVE_FILE_NAME), "wb") as f:
            f.write(DataStructure(data))
            print(DataStructure(data))

if __name__ == "__main__":
    # connection_confirm()
    
    variant = operator.Variant.Pr
    if args.local:
        variant = operator.Variant.Dev

    op = operator.NPServerRequestOperator(variant=variant)
    result = op.request(cgi='connection.php',method=RequestMethod.GET)
    
    writer = ConnectionResultWriter()

    if result.isOK():
        writer.write(CONNECTION_SUCCESS)
    elif result.get_NG_reason() == operator.NPServerRequestNGReason.SERVER_VERSION_INVALID:
        writer.write(CONNECTION_VERSION_ERROR)
    else:
        writer.write(CONNECTION_SERVER_NOT_FOUND)


    print(result)
    print('isOK:', result.isOK())
    print('NG_reason:', result.get_NG_reason())
    print('data:', result.get_data())
