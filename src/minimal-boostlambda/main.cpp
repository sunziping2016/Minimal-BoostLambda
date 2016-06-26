#include <iostream>
#include <vector>
#include <algorithm>
#include "lambda.h"

using namespace std;
using namespace boost::lambda;

int main()
{
    vector<int> array { 1, 2, 3, 4, 5 };
    transform(array.begin(), array.end(), array.begin(), _1 * 2);

    cout << "Transform array:" << endl;
    for (int i: array)
        cout << i << " ";
    cout << endl;

    cout << "Const test:" << endl;
    cout << (_1 + _2)(1, 2) << endl;

    return 0;
}
