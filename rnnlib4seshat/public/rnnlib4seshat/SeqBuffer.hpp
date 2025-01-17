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

#ifndef _INCLUDED_SeqBuffer_h
#define _INCLUDED_SeqBuffer_h

#include "MultiArray.hpp"
#include "RealType.hpp"

template<class R>
struct CoordIterator {
    // data
    Vector<size_t> shape;
    std::vector<int> directions;
    std::vector<int> pt;
    bool end;

    // functions
    CoordIterator(
        const R& sh, const std::vector<int>& dirs = {},
        bool reverse = false)
        : shape(sh)
        , directions(dirs)
        , pt(std::size(shape))
        , end(false)
    {
        directions.resize(shape.size(), 1);
        if (reverse) {
            range_multiply_val(directions, -1);
        }
        begin();
    }

    void step(size_t d)
    {
        if (directions[d] > 0) {
            if (pt[d] == shape[d] - 1) {
                pt[d] = 0;
                if (d) {
                    step(d - 1);
                } else {
                    end = true;
                }
            } else {
                ++pt[d];
            }
        } else {
            if (pt[d] == 0) {
                pt[d] = shape[d] - 1;
                if (d) {
                    step(d - 1);
                } else {
                    end = true;
                }
            } else {
                --pt[d];
            }
        }
    }

    CoordIterator& operator++()
    {
        if (shape.size()) {
            step(shape.size() - 1);
        } else {
            end = true;
        }
        return *this;
    }

    const std::vector<int>& operator*() const
    {
        return pt;
    }

    void begin()
    {
        LOOP(int i, indices(shape))
        {
            pt[i] = ((directions[i] > 0) ? 0 : shape[i] - 1);
        }
        end = false;
    }
};

#define CVI CoordIterator<const std::vector<size_t>>
#define SeqIterator CoordIterator<View<const size_t>>
template<class T>
struct SeqBuffer : public MultiArray<T> {
    // data
    size_t depth;

    // functions
    explicit SeqBuffer(size_t dep = 0)
        : depth(dep)
    {
        reshape(std::vector<size_t>{});
    }

    SeqBuffer(const std::vector<size_t>& shape, size_t dep)
        : depth(dep)
    {
        reshape(shape);
    }

    template<class T2>
    SeqBuffer<T>& operator=(const SeqBuffer<T2>& a)
    {
        depth = a.depth;
        MultiArray<T>::assign(a);
        return *this;
    }

    template<class T2>
    SeqBuffer(const SeqBuffer<T2>& sb)
    {
        SeqBuffer<T>::operator= <T2>(sb);
    }

    virtual ~SeqBuffer() = default;

    const View<const T> operator[](int coord) const
    {
        check(this->shape.size(), "operator [" + str(coord) + "] called for empty array");
        const T* start = &this->data.front() + (coord * this->shape.back());
        const T* end = start + this->shape.back();
        return View<const T>(start, end);
    }

    template<RangeLike R = std::span<int>>
    int seq_offset(const R& coords)
    {
        return this->offset(coords) / this->shape.back();
    }

    using MultiArray<T>::operator[];
    using MultiArray<T>::at;

    const View<T> operator[](int coord)
    {
        check(this->shape.size(), "operator [" + str(coord) + "] called for empty array");
        T* start = &this->data.front() + (coord * this->shape.back());
        T* end = start + this->shape.back();
        return View<T>(start, end);
    }

    const View<T> at(int coord)
    {
        if ((coord >= 0) && ((coord * this->shape.back()) < product(this->shape))) {
            return (*this)[coord];
        }
        return View<T>();
    }

    const View<const T> at(int coord) const
    {
        if ((coord >= 0) && ((coord * this->shape.back()) < product(this->shape))) {
            return (*this)[coord];
        }
        return View<const T>();
    }

    const View<T> front(const std::vector<int>& dirs = {})
    {
        return (*this)[*begin(dirs)];
    }

    const View<T> back(const std::vector<int>& dirs = {})
    {
        return (*this)[*rbegin(dirs)];
    }

    SeqIterator begin(const std::vector<int>& dirs = {}) const
    {
        return SeqIterator(seq_shape(), dirs);
    }

    SeqIterator rbegin(const std::vector<int>& dirs = {}) const
    {
        return SeqIterator(seq_shape(), dirs, true);
    }

    const View<const size_t> seq_shape() const
    {
        if (this->shape.empty()) {
            return View<const size_t>();
        }
        return View<const size_t>(&this->shape.front(), &this->shape.back());
    }

    std::vector<real_t>& seq_means() const
    {
        static std::vector<real_t> seqMean;
        seqMean.resize(depth);
        fill(seqMean, 0);
        int seqSize = seq_size();
        LOOP(int i, iota_range(seqSize))
        {
            range_plus_equals(seqMean, (*this)[i]);
        }
        range_divide_val(seqMean, seqSize);
        return seqMean;
    }

    size_t seq_size() const
    {
        return product(seq_shape());
    }

    size_t num_seq_dims() const
    {
        return this->shape.size() - 1;
    }

    template<class T2>
    void reshape(const SeqBuffer<T2>& buff)
    {
        reshape(buff.seq_shape());
    }

    template<class T2>
    void reshape(const SeqBuffer<T2>& buff, const T& fillVal)
    {
        reshape(buff.seq_shape(), fillVal);
    }

    template<class R>
    void reshape(const R& newSeqShape)
    {
        if (depth) {
            // check(!in(newSeqShape, 0), "reshape called with shape " +
            //       str(newSeqShape) + ", all dimensions must be >0");
            this->shape = newSeqShape;
            this->shape.push_back(depth);
            this->resize_data();
        }
    }

    template<class R>
    void reshape(const R& newSeqShap, const T& fillval)
    {
        reshape(newSeqShap);
        this->fill_data(fillval);
    }

    template<class R>
    void reshape_with_depth(const R& newSeqShap, size_t dep)
    {
        depth = dep;
        reshape(newSeqShap);
    }

    template<class R>
    void reshape_with_depth(
        const R& newSeqShap, size_t dep, const T& fillval)
    {
        reshape_with_depth(newSeqShap, dep);
        this->fill_data(fillval);
    }

    void print(std::ostream& out) const
    {
        // out << "DIMENSIONS: " << seq_shape() << std::endl;
        LOOP(int j, iota_range(this->shape.back()))
        {
            LOOP(int i, iota_range(seq_size()))
            {
                out << (*this)[i][j] << " ";
            }
            out << std::endl;
        }
    }
};

template<class T>
static std::ostream& operator<<(
    std::ostream& out, const SeqBuffer<T>& a)
{
    a.print(out);
    return out;
}

#endif
