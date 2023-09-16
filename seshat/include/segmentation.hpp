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
#ifndef _SEGMENTATION_MODEL_
#define _SEGMENTATION_MODEL_

#include "gmm.hpp"
#include <cstdio>
#include <cstdlib>
#include <optional>

namespace seshat {

class CellCYK;
class Samples;

class SegmentationModelGMM {
    std::vector<int> strokes_list;
    std::optional<GMM> model;

public:
    SegmentationModelGMM(const char* mod);

    float prob(CellCYK* cd, Samples* m);
};

}

#endif
