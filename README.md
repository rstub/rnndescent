# rnndescent

<!-- badges: start -->
[![R-CMD-check](https://github.com/jlmelville/rnndescent/workflows/R-CMD-check/badge.svg)](https://github.com/jlmelville/rnndescent/actions)
[![Travis-CI Build Status](https://travis-ci.org/jlmelville/rnndescent.svg?branch=master)](https://travis-ci.org/jlmelville/rnndescent) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/jlmelville/rnndescent?branch=master&svg=true)](https://ci.appveyor.com/project/jlmelville/rnndescent) [![Coverage Status](https://img.shields.io/codecov/c/github/jlmelville/rnndescent/master.svg)](https://codecov.io/github/jlmelville/rnndescent?branch=master)
[![Last Commit](https://img.shields.io/github/last-commit/jlmelville/rnndescent)](https://github.com/jlmelville/rnndescent)
<!-- badges: end -->

An R package implementing the Nearest Neighbor Descent method
([Dong et al., 2011](https://doi.org/10.1145/1963405.1963487)) for finding
approximate nearest neighbors, based on the Python library
[PyNNDescent](https://github.com/lmcinnes/pynndescent).

Lightly in development, but possibly not totally useless for optimizing
an initial set of nearest neighbors, e.g. those generated by
[RcppAnnoy](https://cran.r-project.org/package=RcppAnnoy) or
[RcppHNSW](https://cran.r-project.org/package=RcppHNSW).

## Current Status

*4 October 2020* Added `"correlation"` as a metric and the `k_occur` function
to help diagnose potential
[hubness](https://www.jmlr.org/papers/v11/radovanovic10a.html) in a dataset.

*23 November 2019* Added `merge_knn` and `merge_knnl` for combining multiple
nn results.

*15 November 2019* It is now possible to query a reference set of data to
produce the approximate knn graph relative to the references (i.e. none of the
queries will be selected as neighbors) via `nnd_knn_query` (and related
`brute_force` and `random` variants).

*27 October 2019* `rnndescent` creeps towards usability. A multi-threaded
implementation (using
[RcppParallel](https://cran.r-project.org/package=RcppParallel)) has now been
added.

## Installation

```r
remotes::install_github("jlmelville/rnndescent")
```

This packages makes use of C++ code which must be compiled. You may have to
carry out a few extra steps before being able to build:

**Windows**: install
[Rtools](https://cran.r-project.org/bin/windows/Rtools/) and ensure
`C:\Rtools\bin` is on your path.

**Mac OS X**: using a custom `~/.R/Makevars`
[may cause linking errors](https://github.com/jlmelville/uwot/issues/1).
This sort of thing is a potential problem on all platforms but seems to bite
Mac owners more.
[The R for Mac OS X FAQ](https://cran.r-project.org/bin/macosx/RMacOSX-FAQ.html#Installation-of-source-packages)
may be helpful here to work out what you can get away with. To be on the safe
side, I would advise building without a custom `Makevars`.

## Example

Optimizing an initial set of approximate nearest neighbors:

```R
# install.packages("RcppHNSW")
library(rnndescent)

# both hnsw_knn and nnd_knn will remove non-numeric columns from data-frames
# for you, but to avoid confusion, these examples will use a matrix
irism <- as.matrix(iris[, -5])
# Use settings that don't get perfect results straight away
iris_hnsw_nn <- RcppHNSW::hnsw_knn(irism, k = 15, M = 2, distance = "euclidean")

# nn descent improves results: set verbose = TRUE to track distance sum progress
# over iterations
res <- nnd_knn(irism, metric = "euclidean", init = iris_hnsw_nn, verbose = TRUE)

# search can be multi-threaded
res <- nnd_knn(irism, metric = "euclidean", init = iris_hnsw_nn, verbose = TRUE, n_threads = 4)

# a faster version of the algorithm is available that avoids some repeated distance
# calculations at the cost of using more memory. Currently off by default.
res <- nnd_knn(irism, metric = "euclidean", init = iris_hnsw_nn, verbose = TRUE, n_threads = 4,
               low_memory = FALSE)
```

You can also query a "reference" set of data with query data, so that the
returned knn indices refer only to the reference data:

```R
# 100 reference iris items
iris_ref <- iris[iris$Species %in% c("setosa", "versicolor"), ]

# 50 query items
iris_query <- iris[iris$Species == "versicolor", ]

# First, find the approximate 10-nearest neighbor graph for the references:
iris_ref_knn <- nnd_knn(iris_ref, k = 10)

# For each item in iris_query find the 10 nearest neighbors in iris_ref
# You need to pass both the reference data and the knn graph indices (the
# 'idx' matrix in the return value of nnd_knn).
iris_query_nn <- graph_knn_query(iris_ref, iris_ref_knn$idx, iris_query,
  k = 4, metric = "euclidean",
  verbose = TRUE
)
```

## Initialization

Currently, only the nearest neighbor descent part of PyNNDescent is implemented,
not the random projection tree method to create the initial set of neighbors. If
you really wish to avoid any dependencies from another library, then the search
is initialized from a random set of neighbors:

```R
library(rnndescent)

irism <- as.matrix(iris[, -5])

# picks indices at random and then carries out nearest neighbor descent
res <- nnd_knn(irism, k = 15, metric = "euclidean", n_threads = 4)

# if you want the random indices and their distances:
iris_rand_nn <- random_knn(irism, k = 15, metric = "euclidean", n_threads = 4)
```

Although the initialization can also be multi-threaded (and at least has speed
in its favor), the descent stage will take more iterations to get to a good
result and may converge to a less optimal result. For initializing a knn query,
there is also `random_knn_query`.

## Brute Force

For comparison with exact results, there is also `brute_force_knn` and
`brute_force_knn_query` functions, that will generate the exact nearest
neighbors by the simple process of trying every possible pair in the dataset.
Obviously this becomes a very time consuming process as your dataset grows in
size, even with multithreading (although the `iris` dataset in the example below
doesn't present any issues).

```R
iris_exact_nn <- brute_force_knn(irism, k = 15, metric = "euclidean", n_threads = 4)
```

## Merging

Also available are two functions for merging multiple approximate nearest neighbor
graphs, which will result in a new graph which is at least as good as the best
graph provided. For merging pairs of graphs, use `merge_knn`:

```R
set.seed(1337)
# Nearest neighbor descent with 15 neighbors for iris three times,
# starting from a different random initialization each time
iris_rnn1 <- nnd_knn(iris, k = 15, n_iters = 1)
iris_rnn2 <- nnd_knn(iris, k = 15, n_iters = 1)

# Merged results should be an improvement over either individual results
iris_mnn <- merge_knn(iris_rnn1, iris_rnn2)
sum(iris_mnn$dist) < sum(iris_rnn1$dist)
sum(iris_mnn$dist) < sum(iris_rnn2$dist)
```

If you have more than two graphs stored in memory, it's more efficient to use
the list-based version, `merge_knnl`:

```R
set.seed(1337)
# Nearest neighbor descent with 15 neighbors for iris three times,
# starting from a different random initialization each time
iris_rnn1 <- nnd_knn(iris, k = 15, n_iters = 1)
iris_rnn2 <- nnd_knn(iris, k = 15, n_iters = 1)
iris_rnn3 <- nnd_knn(iris, k = 15, n_iters = 1)

iris_mnn <- merge_knnl(list(iris_rnn1, iris_rnn2, iris_rnn3))
```

## Hubness Diagnostic

The `k_occur` function takes an `idx` matrix and returns a vector of the
k-occurrences for each item in the dataset. This is just the number of times an
item was found in the k-nearest neighbor list of another item. If you think of
the `idx` matrix as representing a directed graph where the element `idx[i, j]`
in the matrix is an edge from node `i` to node `idx[i, j]`, then the
k_occurrences are calculated by reversing each edge and then counts the number
of edges incident to each node. Alternatively, in the nomenclature of nearest
neighbor descent, it's the size of the "reverse neighbor" list for each node.

```R
iris_nnd <- nnd_knn(iris, k = 15)
kos <- k_occur(iris_nnd$idx)
```

The k-occurrence can take a value between 0 and `N` the number of items in the
dataset. Values much larger than `k` indicate that the item is potentially a
hub. The presence of hubs in a dataset can reduce the accuracy of the
approximate nearest neighbors returned by nearest neighbor descent, but the
presence of hubs as determined by the distribution of k-occurrences is quite
robust even in the case of an approximate nearest neighbor graph of low
accuracy. Therefore calculating the k-occurrences on the output of nearest
neighbor descent is worth doing: if the maximum k-occurrence is a lot larger
than `k` (I suggest `10 * k` as a danger sign), then the accuracy of the
approximate nearest neighbors may be compromised. Items with low k-occurrences
are most likely to be affected in this way. Increasing `k` or the
`max_candidates` parameter can help in these situations.

For more on hubness and nearest neighbors, see for example 
[Radovanović and co-workers, 2010](https://www.jmlr.org/papers/v11/radovanovic10a.html)
and [Bratić and co-workers, 2019](https://doi.org/10.1142/S0218213019600029).

## Supported Metrics

Euclidean, Manhattan, Cosine, Correlation (1 - the Pearson correlation,
implemented as cosine distance on row-centered data) and Hamming. Note that
these have been implemented in a simple fashion, so no clever (but non-portable)
optimizations using AVX/SSE or specialized `popcount` routines are used.

## Citation

Dong, W., Moses, C., & Li, K. (2011, March).
Efficient k-nearest neighbor graph construction for generic similarity measures.
In *Proceedings of the 20th international conference on World wide web* (pp. 577-586). ACM.
[doi.org/10.1145/1963405.1963487](https://doi.org/10.1145/1963405.1963487).

## License

The R and Rcpp interface code as a whole is
[GPLv3 or later](https://www.gnu.org/licenses/gpl-3.0.txt). One of those files is a modification of 
some sampling code from [dqrng](https://github.com/daqana/dqrng), and is 
[AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html). The underlying nearest neighbor descent C++
library, which can be found under `inst/include/tdoann`, is 
[BSD 2-Clause](https://opensource.org/licenses/BSD-2-Clause).

## See Also

* [PyNNDescent](https://github.com/lmcinnes/pynndescent), the Python implementation.
* [nndescent](https://github.com/TatsuyaShirakawa/nndescent), a C++ implementation.
* [NearestNeighborDescent.jl](https://github.com/dillondaudert/NearestNeighborDescent.jl),
a Julia implementation.
* [nn_descent](https://github.com/eskomski/nn_descent), a C implementation.
