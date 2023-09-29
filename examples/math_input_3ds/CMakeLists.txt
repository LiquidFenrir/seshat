
# Source code files
# find source -type f | grep "\.cpp$" | clip
# find include -type f | grep "\.hpp$" | clip
add_executable(math_input_3ds
    include/math_input.hpp
    source/main.cpp
)

# link in the seshat library, and set up common warnings
seshat_add_example_target_options(math_input_3ds)

target_include_directories(math_input_3ds PRIVATE
    include
)

# devkitPro-cmake-specific functions

# Helpful for debugging
dkp_target_generate_symbol_list(math_input_3ds)
# Assets for romfs
dkp_add_asset_target(math_input_3ds_romfs ${PROJECT_SOURCE_DIR}/Config)

# 3DS-specific functions

# Generate a SMDH file for the executable
ctr_generate_smdh(math_input_3ds.smdh
    NAME        "seshat"

    # Other options available:
    DESCRIPTION "Math input recognition"
    AUTHOR      "LiquidFenrir"
)

# Specify that the executable is a 3DSX file
ctr_create_3dsx(math_input_3ds
    SMDH  math_input_3ds.smdh
    ROMFS math_input_3ds_romfs
)

# link 3DS graphics library (currently unneeded)
target_link_libraries(math_input_3ds PRIVATE
    citro2d
    citro3d
)
