import requests
import json
from enum import Enum
from typing import Tuple

TIMEOUT = 6.0

class RequestMethod(Enum):
    GET=0,
    POST=1,

class RequestResult(Enum):
    FAILED=0,
    HTTP_STATUS_NOT_200=1,
    HTTP_STATUS_200=2,

class RequestOperator():
    def __init__(self):
        self.Timeout=TIMEOUT

    def request(self, url:str, method:RequestMethod, data:dict=None)->Tuple[RequestResult, requests.Response]:
        result_ok, response = self.__request(url, method=method, data=data)
        if not result_ok:
            # 接続失敗(タイムアウト含む)
            return RequestResult.FAILED, None

        result_ok = self.__is_response_200(response)
        if not result_ok:
            # HTTPレスポンスエラー
            return RequestResult.HTTP_STATUS_NOT_200, response
        
        return RequestResult.HTTP_STATUS_200, response

    def __request(self, url:str, method:RequestMethod, data:dict=None)->Tuple[bool, requests.Response]:
        # true:OK false:Timeout
        # a={"a":2}
        json_data = json.dumps(data)
        header= {"content-type": "application/json"}
        try:
            if method==RequestMethod.GET:
                response = requests.get(url, data=json_data, headers=header, timeout=(self.Timeout, self.Timeout))
            elif method==RequestMethod.POST:
                response = requests.post(url, data=json_data, headers=header, timeout=(self.Timeout, self.Timeout))
        except requests.Timeout as e:
            return False, None
        except requests.ConnectionError as e:
            return False, None
        except:
            return False, None
        
        return True, response


    def __is_response_200(self, res)->bool:
        status_code = res.status_code
        print("status code:", status_code) # HTTPのステータスコード取得
        if status_code == 200: 
            return True
        else:
            return False
        


