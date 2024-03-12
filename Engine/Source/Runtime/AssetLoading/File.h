#pragma once

#include <string>

#include "Types.h"

namespace sk {
class File {
public:
  File(const std::string &path);
  ~File();

  void *getData();

  u64 getSize();

private:
  void *data;
  const char *path;
  u64 size;
};

} // namespace sk
