// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_image.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <assert.h>

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
#include <ktx.h>

struct Image {
    ImageDimension dimension;
    uint32_t width;
    uint32_t height;
    uint32_t depthOrArrayLayers;
    ImageFormat format;
    uint32_t numLevels;
    uint32_t numFaces;
    bool32   isArray;
    bool32   isCubemap;

    uint32_t dataSize;
    uint8_t* pData;
};

static Image* Image_CreateNew(uint32_t width, uint32_t height, ImageFormat format) {
    Image* result = (Image*)malloc(sizeof(Image));
    assert(image);
    memset(result, 0, sizeof(Image));
    result->dimension = IMAGE_DIMENSION_2D;
    result->width = width;
    result->height = height;
    result->depthOrArrayLayers = 1;
    result->numLevels = 1;
    result->numFaces = 1;
    result->format = format;
    result->isArray = false;
    result->isCubemap = false;

    return result;
}

static Image* dds_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static Image* astc_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static Image* ktx_load_from_memory(const uint8_t* data, size_t size)
{
    ktxTexture* ktx_texture = 0;
    KTX_error_code ktx_result = ktxTexture_CreateFromMemory(
        data,
        size,
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &ktx_texture
    );

    // Not ktx texture.
    if (ktx_result != KTX_SUCCESS)
    {
        return NULL;
    }

    ImageFormat format = IMAGE_FORMAT_RGBA8;
    if (ktx_texture->classId == ktxTexture2_c)
    {
        ktxTexture2* ktx_texture2 = (ktxTexture2*)ktx_texture;

        if (ktxTexture2_NeedsTranscoding(ktx_texture2))
        {
            // Once transcoded, the ktxTexture object contains the texture data in a native GPU format (e.g. BC7)
            ktx_result = ktxTexture2_TranscodeBasis(ktx_texture2, KTX_TTF_BC7_RGBA, 0);
            //result->format = TEXTURE_FORMAT_BC7;
        }
        else
        {
        }
    }

    Image* result = Image_CreateNew(ktx_texture->baseWidth, ktx_texture->baseHeight, format);
    if (ktx_texture->baseDepth > 1)
    {
        result->dimension = IMAGE_DIMENSION_3D;
        result->depthOrArrayLayers = ktx_texture->baseDepth;
    }
    else
    {
        result->dimension = IMAGE_DIMENSION_2D;
        result->depthOrArrayLayers = ktx_texture->numLayers;
    }

    result->numLevels = ktx_texture->numLevels;
    result->numFaces = ktx_texture->numFaces;
    result->isArray = ktx_texture->isArray;
    result->isCubemap = ktx_texture->isCubemap;

    result->dataSize = (uint32_t)ktx_texture->dataSize;
    result->pData = (uint8_t*)malloc(ktx_texture->dataSize);
    memcpy(result->pData, ktx_texture->pData, ktx_texture->dataSize);
    ktxTexture_Destroy(ktx_texture);
    return result;
}

static Image* stb_load_from_memory(const uint8_t* data, size_t size)
{
    int width, height, channels;
    ImageFormat format = IMAGE_FORMAT_RGBA8;
    void* image_data;
    uint32_t memorySize = 0;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        image_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels)
        {
            case 1:
                format = IMAGE_FORMAT_R16;
                memorySize = width * height * sizeof(uint16_t);
                break;
            case 2:
                format = IMAGE_FORMAT_RG16;
                memorySize = width * height * 2 * sizeof(uint16_t);
                break;
            case 4:
                format = IMAGE_FORMAT_RGBA16;
                memorySize = width * height * 4 * sizeof(uint16_t);
                break;
            default:
                assert(0);
        }
    }
    else if (stbi_is_hdr_from_memory(data, (int)size))
    {
        image_data = stbi_loadf_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA32F;
        memorySize = width * height * 4 * sizeof(float);
    }
    else
    {
        image_data = stbi_load_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA8;
        memorySize = width * height * 4 * sizeof(uint8_t);
    }

    if (!image_data) {
        return NULL;
    }

    Image* result = Image_CreateNew(width, height, format);
    result->dataSize = memorySize;
    result->pData = (uint8_t*)malloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

Image* Image_FromMemory(const uint8_t* data, size_t size)
{
    Image* image = NULL;

    if ((image = dds_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = astc_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = ktx_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = stb_load_from_memory(data, size)) != NULL) {
        return image;
    }

    return NULL;
}

void Image_Destroy(Image* image)
{
    if (image)
    {
        if (image->pData)
        {
            free(image->pData);
        }

        free(image);
    }
}

void Image_GetMetadata(Image* image, ImageMetadata* pMetadata) {
    assert(image);
    assert(pMetadata);

    pMetadata->dimension = image->dimension;
    pMetadata->width = image->width;
    pMetadata->height = image->height;
    pMetadata->depthOrArrayLayers = image->depthOrArrayLayers;
}

uint32_t Image_GetNumLevels(Image* image) {
    return image->numLevels;
}

uint32_t Image_GetNumFaces(Image* image) {
    return image->numFaces;
}

ImageFormat Image_GetFormat(Image* image) {
    return image->format;
}

bool32 Image_IsArray(Image* image) {
    return image->isArray;
}

bool32 Image_IsCubemap(Image* image) {
    return image->isCubemap;
}

uint8_t* Image_GetData(Image* image) {
    return image->pData;
}

uint32_t Image_GetDataSize(Image* image) {
    return image->dataSize;
}
