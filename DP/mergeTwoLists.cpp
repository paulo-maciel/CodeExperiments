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

std::vector<double> pw;
double myPow2(double x, int n) {
    if (n == 0) {
        pw[0] = 1.0;
        return 1.0;
    }
    
    if (pw.size() == 0) {
        pw.resize(n + 1);
        std::fill(pw.begin(), pw.end(), -1);
    }

    if (pw[n] == -1) {
        myPow2(x,n-1);
    } 

    pw[n] = x * pw[n - 1];
    return pw[n];
}

double myPowTR(double x, int n, double acc) {
    if (n == 0) {
        return acc;
    }

    if (pw.size() == 0) {
        pw.resize(n + 1);
        std::fill(pw.begin(), pw.end(), -1);
    }
    if (pw[n] == -1) {
        pw[n] = myPowTR(x, n - 1, x*acc);
    }

    return pw[n];
}

double myPow(double x, int n) {
    return myPowTR(x, n, 1.0);
}

    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
        ListNode* p1 = l1;
        ListNode* p2 = l2;
        if (!p1 && !p2) return nullptr;
        if (p1 == nullptr) return p2;
        if (p2 == nullptr) return p1;
        ListNode* m = (p1->val < p2->val ? p1 = p1->next, l1 : p2 = p2->next,l2);
        ListNode* pm = m;
        while(p1 && p2) {
            if (p1->val < p2->val) {
                pm->next = p1;
                pm = p1;
                p1 = p1->next;
            } else {
                pm->next = p2;
                pm = p2;
                p2 = p2->next;
            } 
        }
        if (!p1) {
            pm->next = p2;
        }        
        
        if (!p2) {
            pm->next = p1;
        }
        
        return m;
    }
    
int main() {

    for (auto i = 1; i < 100; ++i) {
        pw.clear();
        cout << "myPow(" << i << "): " <<  myPow(3, i) << endl;
    }

  return 1;
}