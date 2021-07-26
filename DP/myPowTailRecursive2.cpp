#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <iostream>
#include <functional>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>

using namespace std;

std::vector<double> pw;
double myPow2(double x, int n) {
    if (n == 0) {
        pw[0] = 1.0;
        return 1.0;
    }
    
    if (pw.size() == 0) {
        pw.resize(n + 1);
        std::fill(pw.begin(), pw.end(), -1);
    }

    if (pw[n] == -1) {
        myPow2(x,n-1);
    } 

    pw[n] = x * pw[n - 1];
    return pw[n];
}

double myPowTR(double x, int n, double acc) {
    if (n == 0) {
        return acc;
    }

    if (pw.size() == 0) {
        pw.resize(n + 1);
        std::fill(pw.begin(), pw.end(), -1);
    }
    if (pw[n] == -1) {
        pw[n] = myPowTR(x, n - 1, x*acc);
    }
    
    return pw[n];
}

double myPow(double x, int n) {
    return myPowTR(x, n, 1.0);
}
int main() {

    for (auto i = 1; i < 100; ++i) {
        pw.clear();
        cout << "myPow(" << i << "): " <<  myPow(3, i) << endl;
    }

  return 1;
}