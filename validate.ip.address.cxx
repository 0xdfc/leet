#include <benchmark/benchmark.h>
#include <cctype>
#include <cstdint>
#include <string>
#include <vector>

const char *ipv6 = "IPv6", *ipv4 = "IPv4", *neither = "Neither";

const char *loopback = "0.0.0.0";

// clang-format off
const static unsigned char hexset[] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,1,
    1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,1,1,1,
    1,1,1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0
};

const static unsigned char numset[] = {
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,1,
    1,1,1,1,1,1,1,1,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0
    
};
// clang-format on

unsigned char delimitersSeen = 0; // Not thread safe, not intended to be.

// Yes, I am abusing inlining and noexcept keywords :)
// statics are used purely for callability within benchmarks

static inline bool digit(const char &c) {
    return c >= '0' && c <= '9';
}

static inline bool hex(const char &c) {
    return digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

struct SolutionUsingSimpleLoops {
  private:
    static inline std::string vIpv4(const std::string &ip) noexcept {
        if (ip.size() > 15 || ip[0] == '0') {
            return neither;
        }

        int16_t n = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == '.') {
                ++delimitersSeen;

                if (!i || !ip[i + 1] || ip[i + 1] == '.' || delimitersSeen > 3) {
                    return neither;
                }

                n = 0;

                continue;
            } else if (!digit(ip[i]) || (ip[i] == '0' && ip[i + 1] != '.' && !n)) {
                return neither;
            }

            n = (n * 10) + (ip[i] - '0');

            if (n > 255) {
                return neither;
            }
        }

        return delimitersSeen != 3 ? neither : ipv4;
    }

    static inline std::string vIpv6(const std::string &ip) noexcept {
        if (ip.size() < 15) {
            return neither;
        }

        unsigned char length = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == ':') {
                ++delimitersSeen;

                if (!i || !ip[i + 1] || ip[i + 1] == ':' || delimitersSeen > 7) {
                    return neither;
                }

                length = 0;

                continue;
            }

            if (++length > 4 || !hex(ip[i])) {
                return neither;
            }
        }

        return delimitersSeen != 7 ? neither : ipv6;
    }

  public:
    static inline std::string valid(const std::string &ip) noexcept {
        if (ip.size() < 7 || ip.size() > 39) {
            return neither;
        }

        if (ip == loopback) {
            return ipv4;
        }

        delimitersSeen = 0;

        // Imperfect but should be sufficient to invalidate
        return ip[1] == '.' || ip[2] == '.' || ip[3] == '.'
                   ? vIpv4(ip)
                   : vIpv6(ip);
    }
};

struct SolutionUsingSimpleLoopsAndSets {
  private:
    // Can't use reference returns so keep it to pointers until the end
    // so we can avoid string construction every return, small but, eh,
    // may as well.
    static inline const char *vIpv4(const std::string &ip) noexcept {
        if (ip.size() > 15 || ip[0] == '0') {
            return neither;
        }

        int16_t n = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == '.') {
                // curiously incrementing within the condition seems to lead
                // to worse benchmarks, to be sure I need to have a look at
                // the asm (and figure it out)
                ++delimitersSeen;

                if (!i || !ip[i + 1] || ip[i + 1] == '.' || delimitersSeen > 3) {
                    return neither;
                }

                n = 0;

                continue;
            } else if (!numset[static_cast<unsigned char>(ip[i])] || (ip[i] == '0' && ip[i + 1] != '.' && !n)) {
                return neither;
            }

            n = (n * 10) + (ip[i] - '0');

            if (n > 255) {
                return neither;
            }
        }

        return delimitersSeen != 3 ? neither : ipv4;
    }

    static inline const char *vIpv6(const std::string &ip) noexcept {
        if (ip.size() < 15) {
            return neither;
        }

        unsigned char length = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == ':') {
                ++delimitersSeen;

                if (!i || !ip[i + 1] || ip[i + 1] == ':' || delimitersSeen > 7) {
                    return neither;
                }

                length = 0;
            } else if (++length > 4 || !hexset[static_cast<unsigned char>(ip[i])]) {
                return neither;
            }
        }

        return delimitersSeen != 7 ? neither : ipv6;
    }

  public:
    static inline std::string valid(const std::string &ip) noexcept {
        if (ip.size() < 7 || ip.size() > 39) {
            return neither;
        }

        if (ip == loopback) {
            return ipv4;
        }

        delimitersSeen = 0;

        // Imperfect but should be sufficient to invalidate
        // and far better than a .find != npos
        return ip[1] == '.' || ip[2] == '.' || ip[3] == '.'
                   ? vIpv4(ip)
                   : vIpv6(ip);
    }
};

struct SolutionUsingSimpleLoopsAndSetsAndInlinePreincrements {
  private:
    // Can't use reference returns so keep it to pointers until the end
    // so we can avoid string construction every return, small but, eh,
    // may as well.
    static inline const char *vIpv4(const std::string &ip) noexcept {
        if (ip.size() > 15 || ip[0] == '0') {
            return neither;
        }

        int16_t n = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == '.') {
                if (!i || !ip[i + 1] || ip[i + 1] == '.' || ++delimitersSeen > 3) {
                    return neither;
                }

                n = 0;

                continue;
            } else if (!numset[static_cast<unsigned char>(ip[i])] || (ip[i] == '0' && ip[i + 1] != '.' && !n)) {
                return neither;
            }

            n = (n * 10) + (ip[i] - '0');

            if (n > 255) {
                return neither;
            }
        }

        return delimitersSeen != 3 ? neither : ipv4;
    }

    static inline const char *vIpv6(const std::string &ip) noexcept {
        if (ip.size() < 15) {
            return neither;
        }

        unsigned char length = 0;

        for (unsigned char i = 0; i < ip.size(); ++i) {
            if (ip[i] == ':') {
                if (!i || !ip[i + 1] || ip[i + 1] == ':' || ++delimitersSeen > 7) {
                    return neither;
                }

                length = 0;
            } else if (++length > 4 || !hexset[static_cast<unsigned char>(ip[i])]) {
                return neither;
            }
        }

        return delimitersSeen != 7 ? neither : ipv6;
    }

  public:
    static inline std::string valid(const std::string &ip) noexcept {
        if (ip.size() < 7 || ip.size() > 39) {
            return neither;
        }

        if (ip == loopback) {
            return ipv4;
        }

        delimitersSeen = 0;

        // Imperfect but should be sufficient to invalidate
        // and far better than a .find != npos
        return ip[1] == '.' || ip[2] == '.' || ip[3] == '.'
                   ? vIpv4(ip)
                   : vIpv6(ip);
    }
};

class Solution {
  public:
    std::string validIPAddress(const std::string &ip) {
        return SolutionUsingSimpleLoopsAndSetsAndInlinePreincrements::valid(ip);
    }
};

/**
g++ -Wall -Werror -Wextra -O3 validate.ip.address.cxx -lbenchmark && ./a.out

-------------------------------------------------------------------
Benchmark                         Time             CPU   Iterations
-------------------------------------------------------------------
BM_simple_loops                 232 ns          232 ns      2730538
BM_simple_loops_and_sets        216 ns          216 ns      3547373

With CPU cache the preincrements seem to do better? But, different runs
show with and without battling for first place?

--------------------------------------------------------------------------------------------
Benchmark                                                  Time             CPU   Iterations
--------------------------------------------------------------------------------------------
BM_simple_loops                                          276 ns          276 ns      2502625
BM_simple_loops_and_sets                                 278 ns          278 ns      2634227
BM_simple_loops_and_sets_and_inline_preincrements        246 ns          246 ns      2761875
*/

std::vector<std::string>
    cases = {
        "6284.56.78.9",
        "172.16.254.1",
        "2001:0db8:85a3:0:0:8A2E:0370:7334",
        ":2001:0db8:85a3:0:0:8A2E:0370:7334",
        "2001:0db8:85a3:0:0:8A2E:0370:7334:",
        "2001:0db8:85a3:0:0::0370:7334",
        "2001:0db8:85a3:0:0:xc:0370:7334",
        "2001:0db8:85a3:0:0:cx:0370:7334",
        "1.1.1.1",
        "1.1.1.1.",
        ".1.1.1.1",
        "1.1.10.1",
        "1.1.01.1",
};

static void BM_simple_loops(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingSimpleLoops::valid(s);
        }
    }
}

static void BM_simple_loops_and_sets(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingSimpleLoopsAndSets::valid(s);
        }
    }
}

static void BM_simple_loops_and_sets_and_inline_preincrements(benchmark::State &state) {
    for (auto _ : state) {
        for (auto &s : cases) {
            SolutionUsingSimpleLoopsAndSetsAndInlinePreincrements::valid(s);
        }
    }
}

BENCHMARK(BM_simple_loops);
BENCHMARK(BM_simple_loops_and_sets);
BENCHMARK(BM_simple_loops_and_sets_and_inline_preincrements);

BENCHMARK_MAIN();
