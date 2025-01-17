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
#ifndef __GMM__
#define __GMM__

#include "path.hpp"
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <rnnlib4seshat/MultiArray.hpp>
#include <span>

namespace seshat {

class GMM {
    int C, D, G;
    MultiArray<float> invcov, mean, weight, det;
    std::vector<float> prior;

    void loadModel(std::istream& is);
    float pdf(const int c, std::span<const float> v);

public:
    GMM(const fs::path& model);
    GMM(std::istream& is);

    void posterior(std::span<const float> x, std::span<float> pr);
};

}

#endif
