/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* left;
    Node* right;
    Node* next;

    Node() : val(0), left(NULL), right(NULL), next(NULL) {}

    Node(int _val) : val(_val), left(NULL), right(NULL), next(NULL) {}

    Node(int _val, Node* _left, Node* _right, Node* _next)
        : val(_val), left(_left), right(_right), next(_next) {}
};
*/

class Solution {
public:
    Node* connect(Node* root) {
      queue<Node *> q;
      if (!root) return nullptr;
      
      q.push(root);
      while(!q.empty()) {
        cout << endl;
        int sz = q.size();
        Node *prev = nullptr;
        // flush the current level, setting the next pointer.
        for(int i = 0; i < sz; ++i) {
          auto node = q.front();
          q.pop();
          if (!node) {
            continue;
          }
          if (prev) {
            prev->next = node;
          }
            
          prev = node;
          //cout << prev->val << " ";
          q.push(node->left);
          q.push(node->right);
        }
      }
        
      return root;
    }
};