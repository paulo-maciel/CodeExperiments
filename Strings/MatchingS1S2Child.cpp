#include <bits/stdc++.h>

using namespace std;

set<string> words;

// Complete the commonChild function below.
int commonChild(string s1, string s2) {
  string currWord;
  int currPos2 = 0;
  auto gi = 0ul;
  while(gi < s1.length()) {
    for(auto i = gi; i < s1.length(); ++i) {
      auto pos2 = s2.find(s1[i], currPos2);
      if (pos2 == string::npos) {
        continue;
      }
      // Found in 2.
      currWord += s1[i];
      currPos2 = pos2 + 1;
      if (pos2 >= s2.length() || i == s1.length() - 1) {
        // last position of 2.  Start over.
        words.insert(currWord);
        currPos2 = 0;
        currWord = "";
        continue;
      }
    }
    if (currWord.length() > 0) {
      words.insert(currWord);
      currPos2 = 0;
      currWord = "";
    }
    
    ++gi;
    currPos2 = 0;
  }
  
  // return the size of the largest word in the set.
  size_t maxWord = 0;
  for(auto w : words) {
    maxWord = max(maxWord, w.length());
  }
  
  return maxWord;
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    string s1;
    getline(cin, s1);

    string s2;
    getline(cin, s2);

    int result = commonChild(s1, s2);

    fout << result << "\n";

    fout.close();

    return 0;
}

