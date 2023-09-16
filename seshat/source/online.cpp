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

#include <algorithm>
#include <numeric>
#include <online.hpp>
#include <ranges>

using namespace seshat;

//
// "stroke" methods
//

stroke::stroke(int n_p, bool pen_d, bool is_ht)
    : n_points(n_p)
    , pen_down(pen_d)
    , is_hat(is_ht)
{
    points.reserve(n_points);
}

int stroke::F_XMIN()
{
    return points.empty() ? (INT_MAX) : std::ranges::min_element(points, {}, &Point::x)->x;
}

int stroke::F_XMAX()
{
    return points.empty() ? (INT_MIN) : std::ranges::max_element(points, {}, &Point::x)->x;
}

int stroke::F_XMED()
{
    return std::accumulate(points.begin(), points.end(), 0, [](const int val, const auto& pt) {
               return pt.x + val;
           })
        / n_points;
}

//
// "sentence" methods
//

sentence::sentence(int n_s)
    : n_strokes(n_s)
{
    strokes.reserve(n_strokes);
}

// Remove repeated points
sentence sentence::no_repeats() const
{
    sentence sent_norep(n_strokes);

    for (const auto& curstroke : strokes) {
        auto& stroke_norep = sent_norep.strokes.emplace_back();
        for (const auto& pt : curstroke.points) {
            if (stroke_norep.points.empty() || pt != stroke_norep.points.back()) {
                stroke_norep.points.push_back(pt);
                stroke_norep.points.back().resetpu();
            }
        }

        stroke_norep.pen_down = curstroke.pen_down;
        stroke_norep.n_points = stroke_norep.points.size();
    }

    return sent_norep;
}

// Smoothing: median filter
sentence sentence::smoothed(int cont_size) const
{
    sentence sentNorm(n_strokes);

    int sum_x, sum_y;
    for (const auto& curstroke : strokes) {
        auto& strokeNorm = sentNorm.strokes.emplace_back();
        const std::vector<Point>& Points = curstroke.points;
        int np = curstroke.n_points;
        for (int p = 0; p < np; p++) {
            sum_x = 0;
            sum_y = 0;
            for (int c = p - cont_size; c <= p + cont_size; c++) {
                const int pt_idx = std::clamp(c, 0, np);
                sum_x += Points[pt_idx].x;
                sum_y += Points[pt_idx].y;
            }

            strokeNorm.points.emplace_back(int(sum_x / (cont_size * 2 + 1)), int(sum_y / (cont_size * 2 + 1)));
        }
        strokeNorm.pen_down = curstroke.pen_down;
        strokeNorm.n_points = strokeNorm.points.size();
    }
    return sentNorm;
}
