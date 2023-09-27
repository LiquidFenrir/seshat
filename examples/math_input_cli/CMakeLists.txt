
# Source code files
# find source -type f | grep "\.cpp$" | clip
# find include -type f | grep "\.hpp$" | clip
add_executable(math_input_cli
    include/math_input.hpp
    source/main.cpp
)

seshat_add_example_target_options(math_input_cli)

target_include_directories(math_input_cli PRIVATE
    include
)
