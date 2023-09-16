
# Source code files
# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)
