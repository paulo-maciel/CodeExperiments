/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> ret;
        queue<TreeNode*> q;
        q.push(root);
        
        while(!q.empty()) {
            auto sz = q.size();
            vector<int> vec;
            for(int i = 0; i < sz; ++i) {
                TreeNode *n = q.front();
                q.pop();
                if (!n) {
                    // vec.push_back('\0');
                    continue;
                }
                vec.push_back(n->val);
                q.push(n->left);
                q.push(n->right);
            }
            if (!vec.empty()) {
                ret.push_back(vec);
            }
        }
            
        return ret;
    }
};