// Simple solution
class Solution {
public:   
    
    bool backtrack(const vector<int>& nums, int idx, int totSets, int currSet, int currSum, int targetSum) {
        // currSet starts at 0 and totSets starts at 1.
        if (currSet == totSets - 1) {
            return true;
        }
        
        // The current set exceeds the target sum.  Need to backtrack.
        if (currSum > targetSum) {
            return false;
        }
        
        // Finished a set.
        if (currSum == targetSum) {
            return backtrack(nums, 0, totSets, currSet + 1, 0, targetSum);
        }
        
        for(int i = idx; i < nums.size(); ++i) {
            if (!taken[i]) {
                taken[i] = true;
                if (backtrack(nums, i, totSets, currSet, currSum + nums[i], targetSum)) {
                    return true;
                }
                taken[i] = false;
            }
        }
        
        
        return false;
    }
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int totSum = accumulate(begin(nums), end(nums), 0);
        if (totSum % k != 0) return false;
        int targetSum = totSum/k;

        taken = vector<int>(nums.size(), false);
        
        sort(begin(nums), end(nums), greater<int>());
        
        return backtrack(nums, 0, k, 0, 0, targetSum);
    }
    
    vector<int> taken;
    
};

// Listing states.
class Solution {
public:   
    void printSolution() {
        for(auto state : sols) {
            cout << "(";
            for(auto n  = 0; n < state.size(); ++n) {
                cout << state[n];
                if (n + 1 < state.size()) {
                    cout << ",";
                }
            }
            cout << ")" << endl;
        } 
    }
    bool backtrack(const vector<int>& nums, int idx, int totSets, int currSet, int currSum, int targetSum) {
        if (currSet == totSets - 1) {
            return true;
        }
        
        // The current set exceeds the target sum.  Need to backtrack.
        if (currSum > targetSum) {
            return false;
        }
        
        // Finished a set.
        if (currSum == targetSum) {
            sols.push_back(state);
            state.clear();
            return backtrack(nums, 0, totSets, currSet + 1, 0, targetSum);
        }
        
        for(int i = idx; i < nums.size(); ++i) {
            if (!taken[i]) {
                taken[i] = true;
                state.push_back(nums[i]);
                if (backtrack(nums, i, totSets, currSet, currSum + nums[i], targetSum)) {
                    return true;
                }
                if (!state.empty()) {
                    state.pop_back();
                }
                taken[i] = false;
            }
        }
        
        return false;
    }
    bool canPartitionKSubsets(vector<int>& nums, int k) {
        int totSum = accumulate(begin(nums), end(nums), 0);
        if (totSum % k != 0) return false;
        int targetSum = totSum/k;

        taken = vector<bool>(nums.size(), false);
        
        sort(begin(nums), end(nums), greater<int>());
        
        auto ret = backtrack(nums, 0, k, 0, 0, targetSum);
        printSolution();
        return ret;
    }
    
    vector<bool> taken;
    vector<int> state;
    vector<vector<int>> sols;
};