# common headers
add_library(alimer_common INTERFACE)
target_include_directories(alimer_common INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/alimer_common")
target_sources(alimer_common INTERFACE 
    "${CMAKE_CURRENT_SOURCE_DIR}/alimer_common/alimer_internal.h"
)

# image library
add_subdirectory(alimer_image)
add_subdirectory(alimer_audio)
