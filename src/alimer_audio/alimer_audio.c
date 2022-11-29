// Copyright © Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_audio.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <assert.h>

#define MA_API _ALIMER_AUDIO_EXPORT
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

static struct {
    bool initialized;
    ma_engine engine;
} state;

bool audio_init(const audio_config* config) {
    if (state.initialized)
        return false;

    ma_engine_config ma_config = ma_engine_config_init();
    ma_config.listenerCount = 1;
    ma_config.channels = config->channels;
    ma_config.sampleRate = config->sampleRate;

    ma_result result = ma_engine_init(&ma_config, &state.engine);
    if (result != MA_SUCCESS)
        return false;
    state.initialized = true;
    return true;
}

void audio_shutdown(void)
{
    if (!state.initialized)
        return;

    ma_engine_uninit(&state.engine);
}
