#pragma once
#include "ImageSet.h"
#include "SoundSet.h"
#include "FontSet.h"

/**
* アセット管理クラス
* 画像、サウンドを登録し、辞書で管理します
* 文字画像など使いまわさずにその場限りで作成する画像はハンドル登録のみ行います(registImageHandler)
* Assetのデストラクタ実行時に画像、サウンドは解放されます
*/
class Asset
{
	ImageSet imageSet;
	SoundSet soundSet;
	FontSet fontSet;
	void deleteAsset();

public:
	~Asset();

	/// <summary>
	/// 補助記憶装置に存在する画像ハンドルの取得
	/// </summary>
	/// <param name="path">画像パス</param>
	/// <returns>画像ハンドル</returns>
	ImageHandle img(std::wstring path);


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
	std::vector<ImageHandle> imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	/// <summary>
	/// メモリに存在する画像ハンドルの取得
	/// 最初の取得は必ずこのメソッドを使って画像ハンドルを登録してください
	/// </summary>
	/// <param name="tag">メモリに存在する画像ハンドルのタグ</param>
	/// <param name="imageHandle">メモリに存在する画像ハンドル</param>
	/// <returns><画像ハンドル/returns>
	ImageHandle imgFromMem(std::wstring tag, ImageHandle imageHandle);
	
	/// <summary>
	/// メモリに存在する画像ハンドルの取得
	/// </summary>
	/// <param name="tag">取得する画像ハンドルのタグ</param>
	/// <returns>画像ハンドル</returns>
	ImageHandle imgFromMem(std::wstring tag);

	/// <summary>
	/// メモリに存在する画像ハンドルの削除
	/// </summary>
	/// <param name="tag">削除する画像ハンドルのタグ</param>
	void deleteImgFromMem(std::wstring tag);

	int snd(std::wstring path);

	/// <summary>
	/// フォントハンドラを得る
	/// </summary>
	/// <param name="fontInfo">フォント情報</param>
	/// <returns>フォントハンドラ</returns>
	FontHandle font(FontInfo fontInfo);
};