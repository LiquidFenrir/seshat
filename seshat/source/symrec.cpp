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
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <rnnlib4seshat/MultilayerNet.hpp>
#include <rnnlib4seshat/Rprop.hpp>
#include <rnnlib4seshat/SteepestDescent.hpp>
#include <samples.hpp>
#include <symrec.hpp>
#include <vectorimage.hpp>

namespace fs = std::filesystem;
using namespace seshat;

#define TSIZE 2048

SymRec::SymRec(const fs::path& config)
{
    // RNN classifier configuration
    std::string RNNon, RNNoff, RNNmavON, RNNmavOFF, path;
    {
        std::ifstream fd(config);
        if (!fd) {
            std::cerr << "Error: loading config file '" << config << "'\n";
            throw std::runtime_error("Error: loading config file");
        }

        RNNalpha = -1.0;

        std::string id, info;
        while (fd) {
            fd >> id >> std::ws;
            removeEndings(id);

            std::pair<std::string_view, std::string&> which[] = {
                { "RNNon", RNNon },
                { "RNNoff", RNNoff },
                { "RNNmavON", RNNmavON },
                { "RNNmavOFF", RNNmavOFF },
                { "SymbolTypes", path },
            };

            if (id == "RNNalpha") {
                fd >> RNNalpha >> std::ws;
            } else {
                for (auto& [key, into] : which) {
                    if (id == key) {
                        fd >> into >> std::ws;
                        removeEndings(into);
                        break;
                    }
                }
            }
        }

        if (RNNalpha <= 0.0 || RNNalpha >= 1.0) {
            std::cerr << "Error: loading config file '" << config << "': must be 0 < RNNalpha < 1\n";
            throw std::runtime_error("Error: loading RNNalpha in config file");
        }
        if (RNNon.empty()) {
            std::cerr << "Error: loading RNNon in config file\n";
            throw std::runtime_error("Error: loading RNNon in config file");
        }
        if (RNNoff.empty()) {
            std::cerr << "Error: loading RNNoff in config file\n";
            throw std::runtime_error("Error: loading RNNoff in config file");
        }
        if (RNNmavON.empty()) {
            std::cerr << "Error: loading RNNmavON in config file\n";
            throw std::runtime_error("Error: loading RNNmavON in config file");
        }
        if (RNNmavOFF.empty()) {
            std::cerr << "Error: loading RNNmavOFF in config file\n";
            throw std::runtime_error("Error: loading RNNmavOFF in config file");
        }

        // Close config file
    }

    // Load symbol types info
    std::ifstream tp(config.parent_path() / path);
    if (!tp) {
        std::cerr << "Error: loading SymbolTypes file '" << path << "'\n";
        throw std::runtime_error("Error: loading SymbolTypes file");
    }

    tp >> C;

    key2cl.resize(C);
    type.resize(C);

    // Load classes and symbol types
    char T = '\0';
    std::string clase;
    for (int idclase = 0; idclase < C && tp; ++idclase) {
        tp >> clase >> std::ws >> T >> std::ws;
        key2cl[idclase] = clase;
        cl2key[clase] = idclase;

        switch (T) {
        case 'n':
            type[idclase] = SymbolType::Normal;
            break;
        case 'a':
            type[idclase] = SymbolType::Ascend;
            break;
        case 'd':
            type[idclase] = SymbolType::Descend;
            break;
        case 'm':
            type[idclase] = SymbolType::Middle;
            break;
        default:
            std::cerr << "SymRec: Error reading symbol types: " << T << "\n";
            throw std::runtime_error("vSymRec: Error reading symbol types");
        }
    }

    // Features extraction
    FEAS.emplace(config.parent_path() / RNNmavON, config.parent_path() / RNNmavOFF);

    // Create and load BLSTM models

    // Online info
    ConfigFile conf_on((config.parent_path() / RNNon).string());
    header_on.targetLabels = conf_on.get_list<std::string>("targetLabels");
    header_on.inputSize = conf_on.get<int>("inputSize");
    header_on.outputSize = header_on.targetLabels.size();
    header_on.numDims = 1;

    // Create WeightContainer online
    wc_on = std::make_unique<WeightContainer>(&deh_on);

    // Load online BLSTM
    blstm_on = std::make_unique<MultilayerNet>(std::cout, conf_on, header_on, wc_on.get(), &deh_on);

    // build weight container after net is created
    wc_on->build();

    // build the network after the weight container
    blstm_on->build();

    if (conf_on.get<bool>("loadWeights", false))
        deh_on.load(conf_on, std::cout);

    // Offline info
    ConfigFile conf_off((config.parent_path() / RNNoff).string());

    // Check if the targetLabels are the same for both online and offline RNN-BLSTM
    std::vector<std::string> aux = conf_off.get_list<std::string>("targetLabels");
    if (aux.size() != header_on.targetLabels.size()) {
        std::cerr << "Error: Target labels of online and offline symbol classifiers do not match\n";
        throw std::runtime_error("Error: Target labels of online and offline symbol classifiers do not match");
    }

    if (!std::equal(aux.begin(), aux.end(), header_on.targetLabels.begin(), [](const auto& a, const auto& b) {
            return a.compare(b) == 0;
        })) {
        std::cerr << "Error: Target labels of online and offline symbol classifiers do not match\n";
        throw std::runtime_error("Error: Target labels of online and offline symbol classifiers do not match");
    }

    header_off.targetLabels = conf_off.get_list<std::string>("targetLabels");
    header_off.inputSize = conf_off.get<int>("inputSize");
    header_off.outputSize = header_off.targetLabels.size();
    header_off.numDims = 1;

    // Create WeightContainer offline
    wc_off = std::make_unique<WeightContainer>(&deh_off);

    // Load offline BLSTM
    blstm_off = std::make_unique<MultilayerNet>(std::cout, conf_off, header_off, wc_off.get(), &deh_off);

    // build weight container after net is created
    wc_off->build();

    // build the network after the weight container
    blstm_off->build();

    if (conf_off.get<bool>("loadWeights", false))
        deh_off.load(conf_off, std::cout);
}

SymRec::~SymRec()
{
}

char* SymRec::strClase(int c)
{
    return key2cl[c].data();
}

int SymRec::keyClase(const std::string& str)
{
    const auto it = cl2key.find(str);
    if (it == cl2key.end()) {
        // std::cerr << "WARNING: Class '" << str << "' doesn't appear in symbols database\n";
        return -1;
    }
    return it->second;
}

bool SymRec::checkClase(const std::string& str)
{
    if (cl2key.find(str) == cl2key.end())
        return false;
    return true;
}

int SymRec::getNClases()
{
    return C;
}

// Returns the type of symbol of class k
SymbolType SymRec::symType(int k)
{
    return type[k];
}

/************
 * Classify *
 ************/

int SymRec::clasificar(Samples& M, int ncomp, const int NB, int* vclase, float* vpr, int& as, int& ds)
{
    const int aux[1] = { ncomp };
    return clasificar(M, aux, NB, vclase, vpr, as, ds);
}

int SymRec::clasificar(Samples& M, std::span<const int> LT, const int NB, int* vclase, float* vpr, int& as, int& ds)
{
    SegmentHyp aux;

    aux.rx = aux.ry = INT_MAX;
    aux.rs = aux.rt = INT_MIN;

    aux.stks.reserve(LT.size());

    for (const auto it : LT) {
        aux.stks.push_back(it);

        const auto& stk = M.getStroke(it);
        if (stk.rx < aux.rx)
            aux.rx = stk.rx;
        if (stk.ry < aux.ry)
            aux.ry = stk.ry;
        if (stk.rs > aux.rs)
            aux.rs = stk.rs;
        if (stk.rt > aux.rt)
            aux.rt = stk.rt;
    }

    return classify(M, aux, NB, vclase, vpr, as, ds);
}

int SymRec::classify(Samples& M, SegmentHyp& SegHyp, const int NB, int* vclase, float* vpr, int& as, int& ds)
{

    int regy = INT_MAX, regt = INT_MIN, N = 0;

    // First compute the vertical centroid (cen) and the ascendant/descendant centroids (as/ds)
    SegHyp.cen = 0;

    for (const auto it_idx : SegHyp.stks) {
        const auto& cur_stroke = M.getStroke(it_idx);
        regy = std::min(regy, cur_stroke.ry);
        regt = std::max(regt, cur_stroke.rt);

        const int nPoints = cur_stroke.getNPoints();
        for (int j = 0; j < nPoints; ++j) {
            SegHyp.cen += cur_stroke.get(j)->y;
        }
        N += nPoints;
    }

    SegHyp.cen /= N;
    as = (SegHyp.cen + regt) / 2;
    ds = (regy + SegHyp.cen) / 2;

    // Feature extraction of hypothesis
    std::unique_ptr<DataSequence> feat_on, feat_off;

    // Online features extraction: PRHLT (7 features)
    feat_on = FEAS->getOnline(M, SegHyp);

    // Render the image representing the set of strokes SegHyp.stks
    {
        VectorImage img;
        M.renderStrokesPBM(SegHyp.stks, img);

        // Offline features extraction: FKI (9 features)
        feat_off = FEAS->getOfflineFKI(img, img.height, img.width);
    }

    // std::cout << feat_off->inputs;

    // n-best classification
    std::vector<std::pair<float, int>> clason(NB), clasoff(NB), clashyb(2 * NB);

    for (int i = 0; i < NB; i++) {
        clason[i].first = 0.0; // probability
        clason[i].second = -1; // class id
        clasoff[i].first = 0.0;
        clasoff[i].second = -1;
        clashyb[i].first = 0.0;
        clashyb[i].second = -1;
    }

    // Online/offline classification
    BLSTMclassification(blstm_on.get(), *feat_on, clason);
    BLSTMclassification(blstm_off.get(), *feat_off, clasoff);

    // Online + Offline n-best linear combination
    // alpha * pr(on) + (1 - alpha) * pr(off)

    const float reverse_rnnalpha = 1.0 - RNNalpha;
    for (int i = 0; i < NB; i++) {
        clason[i].first *= RNNalpha; // online  *    alpha
        clasoff[i].first *= reverse_rnnalpha; // offline * (1-alpha)
    }

    int hybnext = 0;
    for (int i = 0; i < NB; i++) {
        if (clason[i].second >= 0) {
            clashyb[hybnext].first = clason[i].first;
            clashyb[hybnext].second = clason[i].second;

            for (int j = 0; j < NB; j++)
                if (clason[i].second == clasoff[j].second) {
                    clashyb[hybnext].first += clasoff[j].first;
                    break;
                }

            hybnext++;
        }

        if (clasoff[i].second < 0)
            continue;

        bool found = false;
        for (int j = 0; j < NB && !found; j++)
            if (clasoff[i].second == clason[j].second)
                found = true;

        // Add the (1-alpha) probability if the class is in OFF but not in ON
        if (!found) {
            clashyb[hybnext].first = clasoff[i].first;
            clashyb[hybnext].second = clasoff[i].second;
            hybnext++;
        }
    }

    std::sort(&clashyb[0], &clashyb[hybnext], std::greater<std::pair<float, int>>());
    for (int i = 0, e = std::min<int>(hybnext, NB); i < e; i++) {
        vpr[i] = clashyb[i].first;
        vclase[i] = clashyb[i].second;
    }

    return SegHyp.cen;
}

void SymRec::BLSTMclassification(Mdrnn* net, const DataSequence& seq, std::span<std::pair<float, int>> claspr)
{
    // Classify sample with net
    net->train(seq);

    // Get output layer and its shape
    const Layer* L = net->outputLayers.front();
    int NVEC = L->outputActivations.shape[0];
    const int NCLA = L->outputActivations.shape[1];

    auto prob_class = std::make_unique<std::pair<float, int>[]>(NCLA);
    for (int i = 0; i < NCLA; i++) {
        auto& p_c = prob_class[i];
        p_c.first = 0.0;
        p_c.second = cl2key[header_on.targetLabels[i]]; // targetLabels on = targetLabels off
    }

    // Compute the average posterior probability per class
    for (int nvec = 0; nvec < NVEC; nvec++)
        for (int ncla = 0; ncla < NCLA; ncla++)
            prob_class[ncla].first += L->outputActivations.data[nvec * NCLA + ncla];

    for (int ncla = 0; ncla < NCLA; ncla++)
        prob_class[ncla].first /= NVEC;

    // Sort classification result by its probability
    std::span<std::pair<float, int>> prob_class_span(prob_class.get(), NCLA);
    std::sort(prob_class_span.begin(), prob_class_span.end(), std::greater<std::pair<float, int>>());

    // Copy n-best to output vector
    std::copy_n(prob_class_span.begin(), claspr.size(), claspr.begin());
}
