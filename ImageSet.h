#pragma once

#include <unordered_map>
#include <string>
#include "ImageHandle.h"

/// <summary>
/// 画像アセット管理クラス
/// このクラスで読み込んだ画像は画面終了時に破棄されます
/// </summary>
class ImageSet{
public:
	~ImageSet();
	
	/// <summary>
	/// 補助記憶装置に存在する画像ハンドルの取得
	/// </summary>
	/// <param name="path">画像パス</param>
	/// <returns>画像ハンドル</returns>
	ImageHandle getImageHandle(std::wstring path);

	/// <summary>
	/// 補助記憶装置に存在する分割画像の画像ハンドル配列を得る
	/// </summary>
	/// <param name="path">画像パス</param>
	/// <param name="allNum">分割画像の個数</param>
	/// <param name="XNum">横の個数</param>
	/// <param name="YNum">縦の個数</param>
	/// <param name="XSize">Xサイズ</param>
	/// <param name="YSize">Yサイズ</param>
	/// <returns>画像ハンドル配列</returns>
	std::vector<ImageHandle> getImageHandles(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);
	
	/// <summary>
	/// メモリに存在する画像ハンドルを登録する
	/// このメソッドで登録することで自動で解放されます
	/// </summary>
	/// <param name="tag">メモリに存在する画像ハンドルのタグ</param>
	/// <param name="imageHandle">メモリに存在する画像ハンドル</param>
	void registImageHandleFromMem(std::wstring tag, ImageHandle imageHandle);

	/// <summary>
	/// メモリに存在する画像ハンドルの取得
	/// </summary>
	/// <param name="tag">取得する画像ハンドルのタグ</param>
	/// <returns>画像ハンドル</returns>
	ImageHandle getImageHandleFromMem(std::wstring tag);

	/// <summary>
	/// メモリに存在する画像ハンドルの削除
	/// </summary>
	/// <param name="tag">削除する画像ハンドルのタグ</param>
	void deleteImageHandleFromMem(std::wstring tag);

	/// <summary>
	/// 読み込んだ全ての画像ハンドルの削除
	/// </summary>
	void deleteImage();
private:

	std::unordered_map<std::wstring, ImageHandle> img;//(画像パス,ハンドル情報)　の辞書
	std::unordered_map<std::wstring, std::vector<ImageHandle>> imgs;//(画像パス,ハンドル情報配列)　の辞書
	std::unordered_map<std::wstring, ImageHandle> imgFromMem;//(画像タグ,ハンドル情報)の辞書 メモリから読み込む画像用

	void registImage(std::wstring);
	void registImages(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);


};

