# Define target name
set (TARGET_NAME alimer_audio)

file (GLOB SOURCE_FILES *.h *.c *.cpp)

add_library(${TARGET_NAME} ${SOURCE_FILES})
target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_AUDIO_IMPLEMENTATION=1)
if (BUILD_SHARED_LIBS)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_AUDIO_SHARED_LIBRARY=1)
endif ()
