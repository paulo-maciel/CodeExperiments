/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
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
class Solution
{
public:
  TreeNode *sortedListToBST(ListNode *head)
  {
    if (nullptr == head)
    {
      return nullptr;
    }

    TreeNode *root = nullptr;

    // Get list size
    int sz = 0;

    // Find middle of head, l1, l2, r1, r2
    ListNode *left = nullptr;
    ListNode *right = nullptr;
    ListNode *center = nullptr; //breakList(head, &left, &right);

    // size 1
    if (nullptr == head->next)
    {
      return new TreeNode(head->val);
    }

    // size 2
    if (nullptr == head->next->next)
    {
      root = new TreeNode(head->next->val);
      root->left = new TreeNode(head->val);
      return root;
    }

    ListNode *slow = head, *fast = head, *pre = nullptr;
    sz = 1;

    // Break the list in two.
    while (fast && fast->next)
    {
      pre = slow;
      slow = slow->next;
      fast = fast->next->next;
      sz += 2;
    }
    // Close left list
    if (pre && sz != 2)
      pre->next = nullptr;

    // Start right list
    right = slow->next;
    left = head;
    center = slow;
    if (center)
    {
      cout << "center: " << center->val << endl;
      root = new TreeNode(center->val);
      root->left = sortedListToBST(left);
      root->right = sortedListToBST(right);
    }

    return root;
  }
};

// Less efficient solution.
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode() : val(0), next(nullptr) {}
 *     ListNode(int x) : val(x), next(nullptr) {}
 *     ListNode(int x, ListNode *next) : val(x), next(next) {}
 * };
 */
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
class Solution
{
public:
  // Breaks the list and returns its center;
  ListNode *breakList(ListNode *head, ListNode **left, ListNode **right)
  {
    if (nullptr == head)
      return head;

    // Get the size of the list and break it in half.
    *left = head;
    ;
    *right = nullptr;
    int sz = 0;
    auto *node = head;
    while (node)
    {
      node = node->next;
      ++sz;
    }
    if (sz < 3)
    {
      return nullptr;
    }
    int mid = 0.5 * sz;
    *left = head;
    node = head;
    for (int i = 0; i < mid - 1; ++i)
    {
      node = node->next;
    }
    auto midNode = node->next;
    *right = midNode->next;
    node->next = nullptr;

    return midNode;
  }

  TreeNode *sortedListToBST(ListNode *head)
  {
    if (nullptr == head)
    {
      return nullptr;
    }

    // partial list
    auto node = head;
    while (node)
    {
      cout << node->val << ",";
      node = node->next;
    }
    cout << endl;

    // Find middle of head, l1, l2, r1, r2
    ListNode *left = nullptr;
    ListNode *right = nullptr;
    TreeNode *root = nullptr;
    ListNode *center = breakList(head, &left, &right);
    if (center)
    {
      cout << "center: " << center->val << endl;
      root = new TreeNode(center->val);
      root->left = sortedListToBST(left);
      root->right = sortedListToBST(right);
    }
    else
    {
      // List has less than 3 elements;
      if (nullptr == head->next)
      {
        root = new TreeNode(head->val);
      }
      else
      {
        root = new TreeNode(head->next->val);
        root->left = new TreeNode(head->val);
      }
    }

    return root;
  }
};