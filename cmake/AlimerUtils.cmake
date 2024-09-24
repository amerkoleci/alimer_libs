set(COMPILER_DEFINES "")
set(COMPILER_SWITCHES "")
set(LINKER_SWITCHES "")

# Setting Platform
if(EMSCRIPTEN)
    set(ALIMER_PLATFORM_NAME "Web")
elseif(ANDROID)
    set(ALIMER_PLATFORM_NAME "Android")
elseif(IOS)
    set(ALIMER_PLATFORM_NAME "iOS")
elseif(APPLE)
    set(ALIMER_PLATFORM_NAME "macOS")
elseif(WINDOWS_STORE)
    set(ALIMER_PLATFORM_NAME "UWP")
elseif(WIN32)
    set(ALIMER_PLATFORM_NAME "Win32")
elseif(UNIX)
    set(ALIMER_PLATFORM_NAME "Unix")
    set(ALIMER_LINUX_WSI_SELECTION "XCB" CACHE STRING "Select WSI target (XCB, XLIB, WAYLAND, D2D)")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Determines target architecture if not explicitly set
if (DEFINED VCPKG_TARGET_ARCHITECTURE)
    set(ALIMER_ARCH ${VCPKG_TARGET_ARCHITECTURE})
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Ww][Ii][Nn]32$")
    set(ALIMER_ARCH x86)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Xx]64$")
    set(ALIMER_ARCH x64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]$")
    set(ALIMER_ARCH arm)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64$")
    set(ALIMER_ARCH arm64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64EC$")
    set(ALIMER_ARCH arm64ec)
elseif(CMAKE_VS_PLATFORM_NAME_DEFAULT MATCHES "^[Ww][Ii][Nn]32$")
    set(ALIMER_ARCH x86)
elseif(CMAKE_VS_PLATFORM_NAME_DEFAULT MATCHES "^[Xx]64$")
    set(ALIMER_ARCH x64)
elseif(CMAKE_VS_PLATFORM_NAME_DEFAULT MATCHES "^[Aa][Rr][Mm]$")
    set(ALIMER_ARCH arm)
elseif(CMAKE_VS_PLATFORM_NAME_DEFAULT MATCHES "^[Aa][Rr][Mm]64$")
    set(ALIMER_ARCH arm64)
elseif(CMAKE_VS_PLATFORM_NAME_DEFAULT MATCHES "^[Aa][Rr][Mm]64EC$")
    set(ALIMER_ARCH arm64ec)
endif()
#--- Determines host architecture
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "[Aa][Rr][Mm]64|aarch64|arm64")
    set(ALIMER_HOST_ARCH arm64)
else()
    set(ALIMER_HOST_ARCH x64)
endif()

#--- Build with Unicode Win32 APIs per "UTF-8 Everywhere"
if(WIN32)
  list(APPEND COMPILER_DEFINES _UNICODE UNICODE)
endif()

function (alimer_setup_library TARGET_NAME)
    if (MSVC)
        # Set warning level 3
        target_compile_options(${TARGET_NAME} PRIVATE /W3)

        # Warnings as Errors Enabled
        target_compile_options(${TARGET_NAME} PRIVATE /WX)

        # Debug information
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Zi>)

        # Enable full optimization in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:Debug>:/Od> $<$<NOT:$<CONFIG:Debug>>:/Ox>)

        # Inline function expansion
	    target_compile_options(${TARGET_NAME} PRIVATE /Ob2)

        # Enable intrinsic functions in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/Oi>)

        # Favor fast code
	    target_compile_options(${TARGET_NAME} PRIVATE /Ot)

        # Enable fiber-safe optimizations in dev/release
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<NOT:$<CONFIG:Debug>>:/GT>)

	    # Enable string pooling
	    target_compile_options(${TARGET_NAME} PRIVATE /GF)

        # Use security checks only in debug
	    target_compile_options(${TARGET_NAME} PRIVATE $<$<CONFIG:DEBUG>:/sdl> $<$<NOT:$<CONFIG:DEBUG>>:/sdl->)

	    # Enable function-level linking
	    target_compile_options(${TARGET_NAME} PRIVATE /Gy)

        # Use fast floating point model
        target_compile_options(${TARGET_NAME} PRIVATE /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
    else()
        target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
    endif()
endfunction()
