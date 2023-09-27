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

#ifndef _INCLUDED_Connection_h
#define _INCLUDED_Connection_h

#include "Named.hpp"
#include "RealType.hpp"
#include <iostream>

struct Connection : public Named {
    // data
    Layer* from;
    Layer* to;

    // functions
    Connection(const std::string& name, Layer* f, Layer* t)
        : Named(name)
        , from(f)
        , to(t)
    {
        assert(from);
        assert(to);
        assert(from->output_size());
        assert(to->input_size());
    }
    virtual ~Connection() = default;
    virtual size_t num_weights() const { return 0; }
    virtual void feed_forward(const std::vector<int>& coords) { }
    virtual void feed_back(const std::vector<int>& coords) { }
    virtual void update_derivs(const std::vector<int>& coords) { }
    virtual void print(std::ostream& out) const { }
    virtual const View<real_t> weights() { return View<real_t>(); }
};
static std::ostream& operator<<(std::ostream& out, const Connection& c)
{
    c.print(out);
    return out;
}

#endif
