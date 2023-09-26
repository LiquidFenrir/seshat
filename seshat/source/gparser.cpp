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
#include <iostream>

using namespace seshat;

#define SIZE 512

gParser::gParser(Grammar& gram, std::istream& is)
    : g(gram)
{
    parse(is);
}

void gParser::parse(std::istream& is)
{
    std::string line;

    // Read nonterminal symbols
    while (std::getline(is, line) && (removeEndings(line), true) && line != "START") {
        if (line.empty() || line.front() == '#')
            continue;

        g.addNoTerminal(line);
    }

    // Read start symbol(s) of the grammar
    while (std::getline(is, line) && (removeEndings(line), true) && line != "PTERM") {
        if (line.empty() || line.front() == '#')
            continue;

        g.addInitSym(line);
    }

    // Read terminal productions
    float pr = 0;
    std::string toks[7];
    while (std::getline(is, line) && (removeEndings(line), true) && line != "PBIN") {
        if (line.empty() || line.front() == '#')
            continue;

        std::istringstream liness(line);
        liness >> pr >> toks[0] >> toks[1] >> toks[2];
        g.addTerminal(pr, toks[0], toks[1], toks[2]);
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
    while (std::getline(is, line) && (removeEndings(line), true)) {
        if (line.empty() || line.front() == '#')
            continue;

        std::istringstream liness(line);
        for (auto& tok : toks) {
            if (!(liness >> std::ws >> std::quoted(tok))) {
                std::cerr << "Error: Grammar not valid (PBIN)\n";
                throw std::runtime_error("Error: Grammar not valid (PBIN)");
            }
        }

        const auto& k = toks[1];
        bool had = false;
        for (const auto& [s_v, hand] : handlers) {
            if (k == s_v) {
                (g.*hand)(std::stof(toks[0]), toks[2], toks[3], toks[4], toks[5], toks[6]);
                had = true;
                break;
            }
        }

        if (!had) {
            std::cerr << "Error: Binary rule type '" << toks[1] << "' not valid\n";
            throw std::runtime_error("Error: Binary rule type not valid");
        }
    }
}
