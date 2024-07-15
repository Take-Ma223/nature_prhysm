#include"OptionStateSaveLoad.h"
#include"DxLib.h"
#include<sys/stat.h>
#include"STRUCT_OP.h"
#include<nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <codecvt> // codecvt_utf8
#include<algorithm>

using json = nlohmann::json;

// encoding function
std::string to_utf8(std::wstring& wide_string)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	return utf8_conv.to_bytes(wide_string);
}

// decoding function
std::wstring from_utf8(const std::string& utf8_string)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	return utf8_conv.from_bytes(utf8_string);
}


void SaveOptionState(OP option) {
	json option_json;

	for (const auto& i : option.list) {
		option_json[to_utf8(i->getTitle())] = to_utf8(i->toString());
	}

	std::ofstream file(L"save_data/OptionState.dat");
	file << option_json.dump(4);

	return;
}


void LoadOptionState(Option *option) {
	std::ifstream file(L"save_data/OptionState.dat");
	std::string jsonstr;

	if (file.fail()) {//ファイル見つからなかったら
		//何もしない
		return;
	}

	try
	{
		auto data = json::parse(file);

		for (const auto& i : data.items()) {
			//keyがオプションリストに存在するか確認
			auto result = std::find_if(
				option->op.list.begin(), option->op.list.end(),
				[&](OptionItemBase *op) {
					auto title = op->getTitle();
					auto key = from_utf8(i.key());
					return title == key;
				}
			);

			if (result == option->op.list.end()) {

			}
			else {
				(*result)->setIndexFromString(from_utf8(i.value()));
			}

			
		}



	}
	catch (const std::exception&)
	{
		//json読み込みに失敗した場合何もしない
		return;
	}




	return;
}