#include "mlbase.h"
#include <stdlib.h>

NDArray ndarray_new(size_t rows, size_t cols) {
    NDArray arr;
    arr.rows = rows;
    arr.cols = cols;
    arr.data = calloc(rows * cols, sizeof(double));
    return arr;
}

NDArray ndarray_zeros(size_t rows, size_t cols) {
    return ndarray_new(rows, cols);
}

NDArray ndarray_ones(size_t rows, size_t cols) {
    NDArray arr = ndarray_new(rows, cols);
    size_t total = rows * cols;
    for (size_t i = 0; i < total; i++)
        arr.data[i] = 1.0;
    return arr;
}

void ndarray_free(NDArray *arr) {
    if (!arr || !arr->data) return;
    free(arr->data);
    arr->data = NULL;
}

double ndarray_get(const NDArray *arr, size_t r, size_t c) {
    return arr->data[r * arr->cols + c];
}

void ndarray_set(NDArray *arr, size_t r, size_t c, double v) {
    arr->data[r * arr->cols + c] = v;
}

double ndarray_sum(const NDArray *arr) {
    double s = 0;
    for (size_t i = 0; i < arr->rows * arr->cols; i++)
        s += arr->data[i];
    return s;
}

double ndarray_mean(const NDArray *arr) {
    size_t n = arr->rows * arr->cols;
    return n == 0 ? 0 : ndarray_sum(arr) / (double)n;
}

NDArray ndarray_dot(const NDArray *a, const NDArray *b) {
    if (!a || !b || a->cols != b->rows)
        return ndarray_new(0, 0);

    NDArray out = ndarray_new(a->rows, b->cols);

    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0;
            for (size_t k = 0; k < a->cols; k++)
                sum += ndarray_get(a, i, k) * ndarray_get(b, k, j);
            ndarray_set(&out, i, j, sum);
        }
    }
    return out;
}
