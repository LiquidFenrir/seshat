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
*/

#include <meparser.hpp>
#include <samples.hpp>
#include <seshat/seshat.hpp>

namespace seshat {

math_expression::math_expression(const char* config_path)
    : parser{ std::make_unique<meParser>(config_path) }
    , samples{ std::make_unique<Samples>() }
{
}

math_expression::~math_expression() = default;

std::vector<hypothesis> math_expression::parse_sample(const sample& input)
{
    samples->clearAll();

    for (const auto& input_stroke : input.strokes) {
        auto& output_stroke = samples->dataon.emplace_back();
        output_stroke.pseq.reserve(input_stroke.points.size());

        for (const auto& input_point : input_stroke.points) {
            output_stroke.pseq.push_back(input_point);
            if (input_point.x < output_stroke.rx)
                output_stroke.rx = input_point.x;
            if (input_point.y < output_stroke.ry)
                output_stroke.ry = input_point.y;
            if (input_point.x > output_stroke.rs)
                output_stroke.rs = input_point.x;
            if (input_point.y > output_stroke.rt)
                output_stroke.rt = input_point.y;
        }
    }

    samples->makeReady();

    return parser->parse_me(*samples);
}

}
