\\ Find the indices of the two numbers in the array that add up to target.
class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        std::vector<int> outVec;
        std::map<int,int> indMap;
        
        for(int i = 0; i < nums.size(); ++i) {
            int diff = target - nums[i];
            if (indMap.count(diff) != 0) {
                outVec.push_back(indMap[diff]);
                outVec.push_back(i);
                return outVec;
            } else {
                indMap[nums[i]] = i;
            }
        }
        return outVec;
    }

};