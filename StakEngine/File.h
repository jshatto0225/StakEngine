#pragma 

#include "Types.h"

struct file {
  void *Data;
  const char *FilePath;
  u64 Size;
};

file *CreateFile(const char *Path);
void DestroyFile(file **File);
