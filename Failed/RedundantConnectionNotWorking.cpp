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

class Tree {
  public:
    void build(int node, vector<vector<int>>& edges) {
        root = node;
        for(auto edge : edges) {
            addEdge(edge);
        }
    }
    
    // Try to add the edge to a descendant of node.
    // If it doesn't close the tree, return true, else false
    bool tryAddEdge(int node, const vector<int>& edge) {
        auto children = graph[node];
        bool status = false;
        
        // Check for violation.
        // That is, found one extremety and the other one is alreay part
        // of the graph.
        if (edge[0] == node && seenNodes.count(edge[1]) ||
            edge[1] == node && seenNodes.count(edge[0]) ) {
            cout << "Edge violates: " << edge[0] << " -> " << edge[1] << endl;
            return true;
        }
        
        for(auto child : children) {
          status |= tryAddEdge(child, edge);
        }
        
        return status;
    }

    
    void addEdge(const vector<int>& edge) {
        // Need to add edge but maintain a tree
        // if the edge does not maintain the tree
        // store that in a stack
        
        // We only want to add this
        bool violatesTree = tryAddEdge(root, edge);
        if (violatesTree) {
            cout << "Pushing edge." << endl;
            closeEdges.push(edge);
        } else {
            // Only add if it does not violate the tree property.
            graph[edge[0]].push_back(edge[1]);
            cout << "Adding edge: " << edge[0] << " -> " << edge[1] << endl;
        }

        seenNodes.insert(edge[0]);
        seenNodes.insert(edge[1]);
    }
            
    vector<int> getLastEdge() {
        vector<int> out;
        if (closeEdges.empty()) {
            return out;
        }
        
        return closeEdges.top();
    }
    
  private:
    unordered_map<int, list<int>> graph;
    stack<vector<int>> closeEdges;
    set<int> seenNodes;
    int root;
};

vector<int> findRedundantConnection(vector<vector<int>>& edges) {

    Tree tree;
    tree.build(edges[0][0], edges);
    
    // For each edge, place it in a depth first search such that
    // the origin of the edge attaches to one of the existing nodes.
    // if the other end of the edge connects to an already existing node
    // record that fact.
    // At the end, return the most recently added edge that close the tree.
    
    vector<int> out;
    out = tree.getLastEdge();;
    return out;
}

//           Tree
// 1 -2      1 - 2
// 2 - 3     1 - 2 - 3 
// 3 - 4     1 - 2 - 3 - 4
// 1 - 4     1 - 2 - 3 - 4 - 1 => close
// 1 - 5

int main() {

  //vector<vector<int>> edges = {{1,2},{1,3},{2,3}};
  //vector<vector<int>> edges = {{1,2},{2,3},{3,4},{1,4},{1,5}};
  vector<vector<int>> edges = {{4,5},{1,2},{2,4},{3,4},{2,3}};

  vector<int> edge = findRedundantConnection(edges);

  if (!edge.empty()) {
    cout << edge[0] << " -> " << edge[1] << endl;
  }

  return 0;
}