#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>
#include <set>

using namespace std;

    // Intersection of 2 arrays usint sets.
    vector<int> intersection(vector<int>& nums1, vector<int>& nums2) {
        vector<int> intVec;
        unordered_set<int> intSet;
        unordered_set<int> Set;
        vector<int>* vec = nullptr;
                                                                      
        if (nums1.size() < nums2.size()) {
            for(auto e : nums1) {
                Set.insert(e);
                vec = &nums2;
            }
        } else {
            for(auto e : nums2) {
                Set.insert(e);
                vec = &nums1;
            } 
        }
        
        for(auto elem : *vec) {
            if (Set.count(elem) == 1) {
                intSet.insert(elem);
            }
        }
        for(auto e : intSet) {
            intVec.push_back(e);
        }
        return intVec;
    }

template <class Type>
bool findDuplicates(vector<Type>& keys) {
    // Replace Type with actual type of your key
    unordered_set<Type> hashset;
    for (Type key : keys) {
        if (hashset.count(key) > 0) {
            return true;
        }
        hashset.insert(key);
    }
    return false;
}

int main() {
  vector<int> intVec;
  intVec.push_back(1);
  intVec.push_back(2);
  intVec.push_back(2);

  cout << findDuplicates<int>(intVec) <<endl;;


  return 1;
}