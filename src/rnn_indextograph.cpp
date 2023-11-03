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

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,modernize-use-trailing-return-type,readability-magic-numbers)

#include <Rcpp.h>

#include "tdoann/distancebase.h"
#include "tdoann/nngraph.h"

#include "rnn_distance.h"
#include "rnn_parallel.h"
#include "rnn_progress.h"
#include "rnn_rtoheap.h"

using Rcpp::IntegerMatrix;
using Rcpp::IntegerVector;
using Rcpp::List;
using Rcpp::NumericMatrix;
using Rcpp::NumericVector;

template <typename Out, typename Idx>
auto idx_to_graph_impl(const tdoann::BaseDistance<Out, Idx> &distance,
                       const IntegerMatrix &idx, std::size_t n_threads = 0,
                       bool verbose = false) -> List {
  auto idx_vec = r_to_idxt<Idx>(idx);
  RPProgress progress(verbose);
  RParallelExecutor executor;
  auto nn_graph =
      tdoann::idx_to_graph(distance, idx_vec, n_threads, progress, executor);
  constexpr bool unzero = true;
  return graph_to_r(nn_graph, unzero);
}

// [[Rcpp::export]]
List rnn_idx_to_graph_self_sparse(const NumericVector &data,
                                  const IntegerVector &ind,
                                  const IntegerVector &ptr, std::size_t nobs,
                                  std::size_t ndim, const IntegerMatrix &idx,
                                  const std::string &metric = "euclidean",
                                  std::size_t n_threads = 0,
                                  bool verbose = false) {
  auto distance_ptr =
      create_sparse_self_distance(data, ind, ptr, nobs, ndim, metric);
  return idx_to_graph_impl(*distance_ptr, idx, n_threads, verbose);
}

// [[Rcpp::export]]
List rnn_idx_to_graph_self(const NumericMatrix &data, const IntegerMatrix &idx,
                           const std::string &metric = "euclidean",
                           std::size_t n_threads = 0, bool verbose = false) {
  auto distance_ptr = create_self_distance(data, metric);
  return idx_to_graph_impl(*distance_ptr, idx, n_threads, verbose);
}

// [[Rcpp::export]]
List rnn_idx_to_graph_query(const NumericMatrix &reference,
                            const NumericMatrix &query,
                            const IntegerMatrix &idx,
                            const std::string &metric = "euclidean",
                            std::size_t n_threads = 0, bool verbose = false) {
  auto distance_ptr = create_query_distance(reference, query, metric);
  return idx_to_graph_impl(*distance_ptr, idx, n_threads, verbose);
}

// [[Rcpp::export]]
List rnn_idx_to_graph_query_sparse(
    const NumericVector &ref_data, const IntegerVector &ref_ind,
    const IntegerVector &ref_ptr, std::size_t nref,
    const NumericVector &query_data, const IntegerVector &query_ind,
    const IntegerVector &query_ptr, std::size_t nquery, std::size_t ndim,
    const IntegerMatrix &idx, const std::string &metric = "euclidean",
    std::size_t n_threads = 0, bool verbose = false) {
  auto distance_ptr =
      create_sparse_query_distance(ref_data, ref_ind, ref_ptr, nref, query_data,
                                   query_ind, query_ptr, nquery, ndim, metric);
  return idx_to_graph_impl(*distance_ptr, idx, n_threads, verbose);
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers,modernize-use-trailing-return-type,readability-magic-numbers)
