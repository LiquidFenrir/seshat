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
#ifndef _LOGSPACE_
#define _LOGSPACE_

#include "cellcyk.hpp"
#include <cstdio>
#include <list>

namespace seshat {

class LogSpace {
    int N;
    int RX, RY;
    std::unique_ptr<CellCYK*[]> data;

    void quicksort(CellCYK** vec, int ini, int fin);
    int partition(CellCYK** vec, int ini, int fin);
    void bsearch(int sx, int sy, int ss, int st, std::vector<CellCYK*>& set);
    void bsearchStv(int sx, int sy, int ss, int st, std::vector<CellCYK*>& set, bool U_V, CellCYK* cd);
    void bsearchHBP(int sx, int sy, int ss, int st, std::vector<CellCYK*>& set, CellCYK* cd);

public:
    LogSpace(CellCYK* c, int nr, int dx, int dy);

    void getH(CellCYK* c, std::vector<CellCYK*>& set);
    void getV(CellCYK* c, std::vector<CellCYK*>& set);
    void getU(CellCYK* c, std::vector<CellCYK*>& set);
    void getI(CellCYK* c, std::vector<CellCYK*>& set);
    void getM(CellCYK* c, std::vector<CellCYK*>& set);
    void getS(CellCYK* c, std::vector<CellCYK*>& set);
};

}

#endif
