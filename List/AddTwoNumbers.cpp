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
class Solution
{
public:
  ListNode *addTwoNumbers(ListNode *l1, ListNode *l2)
  {
    ListNode *ret = nullptr;
    ListNode *prev = nullptr;
    ListNode *newRoot = nullptr;
    int over = 0;
    while (l1 || l2)
    {
      int sum = 0;
      if (l1 && l2)
      {
        sum = (l1->val + l2->val);
      }
      else if (l1)
      {
        sum = l1->val;
      }
      else
      {
        sum = l2->val;
      }

      if (sum > 9 || sum + over > 9)
      {
        auto val = sum + over - 10;
        ret = new ListNode(val);
        over = 1;
      }
      else
      {
        ret = new ListNode(sum + over);
        over = 0;
      }

      if (nullptr == prev)
      {
        prev = ret;
        newRoot = ret;
      }
      else
      {
        prev->next = ret;
        prev = ret;
      }

      if (l1)
        l1 = l1->next;
      if (l2)
        l2 = l2->next;
    }
    if (over)
    {
      auto ret = new ListNode(1);
      prev->next = ret;
    }
    return newRoot;
  }
};