cmake_minimum_required(VERSION 3.20)

# Declare project containing a single eponymous executable
project(math_input LANGUAGES CXX)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

if(WANT_GPROF_INSTR)
    set(EXTRA_GPROF_FLAGS -pg -no-pie)
endif()
if(WANT_LIBC_STATIC)
    set(USE_STATIC_LIB_FLAGS -static)
endif()

# Options for code generation
target_compile_options(${PROJECT_NAME} PRIVATE
    ${EXTRA_GPROF_FLAGS}
    -Wall -Wextra
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-type-limits
)

add_subdirectory(seshat)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${EXTRA_GPROF_FLAGS}
    ${USE_STATIC_LIB_FLAGS}
    seshat
    m
)

add_subdirectory(math_input)
