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

    int getBestToRob(vector<int>& money, int beg, int end) {

        // find the best to rob in range and add the best of the subranges taking into account the adj restriction.
        // find max
        if (beg >= end) {
            return 0;
        }
        int currMax = 0;
        int maxIdx = beg;
        for(int i = beg; i < end; ++i) {
            if (money[i] > currMax ) {
                maxIdx = i;
                currMax = max(currMax, money[i]);
            } 
        }
        
        return currMax + getBestToRob(money, beg, maxIdx - 1) + getBestToRob(money, maxIdx + 2, end);
    }
    
    // {2, 1, 1 2}
    // get 0 4
    //    maxIdx = 0 max = 2
    //    return 2 + get 0 -1 + get 1 4
    //                  0          maxIdx = 3 return 2 + get 1 2 + get 4 4
    //                                                       0         0
    int rob(vector<int>& money) {
        // Select the best houses to rob in a range taking into account the adjacency restriction.
        return getBestToRob(money, 0, money.size());
    }

int main() {
   // Note: passing 41/68 cases.

  return 0;
}
