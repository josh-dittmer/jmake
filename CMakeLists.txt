cmake_minimum_required(VERSION 3.10)
project(jmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add sources
file(GLOB_RECURSE sources   src/*.cpp src/*.tpp src/*.h)
file(GLOB_RECURSE data      resources/*)

# Set target definitions
add_executable(jmake ${sources})

# Compiler flags
target_compile_options(jmake PUBLIC -g -std=c++26 -Wall -Werror -ftemplate-backtrace-limit=100 -ferror-limit=100)

# Allow include files relative to root to be targeted with <>
target_include_directories(jmake PUBLIC src)

# Copy resources to build directory
file(COPY ${data} DESTINATION resources)

# Add dependencies
add_subdirectory(thirdparty/nlohmann_json)
target_link_libraries(jmake PRIVATE nlohmann_json::nlohmann_json)