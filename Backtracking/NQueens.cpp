// This code finds only one solution and not all of them for a given nxn board.  How to find all of them?

class Solution {
public:
    bool isSafe(const vector<string> &board, int row, int col) {
        return isSafeRow(board, row, col) &&
               isSafeUpperDiag(board, row, col) && 
               isSafeLowerDiag(board, row, col);
    }
       
    bool isSafeRow(const vector<string> &board, int row, int col) {
        for(int c = 0; c < col; ++c ) {
            if (board[row][c] == 'Q') {
                return false;
            }
        }
        return true;
    }
    
    bool isSafeUpperDiag(const vector<string> &board, int row, int col) {
        while(row >= 0 && col >= 0) {
            if (board[row][col] == 'Q') {
                return false;
            }
            --row;
            --col;
        }
        return true;
    }
    
    bool isSafeLowerDiag(const vector<string> &board, int row, int col) {
        while(row < n && col >= 0) {
            if (board[row][col] == 'Q') {
                return false;
            }
            ++row;
            --col;
        }
        return true;    
    }
    
    bool hasQueen(const vector<string> &board, int row) {
        for(int c = 0; c < n; ++c) {
            if (board[row][c] == 'Q') {
                return true;
            }
        }
        return false;
    }
    
    bool boardDone(const vector<string> &board) {
        for(int r = 0; r < n; ++r) {
          if (!hasQueen(board, r)) {
              return false;
          }
        }
        
        return true;
    }
 
    void addQueen(vector<string> &board, int row, int col) {
        board[row][col] = 'Q'; 
    }
    
    void removeQueen(vector<string> &board, int row, int col) {
        board[row][col] = '.'; 
    }
    
#if 0
    bool Solve(Grid<bool> &board, int col)
    {
        if (col >= board.numCols()) return true;         // base case
        
        for (int rowToTry = 0; rowToTry < board.numRows(); rowToTry++) {
            if (IsSafe(board, rowToTry, col)) {
                PlaceQueen(board, rowToTry, col);                 // try queen here
                if (Solve(board, col + 1)) return true;           // recur to place rest
                RemoveQueen(board, rowToTry, col);                // failed, remove, try again
            }
        }
    }
#endif
    
    bool backtrack(vector<string> &board, int col) {

        // If we tried all cols, return true to fill another row
        if (col >= n) {
            return true;
        }
        
        for(int r = 0; r < n; ++r) {
            // Try row r and select the move if save.  Otherwise, backtrack.
            if (isSafe(board, r, col)) {
                addQueen(board, r, col);
                if (backtrack(board, col + 1))
                    return true;
                removeQueen(board, r, col);
            }

        }
        
        return false;
    }
    
    vector<vector<string>> solveNQueens(int sz) {
        n = sz;
        vector<string> board = vector<string>(n, string(n,'.'));
            
        backtrack(board, 0);
        sols.push_back(board);
        return sols;
    }
    
    int n;
    vector<vector<string>> sols;
};