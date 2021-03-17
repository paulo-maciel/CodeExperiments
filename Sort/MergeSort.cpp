#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

void printVec(const vector<int>& vec) {
  for(auto v : vec) {
    cout << v << " ";
  }
  cout << endl;
}

vector<int> mergeSorted(const vector<int>& a, const vector<int>& b, long& inv) {
  cout << "merge sorted: " << endl;
  printVec(a);
  printVec(b);

  vector<int> sorted;
  ulong i = 0, j = 0;
  if (a.size() == 0ul && b.size() == 0ul) return sorted;
  if (a.size() == 0ul) return b;
  if (b.size() == 0ul) return a;

  while(true) {
    if (i >= a.size() || j >= b.size()) {
      break;
    }

    if (a[i] == b[j]) {
      ++i;
      ++j;
      continue;
    }
    while (a[i] < b[j] && i < a.size()) {
      sorted.push_back(a[i]);
      ++i;
    }

    while (b[j] < a[i] && j < b.size()) {
      sorted.push_back(b[j]);
      ++j;
    }
  }

  if (j < b.size()) {
    // append b to sorted
    for(; j < b.size(); ++j) sorted.push_back(b[j]);
  } else if (i < a.size()) {
    // append a to sorted.
    for(;i < a.size();++i) sorted.push_back(a[i]);
  }

  return sorted;
}

vector<int> myMerge(vector<int> v, long low, long high, long& inv) {
  vector<int> vec;

  // only one element.
  if (low == high) {
    vec.emplace_back(v[low]);
    return vec;
  }
  // only  2 elements.
  if (high == low + 1) {
    vec.emplace_back(v[low]);
    vec.emplace_back(v[high]);
    if (v[low] > v[high]) {
      swap(vec[0], vec[1]);
      ++inv;
    }
    return vec;
  }
  // find median
  int median = ceil(0.5f*(low + high));
  cout << "Median: " << median << endl;
  return mergeSorted( myMerge(v, 0, median - 1, inv),
                      myMerge(v, median, v.size() - 1, inv), inv );
}


int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};//, 8, 3, 1, 6};
  vector<int> v1 = { 1, 3, 4, 7, 8};
  vector<int> v2 = { 2, 5, 6, 8, 10, 16};
  printVec(v);
  //printVec(v1);

  long inv = 0;
  // myMerge(v, 0, v.size() - 1, inv);
  // cout << inv << endl;

  //auto vec = mergeSorted(v, v1, inv);
  //printVec(vec);

  auto vec = myMerge(v, 0, v.size() - 1, inv);
  printVec(vec);

  return 1;
}

