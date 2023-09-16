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

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <stroke.hpp>
#include <utility>

using namespace seshat;

bool esNum(char c)
{
    return (c >= '0' && c <= '9') || c == '-' || c == '.';
}

Stroke::Stroke()
{
    cx = cy = 0;
    rx = ry = INT_MAX;
    rs = rt = -INT_MAX;
}

Point* Stroke::get(int idx)
{
    return &pseq[idx];
}

const Point* Stroke::get(int idx) const
{
    return &pseq[idx];
}

int Stroke::getNPoints() const
{
    return (int)pseq.size();
}

void Stroke::print()
{
    printf("STROKE - %d points\n", (int)pseq.size());
    for (int i = 0; i < (int)pseq.size(); i++)
        printf(" (%g,%g)", pseq[i].x, pseq[i].y);
    printf("\n");
}

float Stroke::min_dist(Stroke* st)
{
    float mind = FLT_MAX;
    for (int i = 0; i < (int)pseq.size(); i++) {
        for (int j = 0; j < st->getNPoints(); j++) {
            Point* p = st->get(j);

            float d = (pseq[i].x - p->x) * (pseq[i].x - p->x)
                + (pseq[i].y - p->y) * (pseq[i].y - p->y);

            if (d < mind)
                mind = d;
        }
    }

    return sqrt(mind);
}
