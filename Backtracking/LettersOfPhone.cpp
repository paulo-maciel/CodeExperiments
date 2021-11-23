class Solution
{
public:
  vector<string> combineStrings(vector<string> s1, vector<string> s2)
  {
    vector<string> combination;
    for (auto str1 : s1)
    {
      for (auto str2 : s2)
      {
        combination.push_back(str1 + str2);
      }
    }
    return combination;
  }

  vector<string> letterCombinations(string digits)
  {
    vector<string> combinations;
    unordered_map<char, vector<string>> digitLettersMap{{'2', {"a", "b", "c"}},
                                                        {'3', {"d", "e", "f"}},
                                                        {'4', {"g", "h", "i"}},
                                                        {'5', {"j", "k", "l"}},
                                                        {'6', {"m", "n", "o"}},
                                                        {'7', {"p", "q", "r", "s"}},
                                                        {'8', {"t", "u", "v"}},
                                                        {'9', {"w", "x", "y", "z"}}};
    int digitCount = digits.size();
    if (digitCount == 0)
      return combinations;
    if (digitCount == 1)
    {
      string str;
      for (auto ch : digitLettersMap[digits[0]])
      {
        str += ch;
      }
      combinations.push_back(str);
      return combinations;
    }

    // 2 or more digits.
    vector<vector<string>> possibleComb;
    for (auto digit : digits)
    {
      possibleComb.push_back(digitLettersMap[digit]);
    }

    // generate the combinations
    int i = digitCount - 1;
    for (; i > 0; --i)
    {
      auto iterSecondToLast = possibleComb.end();
      iterSecondToLast -= 2;
      vector<string> secondToLast = *iterSecondToLast;
      auto cStrings = combineStrings(secondToLast, possibleComb.back());
      possibleComb.pop_back();
      possibleComb.pop_back();
      possibleComb.push_back(cStrings);
    }
    combinations = possibleComb[0];

    return combinations;
  }
};