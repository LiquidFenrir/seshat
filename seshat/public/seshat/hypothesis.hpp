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
#ifndef SESHAT_PUBLIC_INTERFACE_HYPOTHESIS
#define SESHAT_PUBLIC_INTERFACE_HYPOTHESIS

#include <span>
#include <string>
#include <vector>

namespace seshat {

struct hypothesis {
    struct token {
        std::string data;
    };
    // parent_id == child_id -> invalid
    struct relation {
        std::size_t parent_id{ 0 };
        std::size_t child_id{ 0 };
    };
    std::vector<token> tokens;
    // ids point into `tokens`
    std::vector<relation> relations;
    // spans point into `relations`
    std::vector<std::span<relation>> tree;

    hypothesis() = default;
    ~hypothesis() = default;

    hypothesis(hypothesis&&) = default;
    hypothesis& operator=(hypothesis&&) = default;
    // this container is trivially move-safe, but not copy-safe.
    hypothesis(const hypothesis&);
    hypothesis& operator=(const hypothesis&);

    /* Example representation: exp.scgink
     * Infix: (x + y)^2
     *
     * AST:
     * --> ^
     *   |
     *   +--> ()
     *   |  |
     *   |  .--> +
     *   |     |
     *   |     +--> x
     *   |     |
     *   |     .--> y
     *   |
     *   .--> 2
     *
     * Tokens:
     * id:   0, 1,  2, 3, 4, 5
     * data: ^, (), +, x, y, 2
     *
     * Relations:
     * id:   0,   1,   2,   3,   4
     * pair: 0-1, 0-5, 1-2, 2-3, 2-4
     *
     * Tree (maps id of a parent to an array of relations with id to children):
     * id 0, root, ^: {0...1} (2 children)
     * id 1, (): {2} (1 child)
     * id 2, +: {3...4} (2 children)
     * id 3, x {} (no children, leaf)
     * id 4, y {} (no children, leaf)
     * id 5, 2 {} (no children, leaf)
     */
};

}

#endif
