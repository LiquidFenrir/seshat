#!/bin/bash

find seshat/ rnnlib4seshat/ math_input_* -iname *.hpp -o -iname *.cpp | xargs clang-format -i
