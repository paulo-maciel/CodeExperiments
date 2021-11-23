class Solution {
public:
    map<int,int> indexDecs;
    
    int numDecs(int index, const string& s) {
        if (indexDecs.count(index)) {
            return indexDecs.at(index);
        }
        
        if (index == s.size()) {
            return 1;
        }
        
        if (s[index] == '0') {
            return 0;
        }
        
        if (index == s.size() - 1) {
            return 1;
        }
        
        int ret = numDecs(index + 1, s);
        if (atoi(s.substr(index, 2).c_str()) <= 26) {
           ret += numDecs(index + 2, s);   
        }
        
        indexDecs[index] = ret;
        
        return ret;
    }
    int numDecodings(string s) {
        return numDecs(0, s);
    }
};