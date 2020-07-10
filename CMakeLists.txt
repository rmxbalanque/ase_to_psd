# Roland Munguia

# Configure CMake Version
cmake_minimum_required(VERSION 3.1...3.16)

# -----------------------------------------------------------------------------
# Start ase_to_psd project
# -----------------------------------------------------------------------------

# Configure version
set(ATP_MAJOR_VERSION 0)
set(ATP_MINOR_VERSION 1)
set(ATP_PATCH_VERSION 0)
set(ATP_VERSION
        ${ATP_MAJOR_VERSION}.${ATP_MINOR_VERSION}.${ATP_PATCH_VERSION})

# Start project
project(ase_to_psd VERSION ${ATP_VERSION} LANGUAGES CXX C)
message(STATUS "Build ase_to_psd: ${PROJECT_VERSION}")

# Setup module subdirectory.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# -----------------------------------------------------------------------------
# Build Settings
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Set default build to debug
# -----------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
endif ()

# -----------------------------------------------------------------------------
# Build Binaries / Library
# -----------------------------------------------------------------------------

# Set sources/includes
set(ATP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/atp_converter.cpp include/ase_to_psd/decoder.h include/ase_to_psd/io_types.h src/decoder.cpp)
message(STATUS ${ATP_SOURCES})
list(APPEND ATP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdAllocator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdExport.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdLayerCanvasCopy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdParseDocument.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdPch.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdBlendMode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdMallocAllocator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdParseImageDataSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdSyncFileReader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdColorMode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdFixedSizeString.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdNativeFile.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdParseImageResourcesSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdSyncFileWriter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdDecompressRle.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdInterleave.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdParseColorModeDataSection.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/Psdminiz.c
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/PsdParseLayerMaskSection.cpp)
message(STATUS ${ATP_SOURCES})

set(ATP_HEADERS /include/ase_to_psd/aseprite.h /scr/parser.h)
add_executable(atp_converter ${ATP_SOURCES})
target_compile_features(atp_converter PRIVATE cxx_std_17)

configure_file("./input/Forest_Pack_(1).ase" "${CMAKE_BINARY_DIR}/input.ase" COPYONLY)

# Set ATP include dirs
target_include_directories(atp_converter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/" "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Photoshop library
#set(PSD_SDK_SOURCES ${CMAKE_SOURCE_DIR}/thirdparty/psd_sdk/)
#macro(add_psd_src src)
#    list(APPEND ${PSD_SDK_SOURCES} scr)
#endmacro()

file(GLOB PSD_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/*.cpp")
#add_library(psd_sdk STATIC ${PSD_SRC_FILES})
#target_include_directories(psd_sdk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/")
#target_link_libraries(atp_converter PUBLIC psd_sdk)

# Decompression Library
add_subdirectory("thirdparty/zlib/")
target_link_libraries(atp_converter PRIVATE zlibstatic)
target_include_directories(atp_converter PRIVATE "thirdparty/zlib")
target_include_directories(atp_converter PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zlib/")