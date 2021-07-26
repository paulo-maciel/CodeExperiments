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

string generateNextStr(string str, string acc) {
    if (str.empty()) {
        cout << "Empty returning acc:" << acc << endl;
        return acc;
    }
    string rep = "01";
    if (str[0] == '1') {
        rep = "10";
    }
    string reducedString = str.empty() ? "" : str.substr(1);
    return generateNextStr(reducedString, rep + reducedString);
}

int kthGrammar(int n, int k) {
    string prev = "0";
    string curr;
    // remember previous
    for(auto i = 1; i < n; ++i) {
        string reducedString = prev.empty() ? "" : prev.substr(1);
        cout << "reducedString: " << reducedString << endl;
        auto temp = generateNextStr(reducedString, (prev[0] == '0' ? "01" : "10") + reducedString);
        prev = curr;
        curr = temp;
        cout << "Prev: " << prev << " curr: " << curr << endl;
    }

    char ch[2];
    ch[0] = curr[k-1];
    ch[1] = '\0';
    return atoi(&ch[0]);
}

int main() {
  cout << "kthGrammar(1,1): " <<  kthGrammar(30,10000) << endl; //434991989) << endl;
//   string str = "0";
//   cout << "substr: " << str.substr(1) << "---" << (str.substr(1).empty() ? "empty" : "not empty") << endl;
  return 1;
}