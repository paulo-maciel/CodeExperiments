/**
 * // This is the robot's control interface.
 * // You should not implement it, or speculate about its implementation
 * class Robot {
 *   public:
 *     // Returns true if the cell in front is open and robot moves into the cell.
 *     // Returns false if the cell in front is blocked and robot stays in the current cell.
 *     bool move();
 *
 *     // Robot will stay in the same cell after calling turnLeft/turnRight.
 *     // Each turn will be 90 degrees.
 *     void turnLeft();
 *     void turnRight();
 *
 *     // Clean the current cell.
 *     void clean();
 * };
 */

class Solution
{
public:
  using Pos = array<int, 2>;
  set<Pos> visited_;
  Robot *robot_;

  // Go back up one position but keeping the same direction.
  void backUp()
  {
    robot_->turnRight();
    robot_->turnRight();
    robot_->move();
    robot_->turnRight();
    robot_->turnRight();
  }
  void visit(int row, int col, int d)
  {
    // left, up, right, down (clockwise)
    int dirs[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    visited_.insert({row, col});
    robot_->clean();

    // going clockwise : 0: 'up', 1: 'right', 2: 'down', 3: 'left'
    // The idea is to choose a given direction (from the given one) and explore it until can no longer move, and then
    // backtrack.
    for (int i = 0; i < 4; ++i)
    {
      int newD = (d + i) % 4;
      int newRow = row + dirs[newD][0];
      int newCol = col + dirs[newD][1];

      if (!visited_.count({newRow, newCol}) && robot_->move())
      {
        // Keep visiting until need to backup.  Note the recursive call 'visit'
        visit(newRow, newCol, newD);
        backUp();
      }

      // turn the robot following chosen direction : clockwise
      robot_->turnRight();
    }
  }
  void cleanRoom(Robot &robot)
  {
    robot_ = &robot;
    visit(0, 0, 1);
  }
};