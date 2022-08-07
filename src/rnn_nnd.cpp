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

// NOLINTBEGIN(modernize-use-trailing-return-type)

#include <Rcpp.h>

#include "rnndescent/random.h"
#include "tdoann/graphupdate.h"
#include "tdoann/nndescent.h"
#include "tdoann/nndparallel.h"

#include "rnn_distance.hpp"
#include "rnn_heaptor.hpp"
#include "rnn_macros.hpp"
#include "rnn_progress.hpp"
#include "rnn_rtoheap.hpp"

using Rcpp::IntegerMatrix;
using Rcpp::List;
using Rcpp::NumericMatrix;

#define NND_IMPL()                                                             \
  return nnd_impl.get_nn<GraphUpdate, Distance, Progress, NNDProgress>(        \
      nn_idx, nn_dist, max_candidates, n_iters, delta, verbose);

#define NND_PROGRESS()                                                         \
  if (progress == "bar") {                                                     \
    using Progress = RPProgress;                                               \
    using NNDProgress = tdoann::NNDProgress<Progress>;                         \
    NND_IMPL()                                                                 \
  }                                                                            \
  using Progress = RIterProgress;                                              \
  using NNDProgress = tdoann::HeapSumProgress<Progress>;                       \
  NND_IMPL()

#define NND_BUILD_UPDATER()                                                    \
  if (n_threads > 0) {                                                         \
    using NNDImpl = NNDBuildParallel;                                          \
    NNDImpl nnd_impl(data, n_threads);                                         \
    if (low_memory) {                                                          \
      using GraphUpdate = tdoann::upd::Factory<tdoann::upd::Batch>;            \
      NND_PROGRESS()                                                           \
    }                                                                          \
    using GraphUpdate = tdoann::upd::Factory<tdoann::upd::BatchHiMem>;         \
    NND_PROGRESS()                                                             \
  }                                                                            \
  using NNDImpl = NNDBuildSerial;                                              \
  NNDImpl nnd_impl(data);                                                      \
  if (low_memory) {                                                            \
    using GraphUpdate = tdoann::upd::Factory<tdoann::upd::Serial>;             \
    NND_PROGRESS()                                                             \
  }                                                                            \
  using GraphUpdate = tdoann::upd::Factory<tdoann::upd::SerialHiMem>;          \
  NND_PROGRESS()

class NNDBuildSerial {

public:
  explicit NNDBuildSerial(const NumericMatrix &data) : data(data) {}

  template <typename GraphUpdate, typename Distance, typename Progress,
            typename NNDProgress>
  auto get_nn(const IntegerMatrix &nn_idx, const NumericMatrix &nn_dist,
              std::size_t max_candidates, std::size_t n_iters, double delta,
              bool verbose) -> List {
    using Out = typename Distance::Output;
    using Index = typename Distance::Index;

    auto nnd_heap =
        r_to_heap<tdoann::HeapAddSymmetric, tdoann::NNDHeap<Out, Index>>(
            nn_idx, nn_dist);
    auto distance = tr_to_dist<Distance>(data);
    auto graph_updater = GraphUpdate::create(nnd_heap, distance);
    Progress progress(n_iters, verbose);
    NNDProgress nnd_progress(progress);
    rnndescent::RRand rand;

    tdoann::nnd_build(graph_updater, max_candidates, n_iters, delta, rand,
                      nnd_progress);

    return heap_to_r(nnd_heap);
  }

private:
  NumericMatrix data;
};

class NNDBuildParallel {
public:
  NNDBuildParallel(const NumericMatrix &data, std::size_t n_threads)
      : data(data), n_threads(n_threads) {}

  template <typename GraphUpdate, typename Distance, typename Progress,
            typename NNDProgress>
  auto get_nn(const IntegerMatrix &nn_idx, const NumericMatrix &nn_dist,
              std::size_t max_candidates, std::size_t n_iters, double delta,
              bool verbose) -> List {
    using Out = typename Distance::Output;
    using Index = typename Distance::Index;

    const std::size_t grain_size = 1;
    auto nnd_heap =
        r_to_heap<tdoann::LockingHeapAddSymmetric, tdoann::NNDHeap<Out, Index>>(
            nn_idx, nn_dist, n_threads, grain_size);
    auto distance = tr_to_dist<Distance>(data);
    auto graph_updater = GraphUpdate::create(nnd_heap, distance);
    Progress progress(n_iters, verbose);
    NNDProgress nnd_progress(progress);
    rnndescent::ParallelRand<rnndescent::PcgRand> parallel_rand;

    tdoann::nnd_build<RParallel>(graph_updater, max_candidates, n_iters, delta,
                                 nnd_progress, parallel_rand, n_threads);

    return heap_to_r(nnd_heap, n_threads);
  }

private:
  NumericMatrix data;
  std::size_t n_threads;
};

// [[Rcpp::export]]
List nn_descent(const NumericMatrix &data, const IntegerMatrix &nn_idx,
                const NumericMatrix &nn_dist, const std::string &metric,
                std::size_t max_candidates, std::size_t n_iters, double delta,
                bool low_memory, std::size_t n_threads, bool verbose,
                const std::string &progress) {
  DISPATCH_ON_DISTANCES(NND_BUILD_UPDATER);
}

// NOLINTEND(modernize-use-trailing-return-type)
