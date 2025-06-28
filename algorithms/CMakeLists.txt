cmake_minimum_required(VERSION 3.16)
project(ticket-to-ride-graphs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Create executable
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    extern
    ${UUID_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    ${UUID_LIBRARY}
)

# Set compiler flags
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17) 
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror=return-type)