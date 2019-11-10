//  rnndescent -- Nearest Neighbor Descent method for approximate nearest
//  neighbors
//
//  Copyright (C) 2019 James Melville
//
//  This file is part of rnndescent
//
//  rnndescent is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  rnndescent is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with rnndescent.  If not, see <http://www.gnu.org/licenses/>.

#ifndef RNN_RNG_H
#define RNN_RNG_H

#include "tauprng.h"

void set_seed();
uint64_t random64();

// Use R API for RNG
struct RRand {
  // a random uniform value between 0 and 1
  double unif();
};

// Use Taus88 RNG
struct TauRand {
  tau_prng prng;
  TauRand();
  TauRand(uint64_t state0, uint64_t state1, uint64_t state2);
  // a random uniform value between 0 and 1
  double unif();
};

#endif // RNN_RNG_H