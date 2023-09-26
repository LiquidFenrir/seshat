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

#include "duration.hpp"
#include "grammar.hpp"
#include "path.hpp"
#include "production.hpp"
#include "samples.hpp"
#include "segmentation.hpp"
#include "sparel.hpp"
#include "symrec.hpp"
#include "tablecyk.hpp"
#include <memory>
#include <optional>
#include <seshat/hypothesis.hpp>
#include <vector>

namespace seshat {

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
    std::vector<CellCYK*> c1setH, c1setV, c1setU, c1setI, c1setM, c1setS;
    std::vector<int> close_list;
    std::vector<int> stks_list;
    std::vector<int> stkvec;
    unsigned maxHypothesis;

    // Private methods
    void loadSymRec(const fs::path& conf);

    void initCYKterms(Samples& m, TableCYK& tcyk, int N, int K);

    void combineStrokes(Samples& M, TableCYK& tcyk, int N);
    CellCYK* fusion(Samples& M, ProductionB* pd, InternalHypothesis* A, InternalHypothesis* B, int N, double prob);

#ifdef SESHAT_HYPOTHESIS_TREE
    // fill with tree representation of the input
    int makeTree(hypothesis& into, const InternalHypothesis* H, int id);
#else
    // fill with LaTeX string
    void makeLatex(hypothesis& into, const InternalHypothesis* H);
#endif

    void fillHypothesis(hypothesis& into, const InternalHypothesis* H);

public:
    meParser(const fs::path& conf);

    // Parse math expression
    void parse_me(Samples& M, std::vector<hypothesis>& output);
    void setMaxHypothesis(unsigned n);
    unsigned getMaxHypothesis() const;
};

}

#endif
