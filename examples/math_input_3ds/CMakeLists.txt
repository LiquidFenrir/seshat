
# Source code files
# find source -type f | grep "\.cpp$" | clip
# find include -type f | grep "\.hpp$" | clip
add_executable(math_input_3ds
    include/math_input.hpp
    source/main.cpp
)

seshat_add_example_target_options(math_input_3ds)

target_include_directories(math_input_3ds PRIVATE
    include
)

dkp_target_generate_symbol_list(math_input_3ds)

dkp_add_asset_target(math_input_3ds_romfs ../Config)

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

target_link_libraries(math_input_3ds PRIVATE
    seshat::seshat
    citro2d
    citro3d
)
