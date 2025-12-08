#ifndef MLBASE_H
#define MLBASE_H

#include <stddef.h>

/* ============================================
   NDArray (Matrix) — NumPy-like
   ============================================ */

typedef struct {
    double *data;
    size_t rows;
    size_t cols;
} NDArray;

NDArray ndarray_new(size_t rows, size_t cols);
NDArray ndarray_zeros(size_t rows, size_t cols);
NDArray ndarray_ones(size_t rows, size_t cols);
void ndarray_free(NDArray *arr);

double ndarray_get(const NDArray *arr, size_t r, size_t c);
void ndarray_set(NDArray *arr, size_t r, size_t c, double v);

double ndarray_sum(const NDArray *arr);
double ndarray_mean(const NDArray *arr);
NDArray ndarray_dot(const NDArray *a, const NDArray *b);

/* ============================================
   DataFrame — Mixed String + Numeric Columns
   ============================================ */

typedef enum {
    COL_NUMERIC = 0,
    COL_STRING  = 1
} ColumnType;

typedef struct {
    char *name;
    ColumnType type;

    double *numValues;     // numeric column
    char   **strValues;    // string column
} DataFrameColumn;

typedef struct {
    size_t rows;
    size_t cols;
    DataFrameColumn *columns;
} DataFrame;

/* DataFrame API */
DataFrame* df_read_csv(const char *filename, char delimiter);
void df_head(const DataFrame *df, size_t n);
void df_describe(const DataFrame *df);

ColumnType df_get_column_type(const DataFrame *df, const char *name);
NDArray df_get_numeric_column(const DataFrame *df, const char *name);
char** df_get_string_column(const DataFrame *df, const char *name, size_t *out_rows);

void df_free(DataFrame *df);

#endif
