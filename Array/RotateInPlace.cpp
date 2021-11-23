class Solution
{
public:
  void rotate(vector<int> &nums, int k)
  {
    int dest = 0;
    if (nums.size() < 2)
      return;
    if (nums.size() < k)
    {
      k = k % nums.size();
    }
    while (k > 0)
    {
      int moveIdx = nums.size() - k;
      while (moveIdx != dest)
      {
        // int tmp = nums[moveIdx];
        // nums[moveIdx] = nums[moveIdx-1];
        // nums[moveIdx-1] = tmp;
        std::swap(nums[moveIdx], nums[moveIdx - 1]);
        --moveIdx;
      }
      dest++;
      k--;
    }
  }
};

// Accepted and performant.
class Solution
{
public:
  void swapArray(vector<int> &nums, int k)
  {
    vector<int> tmp;
    tmp.reserve(nums.size());

    memcpy(&tmp[0], &nums[0], nums.size() * sizeof(int));

    memcpy(&nums[0], &tmp[nums.size() - k], k * sizeof(int));

    memcpy(&nums[k], &tmp[0], (nums.size() - k) * sizeof(int));
  }

  void rotate(vector<int> &nums, int k)
  {
    int dest = 0;
    if (nums.size() < 2)
      return;
    if (nums.size() < k)
    {
      k = k % nums.size();
    }
    swapArray(nums, k);
  }
};