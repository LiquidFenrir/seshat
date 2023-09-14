# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# Source code files
# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
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
    source/sample.cpp
    source/segmentation.cpp
    source/main.cpp
    source/sparel.cpp
    source/stroke.cpp
    source/symfeatures.cpp
    source/symrec.cpp
    source/tablecyk.cpp
)
