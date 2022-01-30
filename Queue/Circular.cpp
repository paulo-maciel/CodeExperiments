class MyCircularQueue {
public:
    MyCircularQueue(int k) {
        sz = k;
        cqueu = new int[k];
        for(int i = 0; i < sz; ++i) {
            cqueu[i] = -1;
        }
        f = b = 0;
    }
    
    bool enQueue(int value) {
        if (isFull()) {
            return false;
        }
        ++nEnq;
       
        cqueu[b] = value;
        b = (b + 1) % sz;
        return true;
    }
    
    bool deQueue() {
        if (isEmpty()) {
            return false;
        }
        
        ++nDeq;
        f = (f + 1) % sz;
        return true;
    }
    
    int Front() {
        return cqueu[f];
    }
    
    int Rear() {
        if (!isEmpty()) {
           if (b == 0) {
              return cqueu[sz - 1];
           } else {
               return cqueu[b - 1];
           }
        }
        return -1;
    }
    
    bool isEmpty() {
        return nEnq == nDeq;
    }
    
    bool isFull() {
        // for(int i = 0; i < sz; ++i) {
        //     cout << cqueu[i] << " ";
        // }
        // cout << endl;
        // cout << b << " " << " " << f << " " << sz << endl;
        return (nEnq - nDeq) >= sz;
    }
    
    int *cqueu = nullptr;
    int f = 0;
    int b = 0;     // Note: b points to the place in the back of the queue for insertions.
    int nEnq = 0;  // Note: Should have used count, instead of nEnq and nDeq.
    int nDeq = 0;
    int sz;
};

/**
 * Your MyCircularQueue object will be instantiated and called as such:
 * MyCircularQueue* obj = new MyCircularQueue(k);
 * bool param_1 = obj->enQueue(value);
 * bool param_2 = obj->deQueue();
 * int param_3 = obj->Front();
 * int param_4 = obj->Rear();
 * bool param_5 = obj->isEmpty();
 * bool param_6 = obj->isFull();
 */