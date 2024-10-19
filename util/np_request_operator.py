import requests
import request_operator
import webbrowser
from enum import Enum
from typing import Tuple

NP_SERVER_ADDRESS_DEV = 'http://localhost/ranking/'
NP_SERVER_ADDRESS_PR = 'http://nature-prhysm.main.jp/ranking/'

CONNECTION_RESULT_KEY_SERVER_VERSION = 'server_version'
NP_SERVER_VERSION_NAME = "natureprhysmserver1.1.0"  # 接続可能なサーバーバージョン 

class Variant(Enum):
    Dev=0,
    Pr=1

class NPServerRequestNGReason(Enum):
    FAILED=0,  # 接続失敗
    HTTP_STATUS_NOT_200=1,  # HTTPステータスが200以外
    JSON_DECODE_ERROR=2,  # JSONデコードエラー
    SERVER_VERSION_NOT_FOUND=3  # サーバーバージョン情報無し
    SERVER_VERSION_INVALID=4  # サーバーバージョンが不正

class NPRequestResult():
    def __init__(self,result:bool, NG_reason:NPServerRequestNGReason=None, status_code:int=None, data:dict=None):
        self.__result = result
        self.__NG_reason = NG_reason
        self.__status_code = status_code
        self.__data = data

    def isOK(self)->bool:
        #  通信が正常に成功したかどうか True:成功 False:失敗
        return self.__result

    def get_NG_reason(self)->NPServerRequestNGReason:
        #  通信失敗時の理由を返す
        return self.__NG_reason
    
    def get_status_code(self)->int:
        #  ステータスコードを返す
        return self.__status_code
    
    def get_data(self)->dict:
        #  レスポンスデータを返す(JSONデコードデータ)
        return self.__data




class NPServerRequestOperator():
    def __init__(self, variant=Variant):
        self.variant=variant
        self.operator=request_operator.RequestOperator()

    def request(self, cgi:str, method:request_operator.RequestMethod, post_data:dict={},)->NPRequestResult:
        #結果はNPRequestResultで返る
        url = self.__get_url(cgi)
        self.__add_server_version_key(post_data)
        
        result, response = self.operator.request(url=url, method=method, data=post_data)
        if result!=request_operator.RequestResult.HTTP_STATUS_200:
            if result==request_operator.RequestResult.FAILED:
                # 接続失敗
                return NPRequestResult(result=False, NG_reason=NPServerRequestNGReason.FAILED)
            elif result==request_operator.RequestResult.HTTP_STATUS_NOT_200:
                # HTTPステータスが200以外
                return NPRequestResult(result=False, status_code=response.status_code, NG_reason=NPServerRequestNGReason.HTTP_STATUS_NOT_200)


        result_ok, response_data = self.__json_decode(response=response)
        if not result_ok:
            # JSONデコードエラー
            return NPRequestResult(result=False, status_code=response.status_code, NG_reason=NPServerRequestNGReason.JSON_DECODE_ERROR)
        
        if not(CONNECTION_RESULT_KEY_SERVER_VERSION in response_data):
            # サーバーバージョン情報無し
            return NPRequestResult(result=False, status_code=response.status_code, NG_reason=NPServerRequestNGReason.SERVER_VERSION_NOT_FOUND)

        if response_data[CONNECTION_RESULT_KEY_SERVER_VERSION][0]!=NP_SERVER_VERSION_NAME:
            # サーバーバージョンが不正
            print(response_data[CONNECTION_RESULT_KEY_SERVER_VERSION][0])
            return NPRequestResult(result=False, status_code=response.status_code, NG_reason=NPServerRequestNGReason.SERVER_VERSION_INVALID)

        return NPRequestResult(result=True, data=response_data)
    
    def open_with_browser(self, cgi:str)->str:
        """
        WEBブラウザでGETメソッド実行
        """
        url = self.__get_url(cgi)
        webbrowser.open(url,new=0)

    def __get_url(self, cgi:str)->str:
        return self.__get_address(self.variant) + '/' + cgi

    def __get_address(self,valiant:Variant):
        if valiant==Variant.Dev:
            return NP_SERVER_ADDRESS_DEV
        elif valiant==Variant.Pr:
            return NP_SERVER_ADDRESS_PR

    def __json_decode(self, response:requests.Response)->Tuple[bool,dict]:
        try:
            data = response.json()
        except requests.exceptions.JSONDecodeError as e:
            print("JSONパースエラー",e)
            print(response.text)
            return False, None
        
        return True, data
    
    def __add_server_version_key(self, data:dict):
        data[CONNECTION_RESULT_KEY_SERVER_VERSION] = NP_SERVER_VERSION_NAME

