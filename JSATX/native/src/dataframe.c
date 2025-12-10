#define _GNU_SOURCE
#include "mlbase.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Trim whitespace / newline */
static void trim(char *s) {
  if (!s)
    return;
  size_t len = strlen(s);
  while (len > 0 && isspace((unsigned char)s[len - 1]))
    s[--len] = '\0';
}

/* Check if token is numeric */
static int is_number(const char *s, double *out) {
  char *end;
  double val = strtod(s, &end);
  if (end == s)
    return 0;
  while (*end && isspace((unsigned char)*end))
    end++;
  if (*end)
    return 0;
  *out = val;
  return 1;
}

/* ============================================================
   CSV LOADING
   ============================================================ */
DataFrame *df_read_csv(const char *filename, char delimiter) {
  FILE *fp = fopen(filename, "r");
  if (!fp)
    return NULL;

  char *line = NULL;
  size_t len = 0;

  getline(&line, &len, fp);
  trim(line);

  size_t cols = 1;
  for (char *p = line; *p; p++)
    if (*p == delimiter)
      cols++;

  DataFrame *df = calloc(1, sizeof(DataFrame));
  df->cols = cols;
  df->columns = calloc(cols, sizeof(DataFrameColumn));

  /* Parse column names */
  size_t ci = 0;
  char *tok, *save;
  tok = strtok_r(line, &delimiter, &save);

  while (tok) {
    trim(tok);
    df->columns[ci].name = strdup(tok);
    df->columns[ci].type = COL_NUMERIC;
    tok = strtok_r(NULL, &delimiter, &save);
    ci++;
  }

  size_t rows = 0, cap = 128;
  char ***temp = calloc(cap, sizeof(char **));

  while (getline(&line, &len, fp) != -1) {
    trim(line);
    if (!*line)
      continue;

    if (rows >= cap) {
      cap *= 2;
      temp = realloc(temp, cap * sizeof(char **));
    }

    temp[rows] = calloc(cols, sizeof(char *));

    ci = 0;
    tok = strtok_r(line, &delimiter, &save);
    while (tok && ci < cols) {
      trim(tok);
      temp[rows][ci] = strdup(tok);
      tok = strtok_r(NULL, &delimiter, &save);
      ci++;
    }
    rows++;
  }

  df->rows = rows;

  /* Detect column types */
  for (size_t c = 0; c < cols; c++) {
    double tmp;
    int isnum = 1;
    for (size_t r = 0; r < rows; r++) {
      if (!is_number(temp[r][c], &tmp)) {
        isnum = 0;
        break;
      }
    }
    df->columns[c].type = isnum ? COL_NUMERIC : COL_STRING;
  }

  /* Allocate */
  for (size_t c = 0; c < cols; c++) {
    if (df->columns[c].type == COL_NUMERIC)
      df->columns[c].numValues = calloc(rows, sizeof(double));
    else
      df->columns[c].strValues = calloc(rows, sizeof(char *));
  }

  /* Fill */
  for (size_t c = 0; c < cols; c++) {
    for (size_t r = 0; r < rows; r++) {
      if (df->columns[c].type == COL_NUMERIC) {
        double v;
        is_number(temp[r][c], &v);
        df->columns[c].numValues[r] = v;
      } else {
        df->columns[c].strValues[r] = strdup(temp[r][c]);
      }
    }
  }

  /* Free */
  for (size_t r = 0; r < rows; r++) {
    for (size_t c = 0; c < cols; c++)
      free(temp[r][c]);
    free(temp[r]);
  }

  free(temp);
  free(line);
  fclose(fp);

  return df;
}

/* ============================================================
   Pretty Printing
   ============================================================ */
void df_head(const DataFrame *df, size_t n) {
  if (!df)
    return;

  size_t limit = n > df->rows ? df->rows : n;
  size_t *width = calloc(df->cols, sizeof(size_t));

  for (size_t c = 0; c < df->cols; c++) {
    width[c] = strlen(df->columns[c].name);
    for (size_t r = 0; r < limit; r++) {
      char buf[64];
      if (df->columns[c].type == COL_NUMERIC)
        snprintf(buf, sizeof(buf), "%.3f", df->columns[c].numValues[r]);
      else
        snprintf(buf, sizeof(buf), "%s", df->columns[c].strValues[r]);
      if (strlen(buf) > width[c])
        width[c] = strlen(buf);
    }
  }

  for (size_t c = 0; c < df->cols; c++)
    printf("%-*s  ", (int)width[c], df->columns[c].name);
  printf("\n");

  for (size_t r = 0; r < limit; r++) {
    for (size_t c = 0; c < df->cols; c++) {
      if (df->columns[c].type == COL_NUMERIC)
        printf("%-*.*f  ", (int)width[c], 3, df->columns[c].numValues[r]);
      else
        printf("%-*s  ", (int)width[c], df->columns[c].strValues[r]);
    }
    printf("\n");
  }

  free(width);
}

/* ============================================================
   describe()
   ============================================================ */
void df_describe(const DataFrame *df) {
  printf("\nColumn        Mean\n-----------------------\n");

  for (size_t c = 0; c < df->cols; c++) {
    if (df->columns[c].type == COL_NUMERIC) {
      double sum = 0;
      for (size_t r = 0; r < df->rows; r++)
        sum += df->columns[c].numValues[r];

      printf("%-12s %.3f\n", df->columns[c].name, sum / df->rows);
    }
  }
}

/* ============================================================
   Column Access (THIS FUNCTION WAS MISSING!)
   ============================================================ */
ColumnType df_get_column_type(const DataFrame *df, const char *name) {
  if (!df || !name)
    return -1;

  for (size_t c = 0; c < df->cols; c++) {
    if (strcmp(df->columns[c].name, name) == 0)
      return df->columns[c].type;
  }

  return -1;
}

NDArray df_get_numeric_column(const DataFrame *df, const char *name) {
  for (size_t c = 0; c < df->cols; c++) {
    if (strcmp(df->columns[c].name, name) == 0 &&
        df->columns[c].type == COL_NUMERIC) {

      NDArray arr = ndarray_new(df->rows, 1);
      for (size_t r = 0; r < df->rows; r++)
        arr.data[r] = df->columns[c].numValues[r];
      return arr;
    }
  }
  return ndarray_new(0, 0);
}

char **df_get_string_column(const DataFrame *df, const char *name,
                            size_t *rowsOut) {
  for (size_t c = 0; c < df->cols; c++) {
    if (strcmp(df->columns[c].name, name) == 0 &&
        df->columns[c].type == COL_STRING) {

      *rowsOut = df->rows;
      return df->columns[c].strValues;
    }
  }
  *rowsOut = 0;
  return NULL;
}

/* ============================================================
   Free memory
   ============================================================ */
void df_free(DataFrame *df) {
  if (!df)
    return;

  for (size_t c = 0; c < df->cols; c++) {
    free(df->columns[c].name);

    if (df->columns[c].type == COL_NUMERIC) {
      free(df->columns[c].numValues);
    } else {
      for (size_t r = 0; r < df->rows; r++)
        free(df->columns[c].strValues[r]);
      free(df->columns[c].strValues);
    }
  }

  free(df->columns);
  free(df);
}
