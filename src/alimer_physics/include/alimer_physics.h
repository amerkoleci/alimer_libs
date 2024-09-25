// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_PHYSICS_H
#define _ALIMER_PHYSICS_H

#if defined(ALIMER_PHYSICS_SHARED_LIBRARY)
#   if defined(_WIN32)
#       if defined(ALIMER_PHYSICS_IMPLEMENTATION)
#           define _ALIMER_PHYSICS_EXPORT __declspec(dllexport)
#       else
#           define _ALIMER_PHYSICS_EXPORT __declspec(dllimport)
#       endif
#   else
#       if defined(ALIMER_PHYSICS_IMPLEMENTATION)
#           define _ALIMER_PHYSICS_EXPORT __attribute__((visibility("default")))
#       else
#           define _ALIMER_PHYSICS_EXPORT
#       endif
#   endif
#else
#   define _ALIMER_PHYSICS_EXPORT
#endif

#ifdef __cplusplus
#   define ALIMER_PHYSICS_API extern "C" _ALIMER_PHYSICS_EXPORT 
#else
#   define ALIMER_PHYSICS_API extern _ALIMER_PHYSICS_EXPORT 
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct PhysicsWorld PhysicsWorld;
typedef struct PhysicsBody PhysicsBody;
typedef struct PhysicsMaterial PhysicsMaterial;

typedef struct WorldInfo {
    uint32_t maxBodies;
} WorldInfo;

ALIMER_PHYSICS_API bool alimerPhysicsInit(void);
ALIMER_PHYSICS_API void alimerPhysicsShutdown(void);

ALIMER_PHYSICS_API PhysicsWorld* alimerPhysicsWorldCreate(const WorldInfo* info);
ALIMER_PHYSICS_API void alimerPhysicsWorldDestroy(PhysicsWorld* world);

#endif /* _ALIMER_PHYSICS_H */
