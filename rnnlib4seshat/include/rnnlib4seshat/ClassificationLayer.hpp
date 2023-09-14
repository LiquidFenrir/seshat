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

#ifndef _INCLUDED_ClassificationLayer_h
#define _INCLUDED_ClassificationLayer_h

#include "ActivationFunctions.hpp"
#include "NeuronLayer.hpp"
#include "SoftmaxLayer.hpp"
#include <initializer_list>

struct ClassificationLayer : public NetworkOutput {
    // data
    std::ostream& out;
    Vector<std::string> labels;
    SeqBuffer<int> targets;
    std::vector<std::vector<int>> confusionMatrix;
    std::vector<int> numErrorsByClass;
    std::vector<int> numTargetsByClass;
    std::vector<int> outputs;

    // functions
    ClassificationLayer(std::ostream& o, const std::vector<std::string>& labs)
        : out(o)
        , labels(labs)
        , targets(labels.size())
        , confusionMatrix(labels.size())
        , numErrorsByClass(labels.size())
        , numTargetsByClass(labels.size())
    {
        LOOP(std::vector<int> & v, confusionMatrix)
        {
            v.resize(labels.size());
        }
        criteria = std::vector<std::string>{ "crossEntropyError", "classificationError" };
    }
    virtual int output_class(int pt) const = 0;
    virtual real_t class_prob(int pt, int index) const = 0;
    virtual real_t set_error(int pt, int targetClass) = 0;
    real_t calculate_errors(const DataSequence& seq)
    {
        LOOP(std::vector<int> & v, confusionMatrix)
        {
            fill(v, 0);
        }
        outputs.clear();
        targets.reshape(seq.targetClasses.seq_shape(), 0);
        real_t crossEntropyError = 0;
        LOOP(int pt, iota_range(seq.targetClasses.seq_size()))
        {
            int outputClass = output_class(pt);
            outputs.push_back(outputClass);
            int targetClass = seq.targetClasses[pt].front();
            if (targetClass >= 0) {
                View<int> targs = targets[pt];
                targs[targetClass] = 1;
                crossEntropyError -= set_error(pt, targetClass);
                ++confusionMatrix[targetClass][outputClass];
            }
        }
        errorMap.clear();
        LOOP(int i, indices(confusionMatrix))
        {
            std::vector<int>& v = confusionMatrix[i];
            numTargetsByClass[i] = sum(v);
            numErrorsByClass[i] = numTargetsByClass[i] - v[i];
        }
        real_t numTargets = sum(numTargetsByClass);
        if (numTargets) {
            errorMap["crossEntropyError"] = crossEntropyError;
            errorMap["classificationError"] = sum(numErrorsByClass) / numTargets;
            LOOP(int i, indices(confusionMatrix))
            {
                if (numTargetsByClass[i]) {
                    errorMap["_" + labels[i]] = numErrorsByClass[i] / numTargets;
                    if (verbose && (confusionMatrix.size() > 2)) {
                        std::vector<int>& v = confusionMatrix[i];
                        LOOP(int j, indices(v))
                        {
                            if (j != i && v[j]) {
                                errorMap["_" + labels[i] + "->" + labels[j]] = v[j] / numTargets;
                            }
                        }
                    }
                }
            }
        }
        return crossEntropyError;
    }
};

struct MulticlassClassificationLayer : public ClassificationLayer, public SoftmaxLayer {
    // functions
    MulticlassClassificationLayer(std::ostream& out, const std::string& name, size_t numSeqDims, const std::vector<std::string>& labels, WeightContainer* wc, DataExportHandler* deh)
        : ClassificationLayer(out, labels)
        , SoftmaxLayer(name, numSeqDims, labels, wc, deh)
    {
        // display(targets, "targets", labels);
    }
    int output_class(int pt) const
    {
        return arg_max(outputActivations[pt]);
    }
    real_t class_prob(int pt, int index) const
    {
        return std::max(realMin, outputActivations[pt][index]);
    }
    real_t set_error(int pt, int targetClass)
    {
        real_t targetProb = class_prob(pt, targetClass);
        View<real_t> errs = outputErrors[pt];
        errs[targetClass] = -(1 / targetProb);
        return log(targetProb);
    }
};

struct BinaryClassificationLayer : public ClassificationLayer, public NeuronLayer<Logistic> {
    BinaryClassificationLayer(std::ostream& out, const std::string& name, size_t numSeqDims, const std::vector<std::string>& labels, WeightContainer* weight, DataExportHandler* deh)
        : ClassificationLayer(out, labels)
        , NeuronLayer<Logistic>(name, numSeqDims, 1, weight, deh)
    {
        // display(targets, "targets", labels);
    }
    int output_class(int pt) const
    {
        return (outputActivations[pt][0] > 0.5 ? 1 : 0);
    }
    real_t class_prob(int pt, int index) const
    {
        real_t act = std::max(realMin, outputActivations[pt][0]);
        return (index == 1 ? act : 1 - act);
    }
    real_t set_error(int pt, int targetClass)
    {
        real_t targetProb = class_prob(pt, targetClass);
        View<real_t> tmp = static_cast<View<real_t>>(outputErrors[pt]);
        tmp[0] = (targetClass ? -(1 / targetProb) : (1 / targetProb));
        return log(targetProb);
    }
};

ClassificationLayer* make_classification_layer(std::ostream& out, const std::string& name, size_t numSeqDims, const std::vector<std::string>& labels, WeightContainer* weight, DataExportHandler* deh);

#endif
