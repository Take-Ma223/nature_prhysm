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
	/// 画像1つのハンドラを得る
	/// </summary>
	/// <param name="path">画像パス</param>
	/// <returns>画像ハンドラ</returns>
	ImageHandle img(std::wstring path);


	/// <summary>
	/// 分割画像のハンドラを得る
	/// </summary>
	/// <param name="path">画像パス</param>
	/// <param name="allNum">分割画像の個数</param>
	/// <param name="XNum">横の個数</param>
	/// <param name="YNum">縦の個数</param>
	/// <param name="XSize">Xサイズ</param>
	/// <param name="YSize">Yサイズ</param>
	/// <returns>画像ハンドラ配列</returns>
	std::vector<ImageHandle> imgs(std::wstring path, int allNum, int XNum, int YNum, int XSize, int YSize);

	/// <summary>
	/// 画像ハンドラを登録する
	/// </summary>
	/// <param name="imageHandle">画像ハンドラ</param>
	void registImageHandler(ImageHandle imageHandle);


	int snd(std::wstring path);

	/// <summary>
	/// フォントハンドラを得る
	/// </summary>
	/// <param name="fontInfo">フォント情報</param>
	/// <returns>フォントハンドラ</returns>
	FontHandle font(FontInfo fontInfo);
};