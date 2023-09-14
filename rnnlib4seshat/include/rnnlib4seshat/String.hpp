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

#ifndef _INCLUDED_String_h
#define _INCLUDED_String_h

#include "Container.hpp"
#include "Helpers.hpp"
#include <cctype>

static std::string ordinal(size_t n)
{
    std::string s = str(n);
    if (n < 100) {
        char c = nth_last(s);
        if (c == '1') {
            return s + "st";
        } else if (c == '2') {
            return s + "nd";
        } else if (c == '3') {
            return s + "rd";
        }
    }
    return s + "th";
}
static void trim(std::string& str)
{
    size_t startpos = str.find_first_not_of(" \t\n");
    size_t endpos = str.find_last_not_of(" \t\n");
    if (std::string::npos == startpos || std::string::npos == endpos) {
        str = "";
    } else {
        str = str.substr(startpos, endpos - startpos + 1);
    }
}
static std::string lower(const std::string& s)
{
    std::string l = s;
    for (auto& c : l) {
        l = std::tolower(c);
    }
    return l;
}
static bool in(const std::string& str, const std::string& search)
{
    return (str.find(search) != std::string::npos);
}
static bool in(const std::string& str, const char* search)
{
    return in(str, std::string(search));
}
static bool begins(const std::string& str, const std::string& search)
{
    return (str.find(search) == 0);
}
static bool begins(const std::string& str, const char* search)
{
    return begins(str, std::string(search));
}
static bool ends(const std::string& str, const std::string& search)
{
    return (str.find(search, str.size() - search.size()) != std::string::npos);
}
static bool ends(const std::string& str, const char* search)
{
    return ends(str, std::string(search));
}
template<class T>
static Vector<T> split(
    const std::string& original, char delim = ' ', size_t maxSplits = 0)
{
    Vector<T> vect;
    std::stringstream ss;
    ss << original;
    std::string s;
    while (delim == ' ' ? ss >> s : getline(ss, s, delim)) {
        vect.push_back(read<T>(s));
        if (vect.size() == maxSplits - 1) {
            delim = '\0';
        }
    }
    return vect;
}
template<class T>
static Vector<T> split_with_repeat(
    const std::string& original, char delim = ' ', char repeater = '*')
{
    Vector<T> vect;
    LOOP(const std::string& s1, split<std::string>(original, delim))
    {
        std::vector<std::string> v = split<std::string>(s1, repeater);
        size_t numRepeats = (v.size() == 1 ? 1 : std::stoi(v[1]));
        T val = read<T>(v[0]);
        vect.insert(vect.end(), numRepeats, val);
    }
    return vect;
}
template<class T, class R>
static std::string join(const R& r, const std::string joinStr = "")
{
    std::string out;
    bool first = true;
    LOOP(const auto& b, r)
    {
        if (first)
            first = false;
        else
            out += joinStr;
        out += b;
    }
    return out;
}

template<class T>
std::string left_pad(const T& val, int width, char fill = '0')
{
    std::ostringstream ss;
    ss << std::setw(width) << std::setfill(fill) << val;
    return ss.str();
}

static std::string int_to_sortable_string(size_t num, size_t max)
{
    assert(num < max);
    return left_pad(num, str(max - 1).size());
}

#endif
