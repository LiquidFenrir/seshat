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

#include <seshat/hypothesis.hpp>

using namespace seshat;

#ifdef SESHAT_HYPOTHESIS_TREE
hypothesis::hypothesis(const hypothesis& other)
{
    *this = other;
}

hypothesis& hypothesis::operator=(const hypothesis& other)
{
    tree.clear();
    tokens = other.tokens;
    relations = other.relations;
    const auto tree_size = other.tree.size();
    tree.resize(tree_size);

    const std::span<relation> relations_span{ relations };
    for (std::size_t i = 0; i < tree_size; ++i) {
        const auto other_span = other.tree[i];
        // if the span is empty, ours need to be empty as well (it is)
        // don't even try pointer arithmetic
        if (other_span.empty())
            continue;

        tree[i] = relations_span.subspan(other_span.data() - other.relations.data(), other_span.size());
    }

    return *this;
}
#endif
