cmake_minimum_required(VERSION 3.14)  # Minimum CMake version
project(JobTracker)                   # Project name

set(CMAKE_CXX_STANDARD 20)            # Use C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add external dependencies
include(FetchContent)

# Fetch sqlite_modern_cpp from GitHub
FetchContent_Declare(
    sqlite_modern_cpp
    GIT_REPOSITORY https://github.com/SqliteModernCpp/sqlite_modern_cpp.git
    GIT_TAG master  # Use the latest commit on the master branch
)

FetchContent_MakeAvailable(sqlite_modern_cpp)

# Add the executable
add_executable(JobTracker src/main.cpp)

# Link SQLite3 and sqlite_modern_cpp
target_link_libraries(JobTracker PRIVATE sqlite3)

# Include sqlite_modern_cpp headers
target_include_directories(JobTracker PRIVATE ${sqlite_modern_cpp_SOURCE_DIR}/hdr)