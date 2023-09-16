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

#include <cstdlib>
#include <cstring>
#include <gparser.hpp>
#include <grammar.hpp>

using namespace seshat;

#define SIZE 512

gParser::gParser(Grammar* gram, FILE* fd)
{
    g = gram;

    parse(fd);
}

bool gParser::isFillChar(char c)
{
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return true;
    default:
        return false;
    }
}

std::vector<std::string> gParser::split(const char* str)
{
    char tokensaux[2 * SIZE];
    int n = 0, i = 0, j = 0;

    while (isFillChar(str[i]))
        i++;

    while (str[i]) {
        if (str[i] == '\"') {
            i++;
            while (str[i] && str[i] != '\"') {
                tokensaux[j] = str[i];
                i++;
                j++;
            }
            i++;
        } else {
            while (str[i] && !isFillChar(str[i])) {
                tokensaux[j] = str[i];
                i++;
                j++;
            }
        }
        tokensaux[j++] = 0;
        n++;
        while (str[i] && isFillChar(str[i]))
            i++;
    }

    std::vector<std::string> toks;
    toks.reserve(n);
    for (i = 0, j = 0; i < n; i++) {
        toks.emplace_back(&tokensaux[j]);
        j += toks.back().size() + 1;
    }

    return toks;
}

bool gParser::nextLine(FILE* fd, char* lin)
{
    do {
        if (fgets(lin, SIZE, fd) == NULL)
            return false;
    } while (lin[0] == '#' || strlen(lin) <= 1);

    return true;
}

void gParser::parse(FILE* fd)
{
    static char linea[SIZE], tok1[SIZE], tok2[SIZE], aux[SIZE];

    // Read nonterminal symbols
    while (nextLine(fd, linea) && strcmp(linea, "START\n")) {
        sscanf(linea, "%s", tok1);
        g->addNoTerminal(tok1);
    }

    // Read start symbol(s) of the grammar
    while (nextLine(fd, linea) && strcmp(linea, "PTERM\n")) {
        sscanf(linea, "%s", tok1);
        g->addInitSym(tok1);
    }

    // Read terminal productions
    while (nextLine(fd, linea) && strcmp(linea, "PBIN\n")) {
        float pr;

        sscanf(linea, "%f %s %s %s", &pr, tok1, tok2, aux);

        g->addTerminal(pr, tok1, tok2, aux);
    }

    using F_t = void (Grammar::*)(float, const std::string&, const std::string&, const std::string&, const std::string&, const std::string&);
    std::pair<std::string_view, F_t> handlers[] = {
        { "H", &Grammar::addRuleH },
        { "V", &Grammar::addRuleV },
        { "Ve", &Grammar::addRuleVe },
        { "Sup", &Grammar::addRuleSup },
        { "Sub", &Grammar::addRuleSub },
        { "SSE", &Grammar::addRuleSSE },
        { "Ins", &Grammar::addRuleIns },
        { "Mrt", &Grammar::addRuleMrt },
    };
    // Read binary productions
    while (nextLine(fd, linea)) {
        std::vector<std::string> tokens = split(linea);

        if (tokens.size() != 7) {
            fprintf(stderr, "Error: Grammar not valid (PBIN)\n");
            exit(-1);
        }

        const auto& k = tokens[1];
        bool had = false;
        for (const auto& [s_v, hand] : handlers) {
            if (k == s_v) {
                (g->*hand)(std::stof(tokens[0]), tokens[2].c_str(), tokens[3].c_str(), tokens[4].c_str(), tokens[5].c_str(), tokens[6].c_str());
                had = true;
                break;
            }
        }

        if (!had) {
            fprintf(stderr, "Error: Binary rule type '%s' nor valid\n", tokens[1].c_str());
            exit(-1);
        }
    }
}
