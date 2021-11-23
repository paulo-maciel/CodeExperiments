class Solution {
public:
    struct Trie {
        vector<Trie *> letters = vector<Trie *>(26, nullptr);
        bool endWord = false;
    };
    
    bool wordBreak(string s, vector<string>& wordDict) {
        // Build a trie from wordDict
        auto root = new Trie();
        auto node = root;
        for(auto word : wordDict) {
            for(auto letter : word) {
                int idx = letter - 0x61;
                if (node->letters[idx] == nullptr) {
                    auto newNode = new Trie();
                    node->letters[idx] = newNode;
                    node = newNode;
                } else {
                    node = node->letters[idx];
                } 
            }
            node->endWord = true;
            node = root;
        }
        

        
        // Iterate through s and as long as we can find a word in the trie
        // return true, return false at the first failure.
        node = root;  
        for(auto letter : s) {
            auto idx = letter - 0x61;
            if (node->letters[idx] == nullptr) {
                return false;
            }
            
            node = node->letters[idx];
            if (node->endWord) {
                node = root;
            }
        }
        if (node != root && !node->endWord) {
            cout << "false" << endl;
            return false;
        }
        return true;
    }
};