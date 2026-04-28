#include <benchmark/benchmark.h>
#include <cstring>
#include <stack>
#include <string>
#include <vector>

// ? We avoid use of class props to avoid skipping
// ? timing of intermediate variable construction which
// ? is seemingly something LC skips.

struct ContiguousMemoryOptimization {
  char *path;
  short i, next;
  std::vector<char *> parts;
};

struct SolutionUsingMostlyRawDataAndStringBrickingWithStruct {
  static std::string simplify(std::string &path) {
    ContiguousMemoryOptimization m = {
        .path = const_cast<char *>(path.c_str()),
        .i = 0,
        .next = 0,
        .parts = std::vector<char *>(),
    };

    m.parts.reserve(1000);

    while (m.path[m.i]) {
      if (m.path[m.i] == '/') {
        ++m.i;
        continue;
      }

      while (m.path[++m.next] && m.path[m.next] != '/')
        ;

      const short length = m.next - m.i;

      if (m.path[m.next])
        m.path[m.next++] = 0;

      if (!strcmp(m.path + m.i, "..")) {
        if (m.parts.size())
          m.parts.pop_back();
      } else if (length > 1 || m.path[m.i] != '.') {
        m.parts.push_back(m.path + m.i);
      }

      m.i = m.next;
    }

    if (!m.parts.size()) {
      return "/";
    }

    short ri = m.parts.size();
    std::string simplified;

    while (ri)
      simplified = "/" + (m.parts[--ri] + simplified);

    return simplified.size() ? simplified : "/";
  }
};

struct SolutionUsingMostlyRawDataAndStringBricking {
  static std::string simplify(std::string &path) {
    char *cpath = const_cast<char *>(path.c_str());

    std::vector<char *> parts;

    parts.reserve(1000);

    short i = 0;

    while (cpath[i]) {
      if (cpath[i] == '/') {
        ++i;
        continue;
      }

      short next = i;

      while (cpath[++next] && cpath[next] != '/')
        ;

      const short length = next - i;

      if (cpath[next])
        cpath[next++] = 0;

      if (!strcmp(cpath + i, "..")) {
        if (parts.size())
          parts.pop_back();
      } else if (length > 1 || cpath[i] != '.') {
        parts.push_back(cpath + i);
      }

      i = next;
    }

    if (!parts.size()) {
      return "/";
    }

    std::string simplifiedPath;
    simplifiedPath.reserve(i);

    for (auto part : parts) {
      simplifiedPath += "/";
      simplifiedPath += part;
    }

    return simplifiedPath.size() ? simplifiedPath : "/";
  }
};

struct SolutionUsingCxxUtility {
  static std::string simplify(std::string &path) {
    std::vector<std::string> parts;
    const char *cpath;
    short i = 0;

    parts.reserve(1000);
    cpath = path.c_str();

    while (cpath[i]) {
      if (cpath[i] == '/') {
        ++i;
        continue;
      }

      short next = i;

      while (cpath[++next] && cpath[next] != '/')
        ;

      const short length = next - i;

      auto chunk = std::string(cpath + i, length);

      if (chunk == "..") {
        if (parts.size()) {
          parts.pop_back();
        }
      } else if (length > 1 || cpath[i] != '.') {
        parts.push_back(chunk);
      }

      i = next;
    }

    if (!parts.size()) {
      return "/";
    }

    std::string simplifiedPath;
    // These add about 2k ns
    auto it = parts.rbegin();
    auto ite = parts.rend();

    while (it != ite) {
      simplifiedPath = "/" + *it++ + simplifiedPath;
    }

    return simplifiedPath.size() ? simplifiedPath : "/";
  }
};

struct SolutionNaiveUsingStackPopping {
  static std::string simplify(std::string &path) {
    const char *cpath = path.c_str();
    size_t i = 0;
    std::stack<std::string> parts;

    while (cpath[i]) {
      if (cpath[i] == '/' && ++i)
        continue;

      size_t next = i;

      while (cpath[++next] && cpath[next] != '/')
        ;

      auto chunk = std::string(cpath + i, next - i);

      if (chunk == "..") {
        if (parts.size()) {
          parts.pop();
        }
      } else if (chunk != ".") {
        parts.push(chunk);
      }

      i = next;
    }

    if (!parts.size()) {
      return "/";
    }

    std::string simplifiedPath;

    while (parts.size()) {
      std::string part = parts.top();

      simplifiedPath = "/" + parts.top() + simplifiedPath;

      parts.pop();
    }

    return simplifiedPath.size() ? simplifiedPath : "/";
  }
};

class Solution {
public:
  std::string simplifyPath(std::string &path) {
    return SolutionUsingMostlyRawDataAndStringBricking::simplify(path);
  }
};

std::vector<std::string> cases = {
    "/home/user/Documents/../Pictures",
    "/",
    "///",
    "/././",
    "/../",
    "/../..././././",
    "/home/",
    "/home//foo/",
    "/.../a/../b/c/../d/./",
    "/a/./b/../../c/",
    "/a/../../b/../c//.//",
    "/a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../"
    "c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../"
    "../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.//"
    "/a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../"
    "c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../"
    "../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.//"
    "/a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../"
    "c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../"
    "../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.//"
    "/a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../"
    "c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../"
    "../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.//"
    "/a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../"
    "c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../"
    "../b/../c//.///a/../../b/../c//.///a/../../b/../c//.///a/../../b/../c//.//"
    "/a/../../b/../c//.//",
};

static void BM_raw_data_solution(benchmark::State &state) {
  for (auto _ : state) {
    for (auto c : cases) {
      SolutionUsingMostlyRawDataAndStringBricking::simplify(c);
    }
  }
}

static void BM_cxx_utility_solution(benchmark::State &state) {
  for (auto _ : state) {
    for (auto c : cases) {
      SolutionUsingCxxUtility::simplify(c);
    }
  }
}

static void BM_naive_solution(benchmark::State &state) {
  for (auto _ : state) {
    for (auto c : cases) {
      SolutionNaiveUsingStackPopping::simplify(c);
    }
  }
}

static void BM_contig_struct_memory(benchmark::State &state) {
  for (auto _ : state) {
    for (auto c : cases) {
      SolutionUsingMostlyRawDataAndStringBrickingWithStruct::simplify(c);
    }
  }
}

BENCHMARK(BM_raw_data_solution);
BENCHMARK(BM_cxx_utility_solution);
BENCHMARK(BM_naive_solution);
BENCHMARK(BM_contig_struct_memory);

BENCHMARK_MAIN();
