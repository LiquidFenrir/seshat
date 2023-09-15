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
#include <unistd.h>

static void usage(const char* str)
{
    fprintf(stderr, "SESHAT - Handwritten math expression parser\nhttps://github.com/falvaro/seshat\n");
    fprintf(stderr, "Copyright (C) 2014, Francisco Alvaro\n\n");
    fprintf(stderr, "Usage: %s -c config -i input [-r render]\n\n", str);
    fprintf(stderr, "  -c config: set the configuration file\n");
    fprintf(stderr, "  -r render: save in 'render' the image representing the input expression (PGM format)\n");
}

static char input[512] = { 0 }, config[512] = { 0 }, render[512] = { 0 };
int main(int argc, char* argv[])
{
    int option;
    while ((option = getopt(argc, argv, "c:i:r:")) != -1)
        switch (option) {
        case 'c':
            strncpy(config, optarg, sizeof(config));
            break;
        case 'i':
            strncpy(input, optarg, sizeof(input));
            break;
        case 'r':
            strncpy(render, optarg, sizeof(render));
            break;
        case '?':
            usage(argv[0]);
            return -1;
        }

    if (input[0] == 0 || config[0] == 0) {
        usage(argv[0]);
        return -1;
    }

    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);

    // Load sample and system configuration
    Sample m(input);
    meParser seshat(config);

    // Render image to file
    if (render[0] != 0)
        m.render_img(render);

    // Print sample information
    m.print();
    printf("\n");

    // Parse math expression
    seshat.parse_me(m);

    return 0;
}
