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

// for pop_heap
#include <algorithm>

I am Lev.
  Q: Write a test that is guaranteed to deadlock.
    
   using anmespace std;
   int test() {
    // Open file descriptor fd1, fd2;
    thread t1(func(true));
    thread t2(func(false));
     
    t1.join();
    t2.join();
  }
    
    void func(boot cond) {
       static mutex mtx1;
       static mutex mtx2;
       condition_variable v;
       unique_lock<std::mutex> lk(m);
       bool flag;

       if (cond) {
         mtx1.lock();
          // Needs to signal locked t1
          v.notify_one();
         
          // Needs to wait until t2 executes lock2 
          v.wait(lk, []{ return flag });

         // work
         mtx2.lock();
       } else {
         // Need to signal thread 1 that it has locked lock2.
          mtx2.lock();
          v.notify_one();
         
         // Needs to wait until t1 executes lock1. 
         v.waitlx, []{ return flag });
         
         // work
         mtx1.lock();        
       }
          
        //mtx1.unlock();
        //mtx2.unlock();
       }
  }
    
// Shuffle need to be random.
// No value in its original position.
int seedVal = seed(time(NULL));

int getRandomPos(int range) {
  return (rand()/MAX_INT)) * range;
           // between 0 and one
}

// uniform_distribution

void shuffle(int* array, int len) {
  // Random number generator that gives me a position between 0, len -1;
  // Remove two position and make sure they are different.
  // while we have positions, we swap the two element
  set<int> usedPos;
  int count = len;
  int pos1, pos2;
  while(count > 1) {
    
     // While there are positions available
     //  get pos 1
     //  get pos 2
     //  if (pos 1 != pos2) {
     //     break;
    while(true) {
      pos1 = getRandomPos();
      if (!usedPos.count(pos1)) {
        break;
      }
    }
    while(true) {
      pos2 = getRandomPos();
      if (!usedPos.count(pos2)) {
        break;
      }
    }
    int pos2 = getRandomPos();
    if (pos1 != pos2) {
      swap(arr[pos1], arr[pos2]); 
      usedPos.insert(pos1);
      usdePos.insert(pos2);
      count -= 2;
    }
  } 
  
  if (count == 1) {
    int randPos = getRandomPos();
    int posNotInSet = 0;
    // Set with all position.
    int i = 0;
    for(; i < len; ++i) {
      if (!usedPos.count(i)) {
        while (randPos == i) {
          randPos = getRandomPos();
        }
        break;
      }
    }
    swap(arr[randPos], arr[i]);
  }

}

void swap(int& a, int& b) {
     int temp = b;
     b = a;
     a = temp;
}
      

void shuffle(arr, beg, end) {
  int mid = (beg + end)/2;
  
  shuffle(arr, beg, mid - 1, mid + 1, end);
  
}
# tiler: argument is a vbo (array of triangles)

divide the screen into tiles (width, height)
  for each tile (x, y)
     for each triangle
        if triangle in tile
           add triangle to tile list
           draw triangles.
           clip triangle edge agains the tile
              Send the 3 vertices to throught the pipeline
              or
              Redivide the a parallelogram into 3 triangles
                 for each triangle
                     send 3 vertices through the pipe 
             
          

Porter-Duff compositing

dst = src * alpha + dst * (1 - alpha)

blit:

dst = src





using namespace std;

int main() {

  return 0;
}
