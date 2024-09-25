// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_IMAGE_H
#define _ALIMER_IMAGE_H

#if defined(ALIMER_IMAGE_SHARED_LIBRARY)
#    if defined(_WIN32)
#        if defined(ALIMER_IMAGE_IMPLEMENTATION)
#            define _ALIMER_IMAGE_EXPORT __declspec(dllexport)
#        else
#            define _ALIMER_IMAGE_EXPORT __declspec(dllimport)
#        endif
#    else
#        if defined(ALIMER_IMAGE_IMPLEMENTATION)
#            define _ALIMER_IMAGE_EXPORT __attribute__((visibility("default")))
#        else
#            define _ALIMER_IMAGE_EXPORT
#        endif
#    endif
#else
#    define _ALIMER_IMAGE_EXPORT
#endif

#ifdef __cplusplus
#    define _ALIMER_IMAGE_EXTERN extern "C"
#else
#    define _ALIMER_IMAGE_EXTERN extern
#endif

#define ALIMER_IMAGE_API _ALIMER_IMAGE_EXTERN _ALIMER_IMAGE_EXPORT 

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct AlimerImage AlimerImage;

typedef enum PixelFormat {
    PixelFormat_Undefined = 0,
    // 8-bit formats
    PixelFormat_R8Unorm,
    PixelFormat_R8Snorm,
    PixelFormat_R8Uint,
    PixelFormat_R8Sint,
    // 16-bit formats
    PixelFormat_R16Unorm,
    PixelFormat_R16Snorm,
    PixelFormat_R16Uint,
    PixelFormat_R16Sint,
    PixelFormat_R16Float,
    PixelFormat_RG8Unorm,
    PixelFormat_RG8Snorm,
    PixelFormat_RG8Uint,
    PixelFormat_RG8Sint,
    // Packed 16-Bit formats
    PixelFormat_BGRA4Unorm,
    PixelFormat_B5G6R5Unorm,
    PixelFormat_BGR5A1Unorm,
    // 32-bit formats
    PixelFormat_R32Uint,
    PixelFormat_R32Sint,
    PixelFormat_R32Float,
    PixelFormat_RG16Unorm,
    PixelFormat_RG16Snorm,
    PixelFormat_RG16Uint,
    PixelFormat_RG16Sint,
    PixelFormat_RG16Float,
    PixelFormat_RGBA8Unorm,
    PixelFormat_RGBA8UnormSrgb,
    PixelFormat_RGBA8Snorm,
    PixelFormat_RGBA8Uint,
    PixelFormat_RGBA8Sint,
    PixelFormat_BGRA8Unorm,
    PixelFormat_BGRA8UnormSrgb,
    // Packed 32-Bit Pixel Formats
    PixelFormat_RGB10A2Unorm,
    PixelFormat_RGB10A2Uint,
    PixelFormat_RG11B10UFloat,
    PixelFormat_RGB9E5UFloat,
    // 64-bit formats
    PixelFormat_RG32Uint,
    PixelFormat_RG32Sint,
    PixelFormat_RG32Float,
    PixelFormat_RGBA16Unorm,
    PixelFormat_RGBA16Snorm,
    PixelFormat_RGBA16Uint,
    PixelFormat_RGBA16Sint,
    PixelFormat_RGBA16Float,
    // 128-bit formats
    PixelFormat_RGBA32Uint,
    PixelFormat_RGBA32Sint,
    PixelFormat_RGBA32Float,
    // Depth-stencil formats
    PixelFormat_Depth16Unorm,
    PixelFormat_Depth24UnormStencil8,
    PixelFormat_Depth32Float,
    PixelFormat_Depth32FloatStencil8,
    // BC compressed formats
    PixelFormat_BC1RGBAUnorm,
    PixelFormat_BC1RGBAUnormSrgb,
    PixelFormat_BC2RGBAUnorm,
    PixelFormat_BC2RGBAUnormSrgb,
    PixelFormat_BC3RGBAUnorm,
    PixelFormat_BC3RGBAUnormSrgb,
    PixelFormat_BC4RUnorm,
    PixelFormat_BC4RSnorm,
    PixelFormat_BC5RGUnorm,
    PixelFormat_BC5RGSnorm,
    PixelFormat_BC6HRGBUfloat,
    PixelFormat_BC6HRGBFloat,
    PixelFormat_BC7RGBAUnorm,
    PixelFormat_BC7RGBAUnormSrgb,
    // ETC2/EAC compressed formats
    PixelFormat_ETC2RGB8Unorm,
    PixelFormat_ETC2RGB8UnormSrgb,
    PixelFormat_ETC2RGB8A1Unorm,
    PixelFormat_ETC2RGB8A1UnormSrgb,
    PixelFormat_ETC2RGBA8Unorm,
    PixelFormat_ETC2RGBA8UnormSrgb,
    PixelFormat_EACR11Unorm,
    PixelFormat_EACR11Snorm,
    PixelFormat_EACRG11Unorm,
    PixelFormat_EACRG11Snorm,
    // ASTC compressed formats
    PixelFormat_ASTC4x4Unorm,
    PixelFormat_ASTC4x4UnormSrgb,
    PixelFormat_ASTC5x4Unorm,
    PixelFormat_ASTC5x4UnormSrgb,
    PixelFormat_ASTC5x5Unorm,
    PixelFormat_ASTC5x5UnormSrgb,
    PixelFormat_ASTC6x5Unorm,
    PixelFormat_ASTC6x5UnormSrgb,
    PixelFormat_ASTC6x6Unorm,
    PixelFormat_ASTC6x6UnormSrgb,
    PixelFormat_ASTC8x5Unorm,
    PixelFormat_ASTC8x5UnormSrgb,
    PixelFormat_ASTC8x6Unorm,
    PixelFormat_ASTC8x6UnormSrgb,
    PixelFormat_ASTC8x8Unorm,
    PixelFormat_ASTC8x8UnormSrgb,
    PixelFormat_ASTC10x5Unorm,
    PixelFormat_ASTC10x5UnormSrgb,
    PixelFormat_ASTC10x6Unorm,
    PixelFormat_ASTC10x6UnormSrgb,
    PixelFormat_ASTC10x8Unorm,
    PixelFormat_ASTC10x8UnormSrgb,
    PixelFormat_ASTC10x10Unorm,
    PixelFormat_ASTC10x10UnormSrgb,
    PixelFormat_ASTC12x10Unorm,
    PixelFormat_ASTC12x10UnormSrgb,
    PixelFormat_ASTC12x12Unorm,
    PixelFormat_ASTC12x12UnormSrgb,

    // MultiAspect format
    //PixelFormat_R8BG8Biplanar420Unorm,
    //PixelFormat_R10X6BG10X6Biplanar420Unorm,

    _PixelFormat_Count,
    _PixelFormat_Force32 = 0x7FFFFFFF
} PixelFormat;

typedef enum ImageDimension {
    /// One-dimensional Texture.
    ImageDimension_1D = 0,
    /// Two-dimensional Texture.
    ImageDimension_2D = 1,
    /// Three-dimensional Texture.
    ImageDimension_3D = 2,
    /// Cubemap Texture.
    ImageDimension_Cube = 3,

    _ImageDimensiont_Force32 = 0x7FFFFFFF
} ImageDimension;

typedef enum ImageFileFormat {
    ImageFileFormat_Bmp,
    ImageFileFormat_Png,
    ImageFileFormat_Jpg,
    ImageFileFormat_Tga,
    ImageFileFormat_Hdr,

    _ImageFileFormat_Force32 = 0x7FFFFFFF
} ImageFileFormat;

typedef struct ImageDesc {
    ImageDimension dimension;
    PixelFormat format;
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
    uint32_t mipLevelCount;
} ImageDesc;

ALIMER_IMAGE_API AlimerImage* alimerImageCreate2D(PixelFormat format, uint32_t width, uint32_t height, uint32_t arrayLayers, uint32_t mipLevelCount);
ALIMER_IMAGE_API AlimerImage* alimerImageCreateFromMemory(const uint8_t* pData, size_t dataSize);
ALIMER_IMAGE_API void alimerImageDestroy(AlimerImage* image);

ALIMER_IMAGE_API void alimerImageGetDesc(AlimerImage* image, ImageDesc* pDesc);
ALIMER_IMAGE_API ImageDimension AlimerImage_GetDimension(AlimerImage* image);
ALIMER_IMAGE_API PixelFormat AlimerImage_GetFormat(AlimerImage* image);
ALIMER_IMAGE_API uint32_t AlimerImage_GetWidth(AlimerImage* image, uint32_t level);
ALIMER_IMAGE_API uint32_t AlimerImage_GetHeight(AlimerImage* image, uint32_t level);
ALIMER_IMAGE_API uint32_t AlimerImage_GetDepth(AlimerImage* image, uint32_t level);
ALIMER_IMAGE_API uint32_t AlimerImage_GetArrayLayers(AlimerImage* image);
ALIMER_IMAGE_API uint32_t AlimerImage_GetMipLevelCount(AlimerImage* image);
ALIMER_IMAGE_API void* AlimerImage_GetData(AlimerImage* image, size_t* size);

typedef void (*AlimerImageSaveCallback)(AlimerImage* image, void* pData, uint32_t dataSize);

ALIMER_IMAGE_API bool alimerImageSave(AlimerImage* image, ImageFileFormat format, int quality, AlimerImageSaveCallback callback);

#endif /* _ALIMER_IMAGE_H */
