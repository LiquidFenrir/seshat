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
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gparser.hpp>
#include <grammar.hpp>
#include <map>
#include <sparel.hpp>
#include <string>
#include <symrec.hpp>
#include <tablecyk.hpp>
#include <vector>

using namespace seshat;

#define MIN_SPR_PR 0.01

// Aux functions

void error(const char* msg)
{
    static char tmp[1024];
    sprintf(tmp, "Grammar err[%s]\n", msg);
    fputs(tmp, stderr);
    throw std::runtime_error(tmp);
}

void error(const char* msg, std::string_view str)
{
    static char tmp[1024], tmp2[1024];
    sprintf(tmp, "Grammar err[%s]\n", msg);
    sprintf(tmp2, tmp, (int)str.size(), str.data());
    fputs(tmp2, stderr);
    throw std::runtime_error(tmp2);
}

//
// Grammar methods
//

Grammar::Grammar(const fs::path& path, SymRec* sr)
{
    // Load grammar file
    std::ifstream fd(path);
    if (!fd) {
        std::cerr << "Error loading grammar '" << path << "'\n";
        throw std::runtime_error("Error loading grammar");
    }

    // Save the symbol recognizer to convert between LaTeX and symbol id
    sym_rec = sr;

    gParser GP(this, fd);

    esInit = std::make_unique<bool[]>(noTerminales.size());
    std::fill_n(esInit.get(), noTerminales.size(), false);

    for (const auto it : initsyms)
        esInit[it] = true;
}

void Grammar::addInitSym(const std::string& str)
{
    const auto it = noTerminales.find(str);
    if (it == noTerminales.end())
        error("addInitSym: Non-terminal '%.*s' not defined.", str);

    initsyms.push_back(it->second);
}

void Grammar::addNoTerminal(const std::string& str)
{
    int key = noTerminales.size();
    noTerminales[str] = key;
}

void Grammar::addTerminal(float pr, const std::string& S, const std::string& T, const std::string& tex)
{
    const auto not_it = noTerminales.find(S);
    if (not_it == noTerminales.end())
        error("addTerminal: Non-terminal '%.*s' not defined.", S);

    const std::string str_T = T;
    const std::string str_tex = tex;
    bool create = true;
    for (const auto& it : prodTerms) {
        if (it->getNoTerm() == not_it->second) {
            int id = sym_rec->keyClase(str_T);
            if (id >= 0) {
                it->setClase(id, pr, tex, 'i');
            } else
                fprintf(stderr, "ERROR: %s -> %s (id < 0)\n", S, T);

            create = false;
            break;
        }
    }

    if (create) {
        auto pt = std::make_unique<ProductionT>(not_it->second, sym_rec->getNClases());

        int id = sym_rec->keyClase(str_T);
        if (id >= 0) {
            pt->setClase(id, pr, tex, 'i');
        } else
            fprintf(stderr, "ERROR: %s -> %s (id < 0)\n", S, T);

        prodTerms.push_back(std::move(pt));
    }
}

void Grammar::addRuleH(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionH>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsH.push_back(std::move(pd));
}

void Grammar::addRuleV(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionV>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsV.push_back(std::move(pd));
}

void Grammar::addRuleVe(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionVe>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsVe.push_back(std::move(pd));
}

void Grammar::addRuleSSE(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionSSE>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsSSE.push_back(std::move(pd));
}

void Grammar::addRuleSup(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionSup>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsSup.push_back(std::move(pd));
}

void Grammar::addRuleSub(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionSub>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsSub.push_back(std::move(pd));
}

void Grammar::addRuleIns(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionIns>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsIns.push_back(std::move(pd));
}

void Grammar::addRuleMrt(float pr, const std::string& S, const std::string& A, const std::string& B, const std::string& out, const std::string& merge)
{
    const auto s_it = noTerminales.find(S);
    if (s_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", S);
    const auto a_it = noTerminales.find(A);
    if (a_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", A);
    const auto b_it = noTerminales.find(B);
    if (b_it == noTerminales.end())
        error("Rule: Non-terminal '%.*s' not defined.", B);

    auto pd = std::make_unique<ProductionMrt>(s_it->second, a_it->second, b_it->second, pr, out);

    pd->setMerges(merge[0]);
    prodsMrt.push_back(std::move(pd));
}

const char* Grammar::key2str(int k)
{
    for (const auto& [pair_k, pair_val] : noTerminales) {
        if (pair_val == k)
            return pair_k.c_str();
    }
    return "NULL";
}
