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

typedef uint32_t Bool32;

typedef enum AudioFormat
{
    AudioFormat_Unknown = 0,
    AudioFormat_U8 = 1,
    AudioFormat_S16 = 2,
    AudioFormat_S24 = 3,
    AudioFormat_S32 = 4,
    AudioFormat_F32 = 5,

    _AudioFormat_Count,
    _AudioFormat_Force32 = 0x7fffffff
} AudioFormat;

typedef enum SoundFlags
{
    SoundFlags_None = 0,
    SoundFlags_Stream = 0x00000001,
    SoundFlags_Decode = 0x00000002,
    SoundFlags_NoSpatialization = 0x00004000,

    _SoundFlags_Count,
    _SoundFlags_Force32 = 0x7fffffff
} SoundFlags;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct AudioConfig {
    uint32_t listenerCount;
    uint32_t channels;
    uint32_t sampleRate;
} AudioConfig;

typedef struct AudioEngineImpl* AudioEngine;
typedef struct Sound Sound;
typedef struct SoundGroup SoundGroup;

ALIMER_AUDIO_API void alimerAudioGetVersion(int* major, int* minor, int* patch);

ALIMER_AUDIO_API Bool32 alimerAudioInit(const AudioConfig* config);
ALIMER_AUDIO_API void alimerAudioShutdown(void);
ALIMER_AUDIO_API Bool32 alimerAudioResume(void);
ALIMER_AUDIO_API Bool32 alimerAudioSuspend(void);

ALIMER_AUDIO_API uint32_t alimerAudioGetChannels(void);
ALIMER_AUDIO_API uint32_t alimerAudioGetSampleRate(void);
ALIMER_AUDIO_API float alimerAudioGetMasterVolume(void);
ALIMER_AUDIO_API Bool32 alimerAudioSetMasterVolume(float volume);
ALIMER_AUDIO_API uint64_t alimerAudioGetTimePcmFrames(void);
ALIMER_AUDIO_API void alimerAudioSetTimePcmFrames(uint64_t value);
ALIMER_AUDIO_API uint32_t alimerAudioGetListenerCount(void);

ALIMER_AUDIO_API Bool32 alimerAudioListenerGetEnabled(uint32_t index);
ALIMER_AUDIO_API void alimerAudioListenerSetEnabled(uint32_t index, Bool32 value);
ALIMER_AUDIO_API void alimerAudioListenerGetPosition(uint32_t index, Vector3* result);
ALIMER_AUDIO_API void alimerAudioListenerSetPosition(uint32_t index, const Vector3* value);
ALIMER_AUDIO_API void alimerAudioListenerGetVelocity(uint32_t index, Vector3* result);
ALIMER_AUDIO_API void alimerAudioListenerSetVelocity(uint32_t index, const Vector3* value);
ALIMER_AUDIO_API void alimerAudioListenerGetDirection(uint32_t index, Vector3* result);
ALIMER_AUDIO_API void alimerAudioListenerSetDirection(uint32_t index, const Vector3* value);
ALIMER_AUDIO_API void alimerAudioListenerGetCone(uint32_t index, float* pInnerAngleInRadians, float* pOuterAngleInRadians, float* pOuterGain);
ALIMER_AUDIO_API void alimerAudioListenerSetCone(uint32_t index, float innerAngleInRadians, float outerAngleInRadians, float outerGain);
ALIMER_AUDIO_API void alimerAudioListenerGetWorldUp(uint32_t index, Vector3* result);
ALIMER_AUDIO_API void alimerAudioListenerSetWorldUp(uint32_t index, const Vector3* value);

ALIMER_AUDIO_API void* alimerAudioDecode(void* data, size_t length, AudioFormat* format, uint32_t* channels, uint32_t* sampleRate, uint64_t* decodedFrameCount);
ALIMER_AUDIO_API void alimerAudioFree(void* data);

ALIMER_AUDIO_API void alimerAudioRegisterEncodedData(const char* name, void* data, size_t length);
ALIMER_AUDIO_API void alimerAudioRegisterDecodedData(const char* name, const void* data, uint64_t frameCount, AudioFormat format, uint32_t channels, uint32_t sampleRate);
ALIMER_AUDIO_API void alimerAudioUnregisterData(const char* name);

ALIMER_AUDIO_API Sound* alimerSoundCreate(const char* path, uint32_t flags, SoundGroup* soundGroup);
ALIMER_AUDIO_API void alimerSoundDestroy(Sound* sound);
ALIMER_AUDIO_API void alimerSoundPlay(Sound* sound);
ALIMER_AUDIO_API void alimerSoundStop(Sound* sound);

ALIMER_AUDIO_API void alimerSoundGetDataFormat(Sound* sound, AudioFormat* format, uint32_t* channels, uint32_t* sampleRate);
ALIMER_AUDIO_API uint64_t alimerSoundGetLengthPcmFrames(Sound* sound);
ALIMER_AUDIO_API uint64_t alimerSoundGetCursorPcmFrames(Sound* sound);
ALIMER_AUDIO_API void alimerSoundSetCursorPcmFrames(Sound* sound, uint64_t value);

ALIMER_AUDIO_API Bool32 alimerSoundGetPlaying(Sound* sound);
ALIMER_AUDIO_API Bool32 alimerSoundGetFinished(Sound* sound);
ALIMER_AUDIO_API Bool32 alimerSoundGetLooping(Sound* sound);
ALIMER_AUDIO_API void alimerSoundSetLooping(Sound* sound, Bool32 value);
ALIMER_AUDIO_API float alimerSoundGetVolume(Sound* sound);
ALIMER_AUDIO_API void alimerSoundSetVolume(Sound* sound, float value);
ALIMER_AUDIO_API float alimerSoundGetPitch(Sound* sound);
ALIMER_AUDIO_API void alimerSoundSetPitch(Sound* sound, float value);
ALIMER_AUDIO_API float alimerSoundGetPan(Sound* sound);
ALIMER_AUDIO_API void alimerSoundSetPan(Sound* sound, float value);


#endif /* _ALIMER_IMAGE_H */
