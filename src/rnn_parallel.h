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

// Generic parallel helper code

#ifndef RNN_PARALLEL_H
#define RNN_PARALLEL_H

#include <Rcpp.h>
// [[Rcpp::depends(RcppParallel)]]
#include <RcppParallel.h>

#include "tdoann/heap.h"
#include "tdoann/progress.h"

#include "rnn.h"

struct ParallelOnlyWorker {
  RcppParallel::Worker &parallel_worker;

  ParallelOnlyWorker(RcppParallel::Worker &parallel_worker)
      : parallel_worker(parallel_worker){};

  void after_parallel(std::size_t begin, std::size_t end) {}
};

template <typename BatchParallelWorker, typename Progress>
void batch_parallel_for(BatchParallelWorker &rnn_worker, Progress &progress,
                        std::size_t n, std::size_t block_size,
                        std::size_t grain_size, bool &interrupted) {
  interrupted = false;
  const auto n_blocks = (n / block_size) + 1;
  for (std::size_t i = 0; i < n_blocks; i++) {
    const auto begin = i * block_size;
    const auto end = std::min(n, begin + block_size);
    RcppParallel::parallelFor(begin, end, rnn_worker.parallel_worker,
                              grain_size);
    interrupted = progress.check_interrupt();
    if (interrupted) {
      break;
    }
    rnn_worker.after_parallel(begin, end);
    progress.block_finished();
    interrupted = progress.check_interrupt();
    if (interrupted) {
      break;
    }
  }
}

template <typename Progress>
void batch_parallel_for(RcppParallel::Worker &worker, Progress &progress,
                        std::size_t n, std::size_t block_size,
                        std::size_t grain_size, bool &interrupted) {
  ParallelOnlyWorker parallel_only_worker(worker);
  batch_parallel_for(parallel_only_worker, progress, n, block_size, grain_size,
                     interrupted);
}

template <typename Progress>
void batch_parallel_for(RcppParallel::Worker &worker, Progress &progress,
                        std::size_t n, std::size_t block_size,
                        std::size_t grain_size) {
  bool dont_care_if_interrupted = false;
  batch_parallel_for(worker, progress, n, block_size, grain_size,
                     dont_care_if_interrupted);
}

template <typename HeapAdd, typename NbrHeap = tdoann::SimpleNeighborHeap>
struct RToHeapWorker : public RcppParallel::Worker {
  NbrHeap &heap;
  RcppParallel::RMatrix<int> idx;
  RcppParallel::RMatrix<double> dist;
  int max_idx;

  RToHeapWorker(NbrHeap &heap, Rcpp::IntegerMatrix idx,
                Rcpp::NumericMatrix dist,
                int max_idx = (std::numeric_limits<int>::max)())
      : heap(heap), idx(idx), dist(dist), max_idx(max_idx) {}

  void operator()(std::size_t begin, std::size_t end) {
    r_to_heap<HeapAdd, NbrHeap, RcppParallel::RMatrix<int>,
              RcppParallel::RMatrix<double>>(heap, idx, dist, begin, end,
                                             max_idx);
  }
};

template <typename HeapAdd, typename NbrHeap = tdoann::SimpleNeighborHeap>
void r_to_heap_parallel(NbrHeap &heap, Rcpp::IntegerMatrix idx,
                        Rcpp::NumericMatrix dist, std::size_t block_size,
                        std::size_t grain_size) {
  const std::size_t n_points = idx.nrow();
  RToHeapWorker<HeapAdd, NbrHeap> worker(heap, idx, dist);
  tdoann::NullProgress progress;
  batch_parallel_for(worker, progress, n_points, block_size, grain_size);
}

template <typename HeapAdd, typename NbrHeap = tdoann::SimpleNeighborHeap>
void sort_knn_graph_parallel(Rcpp::IntegerMatrix idx, Rcpp::NumericMatrix dist,
                             std::size_t block_size, std::size_t grain_size) {
  const std::size_t n_points = idx.nrow();
  const std::size_t n_nbrs = idx.ncol();

  NbrHeap heap(n_points, n_nbrs);
  r_to_heap_parallel<HeapAdd, NbrHeap>(heap, idx, dist, block_size, grain_size);
  heap.deheap_sort();
  heap_to_r(heap, idx, dist);
}

#endif // RNN_PARALLEL_H
