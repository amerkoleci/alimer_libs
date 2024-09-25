# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")
target_sources(stb INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image_write.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb/stb_image_resize2.h
)

# libktx library
if (ALIMER_IMAGE AND ALIMER_IMAGE_KTX)
    # ktx
    set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ktx)

    set(KTX_INCLUDE_DIRS
        ${KTX_DIR}/include
        ${KTX_DIR}/lib
        ${KTX_DIR}/utils
        ${KTX_DIR}/external    
        ${KTX_DIR}/external/basisu/zstd
        ${KTX_DIR}/external/basisu/transcoder
        ${KTX_DIR}/other_include
    )

    set(KTX_SOURCES
        ${KTX_DIR}/external/dfdutils/createdfd.c
        ${KTX_DIR}/external/dfdutils/colourspaces.c
        ${KTX_DIR}/external/dfdutils/dfd.h
        ${KTX_DIR}/external/dfdutils/dfd2vk.inl
        ${KTX_DIR}/external/dfdutils/interpretdfd.c
        ${KTX_DIR}/external/dfdutils/printdfd.c
        ${KTX_DIR}/external/dfdutils/queries.c
        ${KTX_DIR}/external/dfdutils/vk2dfd.c
        ${KTX_DIR}/external/etcdec/etcdec.cxx
        ${KTX_DIR}/external/basisu/transcoder/basisu_containers.h
        ${KTX_DIR}/external/basisu/transcoder/basisu_containers_impl.h
        ${KTX_DIR}/external/basisu/transcoder/basisu_file_headers.h
        ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder_internal.h
        ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder_uastc.h
        ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder.cpp
        ${KTX_DIR}/external/basisu/transcoder/basisu_transcoder.h
        ${KTX_DIR}/external/basisu/transcoder/basisu.h
        ${KTX_DIR}/external/basisu/zstd/zstd.c
        ${KTX_DIR}/lib/checkheader.c
        ${KTX_DIR}/lib/etcunpack.cxx
        ${KTX_DIR}/lib/filestream.c
        ${KTX_DIR}/lib/filestream.h
        ${KTX_DIR}/lib/formatsize.h
        ${KTX_DIR}/lib/hashlist.c
        ${KTX_DIR}/lib/info.c
        ${KTX_DIR}/lib/ktxint.h
        ${KTX_DIR}/lib/memstream.c
        ${KTX_DIR}/lib/memstream.h
        ${KTX_DIR}/lib/strings.c
        ${KTX_DIR}/lib/swap.c
        ${KTX_DIR}/lib/uthash.h
        ${KTX_DIR}/lib/texture.c
        ${KTX_DIR}/lib/texture.h
        ${KTX_DIR}/lib/vkformat_check.c
        #${KTX_DIR}/lib/vkformat_str.c
        ${KTX_DIR}/lib/vkformat_typesize.c

        # Basis Universal
        ${KTX_DIR}/lib/basis_sgd.h
        ${KTX_DIR}/lib/basis_transcode.cpp
        ${KTX_DIR}/lib/miniz_wrapper.cpp
    

         # KT1
        ${KTX_DIR}/lib/texture1.c
        ${KTX_DIR}/lib/texture1.h

        # KTX2
        ${KTX_DIR}/lib/texture2.c
        ${KTX_DIR}/lib/texture2.h
    )

    add_library(ktx ${KTX_SOURCES})

    target_compile_definitions(ktx PUBLIC LIBKTX)
    if (WIN32)
        target_compile_definitions(ktx PUBLIC "KTX_API=__declspec(dllexport)")
    endif()
    target_compile_definitions(ktx PUBLIC KTX_FEATURE_WRITE=0)
    target_compile_definitions(ktx PUBLIC BASISD_SUPPORT_KTX2_ZSTD=0)
    target_compile_definitions(ktx PUBLIC BASISU_NO_ITERATOR_DEBUG_LEVEL)

    target_include_directories(ktx PUBLIC ${KTX_INCLUDE_DIRS})
    #target_link_libraries(ktx PUBLIC vulkan)
    set_property(TARGET ktx PROPERTY FOLDER "ThirdParty")
endif()
