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
#ifndef _SAMPLE_
#define _SAMPLE_

class SymRec;
class TableCYK;

#include "cellcyk.hpp"
#include "grammar.hpp"
#include "stroke.hpp"
#include "symrec.hpp"
#include "tablecyk.hpp"
#include "vectorimage.hpp"
#include <climits>
#include <cstdio>
#include <span>
#include <vector>

// Segmentation hypothesis
struct SegmentHyp {
    std::vector<int> stks; // List of strokes

    // Bounding box (online coordinates)
    int rx, ry; // Top-left
    int rs, rt; // Bottom-right

    int cen;
};

class Sample {
    std::vector<Stroke> dataon;
    VectorImagef stk_dis;

    VectorImage dataoff;
    int X, Y;
    int IMGxMIN, IMGyMIN, IMGxMAX, IMGyMAX;
    VectorImage pix_stk;

    void loadSCGInk(const char* str);

    void linea(VectorImage& img, Punto* pa, Punto* pb, int stkid);
    void linea_pbm(VectorImage& img, Punto* pa, Punto* pb, int stkid);
    bool not_visible(int si, int sj, Punto* pi, Punto* pj);

public:
    // Normalized reference symbol size
    int RX, RY;
    float INF_DIST; // Infinite distance value (visibility)
    float NORMF; // Normalization factor for distances

    int ox, oy, os, ot; // Online bounding box
    int bx, by, bs, bt; // Offline bounding box

    Sample(const char* in);

    int dimX();
    int dimY();
    int nStrokes();
    int get(int x, int y);
    Stroke& getStroke(int i);

    void getCentroids(CellCYK* cd, int* ce, int* as, int* ds);
    void getAVGstroke_size(float* avgw, float* avgh);

    void detRefSymbol();
    void compute_strokes_distances(int rx, int ry);
    float stroke_distance(int si, int sj);
    float getDist(int si, int sj);
    void get_close_strokes(int id, std::vector<int>& L, float dist_th);

    float group_penalty(CellCYK* A, CellCYK* B);
    bool visibility(std::span<const int> strokes_list);

    void setRegion(CellCYK& c, int nComp);
    void setRegion(CellCYK& c, std::span<const int> LT);

    VectorImage render();
    void renderStrokesPBM(std::span<const int> SL, VectorImage& img);

    void render_img(const char* out);

    void print();
};

#endif