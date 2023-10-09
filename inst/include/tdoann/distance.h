// BSD 2-Clause License
//
// Copyright 2019 James Melville
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

#ifndef TDOANN_DISTANCE_H
#define TDOANN_DISTANCE_H

#include <cstdint>
#include <vector>

#include "bitvec.h"

// NOLINTBEGIN(readability-identifier-length)

namespace tdoann {

// distance functions

template <typename Out, typename It>
inline auto l2sqr(const It xbegin, const It xend, const It ybegin) -> Out {
  Out sum{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    const Out diff = *xit - *yit;
    sum += diff * diff;
  }
  return sum;
}

template <typename Out, typename It>
inline auto euclidean(const It xbegin, const It xend, const It ybegin) -> Out {
  return std::sqrt(l2sqr<Out>(xbegin, xend, ybegin));
}

template <typename Out, typename It>
auto inner_product(const It xbegin, const It xend, const It ybegin) -> Out {
  Out sum{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    sum += *xit * *yit;
  }
  return std::max(1 - sum, Out{0});
}

// used by cosine and correlation to avoid division by zero
template <typename Out>
inline auto angular_dist(const Out &normx, const Out &normy, const Out &res)
    -> Out {
  Out zero{0};
  if (normx == zero && normy == zero) {
    return zero;
  }
  Out one{1};
  if (normx == zero || normy == zero) {
    return one;
  }
  return one - (res / std::sqrt(normx * normy));
}

template <typename Out, typename It>
inline auto cosine(const It xbegin, const It xend, const It ybegin) -> Out {
  Out res{0};
  Out normx{0};
  Out normy{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    Out x = *xit;
    Out y = *yit;
    res += x * y;
    normx += x * x;
    normy += y * y;
  }
  return angular_dist(normx, normy, res);
}

template <typename Out, typename It>
inline auto correlation(const It xbegin, const It xend, const It ybegin)
    -> Out {
  // calculate mean
  Out xmu{0};
  Out ymu{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    xmu += *xit;
    ymu += *yit;
  }
  const auto n = static_cast<Out>(xend - xbegin);
  xmu /= n;
  ymu /= n;

  // cosine on mean centered data
  Out res{0};
  Out normx{0};
  Out normy{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    Out x = *xit - xmu;
    Out y = *yit - ymu;
    res += x * y;
    normx += x * x;
    normy += y * y;
  }

  return angular_dist(normx, normy, res);
}

template <typename Out, typename It>
inline auto manhattan(const It xbegin, const It xend, const It ybegin) -> Out {
  Out sum{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    sum += std::abs(*xit - *yit);
  }
  return sum;
}

template <typename Out, typename It>
inline auto hamming(const It xbegin, const It xend, const It ybegin) -> Out {
  Out sum{0};
  for (It xit = xbegin, yit = ybegin; xit != xend; ++xit, ++yit) {
    sum += *xit != *yit;
  }
  return sum;
}

// Functions for initializing input vectors in functor structs

template <typename T>
auto do_nothing(const std::vector<T> &vec, std::size_t /* ndim */)
    -> std::vector<T> {
  return vec;
}

// relies on NRVO to avoid a copy
template <typename T>
auto normalize(const std::vector<T> &vec, std::size_t ndim) -> std::vector<T> {
  const constexpr T MIN_NORM = 1e-30;

  std::vector<T> normalized(vec.size());
  std::size_t npoints = vec.size() / ndim;
  for (std::size_t i = 0; i < npoints; i++) {
    std::size_t di = ndim * i;
    T norm = 0.0;

    for (std::size_t d = 0; d < ndim; d++) {
      auto val = vec[di + d];
      norm += val * val;
    }
    norm = std::sqrt(norm) + MIN_NORM;
    for (std::size_t d = 0; d < ndim; d++) {
      normalized[di + d] = vec[di + d] / norm;
    }
  }
  return normalized;
}

template <typename T>
auto mean_center(const std::vector<T> &vec, std::size_t ndim)
    -> std::vector<T> {
  std::vector<T> centered(vec.size());
  std::size_t npoints = vec.size() / ndim;

  for (std::size_t i = 0; i < npoints; i++) {
    T mu{0};
    const std::size_t di = ndim * i;
    for (std::size_t d = 0; d < ndim; d++) {
      mu += vec[di + d];
    }
    mu /= static_cast<T>(ndim);
    for (std::size_t d = 0; d < ndim; d++) {
      centered[di + d] = vec[di + d] - mu;
    }
  }
  return centered;
}

template <typename T>
auto normalize_center(const std::vector<T> &vec, std::size_t ndim)
    -> std::vector<T> {
  return normalize(mean_center(vec, ndim), ndim);
}

// template for distance functors

template <typename In, typename Out, typename It, Out (*dfun)(It, It, It),
          std::vector<In> (*initfun)(const std::vector<In> &vec,
                                     std::size_t ndim) = do_nothing,
          typename Idx = uint32_t>
struct SelfDistance {
  SelfDistance(const std::vector<In> &data, std::size_t ndim)
      : x(initfun(data, ndim)), ndim(ndim), nx(data.size() / ndim), ny(nx) {}

  inline auto operator()(Idx i, Idx j) const -> Out {
    const std::size_t di = ndim * i;
    return dfun(x.begin() + di, x.begin() + di + ndim, x.begin() + ndim * j);
  }

  const std::vector<In> x;
  std::size_t ndim;
  Idx nx;
  Idx ny;

  using Input = In;
  using Output = Out;
  using Index = Idx;
};

template <typename In, typename Out, typename It, Out (*dfun)(It, It, It),
          std::vector<In> (*initfun)(const std::vector<In> &vec,
                                     std::size_t ndim) = do_nothing,
          typename Idx = uint32_t>
struct QueryDistance {
  QueryDistance(const std::vector<In> &x, const std::vector<In> &y,
                std::size_t ndim)
      : x(initfun(x, ndim)), y(initfun(y, ndim)), ndim(ndim),
        nx(x.size() / ndim), ny(y.size() / ndim) {}

  inline auto operator()(Idx i, Idx j) const -> Out {
    const std::size_t di = ndim * i;
    return dfun(x.begin() + di, x.begin() + di + ndim, y.begin() + ndim * j);
  }

  const std::vector<In> x;
  const std::vector<In> y;
  std::size_t ndim;
  Idx nx;
  Idx ny;

  using Input = In;
  using Output = Out;
  using Index = Idx;
};

// Specialized binary Hamming functor

template <typename Out, typename Idx = uint32_t>
auto bhamming_impl(const BitVec &x, const Idx i, const BitVec &y, Idx j,
                   std::size_t len) -> Out {
  Out sum = 0;
  const std::size_t di = len * i;
  const std::size_t dj = len * j;

  for (std::size_t d = 0; d < len; d++) {
    sum += (x[di + d] ^ y[dj + d]).count();
  }

  return sum;
}

template <typename In, typename Out, typename Idx = uint32_t>
struct BHammingSelf {
  const BitVec bitvec;
  std::size_t vec_len; // size of the bitvec
  std::size_t ndim;
  Idx nx;
  Idx ny;

  BHammingSelf(const std::vector<In> &data, std::size_t ndim)
      : bitvec(to_bitvec(data, ndim)), vec_len(bitvec_size(ndim)), ndim(ndim),
        nx(data.size() / ndim), ny(nx) {}

  auto operator()(Idx i, Idx j) const -> Out {
    return bhamming_impl<Out>(bitvec, i, bitvec, j, vec_len);
  }

  using Input = In;
  using Output = Out;
  using Index = Idx;
};

template <typename In, typename Out, typename Idx = uint32_t>
struct BHammingQuery {
  const BitVec bx;
  const BitVec by;
  std::size_t vec_len;
  std::size_t ndim;
  Idx nx;
  Idx ny;

  BHammingQuery(const std::vector<In> &x, const std::vector<In> &y,
                std::size_t ndim)
      : bx(to_bitvec(x, ndim)), by(to_bitvec(y, ndim)),
        vec_len(bitvec_size(ndim)), ndim(ndim), nx(x.size() / ndim),
        ny(y.size() / ndim) {}

  auto operator()(Idx i, Idx j) const -> Out {
    return bhamming_impl<Out>(bx, i, by, j, vec_len);
  }

  using Input = In;
  using Output = Out;
  using Index = Idx;
};

} // namespace tdoann
#endif // TDOANN_DISTANCE_H
// NOLINTEND(readability-identifier-length)
