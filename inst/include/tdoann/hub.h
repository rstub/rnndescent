// BSD 2-Clause License
//
// Copyright 2021 James Melville
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// OF SUCH DAMAGE.

#ifndef TDOANN_HUB_H
#define TDOANN_HUB_H

#include <algorithm>
#include <iostream>
#include <limits>
#include <numeric>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include "heap.h"
#include "nngraph.h"

namespace tdoann {

template <typename T>
auto reverse_nbr_counts_impl(const std::vector<T> &forward_nbrs,
                             std::size_t n_points) -> std::vector<std::size_t> {
  const std::size_t n_nbrs = forward_nbrs.size() / n_points;

  std::vector<std::size_t> counts(n_points);
  std::size_t innbrs;
  std::size_t inbr;

  for (std::size_t i = 0; i < n_points; i++) {
    innbrs = i * n_nbrs;
    for (std::size_t j = 0; j < n_nbrs; j++) {
      inbr = forward_nbrs[innbrs + j];
      if (inbr == i) {
        continue;
      }
      ++counts[inbr];
    }
  }
  return counts;
}

// treat the knn graph as directed where each point is the "head" of the
// directed edge, and each neighbor is the "tail". The head and tail nodes may
// be entirely disjoint, e.g. coming from query and reference nodes,
// respectively.
template <typename T>
auto reverse_nbr_counts_impl(const std::vector<T> &forward_nbrs,
                             std::size_t n_head_points,
                             std::size_t n_tail_points)
    -> std::vector<std::size_t> {
  const std::size_t n_nbrs = forward_nbrs.size() / n_head_points;

  std::vector<std::size_t> counts(n_tail_points);
  std::size_t innbrs;

  for (std::size_t i = 0; i < n_head_points; i++) {
    innbrs = i * n_nbrs;
    for (std::size_t j = 0; j < n_nbrs; j++) {
      ++counts[forward_nbrs[innbrs + j]];
    }
  }
  return counts;
}

template <typename T>
auto reverse_nbr_counts(const std::vector<T> &forward_nbrs,
                        std::size_t n_points, bool include_self)
    -> std::vector<std::size_t> {

  // include_self: either the index into the nbrs and the nbrs are from two
  // disjoint set of nodes or it's the same nodes and we don't mind counting
  // loops
  if (include_self) {
    auto n_tail_points =
        *std::max_element(forward_nbrs.begin(), forward_nbrs.end()) + 1;
    return reverse_nbr_counts_impl(forward_nbrs, n_points, n_tail_points);
  } else {
    return reverse_nbr_counts_impl(forward_nbrs, n_points);
  }
}

template <typename DistOut, typename Value> struct DistPair {
  DistOut dist;
  Value value;
  DistPair()
      : dist((std::numeric_limits<DistOut>::max)()),
        value(static_cast<Value>(-1)) {}
  DistPair(const DistOut &d, const Value &v) : dist(d), value(v) {}
};

template <typename D, typename V>
bool operator<(const DistPair<D, V> &x, const DistPair<D, V> &y) {
  return std::tie(x.value, x.dist) < std::tie(y.value, y.dist);
}

template <typename D, typename V>
bool operator>=(const DistPair<D, V> &x, const DistPair<D, V> &y) {
  return std::tie(x.value, x.dist) >= std::tie(y.value, y.dist);
}

template <typename NbrHeap>
auto kograph(const NbrHeap &heap)
    -> NNHeap<DistPair<typename NbrHeap::DistanceOut, typename NbrHeap::Index>,
              typename NbrHeap::Index> {
  using KDist =
      DistPair<typename NbrHeap::DistanceOut, typename NbrHeap::Index>;
  auto kos = reverse_nbr_counts(heap.idx, heap.n_points, true);

  NNHeap<KDist, typename NbrHeap::Index> kog(heap.n_points, heap.n_nbrs);
  for (typename NbrHeap::Index i = 0; i < heap.n_points; i++) {
    for (std::size_t j = 0; j < heap.n_nbrs; j++) {
      auto nbr = heap.index(i, j);
      auto d = heap.distance(i, j);
      kog.checked_push(i, KDist(d, kos[nbr]), nbr);
    }
  }

  return kog;
}

// Create a degree-adjusted graph, similar to the "Static Degree Adjustment with
// Constraints" algorithm described by Iwazaki and Miyasaki (2018).
// Basically this builds the reverse neighbor graph and then adds back in
// forward neighbors to fill in any gaps.
// n_rev_nbrs controls the maximum number of reverse neighbors to keep in the
// reverse neighbor heap.
// n_adj_nbrs controls both the number of neighbors in the returned graph, and
// also the number of number of forward neighbors used to create the reverse
// neighbor heap. e.g. if you pass in a 40NN heap but set n_adj_nbrs to 20, you
// are effectively using the 20NN to construct the reverse neighbors (and you
// will also only get a 20NN heap back).
// NOTE: this sorts the heap.
template <typename NbrHeap>
auto deg_adj_graph(NbrHeap &heap, std::size_t n_rev_nbrs,
                   std::size_t n_adj_nbrs) -> NbrHeap {
  auto rev_heap = reverse_heap(heap, n_rev_nbrs, n_adj_nbrs);
  NbrHeap adj_heap(heap.n_points, n_adj_nbrs);

  sort_heap(heap);

  for (typename NbrHeap::Index i = 0; i < rev_heap.n_points; i++) {
    for (std::size_t j = 0; j < rev_heap.n_nbrs; j++) {
      if (adj_heap.is_full(i)) {
        break;
      }
      auto nbr = rev_heap.index(i, j);
      if (nbr == rev_heap.npos()) {
        continue;
      }
      adj_heap.checked_push(i, rev_heap.distance(i, j), nbr);
    }
    for (std::size_t j = 0; j < heap.n_nbrs; j++) {
      if (adj_heap.is_full(i)) {
        break;
      }
      auto nbr = heap.index(i, j);
      if (nbr == heap.npos()) {
        continue;
      }
      adj_heap.checked_push(i, heap.distance(i, j), nbr);
    }
  }
  return adj_heap;
}

template <typename NbrHeap> auto deg_adj_graph(const NbrHeap &heap) -> NbrHeap {
  return deg_adj_heap(heap, heap.n_nbrs, heap.n_nbrs);
}

// Similar to deg_adj_graph, but forward neighbors are added according to
// increasing k-occurrence
template <typename NbrHeap>
auto ko_adj_graph(const NbrHeap &heap, std::size_t n_rev_nbrs,
                  std::size_t n_adj_nbrs) -> NbrHeap {
  auto rev_heap = reverse_heap(heap, n_rev_nbrs, n_adj_nbrs);
  NbrHeap adj_heap(heap.n_points, n_adj_nbrs);

  auto ko_heap = kograph(heap);
  sort_heap(ko_heap);

  for (typename NbrHeap::Index i = 0; i < rev_heap.n_points; i++) {
    for (std::size_t j = 0; j < rev_heap.n_nbrs; j++) {
      if (adj_heap.is_full(i)) {
        break;
      }
      auto nbr = rev_heap.index(i, j);
      if (nbr == rev_heap.npos()) {
        continue;
      }
      adj_heap.checked_push(i, rev_heap.distance(i, j), nbr);
    }
    for (std::size_t j = 0; j < heap.n_nbrs; j++) {
      if (adj_heap.is_full(i)) {
        break;
      }
      auto nbr = ko_heap.index(i, j);
      if (nbr == ko_heap.npos()) {
        continue;
      }
      adj_heap.checked_push(i, ko_heap.distance(i, j).dist, nbr);
    }
  }
  return adj_heap;
}

template <typename NbrHeap> auto ko_adj_graph(const NbrHeap &heap) -> NbrHeap {
  return ko_adj_graph(heap, heap.n_nbrs, heap.n_nbrs);
}

// Create a heap containing both forward and reverse neighbors up to size
// n_nbrs. Effectively the same as the degree_prune function in pynndescent
// with n_nbrs = max_degree (I think)
template <typename NbrHeap>
auto mutualize_heap(const NbrHeap &heap, std::size_t n_nbrs) -> NbrHeap {
  NbrHeap mutual_heap(heap.n_points, n_nbrs);

  for (typename NbrHeap::Index i = 0; i < heap.n_points; i++) {
    for (typename NbrHeap::Index j = 0; j < heap.n_nbrs; j++) {
      auto nbr = heap.index(i, j);
      if (nbr == heap.npos()) {
        continue;
      }
      mutual_heap.checked_push_pair(i, heap.distance(i, j), nbr);
    }
  }

  return mutual_heap;
}

template <typename NbrHeap>
auto mutualize_heap(const NbrHeap &heap) -> NbrHeap {
  return mutualize_heap(heap.n_points, heap.n_nbrs);
}

// Create a heap containing all forward neighbors and add on as many reverse
// neighbors until full. n_nbrs should be > heap.n_nbrs
template <typename NbrHeap>
auto partial_mutualize_heap(const NbrHeap &heap, std::size_t n_nbrs)
    -> NbrHeap {
  auto reversed_heap = reverse_heap(heap, n_nbrs, heap.n_nbrs);
  NbrHeap pmutual_heap(heap.n_points, n_nbrs);

  for (typename NbrHeap::Index i = 0; i < heap.n_points; i++) {
    for (typename NbrHeap::Index j = 0; j < heap.n_nbrs; j++) {
      auto nbr = heap.index(i, j);
      if (nbr == heap.npos()) {
        continue;
      }
      pmutual_heap.checked_push(i, heap.distance(i, j), nbr);
    }
    for (typename NbrHeap::Index j = 0; j < reversed_heap.n_nbrs; j++) {
      if (pmutual_heap.is_full(i)) {
        break;
      }
      auto nbr = reversed_heap.index(i, j);
      if (nbr == reversed_heap.npos()) {
        continue;
      }
      pmutual_heap.checked_push(i, reversed_heap.distance(i, j), nbr);
    }
  }

  return pmutual_heap;
}

template <typename NbrHeap>
auto partial_mutualize_heap(const NbrHeap &heap) -> NbrHeap {
  return partial_mutualize_heap(heap.n_points, 2 * heap.n_nbrs);
}

template <typename It>
auto order(It first, It last) -> std::vector<std::size_t> {
  std::vector<std::size_t> idx(last - first);
  std::iota(idx.begin(), idx.end(), static_cast<std::size_t>(0));

  auto cmp = [&first](std::size_t a, std::size_t b) {
    return *(first + a) < *(first + b);
  };
  std::stable_sort(idx.begin(), idx.end(), cmp);

  return idx;
}

template <typename SparseNNGraph>
auto degree_prune(const SparseNNGraph &graph, std::size_t max_degree)
    -> SparseNNGraph {
  using DistOut = typename SparseNNGraph::DistanceOut;
  using Idx = typename SparseNNGraph::Index;

  const std::size_t n_points = graph.n_points;

  std::vector<std::size_t> new_row_ptr(n_points + 1);
  std::vector<Idx> new_col_idx;
  std::vector<DistOut> new_dist;

  for (std::size_t i = 0; i < n_points; i++) {
    const std::size_t i1 = i + 1;
    new_row_ptr[i1] = new_row_ptr[i];

    const auto begin = graph.row_ptr[i];
    const auto end = graph.row_ptr[i1];

    auto ordered = order(graph.dist.begin() + begin, graph.dist.begin() + end);

    const auto unpruned_n_nbrs = end - begin;
    const auto n_nbrs = std::min(unpruned_n_nbrs, max_degree);

    for (std::size_t j = 0; j < n_nbrs; j++) {
      new_col_idx.push_back(graph.col_idx[begin + ordered[j]]);
      new_dist.push_back(graph.dist[begin + ordered[j]]);
    }
    new_row_ptr[i1] += n_nbrs;
  }
  return SparseNNGraph(new_row_ptr, new_col_idx, new_dist);
}

template <typename SparseNNGraph, typename Distance, typename Rand>
auto remove_long_edges_sp(const SparseNNGraph &graph, const Distance &distance,
                          Rand &rand, double prune_probability)
    -> SparseNNGraph {
  using DistOut = typename SparseNNGraph::DistanceOut;
  using Idx = typename SparseNNGraph::Index;

  const std::size_t n_points = graph.n_points;

  std::vector<std::size_t> new_row_ptr(n_points + 1);
  std::vector<Idx> new_col_idx;
  std::vector<DistOut> new_dist;

  for (std::size_t i = 0; i < n_points; i++) {
    const std::size_t i1 = i + 1;
    new_row_ptr[i1] = new_row_ptr[i];

    const std::size_t n_nbrs = graph.row_ptr[i1] - graph.row_ptr[i];
    if (n_nbrs == 0) {
      continue;
    }

    auto ordered = order(graph.dist.begin() + graph.row_ptr[i],
                         graph.dist.begin() + graph.row_ptr[i1]);

    // initialize new graph with closest neighbor
    new_col_idx.push_back(graph.col_idx[graph.row_ptr[i] + ordered[0]]);
    new_dist.push_back(graph.dist[graph.row_ptr[i] + ordered[0]]);
    ++new_row_ptr[i1];

    // search all other neighbors (NB: start at 1)
    for (std::size_t j = 1; j < n_nbrs; j++) {
      Idx nbr = graph.col_idx[graph.row_ptr[i] + ordered[j]];
      DistOut nbr_dist = graph.dist[graph.row_ptr[i] + ordered[j]];

      // Compare this neighbor with those that previously passed the filter
      bool add_nbr = true;
      for (std::size_t k = new_row_ptr[i]; k < new_row_ptr[i1]; k++) {
        Idx ng_nbr = new_col_idx[k];
        DistOut d = distance(nbr, ng_nbr);
        if (d < nbr_dist && rand.unif() < prune_probability) {
          add_nbr = false;
          break;
        }
      }

      if (add_nbr) {
        new_col_idx.push_back(nbr);
        new_dist.push_back(nbr_dist);
        ++new_row_ptr[i1];
      }
    }
  }
  return SparseNNGraph(new_row_ptr, new_col_idx, new_dist);
}

template <typename SparseNNGraph, typename Distance>
auto remove_long_edges_sp(const SparseNNGraph &graph, const Distance &distance)
    -> SparseNNGraph {
  using DistOut = typename SparseNNGraph::DistanceOut;
  using Idx = typename SparseNNGraph::Index;

  const std::size_t n_points = graph.n_points;

  std::vector<std::size_t> new_row_ptr(n_points + 1);
  std::vector<Idx> new_col_idx;
  std::vector<DistOut> new_dist;

  for (std::size_t i = 0; i < n_points; i++) {
    const std::size_t i1 = i + 1;
    new_row_ptr[i1] = new_row_ptr[i];

    const std::size_t n_nbrs = graph.row_ptr[i1] - graph.row_ptr[i];
    if (n_nbrs == 0) {
      continue;
    }

    auto ordered = order(graph.dist.begin() + graph.row_ptr[i],
                         graph.dist.begin() + graph.row_ptr[i1]);

    // initialize new graph with closest neighbor
    new_col_idx.push_back(graph.col_idx[graph.row_ptr[i] + ordered[0]]);
    new_dist.push_back(graph.dist[graph.row_ptr[i] + ordered[0]]);
    ++new_row_ptr[i1];

    // search all other neighbors (NB: start at 1)
    for (std::size_t j = 1; j < n_nbrs; j++) {
      Idx nbr = graph.col_idx[graph.row_ptr[i] + ordered[j]];
      DistOut nbr_dist = graph.dist[graph.row_ptr[i] + ordered[j]];

      // Compare this neighbor with those that previously passed the filter
      bool add_nbr = true;
      for (std::size_t k = new_row_ptr[i]; k < new_row_ptr[i1]; k++) {
        Idx ng_nbr = new_col_idx[k];
        DistOut d = distance(nbr, ng_nbr);
        if (d < nbr_dist) {
          add_nbr = false;
          break;
        }
      }

      if (add_nbr) {
        new_col_idx.push_back(nbr);
        new_dist.push_back(nbr_dist);
        ++new_row_ptr[i1];
      }
    }
  }
  return SparseNNGraph(new_row_ptr, new_col_idx, new_dist);
}

template <typename SparseNNGraph>
auto merge_graphs(const SparseNNGraph &g1, const SparseNNGraph &g2)
    -> SparseNNGraph {
  using DistOut = typename SparseNNGraph::DistanceOut;
  using Idx = typename SparseNNGraph::Index;

  const std::size_t n_points = g1.n_points;

  std::vector<std::size_t> merged_row_ptr(n_points + 1);
  std::vector<Idx> merged_col_idx;
  std::vector<DistOut> merged_dist;

  std::vector<Idx> search_idx = g1.col_idx;
  for (std::size_t i = 0; i < n_points; i++) {
    const auto begin = g1.row_ptr[i];
    const auto end = g1.row_ptr[i + 1];

    std::sort(search_idx.begin() + begin, search_idx.begin() + end);

    std::vector<Idx> col_idx_i(g1.col_idx.begin() + begin,
                               g1.col_idx.begin() + end);
    std::vector<DistOut> dist_i(g1.dist.begin() + begin, g1.dist.begin() + end);

    merged_row_ptr[i + 1] = merged_row_ptr[i] + col_idx_i.size();

    for (std::size_t j = g2.row_ptr[i]; j < g2.row_ptr[i + 1]; j++) {
      if (!std::binary_search(search_idx.begin() + begin,
                              search_idx.begin() + end, g2.col_idx[j])) {
        col_idx_i.push_back(g2.col_idx[j]);
        dist_i.push_back(g2.dist[j]);
        ++merged_row_ptr[i + 1];
      }
    }
    merged_col_idx.insert(merged_col_idx.end(), col_idx_i.begin(),
                          col_idx_i.end());
    merged_dist.insert(merged_dist.end(), dist_i.begin(), dist_i.end());
  }
  return SparseNNGraph(merged_row_ptr, merged_col_idx, merged_dist);
}

// remove neighbors which are "occlusions"
// for point i with neighbors p and q, if d(p, q) < d(i, p), then p occludes q
template <typename NNGraph, typename Distance, typename Rand>
auto remove_long_edges(const NNGraph &graph, const Distance &distance,
                       Rand &rand, double prune_probability) -> NNGraph {
  using DistOut = typename NNGraph::DistanceOut;
  using Idx = typename NNGraph::Index;

  const std::size_t n_points = graph.n_points;
  const std::size_t n_nbrs = graph.n_nbrs;

  NNGraph new_graph(n_points, n_nbrs);

  std::size_t innbrs = 0;
  std::size_t ij = 0;

  for (std::size_t i = 0; i < n_points; i++) {
    innbrs = i * n_nbrs;

    // initialize new graph with closest neighbor
    new_graph.idx[innbrs] = graph.idx[innbrs];
    new_graph.dist[innbrs] = graph.dist[innbrs];

    std::size_t n_new_nbrs = 1;

    // search all other neighbors (NB: start at 1)
    for (std::size_t j = 1; j < n_nbrs; j++) {
      ij = innbrs + j;
      Idx nbr = graph.idx[ij];
      if (nbr == graph.npos()) {
        break;
      }

      bool add_nbr = true;
      DistOut nbr_dist = graph.dist[ij];
      // Compare this neighbor with those that previously passed the filter
      for (std::size_t k = 0; k < n_new_nbrs; k++) {
        Idx ng_nbr = new_graph.idx[innbrs + k];
        DistOut d = distance(nbr, ng_nbr);
        if (d < nbr_dist && rand.unif() < prune_probability) {
          add_nbr = false;
          break;
        }
      }

      if (add_nbr) {
        new_graph.idx[innbrs + n_new_nbrs] = nbr;
        new_graph.dist[innbrs + n_new_nbrs] = nbr_dist;
        ++n_new_nbrs;
      }
    }
  }
  return new_graph;
}

} // namespace tdoann
#endif // TDOANN_HUB_H