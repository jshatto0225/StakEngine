#include "Win32Platform.h"

#ifdef SK_WINDOWS

#include "File.h"

#include "Asserts.h"
#include "Log.h"

/*********************
 * Private Interface *
 *********************/

void
LPOverlappedCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    // TODO: IDEK
}

/********************
 * Public Interface *
 ********************/

file *
CreateFile(const char *Path)
{
    file *File = (file *)malloc(sizeof(file));
    ASSERT(File);

    File->FilePath = Path;
    File->Data = NULL;
    HANDLE FileHandle = CreateFileA(File->FilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    ASSERT(FileHandle);

    LARGE_INTEGER FileSize;
    ASSERT(GetFileSizeEx(FileHandle, &FileSize));

    File->Size = FileSize.QuadPart;
    File->Data = VirtualAlloc(NULL, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    ASSERT(File->Data);

    // TODO: Figure out what this does
    OVERLAPPED IdkWhatThisDoes = {};
    // NOTE: Can fail if file is locked by another program
    if (!ReadFileEx(FileHandle, File->Data, (DWORD)FileSize.QuadPart, &IdkWhatThisDoes, LPOverlappedCompletionRoutine))
    {
        Log::Core::Error("Could not read file");
        VirtualFree(File->Data, File->Size, MEM_RESERVE);
        CloseHandle(FileHandle);
        File->Data = NULL;
        return NULL;
    }
    CloseHandle(FileHandle);
    
    return File;
}

void
DestroyFile(file **File)
{
    ASSERT(File);

    if (*File)
    {
        VirtualFree((*File)->Data, (*File)->Size, MEM_RESERVE);
        
        free(*File);
        
        *File = NULL;
    }
}

#endif
