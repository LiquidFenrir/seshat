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
#ifndef _SYMFEATURES_
#define _SYMFEATURES_

#include "path.hpp"
#include <memory>
#include <rnnlib4seshat/DataSequence.hpp>
#include <string>
#include <vectorimage.hpp>

namespace seshat {

class SegmentHyp;
class Samples;

enum class SymbolType {
    Normal,
    Ascend,
    Descend,
    Middle,
};

class SymFeatures {
    static inline constexpr int ON_FEAT = 7;
    static inline constexpr int OFF_FEAT = 9;
    double means_on[ON_FEAT], means_off[OFF_FEAT];
    double stds_on[ON_FEAT], stds_off[OFF_FEAT];

public:
    SymFeatures(const fs::path& mav_on, const fs::path& mav_off);

    std::unique_ptr<DataSequence> getOnline(Samples& M, SegmentHyp& SegHyp);
    std::unique_ptr<DataSequence> getOfflineFKI(VectorImage& img, int H, int W);
};

}

#endif
