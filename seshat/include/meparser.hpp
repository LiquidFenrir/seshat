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
#ifndef _MEPARSER_
#define _MEPARSER_

#include "cellcyk.hpp"
#include "duration.hpp"
#include "grammar.hpp"
#include "hypothesis.hpp"
#include "sample.hpp"
#include "segmentation.hpp"
#include "sparel.hpp"
#include "symrec.hpp"
#include "tablecyk.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>

class meParser {

    std::unique_ptr<Grammar> G;

    int max_strokes;
    float clusterF, segmentsTH;
    float ptfactor, pbfactor, rfactor;
    float qfactor, dfactor, gfactor, InsPen;

    std::unique_ptr<SymRec> sym_rec;
    std::unique_ptr<GMM> gmm_spr;
    std::optional<DurationModel> duration;
    std::optional<SegmentationModelGMM> segmentation;

    // Private methods
    void loadSymRec(const char* conf);

    void initCYKterms(Sample& m, TableCYK& tcyk, int N, int K);

    void combineStrokes(Sample& M, TableCYK& tcyk, int N);
    CellCYK* fusion(Sample& M, ProductionB* pd, Hypothesis* A, Hypothesis* B, int N, double prob);

public:
    meParser(const char* conf);

    // Parse math expression
    void parse_me(Sample& M);

    // Output formatting methods
    void print_symrec(Hypothesis* H);
    void print_latex(Hypothesis* H);
};

#endif
