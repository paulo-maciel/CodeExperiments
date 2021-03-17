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

// Complete the countTriplets function below.

// NOT WORKING!!!
long countTriplets(vector<long> arr, long r) {
  long tcount = 0;
  int nelem = 0;
  int start = 0;
  if (arr.size() < 3) return tcount;

  for(auto i = 0ul; i < arr.size() - 2; ++i) {
    auto val = arr[i];
    nelem = 1;
    auto j = i;
    auto start = i + 1;
    while(start < arr.size() - 1) {
      cout << "starting j loop with j = " << j << " a[j]: " << arr[j] << endl;
      for(auto j = start; j < arr.size(); ++j) {
        if (arr[j] == r * val) {
          ++nelem;
          if (nelem == 3) {
            // See if there is another tripplet in the array starting with the
            // same value arr[i]
            ++tcount;
            val = arr[i];
            j = arr.size();
          } else {
            val = arr[j];
          }
        }
      }
      ++start;
      cout << " start: " << start << endl;
    }
  }

  return tcount;
}

// Complete the countTriplets function below.  Works but disagrees with hackerrank for 1,3,9,9,27,81 Don't know why?
long countTriplets(vector<long> arr, long r) {
  unordered_map<int, vector<pair<int,int>>> m;

  for(auto i = 0ul; i < arr.size(); ++i) {
    if (i < arr.size() - 2) {
      m[i] = vector<pair<int,int>>();
    }

    for(auto mentry = 0ul; mentry < m.size(); ++mentry) {
      if (arr[i] == arr[mentry]*r) {
        m[mentry].push_back(make_pair(arr[i], -1));
      }
      else if (arr[i] == arr[mentry]*r*r) {
        // found the end of the triplet if any.
        auto& vec = m[mentry];
        for (auto& p : vec) {
          if (p.second == -1) {
            p.second = arr[i];
          }
        }
      }
    }
  }

  // Now count all the valid entries in the map
  long tcount = 0;
  for(auto mentry = 0ul; mentry < m.size(); ++mentry) {
    auto vec = m[mentry];
    if (vec.empty()) continue;
    else {
      for (auto p : vec) {
        if (p.second != -1) {
          ++tcount;
        }
      }
    }
  }

  return tcount;
}

int main()
{


  return 1;
}

