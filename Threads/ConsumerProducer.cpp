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
#include <ostream>
#include <iterator>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <bits/stdc++.h>
#include <time.h>

// for pop_heap
#include <algorithm>

//import helloworld;

using namespace std;
 
mutex mtx;
condition_variable cv;
string data;
bool produced = false;
bool consumed = false;

queue<string> q;

void producer(int totItems, string str) {
  int n = 0;
  while(n < totItems) {
    {
      lock_guard<mutex> lock(mtx);
      cout << "Produced item: " <<  n << endl;
      auto tmp = str + to_string(n);
      q.push(tmp);
      ++n;
      produced = true;
      cv.notify_one();
    }

    {
      // Wait for a consumed signal to produce another.
      unique_lock<mutex> lock(mtx);
      cv.wait(lock, []() {
        return consumed;
      });
      consumed = false;

    }
  }

  lock_guard<mutex> lock(mtx);
  q.push("end");
  produced = true;
  cv.notify_one();

  cout << "Exiting producer and signalling end of production." << endl;
}

// Comsumer Code
int main()
{
  thread th(producer, 10, "produced item " );

  bool terminate = false;
  while(!terminate) {
    {
      {
        // Wait for a produced item.
        unique_lock<mutex> lock(mtx);

        cv.wait(lock, []() {
          return produced;
        });
        
        auto producedItem = q.front();
        q.pop();
        cout << "Consuming item: " << producedItem << endl;
        produced = false;

        if (producedItem == "end") {
          terminate = true;
          cout << "Received end";
          break;
        }
      }

      {
        // signal consumed.
        lock_guard<mutex> lock(mtx);
        consumed = true;
        cv.notify_one();
      }
    }
  }

  cout << "Joining thread and exitin" << endl;
  th.join();

	return 0;
}

// This is code is contributed by rathbhupendra
