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
#include <map>
#include <ostream>
#include <iterator>

// for pop_heap
#include <algorithm>

using namespace std;

//     Given an integer array nums where the elements are sorted in ascending order, convert it to a height-balanced binary search tree.

// A height-balanced binary tree is a binary tree in which the depth of the two subtrees of every node never differs by more than one.
    
/**
 * Definition for a binary tree node.
 */
 #define COUNT 3 
struct Node {
  int val;
    Node *left;
    Node *right;
    Node() : val(0), left(nullptr), right(nullptr) {}
    Node(int x) : val(x), left(nullptr), right(nullptr) {}
    Node(int x, Node *left, Node *right) : val(x), left(left), right(right) {}
};

Node *buildBST(vector<int>& nums, int beg, int end) {

  if (beg > end) {
    return nullptr;
  }

  int mid = (beg + end)/2;
  Node *root = new Node(nums[mid]);

  root->left = buildBST(nums, beg, mid - 1);
  root->right = buildBST(nums, mid + 1, end);
  
  return root;
}
    
Node* sortedArrayToBST(vector<int>& nums) {  
  return buildBST(nums, 0, nums.size() - 1);
}

void preOrder(Node* node)
{
    if (node == NULL)
        return;
    cout << node->val << " ";
    preOrder(node->left);
    preOrder(node->right);
}
        
int main() {
  vector<int> nums = {1, 2, 3, 4, 5, 6, 7}; // Produces 4 2 1 3 6 5 7 

  preOrder(sortedArrayToBST(nums));

  return 0;
}
