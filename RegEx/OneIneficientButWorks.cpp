class Solution {
public:
    // Recursive approach.
    bool isMatch(string str, string pat) {
        if (pat.empty()) return str.empty();
        
        // base case.
        bool firstMatch = (!str.empty() && (pat[0] == str[0] || pat[0] == '.'));
                
        // if we have only 2 characters or we don't have '*'
        // then first and remaining must match.
        if (pat.length() < 2 || pat[1] != '*') {
            return firstMatch && isMatch(str.substr(1), pat.substr(1));
        } else {
            // pattern size > 2 and pat[1] is a '*'
            // match the remaining pattern since * matches all str. Don't care about first matching or not.
            bool res = isMatch(str, pat.substr(2)) || 
                       (firstMatch && isMatch(str.substr(1), pat));   
            return res;
        }
    }
};


// With DP but not giving the right solution.
class Solution {
public:
    // Recursive approach.
    bool isMatch(string str, string pat) {
        // Why + 1?
        memo = vector<vector<int>>(str.size() + 1, vector<int>(pat.size() + 1, -1));
        return dp(0,0,str,pat);
    }
    
    // dp returns true if string str starting at i matches pattern pat starting at j.
    bool dp(int i, int j, string str, string pat) {
        if (memo[i][j] != -1) {
            return memo[i][j] == 1;
        }
        
        // ret is the return value of for starting i and j positions.
        bool ret;
        if (j == pat.length()) {
            // if processed the whole pattern, has to have processed the whole string as well.
            ret = (i == str.length());
        }
        
        // base case.
        bool firstMatch = (i < str.length() && (pat[j] == str[i] || pat[j] == '.'));
                
        // if we have only 2 characters or we don't have '*'
        // then first and remaining must match.
        if (j + 1 < pat.length() && pat[j + 1] == '*') {
            ret = dp(i,j+2, str, pat) || firstMatch && dp(i+1,j, str, pat);   
        } else {
            ret = firstMatch && dp(i+1,j+1,str,pat);
        }
        
        memo[i][j] = ret ? 1 : 0;
        return ret;
    }
    
    vector<vector<int>> memo;
};