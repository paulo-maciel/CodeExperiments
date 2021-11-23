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
  ListNode *mergeTwoLists(ListNode *l1, ListNode *l2)
  {
    if (nullptr == l1)
    {
      return l2;
    }

    if (nullptr == l2)
    {
      return l1;
    }

    ListNode *pL1 = l1;
    ListNode *pL2 = l2;
    ListNode *head = pL1->val <= pL2->val ? pL1 : pL2;
    ListNode *curr = head;
    while (pL1 && pL2)
    {

      if (pL1 && pL2 && pL1->val <= pL2->val)
      {
        while (pL1 && pL2 && pL1->val <= pL2->val)
        {
          curr = pL1;
          pL1 = pL1->next;
        }
        curr->next = pL2;
      }

      if (pL1 && pL2 && pL2->val <= pL1->val)
      {
        while (pL1 && pL2 && pL2->val <= pL1->val)
        {
          curr = pL2;
          pL2 = pL2->next;
        }

        curr->next = pL1;
      }
    }

    if (nullptr == pL1)
    {
      curr->next = pL2;
    }

    if (nullptr == pL2)
    {
      curr->next = pL1;
    }

    return head;
  }
};