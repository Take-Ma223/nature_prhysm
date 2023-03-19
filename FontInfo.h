#pragma once
#include<string>
#include <DxLib.h>
using namespace std;

/// <summary>
/// �t�H���g�^�C�v
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
/// �t�H���g���N���X
/// </summary>
class FontInfo
{
public:
	wstring fontName;
	int size;
	int thick;
	FontType fontType;

	/// <summary>
	/// �t�H���g���N���X�R���X�g���N�^
	/// </summary>
	/// <param name="fontName">�t�H���g��</param>
	/// <param name="size">�T�C�Y</param>
	/// <param name="thick">����</param>
	/// <param name="fontType">�t�H���g�^�C�v</param>
	FontInfo(wstring fontName = wstring(L"���C���I"), int size = 16, int thick = 2, FontType fontType = FontType::NORMAL);
};

/// <summary>
/// �t�H���g���̓��l����
/// </summary>
/// <param name="a">�t�H���g���</param>
/// <param name="b">�t�H���g���</param>
/// <returns>���l���ǂ���</returns>
inline bool operator==(const FontInfo& a, const FontInfo& b)
{
	return a.fontName == b.fontName
		&& a.size == b.size
		&& a.thick == b.thick
		&& a.fontType == b.fontType;
}

/// <summary>
/// �t�H���g���N���X�̃n�b�V���Z�o�\����
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