#!/bin/bash

export CC=gcc
export CXX=g++

mkdir -p build
cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build build --config Debug
