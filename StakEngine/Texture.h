#pragma once

#include "Types.h"
#include "Image.h"

struct texture_specification
{
  i32 Width;
  i32 Height;
  image_format Format;
  bool GenerateMips;
};

struct texture;
struct texture2d;

texture *CreateTexture(const texture_specification *Spec);
void DestroyTexture(texture **Texture);
void BindTexture(texture *Texture);
const texture_specification *GetTextureSpecification(const texture *Texture);
bool CompareTexture(const texture *Tex1, const texture *Tex2);

texture2d *CreateTexture2D(const char *FilePath);
texture2d *CreateTexture2D(const texture_specification *Spec);
void DestroyTexture2D(texture2d **Tex);
void SetTexture2DData(texture2d *Tex, void *Data, u32 Size);
const char *GetTexture2DFilePath(texture2d *Tex);
u32 GetRendererId(texture2d *Tex);
u32 GetTexture2DHeight(texture2d *Tex);
u32 GetTexture2DWidth(texture2d *Tex);
const texture_specification *GetTexture2DSpecification(texture2d *Tex);
void BindTexture2D(const texture2d *Tex, u32 Slot = 0);
bool IsTexture2DLoaded(const texture2d *Tex);
bool CompareTexture2D(const texture2d *Tex1, const texture2d *Tex2);