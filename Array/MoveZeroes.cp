#include <algorithm>

class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        
#ifdef BUBBLE_ZEROE
    int i, j;
    int count = nums.size();
    for (i = 0; i < count -1; i++)        
        // Last i elements are already in place
        for (j = 0; j < count -i -1; j++)
            if (nums[j] == 0)
                std::swap(nums[j], nums[j+1]);
    }
#endif
    
};

// Terrible runtime
#include <algorithm>

class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int zeroCount = 0;
        // Push all zeroes to the back.
        int initSize = nums.size();
        for(int i = 0; i < initSize; ++i) {
            if (nums[i] == 0) {
                ++zeroCount;
                nums.push_back(0);
            }
        }
        
        // Drop zeroCount;
        int i = 0;
        auto iter = nums.begin();
        while(zeroCount > 0) {
            if (*iter == 0) {
                iter = nums.erase(iter);
                --zeroCount;
                continue;
            }
            ++iter;
        }
    }
    
};