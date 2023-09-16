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
#ifndef _G_PARSER_
#define _G_PARSER_

#include <cstdio>
#include <string>
#include <vector>

namespace seshat {

struct Grammar;

class gParser {
    Grammar* g;

    bool isFillChar(char c);
    std::vector<std::string> split(const char* str);
    bool nextLine(FILE* fd, char* lin);

public:
    gParser(Grammar* gram, FILE* fd);

    void parse(FILE* fd);
};

}

#endif
