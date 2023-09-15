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

#ifndef _INCLUDED_Helpers_h
#define _INCLUDED_Helpers_h

#include "Log.hpp"
#include "RealType.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <concepts>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <random>
#include <ranges>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace std {

template<typename T1, typename T2>
std::size_t size(const std::pair<T1, T2>& p)
{
    return std::distance(p.first, p.second);
}

}

template<class P>
concept PairLike = requires(P p) {
    typename P::first_type;
    typename P::second_type;
    p.first;
    p.second;
};
template<typename R>
auto make_subrange_for(R&& r)
{
    if constexpr (PairLike<std::remove_cvref_t<R>>) {
        const auto p = r;
        return std::ranges::subrange(std::get<0>(p), std::get<1>(p));
    } else
        return std::views::all(std::forward<R>(r));
}
template<class R>
concept RangeLike = requires(R&& r)
{
    make_subrange_for(r);
};

template<RangeLike R>
using RangeValueType_t = std::ranges::range_value_t<decltype(make_subrange_for(std::declval<R>()))>;

#define LOOP(x, y) for (x : make_subrange_for(y))
#define LOOP_BACK(x, y) for (x : make_subrange_for(y) | std::views::reverse)
#define FOR(i, n) for (int i = 0; i < (n); ++i)

typedef std::vector<size_t>::const_iterator VSTCI;
typedef std::vector<real_t>::iterator VDI;
typedef std::vector<real_t>::const_iterator VDCI;
typedef std::vector<real_t>::reverse_iterator VDRI;
typedef std::string::iterator SI;
typedef std::string::const_iterator SCI;
typedef std::vector<int>::iterator VII;
typedef std::vector<std::string>::iterator VSI;
typedef std::vector<std::string>::const_iterator VSCI;
typedef std::vector<int>::reverse_iterator VIRI;
typedef std::vector<std::vector<int>>::reverse_iterator VVIRI;
typedef std::vector<int>::const_iterator VICI;
typedef std::vector<bool>::iterator VBI;
typedef std::vector<real_t>::iterator VFI;
typedef std::vector<real_t>::const_iterator VFCI;
typedef std::vector<std::vector<real_t>>::iterator VVDI;
typedef std::vector<std::vector<real_t>>::const_iterator VVDCI;
typedef std::vector<std::vector<int>>::iterator VVII;
typedef std::vector<std::vector<int>>::const_iterator VVICI;
typedef std::vector<unsigned int>::iterator VUII;
typedef std::vector<std::vector<real_t>>::iterator VVFI;
typedef std::map<std::string, std::string>::iterator MSSI;
typedef std::map<std::string, std::string>::const_iterator MSSCI;
typedef std::map<std::string, real_t>::iterator MSDI;
typedef std::map<std::string, real_t>::const_iterator MSDCI;
typedef std::map<std::string, std::pair<int, real_t>>::iterator MSPIDI;
typedef std::map<std::string, std::pair<int, real_t>>::const_iterator MSPIDCI;
typedef std::vector<std::map<std::string, std::pair<int, real_t>>>::const_iterator VMSDCI;
typedef std::vector<std::map<std::string, std::pair<int, real_t>>>::iterator VMSDI;
typedef std::vector<std::map<std::string, std::pair<int, real_t>>>::reverse_iterator VMSDRI;
typedef std::map<std::string, int>::iterator MSII;
typedef std::map<std::string, int>::const_iterator MSICI;
typedef std::map<int, int>::iterator MIII;
typedef std::map<int, int>::const_iterator MIICI;
typedef std::vector<std::vector<int>>::const_reverse_iterator VVIRCI;
typedef std::vector<int>::const_reverse_iterator VIRCI;
typedef std::vector<const real_t*>::const_iterator VPCFCI;
typedef std::vector<const real_t*>::iterator VPCFI;
typedef std::vector<const real_t*>::const_reverse_iterator VPCFCRI;
typedef std::vector<bool>::const_iterator VBCI;
typedef std::vector<bool>::iterator VBI;
typedef std::map<std::string, std::pair<real_t, int>>::iterator MCSPDII;
typedef std::map<std::string, std::pair<real_t, int>>::const_iterator MCSPDICI;
typedef std::pair<std::string, real_t> PSD;
typedef std::pair<std::string, std::string> PSS;
typedef const std::tuple<real_t&, real_t&, real_t&, real_t&>& TDDDD;
typedef const std::tuple<real_t&, real_t&, real_t&, real_t&, real_t&>& TDDDDD;
typedef const std::tuple<real_t&, real_t&, real_t&>& TDDD;
typedef const std::tuple<real_t&, real_t&, int&>& TDDI;
typedef const std::tuple<real_t&, real_t&, real_t&>& TDDF;
typedef const std::tuple<real_t&, real_t&, real_t>& TDDCF;
typedef const std::tuple<real_t&, real_t&>& TDD;
typedef const std::tuple<int, std::string>& TIS;
typedef const std::tuple<int, int>& TII;
typedef const std::tuple<int, real_t>& TID;
typedef const std::tuple<int, std::set<int>&>& TISETI;
typedef const std::tuple<int&, bool, int>& TIBI;
typedef const std::tuple<real_t, Log<real_t>&>& TDL;
typedef const std::tuple<real_t&, Log<real_t>, Log<real_t>>& TDLL;
typedef Log<real_t> prob_t;

// global variables
static const real_t realMax = std::numeric_limits<real_t>::max();
static const real_t realMin = std::numeric_limits<real_t>::min();
static const real_t infinity = std::numeric_limits<real_t>::infinity();
inline constexpr bool runningGradTest = false;
inline constexpr bool verbose = false;

#define PRINT(x, o) ((o) << std::boolalpha << "" #x " = " << (x) << std::endl)
#define PRINTN(x, o)                                 \
    (o) << std::boolalpha << "" #x ":" << std::endl; \
    print_range((o), (x), std::string("\n"));        \
    (o) << std::endl
#define PRT(x) PRINT(x, std::cout)
#define PRTN(x) PRINTN(x, std::cout)
#define PRINTR(x, o, d)                   \
    (o) << std::boolalpha << "" #x " = "; \
    print_range((o), (x), str(d));        \
    (o) << std::endl
#define PRTR(x, d) PRINTR((x), std::cout, (d))
#define check(condition, str)                         \
    if (!(condition)) {                               \
        std::cout << std::endl                        \
                  << "ERRROR: " << (str) << std::endl \
                  << std::endl;                       \
        (assert((condition)));                        \
    }
#define CHECK_STRICT(condition, str)                  \
    if (!(condition)) {                               \
        std::cout << std::endl                        \
                  << "ERRROR: " << (str) << std::endl \
                  << std::endl;                       \
        (assert((condition)));                        \
        std::exit(0);                                 \
    }

// MISC FUNCTIONS
static bool warn_unless(
    bool condition, const std::string& str, std::ostream& out = std::cout)
{
    if (!condition) {
        out << "WARNING: " << str << std::endl;
    }
    return condition;
}
static void print_time(
    real_t totalSeconds, std::ostream& out = std::cout, bool abbrv = false)
{
    int wholeSeconds = floor(totalSeconds);
    int seconds = wholeSeconds % 60;
    int totalMinutes = wholeSeconds / 60;
    int minutes = totalMinutes % 60;
    int totalHours = totalMinutes / 60;
    int hours = totalHours % 24;
    int totalDays = totalHours / 24;
    int days = totalDays % 365;
    if (days) {
        out << days << " day";
        if (days > 1) {
            out << "s";
        }
        out << " ";
    }
    if (hours) {
        out << hours << (abbrv ? " hr" : " hour");
        if (hours > 1) {
            out << "s";
        }
        out << " ";
    }
    if (minutes) {
        out << minutes << (abbrv ? " min" : " minute");
        if (minutes > 1) {
            out << "s";
        }
        out << " ";
    }
    out << totalSeconds - wholeSeconds + seconds
        << (abbrv ? " secs" : " seconds");
}
static void mark()
{
    static int num = 0;
    std::cout << "MARK " << num << std::endl;
    ++num;
}
template<class T>
static T squared(const T& t)
{
    return t * t;
}
template<class T>
static int sign(const T& t)
{
    if (t < 0) {
        return -1;
    } else if (t > 0) {
        return 1;
    } else {
        return 0;
    }
}

template<class T>
static T bound(const T& v, const T& minVal, const T& maxVal)
{
    return std::clamp(v, minVal, maxVal);
}

template<class C, class Tr, class R>
static void print_range(
    std::basic_ostream<C, Tr>& out, const R& r,
    const std::basic_string<C, Tr>& delim = " ")
{
    bool first = true;
    LOOP(const auto& cval, r)
    {
        if (first)
            first = false;
        else
            out << delim;
        out << cval;
    }
}

template<class C, class Tr, class R>
static std::basic_ostream<C, Tr>& operator<<(
    std::basic_ostream<C, Tr>& out, const R& r)
{
    print_range(out, r);
    return out;
}

// CAST OPERATIONS
template<class T>
static std::string str(const T& t)
{
    std::ostringstream ss;
    ss << t;
    return ss.str();
}

// GENERIC RANGE OPERATIONS
template<RangeLike R, class T>
static auto find(R& r, const T& t)
{
    return std::ranges::find(make_subrange_for(r), t);
}

template<class T1, class T2>
static auto iota_range(T1 t1, T2 t2)
{
    return std::ranges::iota_view<T2, T2>((t1 < t2 ? static_cast<T2>(t1) : t2), t2);
}

template<class T>
static auto iota_range(T t)
{
    return iota_range(0, t);
}

template<RangeLike R>
static auto indices(const R& r)
{
    return iota_range<int>(std::size(r));
}
template<RangeLike R1, RangeLike R2, class F>
static auto transform(const R1& r1, R2& r2, const F& f)
{
    return std::ranges::transform(make_subrange_for(r1), std::begin(make_subrange_for(r2)), f).out;
}
template<RangeLike R>
static bool in_range(R& r, size_t n)
{
    return n >= 0 && n < std::size(r);
}
template<RangeLike R>
static auto& nth_last(R& r, size_t n = 1)
{
    check(in_range(r, n - 1), "nth_last called with n = " + str(n) + " for range of size " + (str(std::size(r))));
    return *(std::end(r) - n);
}
template<RangeLike R>
static size_t last_index(const R& r)
{
    return (std::size(r) - 1);
}
template<RangeLike R, class T>
static bool in(const R& r, const T& t)
{
    auto vr = make_subrange_for(r);
    return std::end(vr) != std::ranges::find(vr, t);
}
template<RangeLike R, class T>
static size_t index(const R& r, const T& t)
{
    auto vr = make_subrange_for(r);
    return std::distance(std::begin(vr), std::ranges::find(vr, t));
}
template<RangeLike R>
static void reverse(R& r)
{
    std::ranges::reverse(make_subrange_for(r));
}

template<RangeLike R>
static auto minmax(const R& r)
{
    const auto [mini_val, maxi_val] = std::ranges::minmax(make_subrange_for(r));
    return std::pair<RangeValueType_t<R>, RangeValueType_t<R>>(mini_val, maxi_val);
}
template<RangeLike R>
static void bound_range(R& r, const RangeValueType_t<R>& minVal, const RangeValueType_t<R>& maxVal)
{
    LOOP(auto& vr, r)
    {
        vr = bound(vr, minVal, maxVal);
    }
}

template<RangeLike R>
static void fill(R& r, const RangeValueType_t<R>& v)
{
    std::ranges::fill(make_subrange_for(r), v);
}
template<typename T>
static void flood(std::vector<T>& r, size_t size, const T& v = 0)
{
    r.resize(size);
    fill(r, v);
}
template<RangeLike R>
static size_t count(const R& r, const RangeValueType_t<R>& v)
{
    return std::ranges::count(make_subrange_for(r), v);
}
template<RangeLike R1, RangeLike R2>
static void copy(const R1& source, R2& dest)
{
    auto a = make_subrange_for(source);
    auto b = make_subrange_for(dest);
    assert(std::ranges::size(a) >= std::ranges::size(b));
    std::ranges::copy(a, std::begin(b));
}

template<RangeLike R1, RangeLike R2>
static bool equal(const R1& source, R2& dest)
{
    return std::ranges::equal(make_subrange_for(source), make_subrange_for(dest));
}

template<RangeLike R>
static R& shuffle(R& r)
{
    static std::mt19937 shufflegen(std::time(nullptr));
    std::ranges::shuffle(make_subrange_for(r), shufflegen);
    return r;
}
// template <RangeLike R> static auto max(const R& r) {
//   return std::ranges::max_element(make_subrange_for(r));
// }

template<class C, class Tr, class R>
static void print_range(std::basic_ostream<C, Tr>& out, const R& r, const char delim)
{
    print_range(out, r, str(delim));
}

template<class C, class Tr, class R>
static std::basic_istream<C, Tr>& operator>>(std::basic_istream<C, Tr>& in, R& r)
{
    LOOP(auto& val, r)
    {
        in >> val;
    }
    return in;
}
template<RangeLike R>
void delete_range(R& r)
{
    LOOP(const auto it, r)
    {
        delete it;
    }
}

template<RangeLike R1, RangeLike R2, RangeLike R3>
static size_t range_min_size(
    const R1& a, const R2& b, const R3& c)
{
    return std::min(std::min(std::size(a), std::size(b)), std::size(c));
}

template<RangeLike R>
static int arg_max(const R& r)
{
    auto vr = make_subrange_for(r);
    return std::distance(std::begin(vr), std::ranges::max_element(vr));
}

template<class... Rs>
using zip = std::ranges::zip_view<Rs...>;

// ARITHMETIC RANGE OPERATIONS
template<RangeLike R1, RangeLike R2>
static RangeValueType_t<R1> inner_product(const R1& a, const R2& b, RangeValueType_t<R1> c = 0)
{
    auto va = make_subrange_for(a);
    auto vb = make_subrange_for(b);
    return std::inner_product(std::begin(va), std::end(va), std::begin(vb), c);
}
template<RangeLike R>
static auto norm(const R& r)
{
    return sqrt(inner_product(r, r));
}

template<RangeLike R>
static auto product(const R& r)
{
    return std::ranges::fold_left(make_subrange_for(r),
                                  RangeValueType_t<R>(1), [](const auto& x, const auto& y) {
                                      return x * y;
                                  });
}
template<RangeLike R>
static auto sum(const R& r)
{
    return std::ranges::fold_left(make_subrange_for(r),
                                  RangeValueType_t<R>(0), [](const auto& x, const auto& y) {
                                      return x + y;
                                  });
}
template<RangeLike R>
static auto abs_sum(const R& r)
{
    decltype(*std::begin(make_subrange_for(r))) v = 0;
    LOOP(const auto& vr, r)
    {
        v += abs(vr);
    }
    return v;
}

template<RangeLike R>
static auto mean(const R& r)
{
    return sum(r) / (decltype(*std::begin(make_subrange_for(r))))std::size(r);
}
template<RangeLike R>
static auto variance(const R& r)
{
    auto M = mean(r);
    decltype(M) v = 0;

    LOOP(const auto& vr, r)
    {
        v += squared(vr - M);
    }
    return v / std::size(r);
}
template<RangeLike R>
static auto std_dev(const R& r)
{
    return sqrt(variance(r));
}
// plus
template<RangeLike R1, RangeLike R2, RangeLike R3>
static R1& range_plus(R1& a, const R2& b, const R3& c)
{
    std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
        return x + y;
    });
    return a;
}
template<RangeLike R1, RangeLike R2>
static void range_plus_equals(R1& a, const R2& b)
{
    range_plus(a, a, b);
}
// minus
template<RangeLike R1, RangeLike R2, RangeLike R3>
static void range_minus(R1& a, const R2& b, const R3& c)
{
    std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
        return x - y;
    });
}
// multiply
template<RangeLike R1, RangeLike R2>
static void range_multiply_val(R1& a, const R2& b, const RangeValueType_t<R2>& c)
{
    std::ranges::transform(make_subrange_for(b), std::begin(make_subrange_for(a)), [&c](const auto& x) {
        return x * c;
    });
}
template<RangeLike R>
static void range_multiply_val(R& a, const RangeValueType_t<R>& b)
{
    range_multiply_val(a, a, b);
}
template<RangeLike R1, RangeLike R2, RangeLike R3>
static void range_multiply(R1& a, const R2& b, const R3& c)
{
    std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
        return x * y;
    });
}

// divide
template<RangeLike R1, RangeLike R2>
static void range_divide_val(R1& a, const R2& b, const RangeValueType_t<R1>& c)
{
    std::ranges::transform(make_subrange_for(b), std::begin(make_subrange_for(a)), [&c](const auto& x) {
        return x / c;
    });
}
template<RangeLike R>
static void range_divide_val(R& a, const RangeValueType_t<R>& b)
{
    range_divide_val(a, a, b);
}

// PAIR OPERATIONS
template<class T>
static bool in_open_interval(std::pair<T, T> interval, T val)
{
    return ((val > interval.first) && (val < interval.second));
}
template<class T>
static bool in_closed_interval(std::pair<T, T> interval, T val)
{
    return ((val >= interval.first) && (val <= interval.second));
}
template<class T1, class T2>
static void operator+=(std::pair<T1, T2>& a, const std::pair<T1, T2>& b)
{
    a.first += b.first;
    a.second += b.second;
}
template<class T1, class T2, class T3>
static std::pair<T1, T2> operator+(const std::pair<T1, T2>& a, const T3& b)
{
    return std::make_pair(a.first + b, a.second + b);
}
template<class T1, class T2>
static std::ostream& operator<<(std::ostream& out, const std::pair<T1, T2>& p)
{
    out << p.first << " " << p.second;
    return out;
}
template<class T1, class T2>
static real_t pair_product(const std::pair<T1, T2>& p)
{
    return (real_t)(p.first * p.second);
}
template<class T1, class T2>
static real_t pair_sum(const std::pair<T1, T2>& p)
{
    return (real_t)(p.first + p.second);
}
template<class T1, class T2>
static real_t pair_mean(const std::pair<T1, T2>& p)
{
    return pair_sum(p) / 2.0;
}
template<class T1, class T2>
static size_t difference(const std::pair<T1, T2>& p)
{
    return p.second - p.first;
}
// std::map OPERATIONS
template<class T1, class T2>
static bool in(const std::map<T1, T2>& a, const T1& b)
{
    return (a.find(b) != a.end());
}
template<class T1, class T2>
static const T2& at(
    const std::map<T1, T2>& a, const T1& b)
{
    typename std::map<T1, T2>::const_iterator it = a.find(b);
    check(it != a.end(), str(b) + " not found in std::map:\n" + str(a));
    return it->second;
}
template<class T1, class T2>
static void print_left(
    const std::map<T1, T2>& m, std::ostream& out = std::cout, const char delim = ' ')
{
    for (typename std::map<T1, T2>::const_iterator it = m.begin(); it != m.end();
         ++it) {
        out << it->first << delim;
    }
}
template<class T1, class T2>
static void print_right(
    const std::map<T1, T2>& m, std::ostream& out = std::cout, const char delim = ' ')
{
    for (typename std::map<T1, T2>::const_iterator it = m.begin(); it != m.end();
         ++it) {
        out << it->second << delim;
    }
}
template<class T1, class T2>
static std::ostream& operator<<(
    std::ostream& out, const std::map<T1, T2>& m)
{
    for (typename std::map<T1, T2>::const_iterator it = m.begin(); it != m.end();
         ++it) {
        out << *it << std::endl;
    }
    return out;
}
template<class T1, class T2>
static std::ostream& operator<<(
    std::ostream& out, const std::map<T1, T2*>& m)
{
    for (typename std::map<T1, T2*>::const_iterator it = m.begin(); it != m.end();
         ++it) {
        out << it->first << " " << *(it->second) << std::endl;
    }
    return out;
}
template<class T1, class T2>
static T2 sum_right(const std::map<T1, T2>& m)
{
    T2 ret = 0;
    for (typename std::map<T1, T2>::const_iterator it = m.begin(); it != m.end();
         ++it) {
        ret += it->second;
    }
    return ret;
}
template<class T1, class T2, class T3, class T4>
static void operator+=(
    std::map<T1, T2>& a, const std::map<T3, T4>& b)
{
    for (typename std::map<T3, T4>::const_iterator it = b.begin(); it != b.end();
         ++it) {
        a[it->first] += it->second;
    }
}
template<class T1, class T2, class T3, class T4>
static void operator-=(
    std::map<T1, T2>& a, const std::map<T3, T4>& b)
{
    for (typename std::map<T3, T4>::const_iterator it = b.begin(); it != b.end();
         ++it) {
        a[it->first] -= it->second;
    }
}
template<class T1, class T2, class T3, class T4>
static void operator/=(
    std::map<T1, T2>& a, const std::map<T3, T4>& b)
{
    for (typename std::map<T3, T4>::const_iterator it = b.begin(); it != b.end();
         ++it) {
        a[it->first] /= it->second;
    }
}
template<class T1, class T2, class T3, class T4>
static void operator*=(
    std::map<T1, T2>& a, const std::map<T3, T4>& b)
{
    for (typename std::map<T3, T4>::const_iterator it = b.begin(); it != b.end();
         ++it) {
        a[it->first] *= it->second;
    }
}
template<class T1, class T2, class T3>
static void operator*=(
    std::map<T1, T2>& a, const T3& b)
{
    for (typename std::map<T1, T2>::iterator it = a.begin(); it != a.end(); ++it) {
        it->second *= b;
    }
}
template<class T1, class T2, class T3>
static void operator/=(
    std::map<T1, T2>& a, const T3& b)
{
    for (typename std::map<T1, T2>::iterator it = a.begin(); it != a.end(); ++it) {
        it->second /= b;
    }
}
template<RangeLike R>
void delete_map(R& r)
{
    for (auto& [it_first, it_second] : make_subrange_for(r)) {
        delete it_second;
    }
}
// MULTIMAP OPERATIONS
template<class T1, class T2>
static bool in(
    const std::multimap<T1, T2>& a, const T1& b)
{
    return (a.find(b) != a.end());
}

// IO OPERATIONS
template<class T>
static void print(const T& t, std::ostream& out = std::cout)
{
    out << t << std::endl;
}
template<class T1, class T2>
static void print(
    const T1& t1, const T2& t2, std::ostream& out = std::cout)
{
    out << t1 << " " << t2 << std::endl;
}
template<class T1, class T2, class T3>
static void print(
    const T1& t1, const T2& t2, const T3& t3, std::ostream& out = std::cout)
{
    out << t1 << " " << t2 << " " << t3 << std::endl;
}
template<class T1, class T2, class T3, class T4>
static void print(
    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
    std::ostream& out = std::cout)
{
    out << t1 << " " << t2 << " " << t3 << " " << t4 << std::endl;
}
template<class T1, class T2, class T3, class T4, class T5>
static void print(
    const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
    std::ostream& out = std::cout)
{
    out << t1 << " " << t2 << " " << t3 << " " << t4 << " " << t5 << std::endl;
}
static void prt_line(std::ostream& out = std::cout)
{
    out << "------------------------------" << std::endl;
}
template<class T>
static T read(const std::string& data)
{
    T val;
    std::stringstream ss;
    ss << std::boolalpha << data;
    check(ss >> val, "cannot read string '" + data + "' into variable with type '" + typeid(T).name() + "'");
    return val;
}
// PROBABILITY FUNCTIONS
static real_t KL_normal(real_t pMean, real_t pVar, real_t qMean, real_t qVar)
{
    return 0.5 * (log(qVar / pVar) - 1 + ((squared(pMean - qMean) + pVar) / qVar));
}
// static real_t nats_to_bits(real_t nats) {
//   static real_t F = 1.0 / Log<real_t>::safe_log(2);
//   return F * nats;
// }
// static real_t bits_to_nats(real_t bits) {
//   static real_t F = Log<real_t>::safe_log(2);
//   return F * bits;
// }

#endif
