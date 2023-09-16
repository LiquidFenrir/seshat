
# Source code files
# find source -type f | grep "\.cpp$" | clip
set(SESHAT_LIB_SRCS
    source/cellcyk.cpp
    source/duration.cpp
    source/featureson.cpp
    source/gmm.cpp
    source/gparser.cpp
    source/grammar.cpp
    source/hypothesis.cpp
    source/logspace.cpp
    source/meparser.cpp
    source/online.cpp
    source/production.cpp
    source/samples.cpp
    source/segmentation.cpp
    source/seshat.cpp
    source/sparel.cpp
    source/stroke.cpp
    source/symfeatures.cpp
    source/symrec.cpp
    source/tablecyk.cpp
)

add_library(seshat STATIC ${SESHAT_LIB_SRCS})

target_compile_features(seshat PRIVATE cxx_std_23)

target_compile_options(seshat PRIVATE
    ${EXTRA_GPROF_FLAGS}
    -Wall -Wextra
    -Wno-sign-compare
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-type-limits
)

add_subdirectory(../rnnlib4seshat)

target_link_libraries(seshat PRIVATE
    ${EXTRA_GPROF_FLAGS}
    rnnlib4seshat
    m
)

target_include_directories(seshat PRIVATE
    include
)

target_include_directories(seshat PUBLIC
    public
)

