#pragma once

#define For_Index(index, max) for (u32 index = 0; index < (u32)max; index++)
#define ForI(max) For_Index(i, max)
#define ForJ(max) For_Index(j, max)
#define ForK(max) For_Index(k, max)

#define For(max) ForI(max)

#define It(iterable) for (auto it : iterable)
#define CIt(iterable) for (const auto it : iterable)

#define Inc(index, start, end) for (u32 index = (u32)start; index < (u32)end; index++)
#define Dec(index, start, end) for (u32 index = (u32)start; index > (u32)end; index--)
