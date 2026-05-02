#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

typedef std::pair<long, long> indices;

const int mod = std::pow(10, 9) + 7;

// struct SolutionWithCalculationOnQuery {
// private:
//   static int calculateOutcome(std::string &s, indices &indices) {
//     auto sv = std::string_view(s.data() + indices.first, indices.second);

//     unsigned long long x = 0, sum = 0, outcome = 0;

//     for (auto c : sv) {
//       if (c == '0')
//         continue;

//       char n = c - '0';

//       x = (x * 10 + n) % mod;
//       sum += (n % mod);
//     }

//     outcome = sum * x;

//     std::cout << "x:" << x << "|sum:" << sum << "|outcome:" << outcome
//               << std::endl;

//     if (outcome > mod) {
//       outcome = outcome % mod;
//     }

//     return outcome;
//   }

// public:
//   static std::vector<int>
//   sumAndMultiply(std::string &s, std::vector<std::vector<int>> &queries) {

//     std::map<indices, int> cache;
//     std::vector<int> outcomes;

//     outcomes.reserve(queries.size());

//     for (auto query : queries) {
//       auto indices = std::pair(query[0], query[1] - query[0] + 1);

//       auto found = cache.find(indices);

//       outcomes.push_back(
//           found != cache.end()
//               ? found->second
//               : SolutionWithCalculationOnQuery::calculateOutcome(s,
//               indices));
//     }

//     return outcomes;
//   }
// };

typedef struct s_sum_x_and_distance {
  long sum, x, distance;
} t_sum_x_and_distance;

struct solution_with_single_runtime_calculation {
public:
  static std::vector<int>
  sumAndMultiply(std::string &s, std::vector<std::vector<int>> &queries) {

    std::vector<t_sum_x_and_distance> precomputed;
    std::vector<int> outcomes;

    outcomes.reserve(queries.size());
    precomputed.reserve(s.size());

    for (size_t i = 0; i < s.size(); ++i) {
      t_sum_x_and_distance computed = {0, 0, 0}, previous = {0, 0, 0};

      const char n = s[i] - '0';

      if (i)
        previous = precomputed[i - 1];

      if (s[i] == '0') {
        computed = previous;
      } else {
        computed.x = (previous.x * 10 + n) % mod;
        computed.sum = (previous.sum + n) % mod;
        computed.distance = previous.distance + 1;
      }

      std::cout << "i:" << i << "|x:" << computed.x << "|sum:" << computed.sum
                << std::endl;

      precomputed.push_back(computed);
    }

    for (auto query : queries) {
      t_sum_x_and_distance r = precomputed[query[1]], l = {0, 0, 0};

      if (query[0])
        l = precomputed[query[0] - 1];

      const long long distance = std::pow(10, r.distance - l.distance);

      std::cout << "sum:" << r.sum - l.sum % mod << "|distance:" << distance
                << "|l.x:" << l.x << "|r.x:" << r.x
                << "|x:" << (r.x - (l.x * distance)) % mod << std::endl;

      outcomes.push_back((r.x - (l.x * distance)) % mod * (r.sum - l.sum) %
                         mod);
    }

    return outcomes;
  }
};

class Solution {
public:
  std::vector<int> sumAndMultiply(std::string &s,
                                  std::vector<std::vector<int>> &queries) {
    return solution_with_single_runtime_calculation::sumAndMultiply(s, queries);
  }
};

int main() {
  std::string ss = "2711785625";
  auto s = Solution();
  std::vector<std::vector<int>> sv = {
      {8, 9},
  };

  for (auto r : s.sumAndMultiply(ss, sv)) {
    std::cout << "r:" << r << std::endl;
  }
}
