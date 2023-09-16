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
#include <seshat/seshat.hpp>

struct FILEDeleter {

    void operator()(FILE* fd)
    {
        fclose(fd);
    }
};
using FILEPtr = std::unique_ptr<FILE, FILEDeleter>;

seshat::sample loadSCGInk(const char* path)
{
    seshat::sample out;

    FILEPtr fd_holder(fopen(path, "r"));
    auto fd = fd_holder.get();
    if (!fd) {
        fprintf(stderr, "Error loading SCGInk file '%s'\n", path);
        return out;
    }

    char line[256];
    fgets(line, 256, fd);
    if (strcmp(line, "SCG_INK\n")) {
        fprintf(stderr, "Error: input file format is not SCG_INK\n");
        return out;
    }

    int nstrokes, npoints;
    fscanf(fd, "%d", &nstrokes);
    out.strokes.resize(nstrokes);
    for (auto& out_stroke : out.strokes) {
        fscanf(fd, "%d", &npoints);
        out_stroke.points.resize(npoints);
        for (auto& out_point : out_stroke.points) {
            fscanf(fd, "%f %f", &out_point.x, &out_point.y);
        }
    }

    return out;
}

int main(int argc, char* argv[])
{
    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);

    // Load sample and system configuration
    seshat::sample s = loadSCGInk("SampleMathExps/exp.scgink");
    seshat::math_expression recog;

    auto hyps = recog.parse_sample(s);
    for (const auto& hyp : hyps) {
        printf("Found hypothesis with %zd tokens\n", hyp.tokens.size());
    }
}
