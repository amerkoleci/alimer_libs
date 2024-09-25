// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_internal.h"
#include <stdio.h>

ALIMER_DISABLE_WARNINGS()
#define STBIW_ASSERT(x) ALIMER_ASSERT(x)
//#define STBIW_MALLOC(sz) alimer_alloc(sz)
//#define STBIW_REALLOC(p, newsz) alimer_realloc(p, newsz)
//#define STBIW_FREE(p) alimer_free(p)
#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define TINYEXR_USE_MINIZ 0
#define TINYEXR_USE_STB_ZLIB 1
#define TINYEXR_IMPLEMENTATION
#include "third_party/tinyexr.h"
ALIMER_ENABLE_WARNINGS()
