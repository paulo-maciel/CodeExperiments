class Solution {
public:
    string addStrings(string num1, string num2) {
        int i = num1.length() - 1;
        int j = num2.length() - 1;
        int carryOver = 0;
        int k = max(i+1,j+1);
        string added;
        added.resize(k);
        while(i >= 0 || j>= 0) {
            int val1 = i >= 0 ? num1[i] - 48 : 0;
            int val2 = j >= 0 ? num2[j] - 48 : 0;
            int valSum = val1 + val2 + carryOver;

            k = max(i,j);
            if (valSum <= 9) {
                carryOver = 0;
            } 
            else {
                // valSum >= 10   valSum <= 18
                carryOver = 1;
                valSum -= 10;
            }
            //cout << "k: " << k << endl;
            added[k] = 48 + valSum;
            --i;
            --j;
        }
        
        if (carryOver) {
            added = '1' + added;
        }
        return added;
    }
};