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

class Solution {
public:
    Solution(vector<int>& nums) {
      //nums_.resize(nums.size());
      copy(nums.begin(), nums.end(), back_inserter(shuffled_));
      copy(nums.begin(), nums.end(), back_inserter(nums_));
      srand(time(NULL)); //use current time as seed for random generator.  Should be called only once.
    }
    
    /** Resets the array to its original configuration and return it. */
    vector<int> reset() {
      copy(nums_.begin(), nums_.end(), shuffled_.begin());  
      return shuffled_;
    }

    int getPos() {
      return static_cast<int>(((float)rand()/RAND_MAX) * nums_.size());
    }
    
    /** Returns a random shuffling of the array. */
    vector<int> shuffle() {
      // say shuffled_ array is: 4 27 19 7
      vector<int> out;
      if (shuffled_.size() < 2) {
        return shuffled_;
      }
      set<int> seen;

      // while seen positions set size != nums_.size
        // get two random positions and discard if they are the same.
        // Get 2 different unseen positions
        //    while(pos1 == pos2 or pos1 || pos2 is seen) ) pos1 = getPos, pos2 = getPos;
        // Add the 2 positions to a 'seen' set.
        // swap(pos1, pos2);
        // Another algo:
        // Put all the positions on a set.
        // Randomly get the position until all the positions are removed from the set.
        // Yet another:
        // The Fisher-Yates algorithm is remarkably similar to 
        // the brute force solution. On each iteration of the algorithm, 
        // we generate a random integer between the current index and the 
        // last index of the array. Then, we swap the elements at the current 
        // index and the chosen index - this simulates drawing (and removing) 
        // the element from the hat, as the next range from which we select a
        //  random index will not include the most recently processed one. 
        //  One small, yet important detail is that it is possible to swap an 
        //  element with itself - otherwise, some array permutations would be 
        //  more likely than others.
      while(seen.size() < nums_.size()) {
        auto p0 = 0;
        auto p1 = p0;

        while(p0 == p1 || seen.count(p0) || seen.count(p1)) {
          // get new positions.
          p0 = getPos();
          p1 = getPos();
        }

        // Have now different positions.
        seen.insert(p0);
        seen.insert(p1);
        swap(shuffled_[p0], shuffled_[p1]);
      }
        
      return shuffled_;
    }

    void printShuffled() {
      cout << "Nums: " ;
      for(auto n : shuffled_) {
        cout << n << " ";
      }
      cout << endl;
    }

    vector<int> nums_; 
    vector<int> shuffled_;
};

int main() {
  vector<int> vec(10);
  iota(vec.begin(), vec.end(), 0);

  auto *sol = new Solution(vec);
  cout << "Original: " << endl;
  for(int i = 0; i <= 10; ++i) {
    sol->printShuffled();
    sol->shuffle();
  }
  cout << endl;

  return 0;
}
