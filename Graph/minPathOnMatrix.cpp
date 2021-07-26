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

using namespace std;

class Graph {
  public:
  Graph() {}

  void addEdge(int i, int j) {
    adj[i].push_back(j);
  }

  void build(int grid[4][4], int n, int m) {
    // Build a map of the matrix edges.
    unordered_map<int, tuple<int,int>> matrixElemIdx;
    for(int i = 0; i < 4; ++i) {
      for(int j = 0; j < 4; ++j) {
        matrixElemIdx[grid[i][j]] = make_tuple(i,j);
      }
    }

    for(auto i = 0; i < 4; ++i) {
      for(auto j = 0; j < 4; ++j) {
        tuple<int,int> pos = matrixElemIdx[grid[i][j]];
        if (get<0>(pos)+1 < 4) {
          addEdge(grid[i][j], grid[get<0>(pos)+1][j]);
        }
        if (get<1>(pos)+1 < 4) {
          addEdge(grid[i][j], grid[i][get<1>(pos)+1]);
        }

        if (i == 3 && j == 3) {
          // end node.
          endNode = grid[i][j];
        }
      }
    }
  }

  int getMin(int root, stack<int>& s) {
    s.push(root);
    if (root == endNode) {
      // print the stack and calculate the min of the path and compare with min.
      stack<int> sdup = s;
      cout << "Path: ";
      int cost = 0;
      while(!sdup.empty()) {
        cost += sdup.top();
        cout << sdup.top() << ",";
        sdup.pop();
      }
      cout << " Total: " << cost << endl;
      s.pop();

      minPath = min(cost, minPath);
      cout << "Returning: " << minPath << endl;
      return minPath;
    }

    auto children = adj[root];
    for(auto child : children) {
      getMin(child, s);
    }
    s.pop();

    return minPath;
  }

private:
  unordered_map<int, list<int>> adj;
  int endNode;
  int minPath = numeric_limits<int>::max();
};

int main() {
  int grid[4][4] = {
      {1, 2, 3, 4},
      {12,14,15,16},
      {10,22,32,34},
      {9, 8, 7, 5},
  };

  Graph g;
  g.build(grid, 4, 4);
  stack<int> s;

  cout << "Min: " << g.getMin(1, s) << endl;

  return 0;
}