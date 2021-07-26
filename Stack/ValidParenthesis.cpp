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

// for pop_heap
#include <algorithm>

using namespace std;

    char counterpart(char c) {
        switch(c) {
            case ')':
                return '(';
            case '}':
                return '{';
            case ']':
                return '[';
        }
        return '\0';
    }
    
    bool isValid(string s) {
        stack<char> st;
        
        for(auto c : s) {
            
            // handle open
            if (c == '(' || c == '{' || c == '[') {
                st.push(c);
                continue;
            } 

            // handle close
            if (c == ')' || c == '}' || c == ']') {
                if (!st.empty() && st.top() == counterpart(c)) {
                    st.pop();
                } else {
                    st.push(c);
                }  
            }
        }
        
        return st.empty();
    }

int main() {


  return 0;
}