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

using namespace std;
class Heap {
    public:
    void addHeap(int val) {
        // append at the end then ....
        heap_.push_back(val);
        //cout << "adding: " << val << endl;
        
        // percolate up checking if current is min
        // for a min heap.
        int curr = heap_.size() - 1;
        int parent = floor((curr-1)/2);
        while(parent >= 0 && heap_[curr] < heap_[parent]) {
            swap(heap_[curr], heap_[parent]);
            curr = parent;
            parent = floor((curr-1)/2);
        }
        // cout << "Current heap: " << endl;
        // for(size_t i = 0; i < heap_.size(); ++i)
        //   cout << " " << heap_[i];
        // cout << "end heap." << endl;
    }
    
    int find(int val) {
       for(size_t i = 0; i < heap_.size(); ++i) {
           if (heap_[i] == val) 
             return i;
       }
    
       return -1;
    }

    bool exist(int n) {
        return (n >=0 && n < heap_.size()) ? true : false;
    }
    
    void deleteHeap(int val) {
        auto last = heap_.size() - 1;
        int curr = find(val);
        swap(heap_[curr], heap_[last]);
        heap_.pop_back();

        while(true) {
          int leftChild = 2*curr + 1;
          int rightChild = 2*curr + 2;
          if (exist(leftChild) && exist(rightChild)) {
            if (heap_[leftChild] < heap_[rightChild]) {
                swap(heap_[curr], heap_[leftChild]);
                curr = 2*curr + 1;
            } else {
                swap(heap_[curr], heap_[rightChild]);
                curr = 2*curr + 2;
            }
          } else {
              // Got to the bottom of the heap.
              if (exist(leftChild)) {
                  if (heap_[curr] > heap_[leftChild]) {
                      swap(heap_[curr], heap_[leftChild]);
                      break;
                  } 
              } else if (exist(rightChild)) {
                  if (heap_[curr] > heap_[rightChild]) {                                    
                      swap(heap_[curr], heap_[rightChild]);
                      break;
                  }
              }
              // bottom of the heap, we are done anyways.
              break;
          }
        }
    }
    
    void printHeap() {
        cout << heap_[0] << endl;
    }
    
    private:
      vector<int> heap_;
};

Heap heap;

void processInput(int op, int val) {
    switch(op) {
        case 1:
          heap.addHeap(val);
          break;
        case 2:
          heap.deleteHeap(val);
          break;
        case 3:
           heap.printHeap();
           break;
        default:
           break;
    }
}

int main() {
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */  
    int numQueries = 0;
    cin >> numQueries;
    vector<int> ops;
    vector<int> vals;
    int op, val;
    for(int i = 0; i < numQueries; ++i) {
        cin >> op;
        ops.push_back(op);
        if (op != 3) {
          cin >> val;
          vals.push_back(val);
        } else {
          vals.push_back(-1);
        }
    }

    for(size_t i = 0; i < ops.size(); ++i) {
        int op = ops[i];
        int v = vals[i];
        //cout << "Processing: " << op << " " << v << endl;
        processInput(op, v);
    }
     
    return 0;
}