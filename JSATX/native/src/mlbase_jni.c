#include "mlbase.h"
#include <jni.h>
#include <stdint.h>
#include <stdlib.h>

static inline NDArray *castArray(jlong ptr) { return (NDArray *)(intptr_t)ptr; }

static inline DataFrame *castDF(jlong ptr) {
  return (DataFrame *)(intptr_t)ptr;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayNew(JNIEnv *env,
                                                                  jclass cls,
                                                                  jint rows,
                                                                  jint cols) {
  NDArray *a = malloc(sizeof(NDArray));
  *a = ndarray_new(rows, cols);
  return (jlong)(intptr_t)a;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayZeros(JNIEnv *env,
                                                                   jclass cls,
                                                                   jint rows,
                                                                   jint cols) {
  NDArray *a = malloc(sizeof(NDArray));
  *a = ndarray_zeros(rows, cols);
  return (jlong)(intptr_t)a;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayOnes(JNIEnv *env,
                                                                  jclass cls,
                                                                  jint rows,
                                                                  jint cols) {
  NDArray *a = malloc(sizeof(NDArray));
  *a = ndarray_ones(rows, cols);
  return (jlong)(intptr_t)a;
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayFree(JNIEnv *env,
                                                                  jclass cls,
                                                                  jlong ptr) {
  NDArray *a = castArray(ptr);
  ndarray_free(a);
  free(a);
}

JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayGet(
    JNIEnv *env, jclass cls, jlong ptr, jint r, jint c) {
  return ndarray_get(castArray(ptr), r, c);
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySet(
    JNIEnv *env, jclass cls, jlong ptr, jint r, jint c, jdouble v) {
  ndarray_set(castArray(ptr), r, c, v);
}

JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySum(JNIEnv *env,
                                                                    jclass cls,
                                                                    jlong ptr) {
  return ndarray_sum(castArray(ptr));
}

JNIEXPORT jdouble JNICALL
Java_com_jsatx_jsatx_NativeLib_ndarrayMean(JNIEnv *env, jclass cls, jlong ptr) {
  return ndarray_mean(castArray(ptr));
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayDot(JNIEnv *env,
                                                                  jclass cls,
                                                                  jlong A,
                                                                  jlong B) {
  NDArray *a = castArray(A);
  NDArray *b = castArray(B);
  NDArray *out = malloc(sizeof(NDArray));
  *out = ndarray_dot(a, b);
  return (jlong)(intptr_t)out;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfNewEmpty(JNIEnv *env,
                                                                  jclass cls,
                                                                  jint rows,
                                                                  jint cols) {
  return (jlong)(intptr_t)df_new_empty(rows, cols);
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfFree(JNIEnv *env,
                                                             jclass cls,
                                                             jlong ptr) {
  df_free(castDF(ptr));
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfHead(JNIEnv *env,
                                                             jclass cls,
                                                             jlong ptr,
                                                             jint n) {
  df_head(castDF(ptr), n);
}

JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfDescribe(JNIEnv *env,
                                                                 jclass cls,
                                                                 jlong ptr) {
  df_describe(castDF(ptr));
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfReadCsv(JNIEnv *env,
                                                                 jclass cls,
                                                                 jstring path,
                                                                 jchar delimiter) {
  const char *cpath = (*env)->GetStringUTFChars(env, path, NULL);
  DataFrame *df = df_read_csv(cpath, (char)delimiter);
  (*env)->ReleaseStringUTFChars(env, path, cpath);
  return (jlong)(intptr_t)df;
}

JNIEXPORT jint JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetColumnType(JNIEnv *env, jclass cls, jlong ptr, jstring colName) {
  const char *cname = (*env)->GetStringUTFChars(env, colName, NULL);
  ColumnType type = df_get_column_type(castDF(ptr), cname);
  (*env)->ReleaseStringUTFChars(env, colName, cname);
  if (type == COL_TYPE_NUMERIC) return 0;
  if (type == COL_TYPE_STRING) return 1;
  return -1;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetNumericColumn(JNIEnv *env, jclass cls, jlong ptr, jstring colName) {
  const char *cname = (*env)->GetStringUTFChars(env, colName, NULL);
  NDArray *arr = df_get_numeric_column(castDF(ptr), cname);
  (*env)->ReleaseStringUTFChars(env, colName, cname);
  return (jlong)(intptr_t)arr;
}

JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetStringColumn(JNIEnv *env, jclass cls, jlong ptr, jstring colName) {
  const char *cname = (*env)->GetStringUTFChars(env, colName, NULL);
  char **strs = df_get_string_column(castDF(ptr), cname);
  (*env)->ReleaseStringUTFChars(env, colName, cname);
  return (jlong)(intptr_t)strs;
}

JNIEXPORT jint JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetStringColumnSize(JNIEnv *env, jclass cls, jlong ptr, jstring colName) {
  const char *cname = (*env)->GetStringUTFChars(env, colName, NULL);
  size_t size = df_get_string_column_size(castDF(ptr), cname);
  (*env)->ReleaseStringUTFChars(env, colName, cname);
  return (jint)size;
}

JNIEXPORT jstring JNICALL Java_com_jsatx_jsatx_NativeLib_dfStringAt(JNIEnv *env, jclass cls, jlong strArrayPtr, jint index) {
  const char **str_array = (const char **)(intptr_t)strArrayPtr;
  const char *str = df_string_at(str_array, (size_t)index);
  return (*env)->NewStringUTF(env, str);
}
