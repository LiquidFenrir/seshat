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

#include <featureson.hpp>
#include <online.hpp>
#include <rnnlib4seshat/RealType.hpp>
#include <samples.hpp>
#include <symfeatures.hpp>
#include <symrec.hpp>

using namespace seshat;

SymFeatures::SymFeatures(const char* mav_on, const char* mav_off)
{
    // Load means and stds normalization
    FILE* fd = fopen(mav_on, "r");
    if (!fd) {
        fprintf(stderr, "Error loading online mav file: %s\n", mav_on);
        exit(-1);
    }

    // Read values online
    for (int i = 0; i < ON_FEAT; i++)
        fscanf(fd, "%lf", &means_on[i]);
    for (int i = 0; i < ON_FEAT; i++)
        fscanf(fd, "%lf", &stds_on[i]);

    fclose(fd);

    fd = fopen(mav_off, "r");
    if (!fd) {
        fprintf(stderr, "Error loading offline mav file: %s\n", mav_off);
        exit(-1);
    }

    // Read values offline
    for (int i = 0; i < OFF_FEAT; i++)
        fscanf(fd, "%lf", &means_off[i]);
    for (int i = 0; i < OFF_FEAT; i++)
        fscanf(fd, "%lf", &stds_off[i]);

    fclose(fd);
}

std::unique_ptr<DataSequence> SymFeatures::getOnline(Samples& M, SegmentHyp& SegHyp)
{
    // Create and fill sequence of points
    sentence sent(SegHyp.stks.size());

    for (const auto it_idx : SegHyp.stks) {
        const auto& cur_stroke = M.getStroke(it_idx);
        auto& st = sent.strokes.emplace_back(cur_stroke.getNPoints(), 1); // means is pendown stroke

        for (int j = 0; j < st.n_points; j++) {
            const Point* p = cur_stroke.get(j);
            st.points.emplace_back(p->x, p->y);
        }
    }

    // Remove repeated points & Median filter
    const sentence sent_filt = sent.no_repeats().smoothed();

    // Compute online features
    sentenceF feat;
    feat.calculate_features(sent_filt);

    // Create DataSequence

    // Set sequence shape
    const int nvec = feat.frames.size();

    // Check number of online features
    if (feat.frames[0].get_fr_dim() != ON_FEAT) {
        fprintf(stderr, "Error: unexpected number of online features\n");
        exit(-1);
    }

    // Create sequence
    auto seq = std::make_unique<DataSequence>(ON_FEAT);

    std::vector<size_t> shape(1);
    shape[0] = nvec;

    // Create aux SeqBuffer to fill data
    seq->inputs = SeqBuffer<real_t>(shape, ON_FEAT);

    // Save the input vectors following the SeqBuffer data representation
    for (int i = 0; i < nvec; i++) {
        for (int j = 0; j < ON_FEAT; j++) {
            double val = feat.frames[i].getFea(j);

            // Normalize to normal(0,1)
            val = (val - means_on[j]) / stds_on[j];

            seq->inputs.data[i * ON_FEAT + j] = val;
        }
    }

    // Create target vector (content doesn't matter, just because it's required)
    std::vector<int> target(nvec);
    shape[0] = nvec;
    seq->targetClasses.data = target;
    seq->targetClasses.shape = shape;
    seq->tag = "none";

    // Return extracted features for the sequence of strokes
    return seq;
}

std::unique_ptr<DataSequence> SymFeatures::getOfflineFKI(VectorImage& img, int H, int W)
{
    // Create sequence
    auto seq = std::make_unique<DataSequence>(OFF_FEAT);

    // Set sequence shape
    int nvec = W;
    std::vector<size_t> shape(1);
    shape[0] = nvec;

    // Create aux SeqBuffer to fill data
    seq->inputs = SeqBuffer<real_t>(shape, OFF_FEAT);

    // Compute FKI offline features
    double c[OFF_FEAT + 1];
    double c4ant = H + 1, c5ant = 0;

    // For every column
    for (int x = 0; x < W; x++) {

        // Compute the FKI 9 features
        for (auto& c_i : c)
            c_i = 0;
        c[4] = H + 1;

        for (int y = 1; y <= H; y++) {
            if (img.img[(y - 1) * img.width + x]) { // Black pixel
                c[1] += 1;
                c[2] += y;
                c[3] += y * y;
                if (y < c[4])
                    c[4] = y;
                if (y > c[5])
                    c[5] = y;
            }
            if (y > 1 && img.img[(y - 1) * img.width + x] != img.img[(y - 2) * img.width + x])
                c[8] += 1;
        }

        c[2] /= H;
        c[3] /= H * H;

        for (int y = c[4] + 1; y < c[5]; y++)
            if (img.img[(y - 1) * img.width + x]) // Black pixel
                c[9] += 1;

        c[6] = H + 1;
        c[7] = 0;
        if (x + 1 < W) {
            for (int y = 1; y <= H; y++) {
                if (img.img[(y - 1) * img.width + x + 1]) { // Black pixel
                    if (y < c[6])
                        c[6] = y;
                    if (y > c[7])
                        c[7] = y;
                }
            }
        }
        c[6] = (c[6] - c4ant) / 2;
        c[7] = (c[7] - c5ant) / 2;

        c4ant = c[4];
        c5ant = c[5];

        // Save the input vectors following the SeqBuffer data representation
        for (int j = 0; j < OFF_FEAT; j++) {
            // Normalize to normal(0,1)
            c[j + 1] = (c[j + 1] - means_off[j]) / stds_off[j];

            seq->inputs.data[x * OFF_FEAT + j] = c[j + 1];
        }
    }

    // Create target vector (content doesn't matter, just because it's required)
    std::vector<int> target(nvec);
    shape[0] = nvec;
    seq->targetClasses.data = target;
    seq->targetClasses.shape = shape;
    seq->tag = "none";

    // Return extracted features for the sequence of strokes
    return seq;
}
