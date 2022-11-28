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

struct Image {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    ImageFormat format;
    uint32_t numLevels;
    uint32_t flags;

    uint32_t dataSize;
    uint8_t* pData;
};

static Image* dds_load_from_memory(const uint8_t* data, uint32_t size)
{
    return NULL;
}

static Image* astc_load_from_memory(const uint8_t* data, uint32_t size)
{
    return NULL;
}

static Image* ktx_load_from_memory(const uint8_t* data, uint32_t size)
{
    return NULL;
}

static Image* stb_load_from_memory(const uint8_t* data, uint32_t size)
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
    else {
        image_data = stbi_load_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = IMAGE_FORMAT_RGBA8;
        memorySize = width * height * 4 * sizeof(uint8_t);
    }

    if (!data) {
        return NULL;
    }

    Image* result = (Image*)malloc(sizeof(Image));
    result->width = width;
    result->height = height;
    result->depth = 1;
    result->numLevels = 1;
    result->format = format;
    result->dataSize = memorySize;
    result->pData = (uint8_t*)malloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

Image* image_load_from_memory(const uint8_t* data, uint32_t size)
{
    Image* image = NULL;

    if ((image = dds_load_from_memory(data, size)) != NULL) return image;
    if ((image = astc_load_from_memory(data, size)) != NULL) return image;
    if ((image = ktx_load_from_memory(data, size)) != NULL) return image;
    if ((image = stb_load_from_memory(data, size)) != NULL) return image;

    return NULL;
}

void image_destroy(Image* image)
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
