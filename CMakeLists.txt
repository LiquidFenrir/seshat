cmake_minimum_required(VERSION 3.20)

# Declare project containing a single eponymous executable
project(seshat LANGUAGES CXX)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG=1)
endif()

# Options for code generation
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-type-limits
)

add_subdirectory(rnnlib4seshat)

target_link_libraries(${PROJECT_NAME} PRIVATE
    -static
    rnnlib4seshat
)

add_subdirectory(seshat)
