#include "Win32Platform.h"

#ifdef SK_WINDOWS

#include "File.h"

#include "Log.h"

///////////////////////
// Private Interface //
///////////////////////

void
LPOverlappedCompletionRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    // TODO: IDEK
}

//////////////////////
// Public Interface //
//////////////////////

file *
CreateFile(const char *Path)
{
    file *File = (file *)malloc(sizeof(file));

    if (!File)
    {
        LogCoreError("Failed to allocate memory for file");
        return NULL;
    }
    
    File->FilePath = Path;
    File->Data = NULL;
    HANDLE FileHandle = CreateFileA(File->FilePath,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    0,
                                    OPEN_EXISTING,
                                    0,
                                    0);
    if (!FileHandle)
    {
        // Error
        LogCoreError("Failed to open file");
        return NULL;
    }
    LARGE_INTEGER FileSize;
    if (!GetFileSizeEx(FileHandle, &FileSize))
    {
        // Error
        LogCoreError("File size error");
        CloseHandle(FileHandle);
        return NULL;
    }
    File->Size = FileSize.QuadPart;
    File->Data = VirtualAlloc(NULL, FileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!File->Data)
    {
        // Error
        LogCoreError("Could not allocate password");
        CloseHandle(FileHandle);
        return NULL;
    }
    OVERLAPPED IdkWhatThisDoes = {};
    if (!ReadFileEx(FileHandle, File->Data, (DWORD)FileSize.QuadPart, &IdkWhatThisDoes, LPOverlappedCompletionRoutine))
    {
        LogCoreError("Could not read file");
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
    if (*File) {
        VirtualFree((*File)->Data, (*File)->Size, MEM_RESERVE);
        
        free(*File);
        
        *File = NULL;
    }
}

#endif
