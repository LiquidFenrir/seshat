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

#ifndef _INCLUDED_NamedObject_h
#define _INCLUDED_NamedObject_h

#include <iostream>
#include <string>

struct Named {
    std::string name;
    Named(const std::string& n)
        : name(n) { }
    virtual ~Named() = default;
    void print(std::ostream& out = std::cout) const
    {
        out << "\"" << name << "\"";
    }
};

static std::ostream& operator<<(std::ostream& out, const Named& n)
{
    n.print(out);
    return out;
}

#endif
