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

int rob(vector<int>& houses) {
    // Select the best houses to rob in a range taking into account the adjacency restriction.
    
    int numHouses = houses.size();
    if (numHouses == 1) {
        return houses[0];
    }
    
    if (numHouses == 2) {
        return max(houses[0], houses[1]);
    }
    
    houses[1] = max(houses[0], houses[1]);
    for(int i = 2; i < numHouses; ++i) {
      houses[i] = max(houses[i-1], houses[i] + houses[i - 2]);    
    }
    
    return houses[numHouses - 1];
}

int main() {
   // Note: passing 41/68 cases.

  return 0;
}
