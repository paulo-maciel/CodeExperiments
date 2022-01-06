// Permute strings
void permuteHelper(vector<string>& stringSet, vector<string>& chosen) {
  if (stringSet.empty()) {
    auto pr = [](string str) { cout << str << " "; };
    cout << "Choice: "; 
    for(auto choice : chosen) {
      pr(choice);
    }
    cout << endl;
    //cout << chosen << endl;
    // base case
  } else {
    // At each recursion, select and element from stringSet (or masterSet) and add it to the
    // recursion again.
    // Steps:
    // For all choices
    //   Select a choice and remove it from the master list.
    //   Add choice
    //   Recur
    //   Remove choice and add it back to the master list.
    for (auto it = begin(stringSet); it != end(stringSet); ++it) {
      string s = *it;
      // Remove from the set of characters.
      it = stringSet.erase(it);

      // choose
      chosen.push_back(s);

      // explore
      permuteHelper(stringSet, chosen);
      
      // un-choose
      chosen.pop_back();

      it = stringSet.insert(it, s);
    }
  }
}

// Outputs all permutations of the given vector.
void permute(vector<string>& stringSet) {
  vector<string> chosen;
  permuteHelper(stringSet, chosen);
}

void listSubset(set<string>& masterSet, set<string>& used) {
  if (masterSet.empty()) {
    auto pr = [](string str) { cout << str << " "; };
    cout << "Used: "; 
    for(auto us : used) {
      pr(us);
    }
    cout << endl;
  } else {
    // At each recursion, remove one element from master and add it to 'used'.
    // Steps:
    // Remove element from master
    // Recurr (without the element)
    // Add element to used
    // Recurr (with element)
    // Remove element from used.
    // Add element back to master.
    auto eIter = masterSet.begin();
    auto elem = *eIter;
    masterSet.erase(eIter);

    // Without the element.
    listSubset(masterSet, used);

    // With the element.
    used.insert(elem);
    listSubset(masterSet, used); 
    masterSet.insert(elem);
    used.erase(elem);
  }
}

void subSets(set<string>& masterSet) {
  set<string> chosen;
  listSubset(masterSet, chosen);
}

// Permute characters.
// Call RecPermute("abcd", "");
void RecPermute(string rest, string used) {
  if (rest.empty()) {
    cout << used << endl;
  } else {
  for (int i = 0; i < rest.length(); i++) {
    // At each iteration, select a character to add.
    // That is, at each iteration we have:
    // prefix ch1
    // prefix ch2
    // ...
    // prefix chn
    // Then, the same applies to 'prefix'
    string remaining = rest.substr(0, i) + rest.substr(i+1);
    RecPermute(remaining, used + rest[i]);
  }
 }
}