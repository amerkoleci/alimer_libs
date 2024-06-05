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
#define MA_ENGINE_MAX_LISTENERS ALIMER_AUDIO_MAX_LISTENERS
#include "third_party/miniaudio.h"
#include "third_party/miniaudio_libvorbis.h"
#include "third_party/miniaudio_qoa.h"
ALIMER_ENABLE_WARNINGS()

static struct {
    Bool32 initialized;
    AudioConfig config;
    ma_engine* engine;
} state;

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
