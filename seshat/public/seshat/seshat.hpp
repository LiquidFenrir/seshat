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
#ifndef SESHAT_PUBLIC_INTERFACE
#define SESHAT_PUBLIC_INTERFACE

#include <memory>
#include <seshat/hypothesis.hpp>
#include <seshat/point.hpp>
#include <vector>

namespace seshat {

struct sample {
    struct stroke {
        std::vector<point> points;
    };
    std::vector<stroke> strokes;
};

// do not use these, forward declarations for the inner workings
class meParser;
class Samples;

// only this
class math_expression {
    std::unique_ptr<meParser> parser;
    std::unique_ptr<Samples> samples;

public:
    explicit math_expression(const char* config_path = "Config/CONFIG");
    ~math_expression();

    std::vector<hypothesis> parse_sample(const sample&);
};

}

#endif
