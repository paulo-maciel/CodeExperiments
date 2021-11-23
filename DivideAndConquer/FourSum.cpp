
// Works but non performant!

class Solution
{
public:
  bool isUsed(const vector<vector<int>> &ret, vector<int> vec)
  {
    if (ret.empty())
      return false;

    for (auto v : ret)
    {
      if (is_permutation(v.begin(), v.end(), vec.begin()))
        return true;
    }
    return false;
  }

  vector<vector<int>> fourSum(vector<int> &nums, int target)
  {
    vector<vector<int>> ret;
    vector<int> perm;
    int sz = nums.size();
    set<int> used;
    for (int i = 0; i < sz; ++i)
    {
      perm.push_back(nums[i]);
      for (int j = 0; j < sz; ++j)
      {
        if (j == i)
          continue;
        perm.push_back(nums[j]);
        for (int k = 0; k < sz; ++k)
        {
          if (k == i || k == j)
            continue;
          perm.push_back(nums[k]);
          for (int l = 0; l < sz; ++l)
          {
            if (l == i || l == j || l == k)
              continue;
            perm.push_back(nums[l]);
            int sum = 0;
            for (int idx = 0; idx < perm.size(); ++idx)
            {
              sum += perm[idx];
            }
            if (sum == target)
            {
              if (!isUsed(ret, perm))
              {
                ret.push_back(perm);
              }
            }
            perm.pop_back();
          }
          perm.pop_back();
        }
        perm.pop_back();
      }
      perm.pop_back();
    }

    return ret;
  }
};