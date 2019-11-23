// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// rnn_brute_force
Rcpp::List rnn_brute_force(Rcpp::NumericMatrix data, int k, const std::string& metric, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose);
RcppExport SEXP _rnndescent_rnn_brute_force(SEXP dataSEXP, SEXP kSEXP, SEXP metricSEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(rnn_brute_force(data, k, metric, parallelize, block_size, grain_size, verbose));
    return rcpp_result_gen;
END_RCPP
}
// rnn_brute_force_query
Rcpp::List rnn_brute_force_query(Rcpp::NumericMatrix x, Rcpp::NumericMatrix y, int k, const std::string& metric, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose);
RcppExport SEXP _rnndescent_rnn_brute_force_query(SEXP xSEXP, SEXP ySEXP, SEXP kSEXP, SEXP metricSEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type x(xSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type y(ySEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(rnn_brute_force_query(x, y, k, metric, parallelize, block_size, grain_size, verbose));
    return rcpp_result_gen;
END_RCPP
}
// merge_nn
Rcpp::List merge_nn(Rcpp::IntegerMatrix nn_idx1, Rcpp::NumericMatrix nn_dist1, Rcpp::IntegerMatrix nn_idx2, Rcpp::NumericMatrix nn_dist2, bool is_query);
RcppExport SEXP _rnndescent_merge_nn(SEXP nn_idx1SEXP, SEXP nn_dist1SEXP, SEXP nn_idx2SEXP, SEXP nn_dist2SEXP, SEXP is_querySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::IntegerMatrix >::type nn_idx1(nn_idx1SEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type nn_dist1(nn_dist1SEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerMatrix >::type nn_idx2(nn_idx2SEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type nn_dist2(nn_dist2SEXP);
    Rcpp::traits::input_parameter< bool >::type is_query(is_querySEXP);
    rcpp_result_gen = Rcpp::wrap(merge_nn(nn_idx1, nn_dist1, nn_idx2, nn_dist2, is_query));
    return rcpp_result_gen;
END_RCPP
}
// nn_descent
Rcpp::List nn_descent(Rcpp::NumericMatrix data, Rcpp::IntegerMatrix nn_idx, Rcpp::NumericMatrix nn_dist, const std::string metric, const std::size_t max_candidates, const std::size_t n_iters, const double delta, bool low_memory, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose, const std::string& progress);
RcppExport SEXP _rnndescent_nn_descent(SEXP dataSEXP, SEXP nn_idxSEXP, SEXP nn_distSEXP, SEXP metricSEXP, SEXP max_candidatesSEXP, SEXP n_itersSEXP, SEXP deltaSEXP, SEXP low_memorySEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP, SEXP progressSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerMatrix >::type nn_idx(nn_idxSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type nn_dist(nn_distSEXP);
    Rcpp::traits::input_parameter< const std::string >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< const std::size_t >::type max_candidates(max_candidatesSEXP);
    Rcpp::traits::input_parameter< const std::size_t >::type n_iters(n_itersSEXP);
    Rcpp::traits::input_parameter< const double >::type delta(deltaSEXP);
    Rcpp::traits::input_parameter< bool >::type low_memory(low_memorySEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type progress(progressSEXP);
    rcpp_result_gen = Rcpp::wrap(nn_descent(data, nn_idx, nn_dist, metric, max_candidates, n_iters, delta, low_memory, parallelize, block_size, grain_size, verbose, progress));
    return rcpp_result_gen;
END_RCPP
}
// nn_descent_query
Rcpp::List nn_descent_query(Rcpp::NumericMatrix reference, Rcpp::IntegerMatrix reference_idx, Rcpp::NumericMatrix query, Rcpp::IntegerMatrix nn_idx, Rcpp::NumericMatrix nn_dist, const std::string metric, const std::size_t max_candidates, const std::size_t n_iters, const double delta, bool low_memory, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose, const std::string& progress);
RcppExport SEXP _rnndescent_nn_descent_query(SEXP referenceSEXP, SEXP reference_idxSEXP, SEXP querySEXP, SEXP nn_idxSEXP, SEXP nn_distSEXP, SEXP metricSEXP, SEXP max_candidatesSEXP, SEXP n_itersSEXP, SEXP deltaSEXP, SEXP low_memorySEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP, SEXP progressSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type reference(referenceSEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerMatrix >::type reference_idx(reference_idxSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type query(querySEXP);
    Rcpp::traits::input_parameter< Rcpp::IntegerMatrix >::type nn_idx(nn_idxSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type nn_dist(nn_distSEXP);
    Rcpp::traits::input_parameter< const std::string >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< const std::size_t >::type max_candidates(max_candidatesSEXP);
    Rcpp::traits::input_parameter< const std::size_t >::type n_iters(n_itersSEXP);
    Rcpp::traits::input_parameter< const double >::type delta(deltaSEXP);
    Rcpp::traits::input_parameter< bool >::type low_memory(low_memorySEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type progress(progressSEXP);
    rcpp_result_gen = Rcpp::wrap(nn_descent_query(reference, reference_idx, query, nn_idx, nn_dist, metric, max_candidates, n_iters, delta, low_memory, parallelize, block_size, grain_size, verbose, progress));
    return rcpp_result_gen;
END_RCPP
}
// random_knn_cpp
Rcpp::List random_knn_cpp(Rcpp::NumericMatrix data, int k, const std::string& metric, bool order_by_distance, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose);
RcppExport SEXP _rnndescent_random_knn_cpp(SEXP dataSEXP, SEXP kSEXP, SEXP metricSEXP, SEXP order_by_distanceSEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type data(dataSEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< bool >::type order_by_distance(order_by_distanceSEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(random_knn_cpp(data, k, metric, order_by_distance, parallelize, block_size, grain_size, verbose));
    return rcpp_result_gen;
END_RCPP
}
// random_knn_query_cpp
Rcpp::List random_knn_query_cpp(Rcpp::NumericMatrix reference, Rcpp::NumericMatrix query, int k, const std::string& metric, bool order_by_distance, bool parallelize, std::size_t block_size, std::size_t grain_size, bool verbose);
RcppExport SEXP _rnndescent_random_knn_query_cpp(SEXP referenceSEXP, SEXP querySEXP, SEXP kSEXP, SEXP metricSEXP, SEXP order_by_distanceSEXP, SEXP parallelizeSEXP, SEXP block_sizeSEXP, SEXP grain_sizeSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type reference(referenceSEXP);
    Rcpp::traits::input_parameter< Rcpp::NumericMatrix >::type query(querySEXP);
    Rcpp::traits::input_parameter< int >::type k(kSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type metric(metricSEXP);
    Rcpp::traits::input_parameter< bool >::type order_by_distance(order_by_distanceSEXP);
    Rcpp::traits::input_parameter< bool >::type parallelize(parallelizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type block_size(block_sizeSEXP);
    Rcpp::traits::input_parameter< std::size_t >::type grain_size(grain_sizeSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(random_knn_query_cpp(reference, query, k, metric, order_by_distance, parallelize, block_size, grain_size, verbose));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_rnndescent_rnn_brute_force", (DL_FUNC) &_rnndescent_rnn_brute_force, 7},
    {"_rnndescent_rnn_brute_force_query", (DL_FUNC) &_rnndescent_rnn_brute_force_query, 8},
    {"_rnndescent_merge_nn", (DL_FUNC) &_rnndescent_merge_nn, 5},
    {"_rnndescent_nn_descent", (DL_FUNC) &_rnndescent_nn_descent, 13},
    {"_rnndescent_nn_descent_query", (DL_FUNC) &_rnndescent_nn_descent_query, 15},
    {"_rnndescent_random_knn_cpp", (DL_FUNC) &_rnndescent_random_knn_cpp, 8},
    {"_rnndescent_random_knn_query_cpp", (DL_FUNC) &_rnndescent_random_knn_query_cpp, 9},
    {NULL, NULL, 0}
};

RcppExport void R_init_rnndescent(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
