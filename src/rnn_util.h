//  rnndescent -- An R package for nearest neighbor descent
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

#ifndef RNN_UTIL_H
#define RNN_UTIL_H

#include <limits>

#include <Rcpp.h>

#include "tdoann/nngraph.h"

void print_time(bool print_date = false);
void ts(const std::string &);
void zero_index(Rcpp::IntegerMatrix,
                int max_idx = (std::numeric_limits<int>::max)());
auto graph_to_r(const tdoann::NNGraph &) -> Rcpp::List;

#endif // RNN_UTIL_H
