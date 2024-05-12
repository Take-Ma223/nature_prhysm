import hashlib
import os

class NPHash():
    def __init__(self):
        pass

    def get_nps_hash(self, nps_path:str):
        """npsファイルのsha3_256ハッシュを取得する
        :param nps_path: npsファイルのパス
        :return: ハッシュ値
        :rtype: str
        :raises FileNotFoundError:
        """

        ext = os.path.basename(nps_path)  # "~.nps"
        with open(nps_path, 'rb') as file:
            # ファイルを読み取る
            fileData = file.read()

            # ファイル内容とファイル名のバイト結合から sha3_256 を返す
            hash_sha3_256 = hashlib.sha3_256(fileData + ext.encode()).hexdigest()
        return hash_sha3_256
    
    def get_nps_hash_old(self, nps_path:str):
        """npsファイルのsha3_256ハッシュを取得する(ファイル内容のみ)
        :param nps_path: npsファイルのパス
        :return: ハッシュ値
        :rtype: str
        :raises FileNotFoundError:
        """

        ext = os.path.basename(nps_path)  # "~.nps"
        with open(nps_path, 'rb') as file:
            # ファイルを読み取る
            fileData = file.read()

            # ファイル内容とファイル名のバイト結合から sha3_256 を返す
            hash_sha3_256 = hashlib.sha3_256(fileData).hexdigest()
        return hash_sha3_256