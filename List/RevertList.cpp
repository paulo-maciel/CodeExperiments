#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

struct LinkNode {
  LinkNode(int v) : val(v) {}
  int val;
  LinkNode* next = nullptr;
};

void printList(LinkNode* head) {
  auto next = head;
  cout << "Begin list: " << endl;
  while(next) {
    cout << next->val << " ";
    next = next->next;
  }
  cout << endl << "End list" << endl;
}

void RevertList(LinkNode*& head) {
  if (!head || !head->next) {
    return;
  }
  LinkNode* prev = head;
  LinkNode* curr = head->next;
  prev->next = nullptr;
  LinkNode* next = curr->next;

// 1->2->3->nil
  while(next) {
    curr->next = prev;
    prev = curr;
    curr = next;

    next = curr->next;
  }
  curr->next = prev;
  head = curr;
}

LinkNode* removeNthFromEnd(LinkNode*& head, int n) {
    RevertList(head);
    printList(head);
    auto* node = head;
    LinkNode* toDelete;
    if (n == 1) {
        toDelete = head;
        head = head->next;
    } else {
        for(int i = 0; i < n - 2; ++i) {
            node = node->next;
        }

        toDelete = node->next;
        node->next = toDelete->next;
    }
    printList(head);
    
    RevertList(head);
    
    printList(head);
    return toDelete;
}

int main()
{
  LinkNode* head = new LinkNode(1);
  head->next = new LinkNode(2);
  head->next->next = new LinkNode(3);

  RevertList(head);

  printList(head);

  return 1;
}