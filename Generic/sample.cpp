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

// Complete the freqQuery function below.
vector<int> freqQuery(vector<vector<int>> queries) {
  list<int> numList;
  unordered_map<int, int> freqMap;
  vector<int> vec;
  for(const auto& query : queries) {
    const auto& cmd = query[0];
    auto num = query[1];
    switch(cmd) {
      case 1: {
        auto it = numList.begin();
        for(; it != numList.end(); ++it) {
          if (*it < num) {
            continue;
          }
          else { // >= num
            numList.insert(it, num);
            freqMap[num]++;
            break;
          }
        }

        if (it == numList.end()) {
          freqMap[num]++;
          numList.insert(it, num);
        }

        break;
      }
      case 2: {
        auto it = numList.begin();
        for(; it != numList.end(); ++it) {
          if (*it == num) {
            numList.erase(it);
            freqMap[num]--;
            break;
          }
        }
        break;
      }
      case 3: {
        // auto it = numList.begin();
        // int count = 0;
        // for(; it != numList.end(); ) {
        //   int first = *it;
        //   count = 0;
        //   do {
        //     ++count;
        //     ++it;
        //   } while(*it == first);
        //   if (count == num) {
        //     vec.push_back(1);
        //     break;
        //   }
        // }
        // if (count != num) {
        //   vec.push_back(0);
        // }
        bool found = false;
        for(auto& kvp : freqMap) {
          if (kvp.second == num) {
            found = true;
            vec.push_back(1);
            break;
          }
        }
        if (!found) {
          vec.push_back(0);
        }
        break;
      }
      default:
        break;
    }
  }

  return vec;
}



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

#include <iostream>
using namespace std;

#include <set>
#include <map>
#include <string>

// in = ”racecart”
// out = “####car#”
// void copyStringWithHash(const char *in, char *out) {
//   set<char> s;

//   int j = 0;
//   for(int i = 0; in[i] != '\0'; ++ i) {
//     if (s.count(in[i])) {
//       out[j] = in[i];
//     } else {
//       // Not in set
//       out[j] = '#';
//       s.insert(in[i]);
//     }
//     ++j;
//   }
//   out[j] = '\0';
// }


// Consider the equation WIRES * AR = AWESOME
//
static unordered_map<char, int> m;


bool isEquationTrue() {
  int wires = m['w'] * 10000 + sm['i'] * 1000 + sm['r'] *100 + sm['e'] * 10 + sm['s'];
  int ar = sm['a']*10   + sm['r'];
  int awesome = m['a'] * 1000000 + m['w'] * 100000 + m['e'] * 10000 + sm['s'] * 1000 + sm['o'] *100 + sm['m'] * 10 + sm['e'];

  return (wires * ar == awesome) ? true : false;
}

void getEntry(int& i) {
  for(auto entry : m) {
    if (m[entry] == i) {
      ++i;
    }
    else {
      break;
    }
  }
}

bool generateMapping() {
  vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};


  // select value from number.
  // Associate to w, i, r, e, s, a, o, m
  int i = 0;
  getEntry(i)
  m['w'] = i;



}


}

bool isSolvable()
{
    /// While there are mapping combinations.
  while(generateMapping()) {
    if (isEquationTrue()) {
      return true;
    }
  }

  return false;
}

int main() {
  char *in = "racecart";
  char out[10];

  copyStringWithHash(in, out);

  cout << out << endl;
  return 0;
}
