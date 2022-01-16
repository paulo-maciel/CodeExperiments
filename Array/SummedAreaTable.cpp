//Range Sum Query 2D - Immutable
// Not very efficient though in computing NumMatrix.  Needs to improve.
class NumMatrix {
public:
    NumMatrix(vector<vector<int>>& matrix) {
        matOrig = matrix;
        matSum = matOrig;
        
        // Horizontal sum
        for(int r = 0; r < matrix.size(); ++r) {
            auto row = matrix[r];
            for(int c = 1; c < row.size(); ++c) {
                row[c] += row[c-1];
            }
            matSum[r] = row;
        }
        
        // Vertical sum
        for(int c = 0; c < matrix[0].size(); ++c) {
            for(int r = 1; r < matrix.size(); ++r) {
                matSum[r][c] += matSum[r-1][c];
            }
        }
    }
    
    int sumRegion(int row1, int col1, int row2, int col2) {
        auto D = matSum[row2][col2];
        int A = 0;
        if (row1 > 0 && col1 > 0) {
            A = matSum[row1 - 1][col1 - 1];
        }
        int B = 0;
        if (row1 > 0) {
            B = matSum[row1 - 1][col2];
        }
        int C = 0;
        if (col1 > 0) {
            C = matSum[row2][col1 - 1];
        }
        return A + D - B - C;;
        
    }
    
    vector<vector<int>> matOrig;
    vector<vector<int>> matSum;
};

/**
 * Your NumMatrix object will be instantiated and called as such:
 * NumMatrix* obj = new NumMatrix(matrix);
 * int param_1 = obj->sumRegion(row1,col1,row2,col2);
 */
