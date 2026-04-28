#include <algorithm>
#include <vector>

class Solution {
public:
  int removeElement(std::vector<int> &nums, int val) {
    nums.erase(std::remove_if(nums.begin(), nums.end(),
                              [val](int v) { return v == val; }),
               nums.end());

    return nums.size();
  }
};
