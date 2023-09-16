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

#include <cellcyk.hpp>
#include <gmm.hpp>
#include <grammar.hpp>
#include <internal_hypothesis.hpp>
#include <logspace.hpp>
#include <meparser.hpp>

using namespace seshat;

// Symbol classifier N-Best
#define NB 10

meParser::meParser(const char* conf)
{
    FILE* fconfig = fopen(conf, "r");
    if (!fconfig) {
        fprintf(stderr, "Error: loading config file '%s'\n", conf);
        exit(-1);
    }

    // Read configuration file
    static char auxstr[1024], path[1024];

    clusterF = -1;
    segmentsTH = -1;
    max_strokes = -1;
    ptfactor = -1;
    pbfactor = -1;
    qfactor = -1;
    dfactor = -1;
    gfactor = -1;
    rfactor = -1;
    path[0] = 0;

    fscanf(fconfig, "%s", auxstr);
    while (!feof(fconfig)) {
        if (!strcmp(auxstr, "GRAMMAR"))
            fscanf(fconfig, "%s", path); // Grammar path
        else if (!strcmp(auxstr, "MaxStrokes")) {
            fscanf(fconfig, "%s", auxstr); // Info
            max_strokes = atoi(auxstr);
        } else if (!strcmp(auxstr, "SpatialRels")) {
            fscanf(fconfig, "%s", auxstr);
            gmm_spr = std::make_unique<GMM>(auxstr);
        } else if (!strcmp(auxstr, "InsPenalty")) {
            fscanf(fconfig, "%s", auxstr);
            InsPen = atof(auxstr);
        } else if (!strcmp(auxstr, "ClusterF")) {
            fscanf(fconfig, "%s", auxstr);
            clusterF = atof(auxstr);
        } else if (!strcmp(auxstr, "SegmentsTH")) {
            fscanf(fconfig, "%s", auxstr);
            segmentsTH = atof(auxstr);
        } else if (!strcmp(auxstr, "ProductionTSF")) {
            fscanf(fconfig, "%s", auxstr);
            ptfactor = atof(auxstr);
        } else if (!strcmp(auxstr, "ProductionBSF")) {
            fscanf(fconfig, "%s", auxstr);
            pbfactor = atof(auxstr);
        } else if (!strcmp(auxstr, "RelationSF")) {
            fscanf(fconfig, "%s", auxstr);
            rfactor = atof(auxstr);
        } else if (!strcmp(auxstr, "SymbolSF")) {
            fscanf(fconfig, "%s", auxstr);
            qfactor = atof(auxstr);
        } else if (!strcmp(auxstr, "DurationSF")) {
            fscanf(fconfig, "%s", auxstr);
            dfactor = atof(auxstr);
        } else if (!strcmp(auxstr, "SegmentationSF")) {
            fscanf(fconfig, "%s", auxstr);
            gfactor = atof(auxstr);
        } else
            fscanf(fconfig, "%s", auxstr); // Info

        fscanf(fconfig, "%s", auxstr); // Next field id
    }

    if (path[0] == 0) {
        fprintf(stderr, "Error: GRAMMAR field not found in config file '%s'\n", conf);
        exit(-1);
    }

    if (!gmm_spr) {
        fprintf(stderr, "Error: Loading GMM model in config file '%s'\n", conf);
        exit(-1);
    }

    if (max_strokes <= 0 || max_strokes > 10) {
        fprintf(stderr, "Error: Wrong MaxStrokes value in config file '%s'\n", conf);
        exit(-1);
    }

    if (clusterF < 0) {
        fprintf(stderr, "Error: Wrong ClusterF value in config file '%s'\n", conf);
        exit(-1);
    }

    if (segmentsTH <= 0) {
        fprintf(stderr, "Error: Wrong SegmentsTH value in config file '%s'\n", conf);
        exit(-1);
    }

    if (InsPen <= 0) {
        fprintf(stderr, "Error: Wrong InsPenalty value in config file '%s'\n", conf);
        exit(-1);
    }

    if (qfactor <= 0)
        fprintf(stderr, "WARNING: SymbolSF = %f\n", qfactor);
    if (ptfactor <= 0)
        fprintf(stderr, "WARNING: ProductionTSF = %f\n", ptfactor);
    if (pbfactor <= 0)
        fprintf(stderr, "WARNING: ProductionBSF = %f\n", pbfactor);
    if (rfactor <= 0)
        fprintf(stderr, "WARNING: RelationSF = %f\n", rfactor);
    if (dfactor < 0)
        fprintf(stderr, "WARNING: DurationSF = %f\n", dfactor);
    if (gfactor < 0)
        fprintf(stderr, "WARNING: SegmentationSF = %f\n", gfactor);

    // Read grammar path
    fscanf(fconfig, "%s", path);

    // Remove the last \n character
    if (path[strlen(path) - 1] == '\n')
        path[strlen(path) - 1] = '\0';

    fclose(fconfig);

    // Load symbol recognizer
    loadSymRec(conf);

    // Load grammar
    G = std::make_unique<Grammar>(path, sym_rec.get());
}

void meParser::loadSymRec(const char* config)
{
    FILE* fd = fopen(config, "r");
    if (!fd) {
        fprintf(stderr, "Error: loading config file '%s'\n", config);
        exit(-1);
    }

    // Read symbol recognition information from config file
    static char auxstr[1024], dur_path[1024], seg_path[1024];
    dur_path[0] = seg_path[0] = 0;

    fscanf(fd, "%s", auxstr);
    while (!feof(fd)) {
        if (!strcmp(auxstr, "Duration"))
            fscanf(fd, "%s", dur_path);
        else if (!strcmp(auxstr, "Segmentation"))
            fscanf(fd, "%s", seg_path);
        else
            fscanf(fd, "%s", auxstr); // Info

        fscanf(fd, "%s", auxstr); // Next field id
    }

    if (dur_path[0] == 0) {
        fprintf(stderr, "Error: Duration field not found in config file '%s'\n", config);
        exit(-1);
    }
    if (seg_path[0] == 0) {
        fprintf(stderr, "Error: Segmentation field not found in config file '%s'\n", config);
        exit(-1);
    }

    // Close configure
    fclose(fd);

    // Load symbol recognizer
    sym_rec = std::make_unique<SymRec>(config);

    // Load duration and segmentation model
    duration.emplace(dur_path, max_strokes, sym_rec.get());
    segmentation.emplace(seg_path);
}

// CYK table initialization with the terminal symbols
void meParser::initCYKterms(Samples& M, TableCYK& tcyk, int N, int K)
{
    int clase[NB];
    float pr[NB];

    for (int i = 0; i < M.nStrokes(); i++) {
        int cmy, asc, des;
        cmy = sym_rec->clasificar(M, i, NB, clase, pr, asc, des);

        auto cd = std::make_unique<CellCYK>(G->noTerminales.size(), N);

        M.setRegion(*cd, i);

        bool insertar = false;
        for (const auto& prod : G->prodTerms) {
            for (int k = 0; k < NB; k++) {
                const auto clase_k = clase[k];
                const auto gotClase = prod->getClase(clase_k);
                const auto gotPrior = prod->getPrior(clase_k);
                const auto gotNoTerm = prod->getNoTerm();
                if (!(pr[k] > 0.0 && gotClase && gotPrior > -FLT_MAX))
                    continue;

                const float prob = log(InsPen) + ptfactor * gotPrior + qfactor * log(pr[k]) + dfactor * log(duration->prob(clase_k, 1));

                if (cd->noterm[gotNoTerm]) {
                    if (cd->noterm[gotNoTerm]->pr > prob + gotPrior)
                        continue;
                }

                insertar = true;

                // Create new symbol
                cd->noterm[gotNoTerm] = std::make_unique<InternalHypothesis>(clase_k, prob, cd.get(), gotNoTerm);
                cd->noterm[gotNoTerm]->pt = prod.get();

                // Compute the vertical centroid according to the type of symbol
                int cen, type = sym_rec->symType(clase_k);
                if (type == 0)
                    cen = cmy; // Normal
                else if (type == 1)
                    cen = asc; // Ascendant
                else if (type == 2)
                    cen = des; // Descendant
                else
                    cen = (cd->t + cd->y) * 0.5; // Middle point

                // Vertical center
                cd->noterm[gotNoTerm]->lcen = cen;
                cd->noterm[gotNoTerm]->rcen = cen;
            }
        }

        if (insertar) {
            // Add to parsing table (size=1)
            tcyk.add(1, cd.release(), -1, G->esInit.get());
        }
    }
}

void meParser::combineStrokes(Samples& M, TableCYK& tcyk, int N)
{
    if (N <= 1)
        return;

    int asc, cmy, des;
    int clase[NB];
    float pr[NB];
    int ntested = 0;

    // Set distance threshold
    float distance_th = segmentsTH;
    close_list.clear();
    stks_list.clear();
    stkvec.clear();

    // For every single stroke
    for (int stkc1 = 1; stkc1 < N; stkc1++) {

        CellCYK* c1 = new CellCYK(G->noTerminales.size(), N);
        M.setRegion(*c1, stkc1);

        for (int size = 2; size <= std::min(max_strokes, N); size++) {
            close_list.clear();

            // Add close and visible strokes to the closer list
            if (size == 2) {
                for (int i = 0; i < stkc1; i++) {
                    if (M.getDist(stkc1, i) < distance_th) {
                        close_list.push_back(i);
                    }
                }
            } else
                M.get_close_strokes(stkc1, close_list, distance_th);

            // If there are not enough strokes to compose a hypothesis of "size", continue
            if ((int)close_list.size() < size - 1)
                continue;

            stkvec.clear();
            stkvec.insert(stkvec.end(), close_list.begin(), close_list.end());
            const int VS = stkvec.size();

            std::sort(stkvec.begin(), stkvec.end());

            for (int i = size - 2; i < VS; i++) {
                stks_list.clear();
                // Add stkc1 and current stroke (ith)
                stks_list.push_back(stkvec[i]);
                stks_list.push_back(stkc1);

                // Add strokes up to size
                stks_list.insert(stks_list.end(), stkvec.begin() + (i - (size - 2)), stkvec.begin() + i);

                // Sort list (stroke's order is important in online classification)
                std::sort(stks_list.begin(), stks_list.end());

                CellCYK* cd = new CellCYK(G->noTerminales.size(), N);
                M.setRegion(*cd, stks_list);

                float seg_prob = segmentation->prob(cd, &M);

                cmy = sym_rec->clasificar(M, stks_list, NB, clase, pr, asc, des);

                ntested++;

                // Add to parsing table
                bool insertar = false;
                for (const auto& prod : G->prodTerms) {
                    for (int k = 0; k < NB; k++)
                        if (pr[k] > 0.0 && prod->getClase(clase[k]) && prod->getPrior(clase[k]) > -FLT_MAX) {

                            float prob = log(InsPen) + ptfactor * prod->getPrior(clase[k]) + qfactor * log(pr[k]) + dfactor * log(duration->prob(clase[k], size)) + gfactor * log(seg_prob);

                            if (cd->noterm[prod->getNoTerm()]) {
                                if (cd->noterm[prod->getNoTerm()]->pr > prob)
                                    continue;

                                cd->noterm[prod->getNoTerm()].reset();
                            }

                            insertar = true;

                            cd->noterm[prod->getNoTerm()] = std::make_unique<InternalHypothesis>(clase[k], prob, cd, prod->getNoTerm());
                            cd->noterm[prod->getNoTerm()]->pt = prod.get();

                            int cen, type = sym_rec->symType(clase[k]);
                            if (type == 0)
                                cen = cmy; // Normal
                            else if (type == 1)
                                cen = asc; // Ascendant
                            else if (type == 2)
                                cen = des; // Descendant
                            else
                                cen = (cd->t + cd->y) * 0.5; // Middle point

                            // Vertical center
                            cd->noterm[prod->getNoTerm()]->lcen = cen;
                            cd->noterm[prod->getNoTerm()]->rcen = cen;
                        }
                }

                if (insertar) {
                    tcyk.add(size, cd, -1, G->esInit.get());
                } else
                    delete cd;
            } // end for close_list (VS)
        } // end for size

    } // end for stroke stkc1
}

// Combine hypotheses A and B to create new hypothesis S using production 'S -> A B'
CellCYK* meParser::fusion(Samples& M, ProductionB* pd, InternalHypothesis* A, InternalHypothesis* B, int N, double prob)
{
    CellCYK* S = nullptr;

    if (!A->parent->compatible(B->parent) || pd->prior == -FLT_MAX)
        return S;

    // Penalty according to distance between strokes
    float grpen;

    if (clusterF > 0.0) {

        grpen = M.group_penalty(A->parent, B->parent);
        // If distance is infinity -> not visible
        if (grpen >= M.INF_DIST)
            return nullptr;

        // Compute penalty
        grpen = 1.0 / (1.0 + grpen);
        grpen = pow(grpen, clusterF);
    } else
        grpen = 1.0;

    // Get nonterminal
    int ps = pd->S;

    // Create new cell
    S = new CellCYK(G->noTerminales.size(), N);

    // Compute the (log)probability
    prob = pbfactor * pd->prior + rfactor * log(prob * grpen) + A->pr + B->pr;

    // Copute resulting region
    S->x = std::min(A->parent->x, B->parent->x);
    S->y = std::min(A->parent->y, B->parent->y);
    S->s = std::max(A->parent->s, B->parent->s);
    S->t = std::max(A->parent->t, B->parent->t);

    // Set the strokes covered
    S->ccUnion(A->parent, B->parent);

    int clase = -1;
    if (!pd->check_out() /* && sym_rec->checkClase(pd->get_outstr()) */)
        clase = sym_rec->keyClase(pd->get_outstr()); // will return -1 on non found anyway

    // Create hypothesis
    S->noterm[ps] = std::make_unique<InternalHypothesis>(clase, prob, S, ps);

    pd->mergeRegions(A, B, S->noterm[ps].get());

    // Save the tree path
    S->noterm[ps]->hi = A;
    S->noterm[ps]->hd = B;
    S->noterm[ps]->prod = pd;

    // Special treatment for binary productions that compose terminal symbols (e.g. Equal --V--> Hline Hline)
    if (clase >= 0) {
        for (const auto& prod : G->prodTerms) {
            if (prod->getClase(clase) && prod->getPrior(clase) > -FLT_MAX) {
                S->noterm[ps]->pt = prod.get();
                break;
            }
        }
    }

    return S;
}

/*************************************
Parse Math Expression
**************************************/
std::vector<hypothesis> meParser::parse_me(Samples& M)
{
    // Compute the normalized size of a symbol for sample M
    M.detRefSymbol();

    const int N = M.nStrokes();
    const int K = G->noTerminales.size();

    // Cocke-Younger-Kasami (CYK) algorithm for 2D-SCFG
    TableCYK tcyk(N, K);

    // printf("CYK table initialization:\n");
    initCYKterms(M, tcyk, N, K);

    // Compute distances and visibility among strokes
    M.compute_strokes_distances(M.RX, M.RY);

    // Spatial structure for retrieving hypotheses within a certain region
    {
        std::vector<std::unique_ptr<LogSpace>> logspace(N);
        SpaRel SPR(*gmm_spr, M);

        // Init spatial space for size 1
        logspace[1] = std::make_unique<LogSpace>(tcyk.get(1), tcyk.size(1), M.RX, M.RY);

        // Init the parsing table with several multi-stroke symbol segmentation hypotheses
        combineStrokes(M, tcyk, N);

        // printf("\nCYK parsing algorithm\n");
        // printf("Size 1: Generated %d\n", tcyk.size(1));

        // CYK algorithm main loop
        for (int talla = 2; talla <= N; talla++) {

            for (int a = 1; a < talla; a++) {
                int b = talla - a;

                for (CellCYK* c1 = tcyk.get(a); c1; c1 = c1->sig) {
                    // Clear lists
                    c1setH.clear();
                    c1setV.clear();
                    c1setU.clear();
                    c1setI.clear();
                    c1setM.clear();
                    c1setS.clear();

                    // Get the subset of regions close to c1 according to different spatial relations
                    logspace[b]->getH(c1, c1setH); // Horizontal (right)
                    logspace[b]->getV(c1, c1setV); // Vertical (down)
                    logspace[b]->getU(c1, c1setU); // Vertical (up)
                    logspace[b]->getI(c1, c1setI); // Inside (sqrt)
                    logspace[b]->getM(c1, c1setM); // mroot (sqrt[i])

                    for (const auto& c2 : c1setH) {

                        for (const auto& it : G->prodsH) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            const int ps = it->S;
                            const int pa = it->A;
                            const int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getHorProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table (size=talla)
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }

                        for (const auto& it : G->prodsSup) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getSupProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }

                        for (const auto& it : G->prodsSub) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getSubProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }

                    } // end c2=c1setH

                    for (const auto& c2 : c1setV) {

                        for (const auto& it : G->prodsV) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getVerProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps])
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                else
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                            }
                        }

                        // prodsVe
                        for (const auto& it : G->prodsVe) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getVerProb(c1->noterm[pa].get(), c2->noterm[pb].get(), true);
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }

                    } // for in c1setV

                    for (const auto& c2 : c1setU) {

                        for (const auto& it : G->prodsV) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pb] && c2->noterm[pa]) {
                                double cdpr = SPR.getVerProb(c2->noterm[pa].get(), c1->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c2->noterm[pa].get(), c1->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }

                        // ProdsVe
                        for (const auto& it : G->prodsVe) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pb] && c2->noterm[pa]) {
                                double cdpr = SPR.getVerProb(c2->noterm[pa].get(), c1->noterm[pb].get(), true);
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c2->noterm[pa].get(), c1->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }
                    }

                    for (const auto& c2 : c1setI) {

                        for (const auto& it : G->prodsIns) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            const int ps = it->S;
                            const int pa = it->A;
                            const int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getInsProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }
                    }

                    // Mroot
                    for (const auto& c2 : c1setM) {
                        for (const auto& it : G->prodsMrt) {
                            if (it->prior == -FLT_MAX)
                                continue;

                            // Production S -> A B
                            int ps = it->S;
                            int pa = it->A;
                            int pb = it->B;

                            if (c1->noterm[pa] && c2->noterm[pb]) {
                                double cdpr = SPR.getMrtProb(c1->noterm[pa].get(), c2->noterm[pb].get());
                                if (cdpr <= 0.0)
                                    continue;

                                CellCYK* cd = fusion(M, it.get(), c1->noterm[pa].get(), c2->noterm[pb].get(), M.nStrokes(), cdpr);

                                if (!cd)
                                    continue;

                                if (cd->noterm[ps]) {
                                    tcyk.add(talla, cd, ps, G->esInit.get()); // Add to parsing table
                                } else {
                                    tcyk.add(talla, cd, -1, G->esInit.get()); // Add to parsing table
                                }
                            }
                        }
                    }
                    // End Mroot

                    // Look for combining {x_subs} y {x^sups} in {x_subs^sups}
                    for (int pps = 0; pps < c1->nnt; pps++) {

                        // If c1->noterm[pa] is a InternalHypothesis of a subscript (parent_son)
                        if (c1->noterm[pps] && c1->noterm[pps]->prod && c1->noterm[pps]->prod->tipo() == 'B') {

                            logspace[b + c1->noterm[pps]->hi->parent->talla]->getS(c1, c1setS); // sup/sub-scripts union

                            for (const auto& c2 : c1setS) {
                                if (c2->x != c1->x || c1 != c2)
                                    continue;

                                for (const auto& it : G->prodsSSE) {
                                    if (it->prior == -FLT_MAX)
                                        continue;

                                    // Production S -> A B
                                    const int ps = it->S;
                                    const int pa = it->A;
                                    const int pb = it->B;

                                    if (c1->noterm[pa] && c2->noterm[pb] && c1->noterm[pa]->prod && c2->noterm[pb]->prod && c1->noterm[pa]->hi == c2->noterm[pb]->hi && c1->noterm[pa]->prod->tipo() == 'B' && c2->noterm[pb]->prod->tipo() == 'P' && c1->noterm[pa]->hd->parent->compatible(c2->noterm[pb]->hd->parent)) {

                                        // Subscript and superscript should start almost vertically aligned
                                        if (abs(c1->noterm[pa]->hd->parent->x - c2->noterm[pb]->hd->parent->x) > 3 * M.RX)
                                            continue;
                                        // Subscript and superscript should not overlap
                                        if (std::max(it->solape(c1->noterm[pa]->hd, c2->noterm[pb]->hd),
                                                     it->solape(c2->noterm[pb]->hd, c1->noterm[pa]->hd))
                                            > 0.1)
                                            continue;

                                        float prob = c1->noterm[pa]->pr + c2->noterm[pb]->pr - c1->noterm[pa]->hi->pr;

                                        CellCYK* cd = new CellCYK(G->noTerminales.size(), M.nStrokes());

                                        cd->x = std::min(c1->x, c2->x);
                                        cd->y = std::min(c1->y, c2->y);
                                        cd->s = std::max(c1->s, c2->s);
                                        cd->t = std::max(c1->t, c2->t);

                                        cd->noterm[ps] = std::make_unique<InternalHypothesis>(-1, prob, cd, ps);

                                        cd->noterm[ps]->lcen = c1->noterm[pa]->lcen;
                                        cd->noterm[ps]->rcen = c1->noterm[pa]->rcen;
                                        cd->ccUnion(c1, c2);

                                        cd->noterm[ps]->hi = c1->noterm[pa].get();
                                        cd->noterm[ps]->hd = c2->noterm[pb]->hd;
                                        cd->noterm[ps]->prod = it.get();
                                        // Save the production of the superscript in order to recover it when printing the used productions
                                        cd->noterm[ps]->prod_sse = c2->noterm[pb]->prod;

                                        tcyk.add(talla, cd, ps, G->esInit.get());
                                    }
                                }
                            } // end for c2 in c1setS

                            c1setS.clear();
                        }
                    } // end for(int pps=0; pps<c1->nnt; pps++)

                } // end for(CellCYK *c1=tcyk.get(a); c1; c1=c1->sig)

            } // for 1 <= a < talla

            if (talla < N) {
                // Create new logspace structure of size "talla"
                logspace[talla] = std::make_unique<LogSpace>(tcyk.get(talla), tcyk.size(talla), M.RX, M.RY);
            }

            // printf("Size %d: Generated %d\n", talla, tcyk.size(talla));

        } // for 2 <= talla <= N

        // Free memory
    }

    std::vector<hypothesis> out;
    // Get Most Likely InternalHypothesis
    for (int mlh_i = 0; mlh_i < TableCYK::NumHypotheses; ++mlh_i) {
        InternalHypothesis* mlh = tcyk.getMLH(mlh_i);

        if (!mlh) {
            fprintf(stderr, "\nNo hypothesis found!!\n");
            exit(1);
        }

        if (mlh->parent->talla == 0)
            break;

        print_latex(mlh);

        auto& hyp = out.emplace_back();
        fillHypothesis(hyp, mlh, 0);
        printf("hypothesis %d filled\n", mlh_i);

        std::stable_sort(hyp.relations.begin(), hyp.relations.end(), [](const auto& a, const auto& b) {
            return a.parent_id < b.parent_id;
        });

        const auto sz = hyp.tokens.size();
        hyp.tree.resize(sz);
        auto start_it = hyp.relations.begin();
        const auto end_it = hyp.relations.end();
        for(std::size_t i = 0; i < sz; ++i)
        {
            auto cur_it = start_it;
            while(cur_it != end_it && cur_it->parent_id == i)
            {
                ++cur_it;
            }
            hyp.tree[i] = std::span(std::exchange(start_it, cur_it), cur_it);
        }
    }

    return out;
}

/*************************************
End Parsing Math Expression
*************************************/

int meParser::fillHypothesis(hypothesis& into, const InternalHypothesis* H, int id)
{
    int nid;

    if (!H->pt) {
        // Binary production
        //  const int a = H->prod->A;
        //  const int b = H->prod->B;

        const char* self_token = G->key2str(H->ntid);
        const auto self_token_idx = into.tokens.size();
        into.tokens.emplace_back(self_token);
        // printf("Added binary token %s at id %zd = %d?\n", self_token, self_token_idx, id);

        // fprintf(fd, "%s%d -> %s%d [label=%c]\n", self_token, id, G->key2str(a), id+1, H->prod->tipo());

        nid = fillHypothesis(into, H->hi, id + 1);
        into.relations.emplace_back(self_token_idx, nid);
        // printf("Added relation A p %zd, c %d\n", self_token_idx, nid);

        // fprintf(fd, "%s%d -> %s%d [label=%c]\n", self_token, id, G->key2str(b), nid, H->prod->tipo());

        nid = fillHypothesis(into, H->hd, nid);
        into.relations.emplace_back(self_token_idx, nid);
        // printf("Added relation B p %zd, c %d\n", self_token_idx, nid);
    } else {
        std::string aux = H->pt->getTeX(H->clase);
        const auto self_token_idx = into.tokens.size();
        into.tokens.emplace_back(aux);
        // printf("Added terminal token %s at id %zd = %d?\n", aux.c_str(), self_token_idx, id);
        into.relations.emplace_back(id, self_token_idx);
        // printf("Added relation p %d, c (self) %zd\n", id, self_token_idx);

        // Terminal production
        //  fprintf(fd, "T%s%d [shape=box,label=\"%s\"]\n", aux.c_str(), id, H->pt->getTeX(H->clase));
        //  fprintf(fd, "%s%d -> T%s%d\n", G->key2str(H->pt->getNoTerm()), id, aux.c_str(), id);

        nid = self_token_idx;
    }

    return nid;
}

void meParser::print_latex(InternalHypothesis *H) {
    if( !H->pt )
        H->prod->printOut( G.get(), H );
    else {
        std::string clatex = H->pt->getTeX( H->clase );
        printf("%s", clatex.c_str() );
    }
    printf("\n");
}
