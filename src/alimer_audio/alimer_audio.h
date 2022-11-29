// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_AUDIO_H
#define _ALIMER_AUDIO_H

#if defined(ALIMER_AUDIO_SHARED_LIBRARY)
#    if defined(_WIN32)
#        if defined(ALIMER_AUDIO_IMPLEMENTATION)
#            define _ALIMER_AUDIO_EXPORT __declspec(dllexport)
#        else
#            define _ALIMER_AUDIO_EXPORT __declspec(dllimport)
#        endif
#    else
#        if defined(ALIMER_AUDIO_IMPLEMENTATION)
#            define _ALIMER_AUDIO_EXPORT __attribute__((visibility("default")))
#        else
#            define _ALIMER_AUDIO_EXPORT
#        endif
#    endif
#else
#    define _ALIMER_AUDIO_EXPORT
#endif

#ifdef __cplusplus
#    define _ALIMER_AUDIO_EXTERN extern "C"
#else
#    define _ALIMER_AUDIO_EXTERN extern
#endif

#define ALIMER_AUDIO_API _ALIMER_AUDIO_EXTERN _ALIMER_AUDIO_EXPORT 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct audio_config {
    uint32_t channels;
    uint32_t sampleRate;
} audio_config;

ALIMER_AUDIO_API bool audio_init(const audio_config* config);
ALIMER_AUDIO_API void audio_shutdown(void);

#endif /* _ALIMER_IMAGE_H */
