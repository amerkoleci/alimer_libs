# Define target name
set (TARGET_NAME alimer_image)

set(SOURCE_FILES
    include/alimer_image.h
    src/alimer_image.cpp
)

add_library(${TARGET_NAME} ${ALIMER_LIBRARY_TYPE} ${SOURCE_FILES})
alimer_setup_library(${TARGET_NAME})

target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_IMAGE_IMPLEMENTATION=1)
if (ALIMER_SHARED_LIBRARY)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_IMAGE_SHARED_LIBRARY=1)
endif ()

target_include_directories(${TARGET_NAME}
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

#target_link_libraries(${TARGET_NAME} PRIVATE alimer_common libktx)
target_link_libraries(${TARGET_NAME} PRIVATE alimer_common)

if (ALIMER_INSTALL)
    install (FILES "include/alimer_image.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
