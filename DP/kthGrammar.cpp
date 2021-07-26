#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>

using namespace std;

string processStr(string str) {
    if (str.empty()) {
        return "";
    }
    string rep = "01";
    if (str[0] == '1') {
        rep = "10";
    }
    return rep + processStr(str.substr(1));
}

int kthGrammar(int n, int k) {
    string prev = "0";
    string curr;
    for(auto i = 1; i < n; ++i) {
        auto temp = processStr(prev);
        prev = curr;
        curr = temp;
    }

    char ch[2];
    ch[0] = curr[k-1];
    ch[1] = '\0';
    return atoi(&ch[0]);
}

int main() {
    cout << "kthGrammar(30,32774): " <<  kthGrammar(30,32765) << endl; //434991989) << endl;
  return 1;
}