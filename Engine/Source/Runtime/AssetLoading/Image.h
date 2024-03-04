#pragma once

#include <fstream>

#include "Types.h"

class Image {
public:
  static u8 *Load(const std::string &path, i32 *width, i32 *height, i32 *channels_in_image, i32 desired_channels);
  static void SetFlipOnLoad(bool flipOnLoad);
  static void Free(u8 *data);

private:
  static u8 *LoadBMP(FILE *f, i32 *width, i32 *height, i32 *channels_in_image, i32 desired_channels);

private:
  static bool flip_on_load;
};