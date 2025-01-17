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

#ifndef _INCLUDED_MultiArray_h
#define _INCLUDED_MultiArray_h

#include "Container.hpp"
#include "Helpers.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <optional>
#include <ranges>
#include <span>
#include <vector>

template<class T>
struct MultiArray {
    // data
    Vector<T> data;
    Vector<size_t> shape;
    std::vector<size_t> strides;

    // functions
    MultiArray() = default;
    MultiArray(const std::vector<size_t>& s)
    {
        reshape(s);
    }
    MultiArray(const std::vector<size_t>& s, const T& fillval)
    {
        reshape(s, fillval);
    }
    virtual ~MultiArray() = default;
    virtual size_t size() const
    {
        return data.size();
    }
    virtual size_t num_dims() const
    {
        return shape.size();
    }
    virtual bool empty() const
    {
        return data.empty();
    }
    virtual void resize_data()
    {
        data.resize(product(shape));
        strides.resize(shape.size());
        strides.back() = 1;
        for (int i = shape.size() - 2; i >= 0; --i) {
            strides.at(i) = strides.at(i + 1) * shape.at(i + 1);
        }
    }
    template<class R>
    void reshape(const R& newShape)
    {
        assert(newShape.size());
        shape = newShape;
        resize_data();
    }
    void fill_data(const T& fillVal)
    {
        fill(data, fillVal);
    }
    template<class R>
    void reshape(const R& dims, const T& fillVal)
    {
        reshape(dims);
        fill_data(fillVal);
    }
    template<RangeLike R = std::span<int>>
    bool in_range(const R& coords) const
    {
        if (std::size(coords) > shape.size()) {
            return false;
        }
        VSTCI shapeIt = shape.begin();
        for (auto coordIt = std::begin(coords); coordIt != std::end(coords);
             ++coordIt, ++shapeIt) {
            int c = *coordIt;
            if (c < 0 || c >= *shapeIt) {
                return false;
            }
        }
        return true;
    }
    template<RangeLike R = std::span<int>>
    T& get(const R& coords)
    {
        check(std::size(coords) == shape.size(), "get(" + str(coords) + ") called with shape " + str(shape));
        return *((*this)[coords].begin());
    }
    template<RangeLike R = std::span<int>>
    const T& get(const R& coords) const
    {
        check(std::size(coords) == shape.size(), "get(" + str(coords) + ") called with shape " + str(shape));
        return (*this)[coords].front();
    }
    template<RangeLike R = std::span<int>>
    size_t offset(const R& coords) const
    {
        return inner_product(coords, strides);
    }
    template<RangeLike R = std::span<int>>
    const View<T> operator[](const R& coords)
    {
        check(std::size(coords) <= shape.size(), "operator [" + str(coords) + "] called with shape " + str(shape));
        if (std::empty(coords)) {
            return View<T>(&data.front(), &data.front() + data.size());
        }
        T* start = &data.front() + offset(coords);
        T* end = start + strides[std::size(coords) - 1];
        return View<T>(start, end);
    }
    template<RangeLike R = std::span<int>>
    const View<const T> operator[](const R& coords) const
    {
        check(std::size(coords) <= shape.size(), "operator [" + str(coords) + "] called with shape " + str(shape));
        if (std::empty(coords)) {
            return View<const T>(&data.front(), &data.front() + data.size());
        }
        const T* start = &data.front();
        VSTCI strideIt = strides.begin() + offset(coords);
        const T* end = start + strides[std::size(coords) - 1];
        return View<const T>(start, end);
    }
    template<RangeLike R = std::span<int>>
    const View<T> at(const R& coords)
    {
        if (in_range(coords)) {
            return (*this)[coords];
        }
        return View<T>(0, 0);
    }
    template<RangeLike R = std::span<int>>
    const View<const T> at(const R& coords) const
    {
        if (in_range(coords)) {
            return (*this)[coords];
        }
        return View<const T>(0, 0);
    }
    template<class T2>
    void assign(const MultiArray<T2>& a)
    {
        reshape(a.shape);
        copy(a.data, data);
    }
    template<class T2>
    MultiArray<T>& operator=(const MultiArray<T2>& a)
    {
        assign(a);
        return *this;
    }
};

template<class T>
static bool operator==(const MultiArray<T>& a, const MultiArray<T>& b)
{
    return (a.data == b.data && a.shape == b.shape);
}

#endif
