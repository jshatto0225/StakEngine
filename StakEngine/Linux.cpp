#if defined(SK_LINUX)

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "File.h"
#include "Log.h"
#include "Image.h"

namespace sk {

File::File(const std::string &path) {
  this->path = path.c_str();

  i32 fd = open(this->path, O_RDONLY);
  if (fd == -1) {
    Log::CoreError("Failed to open file");
    return;
  }

  struct stat file_stat;
  if (fstat(fd, &file_stat) == -1) {
    Log::CoreError("Error getting file size");
    close(fd);
    return;
  }

  this->size = file_stat.st_size;
  this->data = mmap(nullptr, this->size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (this->data == MAP_FAILED) {
    Log::CoreError("Failed to map file to memory");
  }

  close(fd);
}

File::~File() {
  if (munmap(this->data, this->size) == -1) {
    Log::CoreError("Failed to unmap file");
  }
}

void *File::getData() {
  return this->data;
}

u64 File::getSize() {
  return this->size;
}

Image::Image(const std::string &path) {
  this->data.path = path;
  this->bytes = nullptr;

  File f(this->data.path);
  u8 *file_data = (u8 *)f.getData();
  u64 file_size = f.getSize();
  if (path.find(".bmp") != std::string::npos) {
    LoadAsBMP(file_data, file_size);
    return;
  }
  Log::CoreError("Unsupported Image Format");
}

Image::~Image() {
  if (this->bytes) {
    munmap(this->bytes, this->data.size_in_bytes);
  }
}

void Image::LoadAsBMP(u8 *file_data, u64 file_size) {
  if (file_data[0] != 'B' || file_data[1] != 'M') {
    Log::CoreError("Did not find BM");
    return;
  }
  this->data.size_in_bytes = *((u32 *)(file_data + 2));
  u32 offset = *((u32 *)(file_data + 10));
  this->data.width = *((i32 *)(file_data + 18));
  this->data.height = *((i32 *)(file_data + 22));
  u16 bpp = *((u16 *)(file_data + 28));
  switch (bpp) {
  case 1:
    this->data.channels = 1;
    break;
  case 8:
    this->data.channels = 1;
    break;
  case 24:
    this->data.channels = 24;
    break;
  case 32:
    this->data.channels = 32;
    break;
  default:
    Log::CoreError("Invalid BPP: %d", bpp);
    this->data.channels = 0;
    break;
  }
  this->bytes = (u8 *)mmap(nullptr, this->data.size_in_bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (!this->bytes) {
    Log::CoreError("Allocation Failed");
    return;
  }
  memcpy(this->bytes, file_data + offset, this->data.size_in_bytes);
}

u8 *Image::getBytes() {
  return this->bytes;
}

const ImageData &Image::getData() {
  return this->data;
}

} // namespace sk

#endif