void print(vector<vector<char>> result){
  for(int i = 0; i < result.size(); i++){
    cout << "[ ";
    for(int j = 0; j < result[i].size(); j++){
      cout << result[i][j] << ", "; 
    }
    cout << "]" << endl;
  }
}

void printAllBracesRec(
    int n,
    int left_count,
    int right_count,
    vector<char>& output, vector<vector<char>>& result) {

  if (left_count == n && right_count == n) {
    result.push_back(output);
  }

  if (left_count < n) {
    output.push_back('{');
    printAllBracesRec(n, left_count + 1, right_count, output, result);
    output.pop_back();
  }

  if (right_count < left_count) {
    output.push_back('}');
    printAllBracesRec(n, left_count, right_count + 1, output, result);
    output.pop_back();
  }
}

vector<vector<char>> printAllBraces(int n) {
  vector<vector<char>> result;
  vector<char> output;
  printAllBracesRec(n, 0, 0, output, result);
  return result;
}

int main() {
  vector<vector<char>> result = printAllBraces(3);
  print(result);
}