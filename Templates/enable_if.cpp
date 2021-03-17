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

namespace {
  template <typename T>
  typename std::enable_if<std::is_floating_point<T>::value, T>::type
  normalize(T x) { return (x < T(0)) ?  T(-1): T(1); }
}



int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};
  (void)v;

  float x = -0.5f;
  x = normalize(x);
  cout << x << endl;

  // int y = -1;
  // y = normalize(y);
  // cout << y << endl;

  return 1;
}

