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

using namespace std;

int main() {

  //tuple<int,char> tic = { 3, 'c'}; Doesn't work
  tuple<int, string> tis2 = tuple<int, string>(7, "abc");
  tuple<int, string> tis3 = make_tuple(10, "efg");

  // cout << tis << endl;
  cout << get<0>(tis2) << " and " << get<1>(tis2) << endl;
  cout << get<0>(tis3) << " and " << get<1>(tis3) << endl;

  return 0;
}