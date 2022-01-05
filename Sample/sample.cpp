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
#include <utility>

// for pop_heap
#include <algorithm>

//import helloworld;

using namespace std;

void permuteHelper(vector<string>& stringSet, vector<string>& chosen) {
  if (stringSet.empty()) {
    auto pr = [](string str) { cout << str << " "; };
    cout << "Choice: "; 
    for(auto choice : chosen) {
      pr(choice);
    }
    cout << endl;
    //cout << chosen << endl;
    // base case
  } else {
    for (auto it = begin(stringSet); it != end(stringSet); ++it) {
      string s = *it;
      // Remove from the set of characters.
      it = stringSet.erase(it);

      // choose
      chosen.push_back(s);

      // explore
      permuteHelper(stringSet, chosen);
      
      // un-choose
      chosen.pop_back();

      it = stringSet.insert(it, s);
    }
  }
}

// Outputs all permutations of the given vector.
void permute(vector<string>& stringSet) {
  vector<string> chosen;
  permuteHelper(stringSet, chosen);
}

void listSubset(set<string>& masterSet, set<string>& used) {
  if (masterSet.empty()) {
    auto pr = [](string str) { cout << str << " "; };
    cout << "Used: "; 
    for(auto us : used) {
      pr(us);
    }
    cout << endl;
  } else {
    auto eIter = masterSet.begin();
    auto elem = *eIter;
    masterSet.erase(eIter);

    // Without
    listSubset(masterSet, used);

    // With
    used.insert(elem);
    listSubset(masterSet, used); 
    masterSet.insert(elem);
    used.erase(elem);
  }
}

void subSets(set<string>& masterSet) {
  set<string> chosen;
  listSubset(masterSet, chosen);
}



int main() {

  //hello();

  vector<string> stringVec = { "Bob", "Jane", "Dylan", "Fonda"};
  cout << "Permutations: " << endl;
  permute(stringVec);

  cout << "Subsets: " << endl;
  set<string> stringSet(begin(stringVec), end(stringVec));
  subSets(stringSet);

  return 0;
}
