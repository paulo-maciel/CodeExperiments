
// WIP

class Solution
{
public:
  void combine(string str, set<string> &strSet)
  {
  }

  unordered_map<int, string> keyPad;
  vector<string> letterCombinations(string digits)
  {
    vector<string> ret;
    string comb;
    vector<string> strSet;
    // populate strSet with all sets of string in digits.
    for (auto d : digits)
    {
      int num = atoi(d);
      auto letters = keyPad[num];
      for (auto l : letters)
      {
        comb.push_back(l);
      }
      strSet.push_back(comb);
      comb.clear();
    }

    // Now find the permutations.
    set<string> S;
    for (auto str : strSet)
    {
      for (auto letter : str)
      {
        combine(letter, strSet);
      }
    }

    auto iter = strSet.begin();
    for (; iter != strSet.end(); ++iter)
    {
      ret.push_back(*iter);
    }
    return ret;
  }
};