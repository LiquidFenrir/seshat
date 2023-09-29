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
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <seshat/seshat.hpp>

static seshat::sample loadSCGInk(const char* path)
{
    seshat::sample out;

    std::ifstream fd(path);
    if (!fd) {
        std::cerr << "Error loading SCGInk file '" << path << "'\n";
        return out;
    }

    std::string magic;
    std::getline(fd, magic);
    if (magic.back() == '\r')
        magic.pop_back();
    if (magic != "SCG_INK") {
        std::cerr << "Error: input file format is not SCG_INK: " << magic << "\n";
        return out;
    }

    int nstrokes, npoints;
    fd >> nstrokes;
    out.strokes.resize(nstrokes);
    for (auto& out_stroke : out.strokes) {
        fd >> npoints;
        out_stroke.points.resize(npoints);
        for (auto& out_point : out_stroke.points) {
            fd >> out_point.x >> out_point.y;
        }
    }

    return out;
}

int main(int argc, char* argv[])
{
    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path to .scgink file>" << std::endl;
        std::cerr << "Note: run in a directory with a file available at ./Config/CONFIG" << std::endl;
    }

    // Load sample and system configuration
    seshat::sample s = loadSCGInk(argv[1]);
    seshat::math_expression recog;
    auto hyps = recog.parse_sample(s);
    printf("Found %zd hypothesis\n", hyps.size());
    for (const auto& hyp : hyps) {
#ifdef SESHAT_HYPOTHESIS_TREE

#else
        printf("%s\n", hyp.repr.c_str());
#endif
    }
}
