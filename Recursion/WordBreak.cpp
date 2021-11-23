class Solution {
public:
    set<string> wordDictSet;
    vector<int> memo;
    
    bool wordBreakRec(const string &s, int start) {
        if (start == s.length()) {
            return true;
        }
        
       if (memo[start] != -1) {
           return memo[start];
       }
        
       for(int end = start + 1; end <= s.length(); ++end) {
           auto prefix = s.substr(start, end - start);

           if (wordDictSet.count(prefix) && wordBreakRec(s, end)) {
               memo[start] = true;
               return memo[start] ;
           }
       }
       
       return memo[start] = false;
    }
    
    bool wordBreak(string s, vector<string>& wordDict) {
       wordDictSet = set<string>(wordDict.begin(), wordDict.end());
        
       // -1 here means 'don't know yet.
       memo = vector<int>(s.length(), -1);
       return wordBreakRec(s, 0);
    }
};