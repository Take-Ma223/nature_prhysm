#pragma once
#include<vector>
#include "ImageHandle.h"
using namespace std;

/// <summary>
/// ���l�摜�̂��߂̉摜�n���h��
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
