SESHAT - Handwritten math expression parser  
Copyright (C) 2014, Francisco Alvaro  
Heavily modified in 2023 by Théo B.
----------------

More information at https://github.com/falvaro/seshat

----------------
License: Seshat is released under the license GPLv3 (see LICENSE file)

----------------
Requirements:
- Compilation tools: cmake and g++

----------------
Compilation: ./do_compile.sh `<build type>` `[extra cmake arguments here]`

----------------
Usage: ./build-`<build type>`/math_input

----------------
Modifications:  
This version of seshat:
- Has no InkML support anymore (xerces-c dependency removed)
- No longer makes use of boost
- Requires  C++23 support in your compiler (std::ranges::iota_view, std::ranges::zip_view, std::ranges::fold_left)
  - Or at the very least, C++20 (concepts, std::span) with the ranges-v3 library to add the STL ranges missing from this version
- More modern memory management, less abusive usage of std::list and raw new/delete. Who knows, might make it faster.
- As such, this is pretty much standalone, as long as you have a recent compiler and the Config folder

----------------
Fork goals:
To use this in a natural display c++ calculator as an open source Math Input Panel kind of thing.  
In this endeavour, the program will be turned into a library that should let you input mouse/touch strokes over time and get a list of possible results, for user selection.  
- This step has been completed, the calculator part has not.
