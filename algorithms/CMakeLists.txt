cmake_minimum_required(VERSION 3.16)
project(ticket-to-ride-graphs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type configuration
option(BUILD_TYPE "Build type" "Debug")
set(CMAKE_BUILD_TYPE ${BUILD_TYPE} CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)

# Set build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Configure compiler flags based on build type
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -Wall -Wextra -Werror=return-type -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Wextra -Werror=return-type -Werror")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -Wall -Wextra -Werror=return-type -Werror")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -Wall -Wextra -Werror=return-type -Werror")

# Print current build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)

# Find uuid library
find_library(UUID_LIBRARY uuid)
find_path(UUID_INCLUDE_DIR uuid/uuid.h)

if(NOT UUID_LIBRARY OR NOT UUID_INCLUDE_DIR)
    message(FATAL_ERROR "uuid library not found. Please install libuuid-dev (Ubuntu/Debian) or uuid-devel (CentOS/RHEL)")
endif()

# Basic BFS with objects and adjacency list
add_executable(bfs_basic test/bfs_basic.cpp)
target_include_directories(bfs_basic PRIVATE
    include
    extern
    ${UUID_INCLUDE_DIR}
)
target_link_libraries(bfs_basic 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${UUID_LIBRARY}
)
target_compile_features(bfs_basic PRIVATE cxx_std_17)

# Cleaner BFS with pointers and adjacency list
add_executable(bfs_ptr test/bfs_ptr.cpp)
target_include_directories(bfs_ptr PRIVATE
    include
    extern
    ${UUID_INCLUDE_DIR}
)
target_link_libraries(bfs_ptr 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${UUID_LIBRARY}
)
target_compile_features(bfs_ptr PRIVATE cxx_std_17)