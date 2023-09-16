#!/bin/bash

find seshat/ rnnlib4seshat/ math_input/ -iname *.hpp -o -iname *.cpp | xargs clang-format -i
