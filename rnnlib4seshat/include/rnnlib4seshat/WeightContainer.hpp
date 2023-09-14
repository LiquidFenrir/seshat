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


This file is a modification of the RNNLIB original software covered by
the following copyright and permission notice:

*/
/*Copyright 2009,2010 Alex Graves

  This file is part of RNNLIB.

  RNNLIB is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  RNNLIB is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with RNNLIB.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _INCLUDED_WeightContainer_h
#define _INCLUDED_WeightContainer_h

#include "DataExporter.hpp"
#include "Random.hpp"
#include "RealType.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <tuple>
#include <vector>

typedef std::multimap<std::string, std::tuple<std::string, std::string, int, int>>::iterator WC_CONN_IT;
typedef std::pair<std::string, std::tuple<std::string, std::string, int, int>> WC_CONN_PAIR;

struct WeightContainer : public DataExporter {
    // data
    Vector<real_t> weights;
    Vector<real_t> derivatives;
    std::multimap<std::string, std::tuple<std::string, std::string, int, int>> connections;

    // functions
    WeightContainer(DataExportHandler* deh)
        : DataExporter("weightContainer", deh)
    {
    }

    void link_layers(const std::string& fromName, const std::string& toName, const std::string& connName = "", int paramBegin = 0, int paramEnd = 0)
    {
        connections.insert(std::make_pair(toName, std::make_tuple(fromName, connName, paramBegin, paramEnd)));
    }

    std::pair<size_t, size_t> new_parameters(size_t numParams, const std::string& fromName, const std::string& toName, const std::string& connName)
    {
        size_t begin = weights.size();
        weights.resize(weights.size() + numParams);
        size_t end = weights.size();
        link_layers(fromName, toName, connName, begin, end);
        return std::make_pair(begin, end);
    }

    View<real_t> get_weights(std::pair<int, int> range)
    {
        return weights.slice(range);
    }

    View<real_t> get_derivs(std::pair<int, int> range)
    {
        return derivatives.slice(range);
    }

    int randomise(real_t range)
    {
        int numRandWts = 0;
        LOOP(real_t & w, weights)
        {
            if (w == infinity) {
                w = Random::uniform(range);
                ++numRandWts;
            }
        }
        return numRandWts;
    }

    void reset_derivs()
    {
        fill(derivatives, 0);
    }

    void save_by_conns(std::vector<real_t>& container, const std::string& nam)
    {
        LOOP(const WC_CONN_PAIR& p, connections)
        {
            VDI begin = container.begin() + std::get<2>(p.second);
            VDI end = container.begin() + std::get<3>(p.second);
            if (begin != end) {
                save_range(std::make_pair(begin, end), std::get<1>(p.second) + "_" + nam);
            }
        }
    }

    // MUST BE CALLED BEFORE WEIGHT CONTAINER IS USED
    void build()
    {
        fill(weights, infinity);
        derivatives.resize(weights.size());
        save_by_conns(weights, "weights");
        reset_derivs();
    }
};

void perturb_weight(real_t& weight, real_t stdDev, bool additive = true);
template<class R>
void perturb_weights(R& weights, real_t stdDev, bool additive = true);
template<class R>
void perturb_weights(R& weights, R& stdDevs, bool additive = true);

#endif
