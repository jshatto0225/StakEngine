#pragma once

#include <string>

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