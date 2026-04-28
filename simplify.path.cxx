#include <benchmark/benchmark.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class Solution {
public:
  std::string simplifyPath(std::string path) {
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
    short ri = parts.size();

    while (ri)
      simplifiedPath = "/" + (parts[--ri] + simplifiedPath);

    return simplifiedPath.size() ? simplifiedPath : "/";
  }
};

int main() {
  auto s = new Solution;

  std::cout << s->simplifyPath("/home/user/Documents/../Pictures") << std::endl;
}
