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

add_subdirectory(rnnlib4seshat)
# required by the following
add_subdirectory(seshat)

# arm-none-eabi-cmake
if(NINTENDO_3DS)
    dkp_target_generate_symbol_list(${PROJECT_NAME})

    dkp_add_asset_target(${PROJECT_NAME}_3ds_romfs ./Config)

    # Generate a SMDH file for the executable
    ctr_generate_smdh(${PROJECT_NAME}.smdh
        NAME        "seshat"

        # Other options available:
        DESCRIPTION "Math input recognition"
        AUTHOR      "LiquidFenrir"
    )

    # Specify that the executable is a 3DSX file
    ctr_create_3dsx(${PROJECT_NAME}
        SMDH  ${PROJECT_NAME}.smdh
        ROMFS ${PROJECT_NAME}_3ds_romfs
    )

    add_subdirectory(math_input_3ds)

    set(WANT_EXTRA_LIBRARIES citro2d citro3d)
else()
    add_subdirectory(math_input_${WANT_MATHINPUT_VER})
endif()

# Options for code generation
target_compile_options(${PROJECT_NAME} PRIVATE
    ${EXTRA_GPROF_FLAGS}
    -Wall -Wextra
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-unused-variable
    -Wno-type-limits
    -Wno-psabi
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${EXTRA_GPROF_FLAGS}
    ${USE_STATIC_LIB_FLAGS}
    ${WANT_EXTRA_LIBRARIES}
    seshat
    m
)
