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
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <numeric>
#include <queue>
#include <samples.hpp>
#include <vector>

using namespace seshat;

#define PI 3.14159265

static bool isRelation(const char* str)
{
    if (!strcmp(str, "Hor"))
        return true;
    if (!strcmp(str, "Sub"))
        return true;
    if (!strcmp(str, "Sup"))
        return true;
    if (!strcmp(str, "Ver"))
        return true;
    if (!strcmp(str, "Ins"))
        return true;
    if (!strcmp(str, "Mrt"))
        return true;
    return false;
}

void Samples::clearAll()
{
    vmedx.clear();
    vmedy.clear();
    dataon.clear();
    stk_dis.img.clear();
    stk_dis.width = 0;
    stk_dis.height = 0;
    pix_stk.img.clear();
    pix_stk.width = 0;
    pix_stk.height = 0;
    dataoff.img.clear();
    dataoff.width = 0;
    dataoff.height = 0;
}
void Samples::makeReady()
{
    ox = INT_MAX;
    oy = INT_MAX;
    os = -INT_MAX;
    ot = -INT_MAX;
    for (auto& datapoint : dataon) {
        // Compute bouding box
        if (datapoint.rx < ox)
            ox = datapoint.rx;
        if (datapoint.ry < oy)
            oy = datapoint.ry;
        if (datapoint.rs > os)
            os = datapoint.rs;
        if (datapoint.rt > ot)
            ot = datapoint.rt;

        // Compute centroid
        datapoint.cx = 0;
        datapoint.cy = 0;
        const int np_end = datapoint.getNPoints();
        for (int np = 0; np < np_end; np++) {
            Point* pto = datapoint.get(np);
            datapoint.cx += pto->x;
            datapoint.cy += pto->y;
        }
        datapoint.cx /= np_end;
        datapoint.cy /= np_end;
    }

    RX = 0;
    RY = 0;

    // Render image representation
    render();
}

int Samples::get(int x, int y)
{
    return dataoff.img[y * dataoff.width + x];
}

Stroke& Samples::getStroke(int i)
{
    return dataon[i];
}

int Samples::dimX()
{
    return dataoff.width;
}

int Samples::dimY()
{
    return dataoff.height;
}

int Samples::nStrokes()
{
    return (int)dataon.size();
}

void Samples::render_img(const char* out)
{
    FILE* frender = fopen(out, "w");
    if (!frender) {
        fprintf(stderr, "WARNING: Error creating file '%s'\n", out);
        return;
    }

    fprintf(frender, "P2\n%d %d\n255\n", dataoff.width, dataoff.height);
    for (int i = 0; i < dataoff.height; i++) {
        for (int j = 0; j < dataoff.width; j++)
            fprintf(frender, " %3d", dataoff.img[i * dataoff.width + j]);
        fprintf(frender, "\n");
    }
    fclose(frender);
}

void Samples::detRefSymbol()
{
    vmedx.clear();
    vmedy.clear();

    int nregs = 0, lAr;
    float mAr = 0;
    RX = 0, RY = 0;

    const int numStrk = nStrokes();
    // Compute reference symbol for normalization
    for (int i = 0; i < numStrk; i++) {
        int ancho = dataon[i].rs - dataon[i].rx + 1;
        int alto = dataon[i].rt - dataon[i].ry + 1;
        float aspectratio = (float)ancho / alto;
        int area = ancho * alto;

        vmedx.push_back(ancho);
        vmedy.push_back(alto);

        mAr += area;
        if (aspectratio >= 0.25 && aspectratio <= 4.0) {
            RX += ancho;
            RY += alto;
            nregs++;
        }
    }

    // Average area
    mAr /= vmedx.size();
    lAr = (int)(sqrt(mAr) + 0.5);
    lAr *= 0.9;

    if (nregs > 0) {
        RX /= nregs;
        RY /= nregs;
    } else {
        for (int i = 0; i < numStrk; i++) {
            int ancho = dataon[i].rs - dataon[i].rx + 1;
            int alto = dataon[i].rt - dataon[i].ry + 1;

            RX += ancho;
            RY += alto;
            nregs++;
        }
        RX /= nregs;
        RY /= nregs;
    }

    // Compute median
    sort(vmedx.begin(), vmedx.end());
    sort(vmedy.begin(), vmedy.end());

    // Reference is the average of (mean,median,avg_area)
    RX = (RX + vmedx[vmedx.size() / 2] + lAr) / 3.0;
    RY = (RY + vmedy[vmedy.size() / 2] + lAr) / 3.0;
}

void Samples::setRegion(CellCYK& c, int nStk)
{
    c.ccc[nStk] = true;

    c.x = dataon[nStk].rx;
    c.y = dataon[nStk].ry;
    c.s = dataon[nStk].rs;
    c.t = dataon[nStk].rt;
}

void Samples::setRegion(CellCYK& c, std::span<const int> LT)
{

    c.x = c.y = INT_MAX;
    c.s = c.t = INT_MIN;

    for (auto it : LT) {
        const auto& datapt = dataon[it];
        c.ccc[it] = true;

        c.x = std::min<int>(datapt.rx, c.x);
        c.y = std::min<int>(datapt.ry, c.y);
        c.s = std::max<int>(datapt.rs, c.s);
        c.t = std::max<int>(datapt.rt, c.t);
    }
}

void Samples::getAVGstroke_size(float* avgw, float* avgh)
{
    *avgw = *avgh = 0.0;
    for (int i = 0; i < (int)dataon.size(); i++) {
        *avgw += dataon[i].rs - dataon[i].rx;
        *avgh += dataon[i].rt - dataon[i].ry;
    }
    *avgw /= (int)dataon.size();
    *avgh /= (int)dataon.size();
}

void Samples::print()
{
    printf("Number of strokes: %d\n", nStrokes());

    // for(int i=0; i<nStrokes(); i++) {
    //   printf("Stroke %d: (%d,%d)-(%d,%d)\n", i, dataon[i].rx, dataon[i].ry,
    // 	   dataon[i].rs, dataon[i].rt);
    // }
}

void Samples::linea(VectorImage& img, Point* pa, Point* pb, int stkid)
{
    const float dl = 3.125e-3;
    int dx = (int)pb->x - (int)pa->x;
    int dy = (int)pb->y - (int)pa->y;

    for (float l = 0.0; l < 1.0; l += dl) {
        int x = (int)pa->x + (int)(dx * l + 0.5);
        int y = (int)pa->y + (int)(dy * l + 0.5);

        for (int i = y - 1; i <= y + 1; i++)
            for (int j = x - 1; j <= x + 1; j++) {
                img.img[i * img.width + j] = 0;
                if (stkid >= 0)
                    pix_stk.img[i * pix_stk.width + j] = stkid;
            }
    }
}

void Samples::linea_pbm(VectorImage& img, Point* pa, Point* pb, int stkid)
{
    const float dl = 3.125e-3;
    int dx = (int)pb->x - (int)pa->x;
    int dy = (int)pb->y - (int)pa->y;

    for (float l = 0.0; l < 1.0; l += dl) {
        int x = (int)pa->x + (int)(dx * l + 0.5);
        int y = (int)pa->y + (int)(dy * l + 0.5);

        img.img[y * img.width + x] = 0;
    }
}

void Samples::render()
{
    int xMAX = -INT_MAX, yMAX = -INT_MAX, xMIN = INT_MAX, yMIN = INT_MAX;

    for (const auto& data_on_point : dataon) {
        const int np_end = data_on_point.getNPoints();
        for (int np = 0; np < np_end; np++) {
            const Point* pto = data_on_point.get(np);

            if (pto->x > xMAX)
                xMAX = pto->x;
            if (pto->x < xMIN)
                xMIN = pto->x;
            if (pto->y > yMAX)
                yMAX = pto->y;
            if (pto->y < yMIN)
                yMIN = pto->y;
        }
    }

    // Image dimensions
    int W = xMAX - xMIN + 1;
    int H = yMAX - yMIN + 1;
    float R = (float)W / H;

    // Keeping the aspect ratio (R), scale to 256 pixels height

    H = 256;
    W = (int)(H * R);
    if (W <= 0)
        W = 1;

    // Give some margin to the image
    W += 10;
    H += 10;

    // Create image
    auto& img = dataoff;
    img.height = H;
    img.width = W;
    img.img.resize(W * H, 255);

    // Create the structure that stores to which stroke belongs each pixel
    pix_stk.height = H;
    pix_stk.width = W;
    pix_stk.img.clear();
    pix_stk.img.resize(W * H, -1);

    // Render image
    Point pant, aux, *pto;
    for (int i = 0; i < nStrokes(); i++) {

        for (int np = 0; np < dataon[i].getNPoints(); np++) {
            pto = dataon[i].get(np);

            aux.x = 5 + (W - 10) * (float)(pto->x - xMIN) / (xMAX - xMIN + 1);
            aux.y = 5 + (H - 10) * (float)(pto->y - yMIN) / (yMAX - yMIN + 1);

            img.img[(int)aux.y * W + (int)aux.x] = 0;
            pix_stk.img[(int)aux.y * W + (int)aux.x] = i;

            // Draw a line between last point and current point
            if (np >= 1)
                linea(img, &pant, &aux, i);

            // Update last point
            pant.x = aux.x;
            pant.y = aux.y;
        }
    }

    IMGxMIN = xMIN;
    IMGyMIN = yMIN;
    IMGxMAX = xMAX;
    IMGyMAX = yMAX;
}

void Samples::renderStrokesPBM(std::span<const int> SL, VectorImage& img)
{
    // Parameters used to render images while training the RNN classifier
    const int REND_H = 40;
    const int REND_W = 200;
    const int OFFSET = 1;

    int xMin, yMin, xMax, yMax, H, W;
    xMin = yMin = INT_MAX;
    xMax = yMax = -INT_MAX;

    // Calculate bounding box of the region defined by the points
    for (const auto it : SL) {
        const auto& datapt = dataon[it];
        const auto nPoints = datapt.getNPoints();
        for (int i = 0; i < nPoints; i++) {
            const Point* p = datapt.get(i);

            if (p->x < xMin)
                xMin = p->x;
            if (p->y < yMin)
                yMin = p->y;
            if (p->x > xMax)
                xMax = p->x;
            if (p->y > yMax)
                yMax = p->y;
        }
    }

    // Image dimensions
    W = xMax - xMin + 1;
    H = yMax - yMin + 1;

    // Scale image to height REND_H pixels, keeping the aspect ratio
    W = REND_H * (float)W / H;
    H = REND_H;

    // If image is too wide (for example, a fraction bar) truncate width to REND_W
    if (W > REND_W)
        W = REND_W;

    // Enforce a minimum size of 3 in both dimensions: height and width
    if (H < 3)
        H = 3;
    if (W < 3)
        W = 3;

    // Create image
    img.height = H + OFFSET * 2;
    img.width = W + OFFSET * 2;

    img.img.resize(img.height * img.width, 255);

    Point pant, aux;

    if (SL.size() == 1 && dataon[SL.front()].getNPoints() == 1) {
        // A single point is represented with a full black image
        for (int i = OFFSET; i < H - OFFSET; i++)
            for (int j = OFFSET; j < W - OFFSET; j++)
                img.img[i * img.width + j] = 0;
    } else {
        for (const auto it : SL) {
            const auto& datapt = dataon[it];
            const auto nPoints = datapt.getNPoints();
            for (int i = 0; i < nPoints; i++) {
                const Point* p = datapt.get(i);

                aux.x = OFFSET + (W - 1) * (p->x - xMin) / (float)(xMax - xMin + 1);
                aux.y = OFFSET + (H - 1) * (p->y - yMin) / (float)(yMax - yMin + 1);

                img.img[(int)aux.y * img.width + (int)aux.x] = 0;

                // Draw a line between last point and current point
                if (i >= 1)
                    linea_pbm(img, &pant, &aux, -1);
                else if (i == 0 && nPoints == 1)
                    linea_pbm(img, &aux, &aux, -1);

                // Update last point
                pant = aux;
            }
        }
    }

    // Create smoothed image
    VectorImage img_smo = img;

    // Smooth AVG(3x3)
    for (int y = 0; y < img_smo.height; y++) {
        for (int x = 0; x < img_smo.width; x++) {
            for (int i = y - 1; i <= y + 1; i++) {
                for (int j = x - 1; j <= x + 1; j++) {
                    if (i >= 0 && j >= 0 && i < img_smo.height && j < img_smo.width) {
                        if (!(i == y && j == x))
                            img_smo.img[y * img_smo.width + x] += img.img[i * img_smo.width + j];
                    } else
                        img_smo.img[y * img_smo.width + x] += 255; // Background
                }
            }

            img_smo.img[y * img_smo.width + x] /= 9; // 3x3
        }
    }

    // Replace IMG with the smoothed image and free memory
    for (int idx = 0, lim = img.img.size(); idx < lim; ++idx) {
        img.img[idx] = img_smo.img[idx] < 255 ? 1 : 0;
    }
}

void Samples::getCentroids(CellCYK* cd, int* ce, int* as, int* ds)
{
    int regy = INT_MAX, regt = -INT_MAX, N = 0;
    *ce = 0;

    for (int i = 0; i < cd->nc; i++)
        if (cd->ccc[i]) {

            for (int j = 0; j < dataon[i].getNPoints(); j++) {
                Point* p = dataon[i].get(j);

                if (dataon[i].ry < regy)
                    regy = dataon[i].ry;
                if (dataon[i].rt > regt)
                    regt = dataon[i].rt;

                *ce += p->y;

                N++;
            }
        }

    *ce /= N;
    *as = (*ce + regt) / 2;
    *ds = (regy + *ce) / 2;
}

void Samples::compute_strokes_distances(int rx, int ry)
{
    // Create distances matrix NxN (strokes)
    stk_dis.width = nStrokes();
    stk_dis.height = nStrokes();
    stk_dis.img.resize(stk_dis.width * stk_dis.height, 0.0f);

    float aux_x = rx;
    float aux_y = ry;
    NORMF = sqrt(aux_x * aux_x + aux_y * aux_y);
    INF_DIST = FLT_MAX / NORMF;

    // Compute distance among every stroke.
    for (int i = 0; i < stk_dis.height; i++) {
        for (int j = i + 1; j < stk_dis.width; j++) {
            const float curval = stroke_distance(i, j) / NORMF;
            stk_dis.img[i * stk_dis.width + j] = curval;
            stk_dis.img[j * stk_dis.width + i] = curval;
        }
    }

#ifdef VERBOSE
    fprintf(stderr, "===INI Strokes Dist LIST===\n");
    for (int i = 0; i < nStrokes(); i++) {
        for (int j = 0; j < nStrokes(); j++) {
            if (i != j && stk_dis[i][j] < INF_DIST)
                fprintf(stderr, "%d -> %d: d=%.2f\n", i, j, stk_dis[i][j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "===END Strokes Dist LIST===\n\n");
    fprintf(stderr, "===INI DISTANCE MATRIX===\n");
    for (int i = 0; i < nStrokes(); i++) {
        for (int j = 0; j < nStrokes(); j++) {
            if (stk_dis[i][j] >= INF_DIST)
                fprintf(stderr, "   *  ");
            else
                fprintf(stderr, " %5.2f", stk_dis[i][j]);
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "===END DISTANCE MATRIX===\n");
#endif
}

float Samples::stroke_distance(int si, int sj)
{
    Point *pi, *pj, *min_i, *min_j;
    float dmin = FLT_MAX;

    for (int npi = 0; npi < dataon[si].getNPoints(); npi++) {
        pi = dataon[si].get(npi);

        for (int npj = 0; npj < dataon[sj].getNPoints(); npj++) {
            pj = dataon[sj].get(npj);

            float dis = (pi->x - pj->x) * (pi->x - pj->x) + (pi->y - pj->y) * (pi->y - pj->y);

            if (dis < dmin) {
                dmin = dis;
                min_i = pi;
                min_j = pj;
            }
        }
    }

    if (not_visible(si, sj, min_i, min_j))
        dmin = FLT_MAX;

    return dmin < FLT_MAX ? sqrt(dmin) : FLT_MAX;
}

float Samples::getDist(int si, int sj)
{
    if (si < 0 || sj < 0 || si >= nStrokes() || sj >= nStrokes()) {
        fprintf(stderr, "ERROR: stroke id out of range in getDist(%d,%d)\n", si, sj);
        exit(-1);
    }
    return stk_dis.img[si * stk_dis.width + sj];
}

// Go through the pixels from pi to pj checking that there is not a pixel that belongs
// to a stroke that is not si or sj. If so, then sj is not visible from si

bool Samples::not_visible(int si, int sj, Point* pi, Point* pj)
{

    Point pa, pb;
    // Coordinates in pixels of the rendered image
    pa.x = 5 + (dataoff.width - 10) * (float)(pi->x - IMGxMIN) / (IMGxMAX - IMGxMIN + 1);
    pa.y = 5 + (dataoff.height - 10) * (float)(pi->y - IMGyMIN) / (IMGyMAX - IMGyMIN + 1);
    pb.x = 5 + (dataoff.width - 10) * (float)(pj->x - IMGxMIN) / (IMGxMAX - IMGxMIN + 1);
    pb.y = 5 + (dataoff.height - 10) * (float)(pj->y - IMGyMIN) / (IMGyMAX - IMGyMIN + 1);

    const float dl = 3.125e-4;
    int dx = (int)pb.x - (int)pa.x;
    int dy = (int)pb.y - (int)pa.y;

    for (float l = 0.0; l < 1.0; l += dl) {
        int x = (int)pa.x + (int)(dx * l + 0.5);
        int y = (int)pa.y + (int)(dy * l + 0.5);

        if (dataoff.img[y * dataoff.width + x] == 0 && pix_stk.img[y * pix_stk.width + x] != si && pix_stk.img[y * pix_stk.width + x] != sj)
            return true;
    }

    return false;
}

bool Samples::visibility(std::span<const int> strokes_list)
{

    std::map<int, bool> visited;
    for (const auto it : strokes_list)
        visited.insert({ it, false });

    std::queue<int> Q;
    Q.push(strokes_list.front());
    visited[strokes_list.front()] = true;

    while (!Q.empty()) {
        const int id = Q.front();
        Q.pop();

        for (const auto it : strokes_list) {
            if (id == it || visited[it] || getDist(id, it) >= INF_DIST)
                continue;

            visited[it] = true;
            Q.push(it);
        }
    }

    for (const auto it : strokes_list) {
        if (!visited[it])
            return false;
    }

    return true;
}

float Samples::group_penalty(CellCYK* A, CellCYK* B)
{

    // Minimum or single-linkage clustering
    float dmin = FLT_MAX;
    for (int i = 0; i < A->nc; i++)
        if (A->ccc[i]) {
            for (int j = 0; j < B->nc; j++)
                if (B->ccc[j] && j != i && getDist(i, j) < dmin)
                    dmin = getDist(i, j);
        }

    return dmin;
}

void Samples::get_close_strokes(int id, std::vector<int>& L, float dist_th)
{
    std::vector<int> to_maybe_add(id);
    std::iota(to_maybe_add.begin(), to_maybe_add.end(), 0); // all currently possible

    // Add linked strokes with distance < dist_th
    for (auto& i : to_maybe_add) {
        if (getDist(id, i) >= dist_th)
            continue;

        L.push_back(i);
        i = -1;
    }

    // Add second degree distance < dist_th
    for (int it = 0, itend = L.size(); it < itend; ++it) {
        for (auto& i : to_maybe_add) {
            if (i < 0 || getDist(L[it], i) >= dist_th)
                continue;

            L.push_back(i);
            i = -1; // mark as unusable for next loop
        }
    }
}
