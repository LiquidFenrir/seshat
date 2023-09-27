#!/bin/bash

find seshat/ rnnlib4seshat/ examples/* -iname *.hpp -o -iname *.cpp | xargs clang-format --verbose -i
