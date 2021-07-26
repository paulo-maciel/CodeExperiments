#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <queue>
#include <deque>
#include <list>
#include <stack>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <tuple>
#include <map>

// for pop_heap
#include <algorithm>

using namespace std;

// price[day] = [7,1,5,3]
// return maximum profit
// Need to choose a day to buy and a day afterwards to sell.
// maximize sell[dayX] - buy[dayY], where dayY > dayX
int maxProfit(vector<int>& prices) {
    // Brute force is to keep a running maximum  O(n**2)
    int maxPrice = 0;
    // What if i put on an array the best days to sell, highest price.
    // sell[price] = day
    // 7 1
    // 5 3
    // 3 4
    // 1 2
    // map[price, day] with the higest prices firs
    // put on another array the best days to sell, higher price.
    // buy[price, day] with the lowest prices first.
    // 1 2
    // 3 4
    // 5 3
    // 7 1
    // keep a running max
    // iterate through buy map
    //    get buy day.
    //    iterate thorough sell map
              // if max sell day is before buy day
              //    continue
              // maxProfit = max(maxProfit, sellPrice);
    
    map<int, int, std::greater<int>> sellMap;  // sorted in decreasing order.
    map<int, int, std::less<int>> buyMap;    // sorted in ascending order
    for(auto i = 0; i < prices.size(); ++i) {
        sellMap[prices[i]] = i;
        buyMap[prices[i]] = i;
    }
    cout << "Buy map: " << endl;
    for(auto kvp : buyMap) {
      cout << kvp.first << "," << kvp.second << endl;
    }
    cout << "Sell map: " << endl;
    for(auto kvp : sellMap) {
      cout << kvp.first << "," << kvp.second << endl;
    }
    for(auto kvp : buyMap) {
        cout << kvp.first << "," << kvp.second << endl;
    }
    
    for(auto kvp : buyMap) {
        auto buyDay = kvp.second;
        for (auto pvk : sellMap) {
            if (pvk.second <= buyDay ) {
                continue;
            } else {
                maxPrice = max(maxPrice, pvk.first - kvp.first);
                if (maxPrice < 0) {
                    maxPrice = 0;
                }
                //cout << "Max price: " <<  maxPrice << " Buy day: " << buyDay << " Sell day: " << pvk.second << endl;
            }
        }
    }
    
    return maxPrice;
}

int main() {

  //vector<int> prices = {7,1,5,3,6,4};
  vector<int> prices = {1,4,1,4,3,1};
  cout << "maxPrice = " << maxProfit(prices) << endl;
  return 0;
}
