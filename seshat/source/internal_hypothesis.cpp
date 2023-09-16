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

#include <cellcyk.hpp>
#include <internal_hypothesis.hpp>

using namespace seshat;

InternalHypothesis::InternalHypothesis(int c, double p, CellCYK* cd, int nt)
{
    clase = c;
    pr = p;
    hi = nullptr;
    hd = nullptr;
    prod = nullptr;
    prod_sse = nullptr;
    pt = nullptr;
    lcen = 0;
    rcen = 0;
    parent = cd;
    ntid = nt;
}

void InternalHypothesis::copy(const InternalHypothesis& H)
{
    clase = H.clase;
    pr = H.pr;
    hi = H.hi;
    hd = H.hd;
    prod = H.prod;
    prod_sse = H.prod_sse;
    pt = H.pt;
    lcen = H.lcen;
    rcen = H.rcen;
    parent = H.parent;
    ntid = H.ntid;
}
