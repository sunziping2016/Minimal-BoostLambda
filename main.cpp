#include <iostream>
#include <vector>
#include <algorithm>
#include "lambda.h"

using namespace std;

int main()
{
    vector<int> array(10);

    cout << "Initialize:" << endl;
    int index = 0;
    for_each(array.begin(), array.end(), _1 = ++ref(index));
    for_each(array.begin(), array.end(), cout << _1 << " ");
    cout << endl;

    cout << "Sum:" << endl;
    int sum = 0;
    for_each(array.begin(), array.end(), sum += _1);
    cout << sum << endl;

    cout << "Sort:" << endl;
    sort(array.begin(), array.end(), _2 < _1);
    for_each(array.begin(), array.end(), cout << _1 << " ");
    cout << endl;

    return 0;
}
