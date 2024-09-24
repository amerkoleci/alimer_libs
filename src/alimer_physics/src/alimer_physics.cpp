// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_physics.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Jolt/Core/Core.h>

JPH_SUPPRESS_WARNING_PUSH
JPH_SUPPRESS_WARNINGS
#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"

// STL includes
#include <iostream>
#include <cstdarg>
#include <thread>

using namespace JPH;
JPH_SUPPRESS_WARNING_POP

static void TraceImpl(const char* fmt, ...)
{
    // Format the message
    va_list list;
    va_start(list, fmt);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, list);
    va_end(list);

    // Print to the TTY
    //if (s_TraceFunc)
    //{
    //    s_TraceFunc(buffer);
    //}
    //else
    {
        std::cout << buffer << std::endl;
    }
}

void* alimer_calloc(size_t size)
{
    void* data = calloc(1, size);
    if (!data)
        abort();
    return data;
}

void alimer_free(void* ptr)
{
    free(ptr);
}

static struct
{
    bool initialized;
    // 10 MB was not enough for large simulation, let's use TempAllocatorMalloc
    TempAllocator* tempAllocator;
    JobSystemThreadPool* jobSystem;
} state = {};

struct PhysicsWorld
{
    uint32_t refCount;
};

bool alimerPhysicsInit(void)
{
    if (state.initialized)
        return true;

    JPH::RegisterDefaultAllocator();

    // TODO
    JPH::Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = AssertFailedImpl;)

        // Create a factory
        JPH::Factory::sInstance = new JPH::Factory();

    // Register all Jolt physics types
    JPH::RegisterTypes();

    // Init temp allocator
    state.tempAllocator = new TempAllocatorImplWithMallocFallback(8 * 1024 * 1024);

    // Init Job system.
    state.jobSystem = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, (int)std::thread::hardware_concurrency() - 1);

    return true;

    return state.initialized = true;
}

void alimerPhysicsShutdown(void)
{
    if (!state.initialized)
        return;

    delete state.jobSystem; state.jobSystem = nullptr;
    delete state.tempAllocator; state.tempAllocator = nullptr;

    // Unregisters all types with the factory and cleans up the default material
    JPH::UnregisterTypes();

    // Destroy the factory
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;

    state.initialized = false;
    memset(&state, 0, sizeof(state));
}

PhysicsWorld* alimerPhysicsWorldCreate(const WorldInfo* info)
{
    PhysicsWorld* world = (PhysicsWorld*)alimer_calloc(sizeof(PhysicsWorld));
    world->refCount = 1;

    return world;
}

void alimerPhysicsWorldDestroy(PhysicsWorld* world)
{
    alimer_free(world);
}
