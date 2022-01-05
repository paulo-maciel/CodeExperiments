
// Somehow, it is only able to track the state for small inputs.  Crashes when using 'state' push/pop  otherwise.
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
    bool backtrack(vector<int>& arr, int index, int count, int currSum) {
      
        // We made k - 1 subsets with target sum and last subset will also have target sum.
        if (count == totSets) { 
            return true;
        }
        
        // No need to proceed further.
        if (currSum > bucket) { 
            return false;
        }
      
        // When curr sum reaches target then one subset is made.
        // Increment count and reset current sum.
        if (currSum == bucket) {
            sols.push_back(state);
            state.clear();
            return backtrack(arr, 0, count + 1, 0);
        }
        
        // Try not picked elements to make some combinations.
        for (int j = index; j < nElems; ++j) {
            if (!taken[j]) {
                // Include this element in current subset.
                taken[j] = true;
                //state.push_back(arr[j]);
                
                // If using current jth element in this subset leads to make all valid subsets.
                if (backtrack(arr, j + 1, count, currSum + arr[j])) {
                    return true;
                }
                
                // Backtrack step.
                taken[j] = false;
                //state.pop_back();
            }
        } 
      
        // We were not able to make a valid combination after picking each element from the array,
        // hence we can't make k subsets.
        return false;
    }
    
    bool canPartitionKSubsets(vector<int>& arr, int k) {
       totSets = k;
       nElems = arr.size();
       int totalArraySum= accumulate(begin(arr), end(arr), 0);

      
        // If total sum not divisible by k, we can't make subsets.
        if (totalArraySum % totSets != 0) { 
            return false;
        }
        
        // Sort in decreasing order.
        sort(arr.begin(), arr.end(), greater<int>());
      
        bucket = totalArraySum / totSets;
        taken = vector<bool>(nElems, false);
      
        state.clear();
        bool ret = backtrack(arr, 0, 0,0);
        //printSolution();
        return ret;
    }
    
    int nElems;
    int numSets;
    int totSets;
    int bucket;
    vector<bool> taken;
    vector<int> state;
    vector<vector<int>> sols;
    
};