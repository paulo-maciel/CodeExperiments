#include <vector>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <queue>
#include <map>
using namespace std;

class Graph {
    public:

        Graph(int n) {
          this->n = n;
          adj = new vector<int>[n];
          visited = new bool[n];
          for(int i = 0; i < n; ++i) {
            visited[i] = false;
          }
        }
    
        void add_edge(int u, int v) {
          adj[u].push_back(v);
          adj[v].push_back(u);            
        }
    
        vector<int> shortest_reach(int start) {
          vector<int> sol;
          sol.reserve(n);
          queue<int> Q;
          Q.push(start);
          
          for(int i = 0; i < n; ++i) nodeParent[i] = -1;
          for(int i = 0; i < n; ++i) sol[i] = 0;
          
          while(!Q.empty()) {
            int parent = Q.front();
            Q.pop();
            visited[parent] = true;
            for(auto child : adj[parent]) {
              if (!visited[child]) {
                visited[child] = true;
                nodeParent[child] = parent;
                Q.push(child);
              }
            }
          }

          // iterate through all nodes.
          for(int node = 0; node < n; ++node) {
            if (nodeParent[node] == -1) {
              sol[node] = -1;
            } else {
              int parent = node;
              while(parent != start && parent != -1) {
                sol[node] += 6;
                parent = nodeParent[parent];
                if (parent == node) {
                  break;
                }
              }
            }
          }
          
          for (int i = 0; i < n; ++i) {
            if (i == start) continue;
            cout << sol[i] << " ";
          }
          
          return sol;  
        }
    int n;
    vector<int>* adj;
    bool* visited;
    map<int, int> nodeParent;
};

int main() {
    int queries;
    cin >> queries;
        
    for (int t = 0; t < queries; t++) {
      
		int n, m;
        cin >> n;
        // Create a graph of size n where each edge weight is 6: 
        Graph graph(n);
        cin >> m;
        // read and set edges
        for (int i = 0; i < m; i++) {
            int u, v;
            cin >> u >> v;
            u--, v--;
            // add each edge to the graph
            graph.add_edge(u, v);
        }
		int startId;
        cin >> startId;
        startId--;
        // Find shortest reach from node s
        vector<int> distances = graph.shortest_reach(startId);

        for (int i = 0; i < distances.size(); i++) {
            if (i != startId) {
                cout << distances[i] << " ";
            }
        }
        cout << endl;
    }
    
    return 0;
}
