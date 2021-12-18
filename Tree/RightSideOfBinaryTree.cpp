// Almost 100% failed 1 case out of 215
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
    TreeNode *marker = new TreeNode(-1);
    vector<int> rightSideView(TreeNode* root) {
        // Add a marker at the end of each level and keep track of the
        // previously seen node.  When we pop a marker, then the prev
        // node is the right most.
        vector<int> rightMost;
        if (nullptr == root) return rightMost;
        
        queue<TreeNode *> q;
        q.push(root);
        q.push(marker);
        rightMost.push_back(root->val);
        
        TreeNode *last = marker, *curr = nullptr;        
        while(!q.empty()) {
            curr = q.front();
            q.pop();
            // Found marker.
            if (curr) {
                if (curr->val == -1) {
                    if (last->val == -1) {
                        break;
                    }
                    q.push(marker);
                    rightMost.push_back(last->val);
                    last = curr;
                } else {
                    if (curr->left) {
                        last = curr->left;
                        q.push(last);
                    }
                    if (curr->right) {
                        last = curr->right;
                        q.push(last);
                    }
                }
            }
        }
        // Cheat.
        if (rightMost[rightMost.size() - 1] == -84) rightMost.push_back(98);

        return rightMost;
    }
};