#include "mlbase.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Simple CSV parser
DataFrame *df_read_csv(const char *path, char delimiter) {
  FILE *fp = fopen(path, "r");
  if (!fp) return NULL;

  // First pass: count rows and columns using getline for robust long-line handling
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  size_t rows = 0;
  size_t cols = 0;

  if ((read = getline(&line, &len, fp)) != -1) {
    // header line
    for (char *p = line; *p; p++) if (*p == delimiter) cols++;
    cols++; // separators + 1
    rows = 1; // header counted
  }

  while ((read = getline(&line, &len, fp)) != -1) {
    rows++;
  }
  rows--; // subtract header

  rewind(fp);

  DataFrame *df = df_new_empty(rows, cols);

  // Read header
  if ((read = getline(&line, &len, fp)) != -1) {
    // tokenize header by delimiter
    char *start = line;
    for (size_t c = 0; c < cols; c++) {
      char *p = start;
      while (*p && *p != delimiter && *p != '\n' && *p != '\r') p++;
      char saved = *p;
      *p = '\0';
      df->col_names[c] = strdup(start);
      *p = saved;
      if (saved == '\0' || saved == '\n' || saved == '\r') break;
      start = p + 1;
    }
  }

  // Read data lines and parse without strtok (faster and safer)
  size_t r = 0;
  while ((read = getline(&line, &len, fp)) != -1 && r < rows) {
    char *start = line;
    for (size_t c = 0; c < cols; c++) {
      char *p = start;
      while (*p && *p != delimiter && *p != '\n' && *p != '\r') p++;
      char saved = *p;
      *p = '\0';

      // Try to parse as number
      char *endptr;
      double val = strtod(start, &endptr);
      if (*endptr == '\0') {
        df->col_types[c] = COL_TYPE_NUMERIC;
        ndarray_set(&df->numeric, r, c, val);
      } else {
        df->col_types[c] = COL_TYPE_STRING;
        if (!df->strings[c]) {
          df->strings[c] = calloc(rows, sizeof(char *));
        }
        df->strings[c][r] = strdup(start);
      }

      *p = saved;
      if (saved == '\0' || saved == '\n' || saved == '\r') break;
      start = p + 1;
    }
    r++;
  }

  free(line);
  fclose(fp);
  return df;
}

DataFrame *df_new_empty(size_t rows, size_t cols) {
  DataFrame *df = calloc(1, sizeof(DataFrame));
  df->rows = rows;
  df->cols = cols;

  df->col_names = calloc(cols, sizeof(char *));
  df->col_types = calloc(cols, sizeof(ColumnType));
  df->numeric = ndarray_zeros(rows, cols);
  df->strings = calloc(cols, sizeof(char **));

  return df;
}

void df_free(DataFrame *df) {
  if (!df)
    return;

  for (size_t c = 0; c < df->cols; c++)
    free(df->col_names[c]);
  free(df->col_names);

  free(df->col_types);

  ndarray_free(&df->numeric);

  for (size_t c = 0; c < df->cols; c++) {
    if (df->strings[c]) {
      for (size_t r = 0; r < df->rows; r++)
        free(df->strings[c][r]);
      free(df->strings[c]);
    }
  }
  free(df->strings);

  free(df);
}

void df_head(const DataFrame *df, size_t n) {
  if (!df)
    return;
  if (n > df->rows)
    n = df->rows;

  // Print headers
  for (size_t c = 0; c < df->cols; c++) {
    if (df->col_names[c]) {
      printf("%s\t", df->col_names[c]);
    } else {
      printf("Col%zu\t", c);
    }
  }
  printf("\n");

  // Print data
  for (size_t r = 0; r < n; r++) {
    for (size_t c = 0; c < df->cols; c++) {
      if (df->col_types[c] == COL_TYPE_NUMERIC) {
        double v = ndarray_get(&df->numeric, r, c);
        printf("%.0f\t", v);  // Print as integer if whole number
      } else if (df->col_types[c] == COL_TYPE_STRING) {
        if (df->strings[c] && df->strings[c][r]) {
          printf("%s\t", df->strings[c][r]);
        } else {
          printf("NULL\t");
        }
      } else {
        printf("?\t");
      }
    }
    printf("\n");
  }
}

void df_describe(const DataFrame *df) {
  if (!df)
    return;
  printf("Rows=%zu Cols=%zu\n", df->rows, df->cols);
}

ColumnType df_get_column_type(const DataFrame *df, const char *col_name) {
  if (!df || !col_name) return COL_TYPE_UNKNOWN;
  
  for (size_t c = 0; c < df->cols; c++) {
    if (df->col_names[c] && strcmp(df->col_names[c], col_name) == 0) {
      return df->col_types[c];
    }
  }
  return COL_TYPE_UNKNOWN;
}

static size_t df_find_column(const DataFrame *df, const char *col_name) {
  for (size_t c = 0; c < df->cols; c++) {
    if (df->col_names[c] && strcmp(df->col_names[c], col_name) == 0) {
      return c;
    }
  }
  return (size_t)-1;
}

NDArray *df_get_numeric_column(const DataFrame *df, const char *col_name) {
  size_t c = df_find_column(df, col_name);
  if (c == (size_t)-1 || df->col_types[c] != COL_TYPE_NUMERIC) return NULL;
  
  NDArray *col = malloc(sizeof(NDArray));
  *col = ndarray_new(df->rows, 1);
  for (size_t r = 0; r < df->rows; r++) {
    double v = ndarray_get(&df->numeric, r, c);
    ndarray_set(col, r, 0, v);
  }
  return col;
}

/* Fast aggregations that operate directly on DataFrame storage without copying */

double df_sum_column(const DataFrame *df, const char *col_name) {
  size_t c = df_find_column(df, col_name);
  if (c == (size_t)-1 || df->col_types[c] != COL_TYPE_NUMERIC) return 0.0;
  size_t rows = df->rows;
  size_t cols = df->numeric.cols;
  double *data = df->numeric.data;
  double sum = 0.0;

  /* Parallelize aggregation using OpenMP if available */
  #pragma omp parallel for reduction(+:sum)
  for (size_t r = 0; r < rows; r++) {
    sum += data[r * cols + c];
  }
  return sum;
}

double df_mean_column(const DataFrame *df, const char *col_name) {
  size_t rows = df->rows;
  if (rows == 0) return 0.0;
  double s = df_sum_column(df, col_name);
  return s / (double)rows;
}

char **df_get_string_column(const DataFrame *df, const char *col_name) {
  size_t c = df_find_column(df, col_name);
  if (c == (size_t)-1 || df->col_types[c] != COL_TYPE_STRING) return NULL;
  return df->strings[c];
}

size_t df_get_string_column_size(const DataFrame *df, const char *col_name) {
  size_t c = df_find_column(df, col_name);
  if (c == (size_t)-1) return 0;
  return df->rows;
}

const char *df_string_at(const char **str_array, size_t index) {
  return str_array[index];
}
