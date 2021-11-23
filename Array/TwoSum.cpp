class Solution
{
public:
  vector<int> twoSum(vector<int> &nums, int target)
  {
    vector<int> retVec;
    retVec.resize(2);

#ifdef BRUTE
    for (int i = 0; i < nums.size() - 1; ++i)
    {
      for (int j = i + 1; j < nums.size(); ++j)
      {
        if (nums[i] + nums[j] == target)
        {
          retVec[0] = i;
          retVec[1] = j;
          return retVec;
        }
      }
    }
#endif

    // [2,7,11,15, 21]
    // map
    // 2, 0
    // 7, 1
    // 11, 2
    // 15, 3
    // 21, 4  target = 26
    // Create hash map
#ifdef OPT1
    std::unordered_map<int, int> mmap;
    for (int i = 0; i < nums.size(); ++i)
    {
      mmap[nums[i]] = i;
    }
    for (int i = 0; i < nums.size(); ++i)
    {
      // Compute diff
      int diff = target - nums[i];
      if (mmap.count(diff))
      {
        if (i != mmap[diff])
        {
          retVec[0] = i;
          retVec[1] = mmap[diff];
          return retVec;
        }
      }
    }
#endif

#ifdef OPT2 95% speed   27% memory
    std::unordered_map<int, int> mmap;
    for (int i = 0; i < nums.size(); ++i)
    {
      mmap[nums[i]] = i;
      int diff = target - nums[i];
      if (mmap.count(diff))
      {
        if (i != mmap[diff])
        {
          retVec[0] = i;
          retVec[1] = mmap[diff];
          return retVec;
        }
      }
    }
    for (int i = 0; i < nums.size(); ++i)
    {
      // Compute diff
      int diff = target - nums[i];
      if (mmap.count(diff))
      {
        if (i != mmap[diff])
        {
          retVec[0] = i;
          retVec[1] = mmap[diff];
          return retVec;
        }
      }
    }
#endif

#define OPT3 //80% submission speed Memory terrible 8%
#ifdef OPT3
    // Create the map and sort the array.
    std::multimap<int, int> mmap;
    // O(n)
    for (int i = 0; i < nums.size(); ++i)
    {
      mmap.insert({nums[i], i});
    }
    // O(nlogn)
    std::sort(nums.begin(), nums.end());
    int b = 0;
    int e = nums.size() - 1;
    while (b < e)
    {
      int sum = nums[b] + nums[e];
      if (sum == target)
      {
        auto iterb = mmap.find(nums[b]);
        if (nums[b] != nums[e])
        {
          retVec[0] = iterb->second;
          auto itere = mmap.find(nums[e]);
          retVec[1] = itere->second;
          return retVec;
        }
        else
        {

          retVec[0] = iterb->second;
          ++iterb;
          retVec[1] = iterb->second;
          return retVec;
        }
      }
      if (sum < target)
      {
        ++b;
      }
      else
      {
        --e;
      }
    }

#endif
    return retVec;
  }
};