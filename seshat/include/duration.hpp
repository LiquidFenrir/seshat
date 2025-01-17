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
#ifndef _DURATION_MODEL_
#define _DURATION_MODEL_

#include "path.hpp"
#include "symrec.hpp"
#include <cstdio>
#include <memory>
#include <rnnlib4seshat/MultiArray.hpp>
#include <vector>

namespace seshat {

class DurationModel {
    int max_strokes;
    int Nsyms;
    MultiArray<float> duration_prob;

    void loadModel(std::istream& is, SymRec* sr);

public:
    DurationModel(const fs::path& path, int mxs, SymRec* sr);

    float prob(int symclas, int size);
};

}

#endif
