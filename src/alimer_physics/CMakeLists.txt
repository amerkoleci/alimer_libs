# Define target name
set (TARGET_NAME alimer_physics)

set(SOURCE_FILES
    include/alimer_physics.h
    src/alimer_physics.cpp
)

add_library(${TARGET_NAME} ${ALIMER_LIBRARY_TYPE} ${SOURCE_FILES})

target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_PHYSICS_IMPLEMENTATION=1)
if (ALIMER_SHARED_LIBRARY)
    target_compile_definitions (${TARGET_NAME} PRIVATE ALIMER_PHYSICS_SHARED_LIBRARY=1)
endif ()

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_definitions(${TARGET_NAME} PRIVATE ${COMPILER_DEFINES})
target_compile_options(${TARGET_NAME} PRIVATE ${COMPILER_SWITCHES})
target_link_options(${TARGET_NAME} PRIVATE ${LINKER_SWITCHES})

target_link_libraries(${TARGET_NAME} PRIVATE Jolt)

if (ALIMER_INSTALL)
    install (FILES "include/alimer_physics.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS ${TARGET_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif ()
