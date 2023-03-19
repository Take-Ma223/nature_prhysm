#pragma once
#include<string>
#include <DxLib.h>
using namespace std;

/// <summary>
/// フォントタイプ
/// </summary>
enum class FontType
{
	NORMAL = DX_FONTTYPE_NORMAL,
	EDGE = DX_FONTTYPE_EDGE,
	ANTIALIASING = DX_FONTTYPE_ANTIALIASING,
	ANTIALIASING_4X4 = DX_FONTTYPE_ANTIALIASING_4X4,
	ANTIALIASING_8X8 = DX_FONTTYPE_ANTIALIASING_8X8,
	ANTIALIASING_16X16 = DX_FONTTYPE_ANTIALIASING_16X16,
	ANTIALIASING_EDGE = DX_FONTTYPE_ANTIALIASING_EDGE,
	ANTIALIASING_EDGE_4X4 = DX_FONTTYPE_ANTIALIASING_EDGE_4X4,
	ANTIALIASING_EDGE_8X8 = DX_FONTTYPE_ANTIALIASING_EDGE_8X8,
	ANTIALIASING_EDGE_16X16 = DX_FONTTYPE_ANTIALIASING_EDGE_16X16,
};

/// <summary>
/// フォント情報クラス
/// </summary>
class FontInfo
{
public:
	wstring fontName;
	int size;
	int thick;
	FontType fontType;

	/// <summary>
	/// フォント情報クラスコンストラクタ
	/// </summary>
	/// <param name="fontName">フォント名</param>
	/// <param name="size">サイズ</param>
	/// <param name="thick">厚さ</param>
	/// <param name="fontType">フォントタイプ</param>
	FontInfo(wstring fontName = wstring(L"メイリオ"), int size = 16, int thick = 2, FontType fontType = FontType::NORMAL);
};

/// <summary>
/// フォント情報の同値判定
/// </summary>
/// <param name="a">フォント情報</param>
/// <param name="b">フォント情報</param>
/// <returns>同値かどうか</returns>
inline bool operator==(const FontInfo& a, const FontInfo& b)
{
	return a.fontName == b.fontName
		&& a.size == b.size
		&& a.thick == b.thick
		&& a.fontType == b.fontType;
}

/// <summary>
/// フォント情報クラスのハッシュ算出構造体
/// </summary>
struct FontInfoHash {
	inline std::size_t operator()(FontInfo fontInfo) const {
		std::size_t h1 = hash<wstring>()(fontInfo.fontName);
		std::size_t h2 = hash<int>()(fontInfo.size);
		std::size_t h3 = hash<int>()(fontInfo.thick);
		std::size_t h4 = hash<FontType>()(fontInfo.fontType);
		return h1 ^ h2 ^ h3 ^ h4;
	}
};