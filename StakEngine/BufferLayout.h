#pragma once

#include "Types.h"
#include "ShaderDataTypes.h"

/**
 * @brief Struct to store buffer element properties
 */
struct buffer_element
{
    const char *Name; /**< Element name */
    shader_data_type Type; /**< Element data type */
    u32 Size; /**< Element size in bytes */
    u64 Offset; /**< Element offset in bytes */
    bool Normalized; /**< Element normalized flag */
};

/**
 * @brief Struct to store the layout of a buffer 
 */
struct buffer_layout
{
    u64 Size; /**< Size of layout */
    u32 Stride; /**< Stride of layout */
    buffer_element *Elements; /**< Elements in layout */
};

/**
 * @brief Get the buffer element component count
 * 
 * @param BufferElement The buffer element to get the count from
 * @return u32 The count
 */
u32 GetBufferElementComponentCount(const buffer_element *BufferElement);

/**
 * @brief Create a buffer layout object
 * 
 * @param Size Size of layout
 * @return buffer_layout * resulting object
 */
buffer_layout *CreateBufferLayout(u64 Size);

/**
 * @brief Add buffer element to a buffer layout 
 *        The layout object is modified
 * 
 * @param Layout Layout to be modififed
 * @param Position Position of new element
 * @param Type Data type of new element
 * @param Name Name of new element
 * @param Normailized Normalized flag of new element
 */
void AddElementToLayout(buffer_layout *Layout, u64 Position, shader_data_type Type, const char *Name, bool Normailized);

/**
 * @brief Calculate offset and stride of a layout
 *        The layout is modified with the resulting values
 * 
 * @param Layout Layout to update
 */
void CalculateOffsetsAndStride(buffer_layout *Layout);

/**
 * @brief Delete and deallocate a buffer layout
 *        Deleted layout will be set to NULL
 * 
 * @param BufferLayout Layout to be deleted
 */
void DestroyBufferLayout(buffer_layout **BufferLayout);
