#include "mlbase.h"
#include <jni.h>
#include <stdint.h> // for intptr_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   Utility: Convert jstring → malloc'd C string (caller frees)
   ============================================================ */
static char *jstring_to_c(JNIEnv *env, jstring jstr) {
  if (!jstr)
    return NULL;
  const char *tmp = (*env)->GetStringUTFChars(env, jstr, NULL);
  char *copy = strdup(tmp);
  (*env)->ReleaseStringUTFChars(env, jstr, tmp);
  return copy;
}

/* ============================================================
   DataFrame JNI Wrappers
   ============================================================ */

/* dfReadCsv(path, delimiter) → jlong dfPtr */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfReadCsv(JNIEnv *env,
                                                                 jclass cls,
                                                                 jstring jpath,
                                                                 jchar jdelim) {

  char *path = jstring_to_c(env, jpath);
  if (!path)
    return 0;

  DataFrame *df = df_read_csv(path, (char)jdelim);
  free(path);

  return (jlong)(intptr_t)df;
}

/* dfHead(dfPtr, n) */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfHead(JNIEnv *env,
                                                             jclass cls,
                                                             jlong dfPtr,
                                                             jint n) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (df)
    df_head(df, (size_t)n);
}

/* dfDescribe(dfPtr) */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfDescribe(JNIEnv *env,
                                                                 jclass cls,
                                                                 jlong dfPtr) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (df)
    df_describe(df);
}

/* dfGetColumnType(dfPtr, "Age") → 0/1/-1 */
JNIEXPORT jint JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetColumnType(
    JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (!df)
    return -1;

  char *name = jstring_to_c(env, jname);
  ColumnType t = df_get_column_type(df, name);
  free(name);

  return (jint)t;
}

/* Numeric column → return NDArray pointer (on heap) */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetNumericColumn(
    JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (!df)
    return 0;

  char *name = jstring_to_c(env, jname);
  NDArray arr = df_get_numeric_column(df, name);
  free(name);

  NDArray *heap = malloc(sizeof(NDArray));
  if (!heap)
    return 0;

  *heap = arr;
  return (jlong)(intptr_t)heap;
}

/* String column → return Java String[] */
JNIEXPORT jobjectArray JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetStringColumn(
    JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (!df)
    return NULL;

  char *name = jstring_to_c(env, jname);
  size_t rows = 0;
  char **col = df_get_string_column(df, name, &rows);
  free(name);

  if (!col)
    return NULL;

  jclass stringClass = (*env)->FindClass(env, "java/lang/String");
  jobjectArray arr =
      (*env)->NewObjectArray(env, (jsize)rows, stringClass, NULL);

  for (size_t i = 0; i < rows; i++) {
    jstring js = (*env)->NewStringUTF(env, col[i] ? col[i] : "");
    (*env)->SetObjectArrayElement(env, arr, (jsize)i, js);
  }

  return arr;
}

/* dfFree(dfPtr) */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfFree(JNIEnv *env,
                                                             jclass cls,
                                                             jlong dfPtr) {

  DataFrame *df = (DataFrame *)(intptr_t)dfPtr;
  if (df)
    df_free(df);
}

/* ============================================================
   NDArray JNI Wrappers
   ============================================================ */

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayNew(JNIEnv *env,
                                                                  jclass cls,
                                                                  jint rows,
                                                                  jint cols) {

  NDArray arr = ndarray_new(rows, cols);
  NDArray *heap = malloc(sizeof(NDArray));
  *heap = arr;
  return (jlong)(intptr_t)heap;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayZeros(JNIEnv *env,
                                                                    jclass cls,
                                                                    jint rows,
                                                                    jint cols) {

  NDArray arr = ndarray_zeros(rows, cols);
  NDArray *heap = malloc(sizeof(NDArray));
  *heap = arr;
  return (jlong)(intptr_t)heap;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayOnes(JNIEnv *env,
                                                                   jclass cls,
                                                                   jint rows,
                                                                   jint cols) {

  NDArray arr = ndarray_ones(rows, cols);
  NDArray *heap = malloc(sizeof(NDArray));
  *heap = arr;
  return (jlong)(intptr_t)heap;
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayFree(JNIEnv *env,
                                                                  jclass cls,
                                                                  jlong ptr) {

  NDArray *arr = (NDArray *)(intptr_t)ptr;
  if (arr) {
    ndarray_free(arr);
    free(arr);
  }
}

JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayGet(
    JNIEnv *env, jclass cls, jlong ptr, jint r, jint c) {

  NDArray *arr = (NDArray *)(intptr_t)ptr;
  if (!arr)
    return 0.0;

  return ndarray_get(arr, r, c);
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySet(
    JNIEnv *env, jclass cls, jlong ptr, jint r, jint c, jdouble v) {

  NDArray *arr = (NDArray *)(intptr_t)ptr;
  if (arr)
    ndarray_set(arr, r, c, v);
}

JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySum(JNIEnv *env,
                                                                    jclass cls,
                                                                    jlong ptr) {

  NDArray *arr = (NDArray *)(intptr_t)ptr;
  if (!arr)
    return 0.0;

  return ndarray_sum(arr);
}

JNIEXPORT jdouble JNICALL
Java_com_jsatx_jsatx_NativeLib_ndarrayMean(JNIEnv *env, jclass cls, jlong ptr) {

  NDArray *arr = (NDArray *)(intptr_t)ptr;
  if (!arr)
    return 0.0;

  return ndarray_mean(arr);
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayDot(JNIEnv *env,
                                                                  jclass cls,
                                                                  jlong aPtr,
                                                                  jlong bPtr) {

  NDArray *A = (NDArray *)(intptr_t)aPtr;
  NDArray *B = (NDArray *)(intptr_t)bPtr;

  if (!A || !B)
    return 0;

  NDArray res = ndarray_dot(A, B);
  NDArray *heap = malloc(sizeof(NDArray));
  *heap = res;

  return (jlong)(intptr_t)heap;
}
