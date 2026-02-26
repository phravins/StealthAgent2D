cmake_minimum_required(VERSION 3.20)
project(StealthAgent2D)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps/raylib)

# Add source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Create executable
add_executable(StealthAgent2D ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(StealthAgent2D PRIVATE raylib)

# Include directories
target_include_directories(StealthAgent2D PRIVATE src)

# Ensure assets directory is copied to output
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
