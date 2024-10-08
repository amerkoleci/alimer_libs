# Define target name
set (TARGET_NAME alimer_audio)

set(SOURCE_FILES
    include/alimer_audio.h
    src/alimer_audio.c
)

add_library(${TARGET_NAME} ${ALIMER_LIBRARY_TYPE} ${SOURCE_FILES})
alimer_setup_library(${TARGET_NAME})

target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_AUDIO_IMPLEMENTATION=1)
if (ALIMER_SHARED_LIBRARY)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_AUDIO_SHARED_LIBRARY=1)
endif ()

target_compile_definitions(${TARGET_NAME} PRIVATE ${COMPILER_DEFINES})
target_compile_options(${TARGET_NAME} PRIVATE ${COMPILER_SWITCHES})
target_link_options(${TARGET_NAME} PRIVATE ${LINKER_SWITCHES})

target_include_directories(${TARGET_NAME}
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(${TARGET_NAME} PRIVATE alimer_common)

if (ALIMER_INSTALL)
    install (FILES "include/alimer_audio.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
