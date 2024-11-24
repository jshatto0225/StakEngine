#pragma once

#include "RHICore.h"

class IRHIResource {
public:
  virtual ~IRHIResource() = default;

  virtual ERHIResourceUsage GetUsage() const = 0;
  virtual ERHIResourceType GetType() const = 0;
  virtual void SetUsage(ERHIResourceUsage Usage) = 0;
};

class IRHIBuffer : public IRHIResource {
public:
  virtual ~IRHIBuffer() = default;

  virtual FUInt64 GetOffset() const = 0;
  virtual FUInt32 GetStride() const = 0;
  virtual FUInt32 GetSize() const = 0;
  virtual FUInt32 GetElementCount() const = 0;
  virtual const FRHIBufferDescription &GetDescription() const = 0;
};

class IRHITexture : public IRHIResource {
public:
  static TRef<IRHITexture> Create(TRef<IRHIDevice> Device, FRHITextureDescription &Description);

  virtual ~IRHITexture() = default;

  virtual FUInt32 GetWidth() const = 0;
  virtual FUInt32 GetHeight() const = 0;
  virtual FUInt32 GetLayers() const = 0;
  virtual const FRHITextureDescription &GetDescription() const = 0;
};
