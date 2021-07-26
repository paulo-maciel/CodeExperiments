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
    
    // Add a concatenation of the count, ';', word to a heap.
    // then as we extract from the heap, we remove the number
    // and add to output.
    vector<string> wordHeap;
    for( auto kvp : wordMap) {
      stringstream ss;
      ss << kvp.second << ";" << kvp.first;
      wordHeap.push_back(ss.str());
    }
    make_heap(wordHeap.begin(), wordHeap.end());
    
    vector<string> output;
    while(!wordHeap.empty()) {
      pop_heap(wordHeap.begin(), wordHeap.end());
      output.push_back(wordHeap.back());
      wordHeap.pop_back();
    }

    return output; 
}

int main() {

  vector<string> words = {"i", "love", "leetcode", "i", "love", "coding"};
  int k = 2;

  // vector<string> words = {"the", "day", "is", "sunny", "the", "the", "the", "sunny", "is", "is"};
  // int k = 4;

  vector<string> kWords = topKFrequent(words, k);

  cout << "Top k words: " << endl;
  for(auto word : kWords) {
    cout << word << ", ";
  }
  cout << endl;

  return 0;
}