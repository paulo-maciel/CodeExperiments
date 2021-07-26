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

using namespace std;

// divide and conquer
vector<vector<int>> twoSum(vector<int>& nums, int target, int exclude) {
    vector<vector<int>> output;
    vector<int> candidate;
    
    int i = 0, j = 0;
    while(i < nums.size()) {
        if (i == exclude) {
            ++i;
            continue;
        }
        candidate.push_back(nums[i]);
        j = 0;
        while (j < nums.size()) {
            if (i == j) {
                ++j;
                continue;
            }
            if (nums[i] + nums[j] == target) {
              candidate.push_back(nums[j]);
              output.push_back(candidate);
            }
            ++j;
        }
        
        ++i; 
    }
    return output;
}
                    
vector<vector<int>> threeSum(vector<int>& nums, int target, int exclude) {
    vector<vector<int>> output;
    vector<int> candidate;
    
    int i = 0;
    while(i < nums.size()) {
        if (i == exclude) {
            ++i;
            continue;
        }

        int newTarget = target - nums[i];
        auto twoAdd = twoSum(nums, newTarget, exclude);
        for(auto vec2 : twoAdd) {
            candidate.push_back(nums[i]);
            candidate.push_back(vec2[0]);
            candidate.push_back(vec2[1]);
            output.push_back(candidate);
            candidate.clear();
        }
        ++i;
    }
    
    
    return output;
}

vector<vector<int>> fourSum(vector<int>& nums, int target) {
    vector<vector<int>> output;
    vector<int> candidate;
    
    int i = 0;
    while(i < nums.size()) {

        int newTarget = target - nums[i];
        int exclude = i;
        auto threeAdd = threeSum(nums, newTarget, exclude);
        for(auto vec3 : threeAdd) {
            candidate.push_back(nums[i]);
            candidate.push_back(vec3[0]);
            candidate.push_back(vec3[1]);
            candidate.push_back(vec3[2]);
            output.push_back(candidate);
            candidate.clear();
        }
        i++;
    }
    
    return output;
}


int main() {
  // nums = [1,0,-1,0,-2,2], target = 0
  vector<int> nums = {1,0,-1,0,-2,2};
  int target = 0;
  vector<vector<int>> out = fourSum(nums, target);

  cout << "[" ;
  for(auto vec : out) {
    cout << "[";
    for(auto elem : vec) {
      cout << elem << ",";
    }
    cout << "],";
  }
    
  cout << "]" << endl;
  return 0;
}