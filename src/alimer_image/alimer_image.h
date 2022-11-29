// Copyright © Amer Koleci and Contributors.
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

#include <stddef.h>
#include <stdint.h>

typedef enum ImageFormat {
    IMAGE_FORMAT_R8,
    IMAGE_FORMAT_RG8,
    IMAGE_FORMAT_RGBA8,
    IMAGE_FORMAT_R16,
    IMAGE_FORMAT_RG16,
    IMAGE_FORMAT_RGBA16,
    IMAGE_FORMAT_R16F,
    IMAGE_FORMAT_RG16F,
    IMAGE_FORMAT_RGBA16F,
    IMAGE_FORMAT_R32F,
    IMAGE_FORMAT_RG32F,
    IMAGE_FORMAT_RGBA32F,
} ImageFormat;

typedef enum ImageDimension {
    IMAGE_DIMENSION_1D = 0,
    IMAGE_DIMENSION_2D = 1,
    IMAGE_DIMENSION_3D = 2,
} ImageDimension;

typedef struct ImageMetadata {
    ImageDimension dimension;
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
} ImageMetadata;

typedef uint32_t bool32;
typedef struct Image Image;

ALIMER_IMAGE_API Image* Image_FromMemory(const uint8_t* data, size_t size);
ALIMER_IMAGE_API void Image_Destroy(Image* image);

ALIMER_IMAGE_API void Image_GetMetadata(Image* image, ImageMetadata* pMetadata);

ALIMER_IMAGE_API uint32_t Image_GetNumLevels(Image* image);
ALIMER_IMAGE_API uint32_t Image_GetNumFaces(Image* image);
ALIMER_IMAGE_API ImageFormat Image_GetFormat(Image* image);
ALIMER_IMAGE_API bool32 Image_IsArray(Image* image);
ALIMER_IMAGE_API bool32 Image_IsCubemap(Image* image);

ALIMER_IMAGE_API uint8_t* Image_GetData(Image* image);
ALIMER_IMAGE_API uint32_t Image_GetDataSize(Image* image);

#endif /* _ALIMER_IMAGE_H */
