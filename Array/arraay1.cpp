#include <bits/stdc++.h>

#include <queue>

using namespace std;

vector<string> split_string(string);

// Complete the arrayManipulation function below.
long arrayManipulation(int n, vector<vector<int>> queries) {
  auto ar = new long[n];
  for(int i = 0; i < n; ++i)
    ar[i] = 0;
    
  // if there are no intersections, return the highest value.
  bool disjoint = true;
  auto visited = new long[n];
  long currVal = 0;
  for(int i = 0; i < n; ++i)
    visited[i] = false;

  for( auto q : queries) {
    for(long i = q[0] - 1; i < q[1]; ++i) {
      if (visited[i]) {
        disjoint = false;
        break;
      } else {
        visited[i] = true;
        currVal = q[2];
      }
    }
  }
  if (disjoint) return currVal;
  
  long tmax = 0;
  for( auto q : queries) {
    for(long i = q[0] - 1; i < q[1]; ++i) {
      ar[i] += q[2];
      tmax = max(tmax, ar[i]);
    }
  }
    
  return tmax; 
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    string nm_temp;
    getline(cin, nm_temp);

    vector<string> nm = split_string(nm_temp);

    int n = stoi(nm[0]);

    int m = stoi(nm[1]);

    vector<vector<int>> queries(m);
    for (int i = 0; i < m; i++) {
        queries[i].resize(3);

        for (int j = 0; j < 3; j++) {
            cin >> queries[i][j];
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    long result = arrayManipulation(n, queries);

    fout << result << "\n";

    fout.close();

    return 0;
}

vector<string> split_string(string input_string) {
    string::iterator new_end = unique(input_string.begin(), input_string.end(), [] (const char &x, const char &y) {
        return x == y and x == ' ';
    });

    input_string.erase(new_end, input_string.end());

    while (input_string[input_string.length() - 1] == ' ') {
        input_string.pop_back();
    }

    vector<string> splits;
    char delimiter = ' ';

    size_t i = 0;
    size_t pos = input_string.find(delimiter);

    while (pos != string::npos) {
        splits.push_back(input_string.substr(i, pos - i));

        i = pos + 1;
        pos = input_string.find(delimiter, i);
    }

    splits.push_back(input_string.substr(i, min(pos, input_string.length()) - i + 1));

    return splits;
}

