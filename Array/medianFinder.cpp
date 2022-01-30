// Faster than only 5% memory better than 90%
class MedianFinder {
public:
    MedianFinder() {
        
    }
    
    // Find index using binary search
    int find_index(const deque<int> &arr, int n, int K)
    {
        // Lower and upper bounds
        int start = 0;
        int end = n - 1;

        // Traverse the search space
        while (start <= end) {
            int mid = (start + end) / 2;

            // If K is found
            if (arr[mid] == K)
                return mid;

            else if (arr[mid] < K)
                start = mid + 1;

            else
                end = mid - 1;
        }

        // Return insert position
        return end + 1;
    }
    
    void addNum(int num) {
        ++count;
        if (count && count % 2 == 0) {
            ++idx;
        }
            
        isElem = !isElem;
        
        // Assume array is sorted.  Find the insertion position.
        int pos = find_index(elems, elems.size(), num);
        
        auto iter = elems.begin() + pos;
        elems.insert(iter, num);
    }
    
    double findMedian() {
        if (isElem) {
            return elems[idx];
        }
        
        return (elems[idx] + elems[idx+1])*0.5;
        
    }
    // Number of times addNum was called.
    int count = -1;
    int idx = 0;
    
    // Alternates between element and average.
    bool isElem = false;
    
    deque<int> elems;
};

/**
 * Your MedianFinder object will be instantiated and called as such:
 * MedianFinder* obj = new MedianFinder();
 * obj->addNum(num);
 * double param_2 = obj->findMedian();
 */


 // Using priority queues
 class MedianFinder {
    priority_queue<int> lo;                              // max heap
    priority_queue<int, vector<int>, greater<int>> hi;   // min heap

public:
    // Adds a number into the data structure.
    void addNum(int num)
    {
        lo.push(num);                                    // Add to max heap

        hi.push(lo.top());                               // balancing step
        lo.pop();

        if (lo.size() < hi.size()) {                     // maintain size property
            lo.push(hi.top());
            hi.pop();
        }
    }

    // Returns the median of current data stream
    double findMedian()
    {
        return lo.size() > hi.size() ? lo.top() : ((double) lo.top() + hi.top()) * 0.5;
    }
};