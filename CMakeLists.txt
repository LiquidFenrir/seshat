cmake_minimum_required(VERSION 3.20)

# Declare project containing a single eponymous executable
project(seshat LANGUAGES CXX)

macro(seshat_add_target_options NAME)
    target_compile_features(${NAME} PRIVATE cxx_std_23)
    
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${NAME} PRIVATE
            -Wall -Wextra
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-type-limits
            -Wno-psabi
        )
    endif()
endmacro()

macro(seshat_add_library_target_options NAME)
    seshat_add_target_options(${NAME})
    if(NOT SESHAT_BUILD_INPLACE)
        set_target_properties(${NAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
    endif()
endmacro()
macro(seshat_add_example_target_options NAME)
    seshat_add_target_options(${NAME})
    if(NOT SESHAT_BUILD_INPLACE)
        set_target_properties(${NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
    endif()
    target_link_libraries(${NAME} PRIVATE
        seshat::seshat
    )
endmacro()

if(NOT DEFINED SESHAT_BUILD_EXAMPLES)
    if(DEFINED SESHAT_BUILD_EXAMPLE) # specific wanted example(s)
        set(SESHAT_BUILD_EXAMPLES ON)
    else()
        set(SESHAT_BUILD_EXAMPLES OFF)
    endif()
endif()

add_subdirectory(rnnlib4seshat)
# required by the following
add_subdirectory(seshat)

if(SESHAT_BUILD_EXAMPLES)
    if(NOT DEFINED SESHAT_BUILD_EXAMPLE)
        if(NINTENDO_3DS) # arm-none-eabi-cmake
            set(SESHAT_WANT_EXAMPLE 3ds)
        else()
            set(SESHAT_WANT_EXAMPLE cli sdl)
        endif()
    else()
        set(SESHAT_WANT_EXAMPLE ${SESHAT_BUILD_EXAMPLE})
    endif()

    foreach(example ${SESHAT_WANT_EXAMPLE})
        add_subdirectory(examples/math_input_${example})
    endforeach(example)
endif()
