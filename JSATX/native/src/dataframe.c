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

  // First pass: count rows and columns
  char line[1024];
  size_t rows = 0;
  size_t cols = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (rows == 0) {
      // Parse header
      char *token = strtok(line, ",");
      while (token) {
        cols++;
        token = strtok(NULL, ",");
      }
    }
    rows++;
  }
  rows--; // subtract header

  rewind(fp);

  DataFrame *df = df_new_empty(rows, cols);

  // Read header
  if (fgets(line, sizeof(line), fp)) {
    char *token = strtok(line, ",");
    for (size_t c = 0; c < cols && token; c++) {
      // Remove newline
      token[strcspn(token, "\n")] = 0;
      df->col_names[c] = strdup(token);
      token = strtok(NULL, ",");
    }
  }

  // Read data
  for (size_t r = 0; r < rows; r++) {
    if (!fgets(line, sizeof(line), fp)) break;
    
    char *token = strtok(line, ",");
    for (size_t c = 0; c < cols && token; c++) {
      // Remove newline
      token[strcspn(token, "\n")] = 0;
      
      // Try to parse as number
      char *endptr;
      double val = strtod(token, &endptr);
      if (*endptr == '\0') {
        // It's a number
        df->col_types[c] = COL_TYPE_NUMERIC;
        ndarray_set(&df->numeric, r, c, val);
      } else {
        // It's a string
        df->col_types[c] = COL_TYPE_STRING;
        if (!df->strings[c]) {
          df->strings[c] = calloc(rows, sizeof(char *));
        }
        df->strings[c][r] = strdup(token);
      }
      
      token = strtok(NULL, ",");
    }
  }

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
