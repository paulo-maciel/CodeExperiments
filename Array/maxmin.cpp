#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>

using namespace std;

void printVec(const vector<int>& vec) {
  for(auto v : vec) {
    cout << v << " ";
  }
  cout << endl;
}

#include <string>
#include <sstream>
#include <map>

using namespace std;

// Complete the maxMin function below. Passed 16/17 1 timeout when using simple 'std::sort'
template <class ForwardIt>
 void myQuicksort(ForwardIt first, ForwardIt last)
 {
    if(first == last) return;
    int pivot = *std::next(first, std::distance(first,last)/2);
    ForwardIt middle1 = std::partition(first, last,
                         [pivot](const int& em){ return em < pivot; });
    ForwardIt middle2 = std::partition(middle1, last,
                         [pivot](const int& em){ return !(pivot < em); });
    myQuicksort(first, middle1);
    myQuicksort(middle2, last);
 }

// Complete the maxMin function below.
int maxMin(int k, vector<int> arr) {
  auto arrSize = arr.size();
  myQuicksort(arr.begin(), arr.end());

  int minMax = INT_MAX;
  for(auto i = 0ul; i < arrSize - k + 1; ++i) {
    // Select k from i and compute min max.
    int kMin = INT_MAX, kMax = -INT_MAX;
    for(auto j = i; j < i + k; ++j) {
      kMin = min(arr[j], kMin);
      kMax = max(arr[j], kMax);
    }
    minMax = min(minMax, kMax - kMin);
  }

  return minMax;
}



int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};
  (void)v;



  return 1;
}

