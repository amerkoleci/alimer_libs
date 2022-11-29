# stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

# libktx
set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ktx)

set(KTX_SOURCES
    ${KTX_DIR}/include/ktx.h

    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/dfdutils/createdfd.c
    ${KTX_DIR}/lib/dfdutils/colourspaces.c
    ${KTX_DIR}/lib/dfdutils/dfd.h
    ${KTX_DIR}/lib/dfdutils/dfd2vk.inl
    ${KTX_DIR}/lib/dfdutils/interpretdfd.c
    ${KTX_DIR}/lib/dfdutils/printdfd.c
    ${KTX_DIR}/lib/dfdutils/queries.c
    ${KTX_DIR}/lib/dfdutils/vk2dfd.c
    ${KTX_DIR}/lib/etcdec.cxx
    ${KTX_DIR}/lib/etcunpack.cxx
    ${KTX_DIR}/lib/filestream.c
    ${KTX_DIR}/lib/filestream.h
    ${KTX_DIR}/lib/formatsize.h
    ${KTX_DIR}/lib/hashlist.c
    #${KTX_DIR}/lib/info.c
    ${KTX_DIR}/lib/ktxint.h
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/memstream.h
    ${KTX_DIR}/lib/strings.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/uthash.h
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/texture.h

    # Basis Universal
    ${KTX_DIR}/lib/basis_sgd.h
    ${KTX_DIR}/lib/basis_transcode.cpp
    ${KTX_DIR}/lib/basisu/transcoder/basisu_containers.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu_containers_impl.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu_file_headers.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder_internal.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder_uastc.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder.cpp
    ${KTX_DIR}/lib/basisu/transcoder/basisu_transcoder.h
    ${KTX_DIR}/lib/basisu/transcoder/basisu.h
    ${KTX_DIR}/lib/basisu/zstd/zstd.c

    # KT1
    ${KTX_DIR}/lib/texture1.c
    ${KTX_DIR}/lib/texture1.h

    # KTX2
    ${KTX_DIR}/lib/texture2.c
    ${KTX_DIR}/lib/texture2.h
)

add_library(libktx STATIC ${KTX_SOURCES})
target_compile_definitions(libktx PUBLIC LIBKTX)
#if (WIN32)
#    target_compile_definitions(libktx PUBLIC "KTX_API=__declspec(dllexport)")
#endif()
target_compile_definitions(libktx PUBLIC KHRONOS_STATIC=0)
target_compile_definitions(libktx PUBLIC KTX_FEATURE_WRITE=0)
target_compile_definitions(libktx PUBLIC BASISD_SUPPORT_KTX2_ZSTD=0)
target_compile_definitions(libktx PUBLIC BASISU_NO_ITERATOR_DEBUG_LEVEL)

target_include_directories(libktx PUBLIC ${KTX_DIR}/include)
target_include_directories(libktx PRIVATE
    ${KTX_DIR}/lib
    ${KTX_DIR}/lib/dfdutils
    ${KTX_DIR}/lib/basisu/zstd
    ${KTX_DIR}/other_include
    ${KTX_DIR}/utils
)

set_property(TARGET libktx PROPERTY FOLDER "ThirdParty")
