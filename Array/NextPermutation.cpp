class Solution {
public:
    void nextPermutation(vector<int>& nums) {
      if (nums.size() == 1) {
        return;
      }
      
      // from the back of the array, find the point where
      // the number to the left of i is smaller than the one in i
      // since we want to replace that one.
      int i = nums.size() - 2;
      while(i >= 0 && nums[i+1] <= nums[i]) {
        --i;
      }
      
      // starting again from the right, find the smallest greater than nums[i-1]
      if (i >= 0) {
        int j = nums.size() - 1;
        while(nums[j] <= nums[i]) {
          --j;
        }
        
        swap(nums[i], nums[j]);
      }
      
      // if it can't find, sort nums in ascending order 
      sort(nums.begin() + i + 1, nums.end());
    }
};