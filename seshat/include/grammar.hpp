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
#ifndef _GRAMMAR_
#define _GRAMMAR_

#include "path.hpp"
#include "production.hpp"
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace seshat {

class SymRec;

struct Grammar {
    std::map<std::string, int> noTerminales;
    std::vector<int> initsyms;
    std::unique_ptr<bool[]> esInit;
    SymRec* sym_rec;

    std::vector<std::unique_ptr<ProductionB>> prodsH, prodsSup, prodsSub;
    std::vector<std::unique_ptr<ProductionB>> prodsV, prodsVe, prodsIns, prodsMrt, prodsSSE;
    std::vector<std::unique_ptr<ProductionT>> prodTerms;

    Grammar(const fs::path& conf, SymRec* SR);

    const char* key2str(int k);
    void addInitSym(const std::string& str);
    void addNoTerminal(const std::string& str);
    void addTerminal(float pr, const std::string& S, const std::string& T, const std::string& tex);

    void addRuleH(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleV(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleVe(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSup(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSub(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSSE(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleIns(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleMrt(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
};

}

#endif
