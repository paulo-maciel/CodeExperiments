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

vector<string> topKFrequent(vector<string>& words, int k) {
    // Frequency of word sorted highest to lowest
    // If same frequency, sort in ascending alphabetical order
    // There is at least one frequent word
    // use lowercase.
    // O(nlogk) exec and O(n) space
    multiset<string> wordSet;
    for(auto word : words) {
        wordSet.insert(word);
    }
    
    // This is like:
    // sortedWords: the is sunny ball
    // sortedCount:  4  3    2    1
    deque<string> sortedWords;
    deque<int> sortedCount;
    
    // Iterate through the set
    //   get word and place it inside the sorted deque
    //   according to their frequency and if same
    //   sort according to alphabetical order.
    auto it = wordSet.begin();
    for(; it != wordSet.end(); ) {
        auto word = *it;
        auto wordCount = wordSet.count(word);

        // Remove word from the wordSet.
        while(wordSet.count(word) > 0) {
          it = wordSet.erase(it);
        }
        //cout << "Processing word: " << word << endl;
        
        // Find a place to insert and then insert it
        size_t i = 0;
        while (i < sortedCount.size() && wordCount < sortedCount[i]) {
            ++i;
            continue;
        }

        //cout << "Insert it at: " << i << endl;
        // Now insert.
        // Insert alphabetically.
        auto j = i;
        //cout << "word: " << word << " sortedWords[" << j << "]: " << sortedWords[j] << endl;
        auto count = wordCount;
        while(j < sortedWords.size() && count == sortedCount[j] && word > sortedWords[j]) {
            ++j;
        }

        sortedCount.insert(sortedCount.begin() + j, wordCount);
        sortedWords.insert(sortedWords.begin() + j, word);
        //cout << "Inserted: " << word << " at: " << j << endl;
    }
    
    vector<string> output;
    // output k most frequent elements to return.
    for(auto i = 0; i < k && i < sortedWords.size(); i++) {
        output.push_back(sortedWords[i]);
    }
    
    return output; 
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

  return 0;
}