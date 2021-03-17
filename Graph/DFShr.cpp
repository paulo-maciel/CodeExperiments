#include <bits/stdc++.h>
#include <utility>

using namespace std;

static vector<vector<bool>> visited;

int regionCount(int row, int col, const vector<vector<int>>& grid);

// Complete the maxRegion function below.
int maxRegion(vector<vector<int>> grid) {
  int nrows = grid.size();
  int ncols = grid[0].size();

  // Initialize the visited grid.
  for(int row = 0; row < nrows; ++row) {
    vector<bool> rowVec;
    for(int col = 0; col < ncols; ++col) {
      rowVec.push_back(false);
    }
    visited.push_back(move(rowVec));
  }
      
  int maxCount = 0;
  for(auto row = 0; row < nrows; ++row) {
    for(auto col = 0; col < ncols; ++col) {
      if (visited[row][col] || grid[row][col] == 0) {
        continue;
      }
      
      int count = regionCount(row, col, grid);
      maxCount = max(count, maxCount);
    }
  }
  
  return maxCount;
}

vector<pair<int,int>> getNeighbors(int row, int col, const vector<vector<int>>& grid) {
  vector<pair<int,int>> cells;
  int nrows = grid.size();
  int ncols = grid[0].size();

  // assume row, col has 6 neighbors but check boundaries.
  // a  b  c
  // d cell e
  // f  g  h
  
  // a
  if (row - 1 >= 0 && col - 1 >= 0) {
    cells.push_back(make_pair(row - 1, col -1));
  }
  
  // b 
  if (row - 1 >= 0 && col >= 0) {
    cells.push_back(make_pair(row - 1, col));
  }
  
  // c
  if (row - 1 >= 0 && col + 1 <= ncols - 1) {
    cells.push_back(make_pair(row - 1, col + 1));
  }  
  
  // d
  if (row >= 0 && col - 1 >= 0) {
    cells.push_back(make_pair(row, col - 1));
  } 
  
  // e
  if (row >= 0 && col + 1 <= ncols - 1) {
    cells.push_back(make_pair(row , col + 1));
  } 
  
  // f 
  if (row + 1 <= nrows - 1 && col - 1 >= 0) {
    cells.push_back(make_pair(row + 1, col - 1));
  } 
  
  // g
  if (row + 1 <= nrows - 1 && col >= 0) {
    cells.push_back(make_pair(row + 1, col));
  } 
  
  // h 
  if (row + 1 <= nrows - 1 && col + 1 <= ncols - 1) {
    cells.push_back(make_pair(row + 1, col + 1));
  } 
    
  return cells;
}

int regionCount(int row, int col, const vector<vector<int>>& grid) {
  
  if (grid[row][col] == 0 || visited[row][col]) {
    return 0;
  }
  
  visited[row][col] = true;
      
  // Select all neighbors of row,col
  auto neighbors = getNeighbors(row, col, grid);
  if (neighbors.empty()) {
    return 1;
  }
  
  int total = 1;
  for(auto neigh : neighbors) {
    total += regionCount(neigh.first, neigh.second, grid);
  }
  
  return total;
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    int n;
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    int m;
    cin >> m;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<vector<int>> grid(n);
    for (int i = 0; i < n; i++) {
        grid[i].resize(m);

        for (int j = 0; j < m; j++) {
            cin >> grid[i][j];
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int res = maxRegion(grid);

    fout << res << "\n";

    fout.close();

    return 0;
}

