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
#ifndef _TABLECYK_
#define _TABLECYK_

#include "cellcyk.hpp"
#include "internal_hypothesis.hpp"
#include <array>
#include <cfloat>
#include <climits>
#include <cstdio>
#include <map>
#include <optional>
#include <vector>

namespace seshat {

// Structure to handle coordinates

struct coo {
    int x, y, s, t;

    coo()
    {
        x = y = s = t = -1;
    }

    coo(int a, int b, int c, int d)
    {
        x = a;
        y = b;
        s = c;
        t = d;
    }

    bool operator==(coo& R) const
    {
        return x == R.x && y == R.y && s == R.s && t == R.t;
    }

    bool operator<(const coo& R) const
    {
        if (x < R.x)
            return true;
        if (x == R.x) {
            if (y < R.y)
                return true;
            if (y == R.y) {
                if (s < R.s)
                    return true;
                if (s == R.s)
                    if (t < R.t)
                        return true;
            }
        }
        return false;
    }
};


struct InternalOptHypothesis {
    // InternalHypothesis that accounts for the target (input) math expression
    std::optional<InternalHypothesis> Target;
    // Percentage of strokes covered by the most likely hypothesis (target)
    int pm_comps;
};
class TableCYK {
    std::vector<CellCYK*> T;
    std::vector<std::map<coo, CellCYK*>> TS;
    int N, K;

public:
    static inline constexpr int NumHypotheses = 3;
    TableCYK(int n, int k);
    ~TableCYK();

    InternalHypothesis* getMLH(int n);
    CellCYK* get(int n);
    int size(int n);
    void updateTarget(const InternalHypothesis& H);
    void add(int n, CellCYK* celda, int noterm_id, bool* esinit);

private:
    std::array<InternalOptHypothesis, NumHypotheses> Targets;
};

}

#endif
