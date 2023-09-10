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

class gParser;

#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <unordered_set>
#include "production.h"
#include "gparser.h"
#include "symrec.h"

struct Grammar
{
  std::map<std::string, int> noTerminales;
  std::vector<int> initsyms;
  std::unique_ptr<bool[]> esInit;
  SymRec *sym_rec;
  std::unordered_set<std::string> operatorSet = {"-", "+", "\\pm", "\\times", "\\div", "\\cdot", "=", "\\neq", "\\leq", "\\lt", "\\gt", "\\geq",
                                       "\\in", "\\rightarrow", "COMMA", "\\forall", "\\exists", "\\int", "\\sum", "\\log", "\\lim",
                                       "\\sin", "\\cos", "\\tan", "\\lbrace", "\\lbracket", "\\lpar", "\\rbrace", "\\rbracket", "\\rpar",
                                       "\\prime", "\\vee", "\\wedge", "\\equiv", "\\cup"};

  std::vector<std::unique_ptr<ProductionB>> prodsH, prodsSup, prodsSub;
  std::vector<std::unique_ptr<ProductionB>> prodsV, prodsVe, prodsIns, prodsMrt, prodsSSE;
  std::vector<std::unique_ptr<ProductionT>> prodTerms;

  Grammar(char *conf, SymRec *SR);

  const char *key2str(int k);
  void addInitSym(char *str);
  void addNoTerminal(char *str);
  void addTerminal(float pr, char *S, char *T, char *tex);

  void addRuleH(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleV(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleVe(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleSup(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleSub(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleSSE(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleIns(float pr, char *S, char *A, char *B, char *out, char *merge);
  void addRuleMrt(float pr, char *S, char *A, char *B, char *out, char *merge);

  bool isOperator(const char *str) const;
};

#endif
