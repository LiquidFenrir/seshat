
# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 CONFIG COMPONENTS SDL2)
if(NOT SDL2_FOUND) # don't try to build it
    message(SEND_ERROR "SDL2 not found - skipping building SDL2 example")
    return()
endif()

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)

# Source code files
# find source -type f | grep "\.cpp$" | clip
# find include -type f | grep "\.hpp$" | clip
add_executable(math_input_sdl
    include/math_input.hpp
    source/main.cpp
)

seshat_add_example_target_options(math_input_sdl)

target_include_directories(math_input_sdl PRIVATE
    include
)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(math_input_sdl PRIVATE SDL2::SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(math_input_sdl PRIVATE SDL2::SDL2)
