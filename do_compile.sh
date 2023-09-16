#!/bin/bash

if [ -z "$1" ]
then
    export CUR_BUILD_TYPE="Debug"
else
    export CUR_BUILD_TYPE="$1"
fi

export CC=gcc
export CXX=g++
export MY_BUILDDIR="build-$CUR_BUILD_TYPE"

cmake "${@:2}" -DCMAKE_BUILD_TYPE=$CUR_BUILD_TYPE -S . -B $MY_BUILDDIR
cmake --build $MY_BUILDDIR --config $CUR_BUILD_TYPE
