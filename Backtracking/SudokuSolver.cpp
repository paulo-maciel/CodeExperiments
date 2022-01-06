// In pseudocode, our strategy is:
  // Find row, col of an unassigned cell
  // If there is none, return true
  //
  // For digits from 1 to 9
    // if there is no conflict for digit at row,col
    // assign digit to row,col and recursively try fill in rest of grid
    // if recursion successful, return true
    // if !successful, remove digit and try another
  // if all digits have been tried and nothing worked, return false to trigger backtracking