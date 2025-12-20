#ifndef MLBSE_H
#define MLBSE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  size_t rows;
  size_t cols;
  double *data;
} NDArray;

NDArray ndarray_new(size_t rows, size_t cols);
NDArray ndarray_zeros(size_t rows, size_t cols);
NDArray ndarray_ones(size_t rows, size_t cols);
void ndarray_free(NDArray *arr);
double ndarray_get(const NDArray *arr, size_t r, size_t c);
void ndarray_set(NDArray *arr, size_t r, size_t c, double v);
double ndarray_sum(const NDArray *arr);
double ndarray_mean(const NDArray *arr);
NDArray ndarray_dot(const NDArray *A, const NDArray *B);

typedef enum {
  COL_TYPE_UNKNOWN = 0,
  COL_TYPE_NUMERIC = 1,
  COL_TYPE_STRING = 2
} ColumnType;

typedef struct {
  size_t rows;
  size_t cols;
  char **col_names;
  ColumnType *col_types;
  NDArray numeric;
  char ***strings;
} DataFrame;

DataFrame *df_new_empty(size_t rows, size_t cols);
void df_free(DataFrame *df);
void df_head(const DataFrame *df, size_t n);
void df_describe(const DataFrame *df);
DataFrame *df_read_csv(const char *path, char delimiter);
ColumnType df_get_column_type(const DataFrame *df, const char *col_name);
NDArray *df_get_numeric_column(const DataFrame *df, const char *col_name);
char **df_get_string_column(const DataFrame *df, const char *col_name);
size_t df_get_string_column_size(const DataFrame *df, const char *col_name);
const char *df_string_at(const char **str_array, size_t index);

#endif
