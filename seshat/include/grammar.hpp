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

#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace seshat {

class SymRec;
class ProductionB;
class ProductionT;

struct Grammar {
    std::map<std::string, int> noTerminales;
    std::vector<int> initsyms;
    std::unique_ptr<bool[]> esInit;
    SymRec* sym_rec;
    std::unordered_set<std::string> operatorSet = { "-", "+", "\\pm", "\\times", "\\div", "\\cdot", "=", "\\neq", "\\leq", "\\lt", "\\gt", "\\geq",
                                                    "\\in", "\\rightarrow", "COMMA", "\\forall", "\\exists", "\\int", "\\sum", "\\log", "\\lim",
                                                    "\\sin", "\\cos", "\\tan", "\\lbrace", "\\lbracket", "\\lpar", "\\rbrace", "\\rbracket", "\\rpar",
                                                    "\\prime", "\\vee", "\\wedge", "\\equiv", "\\cup" };

    std::vector<std::unique_ptr<ProductionB>> prodsH, prodsSup, prodsSub;
    std::vector<std::unique_ptr<ProductionB>> prodsV, prodsVe, prodsIns, prodsMrt, prodsSSE;
    std::vector<std::unique_ptr<ProductionT>> prodTerms;

    Grammar(const char* conf, SymRec* SR);

    const char* key2str(int k);
    void addInitSym(const char* str);
    void addNoTerminal(const char* str);
    void addTerminal(float pr, const char* S, const char* T, const char* tex);

    void addRuleH(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleV(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleVe(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSup(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSub(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleSSE(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleIns(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);
    void addRuleMrt(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge);

    bool isOperator(const std::string& str) const;
};

}

#endif
