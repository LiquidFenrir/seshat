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

#ifndef _INCLUDED_MultilayerNet_h
#define _INCLUDED_MultilayerNet_h

#include "ClassificationLayer.hpp"
#include "Mdrnn.hpp"

struct MultilayerNet : public Mdrnn {
    // functions
    MultilayerNet(std::ostream& out, ConfigFile& conf, const DataHeader& data, WeightContainer* weight, DataExportHandler* deh)
        : Mdrnn(out, conf, data, weight, deh)
    {
        std::string task = conf.get<std::string>("task");
        std::vector<int> hiddenSizes = conf.get_list<int>("hiddenSize");
        assert(hiddenSizes.size());
        Vector<std::string> hiddenTypes = conf.get_list<std::string>("hiddenType", "lstm", hiddenSizes.size());
        Vector<Vector<size_t>> hiddenBlocks = conf.get_array<size_t>("hiddenBlock");
        assert(hiddenBlocks.size() < hiddenSizes.size());
        Vector<int> subsampleSizes = conf.get_list<int>("subsampleSize");
        assert(subsampleSizes.size() < hiddenSizes.size());
        std::string subsampleType = conf.get<std::string>("subsampleType", "tanh");
        bool subsampleBias = conf.get<bool>("subsampleBias", false);
        Vector<bool> recurrent = conf.get_list<bool>("recurrent", true, hiddenSizes.size());
        Layer* input = this->get_input_layer();
        std::vector<Layer*> blocks;
        LOOP(int i, indices(hiddenSizes))
        {
            std::string level_suffix = int_to_sortable_string(i, hiddenSizes.size());
            this->add_hidden_level(
                hiddenTypes.at(i), hiddenSizes.at(i), recurrent.at(i),
                "hidden_" + level_suffix);
            this->connect_to_hidden_level(input, i);

            blocks.clear();
            if (i < hiddenBlocks.size()) {
                LOOP(Layer * l, hiddenLevels[i])
                {
                    blocks.push_back(this->add_layer(new BlockLayer(l, hiddenBlocks.at(i), wc, deh)));
                }
            }

            std::vector<Layer*>& topLayers = blocks.empty() ? hiddenLevels[i] : blocks;
            if (i < subsampleSizes.size()) {
                input = this->add_layer(
                    subsampleType, "subsample_" + level_suffix, subsampleSizes.at(i),
                    std::vector<int>(this->num_seq_dims(), 1),
                    subsampleBias, false);
                LOOP(Layer * l, topLayers)
                {
                    this->connect_layers(l, input);
                }
            } else if (i < last_index(hiddenSizes)) {
                input = this->add_layer(new GatherLayer(
                    "gather_" + level_suffix, topLayers, wc, deh));
            }
        }
        conf.set_val("inputSize", inputLayer->output_size());
        if (data.targetLabels.size()) {
            std::string labelDelimiters(",.;:|+&_~*%$#^=-<>/?{}[]()");
            LOOP(char c, labelDelimiters)
            {
                bool goodDelim = true;
                LOOP(const std::string& s, data.targetLabels)
                {
                    if (in(s, c)) {
                        goodDelim = false;
                        break;
                    }
                }
                if (goodDelim) {
                    std::stringstream ss;
                    print_range(ss, data.targetLabels, c);
                    conf.set_val("targetLabels", ss.str());
                    conf.set_val("labelDelimiter", c);
                    break;
                }
            }
        }
        std::string outputName = "output";
        Layer* output = 0;
        size_t outSeqDims = (in(task, "sequence_") ? 0 : num_seq_dims());
        if (in(task, "classification")) {
            output = add_output_layer(make_classification_layer(
                out, outputName, outSeqDims, data.targetLabels, wc, deh));
        } else {
            check(false, "unknown task '" + task + "'");
        }
        if (this->num_seq_dims() && in(task, "sequence_")) {
            output = this->collapse_layer(hiddenLayers.back(), output);
        }
        connect_from_hidden_level(last_index(hiddenLevels), output);
    }
};

#endif
