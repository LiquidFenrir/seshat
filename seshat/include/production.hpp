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
#ifndef _PRODUCTION_
#define _PRODUCTION_

#include <cstdio>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace seshat {

class Grammar;
class InternalHypothesis;

// Binary productions of the grammar (2D-PCFG)
class ProductionB {
protected:
    std::string outStr;
    char merge_cen;

public:
    ProductionB(int s, int a, int b);
    ProductionB(int s, int a, int b, float pr, const std::string& out);

    int S;
    int A, B;
    float prior;

    virtual ~ProductionB() = default;

    float solape(InternalHypothesis* a, InternalHypothesis* b);
    void printOut(std::ostream& os, Grammar& G, const InternalHypothesis* H);
    void setMerges(char c);
    void mergeRegions(InternalHypothesis* a, InternalHypothesis* b, InternalHypothesis* s);
    bool check_out();
    const std::string& get_outstr() const;

    // Pure virtual functions
    virtual char tipo() const = 0;
};

// Production S -> A : B
struct ProductionH : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A / B
struct ProductionV : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A /u B
struct ProductionU : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A /e B
struct ProductionVe : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A sse B
struct ProductionSSE : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A ^ B
struct ProductionSup : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A _ B
struct ProductionSub : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A ins B
struct ProductionIns : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production: S -> A mroot B
struct ProductionMrt : public ProductionB {
    using ProductionB::ProductionB;

    char tipo() const override;
};

// Production S -> term ( N clases )
struct ProductionT {
    int S;
    struct InternalData {
        std::string texStr{};
        float probs{ 0 };
        bool clases{ false };
        char mltype{ 'z' };
    };
    std::vector<InternalData> dat;
    int N;

public:
    ProductionT(int s, int nclases);

    void setClase(int k, float pr, const std::string& tex, char mlt);
    bool getClase(int k) const;
    float getPrior(int k) const;
    const char* getTeX(int k) const;
    char getMLtype(int k) const;
    int getNoTerm() const;
};

}

#endif
