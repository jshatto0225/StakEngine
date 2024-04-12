#include "Win32Platform.h"

#ifdef SK_WINDOWS

#include "Image.h"

#include "Log.h"
#include "File.h"

//////////////////////
// Public Interface //
//////////////////////

bool
IsBMPFile(const char *Path)
{
    i32 Len = (i32)strlen(Path);
    if (Len < 4) // If string length is less than 4, it can't end with ".bmp"
        return 0;

    // Compare the last four characters with ".bmp"
    return strcmp(Path + Len - 4, ".bmp") == 0;
}

image *
CreateImage(const char *Path)
{
    image *Image = (image *)malloc(sizeof(image));
    
    if (!Image)
    {
        LogCoreError("Failed to allocate memory for image");
        return NULL;
    }
    
    Image->Data.Path = Path;
    Image->Bytes = NULL;
    
    file *File = CreateFile(Image->Data.Path);
    u8 *FileData = (u8 *)File->Data;
    u64 FileSize = File->Size;
    if (IsBMPFile(Path))
    {
        LoadImageAsBMP(Image, FileData, FileSize);
        DestroyFile(&File);
        return Image;
    }
    LogCoreError("Unsupported Image Format");
    return NULL;
}

void
DestroyImage(image **Image)
{
    if (*Image)
    {
        VirtualFree((*Image)->Bytes, (*Image)->Data.SizeInBytes, MEM_RESERVE);
        
        free(*Image);
        
        *Image = NULL;
    }
}

void
LoadImageAsBMP(image *Image, u8 *FileData, u64 FileSize)
{
    if (FileData[0] != 'B' || FileData[1] != 'M')
    {
        LogCoreError("Did not find BM");
        return;
    }
    Image->Data.SizeInBytes = *((u32 *)(FileData + 2));
    u32 Offset = *((u32 *)(FileData + 10));
    Image->Data.Width = *((i32 *)(FileData + 18));
    Image->Data.Height = *((i32 *)(FileData + 22));
    u16 BPP = *((u16 *)(FileData + 28));
    switch (BPP)
    {
    case 1:
        Image->Data.Channels = 1;
        break;
    case 8:
        Image->Data.Channels = 1;
        break;
    case 24:
        Image->Data.Channels = 3;
        break;
    case 32:
        Image->Data.Channels = 4;
        break;
    default:
        LogCoreError("Invalid BPP: %d", BPP);
        Image->Data.Channels = 0;
        break;
    }
    Image->Bytes = (u8 *)VirtualAlloc(NULL,
                                      Image->Data.SizeInBytes,
                                      MEM_RESERVE | MEM_COMMIT,
                                      PAGE_READWRITE);
    if (!Image->Bytes)
    {
        LogCoreError("Allocation Failed");
        return;
    }
    memcpy(Image->Bytes, FileData + Offset, Image->Data.SizeInBytes);
}
#endif
