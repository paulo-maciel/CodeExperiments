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

// for pop_heap
#include <algorithm>

using namespace std;

class Graph {
  public:
    void build(vector<vector<int>>& edges) {
        for(auto edge : edges) {
            addEdge(edge);
        }
    }
    
    void addEdge(const vector<int>& edge) {
      graph[edge[0]].push_back(edge[1]);
      graph[edge[1]].push_back(edge[0]);      
    }

    bool formsCycle(int parent, int from, int to) {
      bool status = false;
      if (from == to) {
        cout << "Returning Found the cycle." << endl;
        return true;
      }

      cout << endl;
      auto children = graph[from];
      for (auto child : children) {
        cout << "parent: " << parent << " from: " << from << " child: " << child << " to: " << to << endl;
        // Avoid going back to the same node.
        if (parent != child) {
          status |= formsCycle(from, child, to);
        }
        if (status) {
          break;
        }
      }
      cout << "Done children!" << endl;

      return status;
    }
            
  private:
    unordered_map<int, list<int>> graph;
};

vector<int> findRedundantConnection(vector<vector<int>>& edges) {

    Graph graph;
    graph.build(edges);

    vector<int> lastEdge;
    
    // Return the last edge that closed the cycle.
    // for(auto edge : edges) {
    //   if (graph.formsCycle(-1,edge[0], edge[1])) {
    //     lastEdge = edge;
    //   }
    // }

    if (graph.formsCycle(-1, 1, 4)) {
      lastEdge[0] = 1;
      lastEdge[1] = 4;
    }

    return lastEdge;
}


int main() {

  //vector<vector<int>> edges = {{1,2},{1,3},{2,3}};
  vector<vector<int>> edges = {{1,2},{2,3},{3,4},{1,4},{1,5}};
  //vector<vector<int>> edges = {{4,5},{1,2},{2,4},{3,4},{2,3}};

  vector<int> edge = findRedundantConnection(edges);

  if (!edge.empty()) {
    cout << edge[0] << " -> " << edge[1] << endl;
  }

  return 0;
}

// 1 2 3 4
// adding 1 4
// fc(-1, 1, 4)
//   child(1) = 2 4 5
//      fc(1, 2, 4 )
//         child(2) = 1 3
//         fc (2, 3, 4 );
//           child(3) = 2 4
//           fc(3, 4, 4)
//             child(4) = 3 1
//             fc(4, 1, 4 )
//                child(1) = 2 4 5

//      fc(1, 4, 4)

//      fc(1 , 5, 4)