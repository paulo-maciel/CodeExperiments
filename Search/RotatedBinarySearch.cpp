class Solution
{
public:
  int binarySearch(vector<int> &nums, int low, int high, int target)
  {
    int med = 0;
    while (low <= high)
    {
      med = low + floor((high - low) / 2.0);
      if (nums[med] == target)
      {
        return med;
      }

      if (target > nums[med])
      {
        low = med + 1;
      }

      if (target < nums[med])
      {
        high = med - 1;
      }
    }

    return -1;
  }

  int search(vector<int> &nums, int target)
  {
    int low = 0;
    int high = nums.size() - 1;
    int med = 0;

    // Find the pivot
    int pivot = 0;
    while (nums[low] > nums[high])
    {
      med = low + floor((high - low) / 2.0);
      if (med == low)
      {
        pivot = med;
        break;
      }
      // adjust low
      if (nums[med] > nums[high])
      {
        low = med;
      }
      else if (nums[med] < nums[low])
      {
        high = med;
      }
      else
      {
        // array is sorted
        break;
      }
    }
    if (target <= nums[pivot] && target >= nums[0])
    {
      return binarySearch(nums, 0, pivot, target);
    }
    else
    {
      return binarySearch(nums, pivot + 1, nums.size() - 1, target);
    }
  }
};