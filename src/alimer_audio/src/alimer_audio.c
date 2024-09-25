// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_audio.h"
#include "alimer_internal.h"
#include <stdio.h>

ALIMER_DISABLE_WARNINGS()
#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_ALSA
#define MA_ENABLE_COREAUDIO
#define MA_ENABLE_OPENSL
#define MA_ENABLE_WEBAUDIO

//#define MA_ENGINE_MAX_LISTENERS ALIMER_AUDIO_MAX_LISTENERS
#include "third_party/miniaudio.h"
#include "third_party/miniaudio_libvorbis.h"
#include "third_party/miniaudio_qoa.h"
ALIMER_ENABLE_WARNINGS()

static Vector3 ToVector3(ma_vec3f v)
{
    Vector3 value = { v.x, v.y, v.z };
    return value;
}

static AudioFormat ToFormat(ma_format format)
{
    switch (format)
    {
        case ma_format_u8:
            return AudioFormat_U8;

        case ma_format_s16:
            return AudioFormat_S16;

        case ma_format_s24:
            return AudioFormat_S24;

        case ma_format_s32:
            return AudioFormat_S32;

        case ma_format_f32:
            return AudioFormat_F32;

        default:
            return AudioFormat_Unknown;
    }
}

// begin Vorbis
static ma_result ma_decoding_backend_init__libvorbis(void* pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void* pReadSeekTellUserData, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
{
    ma_result result;
    ma_libvorbis* pVorbis;

    (void)pUserData;

    pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
    if (pVorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_libvorbis_init(onRead, onSeek, onTell, pReadSeekTellUserData, pConfig, pAllocationCallbacks, pVorbis);
    if (result != MA_SUCCESS) {
        ma_free(pVorbis, pAllocationCallbacks);
        return result;
    }

    *ppBackend = pVorbis;

    return MA_SUCCESS;
}

static ma_result ma_decoding_backend_init_file__libvorbis(void* pUserData, const char* pFilePath, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
{
    ma_result result;
    ma_libvorbis* pVorbis;

    (void)pUserData;

    pVorbis = (ma_libvorbis*)ma_malloc(sizeof(*pVorbis), pAllocationCallbacks);
    if (pVorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_libvorbis_init_file(pFilePath, pConfig, pAllocationCallbacks, pVorbis);
    if (result != MA_SUCCESS) {
        ma_free(pVorbis, pAllocationCallbacks);
        return result;
    }

    *ppBackend = pVorbis;

    return MA_SUCCESS;
}

static void ma_decoding_backend_uninit__libvorbis(void* pUserData, ma_data_source* pBackend, const ma_allocation_callbacks* pAllocationCallbacks)
{
    ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

    (void)pUserData;

    ma_libvorbis_uninit(pVorbis, pAllocationCallbacks);
    ma_free(pVorbis, pAllocationCallbacks);
}

static ma_result ma_decoding_backend_get_channel_map__libvorbis(void* pUserData, ma_data_source* pBackend, ma_channel* pChannelMap, size_t channelMapCap)
{
    ma_libvorbis* pVorbis = (ma_libvorbis*)pBackend;

    (void)pUserData;

    return ma_libvorbis_get_data_format(pVorbis, NULL, NULL, NULL, pChannelMap, channelMapCap);
}

static ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis =
{
    ma_decoding_backend_init__libvorbis,
    ma_decoding_backend_init_file__libvorbis,
    NULL, /* onInitFileW() */
    NULL, /* onInitMemory() */
    ma_decoding_backend_uninit__libvorbis
};

// end Vorbis

// begin QOA

static ma_result ma_decoding_backend_init__qoa(void* pUserData, ma_read_proc onRead, ma_seek_proc onSeek, ma_tell_proc onTell, void* pReadSeekTellUserData, const ma_decoding_backend_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_data_source** ppBackend)
{
    ma_result result;
    ma_qoa* pQoa;

    (void)pUserData;

    pQoa = (ma_qoa*)ma_malloc(sizeof(*pQoa), pAllocationCallbacks);
    if (pQoa == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_qoa_init(onRead, onSeek, onTell, pReadSeekTellUserData, pConfig, pAllocationCallbacks, pQoa);
    if (result != MA_SUCCESS) {
        ma_free(pQoa, pAllocationCallbacks);
        return result;
    }

    *ppBackend = pQoa;

    return MA_SUCCESS;
}

static void ma_decoding_backend_uninit__qoa(void* pUserData, ma_data_source* pBackend, const ma_allocation_callbacks* pAllocationCallbacks)
{
    ma_qoa* pQoa = (ma_qoa*)pBackend;

    (void)pUserData;

    ma_qoa_uninit(pQoa, pAllocationCallbacks);
    ma_free(pQoa, pAllocationCallbacks);
}

static ma_result ma_decoding_backend_get_channel_map__qoa(void* pUserData, ma_data_source* pBackend, ma_channel* pChannelMap, size_t channelMapCap)
{
    ma_qoa* pQoa = (ma_qoa*)pBackend;

    (void)pUserData;

    return ma_qoa_get_data_format(pQoa, NULL, NULL, NULL, pChannelMap, channelMapCap);
}

static ma_decoding_backend_vtable g_ma_decoding_backend_vtable_qoa =
{
    ma_decoding_backend_init__qoa,
    NULL, /* onInitFile() */
    NULL, /* onInitFileW() */
    NULL, /* onInitMemory() */
    ma_decoding_backend_uninit__qoa
};

// end QOA

/*
Add your custom backend vtables here. The order in the array defines the order of priority. The
vtables will be passed in to the resource manager config.
*/
static ma_decoding_backend_vtable* pCustomBackendVTables[] =
{
    &g_ma_decoding_backend_vtable_qoa,
    &g_ma_decoding_backend_vtable_libvorbis,
};

void alimerAudioGetVersion(int* major, int* minor, int* patch)
{
    if (major) *major = ALIMER_AUDIO_VERSION_MAJOR;
    if (minor) *minor = ALIMER_AUDIO_VERSION_MINOR;
    if (patch) *patch = ALIMER_AUDIO_VERSION_PATCH;
}

static struct {
    Bool32 initialized;
    AudioConfig config;
    ma_engine* engine;
} state;

Bool32 alimerAudioInit(const AudioConfig* config)
{
    if (state.initialized) {
        return true;
    }

    state.engine = ma_malloc(sizeof(ma_engine), NULL);

    ma_resource_manager_config resourceManagerConfig;
    ma_resource_manager* resourceManager = ma_malloc(sizeof(ma_resource_manager), NULL);
    ma_engine_config engineConfig;

    /* Using custom decoding backends requires a resource manager. */
    resourceManagerConfig = ma_resource_manager_config_init();
    resourceManagerConfig.ppCustomDecodingBackendVTables = pCustomBackendVTables;
    resourceManagerConfig.customDecodingBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);
    resourceManagerConfig.pCustomDecodingBackendUserData = NULL;  /* <-- This will be passed in to the pUserData parameter of each function in the decoding backend vtables. */

    /* Using custom decoding backends requires a resource manager. */
    resourceManagerConfig = ma_resource_manager_config_init();
    resourceManagerConfig.ppCustomDecodingBackendVTables = pCustomBackendVTables;
    resourceManagerConfig.customDecodingBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);
    resourceManagerConfig.pCustomDecodingBackendUserData = NULL;  /* <-- This will be passed in to the pUserData parameter of each function in the decoding backend vtables. */

    if (ma_resource_manager_init(&resourceManagerConfig, resourceManager) != MA_SUCCESS)
    {
        //audioLogError("Unable to create Audio Engine (Resource Manager)");
        ma_free(state.engine, NULL);
        ma_free(resourceManager, NULL);
        return false;
    }

    /* Once we have a resource manager we can create the engine. */
    engineConfig = ma_engine_config_init();
    if (config->listenerCount != 0)
    {
        engineConfig.listenerCount = config->listenerCount;
    }
    engineConfig.channels = config->channels;
    engineConfig.sampleRate = config->sampleRate;
    engineConfig.pResourceManager = resourceManager;

    if (ma_engine_init(&engineConfig, state.engine) != MA_SUCCESS)
    {
        //audioLogError("Unable to create Audio Engine");
        ma_free(state.engine, NULL);
        ma_free(resourceManager, NULL);
        return false;
    }

    state.initialized = true;

    return true;
}

void alimerAudioShutdown(void)
{
    if (!state.initialized)
        return;

    ma_engine_uninit(state.engine);
    ma_free(state.engine, NULL);

    state.initialized = false;
}

Bool32 alimerAudioResume(void)
{
    return ma_engine_start(state.engine) == MA_SUCCESS;
}

Bool32 alimerAudioSuspend(void)
{
    return ma_engine_stop(state.engine) == MA_SUCCESS;
}

uint32_t alimerAudioGetChannels(void)
{
    return ma_engine_get_channels(state.engine);
}

uint32_t alimerAudioGetSampleRate(void)
{
    return ma_engine_get_sample_rate(state.engine);
}

float alimerAudioGetMasterVolume(void)
{
    return ma_engine_get_volume(state.engine);
}

Bool32 alimerAudioSetMasterVolume(float volume)
{
    return ma_engine_set_volume(state.engine, volume) == MA_SUCCESS;
}

uint64_t alimerAudioGetTimePcmFrames(void)
{
    return ma_engine_get_time_in_pcm_frames(state.engine);
}

void alimerAudioSetTimePcmFrames(uint64_t value)
{
    ma_engine_set_time_in_pcm_frames(state.engine, (ma_uint64)value);
}

uint32_t alimerAudioGetListenerCount(void)
{
    return ma_engine_get_listener_count(state.engine);
}

Bool32 alimerAudioListenerGetEnabled(uint32_t index)
{
    return ma_engine_listener_is_enabled(state.engine, index);
}

void alimerAudioListenerSetEnabled(uint32_t index, Bool32 value)
{
    ma_engine_listener_set_enabled(state.engine, index, value);
}

void alimerAudioListenerGetPosition(uint32_t index, Vector3* result)
{
    *result = ToVector3(ma_engine_listener_get_position(state.engine, index));
}

void alimerAudioListenerSetPosition(uint32_t index, const Vector3* value)
{
    ma_engine_listener_set_position(state.engine, index, value->x, value->y, value->z);
}

void alimerAudioListenerGetVelocity(uint32_t index, Vector3* result)
{
    *result = ToVector3(ma_engine_listener_get_velocity(state.engine, index));
}

void alimerAudioListenerSetVelocity(uint32_t index, const Vector3* value)
{
    ma_engine_listener_set_velocity(state.engine, index, value->x, value->y, value->z);
}

void alimerAudioListenerGetDirection(uint32_t index, Vector3* result)
{
    *result = ToVector3(ma_engine_listener_get_direction(state.engine, index));
}

void alimerAudioListenerSetDirection(uint32_t index, const Vector3* value)
{
    ma_engine_listener_set_direction(state.engine, index, value->x, value->y, value->z);
}

void alimerAudioListenerGetCone(uint32_t index, float* pInnerAngleInRadians, float* pOuterAngleInRadians, float* pOuterGain)
{
    ma_engine_listener_get_cone(state.engine, index, pInnerAngleInRadians, pOuterAngleInRadians, pOuterGain);
}

void alimerAudioListenerSetCone(uint32_t index, float innerAngleInRadians, float outerAngleInRadians, float outerGain)
{
    ma_engine_listener_set_cone(state.engine, index, innerAngleInRadians, outerAngleInRadians, outerGain);
}

void alimerAudioListenerGetWorldUp(uint32_t index, Vector3* result)
{
    *result = ToVector3(ma_engine_listener_get_world_up(state.engine, index));
}

void alimerAudioListenerSetWorldUp(uint32_t index, const Vector3* value)
{
    ma_engine_listener_set_world_up(state.engine, index, value->x, value->y, value->z);
}

void* alimerAudioDecode(void* data, size_t length, AudioFormat* format, uint32_t* channels, uint32_t* sampleRate, uint64_t* decodedFrameCount)
{
    void* frames = NULL;
    ma_decoder_config config = ma_decoder_config_init(*format, *channels, *sampleRate);
    config.pCustomBackendUserData = NULL;  /* In this example our backend objects are contained within a ma_decoder_ex object to avoid a malloc. Our vtables need to know about this. */
    config.ppCustomBackendVTables = pCustomBackendVTables;
    config.customBackendCount = sizeof(pCustomBackendVTables) / sizeof(pCustomBackendVTables[0]);

    ma_decode_memory(data, length, &config, decodedFrameCount, &frames);
    *format = ToFormat(config.format);
    *channels = config.channels;
    *sampleRate = config.sampleRate;
    return frames;
}

void alimerAudioFree(void* data)
{
    ma_free(data, NULL);
}

void alimerAudioRegisterEncodedData(const char* name, void* data, size_t length)
{
    ma_resource_manager* manager = ma_engine_get_resource_manager(state.engine);
    ma_resource_manager_register_encoded_data(manager, name, data, length);
}

void alimerAudioRegisterDecodedData(const char* name, const void* data, uint64_t frameCount, AudioFormat format, uint32_t channels, uint32_t sampleRate)
{
    ma_resource_manager* manager = ma_engine_get_resource_manager(state.engine);
    ma_resource_manager_register_decoded_data(manager, name, data, frameCount, format, channels, sampleRate);
}

void alimerAudioUnregisterData(const char* name)
{
    ma_resource_manager* manager = ma_engine_get_resource_manager(state.engine);
    ma_resource_manager_unregister_data(manager, name);
}

Sound* alimerSoundCreate(const char* path, uint32_t flags, SoundGroup* soundGroup)
{
    ma_sound* sound = ma_malloc(sizeof(ma_sound), NULL);

    if (MA_SUCCESS != ma_sound_init_from_file(state.engine, path, flags, (ma_sound_group*)soundGroup, NULL, sound))
    {
        //alimerLogError("Unable to create Sound from file");
        ma_free(sound, NULL);
        return NULL;
    }

    return (Sound*)sound;
}

void alimerSoundDestroy(Sound* sound)
{
    ma_sound_uninit((ma_sound*)sound);
    ma_free((ma_sound*)sound, NULL);
}

void alimerSoundPlay(Sound* sound)
{
    ma_sound_start((ma_sound*)sound);
}

void alimerSoundStop(Sound* sound)
{
    ma_sound_stop((ma_sound*)sound);
}

void alimerSoundGetDataFormat(Sound* sound, AudioFormat* format, uint32_t* channels, uint32_t* sampleRate)
{
    ma_sound_get_data_format((ma_sound*)sound, format, channels, sampleRate, NULL, 0);
}

uint64_t alimerSoundGetLengthPcmFrames(Sound* sound)
{
    ma_uint64 value = 0;
    ma_sound_get_length_in_pcm_frames((ma_sound*)sound, &value);
    return (uint64_t)value;
}

uint64_t alimerSoundGetCursorPcmFrames(Sound* sound)
{
    ma_uint64 value = 0;
    ma_sound_get_cursor_in_pcm_frames((ma_sound*)sound, &value);
    return (uint64_t)value;
}

void alimerSoundSetCursorPcmFrames(Sound* sound, uint64_t value)
{
    ma_sound_seek_to_pcm_frame((ma_sound*)sound, (ma_uint64)value);
}

Bool32 alimerSoundGetPlaying(Sound* sound)
{
    return ma_sound_is_playing((ma_sound*)sound);
}

Bool32 alimerSoundGetFinished(Sound* sound)
{
    return ma_sound_at_end((ma_sound*)sound);
}

Bool32 alimerSoundGetLooping(Sound* sound)
{
    return ma_sound_is_looping((ma_sound*)sound);
}

void alimerSoundSetLooping(Sound* sound, Bool32 value)
{
    ma_sound_set_looping((ma_sound*)sound, value);
}

float alimerSoundGetVolume(Sound* sound)
{
    return ma_sound_get_volume((ma_sound*)sound);
}

void alimerSoundSetVolume(Sound* sound, float value)
{
    ma_sound_set_volume((ma_sound*)sound, value);
}

float alimerSoundGetPitch(Sound* sound)
{
    return ma_sound_get_pitch((ma_sound*)sound);
}

void alimerSoundSetPitch(Sound* sound, float value)
{
    ma_sound_set_pitch((ma_sound*)sound, value);
}

float alimerSoundGetPan(Sound* sound)
{
    return ma_sound_get_pan((ma_sound*)sound);
}

void alimerSoundSetPan(Sound* sound, float value)
{
    ma_sound_set_pan((ma_sound*)sound, value);
}
