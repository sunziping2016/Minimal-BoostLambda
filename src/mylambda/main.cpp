#include <iostream>
#include <vector>
#include <algorithm>
#include "lambda.h"

int main()
{
    std::vector<int> array(10);

    std::cout << "Initialize:" << std::endl;
    int index = 0;
    std::for_each(array.begin(), array.end(), _1 = ++ref(index));
    std::for_each(array.begin(), array.end(), std::cout << _1 << " ");
    std::cout << std::endl;

    std::cout << "Sum:" << std::endl;
    int sum = 0;
    std::for_each(array.begin(), array.end(), sum += _1);
    std::cout << sum << std::endl;

    std::cout << "Sort:" << std::endl;
    sort(array.begin(), array.end(), _2 < _1);
    std::for_each(array.begin(), array.end(), std::cout << _1 << " ");
    std::cout << std::endl;

    return 0;
}
