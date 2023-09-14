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

#include <chrono>
#include <random>
#include <rnnlib4seshat/Random.hpp>

typedef std::mt19937 BaseGenType;
static BaseGenType generator(42u);
static std::uniform_real_distribution<real_t> uni_distrib;
static std::normal_distribution<real_t> norm_distrib;

real_t Random::normal()
{
    return norm_distrib(generator);
}
real_t Random::normal(const real_t dev, const real_t mean)
{
    return (normal() * dev) + mean;
}
unsigned int Random::set_seed(unsigned int seed)
{
    if (seed == 0) {
        const auto p = std::chrono::system_clock::now();
        seed = (unsigned int)(p.time_since_epoch().count());
    }
    srand(seed);
    generator.seed(seed);
    return seed;
}
real_t Random::uniform(const real_t range)
{
    return (uniform() * 2 * range) - range;
}
real_t Random::uniform()
{
    return uni_distrib(generator);
}
