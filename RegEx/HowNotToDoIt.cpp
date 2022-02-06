class Solution {
public:
    bool isLetter(char c) {
        string alpha = "abcdefghijklmnopqrstuvwxyz";
        return alpha.find(c) != string::npos ? true : false;
    }
    bool isMatch(string str, string pat) {
        char *s = str.data();
        char *p = pat.data();
        int ip = 0;
        int is = 0;
        while(is < str.size() && ip < pat.size()) {
            // Pattern contain letter and next character is not '*'
            if (isalpha(p[ip]) && (ip + 1 <= pat.size() || p[ip+1] != '*')) {
                if (s[is] == p[ip]) {
                    ++is;++ip;
                    continue;
                } else {
                    return false;
                }
            }
            
            // pattern contain '.'
            if (p[ip] == '.') {
                if (ip + 1 < pat.size() && p[ip + 1] == '*') {
                    // .* found, match the remaining of s.  If pattern has more characters
                    // then string, return false, otherwise we are done.
                    cout << "found .*" << is << "," << ip << endl;
                    return (ip + 2) >= pat.size() ? true : false;
                } else {
                    ++ip;++is; continue;
                }
            } else {
                
                // p is '*'.  Get letter before '*'
                auto letter = p[ip - 1];
                while(s[is] == letter) {
                    ++is;
                } 
                ++ip;
            }
        }
        return (is == str.size() && ip == pat.size()) ? true : false;
    }
};