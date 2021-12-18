class Solution {
public:
//     map<int,int> dp;
//     int tribonacci(int n) {
//         if (n == 0) return 0;
//         if (n == 1 || n == 2) return 1;
        
//         if (dp.count(n) == 0) {
//             dp[n] = tribonacci(n-3) + tribonacci(n - 2) + tribonacci(n - 1);
//         }
        
//         return dp[n];
//     }
    
//     map<int,int> dp;
//     int tribonacci(int n) {
//         int tri = 0;
//         if (n == 0) return 0;
//         if (n <= 2) return 1;
//         dp[0] = 0;
//         dp[1] = 1;
//         dp[2] = 1;
        
//         for(int i = 3; i <= n; ++i) {
//             dp[i] = dp[i-3] + dp[i-2] + dp[i-1];
//         }
        
//         return dp[n];
//     }
    
    
    int tribonacci(int n) {
        int tri = 0;
        if (n == 0) return 0;
        if (n <= 2) return 1;
        int temp0 = 0;
        int temp1 = 1;
        int temp2 = 1;
        
        int tot = 0;
        for(int i = 3; i <= n; ++i) {
            tot = temp0 + temp1 + temp2;
            temp0 = temp1;
            temp1 = temp2;
            temp2 = tot;
        }
        
        return tot;
    }
};