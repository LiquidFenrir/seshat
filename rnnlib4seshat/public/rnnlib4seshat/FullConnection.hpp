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

#ifndef _INCLUDED_FullConnection_h
#define _INCLUDED_FullConnection_h

#include "Connection.hpp"
#include "DataExporter.hpp"
#include "Helpers.hpp"
#include "Layer.hpp"
#include "Matrix.hpp"
#include "WeightContainer.hpp"
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

struct FullConnection : public Connection {
    // data
    std::vector<int> delay;
    std::vector<int> delayedCoords;
    FullConnection* source;
    std::pair<size_t, size_t> paramRange;
    WeightContainer* wc;

    // functions
    FullConnection(Layer* f, Layer* t, WeightContainer* weight, const std::vector<int>& d = {}, FullConnection* s = 0)
        : Connection(make_name(f, t, d), f, t)
        , source(s)
    // paramRange(source ? source->paramRange : wc->new_parameters(this->from->output_size() * this->to->input_size(), this->from->name, this->to->name, name))
    {
        wc = weight;
        if (source) {
            paramRange = source->paramRange;
            wc->link_layers(this->from->name, this->to->name, this->name, paramRange.first, paramRange.second);
        } else
            paramRange = wc->new_parameters(this->from->output_size() * this->to->input_size(), this->from->name, this->to->name, name);
        set_delay(d);
        assert(num_weights() == (this->from->output_size() * this->to->input_size()));
        if (this->from->name != "bias" && this->from != this->to && !this->to->source) {
            this->to->source = this->from;
        }
    }
    ~FullConnection() = default;
    void set_delay(const std::vector<int>& d)
    {
        delay = d;
        assert(delay.size() == 0 || delay.size() == this->from->num_seq_dims());
        delayedCoords.resize(delay.size());
    }
    static const std::string& make_name(Layer* f, Layer* t, const std::vector<int>& d)
    {
        static std::string name;
        name = f->name + "_to_" + t->name;
        if (find_if(d.begin(), d.end(), std::bind(std::not_equal_to<int>(), std::placeholders::_1, 0)) != d.end()) {
            std::ostringstream temp;
            temp << "_delay_";
            std::copy(d.begin(), d.end() - 1, std::ostream_iterator<int>(temp, "_"));
            temp << d.back();
            name += temp.str();
        }
        return name;
    }
    const View<real_t> weights()
    {
        return wc->get_weights(paramRange);
    }
    const View<real_t> derivs()
    {
        return wc->get_derivs(paramRange);
    }
    size_t num_weights() const
    {
        return difference(paramRange);
    }
    const std::vector<int>* add_delay(const std::vector<int>& toCoords)
    {
        if (delay.empty()) {
            return &toCoords;
        }
        range_plus(delayedCoords, toCoords, delay);
        if (this->from->outputActivations.in_range(delayedCoords)) {
            return &delayedCoords;
        }
        return 0;
    }
    void feed_forward(const std::vector<int>& toCoords)
    {
        const std::vector<int>* fromCoords = add_delay(toCoords);
        if (fromCoords) {
            dot(this->from->out_acts(*fromCoords), weights().begin(), this->to->inputActivations[toCoords]);
        }
    }
    void feed_back(const std::vector<int>& toCoords)
    {
        const std::vector<int>* fromCoords = add_delay(toCoords);
        if (fromCoords) {
            dot_transpose(this->to->inputErrors[toCoords], weights().begin(), this->from->out_errs(*fromCoords));
        }
    }
    void update_derivs(const std::vector<int>& toCoords)
    {
        const std::vector<int>* fromCoords = add_delay(toCoords);
        if (fromCoords) {
            outer(this->from->out_acts(*fromCoords), derivs().begin(), this->to->inputErrors[toCoords]);
        }
    }
    void print(std::ostream& out) const
    {
        Named::print(out);
        out << " (" << num_weights() << " wts";
        if (source) {
            out << " shared with " << source->name;
        }
        out << ")";
    }
};

#endif
