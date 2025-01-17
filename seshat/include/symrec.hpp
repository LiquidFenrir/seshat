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
#ifndef _SYMREC_
#define _SYMREC_

#include "path.hpp"
#include "symfeatures.hpp"
#include <cstdio>
#include <cstring>
#include <map>
#include <rnnlib4seshat/DataExporter.hpp>
#include <rnnlib4seshat/DataSequence.hpp>
#include <rnnlib4seshat/Mdrnn.hpp>
#include <rnnlib4seshat/NetcdfDataset.hpp>
#include <rnnlib4seshat/WeightContainer.hpp>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace seshat {

struct SegmentHyp;
class Samples;

class SymRec {
    std::optional<SymFeatures> FEAS;
    DataHeader header_on, header_off;
    DataExportHandler deh_on, deh_off;
    std::unique_ptr<WeightContainer> wc_on, wc_off;
    std::unique_ptr<Mdrnn> blstm_on, blstm_off;
    float RNNalpha;

    // Symbol classes and types information
    std::vector<SymbolType> type;
    std::map<std::string, int> cl2key;
    std::vector<std::string> key2cl;

    int C; // Number of classes

    int classify(Samples& M, SegmentHyp& SegHyp, const int NB, int* vclase, float* vpr, int& as, int& ds);
    void BLSTMclassification(Mdrnn* net, const DataSequence& seq, std::span<std::pair<float, int>>);

public:
    SymRec(const fs::path& path);
    ~SymRec();

    char* strClase(int c);
    int keyClase(const std::string& str);
    bool checkClase(const std::string& str);
    int getNClases();
    SymbolType symType(int k);

    int clasificar(Samples& M, int ncomp, const int NB, int* vclase, float* vpr, int& as, int& ds);
    int clasificar(Samples& M, std::span<const int> LT, const int NB, int* vclase, float* vpr, int& as, int& ds);
};

}

#endif
