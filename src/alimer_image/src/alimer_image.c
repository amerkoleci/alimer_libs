// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_image.h"
#include "alimer_internal.h"
#include <stdio.h>

ALIMER_DISABLE_WARNINGS()
#include <stb_image.h>

#define STBIW_ASSERT(x) ALIMER_ASSERT(x)
//#define STBIW_MALLOC(sz) alimer_alloc(sz)
//#define STBIW_REALLOC(p, newsz) alimer_realloc(p, newsz)
//#define STBIW_FREE(p) alimer_free(p)
#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "third_party/tinyexr.h"
#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION
//#define QOI_MALLOC(sz) STBI_MALLOC(sz)
//#define QOI_FREE(p) STBI_FREE(p) 
#include "third_party/qoi.h"
#if defined(ALIMER_IMAGE_KTX)
#include <ktx.h>
#endif
ALIMER_ENABLE_WARNINGS()

struct AlimerImage {
    ImageDesc desc;
    size_t dataSize;
    void* pData;
};

static AlimerImage* DDS_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    ALIMER_UNUSED(pData);
    ALIMER_UNUSED(dataSize);

    return NULL;
}

static AlimerImage* ASTC_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    ALIMER_UNUSED(pData);
    ALIMER_UNUSED(dataSize);

    return NULL;
}

#if defined(ALIMER_IMAGE_KTX)
static AlimerImage* KTX_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    ktxTexture* ktx_texture = 0;
    KTX_error_code ktx_result = ktxTexture_CreateFromMemory(
        pData,
        dataSize,
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &ktx_texture
    );

    // Not ktx texture.
    if (ktx_result != KTX_SUCCESS)
    {
        return NULL;
    }

    PixelFormat format = PixelFormat_RGBA8Unorm;
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
            // KTX1
            ktxTexture1* ktx_texture1 = (ktxTexture1*)ktx_texture;

            //format = FromVkFormat(vkGetFormatFromOpenGLInternalFormat(ktx_texture1->glInternalformat));

            // KTX-1 files don't contain color space information. Color data is normally
            // in sRGB, but the format we get back won't report that, so this will adjust it
            // if necessary.
            //format = LinearToSrgbFormat(format);
        }
    }

    AlimerImage* result = NULL;
    if (ktx_texture->baseDepth > 1)
    {
        //Initialize3D(format, ktxTexture->baseWidth, ktxTexture->baseHeight, ktxTexture->baseDepth, ktxTexture->numLevels);
    }
    else if (ktx_texture->isCubemap && !ktx_texture->isArray)
    {
        //InitializeCube(format, ktxTexture->baseWidth, ktxTexture->baseHeight, ktxTexture->numFaces, ktxTexture->numLevels);
    }
    else
    {
        result = alimerImageCreate2D(format,
            ktx_texture->baseWidth,
            ktx_texture->baseHeight,
            ktx_texture->isArray ? ktx_texture->numLayers : 1u,
            ktx_texture->numLevels);
    }

#if TODO
    // If the texture contains more than one layer, then populate the offsets otherwise take the mipmap level offsets
    if (ktx_texture->isCubemap || ktx_texture->isArray)
    {
        uint32_t layerCount = ktx_texture->isCubemap ? ktx_texture->numFaces : ktx_texture->numLayers;

        for (uint32_t layer = 0; layer < layerCount; layer++)
        {
            for (uint32_t miplevel = 0; miplevel < ktx_texture->numLevels; miplevel++)
            {
                ktx_size_t     offset;
                KTX_error_code result;
                if (ktx_texture->isCubemap)
                {
                    result = ktxTexture_GetImageOffset(ktx_texture, miplevel, 0, layer, &offset);
                }
                else
                {
                    result = ktxTexture_GetImageOffset(ktx_texture, miplevel, layer, 0, &offset);
                }

                if (result != KTX_SUCCESS)
                {
                    //LOGF("Error loading KTX texture");
                }

                auto levelSize = ktxTexture_GetImageSize(ktx_texture, miplevel);
                auto levelData = GetLevel(miplevel, layer);
                memcpy(levelData->pixels, ktx_texture->pData + offset, levelSize);
            }
        }
    }
    else
    {
        for (uint32_t miplevel = 0; miplevel < ktx_texture->numLevels; miplevel++)
        {
            ktx_size_t     offset;
            KTX_error_code result;
            result = ktxTexture_GetImageOffset(ktx_texture, miplevel, 0, 0, &offset);
            if (result != KTX_SUCCESS)
            {
                //LOGF("Error loading KTX texture");
            }

            auto levelSize = ktxTexture_GetImageSize(ktx_texture, miplevel);
            auto levelData = GetLevel(miplevel);
            memcpy(levelData->pixels, ktx_texture->pData + offset, levelSize);
        }
    }
#endif // TODO


    result->dataSize = (uint32_t)ktx_texture->dataSize;
    result->pData = malloc(ktx_texture->dataSize);
    memcpy(result->pData, ktx_texture->pData, ktx_texture->dataSize);
    ktxTexture_Destroy(ktx_texture);
    return result;
}
#endif /* defined(ALIMER_KTX) */

static AlimerImage* EXR_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    if (!IsEXRFromMemory(pData, dataSize))
        return NULL;

    float* pixelData;
    int width, height;
    const char* err = NULL;
    int ret = LoadEXRFromMemory(&pixelData, &width, &height, pData, dataSize, &err);
    if (ret != TINYEXR_SUCCESS)
    {
        if (err)
        {
            //std::cerr << "Could not load EXR file '" << path << "': " << err << std::endl;
            FreeEXRErrorMessage(err); // release memory of error message.
        }

        return NULL;
    }

    // TODO: Allow conversion  to 16-bit (https://eliemichel.github.io/LearnWebGPU/advanced-techniques/hdr-textures.html)
    AlimerImage* image = alimerImageCreate2D(PixelFormat_RGBA32Float, width, height, 1, 1);
    image->dataSize = width * height * 4 * sizeof(float);
    image->pData = malloc(image->dataSize);
    memcpy(image->pData, pixelData, image->dataSize);
    free(pixelData);

    return image;
}

bool AlimerImage_TestQOI(const uint8_t* data, size_t size)
{
    if (size < QOI_HEADER_SIZE)
        return false;

    int p = 0;
    unsigned int magic = qoi_read_32(data, &p);
    if (magic != QOI_MAGIC)
        return false;

    return true;
}

static AlimerImage* QOI_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    if (!AlimerImage_TestQOI(pData, dataSize))
        return NULL;

    int channels = 4;
    qoi_desc qoi_desc;
    void* result = qoi_decode(pData, (int)dataSize, &qoi_desc, channels);

    if (result != NULL)
    {
        AlimerImage* image = alimerImageCreate2D(PixelFormat_RGBA8Unorm, qoi_desc.width, qoi_desc.height, 1u, 1u);
        image->dataSize = qoi_desc.width * qoi_desc.height * channels * sizeof(uint8_t);
        image->pData = malloc(image->dataSize);
        memcpy(image->pData, result, image->dataSize);

        free(result);
        return image;
    }

    return NULL;
}

static AlimerImage* STB_LoadFromMemory(const uint8_t* pData, size_t dataSize)
{
    int width, height, channels;
    PixelFormat format = PixelFormat_RGBA8Unorm;
    void* image_data;
    uint32_t memorySize = 0;
    if (stbi_is_16_bit_from_memory(pData, (int)dataSize))
    {
        image_data = stbi_load_16_from_memory(pData, (int)dataSize, &width, &height, &channels, 0);
        switch (channels)
        {
            case 1:
                format = PixelFormat_R16Uint;
                memorySize = width * height * sizeof(uint16_t);
                break;
            case 2:
                format = PixelFormat_RG16Uint;
                memorySize = width * height * 2 * sizeof(uint16_t);
                break;
            case 4:
                format = PixelFormat_RGBA16Uint;
                memorySize = width * height * 4 * sizeof(uint16_t);
                break;
            default:
                assert(0);
        }
    }
    else if (stbi_is_hdr_from_memory(pData, (int)dataSize))
    {
        // TODO: Allow conversion  to 16-bit (https://eliemichel.github.io/LearnWebGPU/advanced-techniques/hdr-textures.html)
        image_data = stbi_loadf_from_memory(pData, (int)dataSize, &width, &height, NULL, 4);
        format = PixelFormat_RGBA32Float;
        memorySize = width * height * 4 * sizeof(float);
    }
    else
    {
        image_data = stbi_load_from_memory(pData, (int)dataSize, &width, &height, NULL, 4);
        format = PixelFormat_RGBA8Unorm;
        memorySize = width * height * 4 * sizeof(uint8_t);
    }

    if (!image_data) {
        return NULL;
    }

    AlimerImage* result = alimerImageCreate2D(format, width, height, 1u, 1u);
    result->dataSize = memorySize;
    result->pData = malloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

AlimerImage* alimerImageCreate2D(PixelFormat format, uint32_t width, uint32_t height, uint32_t arrayLayers, uint32_t mipLevelCount)
{
    if (format == PixelFormat_Undefined || !width || !height || !arrayLayers)
        return NULL;

    AlimerImage* image = (AlimerImage*)malloc(sizeof(AlimerImage));
    assert(image);
    memset(image, 0, sizeof(AlimerImage));

    image->desc.dimension = ImageDimension_2D;
    image->desc.format = format;
    image->desc.width = width;
    image->desc.height = height;
    image->desc.depthOrArrayLayers = arrayLayers;
    image->desc.mipLevelCount = mipLevelCount;

    return image;
}

AlimerImage* alimerImageCreateFromMemory(const uint8_t* pData, size_t dataSize)
{
    AlimerImage* image = NULL;

    if ((image = DDS_LoadFromMemory(pData, dataSize)) != NULL)
        return image;

    if ((image = ASTC_LoadFromMemory(pData, dataSize)) != NULL)
        return image;

#if defined(ALIMER_IMAGE_KTX)
    if ((image = KTX_LoadFromMemory(pData, dataSize)) != NULL)
        return image;
#endif

    if ((image = EXR_LoadFromMemory(pData, dataSize)) != NULL)
        return image;

    if ((image = QOI_LoadFromMemory(pData, dataSize)) != NULL)
        return image;

    if ((image = STB_LoadFromMemory(pData, dataSize)) != NULL)
        return image;

    return NULL;
}

void alimerImageDestroy(AlimerImage* image)
{
    if (!image)
        return;

    if (image->pData)
        free(image->pData);

    free(image);
}

void alimerImageGetDesc(AlimerImage* image, ImageDesc* pDesc)
{
    ALIMER_ASSERT(image);
    ALIMER_ASSERT(pDesc);

    *pDesc = image->desc;
}

ImageDimension AlimerImage_GetDimension(AlimerImage* image)
{
    return image->desc.dimension;
}

PixelFormat AlimerImage_GetFormat(AlimerImage* image)
{
    return image->desc.format;
}

uint32_t AlimerImage_GetWidth(AlimerImage* image, uint32_t level)
{
    return ALIMER_MAX(image->desc.width >> level, 1);
}

uint32_t AlimerImage_GetHeight(AlimerImage* image, uint32_t level)
{
    return ALIMER_MAX(image->desc.height >> level, 1);
}

uint32_t AlimerImage_GetDepth(AlimerImage* image, uint32_t level)
{
    if (image->desc.dimension != ImageDimension_3D) {
        return 1u;
    }

    return ALIMER_MAX(image->desc.depthOrArrayLayers >> level, 1);
}

uint32_t AlimerImage_GetArrayLayers(AlimerImage* image)
{
    if (image->desc.dimension == ImageDimension_3D) {
        return 1u;
    }

    return image->desc.depthOrArrayLayers;
}

uint32_t AlimerImage_GetMipLevelCount(AlimerImage* image)
{
    return image->desc.mipLevelCount;
}

void* AlimerImage_GetData(AlimerImage* image, size_t* size)
{
    if (size)
        *size = image->dataSize;

    return image->pData;
}

bool alimerImageSave(AlimerImage* image, ImageFileFormat format, int quality, AlimerImageSaveCallback callback)
{
    int res = 0;
    switch (format)
    {
        case ImageFileFormat_Bmp:
            res = stbi_write_bmp_to_func((stbi_write_func*)callback, image, image->desc.width, image->desc.height, 4, image->pData);
            break;
        case ImageFileFormat_Png:
            res = stbi_write_png_to_func((stbi_write_func*)callback, image, image->desc.width, image->desc.height, 4, image->pData, image->desc.width * 4);
            break;
        case ImageFileFormat_Jpg:
            res = stbi_write_jpg_to_func((stbi_write_func*)callback, image, image->desc.width, image->desc.height, 4, image->pData, quality);
            break;
        case ImageFileFormat_Tga:
            res = stbi_write_tga_to_func((stbi_write_func*)callback, image, image->desc.width, image->desc.height, 4, image->pData);
            break;
        case ImageFileFormat_Hdr:
            res = stbi_write_hdr_to_func((stbi_write_func*)callback, image, image->desc.width, image->desc.height, 4, (const float*)image->pData);
            break;

        default:
            return false;
    }

    if (res != 0)
        return true;

    return false;
}
