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

#ifndef _INCLUDED_DataSequence_h
#define _INCLUDED_DataSequence_h

#include "Helpers.hpp"
#include "RealType.hpp"
#include "SeqBuffer.hpp"
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template<class R>
static std::string label_seq_to_str(const R& labelSeq, const std::vector<std::string>& alphabet, const std::string& delim = " ")
{
    std::stringstream ss;
    bool first = true;
    LOOP(const auto& it, labelSeq)
    {
        if (first)
            first = false;
        else
            ss << delim;

        if (in_range(alphabet, it)) {
            ss << alphabet[it];
        } else {
            ss << "<NULL>";
        }
    }
    return ss.str();
}
static std::vector<int> str_to_label_seq(const std::string& labelSeqString, const std::vector<std::string>& alphabet)
{
    static std::vector<int> v;
    v.clear();
    std::stringstream ss(labelSeqString);
    std::string lab;
    while (ss >> lab) {
        /*		check(in_right(alphabet, lab), lab + " not found in alphabet");*/
        //		if (warn_unless(in_right(alphabet, lab), lab + " not found in alphabet"))
        int i = index(alphabet, lab);
        if (i != alphabet.size()) {
            v.push_back(i);
        }
    }
    return v;
}

struct DataSequence {
    // data
    SeqBuffer<real_t> inputs;
    SeqBuffer<int> inputClasses;
    SeqBuffer<real_t> targetPatterns;
    SeqBuffer<int> targetClasses;
    SeqBuffer<real_t> importance;
    std::vector<int> targetLabelSeq;
    std::string tag;

    // functions
    DataSequence(const DataSequence& ds)
        : inputs(ds.inputs)
        , inputClasses(ds.inputClasses)
        , targetPatterns(ds.targetPatterns)
        , targetClasses(ds.targetClasses)
        , importance(ds.importance)
        , targetLabelSeq(ds.targetLabelSeq)
        , tag(ds.tag)
    {
    }
    DataSequence(size_t inputDepth = 0, size_t targetPattDepth = 0)
        : inputs(inputDepth)
        , inputClasses(0)
        , targetPatterns(targetPattDepth)
        , targetClasses(0)
        , importance(0)
    {
    }
    DataSequence& operator=(const DataSequence& ds)
    {
        inputs = ds.inputs;
        inputClasses = ds.inputClasses;
        targetPatterns = ds.targetPatterns;
        targetClasses = ds.targetClasses;
        importance = ds.importance;
        targetLabelSeq = ds.targetLabelSeq;
        tag = ds.tag;
        return *this;
    }

    size_t num_timesteps() const
    {
        return inputs.seq_size();
    }
    void print(std::ostream& out, std::vector<std::string>* targetLabels = 0, std::vector<std::string>* inputLabels = 0) const
    {
        PRINT(tag, out);
        out << "input shape = (" << inputs.shape << ")" << std::endl;
        out << "timesteps = " << inputs.seq_size() << std::endl;
        if (targetLabelSeq.size() && targetLabels) {
            out << "target label sequence:" << std::endl;
            out << label_seq_to_str(this->targetLabelSeq, *targetLabels) << std::endl;
        }
        if (targetPatterns.size()) {
            out << "target shape = (" << targetPatterns.shape << ")" << std::endl;
        }
        if (verbose) {
            if (targetClasses.size() && targetLabels) {
                out << label_seq_to_str(this->targetClasses.data, *targetLabels) << std::endl;
            }
            if (inputClasses.size() && inputLabels) {
                out << label_seq_to_str(this->inputClasses.data, *inputLabels) << std::endl;
            }
        }
    }
};
static std::ostream& operator<<(std::ostream& out, const DataSequence& seq)
{
    seq.print(out);
    return out;
}

#endif
