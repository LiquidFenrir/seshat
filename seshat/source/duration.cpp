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

#include <duration.hpp>
#include <span>

using namespace seshat;

DurationModel::DurationModel(const fs::path& path, int mxs, SymRec* sr)
{
    std::ifstream fd(path);
    if (!fd) {
        std::cerr << "Error loading duration model '" << path << "'\n";
        throw std::runtime_error("Error loading duration model");
    }

    max_strokes = mxs;
    Nsyms = sr->getNClases();

    duration_prob.reshape(std::array{ Nsyms, max_strokes }, 0);

    loadModel(fd, sr);
}

void DurationModel::loadModel(std::istream& is, SymRec* sr)
{
    std::string str;
    int count, nums;

    // Load data
    while (is >> count >> str >> nums) {
        if (nums <= max_strokes)
            duration_prob.get(std::array{ sr->keyClase(str), nums - 1 }) = count;
    }

    // Compute probabilities
    for (int i = 0; i < Nsyms; ++i) {
        auto& dp = duration_prob[std::array{ i }];
        int total = 0;

        for (auto& dpf : dp) {
            if (dpf == 0) // Add-one smoothing
                dpf = 1;
            total += dpf;
        }

        for (auto& dpf : dp) {
            dpf /= total;
        }
    }
}

float DurationModel::prob(int symclas, int size)
{
    return duration_prob.get(std::array{ symclas, size - 1 });
}
