#include "mlbase.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

NDArray ndarray_new(size_t rows, size_t cols) {
  NDArray a;
  a.rows = rows;
  a.cols = cols;
  a.data = calloc(rows * cols, sizeof(double));
  return a;
}

NDArray ndarray_zeros(size_t rows, size_t cols) {
  return ndarray_new(rows, cols);
}

NDArray ndarray_ones(size_t rows, size_t cols) {
  NDArray a = ndarray_new(rows, cols);
  size_t n = rows * cols;
  for (size_t i = 0; i < n; i++) {
    a.data[i] = 1.0;
  }
  return a;
}

void ndarray_free(NDArray *arr) {
  if (arr && arr->data) {
    free(arr->data);
    arr->data = NULL;
  }
}

static inline size_t idx(const NDArray *a, size_t r, size_t c) {
  return r * a->cols + c;
}

double ndarray_get(const NDArray *arr, size_t r, size_t c) {
  if (!arr || !arr->data)
    return 0;
  if (r >= arr->rows || c >= arr->cols)
    return 0;
  return arr->data[idx(arr, r, c)];
}

void ndarray_set(NDArray *arr, size_t r, size_t c, double v) {
  if (!arr || !arr->data)
    return;
  if (r >= arr->rows || c >= arr->cols)
    return;
  arr->data[idx(arr, r, c)] = v;
}

double ndarray_sum(const NDArray *arr) {
  if (!arr || !arr->data)
    return 0;
  double s = 0;
  size_t n = arr->rows * arr->cols;
  /* Parallel reduction over contiguous storage */
  #pragma omp parallel for reduction(+:s)
  for (size_t i = 0; i < n; i++)
    s += arr->data[i];
  return s;
}

double ndarray_mean(const NDArray *arr) {
  if (!arr || !arr->data)
    return 0;
  size_t n = arr->rows * arr->cols;
  if (n == 0)
    return 0;
  return ndarray_sum(arr) / (double)n;
}

#ifdef USE_BLAS
#include <cblas.h>
#endif

NDArray ndarray_dot(const NDArray *A, const NDArray *B) {
  NDArray out = ndarray_new(0, 0);
  if (!A || !B || !A->data || !B->data)
    return out;
  if (A->cols != B->rows)
    return out;

  out = ndarray_new(A->rows, B->cols);
  size_t AR = A->rows, AC = A->cols, BC = B->cols;
  double *Ad = A->data;
  double *Bd = B->data;
  double *Od = out.data;

#ifdef USE_BLAS
  /* Use BLAS dgemm for best performance (row-major) */
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              (int)AR, (int)BC, (int)AC,
              1.0, Ad, (int)AC, Bd, (int)BC, 0.0, Od, (int)BC);
  return out;
#else
  /* Parallelize over output rows */
  #pragma omp parallel for
  for (size_t i = 0; i < AR; i++) {
    for (size_t j = 0; j < BC; j++) {
      double acc = 0.0;
      size_t baseA = i * AC;
      for (size_t k = 0; k < AC; k++) {
        acc += Ad[baseA + k] * Bd[k * BC + j];
      }
      Od[i * BC + j] = acc;
    }
  }
  return out;
#endif
}
