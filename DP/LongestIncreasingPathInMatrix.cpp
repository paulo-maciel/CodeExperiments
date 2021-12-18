class Solution {
public:
    int nrows;
    int ncols;
    
    vector<vector<int>> dp;
    int dir[4][2] = {{-1,0}, {0,1}, {1,0}, {0,-1}};

    int longestPathRec(vector<vector<int>>& matrix, vector<int> origin) {
      
      int lPath = 1;
      for(int i = 0; i < 4; ++i) {
        vector<int> newCell = {dir[i][0], dir[i][1]};
        newCell[0] += origin[0];
        newCell[1] += origin[1];

        // Check if valid
        if ( (newCell[0] >= 0 && newCell[0] < nrows) && 
             (newCell[1] >= 0 && newCell[1] < ncols) &&
             (matrix[origin[0]][origin[1]] < matrix[newCell[0]][newCell[1]]) ) {
          if (dp[newCell[0]][newCell[1]] == -1) {
            dp[newCell[0]][newCell[1]] = longestPathRec(matrix, newCell);
          }
          lPath = max(lPath, 1 + dp[newCell[0]][newCell[1]] );
        }
      }
      return lPath;
    }
    
    int longestIncreasingPath(vector<vector<int>>& matrix) {
      nrows = matrix.size();
      ncols = matrix[0].size();
      dp = vector<vector<int>>(nrows, vector<int>(ncols, -1));
      int lPath = 1;
      for(int i = 0; i < nrows; ++i) {
        for(int j = 0; j < ncols; ++j) {
          if (dp[i][j] == -1) {
            dp[i][j] = longestPathRec(matrix, {i,j});
          }
          lPath = max(lPath, dp[i][j]);
        }
      }

      return lPath;
    }
};