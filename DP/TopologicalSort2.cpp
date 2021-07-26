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

using namespace std;

class Graph {
    public:
        
    Graph(int n) 
    : numNodes(n) {
        adj = new list<int>[n];
    }

    void addEdge(int p, int c) {
        adj[p].push_back(c);
    }

    void Build(vector<vector<int>>& prerequisites) {
        for(auto req : prerequisites) {
            addEdge(req[1], req[0]);
        }
    }

    void DFS(int node, bool *visited, stack<int>& s) {
        visited[node] = true;
        for(auto n : adj[node]) {
            if (!visited[n]) {
                DFS(n, visited, s);
            } 
        }
        s.push(node);
    }

    private:
        int numNodes;
        list<int> *adj;
    };

    vector<int> topologicalOrder(int numCourses, vector<vector<int>>& prerequisites) {
        vector<int> order;
        Graph g(numCourses);
        g.Build(prerequisites);

        bool *visited = new bool[numCourses];
        for(int i = 0; i < numCourses; ++i) {
          visited[i] = false;
        }

        stack<int> s;
        for(int i = 0; i < numCourses; ++i) {
            if (visited[i] == false) {
                g.DFS(i, visited, s);
            }
        }

        while(!s.empty()) {
            order.push_back(s.top());
            s.pop();
        }
        return order;
    }

int main() {
    // vector<vector<int>> reqs = {{1,0},{2,0},{3,1},{3,2},{2,4}, {0,3}};
    // auto vec = topologicalOrder(5, reqs);
    vector<vector<int>> reqs = {{1,0},{0,1}};
    auto vec = topologicalOrder(2, reqs);

    for (int i = 0; i < 2; ++i) {
        cout << " " << vec[i];
    }
    cout << endl;

    return 1;
}