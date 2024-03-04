#include "Image.h"

bool Image::flip_on_load = false;

u8 *Image::Load(const std::string &path, i32 *width, i32 *height, i32 *channels_in_image, i32 desired_channels) {
  FILE *f = fopen(path.c_str(), "r");
  if (path.substr(path.find_last_of(".") + 1) == "bmp") {
    u8 *ret = Image::LoadBMP(f, width, height, channels_in_image, desired_channels);
    fclose(f);
    return ret;
  }
  return nullptr;
}

void Image::SetFlipOnLoad(bool flipOnLoad) {
  Image::flip_on_load = flipOnLoad;
}

void Image::Free(u8 *data) {
  if (data != nullptr) {
    delete data;
  }
}
