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

// for pop_heap
#include <algorithm>

using namespace std;

// Heap comparator.
struct Comp {
  bool operator()(const tuple<string, int>& t1, const tuple<string, int>& t2) {
    // Check for count first. t1 < t2
    if (get<1>(t1) < get<1>(t2)) {
      return true;
    }

    if (get<1>(t1) > get<1>(t2)) {
      return false;
    }

    // // Check alphabetically.
    if (get<0>(t1) > get<0>(t2)) {
      return true;
    }

    return false;
  }
};

vector<string> topKFrequent(vector<string>& words, int k) {
    // Frequency of word sorted highest to lowest
    // If same frequency, sort in ascending alphabetical order
    // There is at least one frequent word
    // use lowercase.
    // O(nlogk) exec and O(n) space
    unordered_map<string, int> wordMap;
    for(auto word : words) {
      wordMap[word]++;
    }
    
    // Add a string, count tuple to the heap.
    vector<tuple<string, int>> wordHeap;
    for( auto kvp : wordMap) {
      // tuple of count, string.
      auto t = make_tuple(kvp.first, kvp.second);
      wordHeap.push_back(t);
    }
    
    make_heap(wordHeap.begin(), wordHeap.end(), Comp());

cout << "Heap array: " << endl;
for(auto t : wordHeap) {
  cout << get<0>(t) << " ";
}
cout << endl;
for(auto t : wordHeap) {
  cout << get<1>(t) << " ";
}
cout << endl;
    
    // Should be sorted correctly.
    vector<string> output;
    while(!wordHeap.empty()) {
      pop_heap(wordHeap.begin(), wordHeap.end());
      auto t = wordHeap.back();
      output.push_back(get<0>(t));
      wordHeap.pop_back();

// check the heap
cout << "Pop: " << endl;
for(auto t : wordHeap) {
  cout << get<0>(t) << " ";
}
cout << endl;
for(auto t : wordHeap) {
  cout << get<1>(t) << " ";
}
cout << endl;

    }

    return output; 
}

bool comp(const int& i1, const int& i2) {
  return i1 < i2 ? true : false;
}

int main() {

  // vector<string> words = {"i", "love", "leetcode", "i", "love", "coding"};
  // int k = 2;

  vector<string> words = {"the", "day", "is", "sunny", "the", "the", "the", "sunny", "is", "is"};
  int k = 4;

  vector<string> kWords = topKFrequent(words, k);

  cout << "Top k words: " << endl;
  for(auto word : kWords) {
    cout << word << ", ";
  }
  cout << endl;


  vector<tuple<string, int>> tuples = { make_tuple("day", 1), make_tuple("is", 3), make_tuple("sunny", 2), make_tuple("the", 4)};
  make_heap(tuples.begin(), tuples.end(), Comp());
    vector<string> output;
    while(!tuples.empty()) {
      pop_heap(tuples.begin(), tuples.end());
      auto t = tuples.back();
      output.push_back(get<0>(t));
      tuples.pop_back();

// check the heap
cout << "Pop: " << endl;
for(auto t : tuples) {
  cout << get<0>(t) << " ";
}
cout << endl;
for(auto t : tuples) {
  cout << get<1>(t) << " ";
}
cout << endl;

    }


  return 0;
}