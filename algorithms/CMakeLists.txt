cmake_minimum_required(VERSION 3.16)
project(ticket-to-ride-graphs)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)

# Create executable
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# Set compiler flags
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17) 