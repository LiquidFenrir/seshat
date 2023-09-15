#!/bin/bash

if [ "$1" = "Release" ]
then
    export CUR_BUILD_TYPE=Release
else
    export CUR_BUILD_TYPE=Debug
fi

export CC=gcc
export CXX=g++

cmake -DCMAKE_BUILD_TYPE=$CUR_BUILD_TYPE -S . -B build-$CUR_BUILD_TYPE
cmake --build build-$CUR_BUILD_TYPE --config $CUR_BUILD_TYPE
