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

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <gmm.hpp>
#include <internal_hypothesis.hpp>
#include <samples.hpp>
#include <sparel.hpp>

using namespace seshat;

// Aux functions

InternalHypothesis* leftmost(InternalHypothesis* h)
{
    if (h->pt)
        return h;

    InternalHypothesis* izq = leftmost(h->hi);
    InternalHypothesis* der = leftmost(h->hd);

    return izq->parent->x < der->parent->x ? izq : der;
}

InternalHypothesis* rightmost(InternalHypothesis* h)
{
    if (h->pt)
        return h;

    InternalHypothesis* izq = rightmost(h->hi);
    InternalHypothesis* der = rightmost(h->hd);

    return izq->parent->s > der->parent->s ? izq : der;
}

// Percentage of the area of region A that overlaps with region B
float solape(CellCYK* a, CellCYK* b)
{
    int x = std::max(a->x, b->x);
    int y = std::max(a->y, b->y);
    int s = std::min(a->s, b->s);
    int t = std::min(a->t, b->t);

    if (s >= x && t >= y) {
        float aSolap = (s - x + 1.0) * (t - y + 1.0);
        float aTotal = (a->s - a->x + 1.0) * (a->t - a->y + 1.0);

        return aSolap / aTotal;
    }

    return 0.0;
}

//
// SpaRel methods
//

SpaRel::SpaRel(GMM& gmm, Samples& m)
    : model{ gmm }
    , mue{ m }
{
}

void SpaRel::smooth(float* post)
{
    for (int i = 0; i < NRELS; i++)
        post[i] = (post[i] + 0.02) / (1.00 + NRELS * 0.02);
}

void SpaRel::getFeas(InternalHypothesis* a, InternalHypothesis* b, float* sample, int ry)
{
    // Normalization factor: combined height
    float F = std::max(a->parent->t, b->parent->t) - std::min(a->parent->y, b->parent->y) + 1;

    sample[0] = (b->parent->t - b->parent->y + 1) / F;
    sample[1] = (a->rcen - b->lcen) / F;
    sample[2] = ((a->parent->s + a->parent->x) / 2.0 - (b->parent->s + b->parent->x) / 2.0) / F;
    sample[3] = (b->parent->x - a->parent->s) / F;
    sample[4] = (b->parent->x - a->parent->x) / F;
    sample[5] = (b->parent->s - a->parent->s) / F;
    sample[6] = (b->parent->y - a->parent->t) / F;
    sample[7] = (b->parent->y - a->parent->y) / F;
    sample[8] = (b->parent->t - a->parent->t) / F;
}

double SpaRel::compute_prob(InternalHypothesis* h1, InternalHypothesis* h2, int k)
{

    // Set probabilities according to spatial constraints

    if (k <= 2) {
        // Check left-to-right order constraint in Hor/Sub/Sup relationships
        InternalHypothesis* rma = rightmost(h1);
        InternalHypothesis* lmb = leftmost(h2);

        if (lmb->parent->x < rma->parent->x || lmb->parent->s <= rma->parent->s)
            return 0.0;
    }

    // Compute probabilities
    float sample[NFEAT];

    getFeas(h1, h2, sample, mue.RY);

    // Get spatial relationships probability from the model
    model.posterior(sample, probs);

    // Slightly smooth probabilities because GMM classifier can provide
    // to biased probabilities. Thsi way we give some room to the
    // language model (the 2D-SCFG grammar)
    smooth(probs);

    return probs[k];
}

double SpaRel::getHorProb(InternalHypothesis* ha, InternalHypothesis* hb)
{
    return compute_prob(ha, hb, 0);
}
double SpaRel::getSubProb(InternalHypothesis* ha, InternalHypothesis* hb)
{
    return compute_prob(ha, hb, 1);
}
double SpaRel::getSupProb(InternalHypothesis* ha, InternalHypothesis* hb)
{
    return compute_prob(ha, hb, 2);
}
double SpaRel::getVerProb(InternalHypothesis* ha, InternalHypothesis* hb, bool strict)
{
    // Pruning
    if (hb->parent->y < (ha->parent->y + ha->parent->t) / 2 || abs((ha->parent->x + ha->parent->s) / 2 - (hb->parent->x + hb->parent->s) / 2) > 2.5 * mue.RX || (hb->parent->x > ha->parent->s || hb->parent->s < ha->parent->x))
        return 0.0;

    if (!strict)
        return compute_prob(ha, hb, 3);

    // Penalty for strict relationships
    float penalty = abs(ha->parent->x - hb->parent->x) / (3.0 * mue.RX) + abs(ha->parent->s - hb->parent->s) / (3.0 * mue.RX);

    if (penalty > 0.95)
        penalty = 0.95;

    return (1.0 - penalty) * compute_prob(ha, hb, 3);
}

double SpaRel::getInsProb(InternalHypothesis* ha, InternalHypothesis* hb)
{
    if (solape(hb->parent, ha->parent) < 0.5 || hb->parent->x < ha->parent->x || hb->parent->y < ha->parent->y)
        return 0.0;

    return compute_prob(ha, hb, 4);
}
double SpaRel::getMrtProb(InternalHypothesis* ha, InternalHypothesis* hb)
{
    return compute_prob(ha, hb, 5);
}
