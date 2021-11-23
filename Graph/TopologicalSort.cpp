class Solution {
public:
    unordered_map<char, vector<char>> precede;
    string ans;
    string alienOrder(vector<string>& words) {
        int numWords = words.size();
        for(int i = 0; i < numWords - 1; ++i) {
            for(int j = i + 1; j < numWords; ++j) {
                auto lesser = min(words[i].length(), words[j].length());
                
                // Check for prefix constraint here.
                
                for(int k = 0; k < lesser; ++k) {
                    if (words[i][k] != words[j][k]) {
                        char li = words[i][k];
                        char lj = words[j][k];
                        if (precede.count(li)) {
                            // Don't add duplicates
                            vector<char> existingLetters = precede[li];
                            auto pred = [lj](char c) { return c == lj; };
                            auto iter = find_if(existingLetters.begin(), existingLetters.end(), pred);
                            if (iter == existingLetters.end()) {
                                precede[li].push_back(lj);
                            }
                        } else {
                            precede[li].push_back(lj);
                        }
                    }
                }
            }
        }
precede.erase(precede.begin());
        
        // Do topological sort on map precede
        //printMap();
        topologicalSort();
        
        return ans;
    }
    
    void printMap() {
        auto iter = precede.begin();
        while(iter != precede.end()) {
            
            // Get depedency 
            cout << iter->first << "----> ";
            for(auto letter : iter->second) {
                cout << letter << ",";
            }
            cout << endl;
            
            ++iter;
        }
    }
    
    void topologicalSortUtil(char c, vector<bool> &visited, stack<char> &s) {
        visited[c - 0x61] = true;
        for(auto adjLetter : precede[c]) {
            auto adjIdx = adjLetter - 0x61;
            if (!visited[adjIdx]) {
                topologicalSortUtil(adjLetter, visited, s);
            }
        }
        
        s.push(c);
    }
    
    void topologicalSort() {
        // Iterate through the map.
        vector<bool> visited = vector<bool>(26, false);
        stack<char> s;
        auto iter = precede.begin();
        while(iter != precede.end()) {
            auto letter = iter->first;
            
            auto idx = letter - 0x61;
            if (visited[idx] == false) {
                topologicalSortUtil(letter, visited, s);
            }
            ++iter;
        }
        
        while(!s.empty()) {
            ans.push_back(s.top());
            s.pop();
        }
    }
};
