#include <algorithm>
#include <cstdint>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#ifdef UBENCHMARK
#include <benchmark/benchmark.h>
#endif

#ifdef UDEBUG
#include <iostream>
#endif

struct SolutionUsingIntegerSetsAndInitialCount {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[128] = {};

        for (const unsigned char c : s) {
            ++tracker[c];
        }

        auto fillable = tracker['?'];

        if (!fillable) {
            return s;
        }

        std::map<int, std::set<char>> buckets;

        for (unsigned char c = 'a'; c <= 'z'; ++c) {
            buckets[tracker[c]].insert(c);
        }

        std::vector<char> queue;

        queue.reserve(fillable);

        std::string result = s;
        std::int_fast32_t l = s.size();

        do {
            auto control = buckets.begin();

            auto &charset = control->second;

            auto emplaceable = *charset.begin();

            queue.push_back(emplaceable);

            charset.erase(emplaceable);

            buckets[control->first + 1].insert(emplaceable);

            if (charset.empty()) {
                buckets.erase(control);
            }
        } while (--fillable);

        std::ranges::sort(queue);

        for (std::int_fast32_t i = -1, j = -1; i < l;) {
            auto &c = s[++i];

            if (c != '?') {
                continue;
            }

            result[i] = queue[++j];
        }

        return result;
    }
};

struct SolutionUsingIntegerSetsAndInitialCountAndMarkerRanges {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[128] = {};

        std::uint_fast16_t rc = 0;

        // 50000 = s.size() max / 2
        std::int_fast32_t ranges[50000][2] = {};
        std::int_fast32_t range = 0;

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            ++tracker[c];

            if (c != '?') {
                continue;
            }

            if (!range) {
                range = i;
            }

            if (s[i + 1] != '?') {
                ranges[rc][0] = range;
                ranges[rc][1] = i + 1;
                ++rc;
            }
        }

        // backwards patch one edge case instead of checking a condition in the loop above
        // for every element
        if (s[0] == '?') {
            ranges[0][0] = 0;
        }

        auto fillable = tracker['?'];

        if (!fillable) {
            return s;
        }

        std::map<std::int_fast32_t, std::set<char>> buckets;

        for (unsigned char c = 'a'; c <= 'z'; ++c) {
            buckets[tracker[c]].insert(c);
        }

        std::string queue(fillable, 0);

        std::string result = s;
        std::int_fast32_t l = s.size();
        std::uint_fast16_t rcc = rc, rci = 0;

        while (rci != rcc) {
            const auto range = ranges[rci];

            std::int_fast32_t total = range[1] - range[0];

            while (total) {
                auto control = buckets.begin();

                auto &charset = control->second;

                const auto cs = static_cast<std::int_fast32_t>(charset.size());

                auto taking = total > cs ? cs : total;

                auto from = charset.begin(), until = std::next(charset.end(), taking);

                queue.append(from, until);

                buckets[control->first + 1].insert(from, until);

                total -= taking;

                if (total >= cs)
                    buckets.erase(control);
                else
                    charset.erase(from, until);
            }

            ++rci;
        }

        std::ranges::sort(queue);

        for (std::int_fast32_t i = -1, j = -1; i < l;) {
            auto &c = s[++i];

            if (c != '?') {
                continue;
            }

            result[i] = queue[++j];
        }

        return result;
    }
};

struct SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRanges {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[128] = {};
        std::vector<std::int_fast32_t> positions;

        positions.reserve(s.size());

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            ++tracker[c];

            if (c == '?') {
                positions.push_back(i);
            }
        }

        auto fillable = tracker['?'];

        if (!fillable) {
            return s;
        }

        std::string result = s;
        std::map<int, std::set<char>> buckets;
        std::vector<char> queue;

        queue.reserve(fillable);

        for (unsigned char c = 'a'; c <= 'z'; ++c) {
            buckets[tracker[c]].insert(c);
        }

        auto control = buckets.begin();

        do {
            auto &charset = control->second;

            auto emplaceable = *charset.begin();

            queue.push_back(emplaceable);

            charset.erase(emplaceable);

            buckets[control->first + 1].insert(emplaceable);

            if (charset.empty()) {
                ++control;
            }
        } while (--fillable != 0);

        std::ranges::sort(queue);

        std::int_fast32_t j = -1;

        for (const auto i : positions) {
            result[i] = queue[++j];
        }

        return result;
    }
};

struct SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRangesAndWithoutMapsAndSets {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[128] = {};
        std::vector<std::int_fast32_t> positions;

        positions.reserve(s.size());

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            ++tracker[c];

            if (c == '?') {
                positions.push_back(i);
            }
        }

        auto fillable = tracker['?'];

        if (!fillable) {
            return s;
        }

        std::string result = s;
        std::vector<char> queue;

        queue.reserve(fillable);

        std::int_fast32_t fci = -1;

        while (++fci != fillable) {
            unsigned char c = 'a' - 1, next = 255;

            std::int_fast32_t n = 100000;

            while (++c <= 'z') {
                // std::cout << "c pre push:" << c << "|n:" << tracker[c] << std::endl;
                if (n > tracker[c]) {
                    n = tracker[c];
                    next = c;
                }
            }

            ++tracker[next];

            // std::cout << "pushing:" << next << std::endl;
            queue.push_back(next);
        }

        std::ranges::sort(queue);

        std::int_fast32_t j = -1;

        for (const auto i : positions) {
            result[i] = queue[++j];
        }

        return result;
    }
};

struct SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRangesAndWithoutMapsAndSetsSplitQmLoop {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[128] = {};
        std::vector<std::int_fast32_t> positions;

        positions.reserve(s.size());

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            ++tracker[c];

            // if (c == '?') {
            //     positions.push_back(i);
            // }
        }

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            if (c == '?') {
                positions.push_back(i);
            }
        }

        auto fillable = tracker['?'];

        if (!fillable) {
            return s;
        }

        std::string result = s;
        std::vector<char> queue;

        queue.reserve(fillable);

        std::int_fast32_t fci = -1;

        while (++fci != fillable) {
            unsigned char c = 'a' - 1, next = 255;

            std::int_fast32_t n = 100000;

            while (++c <= 'z') {
                // std::cout << "c pre push:" << c << "|n:" << tracker[c] << std::endl;
                if (n > tracker[c]) {
                    n = tracker[c];
                    next = c;
                }
            }

            ++tracker[next];

            // std::cout << "pushing:" << next << std::endl;
            queue.push_back(next);
        }

        std::ranges::sort(queue);

        std::int_fast32_t j = -1;

        for (const auto i : positions) {
            result[i] = queue[++j];
        }

        return result;
    }
};

struct SolutionUsingIntegerSetsAndInitialCountAndMarkersAndSmallerLookupTable {
  public:
    static inline std::string minimize(const std::string &s) noexcept {
        std::int_fast32_t tracker[26] = {};
        std::vector<std::int_fast32_t> positions;

        positions.reserve(s.size());

        for (std::int_fast32_t i = -1; const unsigned char c = s[++i];) {
            if (c != '?') {
                ++tracker[c - 'a'];
            } else {
                positions.push_back(i);
            }
        }

        const std::int_fast32_t fillable = positions.size();

        if (!fillable) {
            return s;
        }

        std::string result = s;
        std::vector<char> queue;

        queue.reserve(fillable);

        std::int_fast32_t fci = -1;

        while (++fci != fillable) {
            std::uint_fast8_t c = 0, next = 255;
            std::int_fast32_t n = 100000;

            while (c < 26) {
                if (n > tracker[c]) {
                    n = tracker[c];
                    next = c;
                }

                ++c;
            }

            ++tracker[next];

            queue.push_back(next);
        }

        std::ranges::sort(queue);

        std::int_fast32_t j = -1;

        for (const auto i : positions) {
            result[i] = queue[++j] + 'a';
        }

        return result;
    }
};

class Solution {
  public:
    std::string minimizeStringValue(const std::string &s) {
        return SolutionUsingIntegerSetsAndInitialCountAndMarkersAndSmallerLookupTable::minimize(s);
    }
};

std::vector<std::string>
    cases = {
        "r?x??qbimy??vzng?tw??qqz?exo?i?i??sag?uqd???j?ayuezd",
        "?htxzbzkngkgna?zrzc??fkcep?",
        "abcdefghijklmnopqrstuvwxy??",
        "abcdefghijklmnopqrstuvwx???",
        "abcdefghijklmnopqrstuvw????",
        "??",
        "?",
        "????????????????????????????????????????",
};

#ifdef UBENCHMARK

static void BM_using_simple_sets_and_count(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCount::minimize(s);
        }
    }
}

static void BM_using_simple_sets_and_count_and_ranges(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCountAndMarkerRanges::minimize(s);
        }
    }
}

static void BM_using_simple_sets_and_count_and_markers(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRanges::minimize(s);
        }
    }
}

static void BM_using_simple_sets_and_count_and_markers_without_maps_and_sets(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRangesAndWithoutMapsAndSets::minimize(s);
        }
    }
}

static void BM_using_simple_sets_and_count_and_split_qm_loop(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCountAndMarkersWithoutRangesAndWithoutMapsAndSetsSplitQmLoop::minimize(s);
        }
    }
}

static void BM_using_simple_sets_and_count_and_small_lookup(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingIntegerSetsAndInitialCountAndMarkersAndSmallerLookupTable::minimize(s);
        }
    }
}

BENCHMARK(BM_using_simple_sets_and_count);
BENCHMARK(BM_using_simple_sets_and_count_and_ranges);
BENCHMARK(BM_using_simple_sets_and_count_and_markers);
BENCHMARK(BM_using_simple_sets_and_count_and_markers_without_maps_and_sets);
BENCHMARK(BM_using_simple_sets_and_count_and_split_qm_loop);
BENCHMARK(BM_using_simple_sets_and_count_and_small_lookup);

BENCHMARK_MAIN();

#endif

#if UDEBUG || UPROFILE
int main() {
    Solution s;

    for (std::int_fast32_t i = -1; ++i < 1000000;) {
        for (const auto &c : cases) {
            s.minimizeStringValue(c);
        }
    }

    // std::cout << s.minimizeStringValue("r?x??qbimy??vzng?tw??qqz?exo?i?i??sag?uqd???j?ayuezd")
    //           << std::endl;

    // std::cout << s.minimizeStringValue("?htxzbzkngkgna?zrzc??fkcep?") << std::endl;
    // std::cout << s.minimizeStringValue("abcdefghijklmnopqrstuvwxy??") << std::endl;
    // std::cout << s.minimizeStringValue("abcdefghijklmnopqrstuvwx???") << std::endl;
    // std::cout << s.minimizeStringValue("abcdefghijklmnopqrstuvw????") << std::endl;
}
#endif
