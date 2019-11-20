#ifndef TDOANN_PROGRESS_H
#define TDOANN_PROGRESS_H

#define TDOANN_BREAKIFINTERRUPTED()                                            \
  if (progress.check_interrupt()) {                                            \
    break;                                                                     \
  }

#define TDOANN_ITERFINISHED()                                                  \
  TDOANN_BREAKIFINTERRUPTED()                                                  \
  progress.iter_finished();

#define TDOANN_BLOCKFINISHED()                                                 \
  TDOANN_BREAKIFINTERRUPTED()                                                  \
  progress.block_finished();

#define TDOANN_CHECKCONVERGENCE()                                              \
  if (tdoann::is_converged(c, tol)) {                                          \
    progress.converged(c, tol);                                                \
    break;                                                                     \
  }

namespace tdoann {
// Defines the methods required, but does nothing. Safe to use from
// multi-threaded code if a dummy no-op version is needed.
struct NullProgress {
  NullProgress() {}
  NullProgress(std::size_t n_iters, bool verbose) {}
  void block_finished() {}
  void iter_finished() {}
  void stopping_early() {}
  bool check_interrupt() { return false; }
};
} // namespace tdoann

#endif // TDOANN_PROGRESS_H
