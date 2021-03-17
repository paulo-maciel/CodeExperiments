#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using namespace std;

int** T;

// Complete the abbreviation function below.
string abbreviation(string a, string b) {
  long n = a.size();
  long m = b.size();

  T = new int*[n];
  for(int i = 0; i < n; ++i) {
    T[i] = new int[m];
  }

  for(int i = 0; i < n; ++i) {
    for(int j = 0; j < m; ++j) {
      T[i][j] = 0;
      if (i == 0) {
        if (j == 0) {
          if (a[i] == b[j] || toupper(a[i]) == b[j]) {
            T[i][j] = 1;
         }
        }
      } else {
        if (j == 0 && (a[i] == b[j] || toupper(a[i]) == b[j] || islower(a[i]))) {
          T[i][j] = 1;
        }
      }
    }
  }

  for(int i = 1; i < n; ++i) {
    for(int j = 1; j < m; ++j) {
      if (isupper(a[i])) {
        if (a[i] == b[j] && T[i - 1][j - 1]) {
          T[i][j] = 1;

        }
      } else {
        // a[i] is lower.
        if (T[i - 1][j] ||
            (toupper(a[i]) == b[j] && T[i-1][j-1]) ) {
          T[i][j] = 1;
        }
      }
    }
  }

  return T[n - 1][m - 1] == 0 ? "NO" : "YES";
}

int main()
{
  string a = "Pi";
  string b = "P";

  cout << abbreviation(a, b) << endl;

  cout << endl;

  long n = a.size();
  long m = b.size();

  for(int j = 0; j < m; ++j) {
    for(int i = 0; i < n; ++i) {
       cout << T[i][j] << " ";
    }
    cout << endl;
  }


  return 1;
}