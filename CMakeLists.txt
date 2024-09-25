# CMake version
cmake_minimum_required(VERSION 3.24.0 FATAL_ERROR)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Only 64 bit is supported
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
	message(FATAL_ERROR "32 bit isn't supported")
    return()
endif()

# Determine if engine is built as a subproject (using add_subdirectory)
# or if it is the master project.
if (NOT DEFINED ALIMER_MASTER_PROJECT)
    set(ALIMER_MASTER_PROJECT OFF)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(ALIMER_MASTER_PROJECT ON)
        message(STATUS "CMake version: ${CMAKE_VERSION}")
    endif ()
endif ()

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Configure CMake global variables
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_INSTALL_MESSAGE LAZY)
set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set (CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
set (CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for OS X")

# Build version
set (ALIMER_VERSION_MAJOR 1)
set (ALIMER_VERSION_MINOR 0)
set (ALIMER_VERSION_PATCH 0)
set (ALIMER_VERSION ${ALIMER_VERSION_MAJOR}.${ALIMER_VERSION_MINOR}.${ALIMER_VERSION_PATCH})

# Project declaration
project (alimer_libs
    VERSION ${ALIMER_VERSION}
    DESCRIPTION "Collection of C/C++ native libraries"
    HOMEPAGE_URL "https://github.com/amerkoleci/alimer_libs"
    LANGUAGES C CXX
)

# Set the default CMAKE_BUILD_TYPE to Release.
# This should be done before the project command since the latter can set
# CMAKE_BUILD_TYPE itself (it does so for nmake).
if (ALIMER_MASTER_PROJECT AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "The default build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endif ()

# Options
if (IOS OR EMSCRIPTEN)
    set(ALIMER_SHARED_LIBRARY OFF CACHE BOOL "Disable shared library on (IOS, WEB)" FORCE)
elseif(ANDROID)
    set(ALIMER_SHARED_LIBRARY ON CACHE BOOL "Always enable shared library on (ANDROID)" FORCE)    
else()
    option(ALIMER_SHARED_LIBRARY "Build as shared library" ON)
endif ()

option(ALIMER_INSTALL "Generate the install target" ${ALIMER_MASTER_PROJECT})
option(ALIMER_SAMPLES "Build samples" ${ALIMER_MASTER_PROJECT})
option(ALIMER_IMAGE "Build image library" ON)
option(ALIMER_IMAGE_KTX "Build image with ktx support" ON)
option(ALIMER_AUDIO "Build audio library" ON)
option(ALIMER_PHYSICS "Build physics library" ON)

if (ALIMER_SHARED_LIBRARY)
    set(ALIMER_LIBRARY_TYPE SHARED)
else ()
    set(ALIMER_LIBRARY_TYPE STATIC)
endif ()

# Include common utilities
include(GNUInstallDirs)
include(cmake/CPM.cmake)
include(cmake/AlimerUtils.cmake)

execute_process(COMMAND git rev-list --count HEAD
    OUTPUT_VARIABLE ALIMER_REVISION_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if ("${ALIMER_REVISION_ID}" STREQUAL "")
    # We don't have any source control so just hard-code all of this information to empty
    set (ALIMER_REVISION_ID 0)
    set (ALIMER_SHORT_CHANGE_SET 0)
    set (ALIMER_CHANGE_SET 0)
    set (ALIMER_CHANGE_SET_DATE "")
else ()
    execute_process(COMMAND git rev-parse --short HEAD
        OUTPUT_VARIABLE ALIMER_SHORT_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git rev-parse HEAD
        OUTPUT_VARIABLE ALIMER_CHANGE_SET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(COMMAND git log -1 --abbrev=12 --date=format:%Y-%m-%d --pretty=format:%cd
        OUTPUT_VARIABLE ALIMER_CHANGE_SET_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

# Source
add_subdirectory (src)

# Print current build configuration
message(STATUS "Engine version: ${ALIMER_VERSION}")
message(STATUS "GIT Revision ID: ${ALIMER_REVISION_ID}")
message(STATUS "GIT Short ChangeSet: ${ALIMER_SHORT_CHANGE_SET}")
message(STATUS "GIT ChangeSet: ${ALIMER_CHANGE_SET}")
message(STATUS "GIT ChangeSet Date: ${ALIMER_CHANGE_SET_DATE}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

message(STATUS "Detected platform: ${ALIMER_PLATFORM_NAME}")
message(STATUS "Detected architecture: ${ALIMER_ARCH}")
message(STATUS "Detected host architecture: ${ALIMER_HOST_ARCH}")

if (ALIMER_SHARED_LIBRARY)
    message(STATUS "  Library         SHARED")
else ()
    set(ALIMER_LIBRARY_TYPE STATIC)
endif ()
