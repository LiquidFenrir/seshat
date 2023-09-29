cmake_minimum_required(VERSION 3.20) # cxx_std_23

project(seshat LANGUAGES CXX)

option(SESHAT_BUILD_EXAMPLES "Build seshat examples" OFF)
option(SESHAT_WHICH_EXAMPLES "Which seshat examples to build" OFF)
if(SESHAT_WHICH_EXAMPLES AND NOT SESHAT_BUILD_EXAMPLES)
    set(SESHAT_BUILD_EXAMPLES ON)
endif()

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
endmacro()
macro(seshat_add_example_target_options NAME)
    seshat_add_target_options(${NAME})
    target_link_libraries(${NAME} PRIVATE
        seshat::seshat
    )
endmacro()

add_subdirectory(rnnlib4seshat)
# required by the following
add_subdirectory(seshat)

if(SESHAT_BUILD_EXAMPLES)
    if(NOT SESHAT_WHICH_EXAMPLES)
        if(NINTENDO_3DS) # arm-none-eabi-cmake
            set(SESHAT_WANT_EXAMPLE 3ds)
        else()
            set(SESHAT_WANT_EXAMPLE cli sdl)
        endif()
    else()
        set(SESHAT_WANT_EXAMPLE ${SESHAT_WHICH_EXAMPLES})
    endif()

    foreach(example ${SESHAT_WANT_EXAMPLE})
        add_subdirectory(examples/math_input_${example})
    endforeach(example)
endif()
