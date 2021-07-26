#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>

using namespace std;

void reverseString(vector<char>& s) {
  int b = 0, e = s.size() - 1;
  while(b <= e) {
    swap(s[b], s[e]);
    ++b;
    --e;
  }
}

#if 0
ListNode* swapPairs(ListNode* head) {
    if (nullptr == head || nullptr == head->next) {
        return head;
    }
    
    ListNode* next = head->next;
    ListNode* nextHead = next->next;
    next->next = head;
    head = next;
    head->next->next = nextHead;
    
    head->next->next = swapPairs(head->next->next);
    
    return head; 
}

ListNode* reverseListIter(ListNode* head) {
    
    if (!head || !head->next) {
        return head;
    }
    ListNode *prev = nullptr, *next = head->next;
    ListNode *curr = head;
    
    while(next) {
        prev = curr;
        curr = next;
        next = curr->next;
      
        // revert
        curr->next = prev;         
    }
    
    head->next = nullptr;
    return curr;
}

ListNode* reverseListRecursive(ListNode* head) {
    if (!head || !head->next) {
        return head;
    }
    
    ListNode *revertedList = reverseListRecursive(head->next);
    ListNode *endList = revertedList;
    
    // append the current head node to the end of the revertedList
    while(endList->next) {
        endList = endList->next;
    }
    endList->next = head;
    head->next = nullptr;
    
    return revertedList;
}

ListNode* reverseList(ListNode* head) {
    return reverseListIter(head);
    //return reverseListRecursive(head);
}

int pascal(int i, int j, int** cache) {
    // base case
    if (j == 0 || j == i) {
        cache[i][j] = 1;
        return 1;
    }
    
    if (cache[i - 1][j - 1] == -1) {
        cache[i - 1][j - 1] = pascal(i - 1, j - 1, cache);
    }
    if (cache[i - 1][j] == -1) {
        cache[i - 1][j] = pascal(i - 1, j, cache);
    }
    
    return cache[i][j] = cache[i - 1][j - 1] + cache[i - 1][j];
}

vector<int> getRow(int rowIndex) {

    vector<int> row;
    int numCols = rowIndex + 1;
    int numRows = numCols;
    int **cache = new int*[numRows];
    
    for(int i = 0; i < numRows; ++i) {
        cache[i] = new int[numCols];
        for(int j = 0; j < numCols; ++j) {
            cache[i][j] = -1;
        }
    }
    int i = rowIndex;
    for(int j = 0; j < numCols; ++j) {
        row.push_back(pascal(i,j, cache));
    }
    return row;
}
#endif

static int32_t *fibCache = nullptr;

int fibStatic(int n) {
    if (!fibCache) {
        fibCache = new int[n + 1];
        for(auto i = 0; i < n; ++i) {
            fibCache[i] = -1;
        }
    } 

    if (n == 0 || n == 1) {
        return n;
    }

    if (fibCache[n - 1] == -1) {
        fibCache[n - 1] = fibStatic(n - 1);
    }

    if (fibCache[n - 2] == -1) {
        fibCache[n - 2] = fibStatic(n - 2);
    }

    fibCache[n] = fibCache[n - 1] + fibCache[n - 2];
    return fibCache[n];   
}

std::vector<int> fibCacheVec;
int fib(int n) {

    if (n == 0 || n == 1) {
        return n;
    }

    if (fibCacheVec.size() == 0) {
        fibCacheVec.resize(n + 1);
        //std::cout << "Distance " << n << ": " << std::distance(fibCacheVec.begin(), fibCacheVec.end()) << endl;
        std::fill(fibCacheVec.begin(), fibCacheVec.end(), -1);
    }
    if (fibCacheVec[n] == -1) {
        fibCacheVec[n] = fib(n - 1) + fib(n - 2);
    }
    return fibCacheVec[n];   
}

int main() {
  // vector<char> str = {'a', 'b', 'c', 'd', 'e', 'f'};
  // reverseString(str);

  // stringstream ss;
  // for(auto ch : str) {
  //   ss << ch;
  // }
  // ss << endl;

  // cout << ss.str() << endl;

    for (auto i = 0; i < 10; ++i) {
        delete fibCache;
        fibCache = nullptr;
        fibCacheVec.clear();
        cout << "fib(" << i << "): " <<  fib(i) << endl;
    }

  return 1;
}