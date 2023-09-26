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
#ifndef _STROKE_
#define _STROKE_

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <seshat/point.hpp>
#include <vector>

namespace seshat {

class math_expression;

class Stroke {
    friend math_expression;

    std::vector<Point> pseq;

public:
    // Coordinates of the region it defines
    int rx, ry, rs, rt;
    int cx, cy; // Centroid

    Stroke();

    Point* get(int idx);
    const Point* get(int idx) const;
    int getNPoints() const;

    float min_dist(Stroke* st);
};

}

#endif
