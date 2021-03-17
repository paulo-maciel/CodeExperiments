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

static vector<int> cache;

#if 1
int fact(int n) {
  cache[0] = 1;
  for(int i = 1; i <= n; ++i) {
    cache[i] = i * cache[i - 1];
  }

	return cache[n];
}
#else
int fact(int n) {
	if (n == 0) {
		return 1;
	}
	return n * fact(n - 1);;
}
#endif

int main()
{
  int num = 19;
  cache.reserve(num + 1);
  for(auto i = 0; i <= num; ++i)
    cache[i] = 0;
  //fill(cache.begin(), cache.end(), -1);

  for(int i = 1; i < 6; ++i)
  cout << fact(i) << endl;

  return 1;
}