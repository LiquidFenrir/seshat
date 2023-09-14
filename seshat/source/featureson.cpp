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
#include <featureson.hpp>

// frame methods
frame::frame(PointR pt)
    : x{ pt.x }
    , y{ pt.y }
    , dx{ 0 }
    , dy{ 0 }
    , ax{ 0 }
    , ay{ 0 }
    , k{ 0 }
{
}

int frame::get_fr_dim()
{
    return 7;
}

// sentenceF methods

void sentenceF::calculate_features(const sentence& S)
{
    {
        const std::vector<PointR> pointsN = [&S, this]() {
            // Aspect normalization
            std::vector<Point> points;

            for (const auto& s_stroke : S.strokes) {
                if (!s_stroke.pen_down)
                    continue;

                points.insert(points.end(), s_stroke.points.begin(), s_stroke.points.end());
                points.back().setpu();
            }

            return normalizaAspect(points);
        }();

        // Normalizaed "x" and "y" as first features
        frames.reserve(pointsN.size());
        std::copy(pointsN.begin(), pointsN.end(), std::back_inserter(frames));

        // Derivatives
        calculate_derivatives(pointsN);
    }

    // kurvature
    calculate_kurvature();
}

// sentenceF Private methods

// Signal normalization
std::vector<PointR> sentenceF::normalizaAspect(const std::vector<Point>& puntos)
{
    double ymax = -100000, xmax = -100000, ymin = 100000, xmin = 100000;

    // Calculate x,y max and min
    for (const auto& pt : puntos) {
        if (pt.y < ymin)
            ymin = pt.y;
        if (pt.y > ymax)
            ymax = pt.y;
        if (pt.x < xmin)
            xmin = pt.x;
        if (pt.x > xmax)
            xmax = pt.x;
    }
    // Prevent the ymin=ymax case (e.g. for "-" and ".")
    if (ymin < (ymax + .5) && ymin > (ymax - .5))
        ymax = ymin + 1;

    std::vector<PointR> trazoNorm;
    trazoNorm.reserve(puntos.size());

    for (const auto& pt : puntos) {
        const float TAM = 100;

        auto& p = trazoNorm.emplace_back(TAM * ((pt.x - xmin) / (ymax - ymin)), TAM * (pt.y - ymin) / (ymax - ymin));

        // Set in 'p' the attribute of last point of the stroke
        if (pt.getpu())
            p.setpu();
    }

    return trazoNorm;
}

// HTK style derivatives
void sentenceF::calculate_derivatives(const std::vector<PointR>& points, bool norm)
{
    constexpr int tamW = 2;
    unsigned int sigma = 0;

    // Denominator calculation
    for (int i = 1; i <= tamW; i++)
        sigma += i * i;
    sigma = 2 * sigma;

    // First derivative
    for (int i = 0; i < points.size(); i++) {
        frames[i].dx = 0;
        frames[i].dy = 0;

        for (int c = 1; c <= tamW; c++) {
            const int ant_idx = std::max<int>(i - c, 0);
            const double context_ant_x = points[ant_idx].x;
            const double context_ant_y = points[ant_idx].y;

            const int post_idx = std::min<int>(i + c, points.size() - 1);
            const double context_post_x = points[post_idx].x;
            const double context_post_y = points[post_idx].y;

            frames[i].dx += c * (context_post_x - context_ant_x) / sigma;
            frames[i].dy += c * (context_post_y - context_ant_y) / sigma;

            // ---------------------------------------------------
            if (norm) {
                const double module = sqrt(frames[i].dx * frames[i].dx + frames[i].dy * frames[i].dy);
                if (module > 0) {
                    frames[i].dx /= module;
                    frames[i].dy /= module;
                }
            }
            // ---------------------------------------------------
        }

        if (fabs(frames[i].dx) < FLT_MIN)
            frames[i].dx = 0.0;
        if (fabs(frames[i].dy) < FLT_MIN)
            frames[i].dy = 0.0;
    }

    // Second derivative
    for (int i = 0; i < points.size(); i++) {
        double context_ant_dx, context_ant_dy, context_post_dx, context_post_dy;
        frames[i].ax = 0;
        frames[i].ay = 0;
        for (int c = 1; c <= tamW; c++) {
            const int ant_idx = std::max<int>(i - c, 0);
            const double context_ant_dx = frames[ant_idx].dx;
            const double context_ant_dy = frames[ant_idx].dy;

            const int post_idx = std::min<int>(i + c, points.size() - 1);
            const double context_post_dx = frames[post_idx].dx;
            const double context_post_dy = frames[post_idx].dy;

            frames[i].ax += c * (context_post_dx - context_ant_dx) / sigma;
            frames[i].ay += c * (context_post_dy - context_ant_dy) / sigma;
        }

        if (fabs(frames[i].ax) < FLT_MIN)
            frames[i].ax = 0.0;
        if (fabs(frames[i].ay) < FLT_MIN)
            frames[i].ay = 0.0;
    }
}

void sentenceF::calculate_kurvature()
{
    for (auto& curframe : frames) {
        const double norma = sqrt(curframe.dx * curframe.dx + curframe.dy * curframe.dy);
        curframe.k = curframe.dx * curframe.ay - curframe.ax * curframe.dy;
        if (norma != 0)
            curframe.k /= norma * norma * norma;
    }
}
