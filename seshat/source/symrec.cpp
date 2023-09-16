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
#include <map>
#include <rnnlib4seshat/MultilayerNet.hpp>
#include <rnnlib4seshat/Rprop.hpp>
#include <rnnlib4seshat/SteepestDescent.hpp>
#include <samples.hpp>
#include <symrec.hpp>
#include <vectorimage.hpp>

using namespace seshat;

#define TSIZE 2048

SymRec::SymRec(const char* config)
{
    FILE* fd = fopen(config, "r");
    if (!fd) {
        fprintf(stderr, "Error: loading config file '%s'\n", config);
        exit(-1);
    }

    // RNN classifier configuration
    static char RNNon[TSIZE], RNNoff[TSIZE];
    static char RNNmavON[TSIZE], RNNmavOFF[TSIZE];

    static char id[TSIZE], info[TSIZE], path[TSIZE];

    RNNon[0] = RNNoff[0] = RNNmavON[0] = RNNmavOFF[0] = 0;
    path[0] = 0;
    RNNalpha = -1.0;

    while (!feof(fd)) {
        fscanf(fd, "%s", id); // Field id
        fscanf(fd, "%s", info); // Info

        // Remove the last \n character
        if (info[strlen(info) - 1] == '\n')
            info[strlen(info) - 1] = '\0';

        if (!strcmp(id, "RNNon"))
            strcpy(RNNon, info);
        else if (!strcmp(id, "RNNoff"))
            strcpy(RNNoff, info);
        else if (!strcmp(id, "RNNmavON"))
            strcpy(RNNmavON, info);
        else if (!strcmp(id, "RNNmavOFF"))
            strcpy(RNNmavOFF, info);
        else if (!strcmp(id, "RNNalpha"))
            RNNalpha = atof(info);
        else if (!strcmp(id, "SymbolTypes"))
            strcpy(path, info);
    }

    if (RNNalpha <= 0.0 || RNNalpha >= 1.0) {
        fprintf(stderr, "Error: loading config file '%s': must be 0 < RNNalpha < 1\n", config);
        exit(-1);
    }
    if (RNNon[0] == 0) {
        fprintf(stderr, "Error: loading RNNon in config file\n");
        exit(-1);
    }
    if (RNNoff[0] == 0) {
        fprintf(stderr, "Error: loading RNNoff in config file\n");
        exit(-1);
    }
    if (RNNmavON[0] == 0) {
        fprintf(stderr, "Error: loading RNNmavON in config file\n");
        exit(-1);
    }
    if (RNNmavOFF[0] == 0) {
        fprintf(stderr, "Error: loading RNNmavOFF in config file\n");
        exit(-1);
    }

    // Close config file
    fclose(fd);

    // Load symbol types info
    FILE* tp = fopen(path, "r");
    if (!tp) {
        fprintf(stderr, "Error: loading SymbolTypes file '%s'\n", path);
        exit(-1);
    }

    // Number of classes
    fscanf(tp, "%d", &C);
    getc(tp);

    key2cl.resize(C);
    type.resize(C);
    char clase[256], T = 0, linea[256]; // aux[256];

    // Load classes and symbol types
    int idclase = 0;
    while (fgets(linea, 256, tp) != NULL) {
        for (int i = 0; linea[i] && linea[i] != '\n'; i++) {
            clase[i] = linea[i];
            if (linea[i] == ' ') {
                clase[i] = 0;
                T = linea[i + 1];
                break;
            }
        }

        key2cl[idclase] = clase;
        cl2key[clase] = idclase;
        idclase++;

        if (T == 'n')
            type[cl2key[clase]] = 0; // Centroid
        else if (T == 'a')
            type[cl2key[clase]] = 1; // Ascender
        else if (T == 'd')
            type[cl2key[clase]] = 2; // Descender
        else if (T == 'm')
            type[cl2key[clase]] = 3; // Middle
        else {
            fprintf(stderr, "SymRec: Error reading symbol types\n");
            exit(-1);
        }
    }

    // Features extraction
    FEAS.emplace(RNNmavON, RNNmavOFF);

    // Create and load BLSTM models

    // Online info
    ConfigFile conf_on(RNNon);
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
    ConfigFile conf_off(RNNoff);

    // Check if the targetLabels are the same for both online and offline RNN-BLSTM
    std::vector<std::string> aux = conf_off.get_list<std::string>("targetLabels");
    if (aux.size() != header_on.targetLabels.size()) {
        fprintf(stderr, "Error: Target labels of online and offline symbol classifiers do not match\n");
        exit(-1);
    }

    if (!std::equal(aux.begin(), aux.end(), header_on.targetLabels.begin(), [](const auto& a, const auto& b) {
            return a.compare(b) == 0;
        })) {
        fprintf(stderr, "Error: Target labels of online and offline symbol classifiers do not match\n");
        exit(-1);
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
        // fprintf(stderr, "WARNING: Class '%s' doesn't appear in symbols database\n", str.c_str());
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
int SymRec::symType(int k)
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
    sort(prob_class_span.begin(), prob_class_span.end(), std::greater<std::pair<float, int>>());

    // Copy n-best to output vector
    std::copy_n(prob_class_span.begin(), claspr.size(), claspr.begin());
}
