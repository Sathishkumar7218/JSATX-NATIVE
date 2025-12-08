#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>      // <-- REQUIRED for intptr_t
#include "mlbase.h"


/* Helper: convert jstring -> C string (caller must free) */
static char* jstring_to_c(JNIEnv *env, jstring jstr) {
    if (!jstr) return NULL;
    const char *tmp = (*env)->GetStringUTFChars(env, jstr, NULL);
    char *copy = strdup(tmp);
    (*env)->ReleaseStringUTFChars(env, jstr, tmp);
    return copy;
}

/* ---------- DataFrame JNI ---------- */

/*
 * Class:     com_jsatx_jsatx_NativeLib
 * Method:    dfReadCsv
 * Signature: (Ljava/lang/String;C)J
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfReadCsv
  (JNIEnv *env, jclass cls, jstring jpath, jchar jdelim) {

    char *path = jstring_to_c(env, jpath);
    if (!path) return (jlong)0;

    DataFrame *df = df_read_csv(path, (char)jdelim);
    free(path);
    return (jlong)(intptr_t)df;
}

/*
 * Class:     com_jsatx_jsatx_NativeLib
 * Method:    dfHead
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfHead
  (JNIEnv *env, jclass cls, jlong dfPtr, jint n) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return;
    df_head(df, (size_t)n);
}

/*
 * dfDescribe
 */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfDescribe
  (JNIEnv *env, jclass cls, jlong dfPtr) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return;
    df_describe(df);
}

/*
 * dfGetColumnType
 * returns 0 = numeric, 1 = string, -1 = not found
 */
JNIEXPORT jint JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetColumnType
  (JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return -1;
    char *name = jstring_to_c(env, jname);
    ColumnType t = df_get_column_type(df, name);
    free(name);
    return (jint)t;
}

/*
 * dfGetNumericColumn -> returns pointer to NDArray on heap (jlong)
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetNumericColumn
  (JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return (jlong)0;
    char *name = jstring_to_c(env, jname);
    NDArray arr = df_get_numeric_column(df, name);
    free(name);

    NDArray *heap = malloc(sizeof(NDArray));
    if (!heap) return (jlong)0;
    *heap = arr;
    return (jlong)(intptr_t)heap;
}

/*
 * dfGetStringColumn -> returns Java String[] (copy of C strings)
 */
JNIEXPORT jobjectArray JNICALL Java_com_jsatx_jsatx_NativeLib_dfGetStringColumn
  (JNIEnv *env, jclass cls, jlong dfPtr, jstring jname) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return NULL;
    char *name = jstring_to_c(env, jname);

    size_t rows = 0;
    char **col = df_get_string_column(df, name, &rows);
    free(name);

    if (!col) return NULL;

    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray jarr = (*env)->NewObjectArray(env, (jsize)rows, stringClass, NULL);

    for (size_t i = 0; i < rows; i++) {
        const char *s = col[i] ? col[i] : "";
        jstring js = (*env)->NewStringUTF(env, s);
        (*env)->SetObjectArrayElement(env, jarr, (jsize)i, js);
    }

    return jarr;
}

/*
 * dfFree
 */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_dfFree
  (JNIEnv *env, jclass cls, jlong dfPtr) {

    DataFrame *df = (DataFrame*)(intptr_t)dfPtr;
    if (!df) return;
    df_free(df);
}

/* ---------- NDArray JNI (simple wrappers) ---------- */

/*
 * ndarrayNew
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayNew
  (JNIEnv *env, jclass cls, jint rows, jint cols) {

    NDArray arr = ndarray_new((size_t)rows, (size_t)cols);
    NDArray *heap = malloc(sizeof(NDArray));
    if (!heap) return (jlong)0;
    *heap = arr;
    return (jlong)(intptr_t)heap;
}

/*
 * ndarrayZeros
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayZeros
  (JNIEnv *env, jclass cls, jint rows, jint cols) {
    NDArray arr = ndarray_zeros((size_t)rows, (size_t)cols);
    NDArray *heap = malloc(sizeof(NDArray));
    if (!heap) return (jlong)0;
    *heap = arr;
    return (jlong)(intptr_t)heap;
}

/*
 * ndarrayOnes
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayOnes
  (JNIEnv *env, jclass cls, jint rows, jint cols) {
    NDArray arr = ndarray_ones((size_t)rows, (size_t)cols);
    NDArray *heap = malloc(sizeof(NDArray));
    if (!heap) return (jlong)0;
    *heap = arr;
    return (jlong)(intptr_t)heap;
}

/*
 * ndarrayFree
 */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayFree
  (JNIEnv *env, jclass cls, jlong ptr) {
    NDArray *arr = (NDArray*)(intptr_t)ptr;
    if (!arr) return;
    ndarray_free(arr);
    free(arr);
}

/*
 * ndarrayGet
 */
JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayGet
  (JNIEnv *env, jclass cls, jlong ptr, jint r, jint c) {
    NDArray *arr = (NDArray*)(intptr_t)ptr;
    if (!arr) return 0.0;
    return (jdouble)ndarray_get(arr, (size_t)r, (size_t)c);
}

/*
 * ndarraySet
 */
JNIEXPORT void JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySet
  (JNIEnv *env, jclass cls, jlong ptr, jint r, jint c, jdouble v) {
    NDArray *arr = (NDArray*)(intptr_t)ptr;
    if (!arr) return;
    ndarray_set(arr, (size_t)r, (size_t)c, (double)v);
}

/*
 * ndarraySum
 */
JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarraySum
  (JNIEnv *env, jclass cls, jlong ptr) {
    NDArray *arr = (NDArray*)(intptr_t)ptr;
    if (!arr) return 0.0;
    return (jdouble)ndarray_sum(arr);
}

/*
 * ndarrayMean
 */
JNIEXPORT jdouble JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayMean
  (JNIEnv *env, jclass cls, jlong ptr) {
    NDArray *arr = (NDArray*)(intptr_t)ptr;
    if (!arr) return 0.0;
    return (jdouble)ndarray_mean(arr);
}

/*
 * ndarrayDot
 */
JNIEXPORT jlong JNICALL Java_com_jsatx_jsatx_NativeLib_ndarrayDot
  (JNIEnv *env, jclass cls, jlong aPtr, jlong bPtr) {
    NDArray *a = (NDArray*)(intptr_t)aPtr;
    NDArray *b = (NDArray*)(intptr_t)bPtr;
    if (!a || !b) return (jlong)0;
    NDArray res = ndarray_dot(a, b);
    NDArray *heap = malloc(sizeof(NDArray));
    if (!heap) return (jlong)0;
    *heap = res;
    return (jlong)(intptr_t)heap;
}
