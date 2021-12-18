class Solution {
public:
    string simplifyPath(string path) {
      // /dir/../ = drop from / to / = 
      // /./
      // Tokenize with ./
      // Put tokens in a stack.
      // Process the stack into another stack
      // convert the stack to a string and return it.
      stringstream ss(path);
      string token;
      stack<string> st;
      while(getline(ss, token, '/')) {
        if (token == "." || token.empty()) {
          continue;
        } else if (token == "..") {
          if (!st.empty()) {
            st.pop();
          }
        } else {
          st.push(token);
        }
      }
      
      // Copy to another stack, st2
      stack<string> st2;
      while(!st.empty()) {
        st2.push(st.top());
        st.pop();
      }
      
      // Build the answer string.
      stringstream retStr;
      while(!st2.empty()) {
        retStr << "/";
        retStr << st2.top();
        st2.pop();
      }

      string finalStr = retStr.str();

      return finalStr.length() > 0 ? finalStr : "/";
    }
};