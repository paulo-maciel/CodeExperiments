#include <bits/stdc++.h>
#include <numeric>
#include <algorithm>
#include <limits>

using namespace std;

vector<string> split_string(string);


// Complete the maxSubsetSum function below.
int maxSubsetSum(vector<int> ar) {
    vector<int> maxArray;
    maxArray.reserve(ar.size());
    fill(maxArray.begin(), maxArray.end(), -INT_MAX);
    
    maxArray[0] = ar[0];
    maxArray[1] = max(ar[0], ar[1]);
    
    int totalMax = -INT_MAX;
    for(auto i = 2ul; i < ar.size(); i++) {
        totalMax = max(ar[i] + maxArray[i - 2], max(maxArray[i - 1], ar[i]));
        maxArray[i] = totalMax;
    }
    
    return totalMax;
}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    int n;
    cin >> n;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string arr_temp_temp;
    getline(cin, arr_temp_temp);

    vector<string> arr_temp = split_string(arr_temp_temp);

    vector<int> arr(n);

    for (int i = 0; i < n; i++) {
        int arr_item = stoi(arr_temp[i]);

        arr[i] = arr_item;
    }

    int res = maxSubsetSum(arr);

    fout << res << "\n";

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

