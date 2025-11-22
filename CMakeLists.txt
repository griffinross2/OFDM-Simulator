cmake_minimum_required(VERSION 3.26)

include(FetchContent)

project(OFDM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(OFDM)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_subdirectory("${CMAKE_SOURCE_DIR}/source")

# Matplot++
FetchContent_Declare(
    matplotplusplus
    GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus.git
    GIT_TAG v1.2.2
)

FetchContent_MakeAvailable(matplotplusplus)

FetchContent_GetProperties(matplotplusplus
    POPULATED matplotplusplus_POPULATED
    SOURCE_DIR matplotplusplus_SOURCE_DIR
    BINARY_DIR matplotplusplus_BINARY_DIR
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    matplot
)

target_compile_options(matplot PRIVATE /wd4834)