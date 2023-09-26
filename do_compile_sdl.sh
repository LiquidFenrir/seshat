#!/bin/bash

export CC=gcc
export CXX=g++
export USING_CMAKE="cmake"
export BUILDDIR_SUFFIX="-sdl"

./do_compile_inner.sh "$@" -DWANT_MATHINPUT_VER="sdl"
