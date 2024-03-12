#pragma once

#include <string>

#include "Types.h"

struct ImageData {
  std::string path;
  i32 width;
  i32 height;
  i32 channels;
  bool flip_on_load;
  u64 size_in_bytes;
};

class Image {
public:
  Image(const std::string &path);
  ~Image();
  u8 *getBytes();
  const ImageData &getData();

private:
  void LoadAsBMP(u8 *file_data, u64 file_size);

private:
  bool flip_on_load;
  u8 *bytes;
  ImageData data;
};
