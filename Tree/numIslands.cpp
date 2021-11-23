class Solution
{
public:
  bool isIsland(vector<vector<char>> &grid, int m, int n, int i, int j)
  {

    if (grid[i][j] == '0')
    {
      return false;
    }

    // Island: only one, either horizontal or vertical neighbor is 1 OR
    //         all neighbors hor/vert are 0.

    // Check left,right, bottom, top (all neighbors hor/vert are 0)
    if ((i - 1 < 0 || grid[i - 1][j] == '0') &&
        (i + 1 >= m || grid[i + 1][j] == '0') &&
        (j - 1 < 0 || grid[i][j - 1] == '0') &&
        (j + 1 >= n || grid[i][j + 1] == '0'))
    {
      //cout << "i: " << i << " j: " << j << " returning: " << true << endl;
      return true;
    }

    int landCount = 0;
    if (i - 1 >= 0 && grid[i - 1][j] == '1')
    {
      ++landCount;
    }
    if (i + 1 < m && grid[i + 1][j] == '1')
    {
      ++landCount;
    }
    if (j - 1 >= 0 && grid[i][j - 1] == '1')
    {
      ++landCount;
    }
    if (j + 1 < n && grid[i][j + 1] == '1')
    {
      ++landCount;
    }

    //cout << "i: " << i << " j: " << j << " landCount: " << landCount << " returning: " << (landCount <= 1 ? true : false) << endl;
    return (landCount <= 1 ? true : false);
  }

  int numIslands(vector<vector<char>> &grid)
  {
    int count = 0;
    int m = grid.size();
    int n = grid[0].size();
    if (m * n == 1)
      return grid[0][0] == '0' ? 0 : 1;
    bool isBigIsland = true;
    for (int i = 0; i < m; ++i)
    {
      for (int j = 0; j < n; ++j)
      {
        if (grid[i][j] == 0)
        {
          isBigIsland = false;
        }
      }
    }
    if (isBigIsland)
      return 1;

    for (int i = 0; i < m; ++i)
    {
      for (int j = 0; j < n; ++j)
      {
        bool isil = isIsland(grid, m, n, i, j);
        if (isil == true)
        {
          ++count;
          //cout << "count: " << count << " isIsland: " << isil << endl;
        }
      }
    }
    return count;
  }
};

// Not fully working.