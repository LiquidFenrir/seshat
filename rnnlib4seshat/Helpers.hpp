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

#include <array>
#include <iterator>
#include <concepts>
#include <ranges>
#include <math.h>
#include <numeric>
#include <utility>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <set>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <map>
#include <random>
#include <cassert>
#include "Log.hpp"

using namespace std;

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

template <typename R>
auto make_subrange_for(R&& r) {
  if constexpr (PairLike<std::remove_cvref_t<R>>)
  {
    const auto p = r;
    return std::ranges::subrange(std::get<0>(p), std::get<1>(p));
  }
  else
    return std::views::all(std::forward<R>(r));
}

template<typename R>
using RangeValueType_t = std::remove_cvref_t<decltype(*std::begin(make_subrange_for(std::declval<R>())))>;

#define LOOP(x, y) for(x : make_subrange_for(y))
#define LOOP_BACK(x, y) for(x : make_subrange_for(y) | std::views::reverse)
#define REPEAT(n) for(int REPEAT_VARn = 0; REPEAT_VARn < (n); ++REPEAT_VARn)
#define FOR(i, n) for(int (i) = 0; (i) < (n); ++(i))

//#define FLOAT_REALS

#ifdef FLOAT_REALS
typedef float real_t;
#else
typedef double real_t;
#endif


typedef vector<size_t>::const_iterator VSTCI;
typedef vector<real_t>::iterator VDI;
typedef vector<real_t>::const_iterator VDCI;
typedef vector<real_t>::reverse_iterator VDRI;
typedef string::iterator SI;
typedef string::const_iterator SCI;
typedef vector<int>::iterator VII;
typedef vector<string>::iterator VSI;
typedef vector<string>::const_iterator VSCI;
typedef vector<int>::reverse_iterator VIRI;
typedef vector<vector<int> >::reverse_iterator VVIRI;
typedef vector<int>::const_iterator VICI;
typedef vector<bool>::iterator VBI;
typedef vector<real_t>::iterator VFI;
typedef vector<real_t>::const_iterator VFCI;
typedef vector<vector<real_t> >::iterator VVDI;
typedef vector<vector<real_t> >::const_iterator VVDCI;
typedef vector<vector<int> >::iterator VVII;
typedef vector<vector<int> >::const_iterator VVICI;
typedef vector<unsigned int>::iterator VUII;
typedef vector<vector<real_t> >::iterator VVFI;
typedef map<string, string>::iterator MSSI;
typedef map<string, string>::const_iterator MSSCI;
typedef map<string, real_t>::iterator MSDI;
typedef map<string, real_t>::const_iterator MSDCI;
typedef map<string, pair<int,real_t> >::iterator MSPIDI;
typedef map<string, pair<int,real_t> >::const_iterator MSPIDCI;
typedef vector< map<string, pair<int,real_t> > >::const_iterator VMSDCI;
typedef vector<map<string, pair<int,real_t> > >::iterator VMSDI;
typedef vector<map<string, pair<int,real_t> > >::reverse_iterator VMSDRI;
typedef map<string, int>::iterator MSII;
typedef map<string, int>::const_iterator MSICI;
typedef map<int, int>::iterator MIII;
typedef map<int, int>::const_iterator MIICI;
typedef vector<vector<int> >::const_reverse_iterator VVIRCI;
typedef vector<int>::const_reverse_iterator VIRCI;
typedef vector<const real_t*>::const_iterator VPCFCI;
typedef vector<const real_t*>::iterator VPCFI;
typedef vector<const real_t*>::const_reverse_iterator VPCFCRI;
typedef vector<bool>::const_iterator VBCI;
typedef vector<bool>::iterator VBI;
typedef map <string, pair<real_t, int> >::iterator MCSPDII;
typedef map <string, pair<real_t, int> >::const_iterator MCSPDICI;
typedef pair<string, real_t> PSD;
typedef pair<int, int> PII;
typedef pair<const string, real_t> PCSD;
typedef pair<string, int> PSI;
typedef pair<int, string> PIS;
typedef pair<string, string> PSS;
typedef const std::tuple<real_t&, real_t&, real_t&, real_t&>& TDDDD;
typedef const std::tuple<real_t&, real_t&, real_t&, real_t&, real_t&>& TDDDDD;
typedef const std::tuple<real_t&, real_t&, real_t&>& TDDD;
typedef const std::tuple<real_t&, real_t&, int&>& TDDI;
typedef const std::tuple<real_t&, real_t&, real_t&>& TDDF;
typedef const std::tuple<real_t&, real_t&, real_t>& TDDCF;
typedef const std::tuple<real_t&, real_t&>& TDD;
typedef const std::tuple<int, string>& TIS;
typedef const std::tuple<int, int>& TII;
typedef const std::tuple<int, real_t>& TID;
typedef const std::tuple<int, set<int>&>& TISETI;
typedef const std::tuple<int&, bool, int>& TIBI;
typedef const std::tuple<real_t, Log<real_t>& >& TDL;
typedef const std::tuple<real_t&, Log<real_t>, Log<real_t> >& TDLL;
typedef Log<real_t> prob_t;

//global variables
static const real_t realMax = numeric_limits<real_t>::max();
static const real_t realMin = numeric_limits<real_t>::min();
static const real_t infinity = numeric_limits<real_t>::infinity();
static bool runningGradTest = false;
static bool verbose = false;
static ostream& COUT = cout;

#define PRINT(x, o) ((o) << boolalpha << #x " = " << (x) << endl)
#define PRINTN(x, o)                                                    \
  (o) << boolalpha << #x ":" << endl;                                   \
  print_range((o), (x), string("\n")); (o) << endl
#define PRT(x) PRINT(x, cout)
#define PRTN(x) PRINTN(x, cout)
#define PRINTR(x, o, d)                                                 \
  (o) << boolalpha << #x " = "; print_range((o), (x), str(d)); (o) << endl
#define PRTR(x, d) PRINTR((x), cout, (d))
#define check(condition, str)                                           \
  if(!(condition)) {                                                    \
    cout << endl << "ERRROR: " << (str) << endl << endl;                \
    (assert((condition)));                                              \
  }
#define CHECK_STRICT(condition, str)                                    \
  if(!(condition)) {                                                    \
    cout << endl << "ERRROR: " << (str) << endl << endl;                \
    (assert((condition))); exit(0);                                     \
  }

//MISC FUNCTIONS
static bool warn_unless (
    bool condition, const string& str, ostream& out = cout) {
  if (!condition) {
    out << "WARNING: " << str << endl;
  }
  return condition;
}
static void print_time(
    real_t totalSeconds, ostream& out = cout, bool abbrv = false) {
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
static void mark() {
  static int num = 0;
  cout << "MARK " << num << endl;
  ++num;
}
template<class T> static T squared(const T& t) {
  return t*t;
}
template<class T> static int sign(const T& t) {
  if (t < 0) {
    return -1;
  } else if (t > 0) {
    return 1;
  } else {
    return 0;
  }
}


template <class T> static T bound (const T& v, const T& minVal, const T& maxVal) {
  return std::clamp(v, minVal, maxVal);
}

template <class C, class Tr, class R> static void print_range(
    basic_ostream<C, Tr>& out, const R& r,
    const basic_string<C, Tr>& delim = " ") {
  bool first = true;
  LOOP(const auto& cval, r) {
    if(first)
      first = false;
    else 
      out << delim;
    out << cval;
  }
}

template <class C, class Tr, class R> static basic_ostream<C, Tr>& operator <<(
    basic_ostream<C, Tr>& out, const R& r) {
  print_range(out, r);
  return out;
}

//CAST OPERATIONS
template<class T> static string str(const T& t) {
  stringstream ss;
  ss << t;
  return ss.str();
}

//GENERIC RANGE OPERATIONS
template <class R, class T> static auto find(R& r, const T& t) {
  return std::ranges::find(make_subrange_for(r), t);
}

template <class T1, class T2> static auto iota_range(T1 t1, T2 t2) {
  return std::ranges::iota_view<T2, T2>((t1 < t2 ? static_cast<T2>(t1) : t2), t2);
}

template <class T> static auto iota_range(T t) {
  return iota_range(0, t);
}

template <class R> static auto indices(const R& r) {
  return iota_range<int>(std::size(r));
}
template <class R1, class R2, class F>
static auto transform(const R1& r1, R2& r2, const F& f) {
  return std::ranges::transform(make_subrange_for(r1), std::begin(make_subrange_for(r2)), f).out;
}
template <class R> static bool in_range(R& r, size_t n) {
  return n >= 0 && n < std::size(r);
}
template <class R>
static auto& nth_last(R& r, size_t n = 1) {
  check(in_range(r, n-1), "nth_last called with n = " + str(n) +
        " for range of size " + (str(std::size(r))));
  return *(std::end(r) - n);
}
template <class R> static size_t last_index(const R& r) {
  return (std::size(r) - 1);
}
template <class R, class T> static bool in(const R& r, const T& t) {
  auto vr = make_subrange_for(r);
  return std::end(vr) != std::ranges::find(vr, t);
}
template <class R, class T> static size_t index(const R& r, const T& t) {
  auto vr = make_subrange_for(r);
  return std::distance(std::begin(vr), std::ranges::find(vr, t));
}
template <class R> static void reverse(R& r) {
  std::ranges::reverse(make_subrange_for(r));
}

template <class R>
static auto minmax(const R& r) {
  const auto [mini_val, maxi_val] = std::ranges::minmax(make_subrange_for(r));
  return std::pair<RangeValueType_t<R>, RangeValueType_t<R>>(mini_val, maxi_val);
}
template <class R> static void bound_range (R& r, const RangeValueType_t<R>& minVal, const RangeValueType_t<R>& maxVal) {
  LOOP(auto& vr, r) {
    vr = bound(vr, minVal, maxVal);
  }
}

template<class R> static void fill(R& r, const RangeValueType_t<R>& v) {
  std::ranges::fill(make_subrange_for(r), v);
}
template<typename T> static void flood(std::vector<T>& r, size_t size, const T& v = 0) {
  r.resize(size);
  fill(r, v);
}
template<class R> static size_t count(const R& r, const RangeValueType_t<R>& v) {
  return std::ranges::count(make_subrange_for(r), v);
}
template<class R1, class R2> static void copy(const R1& source, R2& dest) {
  auto a = make_subrange_for(source);
  auto b = make_subrange_for(dest);
  assert(std::ranges::size(a) >= std::ranges::size(b));
  std::ranges::copy(a, std::begin(b));
}

template<class R1, class R2> static bool equal(const R1& source, R2& dest) {
  return std::ranges::equal(make_subrange_for(source), make_subrange_for(dest));
}

static std::mt19937 shufflegen(std::time(nullptr));
template<class R> static R& shuffle (R& r) {
  std::ranges::shuffle(make_subrange_for(r), shufflegen);
  return r;
}
template <class R> static auto max(const R& r) {
  return std::ranges::max_element(make_subrange_for(r));
}

template <class C, class Tr, class R> static void print_range(
    basic_ostream<C, Tr>& out, const R& r, const char delim) {
  print_range(out, r, str(delim));
}

template <class C, class Tr, class R> static basic_istream<C, Tr>& operator >>(
    basic_istream<C, Tr>& in, R& r) {
  LOOP(auto& val, r) {
    in >> val;
  }
  return in;
}
template<class R> void delete_range(R& r) {
  LOOP(const auto it, r) {
    delete it;
  }
}

template<class R1, class R2, class R3> static size_t range_min_size (
    const R1& a, const R2& b, const R3& c) {
  return min(min(std::size(a), std::size(b)), std::size(c));
}

template <class R> static int arg_max(const R& r) {
  auto vr = make_subrange_for(r);
  return std::distance(std::begin(vr), std::ranges::max_element(vr));
}

template<class... Rs>
using zip = std::ranges::zip_view<Rs...>;

//ARITHMETIC RANGE OPERATIONS
template<class R1, class R2> static RangeValueType_t<R1> inner_product(const R1& a, const R2& b, RangeValueType_t<R1> c = 0) {
  auto va = make_subrange_for(a);
  auto vb = make_subrange_for(b);
  return std::inner_product(std::begin(va), std::end(va), std::begin(vb), c);
}
template <class R> static auto norm(const R& r) {
  return sqrt(inner_product(r, r));
}

template <class R> static auto product(const R& r) {
  return std::ranges::fold_left(make_subrange_for(r),
    RangeValueType_t<R>(1), [](const auto& x, const auto& y) {
      return x * y;
    });
}
template <class R> static auto sum(const R& r) {
  return std::ranges::fold_left(make_subrange_for(r),
    RangeValueType_t<R>(0), [](const auto& x, const auto& y) {
      return x + y;
    });
}
template <class R> static auto abs_sum(const R& r) {
  decltype(*std::begin(make_subrange_for(r))) v = 0;
  LOOP(const auto& vr, r) {
    v += abs(vr);
  }
  return v;
}

template <class R> static auto mean(const R& r) {
  return sum(r) / (decltype(*std::begin(make_subrange_for(r))))std::size(r);
}
template <class R> static auto variance(const R& r) {
  auto M = mean(r);
  decltype(M) v = 0;
  
  LOOP(const auto& vr, r) {
    v += squared(vr - M);
  }
  return v / std::size(r);
}
template <class R> static auto std_dev(const R& r) {
  return sqrt(variance(r));
}
//plus
template<class R1, class R2, class R3>
static R1& range_plus(R1& a, const R2& b, const R3& c) {
  std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
    return x + y;
  });
  return a;
}
template<class R1, class R2> static void range_plus_equals(R1& a, const R2& b) {
  range_plus(a, a, b);
}
//minus
template<class R1, class R2, class R3> static void range_minus(R1& a, const R2& b, const R3& c) {
  std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
    return x - y;
  });
}
//multiply
template<class R1, class R2> static void range_multiply_val(R1& a, const R2& b, const RangeValueType_t<R2>& c) {
  std::ranges::transform(make_subrange_for(b), std::begin(make_subrange_for(a)), [&c](const auto& x) {
    return x * c;
  });
}
template<class R> static void range_multiply_val(R& a, const RangeValueType_t<R>& b) {
  range_multiply_val(a, a, b);
}
template<class R1, class R2, class R3> static void range_multiply(R1& a, const R2& b, const R3& c) {
  std::ranges::transform(make_subrange_for(b), make_subrange_for(c), std::begin(make_subrange_for(a)), [](const auto& x, const auto& y) {
    return x * y;
  });
}

//divide
template<class R1, class R2> static void range_divide_val(R1& a, const R2& b, const RangeValueType_t<R1>& c) {
  std::ranges::transform(make_subrange_for(b), std::begin(make_subrange_for(a)), [&c](const auto& x) {
    return x / c;
  });
}
template<class R> static void range_divide_val(R& a, const RangeValueType_t<R>& b) {
  range_divide_val(a, a, b);
}


//PAIR OPERATIONS
template<class T> static bool in_open_interval(pair<T,T> interval, T val) {
  return ((val > interval.first) && (val < interval.second));
}
template<class T> static bool in_closed_interval(pair<T,T> interval, T val) {
  return ((val >= interval.first) && (val <= interval.second));
}
template<class T1, class T2> static void operator +=(
    pair<T1, T2>& a, const pair<T1, T2>& b) {
  a.first += b.first;
  a.second += b.second;
}
template<class T1, class T2, class T3> static pair<T1, T2> operator +(
    const pair<T1, T2>& a, const T3& b) {
  return make_pair(a.first + b, a.second + b);
}
template<class T1, class T2> static ostream& operator <<(
    ostream& out, const pair<T1, T2>& p) {
  out << p.first << " " << p.second;
  return out;
}
template<class T1, class T2> static real_t pair_product(const pair<T1, T2>& p) {
  return (real_t)(p.first * p.second);
}
template<class T1, class T2> static real_t pair_sum(const pair<T1, T2>& p) {
  return (real_t)(p.first + p.second);
}
template<class T1, class T2> static real_t pair_mean(const pair<T1, T2>& p) {
  return pair_sum(p)/2.0;
}
template <class T1, class T2> static size_t difference(const pair<T1,T2>& p)
{
  return p.second - p.first;
}
//MAP OPERATIONS
template<class T1, class T2> static bool in(const map<T1, T2>& a, const T1& b) {
  return (a.find(b) != a.end());
}
template<class T1, class T2> static const T2& at(
    const map<T1, T2>& a, const T1& b) {
  typename map<T1, T2>::const_iterator it = a.find(b);
  check(it != a.end(), str(b) + " not found in map:\n" + str(a));
  return it->second;
}
template<class T1, class T2> static void print_left(
    const map<T1, T2>& m, ostream& out = cout, const char delim = ' ') {
  for (typename map<T1, T2>::const_iterator it = m.begin(); it != m.end();
       ++it) {
    out << it->first << delim;
  }
}
template<class T1, class T2> static void print_right(
    const map<T1, T2>& m, ostream& out = cout, const char delim = ' ') {
  for (typename map<T1, T2>::const_iterator it = m.begin(); it != m.end();
       ++it) {
    out << it->second << delim;
  }
}
template<class T1, class T2> static ostream& operator <<(
    ostream& out, const map<T1, T2>& m) {
  for (typename map<T1, T2>::const_iterator it = m.begin(); it != m.end();
       ++it) {
    out << *it << endl;
  }
  return out;
}
template<class T1, class T2> static ostream& operator <<(
    ostream& out, const map<T1, T2*>& m) {
  for (typename map<T1, T2*>::const_iterator it = m.begin(); it != m.end();
       ++it) {
    out << it->first << " " << *(it->second) << endl;
  }
  return out;
}
template<class T1, class T2> static T2 sum_right(const map<T1, T2>& m) {
  T2 ret = 0;
  for (typename map<T1, T2>::const_iterator it = m.begin(); it != m.end();
       ++it) {
    ret += it->second;
  }
  return ret;
}
template<class T1, class T2, class T3, class T4> static void operator +=(
    map<T1, T2>& a, const map<T3, T4>& b) {
  for (typename map<T3, T4>::const_iterator it = b.begin(); it != b.end();
       ++it) {
    a[it->first] += it->second;
  }
}
template<class T1, class T2, class T3, class T4> static void operator -=(
    map<T1, T2>& a, const map<T3, T4>& b) {
  for (typename map<T3, T4>::const_iterator it = b.begin(); it != b.end();
       ++it) {
    a[it->first] -= it->second;
  }
}
template<class T1, class T2, class T3, class T4> static void operator /=(
    map<T1, T2>& a, const map<T3, T4>& b){
  for (typename map<T3, T4>::const_iterator it = b.begin(); it != b.end();
       ++it) {
    a[it->first] /= it->second;
  }
}
template<class T1, class T2, class T3, class T4> static void operator *=(
    map<T1, T2>& a, const map<T3, T4>& b) {
  for (typename map<T3, T4>::const_iterator it = b.begin(); it != b.end();
       ++it) {
    a[it->first] *= it->second;
  }
}
template<class T1, class T2, class T3> static void operator *=(
    map<T1, T2>& a, const T3& b) {
  for (typename map<T1, T2>::iterator it = a.begin(); it != a.end(); ++it) {
    it->second *= b;
  }
}
template<class T1, class T2, class T3> static void operator /=(
    map<T1, T2>& a, const T3& b) {
  for (typename map<T1, T2>::iterator it = a.begin(); it != a.end(); ++it) {
    it->second /= b;
  }
}
template<class R> void delete_map(R& r) {
  for (auto it = std::begin(r); it != std::end(r); ++it) {
    delete it->second;
  }
}
//MULTIMAP OPERATIONS
template<class T1, class T2> static bool in(
    const multimap<T1, T2>& a, const T1& b) {
  return (a.find(b) != a.end());
}

//IO OPERATIONS
template<class T> static void print(const T& t, ostream& out = cout) {
  out << t << endl;
}
template<class T1, class T2> static void print(
    const T1& t1, const T2& t2, ostream& out = cout) {
  out << t1 << " " << t2 << endl;
}
template<class T1, class T2, class T3> static void print(
    const T1& t1, const T2& t2, const T3& t3, ostream& out = cout) {
  out << t1 << " " << t2 << " " << t3 << endl;
}
template<class T1, class T2, class T3, class T4> static void print(
    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
    ostream& out = cout) {
  out << t1 << " " << t2 << " " << t3  << " " << t4 << endl;
}
template<class T1, class T2, class T3, class T4, class T5> static void print(
    const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5,
    ostream& out = cout) {
  out << t1 << " " << t2 << " " << t3  << " " << t4 << " " << t5 << endl;
}
static void prt_line(ostream& out = cout) {
  out << "------------------------------" << endl;
}
template<class T> static T read(const string& data) {
  T val;
  stringstream ss;
  ss << boolalpha << data;
  check(ss >> val, "cannot read string '" + data +
        "' into variable with type '" + typeid(T).name() + "'");
  return val;
}
//PROBABILITY FUNCTIONS
static real_t KL_normal(real_t pMean, real_t pVar, real_t qMean, real_t qVar) {
  return 0.5 * (log(qVar/pVar) - 1 + ((squared(pMean - qMean) + pVar) / qVar));
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
