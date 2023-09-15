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
#include <array>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <gmm.hpp>

#define PI 3.14159265359

GMM::GMM(const char* model)
{
    loadModel(model);
}

void GMM::loadModel(const char* str)
{
    FILE* fd = fopen(str, "r");
    if (!fd) {
        fprintf(stderr, "Error loading GMM model file '%s'\n", str);
        exit(-1);
    }

    // Read parameters
    fscanf(fd, "%d %d %d", &C, &D, &G);

    // Read prior probabilities
    prior.resize(C);
    for (auto& f_i : prior)
        fscanf(fd, "%f", &f_i);

    invcov.reshape(std::array{C, G, D});
    mean.reshape(std::array{C, G, D});
    weight.reshape(std::array{C, G});
    det.reshape(std::array{C, G}, 1.0f);

    // Read a GMM for each class
    std::array<int, 1> c_arr = {0};
    for (int& c = c_arr[0]; c < C; c++) {

        // Read diagonal covariances
        for (int i = 0; i < G; i++) {
            auto& cur_det = det.get(std::array{c, i});
            for (auto& ic_j : invcov[std::array{c, i}]) {
                fscanf(fd, "%f", &ic_j);

                // Compute determinant of convariance matrix (diagonal)
                cur_det *= ic_j;

                // Save the inverse of the convariance to save future computations
                if (ic_j == 0) {
                    fprintf(stderr, "Warning: covariance value equal to zero in GMM\n");
                    ic_j = 1.0 / 1.0e-10;
                } else
                    ic_j = 1.0 / ic_j;
            }
        }

        // Read means
        for (auto& m_i : mean[c_arr]) {
            fscanf(fd, "%f", &m_i);
        }

        // Read mixture weights
        for (auto& w_i : weight[c_arr])
            fscanf(fd, "%f", &w_i);
    }

    fclose(fd);
}

// Probability density function
float GMM::pdf(const int c, std::span<const float> v)
{
    assert(v.size() >= D);
    float pr = 0.0;

    for (int i = 0; i < G; i++) {

        float exponent = 0.0;
        for (int j = 0; j < D; j++)
            exponent += (v[j] - mean.get(std::array{c, i, j})) * invcov.get(std::array{c, i, j}) * (v[j] - mean.get(std::array{c, i, j}));

        exponent *= -0.5;

        pr += weight.get(std::array{c, i}) * pow(2 * PI, -D / 2.0) * pow(det.get(std::array{c, i}), -0.5) * exp(exponent);
    }

    return prior[c] * pr;
}

void GMM::posterior(std::span<const float> x, std::span<float> pr)
{
    assert(pr.size() >= C);

    float total = 0;
    for (int c = 0; c < C; c++) {
        pr[c] = pdf(c, x);
        total += pr[c];
    }

    for (int c = 0; c < C; c++)
        pr[c] /= total;
}
