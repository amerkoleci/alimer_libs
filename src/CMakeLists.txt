# common headers
add_library(alimer_common INTERFACE)
target_include_directories(alimer_common INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/alimer_common")
target_sources(alimer_common INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/alimer_common/alimer_internal.h"
)

# Third party libs
add_subdirectory(third_party)

# image library
if (ALIMER_IMAGE)
    add_subdirectory(alimer_image)
endif()

# Audio
if (ALIMER_AUDIO)
    add_subdirectory(alimer_audio)
endif()

# Physics
if (ALIMER_PHYSICS)
    # Jolt
    CPMAddPackage(
        NAME JoltPhysics
        GITHUB_REPOSITORY jrouwe/JoltPhysics
        GIT_TAG f15b519d629bdad9645c3fcee3baddff1a02dda7
        GIT_SHALLOW ON
        SOURCE_SUBDIR Build
        OPTIONS
            "DOUBLE_PRECISION OFF"
            "GENERATE_DEBUG_SYMBOLS OFF"
            "ENABLE_INSTALL OFF"
            "ENABLE_ALL_WARNINGS OFF"
            "USE_STATIC_MSVC_RUNTIME_LIBRARY OFF"
    )
    set_property(TARGET Jolt PROPERTY FOLDER "ThirdParty")

    add_subdirectory(alimer_physics)
endif()
