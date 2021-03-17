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

int SuperDigit(string n);
map<int, int> sdMap;

// Complete the superDigit function below.
int superDigit(string n, int k) {
  string origN = n;
  for(auto i = 0; i < k - 1; ++i) {
    n += origN;
  }

  sdMap.clear();
  for(int i = 0; i < 10; ++i) {
    sdMap[i] = i;
  }

  return SuperDigit(n);
}

int SuperDigit(string n) {
  if (n.length() == 1) {
    return sdMap[stoi(n)];
  }

  // Compute the sum.
  int sum = 0;
  for(char dig : n) {
    sum += stoi(&dig);
  }

  if (sdMap.count(sum)) {
    return sdMap[sum];
  }

  // Convert sum to string and compute the super digit
  return sdMap[sum] = SuperDigit(to_string(sum));
}


int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};
  (void)v;


  return 1;
}

