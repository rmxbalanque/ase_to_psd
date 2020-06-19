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
project(ase_to_psd VERSION ${ATP_VERSION} LANGUAGES CXX)
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
set(ATP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/atp_converter.cpp)
add_executable(atp_converter ${ATP_SOURCES})

# Set itp include dirs
target_include_directories(atp_converter PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/rapidjson/include/")