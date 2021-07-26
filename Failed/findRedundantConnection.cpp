#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <tuple>

using namespace std;

class Solution {
public:
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        vector<int> out;
        for(auto edge : edges) {
            if (checkClose(edge[0], edge[1])) {
                st.push(make_tuple(edge[0], edge[1]));
            }
        }
        tuple<int,int> t = st.top();
        out.push_back(get<0>(t));
        out.push_back(get<1>(t));
        return out;
    }

    bool checkClose(int i, int j) {
        if (s.count(j) && s.count(i)) {
            return true;
        }
        if (!s.count(j)) {
            s.insert(j);
        } 
        if (!s.count(i)) {
            s.insert(i);
        }
        
        return false;
    }
    
    set<int> s;
    stack<tuple<int,int>> st;
};

int main() {



  return 0;
}