#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>

using namespace std;

std::vector<int> cache;
int climbStairs(int n) {
    if (n == 1) return 1;
    if (n == 2) return 2;

    if (cache.size() == 0) {
        cache.resize(n+1);
        std::fill(cache.begin(), cache.end(), -1);
    }

    if (cache[n] == -1) {
        // Number of ways to get to n is equal to the
        // number of ways to climb in which only one step is left
        // and the number of ways to climb in which two steps are left.
        cache[n] = climbStairs(n-1) + climbStairs(n-2);
    }

    return cache[n];
}

int main() {

    for (auto i = 1; i < 10; ++i) {
        cache.clear();
        cout << "climbStairs(" << i << "): " <<  climbStairs(i) << endl;
    }

  return 1;
}