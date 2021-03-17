#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;


int main()
{
  glm::quat q{1,0,0,0};
  cout << "Quat: " << q.x << " " << q.y << " " << q.z << " " << q.w << endl;

  set<int> testSet = {1, 2, 2, 3, 3, 3, 4, 5, 5, 2, 3, 1, 6};

  cout << "Size of set: " << testSet.size() << endl;

  unordered_map<int, int> mymap = {};

  cout << "Size of map: " << mymap.size() << " is empty: " << mymap.empty() << endl;

  return 1;
}