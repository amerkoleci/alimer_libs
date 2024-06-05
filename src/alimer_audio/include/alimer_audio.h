// Copyright (c) Amer Koleci and Contributors.
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

#include <stddef.h>
#include <stdint.h>

/* Defines */
#define ALIMER_AUDIO_VERSION_MAJOR	1
#define ALIMER_AUDIO_VERSION_MINOR	0
#define ALIMER_AUDIO_VERSION_PATCH	0

#ifndef ALIMER_AUDIO_MAX_LISTENERS
#define ALIMER_AUDIO_MAX_LISTENERS 4
#endif

typedef uint32_t Bool32;

typedef struct AudioConfig {
    uint32_t listenerCount;
    uint32_t channels;
    uint32_t sampleRate;
} AudioConfig;

ALIMER_AUDIO_API void alimerAudioGetVersion(int* major, int* minor, int* patch);

ALIMER_AUDIO_API Bool32 alimerAudioInit(const AudioConfig* config);
ALIMER_AUDIO_API void alimerAudioShutdown(void);
ALIMER_AUDIO_API Bool32 alimerAudioResume(void);
ALIMER_AUDIO_API Bool32 alimerAudioSuspend(void);

#endif /* _ALIMER_IMAGE_H */
