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

using namespace std;


struct Dict {
    int getIndex(char ch) {
        return ch - (0x60 +  1);
    }
    struct Node {
        Node(char l) : letter(l), isPrefix(false) {
            fill(letters.begin(),letters.end(), nullptr);
        }
        char letter;
        bool isPrefix;
        array<Node*, 25> letters;
    };
    
    Dict() : root(new Node(0xFF)) {}
    
    Node *addLetter(Node* node, char letter) {
        // check for null node.
        if (nullptr == node) {
            return node;
        }
        
        int ind = getIndex(letter);
        if (ind < 0) return nullptr;
        if (node->letters[ind] == nullptr) {
            node->letters[ind] = new Node(letter);
        }
            
        return node->letters[ind];
    }
    
    // word is a perfix
    Node *getWord(Node *root, const string& inWord, string& outWord) {
        auto *node = root;
        for(auto letter : inWord) {
            int ind = getIndex(letter);
            if (node->letters[ind] == nullptr) {
                break;
            } else {
                node = node->letters[ind];
                outWord += letter;
            }
        }
        
        return (node && node->isPrefix) ? node : nullptr;
    }
    
    Node *root;
};

string replaceWords(vector<string>& dictionary, string sentence) {
    /* Build dictionary
        for each word in dictionary
            for each letter in word
                letterNode = Add letter to dictionary(root, letter);
            
    */
    Dict dict;
    using Node = Dict::Node;
    for(auto word : dictionary) {
        Node *lastNode = dict.root;
        for(auto letter : word) {
            lastNode = dict.addLetter(lastNode, letter);
        }
        lastNode->isPrefix = true;
    }
    
    string outString;
    string prefix;
    /* For each wordSentence in sentence
            wordDict = Get word from dictionary (root, wordSentence)
            if word empty
            outString += wordSentence
            else
            outString += wordDict
            outString += " ";
    
    */
    vector<string> tokenSentence;
    size_t ssz = sentence.size();
    char* sent = new char[ssz];
    strncpy(sent, sentence.c_str(), ssz);
    auto token = strtok(sent, " ");
    while(token) {
        tokenSentence.push_back(token);
        token = strtok(NULL, " ");
    }
    cout << endl;
    for(auto token : tokenSentence) {
        prefix.clear();
        auto dictWordNode = dict.getWord(dict.root, token, prefix);
        if (dictWordNode == nullptr) {
            outString += token;
        } else {
            outString += prefix;
        }
        outString += " ";
    }
    
    return outString;
}

int main() {
    vector<string> dictionary = {"cat","bat","rat"};
    string sentence = "the cattle was rattled by the battery";

    string outString = replaceWords(dictionary, sentence);
    cout << "Input: " << sentence << endl;
    cout << "Output: " << outString << endl;
    return 1;
}