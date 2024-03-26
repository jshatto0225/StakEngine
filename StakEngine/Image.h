#pragma once

#include "Types.h"

struct image_data {
  const char *Path;
  i32 Width;
  i32 Height;
  i32 Channels;
  bool FlipOnLoad;
  u64 SizeInBytes;
};

struct image {
  bool FlipOnLoad;
  u8 *Bytes;
  image_data Data;
};

image *CreateImage(const char *Path);
void DestoyImage(image **Image);
void LoadImageAsBMP(image *Image, u8 *FileData, u64 FileSize);
