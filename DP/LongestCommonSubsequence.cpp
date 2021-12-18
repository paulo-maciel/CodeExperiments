
// Works but not efficient.
class Solution {
public:
    vector<vector<int>> DP;
    int dp(int i, int j) {
        if (i < 0) return 0;
        if (j < 0) return 0;
        
        if (t1[i] == t2[j]) {
            if (DP[i][j] == -1) {
              DP[i][j] = 1 + dp(i-1,j-1);
            }
        } else {
            if (DP[i][j] == -1) {
              DP[i][j] = max(dp(i-1,j), dp(i, j-1));
            }
        }
        
        return DP[i][j];
    }
    int longestCommonSubsequence(string text1, string text2) {
        // Let dp(i, j) represent the longest common subsequence between 
        // the string text1 up to index i and text2 up to index j.
        //If text1[i] == text2[j], then we should use this character, giving us 1 + dp(i - 1, j - 1). 
        // Otherwise, we can either move one character back from text1, or 1 character back from text2. Try both.
        t1 = text1;
        t2 = text2;
        DP= vector(t1.length(), vector<int>(t2.length(),-1));
        return dp(t1.length()-1, t2.length()-1);
    }
    string t1;
    string t2;
};