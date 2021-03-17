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

include <string>
#include <sstream>
#include <map>

using namespace std;

vector<string> split_string(string);

int SuperDigit(string n);

// map of sum (as a string) to super digit.
map<string, int> stringSdMap;

// Complete the superDigit function below.
int superDigit(string n, int k) {
  string origN = n;
  for(auto i = 0; i < k - 1; ++i) {
    n += origN;
  }

  stringSdMap.clear();
  for(int i = 0; i < 10; ++i) {
    stringstream ss;
    ss << i;
    stringSdMap[ss.str()] = i;
  }

  return SuperDigit(n);
}

int SuperDigit(string n) {
  if (n.empty()) return 0;
  if (n.length() == 1) {
    return stringSdMap[n];
  }

  // Try to avoid computing the sum.
  if (stringSdMap.count(n)) {
    return stringSdMap[n];
  }

  // Compute the sum.
  int sum = 0;
  for(char dig : n) {
    sum += stoi(&dig);
  }

  // Convert sum to string and compute the super digit
  auto newStr = to_string(sum);
  int val = SuperDigit(newStr);
  stringSdMap[newStr] = val;
  return val;
}


int main()
{
  vector<int> v = { 9 , 4, 7, 2, 1};
  (void)v;


  return 1;
}

