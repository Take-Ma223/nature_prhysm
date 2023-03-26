#pragma once
#include<vector>
#include "ImageHandle.h"
using namespace std;

/// <summary>
/// ”’l‰æ‘œ‚Ì‚½‚ß‚Ì‰æ‘œƒnƒ“ƒhƒ‹
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
