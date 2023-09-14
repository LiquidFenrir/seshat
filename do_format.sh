#!/bin/bash

find seshat/ rnnlib4seshat/ -iname *.hpp -o -iname *.cpp | xargs clang-format -i
