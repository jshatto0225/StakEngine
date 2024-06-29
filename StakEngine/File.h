#pragma once

#include "Types.h"

/**
 * @brief Struct to store data from a file
 */
struct file
{
    void *Data; /**< File data */
    const char *FilePath; /**< File path */
    u64 Size; /**< File size in bytes */
};

/**
 * @brief Create a file object
 * 
 * @param Path File path to open
 * @return file * resulting object
 */
file *CreateFile(const char *Path);

/**
 * @brief Destry file object
 *        File will be set to NULL
 * 
 * @param File File to destroy
 */
void DestroyFile(file **File);
