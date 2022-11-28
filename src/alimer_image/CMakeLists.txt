# Define target name
set (TARGET_NAME alimer_image)

set(SOURCE_FILES
    alimer_image.h
    alimer_image.c
)

add_library(${TARGET_NAME} ${SOURCE_FILES})
target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_IMAGE_IMPLEMENTATION=1)
if (BUILD_SHARED_LIBS)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_IMAGE_SHARED_LIBRARY=1)
endif ()

target_link_libraries(${TARGET_NAME} PRIVATE stb libktx)
