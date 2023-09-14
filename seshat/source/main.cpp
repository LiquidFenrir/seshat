/*Copyright 2014 Francisco Alvaro

 This file is part of SESHAT.

    SESHAT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SESHAT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SESHAT.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <cstdio>
#include <cstring>
#include <iostream>
#include <meparser.hpp>
#include <sample.hpp>

int main(int argc, char* argv[])
{
    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);

    // Load sample and system configuration
    Sample m("SampleMathExps/exp.scgink");
    meParser seshat("Config/CONFIG");

    // Render image to file
    if (argc > 1)
        m.render_img("render.pgm");

    // Print sample information
    m.print();
    printf("\n");

    // Parse math expression
    seshat.parse_me(m);

    return 0;
}
