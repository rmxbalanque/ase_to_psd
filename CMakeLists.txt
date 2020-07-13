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
set(ATP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/atp_converter.cpp src/decoder.cpp src/converter.cpp "${CMAKE_CURRENT_SOURCE_DIR}/include/ase_to_psd/converter.h")
set(ATP_HEADERS /include/ase_to_psd/aseprite.h
                /include/ase_to_psd/converter.h
                /include/ase_to_psd/decoder.h
                /include/ase_to_psd/io_types.h)

# Add ATP exetuable
add_executable(atp_converter ${ATP_SOURCES})

# C++ 17 enabled
target_compile_features(atp_converter PRIVATE cxx_std_17)

# Set input test file in bynary directory
configure_file("./input/Forest_Pack_(1).ase" "${CMAKE_BINARY_DIR}/input.ase" COPYONLY)

# Set ATP include dirs
target_include_directories(atp_converter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/" "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Photoshop library
file(GLOB PSD_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/*.cpp"
                        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/Psd/*.c")
add_library(psd_sdk STATIC ${PSD_SRC_FILES})
target_include_directories(psd_sdk PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/psd_sdk/src/")
target_link_libraries(atp_converter PUBLIC psd_sdk)

# ZLIB Library
add_subdirectory("thirdparty/zlib/")
target_link_libraries(atp_converter PRIVATE zlibstatic)
target_include_directories(atp_converter PRIVATE "thirdparty/zlib")
target_include_directories(atp_converter PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zlib/")

# CXXOPTS Library
add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts/include/")
target_link_libraries(atp_converter PUBLIC cxxopts)

# TODO: Organize CMakelists.txt file