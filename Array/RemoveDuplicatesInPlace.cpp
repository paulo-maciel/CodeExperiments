class Solution
{
public:
  int removeDuplicates(vector<int> &nums)
  {
    int next = 1;
    int k = 0;

    if (nums.empty())
      return 0;
    if (nums.size() == 1)
      return 1;

    while (next < nums.size())
    {
      if (nums[k] != nums[next])
      {
        ++k;
        nums[k] = nums[next];
      }
      ++next;
    }
    ++k;
    nums.resize(k);
    return k;
  }
};