cmake_minimum_required(VERSION 3.14)
project(dynoXOR LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add Catch2 subdirectory (this uses Catch2's CMakeLists.txt)
# Disable Catch2's own tests to speed up build
set(CATCH_BUILD_TESTING OFF CACHE BOOL "Disable Catch2 self tests")
add_subdirectory(externals/Catch2)

# Your main executable (dynoXOR tool)
add_executable(dynoXOR 
    src/main.cpp 
    src/functions.cpp
)

# Your test executable (separate from main)
add_executable(test_dynoXOR 
    tests/test_dynoXOR.cpp 
    src/functions.cpp
)

# Link Catch2 to your test executable
target_link_libraries(test_dynoXOR PRIVATE Catch2::Catch2WithMain)

# Enable testing
enable_testing()
add_test(NAME RunTests COMMAND test_dynoXOR)

