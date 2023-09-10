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
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <string>
#include <cstring>
#include <map>
#include <vector>
#include <algorithm>
#include "grammar.h"
#include "tablecyk.h"
#include "logspace.h"
#include "sparel.h"

#define MIN_SPR_PR 0.01

// Aux functions

void error(const char *msg)
{
  fprintf(stderr, "Grammar err[%s]\n", msg);
  exit(-1);
}

void error(const char *msg, const char *str)
{
  char linea[1024];
  sprintf(linea, "Grammar err[%s]\n", msg);
  fprintf(stderr, linea, str);
  exit(-1);
}

//
// Grammar methods
//

Grammar::Grammar(char *path, SymRec *sr)
{
  // Load grammar file
  FILE *fd = fopen(path, "r");
  if (!fd)
  {
    fprintf(stderr, "Error loading grammar '%s'\n", path);
    exit(-1);
  }

  // Get the path's prefix to determine relative locations
  int i = strlen(path) - 1;
  while (i >= 0 && path[i] != '/')
    i--;

  path[i + 1] = 0;

  // Save the symbol recognizer to convert between LaTeX and symbol id
  sym_rec = sr;

  gParser GP(this, fd);

  fclose(fd);

  esInit = std::make_unique<bool[]>(noTerminales.size());
  std::fill_n(esInit.get(), noTerminales.size(), false);

  for (const auto it : initsyms)
    esInit[it] = true;
}

void Grammar::addInitSym(char *str)
{
  std::string s(str);
  const auto it = noTerminales.find(s);
  if (it == noTerminales.end())
    error("addInitSym: Non-terminal '%s' not defined.", str);

  initsyms.push_back(it->second);
}

void Grammar::addNoTerminal(char *str)
{
  int key = noTerminales.size();
  noTerminales[str] = key;
}

void Grammar::addTerminal(float pr, char *S, char *T, char *tex)
{
  std::string str(S);
  const auto not_it = noTerminales.find(str);
  if (not_it == noTerminales.end())
    error("addTerminal: Non-terminal '%s' not defined.", S);

  bool create = true;
  for (const auto& it : prodTerms)
  {
    if (it->getNoTerm() == not_it->second)
    {
      int id = sym_rec->keyClase(T);
      if (id >= 0)
      {
        char inkMLClass = isOperator(tex) ? 'o' : 'i';
        it->setClase(id, pr, tex, inkMLClass);
      }
      else
        fprintf(stderr, "ERROR: %s -> %s (id < 0)\n", S, T);

      create = false;
      break;
    }
  }

  if (create)
  {
    auto pt = std::make_unique<ProductionT>(not_it->second, sym_rec->getNClases());

    int id = sym_rec->keyClase(T);
    if (id >= 0)
    {
        char inkMlClass = isOperator(tex) ? 'o' : 'i';
        pt->setClase(id, pr, tex, inkMlClass);
    }
    else
      fprintf(stderr, "ERROR: %s -> %s (id < 0)\n", S, T);

    prodTerms.push_back(std::move(pt));
  }
}

void Grammar::addRuleH(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionH(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);
  prodsH.emplace_back(pd);
}

void Grammar::addRuleV(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionV(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);
  prodsV.emplace_back(pd);
}

void Grammar::addRuleVe(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionVe(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);

  prodsVe.emplace_back(pd);
}

void Grammar::addRuleSSE(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionSSE(s_it->second, a_it->second, b_it->second, pr, out);
  pd->setMerges(merge[0]);

  prodsSSE.emplace_back(pd);
}

void Grammar::addRuleSup(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionSup(s_it->second, a_it->second, b_it->second, pr, out);
  pd->setMerges(merge[0]);

  prodsSup.emplace_back(pd);
}

void Grammar::addRuleSub(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionSub(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);

  prodsSub.emplace_back(pd);
}

void Grammar::addRuleIns(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionIns(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);

  prodsIns.emplace_back(pd);
}

void Grammar::addRuleMrt(float pr, char *S, char *A, char *B, char *out, char *merge)
{
  const auto s_it = noTerminales.find(S);
  if (s_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", S);
  const auto a_it = noTerminales.find(A);
  if (a_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", A);
  const auto b_it = noTerminales.find(B);
  if (b_it == noTerminales.end())
    error("Rule: Non-terminal '%s' not defined.", B);

  ProductionB *pd = new ProductionMrt(s_it->second, a_it->second, b_it->second, pr, out);

  pd->setMerges(merge[0]);

  prodsMrt.emplace_back(pd);
}

const char *Grammar::key2str(int k)
{
  for (map<string, int>::iterator it = noTerminales.begin(); it != noTerminales.end(); it++)
  {
    if (it->second == k)
      return it->first.c_str();
  }
  return "NULL";
}

bool Grammar::isOperator(const char *str) const
{
  return operatorSet.find(str) != operatorSet.end();
}