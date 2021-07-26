#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <tuple>
#include <map>
#include <ostream>
#include <iterator>

// for pop_heap
#include <algorithm>

using namespace std;

int sumOfUnique(vector<int>& nums) {

    map<int,int> countMap;
    for(auto n : nums) {
        countMap[n]++;
    }
    int total = 0;
    for(auto kvp:countMap) {
        if (kvp.second == 1) {
            total += kvp.first;
        }
    }
    

    return total;
}

int maxSubArray(vector<int>& nums) {

  int localMax = nums[0];
  int globalMax = localMax;
  for(int i = 1; i < nums.size(); ++i) {
      localMax = max(localMax + nums[i], nums[i]);
      globalMax = max (globalMax, localMax);
  }
    
  return globalMax;
}

int main() {
   // Note: passing 41/68 cases.

  return 0;
}
