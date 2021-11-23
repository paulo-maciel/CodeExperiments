class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
        vector<int> digitsPlusOne;
        int i = digits.size() - 1;
        bool carryOver = false;
        while(i >= 0) {
            if (i == digits.size() - 1) {
                if (digits[i] + 1 > 9) {
                    carryOver = true;
                    digitsPlusOne.insert(digitsPlusOne.begin(), 0);
                } else {
                    digitsPlusOne.insert(digitsPlusOne.begin(), digits[i] + 1);
                }
            } else {
                if (!carryOver) {
                    digitsPlusOne.insert(digitsPlusOne.begin(), digits[i]);
                } else {
                    if (digits[i] + 1 <= 9) {
                        digitsPlusOne.insert(digitsPlusOne.begin(), digits[i] + 1);
                        carryOver = false;
                    } else {
                        digitsPlusOne.insert(digitsPlusOne.begin(), 0);
                    }
                }
            }
            --i;
        };
        
        if (carryOver) {
            digitsPlusOne.insert(digitsPlusOne.begin(), 1);
        }
        
        return digitsPlusOne;
    }
};

// Correct optimal Solution
class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
    int digit =digits.size() -1;
        while(digit>=0){
            if(digits[digit]==9){
                digits[digit]=0;
            }else{
                digits[digit] +=1;
                return digits;
            }
           digit--;
        }
        digits.insert(digits.begin(),1);
        return digits;
    }
};

/*three cases ..
4221   --> if last digit is less then 9 so add 1 to it and  return then .


539   --> if last digit is 9 so place zero [0] to it  and   move on index--  check if 9 so     again index-- or if not so add 1 one their.
   
   
999 --> in the begning add one(1) and make all zeros.

*/