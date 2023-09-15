
# Source code files
# find source -type f | grep "\.cpp$" | clip
set(RNN_LIB_SRCS
    source/ClassificationLayer.cpp
    source/DataExporter.cpp
    source/Layer.cpp
    source/Mdrnn.cpp
    source/Optimiser.cpp
    source/Random.cpp
    source/WeightContainer.cpp
)

add_library(rnnlib4seshat STATIC ${RNN_LIB_SRCS})

target_compile_features(rnnlib4seshat PRIVATE cxx_std_23)

target_compile_options(rnnlib4seshat PRIVATE
    ${EXTRA_GPROF_FLAGS}
    -Wall -Wextra
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-type-limits
)

target_link_libraries(rnnlib4seshat PRIVATE
    ${EXTRA_GPROF_FLAGS}
    -static
    m
)

target_include_directories(rnnlib4seshat PUBLIC
    include
)
