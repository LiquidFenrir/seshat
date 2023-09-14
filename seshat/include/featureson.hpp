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

This file is a modification of the online features original software
covered by the following copyright and permission notice:

*/
/*
    Copyright (C) 2006,2007 Moisés Pastor <mpastorg@dsic.upv.es>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef FEATURES_H
#define FEATURES_H

#include "online.hpp"
#include <float.h>
#include <iomanip>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <vector>

#define MAXNUMHATS 200
#define OFFSET_INS 20

class frame {
public:
    double x, y, dx, dy, ax, ay, k;
    frame(PointR pt);

    void print(std::ostream& fd);
    int get_fr_dim();

    double getFea(int i)
    {
        switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return dx;
        case 3:
            return dy;
        case 4:
            return ax;
        case 5:
            return ay;
        case 6:
            return k;
        default:
            fprintf(stderr, "Error: getFea(%d)\n", i);
            exit(-1);
        }
    }
};

class sentenceF {
public:
    std::vector<frame> frames;

    bool data_plot(std::ostream& fd);
    bool print(std::ostream& fd);

    void calculate_features(const sentence& s);

private:
    std::vector<PointR> normalizaAspect(const std::vector<Point>& puntos);
    void calculate_derivatives(const std::vector<PointR>& points, bool norm = true);
    void calculate_kurvature();
};

#endif
