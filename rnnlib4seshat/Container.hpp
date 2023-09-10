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

#ifndef _INCLUDED_Container_h
#define _INCLUDED_Container_h

#include "Helpers.hpp"
#include <span>
#include <vector>

template<class T> struct Vector;

template<class T> struct View : public std::span<T> {
  View(const pair<T*, T*>& p) : std::span<T>(p.first, p.second) {}
  View(T* first, T* second) : std::span<T>(first, second) {}
  View() : std::span<T>() {}
  View slice(int first = 0, int last = numeric_limits<int>::max()) {
    first = bound(first, 0, (int)this->size());
    if (last < 0) {
      last += (int)this->size();
    }
    last = bound(last, first, (int)this->size());
    return View(&((*this)[first]), &((*this)[last]));
  }
  View slice(pair<int, int>& r) {
    return slice(r.first, r.second);
  }

  auto begin() const noexcept
  {
    return std::to_address(std::span<T>::begin());
  }
  auto cbegin() const noexcept
  {
    return std::to_address(std::span<T>::cbegin());
  }

  auto end() const noexcept
  {
    return std::to_address(std::span<T>::end());
  }
  auto cend() const noexcept
  {
    return std::to_address(std::span<T>::cend());
  }

  const View slice(int first = 0, int last = numeric_limits<int>::max()) const {
    return slice(first, last);
  }
  const View slice(pair<int, int>& r) const {
    return slice(r.first, r.second);
  }
  T& at(size_t i) {
    check(i < this->size(), "at(" + str(i) + ") called for view of size " +
          str(this->size()));
    return (*this)[i];
  }
  const T& at(size_t i) const {
    check(i < this->size(), "at(" + str(i) + ") called for view of size " +
          str(this->size()));
    return (*this)[i];
  }
  template<class R> const View<T>& operator =(const R& r) const {
    check(std::size(r) == this->size(), "can't assign range " + str(r) +
          " to view " + str(*this));
    copy(r, *this);
    return *this;
  }
  /*
  template<class T2> Vector<T2> to() const {
    Vector<T2> v;
    LOOP(const T& t, *this) {
      v += lexical_cast<T2>(t);
    }
    return v;
  }
  */
};

template<class T> struct Vector : public std::vector<T> {
  Vector() { }
  Vector(const vector<T>& v): vector<T>(v) {}
  Vector(const View<const T>& v) {
    *this = v;
  }
  Vector(size_t n): vector<T>(n) {}
  Vector(size_t n, const T& t): vector<T>(n, t) {}
  Vector<T>& grow(size_t length) {
    this->resize(this->size() + length);
    return *this;
  }
  Vector<T>& shrink(size_t length) {
    this->resize(max((size_t)0, this->size() - length));
    return *this;
  }
  void push_front(const T& t) {
    this->insert(this->begin(), t);
  }
  T& pop_front() {
    T& front = front();
    erase(this->begin());
    return front;
  }
  View<T> slice(int first = 0, int last = numeric_limits<int>::max()) {
    first = bound(first, 0, (int)this->size());
    if (last < 0) {
      last += (int)this->size();
    }
    last = bound(last, first, (int)this->size());
    return View<T>(&((*this)[first]), &((*this)[last]));
  }
  View<T> slice(pair<int, int>& r) {
    return slice(r.first, r.second);
  }
  const View<T> slice(
      int first = 0, int last = numeric_limits<int>::max()) const {
    return slice(first, last);
  }
  const View<T> slice(pair<int, int>& r) const {
    return slice(r.first, r.second);
  }
  template<class R> Vector<T>& extend(const R& r) {
    size_t oldSize = this->size();
    grow(std::size(r));
    std::ranges::copy(make_subrange_for(r), this->begin() + oldSize);
    return *this;
  }
  Vector<T> replicate(size_t times) const {
    Vector<T> v;
    REPEAT(times) {
      v.extend(*this);
    }
    return v;
  }
  template<class R> Vector<T>& operator =(const R& r) {
    vector<T>::resize(std::size(r));
    copy(r, *this);
    return *this;
  }
  template<class T2> Vector<T2> to() const {
    Vector<T2> v;
    LOOP(const T& t, *this) {
      v += lexical_cast<T2>(t);
    }
    return v;
  }
};

template<class T> struct Set: public set<T> {
  Set() {}
  Set(const vector<T>& v) {
    *this = v;
  }
  Set(const View<T>& v) {
    *this = v;
  }
  Set& operator +=(const T& val) {
    this->insert(val);
    return *this;
  }
  template<class R> Set<T>& operator =(const R& r) {
    this->clear();
    return this->extend(r);
  }
  template<class R> Set<T>& extend(const R& r) {
    LOOP(const auto& val, r) {
      (*this) += val;
    }
    return *this;
  }
};

#endif
