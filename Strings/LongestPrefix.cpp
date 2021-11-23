class Solution
{
public:
  struct TrieNode
  {
    TrieNode()
    {
      for (auto &letter : letters)
      {
        letter = nullptr;
      }
    }
    array<TrieNode *, 25> letters;

    bool getOnlyChild(int &idx)
    {
      int count = 0;
      idx = 0;
      for (int i = 0; i < letters.size(); ++i)
      {
        if (letters[i] != nullptr)
        {
          ++count;
          if (count == 1)
          {
            idx = i;
          }
        }
      }
      //cout << "idx " << idx << " count: " << count << endl;
      return count == 1 ? true : false;
    }
    bool isEnd = false;
  };

  void getLongest(TrieNode *root, string &maxPrefix)
  {
    TrieNode *node = root;
    bool found = false;
    int idx;
    while (node->getOnlyChild(idx) && !node->isEnd)
    {
      maxPrefix.push_back(idx + 0x61);
      node = node->letters[idx];
    }
  }

  string longestCommonPrefix(vector<string> &strs)
  {
    TrieNode root;
    string maxPref;

    // Build a trie.
    TrieNode *node = nullptr;
    for (auto str : strs)
    {
      if (str.empty())
        return maxPref;
      node = &root;
      for (auto letter : str)
      {
        // get letter index.
        int idx = letter - 0x61;
        if (node->letters[idx] == nullptr)
        {
          // add key.
          node->letters[idx] = new TrieNode();
          node = node->letters[idx];
          continue;
        }
        node = node->letters[idx];
      }
      node->isEnd = true;
    }

    // Depth first on trie to find the longest one.
    getLongest(&root, maxPref);

    return maxPref;
  }
};