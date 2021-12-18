class Solution {
public:
  // dpSwap contains the value after swaping i and j, if set.  -INT_MAX otherwise.
  vector<vector<int>> dpSwap;
  int maxNum = -INT_MAX;
  int maximumSwap(int num) {
    maxNum = num;
    auto numStr = to_string(num);
    int numDigits = numStr.length();
    if (numDigits == 1) {
      return num;
    }
    dpSwap = vector<vector<int>>(numDigits, vector<int>(numDigits, -INT_MAX));
    
    for(int i = 0; i < numDigits; ++i) {
      for(int j = i + 1; j < numDigits; ++j) {
        if (dpSwap[i][j] == -INT_MAX) {
          auto temp = numStr;
          swap(temp[i], temp[j]);
          dpSwap[i][j] = atoi(temp.c_str());
        }
        maxNum = max(maxNum, dpSwap[i][j]);
      }
    }
    
    return maxNum;
  }
};