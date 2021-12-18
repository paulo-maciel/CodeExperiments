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

    vector<vector<int>> verticalOrder(TreeNode* root) {
        vector<vector<int>> ans;
        if (nullptr == root) {
            return ans;
        }
        queue<pair<TreeNode *,int>> q;
        q.push({root,0});
        
        while(!q.empty()) {
            pair<TreeNode *,int> nodeCol = q.front();
            q.pop();
            
            int col = nodeCol.second;
            TreeNode *node = nodeCol.first;
            if (nullptr == node) {
                continue;
            }
            colVal[col].push_back(node->val);
            q.push({node->left, col-1});
            q.push({node->right, col+1});
        }
        
        
        for(auto it = colVal.begin(); it != colVal.end(); ++it) {
            auto vec = it->second;
            ans.push_back(vec);
            
        }
        
        return ans;
    }
    // all the values associated to a column.
    map<int, vector<int>> colVal;
};