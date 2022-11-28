# CMake version
cmake_minimum_required (VERSION 3.16)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "In-source builds are not supported. Please choose a different binary directory.")
	return()
endif()

# Build version
set (LIB_VERSION_MAJOR 1)
set (LIB_VERSION_MINOR 0)
set (LIB_VERSION_PATCH 0)
set (LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

# Project declaration
project (alimer_libs
    VERSION ${LIB_VERSION}
    DESCRIPTION "Collection of C/C++ native libraries"
    HOMEPAGE_URL "https://github.com/amerkoleci/alimer_libs"
    LANGUAGES CXX C)

# Configure CMake global variables
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_C_STANDARD 99)

# Configure CMake global variables
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Use solution folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Build Type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	# By default, we use Release
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE "Release" CACHE
		STRING "Choose the type of build." FORCE
	)

	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif()

# Options
if (IOS OR EMSCRIPTEN)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable shared library on (IOS, WEB)" FORCE)
else()
    option(BUILD_SHARED_LIBS "Build shared library" ON)
endif ()

# Setting Platform
if(ANDROID)
    set(LIB_NATIVE_PLATFORM "Android")
elseif(IOS)
    set(LIB_NATIVE_PLATFORM "iOS")
elseif(APPLE)
    set(LIB_NATIVE_PLATFORM "macOS")
elseif(WINDOWS_STORE)
    set(LIB_NATIVE_PLATFORM "UWP")
elseif(WIN32)
    set(LIB_NATIVE_PLATFORM "Win32")
elseif(UNIX)
    set(LIB_NATIVE_PLATFORM "Unix")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Platform Flags
if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
elseif(ANDROID)
elseif(UNIX)
    if(APPLE)
	    set(CMAKE_MACOSX_RPATH ON)
	    set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)
	    set(LOBJC "objc")
    else()
        set(CMAKE_SKIP_RPATH OFF)
    endif()
elseif(WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

# src
add_subdirectory(src)

# Print current build configuration
message(STATUS "Version ${LIB_VERSION}")
message(STATUS "Platform: ${LIB_NATIVE_PLATFORM}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (BUILD_SHARED_LIBS)
    message(STATUS "  Library         SHARED")
else ()
    message(STATUS "  Library         STATIC")
endif ()
