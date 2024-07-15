#pragma once

#include<algorithm>
#include<string>
#include<vector>
#include <cassert>

class OptionItemBase
{
public:

	std::wstring toString() {
		return nameList[selectedIndex];
	};
	
	void setIndex(int index) {
		selectedIndex = index;

		assert(selectedIndex < nameList.size());

	};

	void setIndexFromString(std::wstring str) {
		auto result = std::find(nameList.begin(), nameList.end(), str);

		if (result == nameList.end()) {
			selectedIndex = 0;
		}
		else {
			selectedIndex = std::distance(nameList.begin(), result);
		}
	};

	OptionItemBase() {};

	virtual ~OptionItemBase() {};

	std::wstring getTitle() { return title;};
	std::wstring getItemName(int index) { return nameList[index]; };
	std::wstring getItemDescription(int index) { return descriptionList[index]; };
	std::wstring getDescription() { return getItemDescription(getIndex()); };

	virtual int itemCount(){ return nameList.size(); };

	void incrementIndex() {
		selectedIndex++;
		if (selectedIndex >= itemCount()) {
			selectedIndex = 0;
		}
	}

	void decrementIndex() {
		selectedIndex--;
		if (selectedIndex < 0) {
			selectedIndex = itemCount() - 1;
		}
	}

	int getIndex() {
		return selectedIndex;
	}

	/// <summary>
	/// 初期化処理
	/// インデックスは各OptionItem派生クラスのEnumを元にする
	/// </summary>
	virtual void initData() = 0;

protected:
	int selectedIndex = 0; //UI State Holder
	
	std::wstring title;
	std::vector<std::wstring> nameList;
	std::vector<std::wstring> descriptionList;

};