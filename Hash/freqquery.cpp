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

// Complete the freqQuery function below.
vector<int> freqQuery(vector<vector<int>> queries) {
  unordered_map<int, long> mMap;
  vector<int> outVec;
  multiset<long> multip;

  for(size_t i = 0; i < queries.size(); ++i) {
    int cmd = queries[i][0];
    int val = queries[i][1];
    switch(cmd) {
      case 1:
        if (mMap.count(val)) {
          mMap[val] += 1;
        } else {
          mMap[val] = 1;
        }
        multip.insert(mMap[val]);
        break;
      case 2:
        if (mMap.count(val)) {
          mMap[val] -= 1;
          multip.insert(mMap[val]);
        }
        break;
      case 3:
        auto it = multip.find(val);
        if (it == multip.end()) {
          outVec.push_back(0);
        } else {
          outVec.push_back(1);
        }
        break;
    }
  }

  return outVec;
}

int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};
  (void)v;



  return 1;
}

