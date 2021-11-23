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

void parse(const string &str, std::vector<int> &pVec)
{
  stringstream ss(str);

  string tempString;
#if 0
  while(ss.good()) {
    getline(ss, tempString, ',');
#endif
  while (getline(ss, tempString, ','))
  {
    pVec.push_back(atoi(tempString.c_str()));
  }
}

int main()
{

  string str = "1, 2, 3, 4, 5";
  vector<int> pVec;
  parse(str, pVec);
  for (auto i : pVec)
  {
    cout << i << " ";
  }
  cout << endl;

  return 0;
}
