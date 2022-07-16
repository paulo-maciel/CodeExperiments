#include <iostream>
#include <map>
using namespace std;

struct Node {
    int val;
    Node* next;
    Node* random;

    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};

//A->A'
//B->B'
//C->C'

//A, A'
//A'.random = map(A.random);

static Node* deepCopy(Node* head) {
    if (!head) {
      return nullptr;
    }

    map<Node*, Node*> randMap;
    Node* headP = new Node(head->val);
    Node* prevP = headP;
    Node* next = nullptr;
    Node* curr = head;
    while(!curr) {
       next = curr->next;
       prevP->next = new Node(next->val); 

      randMap[curr] = prevP->next;
       prevP = prevP->next;
       curr = next;
    }

    curr = head;
    prevP = headP;
    while(!curr) {
    }

    return headP;
}

//A->B->C
//A.random -> C
//B.random -> nullptr
//C.random -> A

//A'->B'->C'
//A'.random -> C'
//B'.random -> nullptr
//C'.random -> A'

// To execute C++, please define "int main()"
int main() {
  auto words = { "Hello, ", "World!", "\n" };
  for (const string& word : words) {
    cout << word;
  }
  return 0;
}

// Your previous Plain Text content is preserved below:

// This is just a simple shared plaintext pad, with no execution capabilities.

// When you know what language you'd like to use for your interview,
// simply choose it from the dropdown in the top bar.

// You can also change the default language your pads are created with
// in your account settings: https://app.coderpad.io/settings

// Enjoy your interview!


