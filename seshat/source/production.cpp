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
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <grammar.hpp>
#include <internal_hypothesis.hpp>
#include <iostream>
#include <production.hpp>

using namespace seshat;

#define PSOLAP 0.75
#define PENALTY 0.05

// Aux functions

static int check_str(std::string_view str, std::string_view pat)
{
    const auto out = str.find(pat);
    if (out == std::string_view::npos)
        return -1;
    else
        return out;
}

//
// ProductionB methods
//

ProductionB::ProductionB(int s, int a, int b)
    : S{ s }
    , A{ a }
    , B{ b }
{
}

ProductionB::ProductionB(int s, int a, int b, float pr, const std::string& out)
    : outStr{ out }
    , S{ s }
    , A{ a }
    , B{ b }
{
    prior = pr > 0.0 ? log(pr) : -FLT_MAX;

    setMerges('C');
}

// Percentage of the are of regin A that overlaps with region B
float ProductionB::solape(InternalHypothesis* a, InternalHypothesis* b)
{
    int x = std::max(a->parent->x, b->parent->x);
    int y = std::max(a->parent->y, b->parent->y);
    int s = std::min(a->parent->s, b->parent->s);
    int t = std::min(a->parent->t, b->parent->t);

    if (s >= x && t >= y) {
        float aSolap = (s - x + 1.0) * (t - y + 1.0);
        float aTotal = (a->parent->s - a->parent->x + 1.0) * (a->parent->t - a->parent->y + 1.0);

        return aSolap / aTotal;
    }

    return 0.0;
}

bool ProductionB::check_out()
{
    if (check_str(outStr, "$1") < 0 && check_str(outStr, "$2") < 0)
        return false;
    return true;
}

const std::string& ProductionB::get_outstr() const
{
    return outStr;
}

void ProductionB::printOut(std::ostream& os, Grammar& G, const InternalHypothesis* H)
{
    if (outStr.empty())
        return;

    std::string_view outStrv = outStr;

    int pd1 = check_str(outStrv, "$1");
    int pd2 = check_str(outStrv, "$2");

    int i = 0;
    if (pd2 >= 0 && pd1 >= 0 && pd2 < pd1) {
        os << outStrv.substr(i, pd2 - i);
        i = pd2 + 2;

        if (H->hd->clase < 0)
            H->hd->prod->printOut(os, G, H->hd);
        else
            os << H->hd->pt->getTeX(H->hd->clase);

        os << outStrv.substr(i, pd1 - i);
        i = pd1 + 2;

        if (H->hi->clase < 0)
            H->hi->prod->printOut(os, G, H->hi);
        else
            os << H->hi->pt->getTeX(H->hi->clase);
    } else {
        if (pd1 >= 0) {
            os << outStrv.substr(i, pd1 - i);
            i = pd1 + 2;

            if (H->hi->clase < 0)
                H->hi->prod->printOut(os, G, H->hi);
            else
                os << H->hi->pt->getTeX(H->hi->clase);
        }
        if (pd2 >= 0) {
            os << outStrv.substr(i, pd2 - i);
            i = pd2 + 2;

            if (H->hd->clase < 0)
                H->hd->prod->printOut(os, G, H->hd);
            else
                os << H->hd->pt->getTeX(H->hd->clase);
        }
    }

    os << outStrv.substr(i);
}

void ProductionB::setMerges(char c)
{
    merge_cen = c;
}

void ProductionB::mergeRegions(InternalHypothesis* a, InternalHypothesis* b, InternalHypothesis* s)
{
    switch (merge_cen) {
    case 'A': // Data InternalHypothesis a
        s->lcen = a->lcen;
        s->rcen = a->rcen;
        break;
    case 'B': // Data InternalHypothesis b
        s->lcen = b->lcen;
        s->rcen = b->rcen;
        break;
    case 'C': // Center point
        s->lcen = (a->parent->y + a->parent->t) / 2;
        s->rcen = (b->parent->y + b->parent->t) / 2;
        break;
    case 'M': // Mean of both centers
        s->lcen = (a->lcen + b->lcen) / 2; // a->lcen;
        s->rcen = (a->rcen + b->rcen) / 2; // b->rcen;
        break;
    default:
        std::cerr << "Error: Unrecognized option '" << merge_cen << "' in merge regions\n";
        throw std::runtime_error("Error: Unrecognized option in merge regions");
    }
}

char ProductionH::tipo() const
{
    return 'H';
}

char ProductionV::tipo() const
{
    return 'V';
}

char ProductionVe::tipo() const
{
    return 'e';
}

char ProductionSSE::tipo() const
{
    return 'S';
}

char ProductionSup::tipo() const
{
    return 'P';
}

char ProductionSub::tipo() const
{
    return 'B';
}

char ProductionIns::tipo() const
{
    return 'I';
}

char ProductionMrt::tipo() const
{
    return 'M';
}

//
// ProductionT methods
//

ProductionT::ProductionT(int s, int nclases)
{
    S = s;
    N = nclases;
    dat.resize(N);
}

void ProductionT::setClase(int k, float pr, const std::string& tex, char mlt)
{
    dat[k].clases = true;
    if (!dat[k].texStr.empty())
        std::cerr << "WARNING: Terminal " << k << " redefined with label '" << tex << "' (previously '" << dat[k].texStr << "')\n";
    else {
        dat[k].texStr = tex;
        dat[k].texStr[tex.size()] = 0;
        dat[k].probs = pr > 0.0 ? log(pr) : -FLT_MAX;
        dat[k].mltype = mlt;
    }
}

bool ProductionT::getClase(int k) const
{
    return dat[k].clases;
}

const char* ProductionT::getTeX(int k) const
{
    return dat[k].texStr.c_str();
}

char ProductionT::getMLtype(int k) const
{
    return dat[k].mltype;
}

float ProductionT::getPrior(int k) const
{
    return dat[k].probs;
}

int ProductionT::getNoTerm() const
{
    return S;
}
