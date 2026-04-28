#include <algorithm>
#include <vector>

class Solution {
public:
  int removeDuplicates(std::vector<int> &nums) {
    const auto it = std::unique(nums.begin(), nums.end());

    nums.resize(it - nums.begin());

    return nums.size();
  }
};
