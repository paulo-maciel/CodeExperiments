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

#include <set>

using namespace std;

vector<string> split_string(string);

// Complete the whatFlavors function below.
void whatFlavors(vector<int> cost, int money) {

  multimap<int, int> m;
  for(auto i = 0ul; i < cost.size(); ++i) {
    m.insert({cost[i], i});
  }

  pair<int, int> sol;
  for(auto i = 0ul; i < cost.size(); ++i) {
    if (cost[i] >= money) continue;

    for(auto entry : m) {
      if (entry.second == i) continue;
      if (entry.first + cost[i] == money) {
        // Found
        if (entry.second < cost[i]) {
          sol.first = entry.second + 1;
          sol.second = i + 1;
        } else {
          sol.first = i + 1;
          sol.second = entry.second  + 1;
        }

        break;
      }
    }
  }

   cout << sol.first << " " << sol.second << endl;
}

void whatFlavors(vector<int> cost, int money) {
  pair<int,int> sol;

  for(auto i = 0ul; i < cost.size(); ++i) {
    if (cost[i] >= money) continue;
    for(auto j = i + 1; j < cost.size(); ++j) {
      if (cost[i] + cost[j] == money) {
        sol.first = min(i,j) + 1;
        sol.second = max(i,j) + 1;
        break;
      }
    }
  }

   cout << sol.first << " " << sol.second << endl;
}

// Complete the whatFlavors function below.  WORKING SOLUTION.
void whatFlavors(vector<int> cost, int money) {
  pair<int,int> sol;
  // Map containing diff cost and index.
  ulong csize = cost.size();
  unordered_map<ulong,ulong> m;
  for(auto i = 0ul; i < csize; ++i) {
    if (cost[i] >= money) continue;
    m.insert({cost[i], i});
  }

  for(auto i = 0ul; i < csize; ++i) {
    if (cost[i] >= money) continue;
    ulong diff = money - cost[i];
    if (m.count(diff) && m[diff] != i) {
      sol.first = min(i, m[diff]) + 1;
      sol.second = max(i, m[diff]) + 1;
      break;
    }
  }

  cout << sol.first << " " << sol.second << endl;
}

// Complete the poisonousPlants function below.
int poisonousPlants(vector<int> p) {

  int nDays = 0;
  while(p.size() > 1ul) {
    set<int> toErase;
    for(auto i = 1ul; i < p.size(); ++i) {
      if (p[i] > p[i - 1]) {
        toErase.insert(i);
      }
    }

    if (toErase.empty()) break;

    vector<int> newP;
    newP.push_back(p[0]);
    for(auto i = 1ul; i < p.size(); ++i) {
      if (!toErase.count(i)) {
        newP.push_back(p[i]);
      }
    }

    if (newP.size() > 1) {
      p.swap(newP);
    }

    ++nDays;
  }

  return nDays;
}
int main()
{


  return 1;
}

