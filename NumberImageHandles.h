#pragma once
#include<vector>
#include "ImageHandle.h"
using namespace std;

/// <summary>
/// 数値画像のための画像ハンドル
/// </summary>
class NumberImageHandles {
    vector<ImageHandle> numbers;
    ImageHandle point;
public:
    NumberImageHandles(vector<ImageHandle> numbers = vector<ImageHandle>(), ImageHandle point = ImageHandle()) {
        NumberImageHandles::numbers = numbers;
        NumberImageHandles::point = point;
    }

    vector<ImageHandle> getNumbers() { return numbers; }
    ImageHandle getPoint() { return point; }
};
