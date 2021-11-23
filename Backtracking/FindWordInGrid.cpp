// Incomplete

class Solution
{
public:
  bool existStarting(vector<vector<char>> &board, int k, int l, int prevk, int prevl, int widx, string word)
  {
    int m = board.size();
    int n = board[0].size();
    if (widx >= word.size())
    {
      return false;
    }

    // Next letter
    auto nextLetter = word[widx];
    // Examine the 4 corners around k,l, except prevk, prevl
    if (isValid(k - 1, l, prevK, prevl))

      return true;
  }

  bool exist(vector<vector<char>> &board, string word)
  {
    // find the all positions of all words starting with word[0]
    int m = board.size();
    int n = board[0].size();
    bool wordStarted = false;
    int k = 0;
    int l = 0;
    int wordIdx = 0;
    for (int i = 0; i < m; i++)
    {
      for (int j = 0; j < n; ++j)
      {
        if (!wordStarted)
        {
          if (board[i][j] == word[0])
          {
            wordStarted = true;
            k = i;
            l = j;
            wordIdx++;
          }
        }
        else
        {
          // In process of finding word.  Try to find a match of the
          // next letter around board[k][l]
          if (j == 0)
          {
            prevk = k - 1;
            prevl = n - 1;
          }
          else
          {
            prevk = k;
            prevl = j - 1;
          }
          if (existStarting(board, k, l, prevk, prevl, wordIdx, word))
          {
            return true;
          }
          else
          {
            // Keep looking for a start.
            wordStarted = false;
          }
        }
      }
    }

    return false;
  }
};