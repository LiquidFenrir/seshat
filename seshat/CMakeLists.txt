
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
    source/internal_hypothesis.cpp
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
# find include -type f | grep "\.hpp$" | clip
set(SESHAT_LIB_HEADERS
    include/cellcyk.hpp
    include/duration.hpp
    include/featureson.hpp
    include/gmm.hpp
    include/gparser.hpp
    include/grammar.hpp
    include/internal_hypothesis.hpp
    include/logspace.hpp
    include/meparser.hpp
    include/online.hpp
    include/path.hpp
    include/production.hpp
    include/samples.hpp
    include/segmentation.hpp
    include/sparel.hpp
    include/stroke.hpp
    include/symfeatures.hpp
    include/symrec.hpp
    include/tablecyk.hpp
    include/vectorimage.hpp
)
# find public -type f | grep "\.hpp$" | clip
set(SESHAT_LIB_INTERFACES
    public/seshat/hypothesis.hpp
    public/seshat/point.hpp
    public/seshat/seshat.hpp
)

add_library(seshat_lib_seshat STATIC
    ${SESHAT_LIB_SRCS}
    ${SESHAT_LIB_HEADERS}
    ${SESHAT_LIB_INTERFACES}
)
add_library(seshat::seshat ALIAS seshat_lib_seshat)

seshat_add_library_target_options(seshat_lib_seshat)

target_link_libraries(seshat_lib_seshat PRIVATE
    seshat::rnnlib4seshat
)
target_include_directories(seshat_lib_seshat
    PRIVATE
        include
    PUBLIC
        public
)
